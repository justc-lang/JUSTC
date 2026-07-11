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

struct ClassInfo;
struct ObjectContext;

#ifdef __EMSCRIPTEN__
    class GlobalContext {
    private:
        // variables
        std::unordered_map<std::string, Value> m_variables;
        std::unordered_map<std::string, bool> m_constVars;
        std::unordered_map<std::string, bool> m_JUSTCVars;
        uint64_t m_rootCounter = 0;

        // classes, objects
        std::unordered_map<uint64_t, std::shared_ptr<ClassInfo>> m_classes;
        std::unordered_map<uint64_t, std::shared_ptr<ObjectContext>> m_objects;
        uint64_t m_nextClassId = 1;
        uint64_t m_nextObjectId = 1;
        std::unordered_map<uint64_t, uint64_t> m_objectToClass;
        std::unordered_map<uint64_t, std::unordered_map<std::string, uint64_t>> m_scopeClassNames;
        uint64_t m_currentScopeId = 0;
        
        bool isInstanceOfWithParent(uint64_t objectId, uint64_t targetClassId) const {
            auto classInfo = getClass(getObjectClassId(objectId));
            if (classInfo && classInfo->parentClass.lock()) {
                auto parent = classInfo->parentClass.lock();
                if (parent->id == targetClassId) return true;
                return isInstanceOfWithParent(parent->id, targetClassId);
            }
            return false;
        }

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

        uint64_t registerClass(const std::string& name, std::shared_ptr<ClassInfo> classInfo, uint64_t scopeId = 0) {
            uint64_t classId = m_nextClassId++;
            m_classes[classId] = classInfo;
            classInfo->id = classId;
            classInfo->name = name;
            
            if (scopeId > 0) {
                m_scopeClassNames[scopeId][name] = classId;
            }
            m_scopeClassNames[0][name] = classId;
            
            return classId;
        }
        
        std::shared_ptr<ClassInfo> getClass(uint64_t classId) const {
            auto it = m_classes.find(classId);
            if (it != m_classes.end()) {
                return it->second;
            }
            return nullptr;
        }
        
        std::shared_ptr<ClassInfo> getClassByName(const std::string& name, uint64_t scopeId = 0) const {
            auto scopeIt = m_scopeClassNames.find(scopeId);
            if (scopeIt != m_scopeClassNames.end()) {
                auto nameIt = scopeIt->second.find(name);
                if (nameIt != scopeIt->second.end()) {
                    return getClass(nameIt->second);
                }
            }
            auto globalIt = m_scopeClassNames.find(0);
            if (globalIt != m_scopeClassNames.end()) {
                auto nameIt = globalIt->second.find(name);
                if (nameIt != globalIt->second.end()) {
                    return getClass(nameIt->second);
                }
            }
            return nullptr;
        }
        
        bool hasClassInScope(const std::string& name, uint64_t scopeId) const {
            auto scopeIt = m_scopeClassNames.find(scopeId);
            if (scopeIt != m_scopeClassNames.end()) {
                return scopeIt->second.find(name) != scopeIt->second.end();
            }
            return false;
        }
        
        void removeClassFromScope(const std::string& name, uint64_t scopeId) {
            auto scopeIt = m_scopeClassNames.find(scopeId);
            if (scopeIt != m_scopeClassNames.end()) {
                scopeIt->second.erase(name);
            }
        }
        
        uint64_t createObject(std::shared_ptr<ObjectContext> context, uint64_t classId = 0) {
            uint64_t objectId = m_nextObjectId++;
            m_objects[objectId] = context;
            context->id = objectId;
            if (classId > 0) {
                m_objectToClass[objectId] = classId;
                context->classId = classId;
            }
            return objectId;
        }
        
        std::shared_ptr<ObjectContext> getObject(uint64_t objectId) const {
            auto it = m_objects.find(objectId);
            if (it != m_objects.end()) {
                return it->second;
            }
            return nullptr;
        }
        
        uint64_t getObjectClassId(uint64_t objectId) const {
            auto it = m_objectToClass.find(objectId);
            if (it != m_objectToClass.end()) {
                return it->second;
            }
            return 0;
        }
        
        std::shared_ptr<ClassInfo> getObjectClass(uint64_t objectId) const {
            uint64_t classId = getObjectClassId(objectId);
            if (classId > 0) {
                return getClass(classId);
            }
            return nullptr;
        }
        
        bool isInstanceOf(uint64_t objectId, uint64_t classId) const {
            uint64_t objClassId = getObjectClassId(objectId);
            if (objClassId == classId) return true;
            
            auto classInfo = getClass(objClassId);
            if (classInfo && classInfo->parentClass.lock()) {
                auto parent = classInfo->parentClass.lock();
                if (parent->id == classId) return true;
                return isInstanceOfWithParent(objClassId, classId);
            }
            return false;
        }
        
        void destroyObject(uint64_t objectId) {
            auto obj = getObject(objectId);
            if (obj && obj->classInfo) {
                if (obj->classInfo->destructor) {
                    Value self;
                    self.type = DataType::JUSTC_OBJECT;
                    self.object_context = obj;
                    obj->classInfo->destructor({self});
                }
            }
            m_objects.erase(objectId);
            m_objectToClass.erase(objectId);
        }
        
        void setCurrentScope(uint64_t scopeId) {
            m_currentScopeId = scopeId;
        }
        
        uint64_t getCurrentScope() const {
            return m_currentScopeId;
        }
    };
