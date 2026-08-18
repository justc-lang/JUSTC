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

#ifndef COMPRESSION_HPP
#define COMPRESSION_HPP

#include <vector>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include "../../parser.h"

class COMPRESSION {
public:
    static std::vector<uint64_t> Size(const size_t& size);
    
    template<typename T>
    static size_t extractSizeFromVector(const std::vector<T>& data);
    
    template<typename T>
    static std::vector<uint8_t> convertToBytes(const std::vector<T>& data, size_t offset = 0);
    
    template<typename T>
    static std::vector<T> convertFromBytes(const std::vector<uint8_t>& data);
    
    template<typename T>
    static std::vector<uint8_t> toUint8Vector(const std::vector<T>& data);
    
    template<typename T>
    static std::vector<T> fromUint8Vector(const std::vector<uint8_t>& data);
    
    template<typename T>
    static std::vector<T> CompressGeneric(
        const std::vector<T>& data,
        CompressionAlgorithm algorithm,
        int level = 6
    );
    
    template<typename T>
    static std::vector<T> DecompressGeneric(
        const std::vector<T>& data,
        CompressionAlgorithm algorithm
    );

protected:
    static inline uint64_t bytesToUint64LE(const uint8_t* bytes);
    
    template<typename T>
    static std::vector<uint8_t> shrinkToBytes(const std::vector<T>& data);
};

class ZLIB : public COMPRESSION {
public:
    static std::vector<int8_t> CompressI8(const std::vector<int8_t>& data, int level = 6);
    static std::vector<int16_t> CompressI16(const std::vector<int16_t>& data, int level = 6);
    static std::vector<int32_t> CompressI32(const std::vector<int32_t>& data, int level = 6);
    static std::vector<int64_t> CompressI64(const std::vector<int64_t>& data, int level = 6);
    
    static std::vector<uint8_t> CompressU8(const std::vector<uint8_t>& data, int level = 6);
    static std::vector<uint16_t> CompressU16(const std::vector<uint16_t>& data, int level = 6);
    static std::vector<uint32_t> CompressU32(const std::vector<uint32_t>& data, int level = 6);
    static std::vector<uint64_t> CompressU64(const std::vector<uint64_t>& data, int level = 6);

    static std::vector<int8_t> DecompressI8(const std::vector<int8_t>& data);
    static std::vector<int16_t> DecompressI16(const std::vector<int16_t>& data);
    static std::vector<int32_t> DecompressI32(const std::vector<int32_t>& data);
    static std::vector<int64_t> DecompressI64(const std::vector<int64_t>& data);
    
    static std::vector<uint8_t> DecompressU8(const std::vector<uint8_t>& data);
    static std::vector<uint16_t> DecompressU16(const std::vector<uint16_t>& data);
    static std::vector<uint32_t> DecompressU32(const std::vector<uint32_t>& data);
    static std::vector<uint64_t> DecompressU64(const std::vector<uint64_t>& data);
};

class GZIP : public COMPRESSION {
public:
    static std::vector<int8_t> CompressI8(const std::vector<int8_t>& data, int level = 6);
    static std::vector<int16_t> CompressI16(const std::vector<int16_t>& data, int level = 6);
    static std::vector<int32_t> CompressI32(const std::vector<int32_t>& data, int level = 6);
    static std::vector<int64_t> CompressI64(const std::vector<int64_t>& data, int level = 6);
    
    static std::vector<uint8_t> CompressU8(const std::vector<uint8_t>& data, int level = 6);
    static std::vector<uint16_t> CompressU16(const std::vector<uint16_t>& data, int level = 6);
    static std::vector<uint32_t> CompressU32(const std::vector<uint32_t>& data, int level = 6);
    static std::vector<uint64_t> CompressU64(const std::vector<uint64_t>& data, int level = 6);

    static std::vector<int8_t> DecompressI8(const std::vector<int8_t>& data);
    static std::vector<int16_t> DecompressI16(const std::vector<int16_t>& data);
    static std::vector<int32_t> DecompressI32(const std::vector<int32_t>& data);
    static std::vector<int64_t> DecompressI64(const std::vector<int64_t>& data);
    
    static std::vector<uint8_t> DecompressU8(const std::vector<uint8_t>& data);
    static std::vector<uint16_t> DecompressU16(const std::vector<uint16_t>& data);
    static std::vector<uint32_t> DecompressU32(const std::vector<uint32_t>& data);
    static std::vector<uint64_t> DecompressU64(const std::vector<uint64_t>& data);
};

