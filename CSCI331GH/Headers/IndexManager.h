#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include <map>
#include <string>
#include <cstdint>
#include <fstream>
#include <iostream>

/**
 * @class IndexManager
 * @brief Builds and manages the ZIP → offset index for binary ZIP data files.
 *
 * This class stores a mapping from ZIP codes (strings)
 * to their file offsets (64-bit integers).
 */
class IndexManager {
private:
    std::map<std::string, uint64_t> indexMap; /**< ZIP → file offset. */

public:
    /** 
     * @brief Builds the index from a binary data file.
     * @param dataFileName Path to the binary file (zip_len.dat).
     */
    void buildIndex(const std::string& dataFileName);

    /** 
     * @brief Writes the in-memory index to a binary index file (zip.idx).
     * @param indexFileName Path to output index file.
     */
    void writeIndex(const std::string& indexFileName) const;

    /**
     * @brief Loads an index from an existing binary index file.
     * @param indexFileName Path to input index file.
     */
    void readIndex(const std::string& indexFileName);

    /**
     * @brief Finds the offset for a specific ZIP code.
     * @param zip The ZIP code to find.
     * @return Byte offset if found, or UINT64_MAX if not found.
     */
    uint64_t findOffset(const std::string& zip) const;

    /** @brief Returns how many entries are in the index. */
    size_t size() const { return indexMap.size(); }
};

#endif
