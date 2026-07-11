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

#include "utility.h"
#include "parser.h"
#include <cmath>
#include <string>
#include <iomanip>
#include <bitset>
#include <cstring>
#include <sstream>
#include <cstddef>
#include <unordered_map>
#include <iostream>
#include <cstdint>
#ifdef __EMSCRIPTEN__
#include "utility.emscripten.h"
#endif

std::string Utility::numberValue2string(const Value& value) {
    if (static_cast<bool>(value.numeric_data)) {
        return value.toNumericString();
    } else if (value.number_value == std::floor(value.number_value)) {
        return std::to_string(static_cast<long long>(value.number_value));
    } else {
        return std::to_string(value.number_value);
    }
}

std::string Utility::value2string(const Value& value) {
    switch (value.type) {
        case DataType::NUMBER:
        case DataType::HEXADECIMAL:
        case DataType::BINARY:
        case DataType::OCTAL:
            return numberValue2string(value);
        case DataType::JUSTC_OBJECT:
            if (value.name == "HTTP.Responce") {
                auto text = value.object_value.find("text");
                if (text != value.object_value.end()) return value2string(text->second);
                else return value.toString();
            } else return value.toString();
        default:
            return value.toString();
    }
}

std::string Utility::double2hexString(const double d) {
    uint64_t bits;
    std::memcpy(&bits, &d, sizeof(double));
    std::stringstream ss;
    ss << std::hex << bits;
    return ss.str();
}

std::string Utility::double2octString(const double d) {
    uint64_t bits;
    std::memcpy(&bits, &d, sizeof(double));
    std::stringstream ss;
    ss << std::oct << bits;
    return ss.str();
}

std::string Utility::double2binString(const double d) {
    uint64_t bits;
    std::memcpy(&bits, &d, sizeof(double));
    return std::bitset<64>(bits).to_string();
}

bool Utility::checkNumber(const Value& val) {
    switch (val.type) {
        case DataType::NUMBER:
        case DataType::HEXADECIMAL:
        case DataType::BINARY:
        case DataType::OCTAL:
        case DataType::BIGNUM:
        case DataType::BASE64:
            return true;
        default:
            return false;
    }
}
bool Utility::checkNumbers(const Value& left, const Value& right) {
    return (checkNumber(left) && checkNumber(right));
}
bool Utility::checkObject(const Value& val) {
    switch (val.type) {
        case DataType::JSON_OBJECT:
        case DataType::JUSTC_OBJECT:
            return true;
        default:
            return false;
    }
}
bool Utility::checkObjects(const Value& left, const Value& right) {
    return (checkObject(left) && checkObject(right));
}
bool Utility::checkString(const Value& val) {
    switch (val.type) {
        case DataType::STRING:
        case DataType::UNKNOWN:
            return true;
        default:
            return false;
    }
}
bool Utility::checkStrings(const Value& left, const Value& right) {
    return (checkString(left) && checkString(right));
}

std::pair<size_t, size_t> Utility::pos(const size_t& pos, const std::string& script) {
    if (script.empty() || pos >= script.length()) {
        return {1, 1};
    }

    size_t line = 1;
    size_t column = 1;
    size_t current_pos = 0;

    while (current_pos < pos && current_pos < script.length()) {
        char current_char = script[current_pos];

        if (current_char == '\n') {                                                     //      \n
            line++;
            column = 1;
            current_pos++;
        } else if (current_char == '\r') {
            if (current_pos + 1 < script.length() && script[current_pos + 1] == '\n') { //      \r\n
                line++;
                column = 1;
                current_pos += 2;
            } else {                                                                    //      \r
                line++;
                column = 1;
                current_pos++;
            }
        } else {
            column++;
            current_pos++;
        }
    }

    return {line, column};
}

std::string Utility::position(const size_t& pos_, const std::string& script) {
    std::pair<size_t, size_t> position = pos(pos_, script);
    size_t line = position.first;
    size_t column = position.second;
    return "line " + std::to_string(line) + ", column " + std::to_string(column);
}