class BZIP2 : public COMPRESSION {
public:
    static std::vector<int8_t> CompressI8(const std::vector<int8_t>& data, int level = 6);
    static std::vector<int16_t> CompressI16(const std::vector<int16_t>& data, int level = 6);
    static std::vector<int32_t> CompressI32(const std::vector<int32_t>& data, int level = 6);
    static std::vector<int64_t> CompressI64(const std::vector<int64_t>& data, int level = 6);
    
    static std::vector<uint8_t> CompressU8(const std::vector<uint8_t>& data, int level = 6);
    static std::vector<uint16_t> CompressU16(const std::vector<uint16_t>& data, int level = 6);
    static std::vector<uint32_t> CompressU32(const std::vector<uint32_t>& data, int level = 6);
    static std::vector<uint64_t> CompressU64(const std::vector<uint64_t>& data, int level = 6);

    static std::vector<int8_t> DecompressI8(const std::vector<int8_t>& data);
    static std::vector<int16_t> DecompressI16(const std::vector<int16_t>& data);
    static std::vector<int32_t> DecompressI32(const std::vector<int32_t>& data);
    static std::vector<int64_t> DecompressI64(const std::vector<int64_t>& data);
    
    static std::vector<uint8_t> DecompressU8(const std::vector<uint8_t>& data);
    static std::vector<uint16_t> DecompressU16(const std::vector<uint16_t>& data);
    static std::vector<uint32_t> DecompressU32(const std::vector<uint32_t>& data);
    static std::vector<uint64_t> DecompressU64(const std::vector<uint64_t>& data);
};

class LZMA : public COMPRESSION {
public:
    static std::vector<int8_t> CompressI8(const std::vector<int8_t>& data, int level = 6);
    static std::vector<int16_t> CompressI16(const std::vector<int16_t>& data, int level = 6);
    static std::vector<int32_t> CompressI32(const std::vector<int32_t>& data, int level = 6);
    static std::vector<int64_t> CompressI64(const std::vector<int64_t>& data, int level = 6);
    
    static std::vector<uint8_t> CompressU8(const std::vector<uint8_t>& data, int level = 6);
    static std::vector<uint16_t> CompressU16(const std::vector<uint16_t>& data, int level = 6);
    static std::vector<uint32_t> CompressU32(const std::vector<uint32_t>& data, int level = 6);
    static std::vector<uint64_t> CompressU64(const std::vector<uint64_t>& data, int level = 6);

    static std::vector<int8_t> DecompressI8(const std::vector<int8_t>& data);
    static std::vector<int16_t> DecompressI16(const std::vector<int16_t>& data);
    static std::vector<int32_t> DecompressI32(const std::vector<int32_t>& data);
    static std::vector<int64_t> DecompressI64(const std::vector<int64_t>& data);
    
    static std::vector<uint8_t> DecompressU8(const std::vector<uint8_t>& data);
    static std::vector<uint16_t> DecompressU16(const std::vector<uint16_t>& data);
    static std::vector<uint32_t> DecompressU32(const std::vector<uint32_t>& data);
    static std::vector<uint64_t> DecompressU64(const std::vector<uint64_t>& data);
};

class ZSTD : public COMPRESSION {
public:
    static std::vector<int8_t> CompressI8(const std::vector<int8_t>& data, int level = 6);
    static std::vector<int16_t> CompressI16(const std::vector<int16_t>& data, int level = 6);
    static std::vector<int32_t> CompressI32(const std::vector<int32_t>& data, int level = 6);
    static std::vector<int64_t> CompressI64(const std::vector<int64_t>& data, int level = 6);
    
    static std::vector<uint8_t> CompressU8(const std::vector<uint8_t>& data, int level = 6);
    static std::vector<uint16_t> CompressU16(const std::vector<uint16_t>& data, int level = 6);
    static std::vector<uint32_t> CompressU32(const std::vector<uint32_t>& data, int level = 6);
    static std::vector<uint64_t> CompressU64(const std::vector<uint64_t>& data, int level = 6);

