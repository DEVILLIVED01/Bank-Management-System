#include <iostream>
#include <fstream>
#include <string>
using namespace std;
void xorEncryptDecrypt(const string& inputFile, const string& outputFile, const char key) {
    ifstream input(inputFile, ios::binary);
    ofstream output(outputFile, ios::binary);
    if (!input.is_open()) {
        cerr << "Error: Could not open input file!" << endl;
        return;
    }
    if (!output.is_open()) {
        cerr << "Error: Could not open output file!" << endl;
        return;
    }
    char byte;
    while (input.get(byte)) {
        byte ^= key;  
        output.put(byte);
    }
    cout << "File processed successfully!" << endl;
    input.close();
    output.close();
}
int main() {
    string inputFile, outputFile;
    char key;
    int choice;
    cout << "Enter a single character key for encryption/decryption: ";
    cin >> key;
    cout << "Choose operation:\n1. Encrypt\n2. Decrypt\n";
    cout << "Enter your choice (1 or 2): ";
    cin >> choice;
    cout << "Enter the path of the input file: ";
    cin >> inputFile;
    cout << "Enter the path for the output file: ";
    cin >> outputFile;
    if (choice == 1) {
        xorEncryptDecrypt(inputFile, outputFile, key);
    }
    else if (choice == 2) {
        xorEncryptDecrypt(inputFile, outputFile, key);
    }
    else {
        cout << "Invalid choice!" << endl;
    }
}
