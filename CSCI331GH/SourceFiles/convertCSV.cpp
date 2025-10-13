#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "lenReadWrite.h"
using namespace std;

//Authors: Team 5
//Date: 2025-10-11
/*Purpose: This file contains the implementation of functions to read a CSV file and
 write its contents to a binary file with length-prefixed records. */

void processFile(string& inputFileName, const string& outputFileName) {
    ifstream inputFile(inputFileName);
    ofstream outputFile(outputFileName, ios::binary);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "Error opening file!" << endl;
        return;
    }

    string line;

    while (getline(inputFile, line)) {
        // Call your length-prefixed writing function correctly
        lenRead(outputFile, line);
    }

    inputFile.close();
    outputFile.close();
}

// Consider renaming lenRead to something more descriptive!
void lenRead(ofstream& output, const string& record) {
    size_t recordLength = record.length();
    output.write(reinterpret_cast<const char*>(&recordLength), sizeof(recordLength)); // length prefix
    output.write(record.c_str(), recordLength);
}
