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

#ifndef HTTP_HPP
#define HTTP_HPP

#include <string>
#include <unordered_map>
#include "../../parser.h"

class HTTP {
    public:
        static Value GET(    const std::string& url, const std::unordered_map<std::string, std::string>& headers);
        static Value POST(   const std::string& url, const std::unordered_map<std::string, std::string>& headers, const std::string& body);
        static Value PUT(    const std::string& url, const std::unordered_map<std::string, std::string>& headers, const std::string& body);
        static Value PATCH(  const std::string& url, const std::unordered_map<std::string, std::string>& headers, const std::string& body);
        static Value DELETE_(const std::string& url, const std::unordered_map<std::string, std::string>& headers);
        static Value HEAD(   const std::string& url, const std::unordered_map<std::string, std::string>& headers);
        static Value OPTIONS(const std::string& url, const std::unordered_map<std::string, std::string>& headers);
//      static Value request(const std::string& url, const std::unordered_map<std::string, std::string>& headers, const std::string& body, const std::string& method);
};

#endif
