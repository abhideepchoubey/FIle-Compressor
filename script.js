class Node {
    constructor(char, freq, left=null, right=null) {
        this.char = char;
        this.freq = freq;
        this.left = left;
        this.right = right;
    }
}

function buildHuffmanTree(freqMap) {
    const pq = [];
    for (const [char, freq] of Object.entries(freqMap)) {
        pq.push(new Node(char, freq));
    }
    pq.sort((a,b)=>a.freq-b.freq);

    while (pq.length > 1) {
        const left = pq.shift();
        const right = pq.shift();
        const parent = new Node(null, left.freq + right.freq, left, right);
        pq.push(parent);
        pq.sort((a,b)=>a.freq-b.freq);
    }
    return pq[0];
}

function generateCodes(root, prefix="", codes={}) {
    if (!root) return;
    if (!root.left && !root.right) {
        codes[root.char] = prefix;
    }
    generateCodes(root.left, prefix + "0", codes);
    generateCodes(root.right, prefix + "1", codes);
    return codes;
}

function compressText(text) {
    const freqMap = {};
    for (let ch of text) freqMap[ch] = (freqMap[ch] || 0) + 1;
    const tree = buildHuffmanTree(freqMap);
    const codes = generateCodes(tree);
    let binary = "";
    for (let ch of text) binary += codes[ch];
    return {codes, binary};
}

function downloadCompressed(binary, filename="compressed.bin") {
    const bytes = [];
    for (let i = 0; i < binary.length; i += 8) {
        const byte = binary.substring(i, i + 8);
        bytes.push(parseInt(byte.padEnd(8, "0"), 2));
    }
    const blob = new Blob([new Uint8Array(bytes)], {type: "application/octet-stream"});
    const link = document.createElement("a");
    link.href = URL.createObjectURL(blob);
    link.download = filename;
    link.click();
}

function decompressBinary(binary, codes) {
    const reversed = {};
    for (const [char, code] of Object.entries(codes)) reversed[code] = char;
    let result = "", temp = "";
    for (let bit of binary) {
        temp += bit;
        if (reversed[temp]) {
            result += reversed[temp];
            temp = "";
        }
    }
    return result;
}

document.getElementById("compressBtn").addEventListener("click", () => {
    const text = document.getElementById("inputText").value;
    if (!text) return alert("Enter some text to compress!");
    const {codes, binary} = compressText(text);

    document.getElementById("codesOutput").textContent = JSON.stringify(codes, null, 2);
    document.getElementById("binaryOutput").textContent = binary;

    downloadCompressed(binary);
    localStorage.setItem("huffmanCodes", JSON.stringify(codes));
});

document.getElementById("decompressBtn").addEventListener("click", () => {
    const file = document.getElementById("fileInput").files[0];
    if (!file) return alert("Select a .bin file first!");

    const reader = new FileReader();
    reader.onload = function(e) {
        const array = new Uint8Array(e.target.result);
        let binary = "";
        for (let byte of array) binary += byte.toString(2).padStart(8, "0");
        const codes = JSON.parse(localStorage.getItem("huffmanCodes") || "{}");
        const text = decompressBinary(binary, codes);
        document.getElementById("decompressedOutput").textContent = text;
    };
    reader.readAsArrayBuffer(file);
});
