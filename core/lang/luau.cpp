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

#ifndef JUSTC_NOLUAU

#include "luau.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cstring>
#include <utility>
#include <sstream>

#include "lua.h"
#include "lualib.h"
#include "luacode.h"

class LuaStateManager {
private:
    lua_State* L;

public:
    LuaStateManager() {
        L = luaL_newstate();
        if (!L) {
            throw std::runtime_error("Failed to create Lua state");
        }

        luaL_openlibs(L);

        restrictEnvironment();
    }

    ~LuaStateManager() {
        if (L) {
            lua_close(L);
        }
    }

    lua_State* getState() {
        return L;
    }

private:
    void restrictEnvironment() {
        const char* dangerous_globals[] = {
            "dofile", "loadfile", "io", "os", "package", "debug",
            "collectgarbage", "getmetatable", "setmetatable", nullptr
        };

        for (int i = 0; dangerous_globals[i] != nullptr; i++) {
            lua_pushnil(L);
            lua_setglobal(L, dangerous_globals[i]);
        }

        lua_getglobal(L, "string");
        lua_setglobal(L, "string");

        lua_getglobal(L, "table");
        lua_setglobal(L, "table");

        lua_getglobal(L, "math");
        lua_setglobal(L, "math");

        lua_getglobal(L, "utf8");
        lua_setglobal(L, "utf8");
    }
};

void RunLuau::runScript(const std::string& code) {
    LuaStateManager luaManager;
    lua_State* L = luaManager.getState();

    if (code.empty()) {
        return;
    }

    size_t bytecodeSize = 0;
    char* bytecode = luau_compile(code.c_str(), code.size(), NULL, &bytecodeSize);
    if (!bytecode) {

        throw std::runtime_error("Failed to compile Luau code");
    }

    int result = luau_load(L, "justc_luau_code", bytecode, bytecodeSize, 0);
    free(bytecode);

    if (result != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        std::string errorMsg = error ? std::string(error) : "Unknown Luau compilation error";

        lua_pop(L, 1);
        throw std::runtime_error("Luau compilation error: " + errorMsg);
    }

    result = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (result != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        std::string errorMsg = error ? std::string(error) : "Unknown Luau runtime error";

        lua_pop(L, 1);
        throw std::runtime_error("Luau runtime error: " + errorMsg);
    }
}

static std::string tableToJSON(lua_State* L, int index) {
    std::string result = "{";
    bool first = true;

    int baseIndex = (index < 0) ? lua_gettop(L) + index + 1 : index;

    lua_pushnil(L);
    while (lua_next(L, baseIndex) != 0) {
        if (!first) result += ",";
        first = false;

        int keyType = lua_type(L, -2);
        if (keyType == LUA_TSTRING) {
            const char* key = lua_tostring(L, -2);
            result += "\"" + std::string(key) + "\":";
        } else if (keyType == LUA_TNUMBER) {
            double keyNum = lua_tonumber(L, -2);
            result += std::to_string(keyNum) + ":";
        } else {
            result += "\"key\":";
        }

        int valType = lua_type(L, -1);
        switch (valType) {
            case LUA_TSTRING: {
                const char* val = lua_tostring(L, -1);
                std::string escaped;
                for (const char* p = val; *p; p++) {
                    if (*p == '"') escaped += "\\\"";
                    else if (*p == '\\') escaped += "\\\\";
                    else if (*p == '\n') escaped += "\\n";
                    else if (*p == '\r') escaped += "\\r";
                    else if (*p == '\t') escaped += "\\t";
                    else escaped += *p;
                }
                result += "\"" + escaped + "\"";
                break;
            }
            case LUA_TNUMBER:
                result += std::to_string(lua_tonumber(L, -1));
                break;
            case LUA_TBOOLEAN:
                result += lua_toboolean(L, -1) ? "true" : "false";
                break;
            case LUA_TNIL:
                result += "null";
                break;
            case LUA_TTABLE:
                result += tableToJSON(L, lua_gettop(L));
                break;
            default:
                result += "\"" + std::string(lua_typename(L, valType)) + "\"";
                break;
        }

        lua_pop(L, 1);
    }

    result += "}";
    return result;
}

static std::string arrayToJSON(lua_State* L, int index) {
    std::string result = "[";
    bool first = true;

    int baseIndex = (index < 0) ? lua_gettop(L) + index + 1 : index;
    int len = lua_objlen(L, baseIndex);

    for (int i = 1; i <= len; i++) {
        if (!first) result += ",";
        first = false;

        lua_pushnumber(L, i);
        lua_gettable(L, baseIndex);

        int valType = lua_type(L, -1);
        switch (valType) {
            case LUA_TSTRING: {
                const char* val = lua_tostring(L, -1);
                std::string escaped;
                for (const char* p = val; *p; p++) {
                    if (*p == '"') escaped += "\\\"";
                    else if (*p == '\\') escaped += "\\\\";
                    else if (*p == '\n') escaped += "\\n";
                    else if (*p == '\r') escaped += "\\r";
                    else if (*p == '\t') escaped += "\\t";
                    else escaped += *p;
                }
                result += "\"" + escaped + "\"";
                break;
            }
            case LUA_TNUMBER:
                result += std::to_string(lua_tonumber(L, -1));
                break;
            case LUA_TBOOLEAN:
                result += lua_toboolean(L, -1) ? "true" : "false";
                break;
            case LUA_TNIL:
                result += "null";
                break;
            case LUA_TTABLE:
                if (lua_objlen(L, -1) > 0) {
                    result += arrayToJSON(L, lua_gettop(L));
                } else {
                    result += tableToJSON(L, lua_gettop(L));
                }
                break;
            default:
                result += "\"" + std::string(lua_typename(L, valType)) + "\"";
                break;
        }

        lua_pop(L, 1);
    }

    result += "]";
    return result;
}