DataType Utility::typeDeclaration2dataType(const std::string& typeDeclaration, const std::string& position) {
    static const std::unordered_map<std::string, DataType> typeMap = {
        { "number",      DataType::NUMBER       },     { "num",  DataType::NUMBER       },
        { "string",      DataType::STRING       },     { "str",  DataType::STRING       },
        { "boolean",     DataType::BOOLEAN      },     { "bool", DataType::BOOLEAN      },
        { "null",        DataType::NULL_TYPE    },     { "nil",  DataType::NULL_TYPE    },
        { "link",        DataType::LINK         },
        { "path",        DataType::PATH         },
        { "binary",      DataType::BINARY       },     { "bin",  DataType::BINARY       },
        { "octal",       DataType::OCTAL        },     { "oct",  DataType::OCTAL        },
        { "hexadecimal", DataType::HEXADECIMAL  },     { "hex",  DataType::HEXADECIMAL  },
        { "object",      DataType::JUSTC_OBJECT },     { "obj",  DataType::JUSTC_OBJECT },
        { "json",        DataType::JSON_OBJECT  },
        { "array",       DataType::JSON_ARRAY   },
        { "nan",         DataType::NOT_A_NUMBER },
        { "infinity",    DataType::INFINITE     },     { "inf",  DataType::INFINITE     },
        { "data",        DataType::BINARY_DATA  },
        { "auto",        DataType::UNKNOWN      },
    };

    auto it = typeMap.find(typeDeclaration);
    if (it != typeMap.end()) {
        return it->second;
    }

    throw std::runtime_error("Invalid type declaration \"" + typeDeclaration + "\" at " + position + ".");
}

Value Utility::convert(const Value value, const DataType type) {
    Value result = value;
    result.type = type;
    switch (type) {
        case DataType::NUMBER:
            break;
        case DataType::BINARY:
            result.name = double2binString(value.number_value);
            break;
        case DataType::HEXADECIMAL:
            result.name = double2hexString(value.number_value);
            break;
        case DataType::OCTAL:
            result.name = double2octString(value.number_value);
            break;
        default: // warning: 15 enumeration values not handled in switch: 'UNKNOWN', 'JUSTC_OBJECT', 'STRING'... [-Wswitch]
            throw std::runtime_error("JUSTC/core/utility.cpp error: Incorrect usage.");
    }
    return result;
}

Value Utility::ParseResult2Value(const ParseResult parseresult) {
    Value result;
    result.type = DataType::JUSTC_OBJECT;
    result.object_value = parseresult.returnValues;
    result.name = "(Object)";
    return result;
}

bool Utility::isGitHubActions() {
    const char* githubActions = std::getenv("GITHUB_ACTIONS");
    return (githubActions && std::string(githubActions) == "true");
}

std::unordered_map<std::string, std::string> Utility::ParseHeaders(const std::string& headers) {
    std::unordered_map<std::string, std::string> output;
    std::istringstream lines_stream(headers);
    std::string line;
    while (std::getline(lines_stream, line, '\n')) {
        std::istringstream pair_stream(line);
        std::string key;
        std::string value;
        if (std::getline(pair_stream, key, ':')) {
            if (std::getline(pair_stream, value)) {
                output[key] = value;
            }
        }
    }
    return output;
}

std::string Utility::defaultHTTPAccept = "text/*, application/x-justc, application/json, application/lua, application/hocon, application/xml, application/yaml, */*";

void Utility::Warn(const std::string& warning) {
    #ifdef __EMSCRIPTEN__
    console_warn(Parser::getCurrentTimestamp().c_str(), warning.c_str());
    #else
    if (isGitHubActions()) {
        std::cout << "::warning::" + warning << std::endl;
    } else {
        std::cout << "JUSTC: Warning: " + warning << std::endl;
    }
    #endif
}

std::string Utility::escapeJUSTCString(const std::string& str) {
    return str; // TODO: 1. escape sequences in JUSTC; 2. this function
}

