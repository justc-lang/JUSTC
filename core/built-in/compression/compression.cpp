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

#include "compression.hpp"
#include "../../compiler/justb.hpp"
#include "../../utility.h"
#include "../../loader/justb.hpp"
#include "../../parser.h"

std::vector<uint64_t> COMPRESSION::Size(const size_t& size) {
    return {static_cast<uint64_t>(size)};
}

inline uint64_t COMPRESSION::bytesToUint64LE(const uint8_t* bytes) {
    uint64_t result = 0;
    for (size_t i = 0; i < 8; ++i) {
        result |= static_cast<uint64_t>(bytes[i]) << (i * 8);
    }
    return result;
}

#define INSTANTIATE_COMPRESSION_TEMPLATES(T) \
    template size_t COMPRESSION::extractSizeFromVector<T>(const std::vector<T>&); \
    template std::vector<uint8_t> COMPRESSION::convertToBytes<T>(const std::vector<T>&, size_t); \
    template std::vector<T> COMPRESSION::convertFromBytes<T>(const std::vector<uint8_t>&); \
    template std::vector<uint8_t> COMPRESSION::toUint8Vector<T>(const std::vector<T>&); \
    template std::vector<T> COMPRESSION::fromUint8Vector<T>(const std::vector<uint8_t>&); \
    template std::vector<T> COMPRESSION::CompressGeneric<T>(const std::vector<T>&, CompressionAlgorithm, int); \
    template std::vector<T> COMPRESSION::DecompressGeneric<T>(const std::vector<T>&, CompressionAlgorithm); \
    template std::vector<uint8_t> COMPRESSION::shrinkToBytes<T>(const std::vector<T>&);

INSTANTIATE_COMPRESSION_TEMPLATES(int8_t)
INSTANTIATE_COMPRESSION_TEMPLATES(int16_t)
INSTANTIATE_COMPRESSION_TEMPLATES(int32_t)
INSTANTIATE_COMPRESSION_TEMPLATES(int64_t)
INSTANTIATE_COMPRESSION_TEMPLATES(uint8_t)
INSTANTIATE_COMPRESSION_TEMPLATES(uint16_t)
INSTANTIATE_COMPRESSION_TEMPLATES(uint32_t)
INSTANTIATE_COMPRESSION_TEMPLATES(uint64_t)

#undef INSTANTIATE_COMPRESSION_TEMPLATES

template<typename T>
std::vector<T> COMPRESSION::CompressGeneric(
    const std::vector<T>& data,
    CompressionAlgorithm algorithm,
    int level
) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    
    std::vector<uint8_t> bytes = toUint8Vector(data);
    
    size_t originalSize = bytes.size();
    
    std::vector<uint8_t> compressed;
    switch (algorithm) {
        case CompressionAlgorithm::ZLIB:
            compressed = JustbCompiler::compressZlib(bytes, level);
            break;
        case CompressionAlgorithm::GZIP:
            compressed = JustbCompiler::compressGzip(bytes, level);
            break;
        case CompressionAlgorithm::BZIP2:
            compressed = JustbCompiler::compressBzip2(bytes, level);
            break;
        case CompressionAlgorithm::LZMA:
            compressed = JustbCompiler::compressLzma(bytes, level);
            break;
        case CompressionAlgorithm::ZSTD:
            compressed = JustbCompiler::compressZstd(bytes, level);
            break;
        case CompressionAlgorithm::LZ4:
            compressed = JustbCompiler::compressLz4(bytes);
            break;
        case CompressionAlgorithm::SNAPPY:
            compressed = JustbCompiler::compressSnappy(bytes);
            break;
        case CompressionAlgorithm::DEFLATE:
            compressed = JustbCompiler::compressDeflate(bytes, level);
            break;
        default:
            throw std::runtime_error("Unsupported compression algorithm");
    }
    
    std::vector<uint8_t> sizeBytes = shrinkToBytes(Size(originalSize));
    
    std::vector<uint8_t> result;
    result.reserve(sizeBytes.size() + compressed.size());
    result.insert(result.end(), sizeBytes.begin(), sizeBytes.end());
    result.insert(result.end(), compressed.begin(), compressed.end());
    
    return fromUint8Vector<T>(result);
}

