/**
 * This license is for this file only not the whole project.
 * Original source is from: https://github.com/pulzed/mINI
 * From version v0.9.11
 *
 * The MIT License (MIT)
 * Copyright (c) 2018-2022 Danijel Durakovic
 * Copyright (c) 2022 Mohammad Bahoosh
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

///////////////////////////////////////////////////////////////////////////////
//
//  An INI file reader and writer for the modern age.
//
///////////////////////////////////////////////////////////////////////////////
//
//  A tiny utility library for manipulating INI files with a straightforward
//  API and a minimal footprint. It conforms to the (somewhat) standard INI
//  format - sections and keys are case insensitive and all leading and
//  trailing whitespace is ignored. Comments are lines that begin with a
//  semicolon. Trailing comments are allowed on section lines.
//
//  Files are read on demand, upon which data is kept in memory and the file
//  is closed. This utility supports lazy writing, which only writes changes
//  and updates to a file and preserves custom formatting and comments. A lazy
//  write invoked by a write() call will read the output file, find what
//  changes have been made and update the file accordingly. If you only need to
//  generate files, use generate() instead. Section and key order is preserved
//  on read, write and insert.
//
///////////////////////////////////////////////////////////////////////////////
//
//  /* BASIC USAGE EXAMPLE: */
//
//  /* read from file */
//  webpp::ini_file file("myfile.ini");
//  webpp::INIStructure ini;
//  file.read(ini);
//
//  /* read value; gets a reference to actual value in the structure.
//     if key or section don't exist, a new empty value will be created */
//  stl::string& value = ini["section"]["key"];
//
//  /* read value safely; gets a copy of value in the structure.
//     does not alter the structure */
//  stl::string value = ini.get("section").get("key");
//
//  /* set or update values */
//  ini["section"]["key"] = "value";
//
//  /* set multiple values */
//  ini["section2"].set({
//      {"key1", "value1"},
//      {"key2", "value2"}
//  });
//
//  /* write updates back to file, preserving comments and formatting */
//  file.write(ini);
//
//  /* or generate a file (overwrites the original) */
//  file.generate(ini);
//
///////////////////////////////////////////////////////////////////////////////
//
//  Long live the INI file!!!
//
///////////////////////////////////////////////////////////////////////////////

#ifndef WEBPP_INI_INI_H_
#define WEBPP_INI_INI_H_

