#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "lenReadWrite.h"
using namespace std;

//Authors: Team 5
//Date: 2025-10-11
/*Purpose: This file contains the implementation of functions to read a CSV file and
 write its contents to a binary file with length - prefixed records. */




void processFile(string& inputFileName, const string& outputFileName) {
	ifstream inputFile(inputFileName);
	ofstream outputFile(outputFileName, ios::binary);

	if (!inputFile.is_open() || !outputFile.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		return;
	}


	string line;

	while (std::getline(inputFile, line)) {
		// Process each line of the CSV and write it with the length prefix
		writeWithLengthPrefix(outputFile, line);
	}

	inputFile.close();
	outputFile.close();

}

void lenRead(ofstream& output, const string& record) {

	size_t recordLength = record.length(); 
	outputFile.write(reinterpret_cast<const char*>(&recordLength), sizeof(recordLength)); //fixed:4 bytes

	outputFile.write(record.c_str(), recordLength);




}