template<typename T>
std::vector<T> COMPRESSION::DecompressGeneric(
    const std::vector<T>& data,
    CompressionAlgorithm algorithm
) {
    static_assert(std::is_integral_v<T>, "T must be integral type");
    
    size_t originalSize = extractSizeFromVector(data);
    
    std::vector<uint8_t> compressedBytes = convertToBytes(data, 8);
    
    std::vector<uint8_t> decompressed = JustbLoader::decompressData(
        compressedBytes, 
        static_cast<uint8_t>(algorithm), 
        originalSize
    );
    
    if (decompressed.size() != originalSize) {
        throw std::runtime_error("Decompressed size mismatch: expected " + 
                                 std::to_string(originalSize) + ", got " + 
                                 std::to_string(decompressed.size()));
    }
    
    return fromUint8Vector<T>(decompressed);
}

std::vector<int8_t> ZLIB::CompressI8(const std::vector<int8_t>& data, int level) {
    return CompressGeneric<int8_t>(data, CompressionAlgorithm::ZLIB, level);
}

std::vector<int16_t> ZLIB::CompressI16(const std::vector<int16_t>& data, int level) {
    return CompressGeneric<int16_t>(data, CompressionAlgorithm::ZLIB, level);
}

std::vector<int32_t> ZLIB::CompressI32(const std::vector<int32_t>& data, int level) {
    return CompressGeneric<int32_t>(data, CompressionAlgorithm::ZLIB, level);
}

std::vector<int64_t> ZLIB::CompressI64(const std::vector<int64_t>& data, int level) {
    return CompressGeneric<int64_t>(data, CompressionAlgorithm::ZLIB, level);
}

std::vector<uint8_t> ZLIB::CompressU8(const std::vector<uint8_t>& data, int level) {
    return CompressGeneric<uint8_t>(data, CompressionAlgorithm::ZLIB, level);
}

std::vector<uint16_t> ZLIB::CompressU16(const std::vector<uint16_t>& data, int level) {
    return CompressGeneric<uint16_t>(data, CompressionAlgorithm::ZLIB, level);
}

std::vector<uint32_t> ZLIB::CompressU32(const std::vector<uint32_t>& data, int level) {
    return CompressGeneric<uint32_t>(data, CompressionAlgorithm::ZLIB, level);
}

std::vector<uint64_t> ZLIB::CompressU64(const std::vector<uint64_t>& data, int level) {
    return CompressGeneric<uint64_t>(data, CompressionAlgorithm::ZLIB, level);
}

std::vector<int8_t> ZLIB::DecompressI8(const std::vector<int8_t>& data) {
    return DecompressGeneric<int8_t>(data, CompressionAlgorithm::ZLIB);
}

std::vector<int16_t> ZLIB::DecompressI16(const std::vector<int16_t>& data) {
    return DecompressGeneric<int16_t>(data, CompressionAlgorithm::ZLIB);
}

std::vector<int32_t> ZLIB::DecompressI32(const std::vector<int32_t>& data) {
    return DecompressGeneric<int32_t>(data, CompressionAlgorithm::ZLIB);
}

std::vector<int64_t> ZLIB::DecompressI64(const std::vector<int64_t>& data) {
    return DecompressGeneric<int64_t>(data, CompressionAlgorithm::ZLIB);
}

std::vector<uint8_t> ZLIB::DecompressU8(const std::vector<uint8_t>& data) {
    return DecompressGeneric<uint8_t>(data, CompressionAlgorithm::ZLIB);
}

