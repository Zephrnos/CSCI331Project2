#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "HeaderBuffer.h"
#include "readBinaryFile.h"
#include "ZipCodeRecordBuffer.h"

using namespace std;

void readBinaryFile(const string& inputFileName, const string& outputFileName) {
    ifstream inputFile(inputFileName, ios::binary);
    ofstream outputFile(outputFileName);

    if (!inputFile.is_open()) {
        cerr << "Error opening binary file!" << endl;
        return;
    }
    if (!outputFile.is_open()) {
        cerr << "Error opening CSV file!" << endl;
        return;
    }

    // Read header first
    HeaderRecordBuffer header;
    if (!header.readHeader(inputFile)) {
        cerr << "Failed to read header record!" << endl;
        return;
    }

    cout << "File version: " << header.getVersion() << endl;
    cout << "Record count: " << header.getRecordCount() << endl;
    cout << "Creation date: " << header.getCreationDate() << endl;

    // Write CSV header row
    outputFile << "RecordLength,ZipCode,PlaceName,State,County,Latitude,Longitude\n";

    // Read all records according to header record count
    for (uint32_t i = 0; i < header.getRecordCount(); ++i) {
        uint32_t recordLength;
        if (!inputFile.read(reinterpret_cast<char*>(&recordLength), sizeof(recordLength))) {
            if (inputFile.eof()) {
                 cout << "Reached end of file unexpectedly. Processed " << i << " records." << endl;
            } else {
                 cerr << "Error reading record length for record " << i << "!" << endl;
            }
            break;
        }

        string record(recordLength, '\0');
        if (!inputFile.read(&record[0], recordLength)) {
            cerr << "Error reading record data for record " << i << "!" << endl;
            break;
        }

        ZipCodeRecordBuffer buffer;
        istringstream ss(record);
        // Use the simplified ReadRecord which expects 6 fields
        if (buffer.ReadRecord(ss)) {
            outputFile << recordLength << ","
                << buffer.getZipCode() << ","
                << buffer.getPlaceName() << ","
                << buffer.getState() << ","
                << buffer.getCounty() << ","
                << buffer.getLatitude() << ","
                << buffer.getLongitude() << "\n";
        }
        else {
            // REMOVED: The erroneous header check is no longer needed.
            std::cerr << "Error parsing record " << i << "!\n";
         }
    }

    inputFile.close();
    outputFile.close();

    cout << "CSV file with lengths has been written successfully!" << endl;
}
