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

#ifndef GLOBAL_FWD_H
#define GLOBAL_FWD_H

#include <cstdint>
#include <memory>

struct Value;
struct ClassInfo;
struct ObjectContext;

uint64_t registerClass(const std::string& name, std::shared_ptr<ClassInfo> classInfo, uint64_t scopeId = 0);
std::shared_ptr<ClassInfo> getClass(uint64_t classId);
std::shared_ptr<ClassInfo> getClassByName(const std::string& name, uint64_t scopeId = 0);
uint64_t createObject(std::shared_ptr<ObjectContext> context, uint64_t classId = 0);
std::shared_ptr<ObjectContext> getObject(uint64_t objectId);
uint64_t getObjectClassId(uint64_t objectId);
std::shared_ptr<ClassInfo> getObjectClass(uint64_t objectId);
bool isInstanceOf_(uint64_t objectId, uint64_t classId);
void destroyObject(uint64_t objectId);

#endif