std::vector<uint16_t> ZLIB::DecompressU16(const std::vector<uint16_t>& data) {
    return DecompressGeneric<uint16_t>(data, CompressionAlgorithm::ZLIB);
}

std::vector<uint32_t> ZLIB::DecompressU32(const std::vector<uint32_t>& data) {
    return DecompressGeneric<uint32_t>(data, CompressionAlgorithm::ZLIB);
}

std::vector<uint64_t> ZLIB::DecompressU64(const std::vector<uint64_t>& data) {
    return DecompressGeneric<uint64_t>(data, CompressionAlgorithm::ZLIB);
}

std::vector<int8_t> GZIP::CompressI8(const std::vector<int8_t>& data, int level) {
    return CompressGeneric<int8_t>(data, CompressionAlgorithm::GZIP, level);
}

std::vector<int16_t> GZIP::CompressI16(const std::vector<int16_t>& data, int level) {
    return CompressGeneric<int16_t>(data, CompressionAlgorithm::GZIP, level);
}

std::vector<int32_t> GZIP::CompressI32(const std::vector<int32_t>& data, int level) {
    return CompressGeneric<int32_t>(data, CompressionAlgorithm::GZIP, level);
}

std::vector<int64_t> GZIP::CompressI64(const std::vector<int64_t>& data, int level) {
    return CompressGeneric<int64_t>(data, CompressionAlgorithm::GZIP, level);
}

std::vector<uint8_t> GZIP::CompressU8(const std::vector<uint8_t>& data, int level) {
    return CompressGeneric<uint8_t>(data, CompressionAlgorithm::GZIP, level);
}

std::vector<uint16_t> GZIP::CompressU16(const std::vector<uint16_t>& data, int level) {
    return CompressGeneric<uint16_t>(data, CompressionAlgorithm::GZIP, level);
}

std::vector<uint32_t> GZIP::CompressU32(const std::vector<uint32_t>& data, int level) {
    return CompressGeneric<uint32_t>(data, CompressionAlgorithm::GZIP, level);
}

std::vector<uint64_t> GZIP::CompressU64(const std::vector<uint64_t>& data, int level) {
    return CompressGeneric<uint64_t>(data, CompressionAlgorithm::GZIP, level);
}

std::vector<int8_t> GZIP::DecompressI8(const std::vector<int8_t>& data) {
    return DecompressGeneric<int8_t>(data, CompressionAlgorithm::GZIP);
}

std::vector<int16_t> GZIP::DecompressI16(const std::vector<int16_t>& data) {
    return DecompressGeneric<int16_t>(data, CompressionAlgorithm::GZIP);
}

std::vector<int32_t> GZIP::DecompressI32(const std::vector<int32_t>& data) {
    return DecompressGeneric<int32_t>(data, CompressionAlgorithm::GZIP);
}

std::vector<int64_t> GZIP::DecompressI64(const std::vector<int64_t>& data) {
    return DecompressGeneric<int64_t>(data, CompressionAlgorithm::GZIP);
}

std::vector<uint8_t> GZIP::DecompressU8(const std::vector<uint8_t>& data) {
    return DecompressGeneric<uint8_t>(data, CompressionAlgorithm::GZIP);
}

std::vector<uint16_t> GZIP::DecompressU16(const std::vector<uint16_t>& data) {
    return DecompressGeneric<uint16_t>(data, CompressionAlgorithm::GZIP);
}

std::vector<uint32_t> GZIP::DecompressU32(const std::vector<uint32_t>& data) {
    return DecompressGeneric<uint32_t>(data, CompressionAlgorithm::GZIP);
}

std::vector<uint64_t> GZIP::DecompressU64(const std::vector<uint64_t>& data) {
    return DecompressGeneric<uint64_t>(data, CompressionAlgorithm::GZIP);
}

std::vector<int8_t> BZIP2::CompressI8(const std::vector<int8_t>& data, int level) {
    return CompressGeneric<int8_t>(data, CompressionAlgorithm::BZIP2, level);
}

