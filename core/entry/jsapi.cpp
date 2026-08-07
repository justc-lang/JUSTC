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

#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include "../lexer.h"
#include "../parser.h"
#include "../json.hpp"
#include "../fetch.h"
#include "../version.h"
#include <tuple>
#include "../justo.hpp"
#include <functional>
#include <mutex>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include "../compiler/justb.hpp"
#include "../loader/justb.hpp"
#include <cstdint>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

std::string outputString(const std::string& mode, const ParseResult& result) {
    if (mode == "xml") {
        return XmlSerializer::serialize(result);
    } else if (mode == "yaml") {
        return YamlSerializer::serialize(result);
    } else if (mode == "justo") {
        return JUSTOSerializer::serialize(result);
    } else if (mode == "justb") {
        if (!result.error.empty()) {
            return "{\"error\":\"" + JsonSerializer::escapeJsonString(result.error) + "\"}";
        } else {
            std::stringstream ss;
            JustbCompiler::compile(result, ss);
            
            std::stringstream json;
            json << "{";

            json << "\"type\":\"justb\",\"return\":\"" << JsonSerializer::escapeJsonString(ss.str()) << "\",";
            json << "\"logs\":" << JsonSerializer::serialize(result.logs) << ",";

            // logfile object
            json << "\"logfile\":{";
            json << "\"file\":\"" << JsonSerializer::escapeJsonString(result.logFilePath) << "\",";
            json << "\"logs\":\"" << JsonSerializer::escapeJsonString(result.logFileContent) << "\"";
            json << "},";

            // import logs array
            json << "\"imported\":";
            json << JsonSerializer::serialize(result.importLogs);

            json << "}";
            return json.str();
        }
    } else {
        return JsonSerializer::serialize(result);
    }
}
std::string outputString(const std::string& mode, const std::vector<ParserToken>& tokens, const std::string& input) {
    if (mode == "xml") {
        return XmlSerializer::serialize(tokens, input);
    } else if (mode == "yaml") {
        return YamlSerializer::serialize(tokens, input);
    } else if (mode == "justo") {
        return JUSTOSerializer::serialize(tokens, input);
    } else {
        return JsonSerializer::serialize(tokens, input);
    }
}

static std::unique_ptr<Parser> globalParser = nullptr;
static std::mutex globalParserMutex;

static std::unordered_map<std::string, Value> justoPointers;
static std::mutex justoPointersMutex;

static std::vector<std::function<void(const std::string&, const Value&)>> varUpdateListeners;
static std::mutex varUpdateListenersMutex;

static std::unordered_map<std::string, int> jsFunctions;
static std::mutex jsFunctionsMutex;

void ensureGlobalParser() {
    if (!globalParser) {
        globalParser = std::make_unique<Parser>(
            std::vector<ParserToken>(), true, false, "", true, true,
            "global", "global", true, true, false, nullptr, CharType::GRAPHEME
        );
    }
}

void initializeJUSTOPointers() {
    std::lock_guard<std::mutex> lock(justoPointersMutex);

    Value nanVal;
    nanVal.type = DataType::NOT_A_NUMBER;
    nanVal.name = "NaN";
    justoPointers["nan"] = nanVal;

    Value infVal;
    infVal.type = DataType::INFINITE;
    infVal.name = "Infinity";
    justoPointers["inf"] = infVal;
}

Value justoToValue(const std::string& justo) {
    JUSTO::JUSTOParser parser;
    for (const auto& [key, value] : justoPointers) {
        parser.registerPointer(key, value);
    }
    return parser.parse(justo);
}

std::string argsToJUSTOArray(const std::vector<Value>& args) {
    std::string result = "a[";
    for (size_t i = 0; i < args.size(); i++) {
        if (i > 0) result += ",";
        result += JUSTO::valueToJUSTO(args[i]);
    }
    result += "]";
    return result;
}

void triggerVariableUpdate(const std::string& name, const Value& value) {
    std::lock_guard<std::mutex> lock(varUpdateListenersMutex);
    for (const auto& listener : varUpdateListeners) {
        try {
            listener(name, value);
        } catch (const std::exception& e) {}
    }
}

#ifdef __EMSCRIPTEN__
extern "C" {
    extern void jsCallFunction(const char* name, const char* argsJUSTO, char** resultJUSTO);
}
#endif

