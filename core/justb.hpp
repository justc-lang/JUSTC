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

JUSTB - Just an Ultimate Site Tool Binary file format.

*/

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include "version.h"

namespace JUSTB {

const char MAGIC[] = "JUSTB";
const size_t MAGIC_SIZE = 5;

struct Header {
    char magic[MAGIC_SIZE];
    uint8_t filetype;
    uint8_t version_len;
    std::string version;
    uint8_t compression;
};

bool writeHeader(std::ostream& out, const std::string& version = JUSTC_VERSION, const uint8_t& compression = 0, const uint8_t& type = 0);
bool readHeader(std::istream& in, Header& header);
bool validateHeader(const Header& header);

}