std::vector<int16_t> BZIP2::CompressI16(const std::vector<int16_t>& data, int level) {
    return CompressGeneric<int16_t>(data, CompressionAlgorithm::BZIP2, level);
}

std::vector<int32_t> BZIP2::CompressI32(const std::vector<int32_t>& data, int level) {
    return CompressGeneric<int32_t>(data, CompressionAlgorithm::BZIP2, level);
}

std::vector<int64_t> BZIP2::CompressI64(const std::vector<int64_t>& data, int level) {
    return CompressGeneric<int64_t>(data, CompressionAlgorithm::BZIP2, level);
}

std::vector<uint8_t> BZIP2::CompressU8(const std::vector<uint8_t>& data, int level) {
    return CompressGeneric<uint8_t>(data, CompressionAlgorithm::BZIP2, level);
}

std::vector<uint16_t> BZIP2::CompressU16(const std::vector<uint16_t>& data, int level) {
    return CompressGeneric<uint16_t>(data, CompressionAlgorithm::BZIP2, level);
}

std::vector<uint32_t> BZIP2::CompressU32(const std::vector<uint32_t>& data, int level) {
    return CompressGeneric<uint32_t>(data, CompressionAlgorithm::BZIP2, level);
}

std::vector<uint64_t> BZIP2::CompressU64(const std::vector<uint64_t>& data, int level) {
    return CompressGeneric<uint64_t>(data, CompressionAlgorithm::BZIP2, level);
}

std::vector<int8_t> BZIP2::DecompressI8(const std::vector<int8_t>& data) {
    return DecompressGeneric<int8_t>(data, CompressionAlgorithm::BZIP2);
}

std::vector<int16_t> BZIP2::DecompressI16(const std::vector<int16_t>& data) {
    return DecompressGeneric<int16_t>(data, CompressionAlgorithm::BZIP2);
}

std::vector<int32_t> BZIP2::DecompressI32(const std::vector<int32_t>& data) {
    return DecompressGeneric<int32_t>(data, CompressionAlgorithm::BZIP2);
}

std::vector<int64_t> BZIP2::DecompressI64(const std::vector<int64_t>& data) {
    return DecompressGeneric<int64_t>(data, CompressionAlgorithm::BZIP2);
}

std::vector<uint8_t> BZIP2::DecompressU8(const std::vector<uint8_t>& data) {
    return DecompressGeneric<uint8_t>(data, CompressionAlgorithm::BZIP2);
}

std::vector<uint16_t> BZIP2::DecompressU16(const std::vector<uint16_t>& data) {
    return DecompressGeneric<uint16_t>(data, CompressionAlgorithm::BZIP2);
}

std::vector<uint32_t> BZIP2::DecompressU32(const std::vector<uint32_t>& data) {
    return DecompressGeneric<uint32_t>(data, CompressionAlgorithm::BZIP2);
}

std::vector<uint64_t> BZIP2::DecompressU64(const std::vector<uint64_t>& data) {
    return DecompressGeneric<uint64_t>(data, CompressionAlgorithm::BZIP2);
}

std::vector<int8_t> LZMA::CompressI8(const std::vector<int8_t>& data, int level) {
    return CompressGeneric<int8_t>(data, CompressionAlgorithm::LZMA, level);
}

std::vector<int16_t> LZMA::CompressI16(const std::vector<int16_t>& data, int level) {
    return CompressGeneric<int16_t>(data, CompressionAlgorithm::LZMA, level);
}

std::vector<int32_t> LZMA::CompressI32(const std::vector<int32_t>& data, int level) {
    return CompressGeneric<int32_t>(data, CompressionAlgorithm::LZMA, level);
}

std::vector<int64_t> LZMA::CompressI64(const std::vector<int64_t>& data, int level) {
    return CompressGeneric<int64_t>(data, CompressionAlgorithm::LZMA, level);
}

