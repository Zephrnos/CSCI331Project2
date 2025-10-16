#ifndef ZipCodeRecordBuffer_H
#define ZipCodeRecordBuffer_H

#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cctype>
#include <vector>

const int ZIP_CODE_LENGTH = 5;
const int PLACE_NAME_LENGTH = 50;
const int STATE_LENGTH = 2;
const int COUNTY_LENGTH = 50;
const int LAT_LONG_LENGTH = 10;

class ZipCodeRecordBuffer {
public:
    ZipCodeRecordBuffer() {
        for (int i = 0; i < 6; ++i) m_fields[i] = "";
    }

    // Reads a single, valid 6-field data record from a stream
    bool ReadRecord(std::istream& file) {
        std::string line;
        if (std::getline(file, line)) {
            if (line.empty()) return false;

            std::vector<std::string> fields;
            std::istringstream ss(line);
            std::string token;
            while (std::getline(ss, token, ',')) {
                trim(token);
                if (token.size() >= 2 && token.front() == '"' && token.back() == '"') {
                    token = token.substr(1, token.size() - 2);
                    trim(token);
                }
                fields.push_back(token);
            }
            
            // Expect exactly 6 fields for a valid zip data record
            if (fields.size() != 6) return false;
            
            // Map into m_fields
            m_fields[0] = truncateTo(fields[0], ZIP_CODE_LENGTH);
            m_fields[1] = truncateTo(fields[1], PLACE_NAME_LENGTH);
            m_fields[2] = truncateTo(fields[2], STATE_LENGTH);
            m_fields[3] = truncateTo(fields[3], COUNTY_LENGTH);
            std::string latStr = truncateTo(fields[4], LAT_LONG_LENGTH);
            std::string lonStr = truncateTo(fields[5], LAT_LONG_LENGTH);

            try {
                latitude = std::stod(latStr);
                longitude = std::stod(lonStr);
            } catch (...) {
                return false; // malformed lat/lon
            }

            return true; // Success
        }
        return false; // EOF
    }

    std::string getZipCode() const { return m_fields[0]; }
    std::string getPlaceName() const { return m_fields[1]; }
    std::string getState() const { return m_fields[2]; }
    std::string getCounty() const { return m_fields[3]; }
    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }

private:
    std::string m_fields[6];
    double latitude = std::numeric_limits<double>::quiet_NaN();
    double longitude = std::numeric_limits<double>::quiet_NaN();

    static inline void trim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    static inline std::string truncateTo(const std::string &s, size_t maxLen) {
        if (s.length() <= maxLen) return s;
        return s.substr(0, maxLen);
    }
};

#endif // ZipCodeRecordBuffer_H
