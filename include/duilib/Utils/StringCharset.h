#ifndef UI_UTILS_STRING_CHARSET_H_
#define UI_UTILS_STRING_CHARSET_H_

#include "duilib/duilib_defs.h"
#include <string>

namespace ui
{
/** Definition of supported character sets
*/
enum class DUILIB_API CharsetType
{
    UNKNOWN,
    ANSI,
    UTF8,
    UTF16_LE,
    UTF16_BE
};

/** String character set helper class
*/
class DUILIB_API StringCharset
{   
public:
    /** Get the BOM header length of the specified encoding
    @param [in] charsetType The encoding type
    */
    static uint32_t GetBOMSize(CharsetType charsetType);
    
    /** Detect the character set type of the data, based only on the data's BOM header
    @param [in] data The start address of the data
    @param [in] length The length of the data
    @return Returns the character set type, or UNKNOWN if detection fails
    */
    static CharsetType GetDataCharsetByBOM(const char* data, uint32_t length);

    /** Detect the character set type of the data, based only on the data itself, without checking the BOM header
    @param [in] data The start address of the data
    @param [in] length The length of the data
    @return Returns the character set type, or UNKNOWN if detection fails
    */
    static CharsetType GetDataCharset(const char* data, uint32_t length);

    /** Convert stream data to a string; the type is detected by the BOM header first, and if that fails, the encoding type is detected from the data stream
    @param [in] data The start address of the data, whose encoding is unknown
    @param [in] length The length of the data
    @param [out] result Returns the Unicode string value
    */
    static bool GetDataAsString(const char* data, uint32_t length, std::wstring& result);

    /** Convert stream data to a string; the type is detected by the BOM header first, and if that fails, the encoding type is detected from the data stream
    @param [in] data The start address of the data, whose encoding is unknown
    @param [in] length The length of the data
    @param [in] inCharsetType The encoding used when reading the file; if UNKNOWN, the text encoding type is auto-detected
    @param [out] result Returns the Unicode string value
    */
    static bool GetDataAsString(const char* data, uint32_t length, CharsetType inCharsetType, std::wstring& result);

    /** Convert stream data to a string; the type is detected by the BOM header first, and if that fails, the encoding type is detected from the data stream
    @param [in] data The start address of the data, whose encoding is unknown
    @param [in] length The length of the data
    @param [out] result Returns the Unicode string value
    @param [out] outCharsetType The encoding type used when reading the file
    @param [out] bomSize The size of the detected Unicode BOM header when reading the file
    */
    static bool GetDataAsString(const char* data, uint32_t length, 
                                std::wstring& result, CharsetType& outCharsetType, uint32_t& bomSize);

     /** Convert stream data to a string; the type is detected by the BOM header first, and if that fails, the encoding type is detected from the data stream
    @param [in] data The start address of the data, whose encoding is unknown
    @param [in] length The length of the data
    @param [in] inCharsetType The encoding used when reading the file; if UNKNOWN, the text encoding type is auto-detected
    @param [out] result Returns the Unicode string value
    @param [out] outCharsetType The encoding type used when reading the file
    @param [out] bomSize The size of the detected Unicode BOM header when reading the file
    */
    static bool GetDataAsString(const char* data, uint32_t length, CharsetType inCharsetType,
                                std::wstring& result, CharsetType& outCharsetType, uint32_t& bomSize);

public:
    /** Used to verify whether the given char stream is ASCII encoded
    @param [in] stream: Pointer to the char stream
    #param [in] length: Length of the stream, in bytes
    */
    static bool IsValidateASCIIStream(const char* stream, uint32_t length);

    /** Used to verify whether the given char stream is GBK encoded
    @param [in] stream: Pointer to the char stream
    #param [in] length: Length of the stream, in bytes
    */
    static bool IsValidateGBKStream(const char* stream, uint32_t length);

    /** Used to verify whether the given char stream is UTF-8 encoded
    @param [in] stream: Pointer to the char stream
    #param [in] length: Length of the stream, in bytes
    */
    static bool IsValidateUTF8Stream(const char* stream, uint32_t length);

    /** Used to verify whether the given char stream is UTF16-LE encoded
    @param [in] stream: Pointer to the char stream
    #param [in] length: Length of the stream, in bytes
    */
    static bool IsValidateUTF16LEStream(const char* stream, uint32_t length);

    /** Used to verify whether the given char stream is UTF16-BE encoded
    @param [in] stream: Pointer to the char stream
    #param [in] length: Length of the stream, in bytes
    */
    static bool IsValidateUTF16BEStream(const char* stream, uint32_t length);
};

}//namespace ui

#endif //UI_UTILS_STRING_CHARSET_H_
