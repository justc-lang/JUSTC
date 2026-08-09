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

#include "http.hpp"
#include "../../fetch.h"
#include "../../parser.h"

Value HTTP::GET(const std::string& url, const std::unordered_map<std::string, std::string>& headers) {
    return Fetch::request(url, "GET", headers);
}
Value HTTP::POST(const std::string& url, const std::unordered_map<std::string, std::string>& headers, const std::string& body) {
    return Fetch::request(url, "POST", headers, body);
}
Value HTTP::PUT(const std::string& url, const std::unordered_map<std::string, std::string>& headers, const std::string& body) {
    return Fetch::request(url, "PUT", headers, body);
}
Value HTTP::PATCH(const std::string& url, const std::unordered_map<std::string, std::string>& headers, const std::string& body) {
    return Fetch::request(url, "PATCH", headers, body);
}
Value HTTP::DELETE_(const std::string& url, const std::unordered_map<std::string, std::string>& headers) {
    return Fetch::request(url, "DELETE", headers);
}
Value HTTP::HEAD(const std::string& url, const std::unordered_map<std::string, std::string>& headers) {
    return Fetch::request(url, "HEAD", headers);
}
Value HTTP::OPTIONS(const std::string& url, const std::unordered_map<std::string, std::string>& headers) {
    return Fetch::request(url, "OPTIONS", headers);
}