std::vector<uint8_t> LZMA::CompressU8(const std::vector<uint8_t>& data, int level) {
    return CompressGeneric<uint8_t>(data, CompressionAlgorithm::LZMA, level);
}

std::vector<uint16_t> LZMA::CompressU16(const std::vector<uint16_t>& data, int level) {
    return CompressGeneric<uint16_t>(data, CompressionAlgorithm::LZMA, level);
}

std::vector<uint32_t> LZMA::CompressU32(const std::vector<uint32_t>& data, int level) {
    return CompressGeneric<uint32_t>(data, CompressionAlgorithm::LZMA, level);
}

std::vector<uint64_t> LZMA::CompressU64(const std::vector<uint64_t>& data, int level) {
    return CompressGeneric<uint64_t>(data, CompressionAlgorithm::LZMA, level);
}

std::vector<int8_t> LZMA::DecompressI8(const std::vector<int8_t>& data) {
    return DecompressGeneric<int8_t>(data, CompressionAlgorithm::LZMA);
}

std::vector<int16_t> LZMA::DecompressI16(const std::vector<int16_t>& data) {
    return DecompressGeneric<int16_t>(data, CompressionAlgorithm::LZMA);
}

std::vector<int32_t> LZMA::DecompressI32(const std::vector<int32_t>& data) {
    return DecompressGeneric<int32_t>(data, CompressionAlgorithm::LZMA);
}

std::vector<int64_t> LZMA::DecompressI64(const std::vector<int64_t>& data) {
    return DecompressGeneric<int64_t>(data, CompressionAlgorithm::LZMA);
}

std::vector<uint8_t> LZMA::DecompressU8(const std::vector<uint8_t>& data) {
    return DecompressGeneric<uint8_t>(data, CompressionAlgorithm::LZMA);
}

std::vector<uint16_t> LZMA::DecompressU16(const std::vector<uint16_t>& data) {
    return DecompressGeneric<uint16_t>(data, CompressionAlgorithm::LZMA);
}

std::vector<uint32_t> LZMA::DecompressU32(const std::vector<uint32_t>& data) {
    return DecompressGeneric<uint32_t>(data, CompressionAlgorithm::LZMA);
}

std::vector<uint64_t> LZMA::DecompressU64(const std::vector<uint64_t>& data) {
    return DecompressGeneric<uint64_t>(data, CompressionAlgorithm::LZMA);
}

std::vector<int8_t> ZSTD::CompressI8(const std::vector<int8_t>& data, int level) {
    return CompressGeneric<int8_t>(data, CompressionAlgorithm::ZSTD, level);
}

std::vector<int16_t> ZSTD::CompressI16(const std::vector<int16_t>& data, int level) {
    return CompressGeneric<int16_t>(data, CompressionAlgorithm::ZSTD, level);
}

std::vector<int32_t> ZSTD::CompressI32(const std::vector<int32_t>& data, int level) {
    return CompressGeneric<int32_t>(data, CompressionAlgorithm::ZSTD, level);
}

std::vector<int64_t> ZSTD::CompressI64(const std::vector<int64_t>& data, int level) {
    return CompressGeneric<int64_t>(data, CompressionAlgorithm::ZSTD, level);
}

std::vector<uint8_t> ZSTD::CompressU8(const std::vector<uint8_t>& data, int level) {
    return CompressGeneric<uint8_t>(data, CompressionAlgorithm::ZSTD, level);
}

std::vector<uint16_t> ZSTD::CompressU16(const std::vector<uint16_t>& data, int level) {
    return CompressGeneric<uint16_t>(data, CompressionAlgorithm::ZSTD, level);
}

std::vector<uint32_t> ZSTD::CompressU32(const std::vector<uint32_t>& data, int level) {
    return CompressGeneric<uint32_t>(data, CompressionAlgorithm::ZSTD, level);
}

