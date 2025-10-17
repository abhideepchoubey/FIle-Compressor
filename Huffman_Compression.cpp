#include <bits/stdc++.h>
using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    Node(Node* l, Node* r) : ch('\0'), freq(l->freq + r->freq), left(l), right(r) {}
};

struct compare {
    bool operator()(Node* a, Node* b) { return a->freq > b->freq; }
};

void buildCodes(Node* root, string str, unordered_map<char,string>& codes) {
    if(!root) return;
    if(!root->left && !root->right) codes[root->ch] = str;
    buildCodes(root->left, str + "0", codes);
    buildCodes(root->right, str + "1", codes);
}

string readFile(const string& filename) {
    ifstream in(filename);
    if(!in) {
        cerr << "Error: Cannot open input file.\n";
        exit(1);
    }
    stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

void writeBinaryFile(const string& binary, const string& filename) {
    vector<unsigned char> bytes;
    for(size_t i = 0; i < binary.size(); i += 8) {
        string byteStr = binary.substr(i, 8);
        while(byteStr.size() < 8) byteStr += "0";
        bytes.push_back(stoi(byteStr, nullptr, 2));
    }
    ofstream out(filename, ios::binary);
    out.write((char*)bytes.data(), bytes.size());
    out.close();
}

void saveCodes(const unordered_map<char,string>& codes, const string& filename) {
    ofstream out(filename);
    for(auto& p : codes) {
        if(p.first == '\n') out << "\\n " << p.second << "\n";
        else if(p.first == ' ') out << "space " << p.second << "\n";
        else out << p.first << " " << p.second << "\n";
    }
    out.close();
}

int main() {
    string inputText = readFile("input.txt");
    if(inputText.empty()) {
        cout << "Input file is empty.\n";
        return 0;
    }

    unordered_map<char,int> freq;
    for(char c : inputText) freq[c]++;

    priority_queue<Node*, vector<Node*>, compare> pq;
    for(auto& p : freq) pq.push(new Node(p.first, p.second));

    while(pq.size() > 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        pq.push(new Node(left, right));
    }

    Node* root = pq.top();
    unordered_map<char,string> codes;
    buildCodes(root, "", codes);

    string binary = "";
    for(char c : inputText) binary += codes[c];

    writeBinaryFile(binary, "compressed.bin");
    
    ofstream outBits("compressed_bits.txt");
    outBits << binary;
    outBits.close();

    saveCodes(codes, "codes.txt");

    cout << "\n✅ Compression complete!\n";
    cout << "→ Compressed binary file saved as: compressed.bin\n";
    cout << "→ Readable binary saved as: compressed_bits.txt\n";
    cout << "→ Huffman codes saved as: codes.txt\n";

    cout << "\n📘 Huffman Codes:\n";
    for(auto& p : codes) cout << (p.first == '\n' ? "\\n" : string(1,p.first)) << " : " << p.second << "\n";

    cout << "\n🧩 Compressed Binary (first 200 bits):\n";
    cout << binary.substr(0, 200) << (binary.size() > 200 ? "..." : "") << "\n";
    cout << "Total bits: " << binary.size() << "\n";

    return 0;
}
