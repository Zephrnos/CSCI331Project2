#ifndef convertCSV_h
#define convertCSV_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// A single function to create both the binary file and its index.
void createBinaryAndIndex(const std::string& inputCSV, const std::string& outputBinary, const std::string& outputIndex);


#endif // convertCSV_h