std::vector<uint64_t> ZSTD::CompressU64(const std::vector<uint64_t>& data, int level) {
    return CompressGeneric<uint64_t>(data, CompressionAlgorithm::ZSTD, level);
}

std::vector<int8_t> ZSTD::DecompressI8(const std::vector<int8_t>& data) {
    return DecompressGeneric<int8_t>(data, CompressionAlgorithm::ZSTD);
}

std::vector<int16_t> ZSTD::DecompressI16(const std::vector<int16_t>& data) {
    return DecompressGeneric<int16_t>(data, CompressionAlgorithm::ZSTD);
}

std::vector<int32_t> ZSTD::DecompressI32(const std::vector<int32_t>& data) {
    return DecompressGeneric<int32_t>(data, CompressionAlgorithm::ZSTD);
}

std::vector<int64_t> ZSTD::DecompressI64(const std::vector<int64_t>& data) {
    return DecompressGeneric<int64_t>(data, CompressionAlgorithm::ZSTD);
}

std::vector<uint8_t> ZSTD::DecompressU8(const std::vector<uint8_t>& data) {
    return DecompressGeneric<uint8_t>(data, CompressionAlgorithm::ZSTD);
}

std::vector<uint16_t> ZSTD::DecompressU16(const std::vector<uint16_t>& data) {
    return DecompressGeneric<uint16_t>(data, CompressionAlgorithm::ZSTD);
}

std::vector<uint32_t> ZSTD::DecompressU32(const std::vector<uint32_t>& data) {
    return DecompressGeneric<uint32_t>(data, CompressionAlgorithm::ZSTD);
}

std::vector<uint64_t> ZSTD::DecompressU64(const std::vector<uint64_t>& data) {
    return DecompressGeneric<uint64_t>(data, CompressionAlgorithm::ZSTD);
}

std::vector<int8_t> LZ4::CompressI8(const std::vector<int8_t>& data, int level) {
    return CompressGeneric<int8_t>(data, CompressionAlgorithm::LZ4, level);
}

std::vector<int16_t> LZ4::CompressI16(const std::vector<int16_t>& data, int level) {
    return CompressGeneric<int16_t>(data, CompressionAlgorithm::LZ4, level);
}

std::vector<int32_t> LZ4::CompressI32(const std::vector<int32_t>& data, int level) {
    return CompressGeneric<int32_t>(data, CompressionAlgorithm::LZ4, level);
}

std::vector<int64_t> LZ4::CompressI64(const std::vector<int64_t>& data, int level) {
    return CompressGeneric<int64_t>(data, CompressionAlgorithm::LZ4, level);
}

std::vector<uint8_t> LZ4::CompressU8(const std::vector<uint8_t>& data, int level) {
    return CompressGeneric<uint8_t>(data, CompressionAlgorithm::LZ4, level);
}

std::vector<uint16_t> LZ4::CompressU16(const std::vector<uint16_t>& data, int level) {
    return CompressGeneric<uint16_t>(data, CompressionAlgorithm::LZ4, level);
}

std::vector<uint32_t> LZ4::CompressU32(const std::vector<uint32_t>& data, int level) {
    return CompressGeneric<uint32_t>(data, CompressionAlgorithm::LZ4, level);
}

std::vector<uint64_t> LZ4::CompressU64(const std::vector<uint64_t>& data, int level) {
    return CompressGeneric<uint64_t>(data, CompressionAlgorithm::LZ4, level);
}

std::vector<int8_t> LZ4::DecompressI8(const std::vector<int8_t>& data) {
    return DecompressGeneric<int8_t>(data, CompressionAlgorithm::LZ4);
}

std::vector<int16_t> LZ4::DecompressI16(const std::vector<int16_t>& data) {
    return DecompressGeneric<int16_t>(data, CompressionAlgorithm::LZ4);
}

