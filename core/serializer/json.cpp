/*

MIT License

Copyright (c) 2025-2026 JustStudio. <https://juststudio.is-a.dev/>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "json.hpp"
#include <sstream>
#include <algorithm>
#include <iostream>
#include "../parser.h"
#include <cmath>
#include "../utility.h"
#include "../version.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

std::string JsonSerializer::escapeJsonString(const std::string& str) {
    std::stringstream ss;
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        switch (c) {
            case  '"': ss << "\\\""; break;
            case '\\': ss << "\\\\"; break;
            case '\b': ss << "\\b";  break;
            case '\f': ss << "\\f";  break;
            case '\n': ss << "\\n";  break;
            case '\r': ss << "\\r";  break;
            case '\t': ss << "\\t";  break;
            default:
                if (static_cast<unsigned char>(c) < 0x20 || static_cast<unsigned char>(c) == 0x7F) {
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                    ss << buf;
                } else {
                    ss << c;
                }
                break;
        }

        if (i > 100000) {
            #ifdef __EMSCRIPTEN__
            EM_ASM({
                console.warn("[JUSTC] (" + $0 + ") string is too long. It will be truncated in the JSON output.");
            }, Parser::getCurrentTimestamp().c_str());
            #else
            std::cout << "JUSTC: Warning: string is too long. It will be truncated in the JSON output." << std::endl;
            #endif
            break;
        }
    }
    return ss.str();
}

std::string JsonSerializer::valueToJson(const Value& value) {
    switch (value.type) {
        case DataType::JUSTC_OBJECT:
        case DataType::JSON_OBJECT: {
            std::stringstream ss;
            ss << "{";
            bool first = true;
            for (const auto& pair : value.properties) {
                if (!first) ss << ",";
                first = false;
                ss << "\"" << escapeJsonString(pair.first) << "\":"
                   << valueToJson(pair.second.value);
            }
            ss << "}";
            return ss.str();
        }
        case DataType::JSON_ARRAY: {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < value.array_elements.size(); i++) {
                if (i > 0) ss << ",";
                ss << valueToJson(value.array_elements[i]);
            }
            ss << "]";
            return ss.str();
        }
        case DataType::NUMBER:
        case DataType::HEXADECIMAL:
        case DataType::BINARY:
        case DataType::OCTAL:
            return Utility::numberValue2string(value);
        case DataType::STRING:
        case DataType::LINK:
        case DataType::PATH:
        case DataType::VARIABLE:
            return "\"" + escapeJsonString(value.string_value) + "\"";
        case DataType::BOOLEAN:
            return value.boolean_value ? "true" : "false";
        case DataType::NULL_TYPE:
            return "null";
        case DataType::NOT_A_NUMBER:
            return "\"NaN\"";
        case DataType::INFINITE:
            return "\"Infinity\"";
        case DataType::BINARY_DATA: {
            std::stringstream ss;
            ss << "[";
            for (size_t i = 0; i < value.binary_data.size(); i++) {
                unsigned char binchar = value.binary_data[i];
                double binnum = static_cast<double>(binchar);
                if (i > 0) ss << ",";
                ss << binnum;
            }
            ss << "]";
            return ss.str();
        }
        case DataType::INT8_ARRAY: {
            std::stringstream ss;
            ss << "[";
            std::vector<int8_t> arr = value.getComplexData<std::vector<int8_t>>();
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) ss << ",";
                ss << "\"" << +arr[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        case DataType::INT16_ARRAY: {
            std::stringstream ss;
            ss << "[";
            std::vector<int16_t> arr = value.getComplexData<std::vector<int16_t>>();
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) ss << ",";
                ss << "\"" << arr[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        case DataType::INT32_ARRAY: {
            std::stringstream ss;
            ss << "[";
            std::vector<int32_t> arr = value.getComplexData<std::vector<int32_t>>();
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) ss << ",";
                ss << "\"" << arr[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        case DataType::INT64_ARRAY: {
            std::stringstream ss;
            ss << "[";
            std::vector<int64_t> arr = value.getComplexData<std::vector<int64_t>>();
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) ss << ",";
                ss << "\"" << arr[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        case DataType::UINT8_ARRAY: case DataType::CUINT8_ARRAY: {
            std::stringstream ss;
            ss << "[";
            std::vector<uint8_t> arr = value.getComplexData<std::vector<uint8_t>>();
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) ss << ",";
                ss << "\"" << +arr[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        case DataType::UINT16_ARRAY: case DataType::CUINT16_ARRAY: {
            std::stringstream ss;
            ss << "[";
            std::vector<uint16_t> arr = value.getComplexData<std::vector<uint16_t>>();
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) ss << ",";
                ss << "\"" << arr[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        case DataType::UINT32_ARRAY: case DataType::CUINT32_ARRAY: {
            std::stringstream ss;
            ss << "[";
            std::vector<uint32_t> arr = value.getComplexData<std::vector<uint32_t>>();
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) ss << ",";
                ss << "\"" << arr[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        case DataType::UINT64_ARRAY: case DataType::CUINT64_ARRAY: {
            std::stringstream ss;
            ss << "[";
            std::vector<uint64_t> arr = value.getComplexData<std::vector<uint64_t>>();
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) ss << ",";
                ss << "\"" << arr[i] << "\"";
            }
            ss << "]";
            return ss.str();
        }
        case DataType::FLOAT32_ARRAY: {
            std::stringstream ss;
            ss << "[";
            std::vector<float> arr = value.getComplexData<std::vector<float>>();
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) ss << ",";
                std::stringstream numstr;
                numstr << std::setprecision(std::numeric_limits<float>::max_digits10) << arr[i];
                ss << "\"" << numstr.str() << "\"";
            }
            ss << "]";
            return ss.str();
        }
        case DataType::FLOAT64_ARRAY: {
            std::stringstream ss;
            ss << "[";
            std::vector<double> arr = value.getComplexData<std::vector<double>>();
            for (size_t i = 0; i < arr.size(); i++) {
                if (i > 0) ss << ",";
                std::stringstream numstr;
                numstr << std::setprecision(std::numeric_limits<double>::max_digits10) << arr[i];
                ss << "\"" << numstr.str() << "\"";
            }
            ss << "]";
            return ss.str();
        }
        default:
            return "\"" + escapeJsonString(value.toString()) + "\"";
    }
}

std::string JsonSerializer::tokensToJson(const std::vector<ParserToken>& tokens) {
    std::stringstream json;
    json << "[";

    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& token = tokens[i];

        json << "{";
        json << "\"type\":\"" << escapeJsonString(token.type) << "\",";
        json << "\"value\":\"" << escapeJsonString(token.value) << "\",";
        json << "\"start\":" << token.start;
        json << "}";

        if (i < tokens.size() - 1) {
            json << ",";
        }
    }

    json << "]";
    return json.str();
}

std::string JsonSerializer::serialize(const ParseResult& result) {
    std::stringstream json;

    #ifdef __EMSCRIPTEN__

    json << "{";
    if (!result.error.empty()) {
        json << "\"error\":\"" << escapeJsonString(result.error) << "\"";
    } else {
        // return values
        json << "\"type\":\"json\",\"return\":";
        bool first = true;
        bool doObj = false;
        if (result.array) {
            std::vector<int> indices;
            bool isArray = true;
            for (const auto& pair : result.returnValues) {
                try {
                    int idx = std::stoi(pair.first);
                    indices.push_back(idx);
                } catch (...) {
                    isArray = false;
                    break;
                }
            }

            if (isArray) {
                json << "[";
                std::sort(indices.begin(), indices.end());
                for (size_t i = 0; i < indices.size(); i++) {
                    if (i > 0) json << ",";
                    std::string key = std::to_string(indices[i]);
                    json << valueToJson(result.returnValues.at(key));
                }
                json << "],";
            } else {
                doObj = true;
            }
        } else {
            doObj = true;
        }

        if (doObj) {
            json << "{";
            for (const auto& pair : result.returnValues) {
                if (!first) json << ",";
                first = false;
                json << "\"" << escapeJsonString(pair.first) << "\":" << valueToJson(pair.second);
            }
            json << "},";
        }

        // logs array
        json << "\"logs\":";
        json << serialize(result.logs);
        json << ",";

        // logfile object
        json << "\"logfile\":{";
        json << "\"file\":\"" << escapeJsonString(result.logFilePath) << "\",";
        json << "\"logs\":\"" << escapeJsonString(result.logFileContent) << "\"";
        json << "},";

        // import logs array
        json << "\"imported\":";
        json << serialize(result.importLogs);
    }

    json << "}";

    #else

    bool first = true;
    bool doObj = false;

    if (result.array) {
        std::vector<int> indices;
        bool isArray = true;
        for (const auto& pair : result.returnValues) {
            try {
                int idx = std::stoi(pair.first);
                indices.push_back(idx);
            } catch (...) {
                isArray = false;
                break;
            }
        }

        if (isArray) {
            json << "[";
            std::sort(indices.begin(), indices.end());
            for (size_t i = 0; i < indices.size(); i++) {
                if (i > 0) json << ",";
                std::string key = std::to_string(indices[i]);
                json << valueToJson(result.returnValues.at(key));
            }
            json << "]";
        } else {
            doObj = true;
        }
    } else {
        doObj = true;
    }

    if (doObj) {
        json << "{";
        for (const auto& pair : result.returnValues) {
            if (!first) json << ",";
            first = false;
            json << "\"" << escapeJsonString(pair.first) << "\":" << valueToJson(pair.second);
        }
        json << "}";
    }

    #endif

    return json.str();
}

std::string JsonSerializer::serialize(const std::vector<ParserToken>& tokens, const std::string& input) {
    std::stringstream json;
    json << "{";
    json << "\"version\":\"" << escapeJsonString(JUSTC_VERSION) << "\",";
    json << "\"input\":\"" << escapeJsonString(input) << "\",";
    json << "\"tokens\":" << tokensToJson(tokens);
    json << "}";
    return json.str();
}

std::string JsonSerializer::serialize(const std::vector<LogEntry>& logs) {
    std::stringstream json;
    json << "[";

    for (size_t i = 0; i < logs.size(); i++) {
        const auto& log = logs[i];
        json << "{";
        json << "\"type\":\"" << escapeJsonString(log.type) << "\",";
        json << "\"message\":\"" << escapeJsonString(log.message) << "\",";
        json << "\"position\":" << log.position << ",";
        json << "\"time\":\"" << escapeJsonString(log.timestamp) << "\"";
        json << "}";

        if (i < logs.size() - 1) {
            json << ",";
        }
    }

    json << "]";
    return json.str();
}

std::string JsonSerializer::serialize(const std::vector<std::vector<std::string>>& importLogs) {
    std::stringstream json;
    json << "[";

    for (size_t i = 0; i < importLogs.size(); i++) {
        const auto& log = importLogs[i];

        json << "[";
        for (size_t j = 0; j < log.size(); j++) {
            const std::string importLog = log[j];
            json << "\"" + escapeJsonString(importLog) + "\"";
            if (j < log.size() - 1) {
                json << ",";
            }
        }
        json << "]";

        if (i < importLogs.size() - 1) {
            json << ",";
        }
    }

    json << "]";
    return json.str();
}
