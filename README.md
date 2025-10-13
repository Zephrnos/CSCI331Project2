# 📦 Zip Code Processing System (Group Project 2.0)

## Overview
This project extends *Group Project 1.0* by introducing a new **length-indicated file format** and a **primary-key index** for random access to ZIP-code records.  
It is written in modern **C++17** and demonstrates file structures, buffering, indexing, and command-line interaction.

The system reads, writes, and indexes U.S. ZIP-code data that includes latitude, longitude, and demographic information.

---

## 🎯 Project Goals
1. **Process sequential CSV data** using a buffer class (from Group Project 1.0).  
2. **Convert CSVs** to a **length-indicated file structure** with a header record.  
3. **Read and unpack** length-indicated ZIP-code records.  
4. **Add a header buffer class** for file metadata.  
5. **Build a primary-key index** mapping ZIP → file offset.  
6. **Allow command-line record lookups** using flags such as `-Z56301`.  
7. Maintain functional equivalence with Group Project 1.0 when using randomized XLSX data exported to CSV.

---

## 🧩 File Formats

### 1. Length-Indicated Data File (`zip_len.dat`)





### 2. Index File (`zip.idx`)
Binary index for fast lookups:
| Field | Type | Description |
|--------|------|-------------|
| Entry count | `uint32_t` | Number of entries |
| Key length | `uint16_t` | Length of ZIP string |
| Key | `char[]` | ZIP code |
| Offset | `uint64_t` | Byte offset of record in data file |

---

## 🏗️ Core Components

| Class | Purpose | Key Methods |
|--------|----------|-------------|
| **`ZipCodeRecord`** | Holds a single record and packs/unpacks CSV lines. | `pack()`, `unpack()` |
| **`LengthBuffer`** | Reads/writes variable-length records with 4-byte prefixes. | `writeRecord()`, `readNextRecord()`, `readRecordAt()` |
| **`HeaderBuffer`** | Reads and writes the header record. | `writeHeader()`, `readHeader()` |
| **`IndexManager`** | Builds and manages ZIP→offset mappings. | `buildIndex()`, `writeIndex()`, `readIndex()`, `findOffset()` |

---

## ⚙️ Build Instructions

### Requirements
- C++17 or newer
- CMake ≥ 3.10 or `g++`/`clang++`
- Optional: a CSV parser library (for robust CSV quoting)

### Build with CMake
```bash
mkdir build
cd build
cmake ..
make

Authors:
Team 5
10/12/2025

