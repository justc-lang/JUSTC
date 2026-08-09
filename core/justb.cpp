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

#include "justb.hpp"
#include <cstring>

namespace JUSTB {

bool writeHeader(std::ostream& out, const std::string& version, const uint8_t& compression, const uint8_t& type) {
    Header header;
    memcpy(header.magic, MAGIC, MAGIC_SIZE);
    header.filetype = type;
    header.version_len = static_cast<uint8_t>(version.size());
    header.version = version;
    header.compression = compression;

    out.write(header.magic, MAGIC_SIZE);
    out.write(reinterpret_cast<const char*>(&header.filetype), sizeof(header.filetype));
    out.write(reinterpret_cast<const char*>(&header.version_len), sizeof(header.version_len));
    out.write(header.version.c_str(), header.version_len);
    out.write(reinterpret_cast<const char*>(&header.compression), sizeof(header.compression));
    return out.good();
}

bool readHeader(std::istream& in, Header& header) {
    in.read(header.magic, MAGIC_SIZE);
    if (memcmp(header.magic, MAGIC, MAGIC_SIZE) != 0) return false;
    in.read(reinterpret_cast<char*>(&header.filetype), sizeof(header.filetype));
    in.read(reinterpret_cast<char*>(&header.version_len), sizeof(header.version_len));
    header.version.resize(header.version_len);
    in.read(&header.version[0], header.version_len);
    in.read(reinterpret_cast<char*>(&header.compression), sizeof(header.compression));
    return in.good();
}

bool validateHeader(const Header& header) {
    return memcmp(header.magic, MAGIC, MAGIC_SIZE) == 0;
}

}