    static std::vector<int8_t> DecompressI8(const std::vector<int8_t>& data);
    static std::vector<int16_t> DecompressI16(const std::vector<int16_t>& data);
    static std::vector<int32_t> DecompressI32(const std::vector<int32_t>& data);
    static std::vector<int64_t> DecompressI64(const std::vector<int64_t>& data);
    
    static std::vector<uint8_t> DecompressU8(const std::vector<uint8_t>& data);
    static std::vector<uint16_t> DecompressU16(const std::vector<uint16_t>& data);
    static std::vector<uint32_t> DecompressU32(const std::vector<uint32_t>& data);
    static std::vector<uint64_t> DecompressU64(const std::vector<uint64_t>& data);
};

class LZ4 : public COMPRESSION {
public:
    static std::vector<int8_t> CompressI8(const std::vector<int8_t>& data, int level = 6);
    static std::vector<int16_t> CompressI16(const std::vector<int16_t>& data, int level = 6);
    static std::vector<int32_t> CompressI32(const std::vector<int32_t>& data, int level = 6);
    static std::vector<int64_t> CompressI64(const std::vector<int64_t>& data, int level = 6);
    
    static std::vector<uint8_t> CompressU8(const std::vector<uint8_t>& data, int level = 6);
    static std::vector<uint16_t> CompressU16(const std::vector<uint16_t>& data, int level = 6);
    static std::vector<uint32_t> CompressU32(const std::vector<uint32_t>& data, int level = 6);
    static std::vector<uint64_t> CompressU64(const std::vector<uint64_t>& data, int level = 6);

    static std::vector<int8_t> DecompressI8(const std::vector<int8_t>& data);
    static std::vector<int16_t> DecompressI16(const std::vector<int16_t>& data);
    static std::vector<int32_t> DecompressI32(const std::vector<int32_t>& data);
    static std::vector<int64_t> DecompressI64(const std::vector<int64_t>& data);
    
    static std::vector<uint8_t> DecompressU8(const std::vector<uint8_t>& data);
    static std::vector<uint16_t> DecompressU16(const std::vector<uint16_t>& data);
    static std::vector<uint32_t> DecompressU32(const std::vector<uint32_t>& data);
    static std::vector<uint64_t> DecompressU64(const std::vector<uint64_t>& data);
};

class SNAPPY : public COMPRESSION {
public:
    static std::vector<int8_t> CompressI8(const std::vector<int8_t>& data, int level = 6);
    static std::vector<int16_t> CompressI16(const std::vector<int16_t>& data, int level = 6);
    static std::vector<int32_t> CompressI32(const std::vector<int32_t>& data, int level = 6);
    static std::vector<int64_t> CompressI64(const std::vector<int64_t>& data, int level = 6);
    
    static std::vector<uint8_t> CompressU8(const std::vector<uint8_t>& data, int level = 6);
    static std::vector<uint16_t> CompressU16(const std::vector<uint16_t>& data, int level = 6);
    static std::vector<uint32_t> CompressU32(const std::vector<uint32_t>& data, int level = 6);
    static std::vector<uint64_t> CompressU64(const std::vector<uint64_t>& data, int level = 6);

    static std::vector<int8_t> DecompressI8(const std::vector<int8_t>& data);
    static std::vector<int16_t> DecompressI16(const std::vector<int16_t>& data);
    static std::vector<int32_t> DecompressI32(const std::vector<int32_t>& data);
    static std::vector<int64_t> DecompressI64(const std::vector<int64_t>& data);
    
    static std::vector<uint8_t> DecompressU8(const std::vector<uint8_t>& data);
    static std::vector<uint16_t> DecompressU16(const std::vector<uint16_t>& data);
    static std::vector<uint32_t> DecompressU32(const std::vector<uint32_t>& data);
    static std::vector<uint64_t> DecompressU64(const std::vector<uint64_t>& data);
};

class DEFLATE : public COMPRESSION {
public:
    static std::vector<int8_t> CompressI8(const std::vector<int8_t>& data, int level = 6);
    static std::vector<int16_t> CompressI16(const std::vector<int16_t>& data, int level = 6);
    static std::vector<int32_t> CompressI32(const std::vector<int32_t>& data, int level = 6);
    static std::vector<int64_t> CompressI64(const std::vector<int64_t>& data, int level = 6);
    
