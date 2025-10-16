#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "convertCSV.h"
#include "readBinaryFile.h"
#include "HeaderBuffer.h"
#include "IndexManager.h"
#include <algorithm>
#include <vector>

using namespace std;

// Writes a string record to the output file with a 4-byte length prefix.
void writeLengthPrefixedRecord(ofstream& output, const string& record) {
    uint32_t recordLength = static_cast<uint32_t>(record.length());
    output.write(reinterpret_cast<const char*>(&recordLength), sizeof(recordLength));
    output.write(record.c_str(), recordLength);
}

// Processes the CSV file to create the binary data file.
void processFile(const string& inputFileName, const string& outputFileName) {
    ifstream inputFile(inputFileName);
    ofstream outputFile(outputFileName, ios::binary);

    if (!inputFile.is_open()) {
        cerr << "Error opening file " << inputFileName << endl;
        return;
    }
    if (!outputFile.is_open()) {
        cerr << "Error opening file " << outputFileName << endl;
        return;
    }

    // --- Count Records ---
    // We count lines first to populate the header correctly.
    string tempLine;
    uint32_t recordCount = 0;
    getline(inputFile, tempLine); // Skip header for counting
    while (getline(inputFile, tempLine)) {
        if (!tempLine.empty()) {
            recordCount++;
        }
    }

    // --- Write Header ---
    HeaderRecordBuffer header;
    header.setVersion(1);
    header.setRecordCount(recordCount);
    header.setCreationDate("2025-10-16");
    header.writeHeader(outputFile);

    // --- Write Records ---
    // Reset file to read from the beginning again
    inputFile.clear();
    inputFile.seekg(0, ios::beg);

    // CORRECTED: Explicitly skip the header row before writing records.
    string line;
    getline(inputFile, line); 

    while (getline(inputFile, line)) {
        if (!line.empty()) {
            writeLengthPrefixedRecord(outputFile, line);
        }
    }

    inputFile.close();
    outputFile.close();
}


// A single function to create both the binary file and its index.
void createBinaryAndIndex(const string& inputCSV, const string& outputBinary, const string& outputIndex) {
    cout << "Processing " << inputCSV << " to create binary data file...\n";
    processFile(inputCSV, outputBinary);

    cout << "Building index from " << outputBinary << "...\n";
    IndexManager index;
    index.buildIndex(outputBinary);
    index.writeIndex(outputIndex);
    cout << "Index file " << outputIndex << " created successfully.\n";
}