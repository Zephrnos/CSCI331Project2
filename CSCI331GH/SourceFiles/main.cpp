#include <map>
#include <iomanip>
#include <string>
#include <iostream>
#include <limits> // For numeric_limits
#include <sstream>
#include <fstream>
#include "ZipCodeRecordBuffer.h"
#include "HeaderBuffer.h"
#include "convertCSV.h"
#include "IndexManager.h"

using namespace std;

// A struct to hold the four extreme zip codes AND their coordinates.
struct StateRecord {
    string easternmost_zip;
    double easternmost_lon = -numeric_limits<double>::max(); // Initialize with a very small number
    string westernmost_zip;
    double westernmost_lon = numeric_limits<double>::max();  // Initialize with a very large number
    string northernmost_zip;
    double northernmost_lat = -numeric_limits<double>::max();
    string southernmost_zip;
    double southernmost_lat = numeric_limits<double>::max();
};

int main() {
    binaryToCSV(); // Convert CSV to binary and back to CSV

    // --- Part 1: Compute state extremes ---
    map<string, StateRecord> all_states;
    ZipCodeRecordBuffer buffer;
    ifstream file("../CSCI331GH/Data/converted_postal_codes.csv");

    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    string header;
    getline(file, header);

    while (buffer.ReadRecord(file)) {
        string state = buffer.getState();
        string zip = buffer.getZipCode();
        double latitude = buffer.getLatitude();
        double longitude = buffer.getLongitude();

        if (all_states.find(state) == all_states.end()) {
            all_states[state] = StateRecord{};
        }

        StateRecord& record = all_states[state];

        if (longitude > record.easternmost_lon) { record.easternmost_lon = longitude; record.easternmost_zip = zip; }
        if (longitude < record.westernmost_lon) { record.westernmost_lon = longitude; record.westernmost_zip = zip; }
        if (latitude > record.northernmost_lat) { record.northernmost_lat = latitude; record.northernmost_zip = zip; }
        if (latitude < record.southernmost_lat) { record.southernmost_lat = latitude; record.southernmost_zip = zip; }
    }
    file.close();

    
/*  //Used for project 1, unsure if we still need this for project 2
    cout << left << setw(8) << "State"
         << setw(15) << "Easternmost"
         << setw(15) << "Westernmost"
         << setw(15) << "Northernmost"
         << setw(15) << "Southernmost"
         << "\n";
    cout << string(68, '-') << "\n";
*/


    for (const auto& pair : all_states) {
        const auto& record = pair.second;
        cout << left << setw(8) << pair.first
             << setw(15) << record.easternmost_zip
             << setw(15) << record.westernmost_zip
             << setw(15) << record.northernmost_zip
             << setw(15) << record.southernmost_zip
             << "\n";
    }

    // --- Part 2: Look up ZIP code(s) using the index ---
    IndexManager index;
    index.readIndex("Data/zip.idx");

    string zipInput;
    cout << "\nEnter a ZIP code to look up (or 'exit' to quit): ";
    while (cin >> zipInput) {
        if (zipInput == "exit") break;

        uint64_t offset = index.findOffset(zipInput);
        if (offset == UINT64_MAX) {
            cerr << "ZIP code " << zipInput << " not found.\n";
        } else {
            ifstream binFile("Data/zip_len.dat", ios::binary);
            if (!binFile.is_open()) {
                cerr << "Error opening binary file.\n";
                continue;
            }

            binFile.seekg(offset);
            uint32_t recordLength;
            binFile.read(reinterpret_cast<char*>(&recordLength), sizeof(recordLength));
            string record(recordLength, '\0');
            binFile.read(&record[0], recordLength);

            ZipCodeRecordBuffer zipBuffer;
            istringstream ss(record);
            if (zipBuffer.ReadRecord(ss)) {
                cout << "ZIP Code: " << zipBuffer.getZipCode() << "\n"
                     << "Place Name: " << zipBuffer.getPlaceName() << "\n"
                     << "State: " << zipBuffer.getState() << "\n"
                     << "County: " << zipBuffer.getCounty() << "\n"
                     << "Latitude: " << zipBuffer.getLatitude() << "\n"
                     << "Longitude: " << zipBuffer.getLongitude() << "\n";
            } else {
                cerr << "Error parsing record for ZIP code " << zipInput << endl;
            }
            binFile.close();
        }
        cout << "\nEnter another ZIP code (or 'exit' to quit): ";
    }

    return 0;
}