#else
    #include <shared_mutex>

    class GlobalContext {
    private:
        mutable std::shared_mutex m_mutex;

        // variables
        std::unordered_map<std::string, Value> m_variables;
        std::unordered_map<std::string, bool> m_constVars;
        std::unordered_map<std::string, bool> m_JUSTCVars;
        uint64_t m_rootCounter = 0;

        // classes, objects
        std::unordered_map<uint64_t, std::shared_ptr<ClassInfo>> m_classes;
        std::unordered_map<uint64_t, std::shared_ptr<ObjectContext>> m_objects;
        uint64_t m_nextClassId = 1;
        uint64_t m_nextObjectId = 1;
        std::unordered_map<uint64_t, uint64_t> m_objectToClass;
        std::unordered_map<uint64_t, std::unordered_map<std::string, uint64_t>> m_scopeClassNames;
        uint64_t m_currentScopeId = 0;

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

        uint64_t registerClass(const std::string& name, std::shared_ptr<ClassInfo> classInfo, uint64_t scopeId = 0) {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            uint64_t classId = m_nextClassId++;
            m_classes[classId] = classInfo;
            classInfo->id = classId;
            classInfo->name = name;
            
            if (scopeId > 0) {
                m_scopeClassNames[scopeId][name] = classId;
            }
            m_scopeClassNames[0][name] = classId;
            
            return classId;
        }
        
        std::shared_ptr<ClassInfo> getClass(uint64_t classId) const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            auto it = m_classes.find(classId);
            if (it != m_classes.end()) {
                return it->second;
            }
            return nullptr;
        }
        
        std::shared_ptr<ClassInfo> getClassByName(const std::string& name, uint64_t scopeId = 0) const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            auto scopeIt = m_scopeClassNames.find(scopeId);
            if (scopeIt != m_scopeClassNames.end()) {
                auto nameIt = scopeIt->second.find(name);
                if (nameIt != scopeIt->second.end()) {
                    return getClass(nameIt->second);
                }
            }
            auto globalIt = m_scopeClassNames.find(0);
            if (globalIt != m_scopeClassNames.end()) {
                auto nameIt = globalIt->second.find(name);
                if (nameIt != globalIt->second.end()) {
                    return getClass(nameIt->second);
                }
            }
            return nullptr;
        }
        
        uint64_t createObject(std::shared_ptr<ObjectContext> context, uint64_t classId = 0) {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            uint64_t objectId = m_nextObjectId++;
            m_objects[objectId] = context;
            context->id = objectId;
            if (classId > 0) {
                m_objectToClass[objectId] = classId;
                context->classId = classId;
            }
            return objectId;
        }
        
        std::shared_ptr<ObjectContext> getObject(uint64_t objectId) const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            auto it = m_objects.find(objectId);
            if (it != m_objects.end()) {
                return it->second;
            }
            return nullptr;
        }
        
        uint64_t getObjectClassId(uint64_t objectId) const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            auto it = m_objectToClass.find(objectId);
            if (it != m_objectToClass.end()) {
                return it->second;
            }
            return 0;
        }
        
        std::shared_ptr<ClassInfo> getObjectClass(uint64_t objectId) const {
            uint64_t classId = getObjectClassId(objectId);
            if (classId > 0) {
                return getClass(classId);
            }
            return nullptr;
        }
        
        bool isInstanceOf(uint64_t objectId, uint64_t classId) const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            uint64_t objClassId = getObjectClassId(objectId);
            if (objClassId == classId) return true;
            
            auto classInfo = getClass(objClassId);
            if (classInfo) {
                auto parent = classInfo->getParent();
                while (parent) {
                    if (parent->id == classId) return true;
                    parent = parent->getParent();
                }
            }
            return false;
        }
        
        void destroyObject(uint64_t objectId) {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            auto obj = getObject(objectId);
            if (obj && obj->classInfo) {
                if (!obj->classInfo->destructor.body.empty()) {
                    Value self;
                    self.type = DataType::JUSTC_OBJECT;
                    self.object_context = obj;
                }
            }
            m_objects.erase(objectId);
            m_objectToClass.erase(objectId);
        }
        
        void setCurrentScope(uint64_t scopeId) {
            std::unique_lock<std::shared_mutex> lock(m_mutex);
            m_currentScopeId = scopeId;
        }
        
        uint64_t getCurrentScope() const {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return m_currentScopeId;
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

inline uint64_t registerClass(const std::string& name, std::shared_ptr<ClassInfo> classInfo, uint64_t scopeId = 0) {
    return GlobalContext::getInstance().registerClass(name, classInfo, scopeId);
}

inline std::shared_ptr<ClassInfo> getClass(uint64_t classId) {
    return GlobalContext::getInstance().getClass(classId);
}

inline std::shared_ptr<ClassInfo> getClassByName(const std::string& name, uint64_t scopeId = 0) {
    return GlobalContext::getInstance().getClassByName(name, scopeId);
}

inline uint64_t createObject(std::shared_ptr<ObjectContext> context, uint64_t classId = 0) {
    return GlobalContext::getInstance().createObject(context, classId);
}

inline std::shared_ptr<ObjectContext> getObject(uint64_t objectId) {
    return GlobalContext::getInstance().getObject(objectId);
}

inline uint64_t getObjectClassId(uint64_t objectId) {
    return GlobalContext::getInstance().getObjectClassId(objectId);
}

inline std::shared_ptr<ClassInfo> getObjectClass(uint64_t objectId) {
    return GlobalContext::getInstance().getObjectClass(objectId);
}

inline bool isInstanceOf_(uint64_t objectId, uint64_t classId) {
    return GlobalContext::getInstance().isInstanceOf(objectId, classId);
}

inline void destroyObject(uint64_t objectId) {
    GlobalContext::getInstance().destroyObject(objectId);
}

#endif
