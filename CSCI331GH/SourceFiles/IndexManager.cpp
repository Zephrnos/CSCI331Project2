#include "IndexManager.h"
#include "ZipCodeRecordBuffer.h"
#include "HeaderBuffer.h"

#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <fstream>

/**
 * @brief Builds the index by scanning through the binary data file.
 * * Each record starts with a 4-byte length field followed by CSV data.
 * We read each record, extract the ZIP code, and record its offset.
 */
void IndexManager::buildIndex(const std::string& dataFileName) {
    std::ifstream dataFile(dataFileName, std::ios::binary);
    if (!dataFile.is_open()) {
        std::cerr << "Error: Cannot open " << dataFileName << " for indexing.\n";
        return;
    }

    indexMap.clear();

    HeaderRecordBuffer header;
    if (!header.readHeader(dataFile)) {
        std::cerr << "Error reading header in IndexManager.\n";
        return;
    }

    uint64_t offset = static_cast<uint64_t>(dataFile.tellg());
    uint32_t recordLength = 0;

    // CORRECTED: The offset is now captured BEFORE reading the record length.
    while (dataFile.read(reinterpret_cast<char*>(&recordLength), sizeof(recordLength))) {
        std::string record(recordLength, '\0');
        // Read the record data but don't advance the main file pointer yet,
        // so we can seek from our current position easily.
        dataFile.read(&record[0], recordLength);

        std::istringstream ss(record);
        std::string zip;
        std::getline(ss, zip, ',');

        // Remove quotes and whitespace
        zip.erase(std::remove(zip.begin(), zip.end(), '"'), zip.end());
        zip.erase(std::remove_if(zip.begin(), zip.end(),
            [](unsigned char c){ return std::isspace(c); }), zip.end());

        // Only keep numeric ZIP codes
        if (!zip.empty() && std::all_of(zip.begin(), zip.end(), ::isdigit)) {
            indexMap[zip] = offset;
        }
        
        // Update offset for the next record
        offset = static_cast<uint64_t>(dataFile.tellg());
    }

    dataFile.close();
}

/**
 * @brief Writes the in-memory index map to a binary file.
 *
 * Format:
 * [entryCount:uint32_t]
 * For each entry:
 * [keyLen:uint16_t][ZIP chars][offset:uint64_t]
 */
void IndexManager::writeIndex(const std::string& indexFileName) const {
    std::ofstream out(indexFileName, std::ios::binary);
    if (!out) {
        std::cerr << "Error: Cannot open " << indexFileName << " for writing.\n";
        return;
    }

    uint32_t count = static_cast<uint32_t>(indexMap.size());
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& entry : indexMap) {
        uint16_t keyLen = static_cast<uint16_t>(entry.first.size());
        out.write(reinterpret_cast<const char*>(&keyLen), sizeof(keyLen));
        out.write(entry.first.c_str(), keyLen);
        out.write(reinterpret_cast<const char*>(&entry.second), sizeof(entry.second));
    }

    out.close();
}

/**
 * @brief Reads an index file from disk back into memory.
 */
void IndexManager::readIndex(const std::string& indexFileName) {
    std::ifstream in(indexFileName, std::ios::binary);
    if (!in) {
        std::cerr << "Error: Cannot open " << indexFileName << " for reading.\n";
        return;
    }

    indexMap.clear();

    uint32_t count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (uint32_t i = 0; i < count; ++i) {
        uint16_t keyLen = 0;
        in.read(reinterpret_cast<char*>(&keyLen), sizeof(keyLen));

        std::string zip(keyLen, '\0');
        in.read(&zip[0], keyLen);

        uint64_t offset = 0;
        in.read(reinterpret_cast<char*>(&offset), sizeof(offset));

        indexMap[zip] = offset;
    }

    in.close();
    std::cout << "Loaded index with " << count << " entries.\n";
}

/**
 * @brief Finds the byte offset for a ZIP code in the index.
 */
uint64_t IndexManager::findOffset(const std::string& zip) const {
    auto it = indexMap.find(zip);
    return (it != indexMap.end()) ? it->second : UINT64_MAX;
}