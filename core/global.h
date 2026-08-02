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

#ifndef GLOBAL_H
#define GLOBAL_H

#include <unordered_map>
#include <string>
#include <cstdint>
#include "parser.h"

#ifdef __EMSCRIPTEN__
    class GlobalContext {
    private:
        std::unordered_map<std::string, Value> m_variables;
        std::unordered_map<std::string, bool> m_constVars;
        std::unordered_map<std::string, bool> m_JUSTCVars;
        uint64_t m_rootCounter = 0;

    public:
        static GlobalContext& getInstance() {
            static GlobalContext instance;
            return instance;
        }

        void set(const std::string& name, const Value& value, bool isConst = false, bool isJUSTC = false) {
            m_variables[name] = value;
            m_constVars[name] = isConst;
            m_JUSTCVars[name] = isJUSTC;
        }

        Value get(const std::string& name) const {
            auto it = m_variables.find(name);
            if (it != m_variables.end()) {
                return it->second;
            }
            return Value::createNull();
        }

        bool has(const std::string& name) const {
            return m_variables.find(name) != m_variables.end();
        }

        bool isConst(const std::string& name) const {
            auto it = m_constVars.find(name);
            return it != m_constVars.end() && it->second;
        }
        bool isJUSTC(const std::string& name) const {
            auto it = m_JUSTCVars.find(name);
            return it != m_JUSTCVars.end() && it->second;
        }

        void remove(const std::string& name) {
            m_variables.erase(name);
            m_constVars.erase(name);
            m_JUSTCVars.erase(name);
        }

        void clear() {
            m_variables.clear();
            m_constVars.clear();
            m_JUSTCVars.clear();
        }

        std::unordered_map<std::string, Value> getAll() const {
            return m_variables;
        }

        uint64_t getRootCounter() const {
            return m_rootCounter;
        }

        uint64_t incrementRootCounter() {
            return ++m_rootCounter;
        }
    };
#else
    #include <shared_mutex>

    class GlobalContext {
    private:
        mutable std::shared_mutex m_mutex;
        std::unordered_map<std::string, Value> m_variables;
        std::unordered_map<std::string, bool> m_constVars;
        std::unordered_map<std::string, bool> m_JUSTCVars;
        uint64_t m_rootCounter = 0;

    public:
        static GlobalContext& getInstance() {
            static GlobalContext instance;
            return instance;
        }

        void set(const std::string& name, const Value& value, bool isConst = false, bool isJUSTC = false) {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            m_variables[name] = value;
            m_constVars[name] = isConst;
            m_JUSTCVars[name] = isJUSTC;
        }

        Value get(const std::string& name) const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            auto it = m_variables.find(name);
            if (it != m_variables.end()) {
                return it->second;
            }
            return Value::createNull();
        }

        bool has(const std::string& name) const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return m_variables.find(name) != m_variables.end();
        }

        bool isConst(const std::string& name) const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            auto it = m_constVars.find(name);
            return it != m_constVars.end() && it->second;
        }
        bool isJUSTC(const std::string& name) const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            auto it = m_JUSTCVars.find(name);
            return it != m_JUSTCVars.end() && it->second;
        }

        void remove(const std::string& name) {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            m_variables.erase(name);
            m_constVars.erase(name);
            m_JUSTCVars.erase(name);
        }

        void clear() {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            m_variables.clear();
            m_constVars.clear();
            m_JUSTCVars.clear();
        }

        std::unordered_map<std::string, Value> getAll() const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return m_variables;
        }

        uint64_t getRootCounter() const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return m_rootCounter;
        }

        uint64_t incrementRootCounter() {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            return ++m_rootCounter;
        }
    };
#endif

inline void setGlobal(const std::string& name, const Value& value, bool isConst = false, bool isJUSTC = false) {
    GlobalContext::getInstance().set(name, value, isConst, isJUSTC);
}

inline Value getGlobal_(const std::string& name) {
    return GlobalContext::getInstance().get(name);
}

inline bool hasGlobal_(const std::string& name) {
    return GlobalContext::getInstance().has(name);
}

inline bool isGlobalConst(const std::string& name) {
    return GlobalContext::getInstance().isConst(name);
}
inline bool isGlobalJUSTC(const std::string& name) {
    return GlobalContext::getInstance().isJUSTC(name);
}

inline void removeGlobal(const std::string& name) {
    GlobalContext::getInstance().remove(name);
}

inline void clearGlobals_() {
    GlobalContext::getInstance().clear();
}

inline uint64_t getRootCounter() {
    return GlobalContext::getInstance().getRootCounter();
}

inline uint64_t incrementRootCounter() {
    return GlobalContext::getInstance().incrementRootCounter();
}

#endif