extern "C" {

char* lexer(const char* input, const char* outputMode) {
    if (input == nullptr) return nullptr;
    std::string mode(outputMode == nullptr || outputMode == "justb" ? "justo" : outputMode);

    try {
        auto parsed = Lexer::parse(input, true);
        std::string json = outputString(mode, parsed.second, parsed.first);
        return strdup(json.c_str());

    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"" + JsonSerializer::escapeJsonString(std::string(e.what())) + "\",\"lexer\":true}";
        return strdup(error.c_str());
    }
}

char* parser(const char* tokensJUSTO, const char* outputMode) {
    if (tokensJUSTO == nullptr) return nullptr;
    std::string mode(outputMode == nullptr ? "json" : outputMode);

    try {
        std::vector<ParserToken> parserTokens;
        std::string input = "";

        if (JUSTO_Parser::parseJUSTOTokens(tokensJUSTO, parserTokens, input)) {
            ParseResult result = Parser::parseTokens(parserTokens, false, false, input);
            std::string json = outputString(mode, result);
            return strdup(json.c_str());
        } else {
            std::string error = "{\"error\":\"Failed to parse JUSTO\"}";
            return strdup(error.c_str());
        }

    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"" + JsonSerializer::escapeJsonString(std::string(e.what())) + "\",\"parser\":true}";
        return strdup(error.c_str());
    }
}

char* parse(const char* input, const bool execute, const bool runAsync, const char* outputMode) {
    if (input == nullptr) return nullptr;
    std::string mode(outputMode == nullptr ? "json" : outputMode);

    try {
        auto lexerResult = Lexer::parse(input);
        ParseResult result = Parser::parseTokens(lexerResult.second, execute, runAsync, input);

        if (result.variables) {
            for (const auto& [key, value] : *result.variables) {
                if (globalParser && globalParser->hasGlobal(key)) {
                    Value oldVal = globalParser->getGlobal(key);
                    if (oldVal.toString() != value.toString()) {
                        globalParser->registerGlobal(key, value);
                        triggerVariableUpdate(key, value);
                    }
                }
            }
        }

        std::string json = outputString(mode, result);
        return strdup(json.c_str());

    } catch (const std::exception& e) {
        std::string error = "{\"error\":\"" + JsonSerializer::escapeJsonString(std::string(e.what())) + "\"}";
        return strdup(error.c_str());
    }
}

void free_string(char* str) {
    if (str != nullptr) {
        free(str);
    }
}

char* version() {
    return strdup(JUSTC_VERSION.c_str());
}

int registerGlobal(const char* name, const char* justoValue) {
    if (name == nullptr || justoValue == nullptr) return 0;

    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();

    try {
        Value val = justoToValue(std::string(justoValue));
        globalParser->registerGlobal(std::string(name), val);
        triggerVariableUpdate(std::string(name), val);
        return 1;
    } catch (const std::exception& e) {
        return 0;
    }
}

char* getGlobal(const char* name) {
    if (name == nullptr) return strdup(";");

    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();

    try {
        Value val = globalParser->getGlobal(std::string(name));
        std::string justo = JUSTO::valueToJUSTO(val);
        return strdup(justo.c_str());
    } catch (const std::exception& e) {
        return strdup(";");
    }
}

int hasGlobal(const char* name) {
    if (name == nullptr) return 0;

    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();

    return globalParser->hasGlobal(std::string(name)) ? 1 : 0;
}

int unregisterGlobal(const char* name) {
    if (name == nullptr) return 0;

    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();

    globalParser->unregisterGlobal(std::string(name));
    triggerVariableUpdate(std::string(name), Value::createNull());
    return 1;
}

void clearGlobals() {
    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();
    globalParser->clearGlobals();
}

int registerPointer(const char* name, const char* justoValue) {
    if (name == nullptr || justoValue == nullptr) return 0;

    std::lock_guard<std::mutex> lock(justoPointersMutex);

    try {
        JUSTO::JUSTOParser parser;
        for (const auto& [key, value] : justoPointers) {
            parser.registerPointer(key, value);
        }
        Value val = parser.parse(std::string(justoValue));
        justoPointers[std::string(name)] = val;
        return 1;
    } catch (const std::exception& e) {
        return 0;
    }
}

char* getPointer(const char* name) {
    if (name == nullptr) return strdup(";");

    std::lock_guard<std::mutex> lock(justoPointersMutex);

    auto it = justoPointers.find(std::string(name));
    if (it != justoPointers.end()) {
        std::string justo = JUSTO::valueToJUSTO(it->second);
        return strdup(justo.c_str());
    }
    return strdup(";");
}

int unregisterPointer(const char* name) {
    if (name == nullptr) return 0;

    std::lock_guard<std::mutex> lock(justoPointersMutex);
    justoPointers.erase(std::string(name));
    return 1;
}

void clearPointers() {
    std::lock_guard<std::mutex> lock(justoPointersMutex);
    justoPointers.clear();
    initializeJUSTOPointers();
}

int registerFunction(const char* name, int jsCallbackPtr, int isConst) {
    if (name == nullptr || jsCallbackPtr == 0) return 0;

    std::lock_guard<std::mutex> lock(globalParserMutex);
    std::lock_guard<std::mutex> lock2(jsFunctionsMutex);
    ensureGlobalParser();

    try {
        jsFunctions[std::string(name)] = jsCallbackPtr;

        globalParser->registerFunction(std::string(name),
            [name, jsCallbackPtr](const std::vector<Value>& args) -> Value {
                std::string argsJUSTO = argsToJUSTOArray(args);

                char* resultJUSTO = nullptr;

                #ifdef __EMSCRIPTEN__
                    using JSFunc = void (*)(const char*, const char*, char**);
                    JSFunc jsFunc = reinterpret_cast<JSFunc>(jsCallbackPtr);
                    jsFunc(name, argsJUSTO.c_str(), &resultJUSTO);
                #else
                    resultJUSTO = strdup(";");
                #endif

                Value result = justoToValue(std::string(resultJUSTO));
                free(resultJUSTO);

                return result;
            },
            isConst != 0
        );
        return 1;
    } catch (const std::exception& e) {
        return 0;
    }
}

int unregisterFunction(const char* name) {
    if (name == nullptr) return 0;

    std::lock_guard<std::mutex> lock(globalParserMutex);
    std::lock_guard<std::mutex> lock2(jsFunctionsMutex);
    ensureGlobalParser();

    globalParser->unregisterFunction(std::string(name));
    jsFunctions.erase(std::string(name));
    return 1;
}

void clearUserFunctions() {
    std::lock_guard<std::mutex> lock(globalParserMutex);
    std::lock_guard<std::mutex> lock2(jsFunctionsMutex);
    ensureGlobalParser();
    globalParser->clearUserFunctions();
    jsFunctions.clear();
}

int addVariableUpdateListener(void (*callback)(const char* name, const char* valueJUSTO)) {
    if (callback == nullptr) return 0;

    std::lock_guard<std::mutex> lock(varUpdateListenersMutex);

    varUpdateListeners.push_back([callback](const std::string& name, const Value& value) {
        std::string justo = JUSTO::valueToJUSTO(value);
        callback(name.c_str(), justo.c_str());
    });

    return 1;
}

void clearVariableUpdateListeners() {
    std::lock_guard<std::mutex> lock(varUpdateListenersMutex);
    varUpdateListeners.clear();
}

char* justoParse(const char* justoString) {
    if (justoString == nullptr) return strdup(";");

    try {
        Value val = justoToValue(std::string(justoString));
        std::string justo = JUSTO::valueToJUSTO(val);
        return strdup(justo.c_str());
    } catch (const std::exception& e) {
        return strdup(";");
    }
}

char* load(const unsigned char* bytes, size_t length, const char* outputMode) {
    if (bytes == nullptr) return nullptr;
    std::string mode(outputMode == nullptr ? "json" : outputMode);

    std::string data(reinterpret_cast<const char*>(bytes), length);
    std::stringstream ss(data);
    ParseResult result = JustbLoader::load(ss);

    std::string json = outputString(mode, result);
    return strdup(json.c_str());
}

int unregisterClass(const char* raw_classID_str) {
    if (raw_classID_str == nullptr) return 0;

    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();

    std::string classID_str = std::string(raw_classID_str);
    unsigned long long raw_classID = std::stoull(classID_str);
    uint64_t classID = raw_classID;

    globalParser->unregisterClass(classID);

    return 1;
}

void clearClasses() {
    std::lock_guard<std::mutex> lock(globalParserMutex);
    ensureGlobalParser();
    globalParser->clearClasses();
}

}

struct JUSTOInitializer {
    JUSTOInitializer() {
        initializeJUSTOPointers();
    }
} justoInitializer;