std::vector<int32_t> LZ4::DecompressI32(const std::vector<int32_t>& data) {
    return DecompressGeneric<int32_t>(data, CompressionAlgorithm::LZ4);
}

std::vector<int64_t> LZ4::DecompressI64(const std::vector<int64_t>& data) {
    return DecompressGeneric<int64_t>(data, CompressionAlgorithm::LZ4);
}

std::vector<uint8_t> LZ4::DecompressU8(const std::vector<uint8_t>& data) {
    return DecompressGeneric<uint8_t>(data, CompressionAlgorithm::LZ4);
}

std::vector<uint16_t> LZ4::DecompressU16(const std::vector<uint16_t>& data) {
    return DecompressGeneric<uint16_t>(data, CompressionAlgorithm::LZ4);
}

std::vector<uint32_t> LZ4::DecompressU32(const std::vector<uint32_t>& data) {
    return DecompressGeneric<uint32_t>(data, CompressionAlgorithm::LZ4);
}

std::vector<uint64_t> LZ4::DecompressU64(const std::vector<uint64_t>& data) {
    return DecompressGeneric<uint64_t>(data, CompressionAlgorithm::LZ4);
}

std::vector<int8_t> SNAPPY::CompressI8(const std::vector<int8_t>& data, int level) {
    return CompressGeneric<int8_t>(data, CompressionAlgorithm::SNAPPY, level);
}

std::vector<int16_t> SNAPPY::CompressI16(const std::vector<int16_t>& data, int level) {
    return CompressGeneric<int16_t>(data, CompressionAlgorithm::SNAPPY, level);
}

std::vector<int32_t> SNAPPY::CompressI32(const std::vector<int32_t>& data, int level) {
    return CompressGeneric<int32_t>(data, CompressionAlgorithm::SNAPPY, level);
}

std::vector<int64_t> SNAPPY::CompressI64(const std::vector<int64_t>& data, int level) {
    return CompressGeneric<int64_t>(data, CompressionAlgorithm::SNAPPY, level);
}

std::vector<uint8_t> SNAPPY::CompressU8(const std::vector<uint8_t>& data, int level) {
    return CompressGeneric<uint8_t>(data, CompressionAlgorithm::SNAPPY, level);
}

std::vector<uint16_t> SNAPPY::CompressU16(const std::vector<uint16_t>& data, int level) {
    return CompressGeneric<uint16_t>(data, CompressionAlgorithm::SNAPPY, level);
}

std::vector<uint32_t> SNAPPY::CompressU32(const std::vector<uint32_t>& data, int level) {
    return CompressGeneric<uint32_t>(data, CompressionAlgorithm::SNAPPY, level);
}

std::vector<uint64_t> SNAPPY::CompressU64(const std::vector<uint64_t>& data, int level) {
    return CompressGeneric<uint64_t>(data, CompressionAlgorithm::SNAPPY, level);
}

std::vector<int8_t> SNAPPY::DecompressI8(const std::vector<int8_t>& data) {
    return DecompressGeneric<int8_t>(data, CompressionAlgorithm::SNAPPY);
}

std::vector<int16_t> SNAPPY::DecompressI16(const std::vector<int16_t>& data) {
    return DecompressGeneric<int16_t>(data, CompressionAlgorithm::SNAPPY);
}

std::vector<int32_t> SNAPPY::DecompressI32(const std::vector<int32_t>& data) {
    return DecompressGeneric<int32_t>(data, CompressionAlgorithm::SNAPPY);
}

std::vector<int64_t> SNAPPY::DecompressI64(const std::vector<int64_t>& data) {
    return DecompressGeneric<int64_t>(data, CompressionAlgorithm::SNAPPY);
}

std::vector<uint8_t> SNAPPY::DecompressU8(const std::vector<uint8_t>& data) {
    return DecompressGeneric<uint8_t>(data, CompressionAlgorithm::SNAPPY);
}

