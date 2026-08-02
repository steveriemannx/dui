#ifndef UI_UTILS_STRING_CONVERT_H_
#define UI_UTILS_STRING_CONVERT_H_

#include "duilib/duilib_defs.h"

namespace ui
{
/** String encoding conversion helper class
*/
class DUILIB_API StringConvert
{
public:
    //Convert a UTF8 string to UTF16
    static std::basic_string<DUTF16Char> UTF8ToUTF16(const DUTF8Char* utf8, size_t length);

    //Convert a UTF8 string to DStringW
    static DStringW UTF8ToWString(const std::string& utf8);

    //Convert a UTF16 string to a UTF8 string
    static std::string UTF16ToUTF8(const DUTF16Char* utf16, size_t length);

    //Convert a DStringW string to a UTF8 string
    static std::string WStringToUTF8(const DStringW& wstr);

    //Convert a DString to a UTF8 string
    static std::string TToUTF8(const std::wstring& str);
    static const std::string& TToUTF8(const std::string& str);

    //Convert a UTF8 string to DString
    static DString UTF8ToT(const std::string& utf8);
    static DString UTF8ToT(const DUTF8Char* utf8, size_t length);

    //Convert a DString to DStringW
    static const DStringW& TToWString(const std::wstring& str);
    static DStringW TToWString(const std::string& str);

    //Convert a DStringW to DString
#ifdef DUILIB_UNICODE
    static const DString& WStringToT(const std::wstring& wstr);
#else
    static DString WStringToT(const std::wstring& wstr);
#endif

    //Convert UTF8 to a UTF32 string
    static std::basic_string<DUTF32Char> UTF8ToUTF32(const DUTF8Char* utf8, size_t length);
    static std::basic_string<DUTF32Char> UTF8ToUTF32(const std::string& utf8);

    //Convert UTF32 to a UTF8 string
    static std::string UTF32ToUTF8(const DUTF32Char* utf32, size_t length);
    static std::string UTF32ToUTF8(const std::basic_string<DUTF32Char>& utf32);

    //Convert UTF16 to a UTF32 string
    static std::basic_string<DUTF32Char> UTF16ToUTF32(const DUTF16Char* utf16, size_t length);

    //Convert a DStringW string to a UTF32 string
    static std::basic_string<DUTF32Char> WStringToUTF32(const DStringW& wstr);

    //Convert a UTF32 string to DStringW
    static DStringW UTF32ToWString(const DUTF32Char* utf32, size_t length);
    static DStringW UTF32ToWString(const std::basic_string<DUTF32Char>& utf32);

#ifdef DUILIB_BUILD_FOR_WIN
    //Convert local Ansi encoding or UTF8 encoding, etc., to Unicode encoding
    static std::wstring MBCSToUnicode(const std::string& input, int32_t code_page = CP_ACP);
    static std::wstring MBCSToUnicode2(const char* input, size_t inputSize, int32_t code_page = CP_ACP);

    //Convert Unicode encoding to local Ansi encoding or UTF8 encoding, etc.
    static std::string UnicodeToMBCS(const std::wstring& input, int32_t code_page = CP_ACP);
    //Convert a local encoding string to UTF8 or UTF16 encoding
    static DString MBCSToT(const std::string& input);
    //Return the string encoding: local Ansi encoding
    static std::string TToMBCS(const DString& input);
#endif

    //When non-Unicode:
    //          input is UTF-8 encoded; on Windows platforms, return the local Ansi encoding, on non-Windows platforms return input
    //When Unicode: return input
#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_UNICODE)
    static DString TToLocal(const DString& input);
#else
    static const DString& TToLocal(const DString& input);
#endif

    //When non-Unicode:
    //          On Windows platforms: input is local Ansi encoding, return UTF-8 encoding
    //          On non-Windows platforms: input is local UTF-8 encoding, return UTF-8 encoding
    //When Unicode: return input
#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_UNICODE)
    static DString LocalToT(const DString& input);
#else
    static const DString& LocalToT(const DString& input);
#endif
};

} //namespace ui

#endif // UI_UTILS_STRING_CONVERT_H_