std::string Utility::_stringifyValue(const Value& value, int indentLevel) {
    std::string indent(indentLevel * 2, ' ');
    std::string nextIndent((indentLevel + 1) * 2, ' ');

    switch (value.type) {
        case DataType::NUMBER:
            return numberValue2string(value);

        case DataType::HEXADECIMAL:
            return "0x" + double2hexString(value.number_value);

        case DataType::BINARY:
            return "0b" + double2binString(value.number_value);

        case DataType::OCTAL:
            return "0o" + double2octString(value.number_value);

        case DataType::STRING:
            return "\"" + StringEscape::escape(value.string_value) + "\"";

        case DataType::LINK:
            return "<" + escapeJUSTCString(value.string_value) + ">";

        case DataType::PATH:
            return value.string_value;

        case DataType::BOOLEAN:
            return value.boolean_value ? "y" : "n";

        case DataType::NULL_TYPE:
            return "";

        case DataType::NOT_A_NUMBER:
            return "NaN";

        case DataType::INFINITE:
            return "Infinity";

        case DataType::JUSTC_OBJECT:
        case DataType::JSON_OBJECT: {
            std::string result = "{" + nextIndent;
            bool first = true;

            const auto& props = value.properties;
            for (const auto& [key, val] : props) {
                if (!first) result += "," + nextIndent;
                first = false;
                result += "\"" + escapeJUSTCString(key) + "\":" + _stringifyValue(val, indentLevel + 1);
            }

            if (value.object_context && !value.object_context->variables.empty()) {
                for (const auto& [key, val] : value.object_context->variables) {
                    if (props.find(key) != props.end()) continue;

                    if (!first) result += "," + nextIndent;
                    first = false;
                    result += "\"" + escapeJUSTCString(key) + "\":" + _stringifyValue(val, indentLevel + 1);
                }
            }

            result += indent + "}";
            return result;
        }

        case DataType::JSON_ARRAY: {
            std::string result = "[" + nextIndent;
            for (size_t i = 0; i < value.array_elements.size(); i++) {
                if (i > 0) result += "," + nextIndent;
                result += _stringifyValue(value.array_elements[i], indentLevel + 1);
            }
            result += indent + "]";
            return result;
        }

        case DataType::VARIABLE:
            return value.string_value;

        case DataType::FUNCTION: {
            if (value.native) {
                return value.name;
            }

            std::string result;
            if (value.function_info.isIsolated) {
                result = "isolated ";
            }
            result += "function " + value.name + "(";
            for (size_t i = 0; i < value.function_info.paramNames.size(); i++) {
                if (i > 0) result += ", ";
                result += value.function_info.paramNames[i];
            }
            result += "){" + nextIndent;
            result += value.string_value;
            result += indent + "}";
            return result;
        }

        case DataType::BINARY_DATA: {
            std::string binData;
            for (size_t i = 0; i < value.binary_data.size(); i++) {
                binData += std::to_string(static_cast<int>(value.binary_data[i]));
            }
            return "Binary::FromText(\"" + escapeJUSTCString(binData) + "\")";
        }

        default:
            return "nil";
    }
}

std::string Utility::stringifyValue(const Value& value) {
    return "return " + _stringifyValue(value, 0) + " .";
}

char StringEscape::hexToChar(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

std::string StringEscape::codepointToUTF8(uint32_t cp) {
    std::string result;
    if (cp < 0x80) {
        result += static_cast<char>(cp);
    } else if (cp < 0x800) {
        result += static_cast<char>(0xC0 | (cp >> 6));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp < 0x10000) {
        result += static_cast<char>(0xE0 | (cp >> 12));
        result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    } else {
        result += static_cast<char>(0xF0 | (cp >> 18));
        result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    }
    return result;
}

std::string StringEscape::unescape(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); i++) {
        char c = str[i];
        
        if (c == '\\' && i + 1 < str.length()) {
            char next = str[i + 1];
            i++;
            
            switch (next) {
                case 'n':  result += '\n'; break;
                case 'r':  result += '\r'; break;
                case 't':  result += '\t'; break;
                case 'v':  result += '\v'; break;
                case 'f':  result += '\f'; break;
                case 'b':  result += '\b'; break;
                case 'a':  result += '\a'; break;
                case '\\': result += '\\'; break;
                case '"':  result += '"';  break;
                case '\'': result += '\''; break;
                case '?':  result += '?';  break;
                
                case '0': case '1': case '2': case '3':
                case '4': case '5': case '6': case '7': {
                    std::string octal;
                    octal += next;
                    
                    for (int j = 0; j < 2 && i + 1 < str.length(); j++) {
                        char nextChar = str[i + 1];
                        if (nextChar >= '0' && nextChar <= '7') {
                            octal += nextChar;
                            i++;
                        } else {
                            break;
                        }
                    }
                    
                    int value = 0;
                    for (char oct : octal) {
                        value = (value << 3) + (oct - '0');
                    }
                    
                    if (value <= 0xFF) {
                        result += static_cast<char>(value);
                    } else {
                        result += codepointToUTF8(value);
                    }
                    break;
                }
                
                case 'u': {
                    if (i + 4 < str.length()) {
                        std::string hex = str.substr(i + 1, 4);
                        bool valid = true;
                        for (char h : hex) {
                            if (!((h >= '0' && h <= '9') || 
                                  (h >= 'a' && h <= 'f') || 
                                  (h >= 'A' && h <= 'F'))) {
                                valid = false;
                                break;
                            }
                        }
                        
                        if (valid) {
                            uint32_t cp = 0;
                            for (char h : hex) {
                                cp = (cp << 4) + hexToChar(h);
                            }
                            result += codepointToUTF8(cp);
                            i += 4;
                        } else {
                            result += next;
                        }
                    } else {
                        result += next;
                    }
                    break;
                }
                
                case 'U': {
                    if (i + 8 < str.length()) {
                        std::string hex = str.substr(i + 1, 8);
                        bool valid = true;
                        for (char h : hex) {
                            if (!((h >= '0' && h <= '9') || 
                                  (h >= 'a' && h <= 'f') || 
                                  (h >= 'A' && h <= 'F'))) {
                                valid = false;
                                break;
                            }
                        }
                        
                        if (valid) {
                            uint32_t cp = 0;
                            for (char h : hex) {
                                cp = (cp << 4) + hexToChar(h);
                            }
                            result += codepointToUTF8(cp);
                            i += 8;
                        } else {
                            result += next;
                        }
                    } else {
                        result += next;
                    }
                    break;
                }
                
                case 'x': {
                    if (i + 2 < str.length()) {
                        std::string hex = str.substr(i + 1, 2);
                        bool valid = true;
                        for (char h : hex) {
                            if (!((h >= '0' && h <= '9') || 
                                  (h >= 'a' && h <= 'f') || 
                                  (h >= 'A' && h <= 'F'))) {
                                valid = false;
                                break;
                            }
                        }
                        
                        if (valid) {
                            int value = (hexToChar(hex[0]) << 4) + hexToChar(hex[1]);
                            result += static_cast<char>(value);
                            i += 2;
                        } else {
                            result += next;
                        }
                    } else {
                        result += next;
                    }
                    break;
                }
                
                default:
                    result += next;
                    break;
            }
        } else {
            result += c;
        }
    }
    
    return result;
}

