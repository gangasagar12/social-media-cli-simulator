#include "FileHandler.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void FileHandler::saveBinary(const string &filename, const vector<string> &lines) {
    ofstream fout(filename, ios::binary);
    if (!fout) {
        cerr << "Error saving binary file: " << filename << "\n";
        return;
    }
    
    // Write number of lines
    size_t numLines = lines.size();
    fout.write(reinterpret_cast<const char*>(&numLines), sizeof(numLines));
    
    // Write each line
    for (const auto &ln : lines) {
        size_t length = ln.size();
        fout.write(reinterpret_cast<const char*>(&length), sizeof(length));
        fout.write(ln.c_str(), length);
    }
}

vector<string> FileHandler::loadBinary(const string &filename) {
    vector<string> lines;
    ifstream fin(filename, ios::binary);
    
    if (!fin) {
        // Try to load from text file (for backward compatibility)
        return loadTextBackup(filename);
    }
    
    // Read number of lines
    size_t numLines = 0;
    fin.read(reinterpret_cast<char*>(&numLines), sizeof(numLines));
    
    // Check if we reached EOF or encountered an error
    if (!fin.good() && !fin.eof()) {
        cerr << "Error reading binary file: " << filename << "\n";
        return lines;
    }
    
    // Read each line
    for (size_t i = 0; i < numLines; ++i) {
        size_t length = 0;
        fin.read(reinterpret_cast<char*>(&length), sizeof(length));
        
        if (!fin.good()) {
            cerr << "Error reading line length from: " << filename << "\n";
            break;
        }
        
        string line(length, '\0');
        fin.read(&line[0], length);
        
        if (fin.good()) {
            lines.push_back(line);
        } else {
            cerr << "Error reading line content from: " << filename << "\n";
            break;
        }
    }
    
    return lines;
}

vector<string> FileHandler::loadTextBackup(const string &filename) {
    vector<string> lines;
    // Try to find .txt version for backward compatibility
    string txtFilename = filename;
    size_t dotPos = txtFilename.find_last_of('.');
    if (dotPos != string::npos) {
        txtFilename = txtFilename.substr(0, dotPos) + ".txt";
    }
    
    ifstream fin(txtFilename);
    if (!fin) {
        // If .txt doesn't exist, check .bin as text (last resort)
        fin.open(filename);
        if (!fin) return lines;
    }
    
    string line;
    while (getline(fin, line)) {
        if (!line.empty()) lines.push_back(line);
    }
    
    // If we loaded from .txt, save as .bin for next time
    if (txtFilename != filename && !lines.empty()) {
        saveBinary(filename, lines);
    }
    
    return lines;
}