    static std::vector<uint8_t> CompressU8(const std::vector<uint8_t>& data, int level = 6);
    static std::vector<uint16_t> CompressU16(const std::vector<uint16_t>& data, int level = 6);
    static std::vector<uint32_t> CompressU32(const std::vector<uint32_t>& data, int level = 6);
    static std::vector<uint64_t> CompressU64(const std::vector<uint64_t>& data, int level = 6);

    static std::vector<int8_t> DecompressI8(const std::vector<int8_t>& data);
    static std::vector<int16_t> DecompressI16(const std::vector<int16_t>& data);
    static std::vector<int32_t> DecompressI32(const std::vector<int32_t>& data);
    static std::vector<int64_t> DecompressI64(const std::vector<int64_t>& data);
    
    static std::vector<uint8_t> DecompressU8(const std::vector<uint8_t>& data);
    static std::vector<uint16_t> DecompressU16(const std::vector<uint16_t>& data);
    static std::vector<uint32_t> DecompressU32(const std::vector<uint32_t>& data);
    static std::vector<uint64_t> DecompressU64(const std::vector<uint64_t>& data);
};

template<typename T>
size_t COMPRESSION::extractSizeFromVector(const std::vector<T>& data) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    
    const size_t totalBytes = data.size() * sizeof(T);
    if (totalBytes < 8) {
        throw std::runtime_error("Compressed data too short: missing size header");
    }
    
    uint8_t first8Bytes[8] = {0};
    size_t bytesRead = 0;
    
    for (size_t i = 0; i < data.size() && bytesRead < 8; ++i) {
        T value = data[i];
        for (size_t j = 0; j < sizeof(T) && bytesRead < 8; ++j) {
            first8Bytes[bytesRead++] = static_cast<uint8_t>((value >> (j * 8)) & 0xFF);
        }
    }
    
    return static_cast<size_t>(bytesToUint64LE(first8Bytes));
}

template<typename T>
std::vector<uint8_t> COMPRESSION::convertToBytes(const std::vector<T>& data, size_t offset) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    
    const size_t totalBytes = data.size() * sizeof(T);
    if (offset >= totalBytes) {
        throw std::runtime_error("Offset exceeds data size");
    }
    
    std::vector<uint8_t> result;
    result.reserve(totalBytes - offset);
    
    size_t skipped = 0;
    bool skipComplete = false;
    
    for (const T& val : data) {
        if (!skipComplete) {
            size_t bytesInThisElement = sizeof(T);
            if (skipped + bytesInThisElement <= offset) {
                skipped += bytesInThisElement;
                continue;
            }
            
            size_t startByte = offset - skipped;
            for (size_t j = startByte; j < bytesInThisElement; ++j) {
                result.push_back(static_cast<uint8_t>((val >> (j * 8)) & 0xFF));
            }
            skipComplete = true;
        } else {
            for (size_t j = 0; j < sizeof(T); ++j) {
                result.push_back(static_cast<uint8_t>((val >> (j * 8)) & 0xFF));
            }
        }
    }
    
    return result;
}

template<typename T>
std::vector<T> COMPRESSION::convertFromBytes(const std::vector<uint8_t>& data) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    
    const size_t elemSize = sizeof(T);
    if (data.size() % elemSize != 0) {
        throw std::runtime_error("Data size not aligned to element size");
    }
    
    std::vector<T> result;
    result.reserve(data.size() / elemSize);
    
    for (size_t i = 0; i < data.size(); i += elemSize) {
        T value = 0;
        for (size_t j = 0; j < elemSize && i + j < data.size(); ++j) {
            value |= static_cast<T>(data[i + j]) << (j * 8);
        }
        result.push_back(value);
    }
    
    return result;
}

template<typename T>
std::vector<uint8_t> COMPRESSION::toUint8Vector(const std::vector<T>& data) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    return shrinkToBytes(data);
}

template<typename T>
std::vector<T> COMPRESSION::fromUint8Vector(const std::vector<uint8_t>& data) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    return convertFromBytes<T>(data);
}

template<typename T>
std::vector<uint8_t> COMPRESSION::shrinkToBytes(const std::vector<T>& data) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    
    std::vector<uint8_t> result;
    result.reserve(data.size() * sizeof(T));
    
    for (const T& val : data) {
        for (size_t j = 0; j < sizeof(T); ++j) {
            result.push_back(static_cast<uint8_t>((val >> (j * 8)) & 0xFF));
        }
    }
    
    return result;
}

#endif
