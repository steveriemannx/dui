#include "duilib/Utils/StringCharset.h"
#include "duilib/Utils/StringConvert.h"

namespace ui
{

uint32_t StringCharset::GetBOMSize(CharsetType charsetType)
{
    switch (charsetType)
    {
    case CharsetType::UTF8:
        return 3;
    case CharsetType::UTF16_BE:
    case CharsetType::UTF16_LE:
        return 2;
    default:
        break;
    }
    return 0;
}

CharsetType StringCharset::GetDataCharsetByBOM(const char* data, uint32_t length)
{
    CharsetType charsetType = CharsetType::UNKNOWN;
    if ((length < 2) || (data == nullptr)) {
        return charsetType;
    }
    if (length >= 4 && data[0] == '\x00' && data[1] == '\x00' && data[2] == '\xFE' && data[3] == '\xFF') {
        //"UTF-32BE";
        charsetType = CharsetType::UNKNOWN;
    }
    else if (length >= 4 && data[0] == '\xFF' && data[1] == '\xFE' && data[2] == '\x00' && data[3] == '\x00') {        
        if (length == 4) {
            charsetType = CharsetType::UTF16_LE;
        }
        else {
            //"UTF-32LE";
            charsetType = CharsetType::UNKNOWN;
        }
    }
    else if (length >= 3 && data[0] == '\xEF' && data[1] == '\xBB' && data[2] == '\xBF') {
        charsetType = CharsetType::UTF8;
    }
    else if (length >= 2 && data[0] == '\xFF' && data[1] == '\xFE') {
        charsetType = CharsetType::UTF16_LE;
    }
    else if (length >= 2 && data[0] == '\xFE' && data[1] == '\xFF') {
        charsetType = CharsetType::UTF16_BE;
    }
    return charsetType;
}

CharsetType StringCharset::GetDataCharset(const char* data, uint32_t length)
{
    CharsetType charsetType = CharsetType::UNKNOWN;
    if ((length < 1) || (data == nullptr)) {
        return charsetType;
    }
    if (IsValidateASCIIStream(data, length)) {
        charsetType = CharsetType::ANSI;
    }
    else if (IsValidateGBKStream(data, length)) {
        if (IsValidateUTF8Stream(data, length)) {
            charsetType = CharsetType::UTF8;
        }
        else {
            charsetType = CharsetType::ANSI;
        }
    }
    else if (IsValidateUTF8Stream(data, length)) {
        charsetType = CharsetType::UTF8;
    }
    else if (IsValidateUTF16LEStream(data, length)) {
        charsetType = CharsetType::UTF16_LE;
    }
    else if (IsValidateUTF16BEStream(data, length)) {
        charsetType = CharsetType::UTF16_BE;
    }
    return charsetType;
}

bool StringCharset::GetDataAsString(const char* data, uint32_t length, std::wstring& result)
{
    CharsetType inCharsetType = CharsetType::UNKNOWN;
    CharsetType outCharsetType = CharsetType::UNKNOWN;
    uint32_t bomSize = 0;
    return GetDataAsString(data, length, inCharsetType, result, outCharsetType, bomSize);
}

bool StringCharset::GetDataAsString(const char* data, uint32_t length, CharsetType inCharsetType, std::wstring& result)
{
    CharsetType outCharsetType = CharsetType::UNKNOWN;
    uint32_t bomSize = 0;
    return GetDataAsString(data, length, inCharsetType, result, outCharsetType, bomSize);
}

bool StringCharset::GetDataAsString(const char* data, uint32_t length,
                                    std::wstring& result, CharsetType& outCharsetType, uint32_t& bomSize)
{
    CharsetType inCharsetType = CharsetType::UNKNOWN;
    return GetDataAsString(data, length, inCharsetType, result, outCharsetType, bomSize);
}

bool StringCharset::GetDataAsString(const char* data, uint32_t length, CharsetType inCharsetType,
                                    std::wstring& result, CharsetType& outCharsetType, uint32_t& bomSize)
{
    result.clear();
    outCharsetType = CharsetType::UNKNOWN;
    bomSize = 0;
    if ((length < 1) || (data == nullptr)) {
        return false;
    }
    outCharsetType = GetDataCharsetByBOM(data, length);
    bomSize = GetBOMSize(outCharsetType);
    if (inCharsetType != CharsetType::UNKNOWN) {
        if (inCharsetType != outCharsetType) {            
            if (bomSize > 0) {
                //The data contains a BOM header signature, and the specified format differs from the actual format, so report an error
                return false;
            }
            else {
                //If there is no BOM header signature, force subsequent data to be processed with inCharsetType                
                CharsetType checkCharsetType = GetDataCharset(data, length);
                if (inCharsetType != checkCharsetType) {
                    //The specified data stream encoding differs from the encoding detected in the actual file data
                    return false;
                }
                outCharsetType = inCharsetType;
            }
        }
    }    

    ASSERT(bomSize <= length);
    if (outCharsetType == CharsetType::UNKNOWN) {
        //If detection by the BOM header fails, detect the data stream
        outCharsetType = GetDataCharset(data, length);
        bomSize = 0;
    }
    const char* realData = data + bomSize;
    uint32_t realLen = length - bomSize;

    if (outCharsetType == CharsetType::ANSI) {
#ifdef DUILIB_BUILD_FOR_WIN
        result = StringConvert::MBCSToUnicode2(realData, realLen);
#else
        result = StringConvert::UTF8ToWString(std::string(realData, realLen));
#endif
    }
    else if (outCharsetType == CharsetType::UTF8) {
        result = StringConvert::UTF8ToWString(std::string(realData, realLen));
    }
    else if (outCharsetType == CharsetType::UTF16_LE) {
#if defined(WCHAR_T_IS_UTF16)
        result.assign((const wchar_t*)realData, realLen / sizeof(wchar_t));
#else
        result = StringConvert::UTF16ToUTF32((const DUTF16Char*)realData, realLen / sizeof(DUTF16Char));
#endif
    }
    else if (outCharsetType == CharsetType::UTF16_BE) {
        // Reverse the byte order of the current character and store it in the output LE-encoded string
        uint32_t dataSize = realLen / sizeof(uint16_t);
        result.reserve(dataSize + 1);
        const uint16_t* dataBE = (const uint16_t*)realData;
        for (uint32_t i = 0; i < dataSize; i++) {
            result.push_back((dataBE[i] >> 8) | (dataBE[i] << 8));
        }
    }
    else {
        return false;
    }
    return true;
}

bool StringCharset::IsValidateASCIIStream(const char* stream, uint32_t length)
{
    if ((length < 1) || (stream == nullptr)) {
        return false;
    }

    // Iterate over the stream
    for (uint32_t i = 0; i < length; i++) {
        // Check whether the current byte is an ASCII character
        if (stream[i] >= 0) {
            // If so, continue iterating
            continue;
        }
        else {
            // If not, return false
            return false;
        }
    }

    // If no non-ASCII character is found in the entire stream, return true
    return true;
}

bool StringCharset::IsValidateGBKStream(const char* stream, uint32_t length)
{
    if ((length < 1) || (stream == nullptr)) {
        return false;
    }
    unsigned char* s = (unsigned char*)stream;
    unsigned char* e = s + length;

    for (; s < e; s++) {
        if (*s < 0x80) {
            continue;
        }
        if (*s < 0x81 || 0xFE < *s) {
            break;
        }
        if (++s == e) {
            return false;
        }
        if (*s < 0x40 || 0xFE < *s) {
            break;
        }
    }
    return s == e;
}

bool StringCharset::IsValidateUTF8Stream(const char* stream, uint32_t length)
{
    if ((length < 1) || (stream == nullptr)) {
        return false;
    }
    // Iterate over each byte in the stream
    for (uint32_t i = 0; i < length; i++) {
        // If the current byte is an ASCII character, continue to the next byte
        if ((stream[i] & 0x80) == 0x00) {
            continue;
        }
        // If the current byte is the first byte of a multi-byte UTF-8 encoded character, compute the number of bytes of that character
        else if ((stream[i] & 0xE0) == 0xC0) {
            uint32_t numBytes = 2;
            // Check whether the following bytes are all valid bytes of this character
            for (uint32_t j = 1; j < numBytes; j++) {
                if ((i + j >= length) || ((stream[i + j] & 0xC0) != 0x80)) {
                    return false;
                }
            }
            // If all bytes are valid bytes of this character, continue to the next character
            i += numBytes - 1;
            continue;
        }
        else if ((stream[i] & 0xF0) == 0xE0) {
            uint32_t numBytes = 3;
            // Check whether the following bytes are all valid bytes of this character
            for (uint32_t j = 1; j < numBytes; ++j) {
                if ((i + j >= length) || ((stream[i + j] & 0xC0) != 0x80)) {
                    return false;
                }
            }
            // If all bytes are valid bytes of this character, continue to the next character
            i += numBytes - 1;
            continue;
        }
        else if ((stream[i] & 0xF8) == 0xF0) {
            uint32_t numBytes = 4;
            // Check whether the following bytes are all valid bytes of this character
            for (uint32_t j = 1; j < numBytes; ++j) {
                if ((i + j >= length) || ((stream[i + j] & 0xC0) != 0x80)) {
                    return false;
                }
            }
            // If all bytes are valid bytes of this character, continue to the next character
            i += numBytes - 1;
            continue;
        }
        // If the current byte is the first byte of a multi-byte UTF-8 encoded character, compute the number of bytes of that character
        else {
            return false;
        }
    }
    // If all bytes are valid UTF-8 encoding, return true
    return true;
}

bool StringCharset::IsValidateUTF16LEStream(const char* stream, uint32_t length)
{
    if ((length < 1) || (stream == nullptr)) {
        return false;
    }
    // In UTF-16LE encoding, each character occupies 2 bytes
    // Therefore, the length of the stream must be a multiple of 2
    if (length % 2 != 0) {
        return false;
    }

    // Iterate over each character in the stream
    for (uint32_t i = 0; i < length; i += 2) {
        // If the current character is the first character of a surrogate pair, check whether the next character is the second character of the surrogate pair
        if ((stream[i + 1] & 0xFC) == 0xD8) {
            if (((i + 3) < length) && ((stream[i + 3] & 0xFC) == 0xDC)) {
                // If it is a surrogate pair, continue to the next character
                i += 2;
                continue;
            }
            else {
                // If it is not a surrogate pair, return false
                return false;
            }
        }
        // If the current character is the second character of a surrogate pair, return false
        else if ((stream[i + 1] & 0xFC) == 0xDC) {
            return false;
        }
    }
    // If all characters are valid UTF-16LE encoding, return true
    return true;
}

bool StringCharset::IsValidateUTF16BEStream(const char* stream, uint32_t length)
{
    if ((length < 1) || (stream == nullptr)) {
        return false;
    }
    // In UTF-16BE encoding, each character occupies 2 bytes
    // Therefore, the length of the stream must be a multiple of 2
    if (length % 2 != 0) {
        return false;
    }
    // Iterate over each character in the stream
    for (uint32_t i = 0; i < length; i += 2) {
        if ((stream[i] & 0xFC) == 0xD8) {
            if (((i + 2) < length) && ((stream[i + 2] & 0xFC) == 0xDC)) {
                // If it is a surrogate pair, continue to the next character
                i += 2;
                continue;
            }
            else {
                // If it is not a surrogate pair, return false
                return false;
            }
        }
        // If the current character is the second character of a surrogate pair, return false
        else if ((stream[i] & 0xFC) == 0xDC) {
            return false;
        }
    }
    // If all characters are valid UTF-16BE encoding, return true
    return true;
}

}//namespace ui