std::pair<std::string, int> RunLuau::runScriptWithResult(const std::string& code) {
    LuaStateManager luaManager;
    lua_State* L = luaManager.getState();

    size_t bytecodeSize = 0;
    char* bytecode = luau_compile(code.c_str(), code.size(), NULL, &bytecodeSize);
    if (!bytecode) {
        throw std::runtime_error("Failed to compile Luau code");
    }

    int result = luau_load(L, "user_code", bytecode, bytecodeSize, 0);
    free(bytecode);

    if (result != 0) {
        const char* error = lua_tostring(L, -1);
        throw std::runtime_error(std::string("Luau compilation error: ") + (error ? error : "Unknown Luau compilation error"));
    }

    result = lua_pcall(L, 0, 1, 0);
    if (result != LUA_OK) {
        const char* error = lua_tostring(L, -1);
        throw std::runtime_error(std::string("Luau runtime error: ") + (error ? error : "Unknown Luau runtime error"));
    }

    std::string output;
    int outputtype = 0; // 0 = string; 1 = number; 2 = boolean; 3 = nil; 4 = object; 5 = array

    int type = lua_type(L, -1);

    switch (type) {
        case LUA_TNUMBER:
            output = std::to_string(lua_tonumber(L, -1));
            outputtype = 1;
            break;

        case LUA_TBOOLEAN:
            output = lua_toboolean(L, -1) ? "true" : "false";
            outputtype = 2;
            break;

        case LUA_TNIL:
            output = "null";
            outputtype = 3;
            break;

        case LUA_TSTRING:
            output = lua_tostring(L, -1);
            outputtype = 0;
            break;

        case LUA_TTABLE: {
            int len = lua_objlen(L, -1);
            if (len > 0) {
                output = arrayToJSON(L, -1);
                outputtype = 5; // array
            } else {
                output = tableToJSON(L, -1);
                outputtype = 4; // object
            }
            break;
        }

        case LUA_TFUNCTION:
            if (lua_iscfunction(L, -1)) {
                output = "[Luau C Function]";
            } else {
                output = "[Luau Function]";
            }
            outputtype = 0;
            break;

        case LUA_TTHREAD: {
            lua_State* thread = lua_tothread(L, -1);
            std::stringstream ss;

            int status = lua_status(thread);
            if (status == LUA_OK) {
                ss << "[Luau Thread: normal";
            } else if (status == LUA_YIELD) {
                ss << "[Luau Thread: suspended";
            } else if (status == LUA_ERRRUN) {
                ss << "[Luau Thread: runtime error";
            } else if (status == LUA_ERRMEM) {
                ss << "[Luau Thread: memory error";
            } else {
                ss << "[Luau Thread: unknown";
            }

            int top = lua_gettop(thread);
            ss << " (stack: " << top << " values)]";

            output = ss.str();
            outputtype = 0;
            break;
        }

        case LUA_TLIGHTUSERDATA: {
            void* ptr = lua_touserdata(L, -1);
            std::stringstream ss;
            ss << "[Luau LightUserData: 0x" << std::hex << reinterpret_cast<uintptr_t>(ptr) << "]";
            output = ss.str();
            outputtype = 0;
            break;
        }

        case LUA_TUSERDATA: {
            std::stringstream ss;
            ss << "[Luau UserData: ";

            if (lua_getmetatable(L, -1)) {
                lua_getfield(L, -1, "__name");
                if (lua_isstring(L, -1)) {
                    ss << lua_tostring(L, -1);
                    lua_pop(L, 1);
                } else {
                    ss << "anonymous";
                    lua_pop(L, 1);
                }
                lua_pop(L, 1);
            } else {
                ss << "anonymous";
            }

            void* ptr = lua_touserdata(L, -1);
            ss << " 0x" << std::hex << reinterpret_cast<uintptr_t>(ptr) << "]";

            output = ss.str();
            outputtype = 0;
            break;
        }

        default:
            output = "[Luau " + std::string(lua_typename(L, type)) + "]";
            outputtype = 0;
            break;
    }

    lua_pop(L, 1);
    return {output, outputtype};
}

bool RunLuau::compileScript(const std::string& code, std::string& error) {
    LuaStateManager luaManager;
    lua_State* L = luaManager.getState();

    size_t bytecodeSize = 0;
    char* bytecode = luau_compile(code.c_str(), code.size(), NULL, &bytecodeSize);

    if (!bytecode) {
        error = "Compilation failed";
        return false;
    }

    free(bytecode);
    return true;
}

#endif
