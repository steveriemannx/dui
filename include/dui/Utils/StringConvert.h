#ifndef UI_UTILS_STRING_CONVERT_H_
#define UI_UTILS_STRING_CONVERT_H_

#include "dui/dui_defs.h"

namespace ui
{
/** String encoding conversion helper class
*/
class DUI_API StringConvert
{
public:
    //Convert a UTF8 string to UTF16
    static std::basic_string<DUTF16Char> UTF8ToUTF16(const DUTF8Char* utf8, size_t length);

    //Convert a UTF8 string to std::wstring
    static std::wstring UTF8ToWString(const std::string& utf8);

    //Convert a UTF16 string to a UTF8 string
    static std::string UTF16ToUTF8(const DUTF16Char* utf16, size_t length);

    //Convert a std::wstring string to a UTF8 string
    static std::string WStringToUTF8(const std::wstring& wstr);

    //Convert a std::string to a UTF8 string
    static std::string TToUTF8(const std::wstring& str);
    static const std::string& TToUTF8(const std::string& str);

    //Convert a UTF8 string to std::string
    static std::string UTF8ToT(const std::string& utf8);
    static std::string UTF8ToT(const DUTF8Char* utf8, size_t length);

    //Convert a std::string to std::wstring
    static const std::wstring& TToWString(const std::wstring& str);
    static std::wstring TToWString(const std::string& str);

    //Convert a std::wstring to std::string
    static std::string WStringToT(const std::wstring& wstr);

    //Convert UTF8 to a UTF32 string
    static std::basic_string<DUTF32Char> UTF8ToUTF32(const DUTF8Char* utf8, size_t length);
    static std::basic_string<DUTF32Char> UTF8ToUTF32(const std::string& utf8);

    //Convert UTF32 to a UTF8 string
    static std::string UTF32ToUTF8(const DUTF32Char* utf32, size_t length);
    static std::string UTF32ToUTF8(const std::basic_string<DUTF32Char>& utf32);

    //Convert UTF16 to a UTF32 string
    static std::basic_string<DUTF32Char> UTF16ToUTF32(const DUTF16Char* utf16, size_t length);

    //Convert a std::wstring string to a UTF32 string
    static std::basic_string<DUTF32Char> WStringToUTF32(const std::wstring& wstr);

    //Convert a UTF32 string to std::wstring
    static std::wstring UTF32ToWString(const DUTF32Char* utf32, size_t length);
    static std::wstring UTF32ToWString(const std::basic_string<DUTF32Char>& utf32);

#ifdef DUI_BUILD_FOR_WIN
    //Convert local Ansi encoding or UTF8 encoding, etc., to Unicode encoding
    static std::wstring MBCSToUnicode(const std::string& input, int32_t code_page = CP_ACP);
    static std::wstring MBCSToUnicode2(const char* input, size_t inputSize, int32_t code_page = CP_ACP);

    //Convert Unicode encoding to local Ansi encoding or UTF8 encoding, etc.
    static std::string UnicodeToMBCS(const std::wstring& input, int32_t code_page = CP_ACP);
    //Convert a local encoding string to UTF8 or UTF16 encoding
    static std::string MBCSToT(const std::string& input);
    //Return the string encoding: local Ansi encoding
    static std::string TToMBCS(const std::string& input);
#endif

    //When non-Unicode:
    //          input is UTF-8 encoded; on Windows platforms, return the local Ansi encoding, on non-Windows platforms return input
    //When Unicode: return input
#if defined (DUI_BUILD_FOR_WIN)
    static std::string TToLocal(const std::string& input);
#else
    static const std::string& TToLocal(const std::string& input);
#endif

    //When non-Unicode:
    //          On Windows platforms: input is local Ansi encoding, return UTF-8 encoding
    //          On non-Windows platforms: input is local UTF-8 encoding, return UTF-8 encoding
    //When Unicode: return input
#if defined (DUI_BUILD_FOR_WIN)
    static std::string LocalToT(const std::string& input);
#else
    static const std::string& LocalToT(const std::string& input);
#endif
};

} //namespace ui

#endif // UI_UTILS_STRING_CONVERT_H_