std::string StringEscape::escape(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2);
    
    for (char c : str) {
        switch (c) {
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\v': result += "\\v"; break;
            case '\f': result += "\\f"; break;
            case '\b': result += "\\b"; break;
            case '\a': result += "\\a"; break;
            case '\\': result += "\\\\"; break;
            case '"':  result += "\\\""; break;
            case '\'': result += "\\'"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[5];
                    snprintf(buf, sizeof(buf), "\\x%02x", static_cast<unsigned char>(c));
                    result += buf;
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

bool StringEscape::isValidEscape(const std::string& str) {
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '\\') {
            if (i + 1 >= str.length()) return false;
            
            char next = str[i + 1];
            std::string validEscapes = "nrtvfba\\\"\'?01234567uUx";
            if (validEscapes.find(next) == std::string::npos) {
                return false;
            }
        }
    }
    return true;
}

std::string Utility::doubleToString(double value) {
    std::ostringstream oss;
    oss << std::noshowpoint << value;
    return oss.str();
}

bool Utility::compareValues(const Value& left, const Value& right) {
    if (left.type != right.type && !checkObjects(left, right) && !checkStrings(left, right) && !checkNumbers(left, right)) return false;

    if (checkNumbers(left, right)) return left.number_value == right.number_value;
    if (checkStrings(left, right)) return left.toString() == right.toString();

    switch (left.type) {
        case DataType::NUMBER:
        case DataType::HEXADECIMAL:
        case DataType::BINARY:
        case DataType::OCTAL:
            return left.number_value == right.number_value;
        
        case DataType::STRING:
        case DataType::LINK:
        case DataType::PATH:
        case DataType::VARIABLE:
            return left.string_value == right.string_value;
        
        case DataType::BOOLEAN:
            return left.boolean_value == right.boolean_value;
        
        case DataType::NULL_TYPE:
            return true;

        case DataType::NOT_A_NUMBER:
        case DataType::INFINITE:
            return left.toNumber() == right.toNumber();

        case DataType::JUSTC_OBJECT:
        case DataType::JSON_OBJECT:
            if (left.properties.size() != right.properties.size()) return false;
            for (const auto& [key, val] : left.properties) {
                auto it = right.properties.find(key);
                if (it == right.properties.end()) return false;
                if (!compareValues(val, it->second)) return false;
            }
            return true;

        case DataType::JSON_ARRAY:
            if (left.array_elements.size() != right.array_elements.size()) return false;
            for (size_t i = 0; i < left.array_elements.size(); i++) {
                if (!compareValues(left.array_elements[i], right.array_elements[i])) return false;
            }
            return true;

        case DataType::BINARY_DATA:
            return left.binary_data == right.binary_data;

        case DataType::FUNCTION:
            return _stringifyValue(left) == _stringifyValue(right);

        default: return left.toBoolean() == right.toBoolean();
    }
}
