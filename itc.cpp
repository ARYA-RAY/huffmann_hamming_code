#include <iostream>
#include <queue>
#include <unordered_map>
#include <bitset>
#include <vector>
using namespace std;

struct Node {
    char ch;
    int freq;
    Node *left, *right;
    Node(char character, int frequency, Node* l = nullptr, Node* r = nullptr) {
        ch = character;
        freq = frequency;
        left = l;
        right = r;
    }
};

struct Compare {
    bool operator()(Node* left, Node* right) {
        return left->freq > right->freq;
    }
};

void storeCodes(Node* root, string code, unordered_map<char, string> &huffmanCode) {
    if (!root) return;
    if (!root->left && !root->right) {
        huffmanCode[root->ch] = code;
    }
    storeCodes(root->left, code + "0", huffmanCode);
    storeCodes(root->right, code + "1", huffmanCode);
}

void printHuffmanTree(Node* root, string code = "") {
    if (!root) return;
    if (!root->left && !root->right) {
        cout << "'" << root->ch << "' : " << code << " (Frequency: " << root->freq << ")" << endl;
    }
    printHuffmanTree(root->left, code + "0");
    printHuffmanTree(root->right, code + "1");
}

Node* buildHuffmanTree(string text, unordered_map<char, string> &huffmanCode) {
    unordered_map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;
    }
    
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto pair : freq) {
        pq.push(new Node(pair.first, pair.second));
    }
    
    while (pq.size() != 1) {
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        int sum = left->freq + right->freq;
        pq.push(new Node('\0', sum, left, right));
    }
    
    Node* root = pq.top();
    storeCodes(root, "", huffmanCode);
    return root;
}

string encodeHuffman(string text, unordered_map<char, string> huffmanCode) {
    string encoded = "";
    for (char ch : text) {
        encoded += huffmanCode[ch];
    }
    return encoded;
}

string decodeHuffman(Node* root, string encodedText) {
    string decoded = "";
    Node* current = root;
    for (char bit : encodedText) {
        if (bit == '0') current = current->left;
        else current = current->right;
        
        if (!current->left && !current->right) {
            decoded += current->ch;
            current = root;
        }
    }
    return decoded;
}

bitset<7> generateHammingCode(bitset<4> data) {
    bitset<7> hammingCode;
    hammingCode[2] = data[0];
    hammingCode[4] = data[1];
    hammingCode[5] = data[2];
    hammingCode[6] = data[3];

    hammingCode[0] = hammingCode[2] ^ hammingCode[4] ^ hammingCode[6];
    hammingCode[1] = hammingCode[2] ^ hammingCode[5] ^ hammingCode[6];
    hammingCode[3] = hammingCode[4] ^ hammingCode[5] ^ hammingCode[6];
    
    return hammingCode;
}

bitset<4> correctHammingCode(bitset<7> hammingCode) {
    bitset<4> data;
    int parity1 = hammingCode[0] ^ hammingCode[2] ^ hammingCode[4] ^ hammingCode[6];
    int parity2 = hammingCode[1] ^ hammingCode[2] ^ hammingCode[5] ^ hammingCode[6];
    int parity4 = hammingCode[3] ^ hammingCode[4] ^ hammingCode[5] ^ hammingCode[6];
    
    int errorPosition = parity1 * 1 + parity2 * 2 + parity4 * 4;

    if (errorPosition != 0) {
        hammingCode.flip(errorPosition - 1);
    }

    data[0] = hammingCode[2];
    data[1] = hammingCode[4];
    data[2] = hammingCode[5];
    data[3] = hammingCode[6];

    return data;
}

vector<bitset<7>> encodeHamming(string encodedText) {
    vector<bitset<7>> hammingBlocks;
    for (size_t i = 0; i < encodedText.size(); i += 4) {
        bitset<4> data(0);
        for (size_t j = 0; j < 4 && i + j < encodedText.size(); ++j) {
            data[3 - j] = encodedText[i + j] - '0';
        }
        bitset<7> hammingCode = generateHammingCode(data);
        hammingBlocks.push_back(hammingCode);
        cout << "Block " << (i / 4 + 1) << ": " << hammingCode << endl;
    }
    return hammingBlocks;
}

string decodeHamming(vector<bitset<7>> hammingBlocks) {
    string decodedBits = "";
    for (auto block : hammingBlocks) {
        bitset<4> correctedData = correctHammingCode(block);
        for (int i = 3; i >= 0; --i) {
            decodedBits += correctedData[i] ? '1' : '0';
        }
    }
    return decodedBits;
}

int main() {
    int choice;
    do {
        cout << "\nMenu:\n";
        cout << "1. Huffman Encode/Decode\n";
        cout << "2. Hamming Encode/Decode\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: {
                string text;
                cout << "Enter original text to encode: ";
                getline(cin, text);

                unordered_map<char, string> huffmanCode;
                Node* root = buildHuffmanTree(text, huffmanCode);
                
                string encodedText = encodeHuffman(text, huffmanCode);
                cout << "\nHuffman Encoded Text: " << encodedText << endl;

                cout << "\nHuffman Tree:\n";
                printHuffmanTree(root);

                string inputEncodedText;
                cout << "\nEnter the Huffman encoded text for decoding: ";
                cin >> inputEncodedText;

                string decodedText = decodeHuffman(root, inputEncodedText);
                cout << "\nHuffman Decoded Text: " << decodedText << endl;
                break;
            }

            case 2: {
                string dataString;
                cout << "Enter binary data for Hamming encoding: ";
                cin >> dataString;

                vector<bitset<7>> hammingBlocks = encodeHamming(dataString);

                int errorBlock;
                cout << "\nEnter the block number to introduce error (or 0 for no error): ";
                cin >> errorBlock;

                if (errorBlock > 0 && errorBlock <= hammingBlocks.size()) {
                    hammingBlocks[errorBlock - 1].flip(rand() % 7);
                    cout << "Error introduced in block " << errorBlock << ": " << hammingBlocks[errorBlock - 1] << endl;
                }

                string decodedBits = decodeHamming(hammingBlocks);
                decodedBits = decodedBits.substr(0, dataString.length());

                cout << "\nDecoded Data (after error correction): " << decodedBits << endl;
                break;
            }

            case 3:
                cout << "Exiting..." << endl;
                break;

            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    } while (choice != 3);

    return 0;
}
