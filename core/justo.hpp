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

/*

JUSTO - Just an Ultimate Site Tool Object notation language.

*/

#ifdef _WIN32
    #define NOMINMAX
    #undef INFINITE
    #undef NAN
    #undef ERROR
#endif

#ifndef JUSTO_HPP
#define JUSTO_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>
#include <cctype>
#include "parser.h"
#include "utility.h"

namespace JUSTO {

struct OParser {
    const std::string& input;
    size_t pos;

    OParser(const std::string& in) : input(in), pos(0) {}

    char peek() const {
        return pos < input.length() ? input[pos] : '\0';
    }

    char advance() {
        return pos < input.length() ? input[pos++] : '\0';
    }

    void skipWhitespace() {
        while (pos < input.length() && std::isspace(static_cast<unsigned char>(input[pos]))) {
            pos++;
        }
    }

    bool match(char c) {
        skipWhitespace();
        if (peek() == c) {
            advance();
            return true;
        }
        return false;
    }

    std::string parseString(char quote) {
        skipWhitespace();
        if (peek() != quote) return "";
        advance();

        std::string result;
        while (pos < input.length() && peek() != quote) {
            if (peek() == '\\') {
                advance();
                if (peek() == '\\') result += '\\';
                else if (peek() == '"') result += '"';
                else if (peek() == '\'') result += '\'';
                else result += '\\' + std::string(1, peek());
                advance();
            } else {
                result += peek();
                advance();
            }
        }
        if (peek() == quote) advance();
        return result;
    }

    std::string parseIdentifier() {
        skipWhitespace();
        std::string result;
        if (peek() == '"') return parseString('"');
        while (pos < input.length() && (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')) {
            result += peek();
            advance();
        }
        return result;
    }

    double parseNumber() {
        skipWhitespace();
        std::string numStr;
        bool hasDot = false;

        if (peek() == '-') {
            numStr += '-';
            advance();
        }

        while (pos < input.length() && (std::isdigit(static_cast<unsigned char>(peek())) || peek() == '.')) {
            if (peek() == '.') {
                if (hasDot) break;
                hasDot = true;
            }
            numStr += peek();
            advance();
        }

        return std::stod(numStr);
    }

    bool parseBoolean() {
        skipWhitespace();
        if (peek() == '1') {
            advance();
            return true;
        } else if (peek() == '0') {
            advance();
            return false;
        }
        return false;
    }
};

class JUSTOParser {
private:
    std::unordered_map<std::string, Value> pointers;

    void registerBuiltinPointers() {
        Value nanVal;
        nanVal.type = DataType::NOT_A_NUMBER;
        nanVal.name = "NaN";
        pointers["nan"] = nanVal;

        Value infVal;
        infVal.type = DataType::INFINITE;
        infVal.name = "Infinity";
        pointers["inf"] = infVal;
    }

    Value parseValue(OParser& p) {
        p.skipWhitespace();
        char c = p.peek();

        if (c == 'n') { // number
            p.advance();
            return Value::createNumber(p.parseNumber());
        }
        else if (c == '"') { // string
            std::string str = p.parseString('"');
            return Value::createString(str);
        }
        else if (c == '\'') { // pointer
            std::string ptrName = p.parseString('\'');
            auto it = pointers.find(ptrName);
            if (it != pointers.end()) {
                return it->second;
            }
            return Value::createNull();
        }
        else if (c == '1' || c == '0') { // boolean
            bool val = p.parseBoolean();
            return Value::createBoolean(val);
        }
        else if (c == ';') {
            p.advance();
            return Value::createNull();
        }
        else if (c == 'o') { // object
            p.advance();
            return parseObject(p);
        }
        else if (c == 'a') { // array
            p.advance();
            return parseArray(p);
        }

        return Value::createNull();
    }

    Value parseObject(OParser& p) {
        if (!p.match('{')) {
            return Value::createNull();
        }

        std::unordered_map<std::string, Value> properties;

        while (!p.match('}')) {
            p.skipWhitespace();
            std::string key = p.parseIdentifier();
            if (key.empty()) break;

            if (!p.match(':')) break;

            Value val = parseValue(p);
            properties[key] = val;

            p.skipWhitespace();
            if (p.peek() == ';') {
                p.advance();
            }
        }

        return Value::createJsonObject(properties);
    }

    Value parseArray(OParser& p) {
        if (!p.match('[')) {
            return Value::createNull();
        }

        std::vector<Value> elements;

        while (!p.match(']')) {
            Value val = parseValue(p);
            elements.push_back(val);

            p.skipWhitespace();
            if (p.peek() == ',') {
                p.advance();
            }
        }

        return Value::createJsonArray(elements);
    }

public:
    JUSTOParser() {
        registerBuiltinPointers();
    }

    void registerPointer(const std::string& name, const Value& value) {
        pointers[name] = value;
    }

    Value parse(const std::string& justo) {
        OParser p(justo);
        return parseValue(p);
    }
};

inline std::string valueToJUSTO(const Value& value) {
    switch (value.type) {
        case DataType::NUMBER:
            return "n" + Utility::numberValue2string(value);
        case DataType::HEXADECIMAL:
            return "n" + std::to_string(static_cast<int>(value.number_value));
        case DataType::BINARY:
            return "n" + std::to_string(static_cast<int>(value.number_value));
        case DataType::OCTAL:
            return "n" + std::to_string(static_cast<int>(value.number_value));
        case DataType::STRING: {
            std::string escaped = value.string_value;
            size_t pos = 0;
            while ((pos = escaped.find('\\', pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\\\");
                pos += 2;
            }
            pos = 0;
            while ((pos = escaped.find('"', pos)) != std::string::npos) {
                escaped.replace(pos, 1, "\\\"");
                pos += 2;
            }
            return "\"" + escaped + "\"";
        }
        case DataType::BOOLEAN:
            return value.boolean_value ? "1" : "0";
        case DataType::NULL_TYPE:
            return ";";
        case DataType::NOT_A_NUMBER:
            return "'nan'";
        case DataType::INFINITE:
            return "'inf'";
        case DataType::JSON_OBJECT: {
            std::string result = "o{";
            bool first = true;
            for (const auto& [key, val] : value.properties) {
                if (!first) result += ";";
                first = false;
                result += key + ":" + valueToJUSTO(val.value);
            }
            result += "}";
            return result;
        }
        case DataType::JSON_ARRAY: {
            std::string result = "a[";
            for (size_t i = 0; i < value.array_elements.size(); i++) {
                if (i > 0) result += ",";
                result += valueToJUSTO(value.array_elements[i]);
            }
            result += "]";
            return result;
        }
        default:
            return ";";
    }
}

}

#endif
