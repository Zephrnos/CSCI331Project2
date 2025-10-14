#ifndef ZipCodeRecordBuffer_H
#define ZipCodeRecordBuffer_H

#include <string>
#include <fstream>
#include <sstream>
#include <limits>

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

    bool ReadRecord(std::istream& file) {
        std::string line;
        if (!getline(file, line)) return false;

        std::istringstream ss(line);
        std::string field;
        int field_count = 0;

        while (field_count < 6 && getline(ss, field, ',')) {
            // Optional: trim leading/trailing whitespace
            // field.erase(field.begin(), std::find_if(field.begin(), field.end(), [](int ch) { return !std::isspace(ch); }));
            // field.erase(std::find_if(field.rbegin(), field.rend(), [](int ch) { return !std::isspace(ch); }).base(), field.end());

            // Truncate if necessary
            if (field_count == 0 && field.length() > ZIP_CODE_LENGTH) field = field.substr(0, ZIP_CODE_LENGTH);
            else if (field_count == 1 && field.length() > PLACE_NAME_LENGTH) field = field.substr(0, PLACE_NAME_LENGTH);
            else if (field_count == 2 && field.length() > STATE_LENGTH) field = field.substr(0, STATE_LENGTH);
            else if (field_count == 3 && field.length() > COUNTY_LENGTH) field = field.substr(0, COUNTY_LENGTH);
            else if ((field_count == 4 || field_count == 5) && field.length() > LAT_LONG_LENGTH) field = field.substr(0, LAT_LONG_LENGTH);

            m_fields[field_count++] = field;
        }
        if (field_count != 6) return false;

        if (isHeaderRow()) return false;

        try {
            latitude = stod(m_fields[4]);
            longitude = stod(m_fields[5]);
        } catch (...) {
            return false;
        }
        return true;
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


    bool isHeaderRow() const {
        // If first field equals ZipCode or contains non-digits (very likely header), or lat/long are non-numeric
        std::string f0 = m_fields[0];
        for (auto &c : f0) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        if (f0.find("ZIP") != std::string::npos) return true;
        // quick check lat/long not numeric
        bool latNumeric = !m_fields[4].empty() && ( (m_fields[4][0] == '-') || std::isdigit(static_cast<unsigned char>(m_fields[4][0])) );
        bool lonNumeric = !m_fields[5].empty() && ( (m_fields[5][0] == '-') || std::isdigit(static_cast<unsigned char>(m_fields[5][0])) );
        if (!latNumeric || !lonNumeric) return true;
        return false;
    }
};

#endif // ZipCodeRecordBuffer_H