#include "../std/string.hpp"
#include "../strings/to_case.hpp"
#include "../strings/trim.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <memory>
#include <sstream>
#include <sys/stat.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace webpp {
    namespace details {
        inline void replace(stl::string& str, stl::string const& a, stl::string const& b) {
            if (!a.empty()) {
                stl::size_t pos = 0;
                while ((pos = str.find(a, pos)) != stl::string::npos) {
                    str.replace(pos, a.size(), b);
                    pos += b.size();
                }
            }
        }
#ifdef _WIN32
        const char* const endl = "\r\n";
#else
        const char* const endl = "\n";
#endif
    } // namespace details

    template <typename T>
    class ini_map {
      private:
        using T_DataIndexMap  = stl::unordered_map<stl::string, stl::size_t>;
        using T_DataItem      = stl::pair<stl::string, T>;
        using T_DataContainer = stl::vector<T_DataItem>;
        using T_MultiArgs     = typename stl::vector<stl::pair<stl::string, T>>;

        T_DataIndexMap  dataIndexMap;
        T_DataContainer data;

        inline stl::size_t setEmpty(stl::string& key) {
            stl::size_t index = data.size();
            dataIndexMap[key] = index;
            data.emplace_back(key, T());
            return index;
        }

      public:
        using const_iterator = typename T_DataContainer::const_iterator;

        constexpr ini_map() = default;

        ini_map(ini_map const& other) {
            stl::size_t data_size = other.data.size();
            for (stl::size_t i = 0; i < data_size; ++i) {
                auto const& key = other.data[i].first;
                auto const& obj = other.data[i].second;
                data.emplace_back(key, obj);
            }
            dataIndexMap = T_DataIndexMap(other.dataIndexMap);
        }

        T& operator[](stl::string key) {
            ascii::trim(key);
#ifndef WEBPP_INI_CASE_SENSITIVE
            ascii::to_lower(key);
#endif
            auto        it    = dataIndexMap.find(key);
            bool        hasIt = (it != dataIndexMap.end());
            stl::size_t index = (hasIt) ? it->second : setEmpty(key);
            return data[index].second;
        }

        T get(stl::string key) const {
            ascii::trim(key);
#ifndef WEBPP_INI_CASE_SENSITIVE
            ascii::to_lower(key);
#endif
            auto it = dataIndexMap.find(key);
            if (it == dataIndexMap.end()) {
                return T();
            }
            return T(data[it->second].second);
        }

        bool has(stl::string key) const {
            ascii::trim(key);
#ifndef WEBPP_INI_CASE_SENSITIVE
            ascii::to_lower(key);
#endif
            return (dataIndexMap.count(key) == 1);
        }

        void set(stl::string key, T obj) {
            ascii::trim(key);
#ifndef WEBPP_INI_CASE_SENSITIVE
            ascii::to_lower(key);
#endif
            auto it = dataIndexMap.find(key);
            if (it != dataIndexMap.end()) {
                data[it->second].second = obj;
            } else {
                dataIndexMap[key] = data.size();
                data.emplace_back(key, obj);
            }
        }

        void set(T_MultiArgs const& multiArgs) {
            for (auto const& it : multiArgs) {
                auto const& key = it.first;
                auto const& obj = it.second;
                set(key, obj);
            }
        }

        bool remove(stl::string key) {
            ascii::trim(key);
#ifndef WEBPP_INI_CASE_SENSITIVE
            ascii::to_lower(key);
#endif
            auto it = dataIndexMap.find(key);
            if (it != dataIndexMap.end()) {
                stl::size_t index = it->second;
                data.erase(data.begin() + index);
                dataIndexMap.erase(it);
                for (auto& it2 : dataIndexMap) {
                    auto& vi = it2.second;
                    if (vi > index) {
                        vi--;
                    }
                }
                return true;
            }
            return false;
        }

        void clear() {
            data.clear();
            dataIndexMap.clear();
        }

        stl::size_t size() const noexcept {
            return data.size();
        }

        const_iterator begin() const {
            return data.begin();
        }

        const_iterator end() const {
            return data.end();
        }
    };

    using INIStructure = ini_map<ini_map<stl::string>>;

    namespace ini_parser {
        using T_ParseValues = stl::pair<stl::string, stl::string>;

        enum class PDataType : char {
            PDATA_NONE,
            PDATA_COMMENT,
            PDATA_SECTION,
            PDATA_KEYVALUE,
            PDATA_UNKNOWN
        };

        inline PDataType parseLine(stl::string line, T_ParseValues& parseData) {
            parseData.first.clear();
            parseData.second.clear();
            ascii::trim(line);
            if (line.empty()) {
                return PDataType::PDATA_NONE;
            }
            char firstCharacter = line[0];
            if (firstCharacter == ';') {
                return PDataType::PDATA_COMMENT;
            }
            if (firstCharacter == '[') {
                auto commentAt = line.find_first_of(';');
                if (commentAt != stl::string::npos) {
                    line = line.substr(0, commentAt);
                }
                auto closingBracketAt = line.find_last_of(']');
                if (closingBracketAt != stl::string::npos) {
                    auto section = line.substr(1, closingBracketAt - 1);
                    ascii::trim(section);
                    parseData.first = section;
                    return PDataType::PDATA_SECTION;
                }
            }
            auto lineNorm = line;
            details::replace(lineNorm, "\\=", "  ");
            auto equalsAt = lineNorm.find_first_of('=');
            if (equalsAt != stl::string::npos) {
                auto key = line.substr(0, equalsAt);
                ascii::trim(key);
                details::replace(key, "\\=", "=");
                auto value = line.substr(equalsAt + 1);
                ascii::trim(value);
                parseData.first  = key;
                parseData.second = value;
                return PDataType::PDATA_KEYVALUE;
            }
            return PDataType::PDATA_UNKNOWN;
        }
    } // namespace ini_parser

    class ini_reader {
      public:
        using T_LineData    = stl::vector<stl::string>;
        using T_LineDataPtr = stl::shared_ptr<T_LineData>;

      private:
        stl::ifstream fileReadStream;
        T_LineDataPtr lineData;

        T_LineData readFile() {
            stl::string fileContents;
            fileReadStream.seekg(0, stl::ios::end);
            fileContents.resize(static_cast<stl::size_t>(fileReadStream.tellg()));
            fileReadStream.seekg(0, stl::ios::beg);
            stl::size_t fileSize = fileContents.size();
            fileReadStream.read(&fileContents[0], fileSize);
            fileReadStream.close();
            T_LineData output;
            if (fileSize == 0) {
                return output;
            }
            stl::string buffer;
            buffer.reserve(50);
            for (stl::size_t i = 0; i < fileSize; ++i) {
                char& c = fileContents[i];
                if (c == '\n') {
                    output.emplace_back(buffer);
                    buffer.clear();
                    continue;
                }
                if (c != '\0' && c != '\r') {
                    buffer += c;
                }
            }
            output.emplace_back(buffer);
            return output;
        }

      public:
        ini_reader(stl::string const& filename, bool keepLineData = false) {
            fileReadStream.open(filename, stl::ios::in | stl::ios::binary);
            if (keepLineData) {
                lineData = stl::make_shared<T_LineData>();
            }
        }

        bool operator>>(INIStructure& data) {
            if (!fileReadStream.is_open()) {
                return false;
            }
            T_LineData                fileLines = readFile();
            stl::string               section;
            bool                      inSection = false;
            ini_parser::T_ParseValues parseData;
            for (auto const& line : fileLines) {
                auto parseResult = ini_parser::parseLine(line, parseData);
                if (parseResult == ini_parser::PDataType::PDATA_SECTION) {
                    inSection = true;
                    data[section = parseData.first];
                } else if (inSection && parseResult == ini_parser::PDataType::PDATA_KEYVALUE) {
                    auto const& key    = parseData.first;
                    auto const& value  = parseData.second;
                    data[section][key] = value;
                }
                if (lineData && parseResult != ini_parser::PDataType::PDATA_UNKNOWN) {
                    if (parseResult == ini_parser::PDataType::PDATA_KEYVALUE && !inSection) {
                        continue;
                    }
                    lineData->emplace_back(line);
                }
            }
            return true;
        }

        T_LineDataPtr getLines() {
            return lineData;
        }
    };

    class ini_generator {
      private:
        stl::ofstream fileWriteStream;

      public:
        bool prettyPrint = false;

        ini_generator(stl::string const& filename) {
            fileWriteStream.open(filename, stl::ios::out | stl::ios::binary);
        }

        bool operator<<(INIStructure const& data) {
            if (!fileWriteStream.is_open()) {
                return false;
            }
            if (!data.size()) {
                return true;
            }
            auto it = data.begin();
            for (;;) {
                auto const& section    = it->first;
                auto const& collection = it->second;
                fileWriteStream << "[" << section << "]";
                if (collection.size()) {
                    fileWriteStream << details::endl;
                    auto it2 = collection.begin();
                    for (;;) {
                        auto key = it2->first;
                        details::replace(key, "=", "\\=");
                        auto value = it2->second;
                        ascii::trim(value);
                        fileWriteStream << key << ((prettyPrint) ? " = " : "=") << value;
                        if (++it2 == collection.end()) {
                            break;
                        }
                        fileWriteStream << details::endl;
                    }
                }
                if (++it == data.end()) {
                    break;
                }
                fileWriteStream << details::endl;
                if (prettyPrint) {
                    fileWriteStream << details::endl;
                }
            }
            return true;
        }
    };

    class ini_writer {
      private:
        using T_LineData    = stl::vector<stl::string>;
        using T_LineDataPtr = stl::shared_ptr<T_LineData>;

        stl::string filename;

        T_LineData getLazyOutput(T_LineDataPtr const& lineData, INIStructure& data, INIStructure& original) {
            T_LineData                output;
            ini_parser::T_ParseValues parseData;
            stl::string               sectionCurrent;
            bool                      parsingSection        = false;
            bool                      continueToNextSection = false;
            bool                      discardNextEmpty      = false;
            bool                      writeNewKeys          = false;
            stl::size_t               lastKeyLine           = 0;
            for (auto line = lineData->begin(); line != lineData->end(); ++line) {
                if (!writeNewKeys) {
                    auto parseResult = ini_parser::parseLine(*line, parseData);
                    if (parseResult == ini_parser::PDataType::PDATA_SECTION) {
                        if (parsingSection) {
                            writeNewKeys   = true;
                            parsingSection = false;
                            --line;
                            continue;
                        }
                        sectionCurrent = parseData.first;
                        if (data.has(sectionCurrent)) {
                            parsingSection        = true;
                            continueToNextSection = false;
                            discardNextEmpty      = false;
                            output.emplace_back(*line);
                            lastKeyLine = output.size();
                        } else {
                            continueToNextSection = true;
                            discardNextEmpty      = true;
                            continue;
                        }
                    } else if (parseResult == ini_parser::PDataType::PDATA_KEYVALUE) {
                        if (continueToNextSection) {
                            continue;
                        }
                        if (data.has(sectionCurrent)) {
                            auto&       collection = data[sectionCurrent];
                            auto const& key        = parseData.first;
                            auto const& value      = parseData.second;
                            if (collection.has(key)) {
                                auto outputValue = collection[key];
                                if (value == outputValue) {
                                    output.emplace_back(*line);
                                } else {
                                    ascii::trim(outputValue);
                                    auto lineNorm = *line;
                                    details::replace(lineNorm, "\\=", "  ");
                                    auto equalsAt = lineNorm.find_first_of('=');
                                    auto valueAt =
                                      lineNorm.find_first_not_of(details::whitespaceDelimiters, equalsAt + 1);
                                    stl::string outputLine = line->substr(0, valueAt);
                                    if (prettyPrint && equalsAt + 1 == valueAt) {
                                        outputLine += " ";
                                    }
                                    outputLine += outputValue;
                                    output.emplace_back(outputLine);
                                }
                                lastKeyLine = output.size();
                            }
                        }
                    } else {
                        if (discardNextEmpty && line->empty()) {
                            discardNextEmpty = false;
                        } else if (parseResult != ini_parser::PDataType::PDATA_UNKNOWN) {
                            output.emplace_back(*line);
                        }
                    }
                }
                if (writeNewKeys || stl::next(line) == lineData->end()) {
                    T_LineData linesToAdd;
                    if (data.has(sectionCurrent) && original.has(sectionCurrent)) {
                        auto const& collection         = data[sectionCurrent];
                        auto const& collectionOriginal = original[sectionCurrent];
                        for (auto const& it : collection) {
                            auto key = it.first;
                            if (collectionOriginal.has(key)) {
                                continue;
                            }
                            auto value = it.second;
                            details::replace(key, "=", "\\=");
                            ascii::trim(value);
                            linesToAdd.emplace_back(key + ((prettyPrint) ? " = " : "=") + value);
                        }
                    }
                    if (!linesToAdd.empty()) {
                        output.insert(output.begin() + lastKeyLine, linesToAdd.begin(), linesToAdd.end());
                    }
                    if (writeNewKeys) {
                        writeNewKeys = false;
                        --line;
                    }
                }
            }
            for (auto const& it : data) {
                auto const& section = it.first;
                if (original.has(section)) {
                    continue;
                }
                if (prettyPrint && output.size() > 0 && !output.back().empty()) {
                    output.emplace_back();
                }
                output.emplace_back("[" + section + "]");
                auto const& collection = it.second;
                for (auto const& it2 : collection) {
                    auto key   = it2.first;
                    auto value = it2.second;
                    details::replace(key, "=", "\\=");
                    ascii::trim(value);
                    output.emplace_back(key + ((prettyPrint) ? " = " : "=") + value);
                }
            }
            return output;
        }

      public:
        bool prettyPrint = false;

        ini_writer(stl::string const& filename) : filename(filename) {}

        bool operator<<(INIStructure& data) {
            struct stat buf;
            bool        fileExists = (stat(filename.c_str(), &buf) == 0);
            if (!fileExists) {
                ini_generator generator(filename);
                generator.prettyPrint = prettyPrint;
                return generator << data;
            }
            INIStructure  originalData;
            T_LineDataPtr lineData;
            bool          readSuccess = false;
            {
                ini_reader reader(filename, true);
                if ((readSuccess = reader >> originalData)) {
                    lineData = reader.getLines();
                }
            }
            if (!readSuccess) {
                return false;
            }
            T_LineData    output = getLazyOutput(lineData, data, originalData);
            stl::ofstream fileWriteStream(filename, stl::ios::out | stl::ios::binary);
            if (fileWriteStream.is_open()) {
                if (output.size()) {
                    auto line = output.begin();
                    for (;;) {
                        fileWriteStream << *line;
                        if (++line == output.end()) {
                            break;
                        }
                        fileWriteStream << details::endl;
                    }
                }
                return true;
            }
            return false;
        }
    };

    class ini_file {
      private:
        stl::string filename;

      public:
        ini_file(stl::string const& filename) : filename(filename) {}

        bool read(INIStructure& data) const {
            if (data.size()) {
                data.clear();
            }
            if (filename.empty()) {
                return false;
            }
            ini_reader reader(filename);
            return reader >> data;
        }

        bool generate(INIStructure const& data, bool pretty = false) const {
            if (filename.empty()) {
                return false;
            }
            ini_generator generator(filename);
            generator.prettyPrint = pretty;
            return generator << data;
        }

        bool write(INIStructure& data, bool pretty = false) const {
            if (filename.empty()) {
                return false;
            }
            ini_writer writer(filename);
            writer.prettyPrint = pretty;
            return writer << data;
        }
    };


} // namespace webpp

#endif // WEBPP_INI_INI_H_