std::vector<uint16_t> SNAPPY::DecompressU16(const std::vector<uint16_t>& data) {
    return DecompressGeneric<uint16_t>(data, CompressionAlgorithm::SNAPPY);
}

std::vector<uint32_t> SNAPPY::DecompressU32(const std::vector<uint32_t>& data) {
    return DecompressGeneric<uint32_t>(data, CompressionAlgorithm::SNAPPY);
}

std::vector<uint64_t> SNAPPY::DecompressU64(const std::vector<uint64_t>& data) {
    return DecompressGeneric<uint64_t>(data, CompressionAlgorithm::SNAPPY);
}

std::vector<int8_t> DEFLATE::CompressI8(const std::vector<int8_t>& data, int level) {
    return CompressGeneric<int8_t>(data, CompressionAlgorithm::DEFLATE, level);
}

std::vector<int16_t> DEFLATE::CompressI16(const std::vector<int16_t>& data, int level) {
    return CompressGeneric<int16_t>(data, CompressionAlgorithm::DEFLATE, level);
}

std::vector<int32_t> DEFLATE::CompressI32(const std::vector<int32_t>& data, int level) {
    return CompressGeneric<int32_t>(data, CompressionAlgorithm::DEFLATE, level);
}

std::vector<int64_t> DEFLATE::CompressI64(const std::vector<int64_t>& data, int level) {
    return CompressGeneric<int64_t>(data, CompressionAlgorithm::DEFLATE, level);
}

std::vector<uint8_t> DEFLATE::CompressU8(const std::vector<uint8_t>& data, int level) {
    return CompressGeneric<uint8_t>(data, CompressionAlgorithm::DEFLATE, level);
}

std::vector<uint16_t> DEFLATE::CompressU16(const std::vector<uint16_t>& data, int level) {
    return CompressGeneric<uint16_t>(data, CompressionAlgorithm::DEFLATE, level);
}

std::vector<uint32_t> DEFLATE::CompressU32(const std::vector<uint32_t>& data, int level) {
    return CompressGeneric<uint32_t>(data, CompressionAlgorithm::DEFLATE, level);
}

std::vector<uint64_t> DEFLATE::CompressU64(const std::vector<uint64_t>& data, int level) {
    return CompressGeneric<uint64_t>(data, CompressionAlgorithm::DEFLATE, level);
}

std::vector<int8_t> DEFLATE::DecompressI8(const std::vector<int8_t>& data) {
    return DecompressGeneric<int8_t>(data, CompressionAlgorithm::DEFLATE);
}

std::vector<int16_t> DEFLATE::DecompressI16(const std::vector<int16_t>& data) {
    return DecompressGeneric<int16_t>(data, CompressionAlgorithm::DEFLATE);
}

std::vector<int32_t> DEFLATE::DecompressI32(const std::vector<int32_t>& data) {
    return DecompressGeneric<int32_t>(data, CompressionAlgorithm::DEFLATE);
}

std::vector<int64_t> DEFLATE::DecompressI64(const std::vector<int64_t>& data) {
    return DecompressGeneric<int64_t>(data, CompressionAlgorithm::DEFLATE);
}

std::vector<uint8_t> DEFLATE::DecompressU8(const std::vector<uint8_t>& data) {
    return DecompressGeneric<uint8_t>(data, CompressionAlgorithm::DEFLATE);
}

std::vector<uint16_t> DEFLATE::DecompressU16(const std::vector<uint16_t>& data) {
    return DecompressGeneric<uint16_t>(data, CompressionAlgorithm::DEFLATE);
}

std::vector<uint32_t> DEFLATE::DecompressU32(const std::vector<uint32_t>& data) {
    return DecompressGeneric<uint32_t>(data, CompressionAlgorithm::DEFLATE);
}

std::vector<uint64_t> DEFLATE::DecompressU64(const std::vector<uint64_t>& data) {
    return DecompressGeneric<uint64_t>(data, CompressionAlgorithm::DEFLATE);
}
