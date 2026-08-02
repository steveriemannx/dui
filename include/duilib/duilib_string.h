#ifndef DUILIB_STRING_H_
#define DUILIB_STRING_H_

#ifndef DUILIB_CONFIG_H_
    #error "需要先包含duilib_config.h头文件"
#endif

//String class
#include <string>
#include <cstring>
#include <cstdint>

/** Unicode version string macro definitions
*/
#if !defined(_T)
    #if defined (DUILIB_UNICODE)
        #define _T(x)   L##x
    #else
        #define _T(x)   x
    #endif
#endif

//Detect the wchar_t definition: the wchar_t size on each platform
#if defined(DUILIB_BUILD_FOR_WIN)
    //Windows: wchar_t is 2 bytes (UTF-16)
    #define WCHAR_T_IS_UTF16
#elif defined(DUILIB_BUILD_FOR_LINUX) || defined(DUILIB_BUILD_FOR_MACOS) || defined(DUILIB_BUILD_FOR_FREEBSD)
    #if defined(__GNUC__)
        #if defined(__WCHAR_MAX__)
            #if (__WCHAR_MAX__ == 0x7fffffff || __WCHAR_MAX__ == 0xffffffff)
                //Linux/macOS GCC: wchar_t is usually 4 bytes (UTF-32)
                #define WCHAR_T_IS_UTF32
            #elif (__WCHAR_MAX__ == 0x7fff || __WCHAR_MAX__ == 0xffff)
                // On Posix, we'll detect short wchar_t, but projects aren't guaranteed to
                // compile in this mode (in particular, Chrome doesn't). This is intended for
                // other projects using base who manage their own dependencies and make sure
                // short wchar works for them.
                //May be 2 bytes under certain special configurations
                #define WCHAR_T_IS_UTF16
            #endif
        #endif
    #endif
    
    //If not detected, handle by default
    #if !defined(WCHAR_T_IS_UTF16) && !defined(WCHAR_T_IS_UTF32)
        // By default (Linux/FreeBSD/macOS) wchar_t is 4 bytes
        #define WCHAR_T_IS_UTF32
    #endif
#else
    #error "不支持的平台，请添加对您的编译器的支持"
#endif

typedef char DUTF8Char;
#if defined(WCHAR_T_IS_UTF16)
    typedef wchar_t  DUTF16Char;
    typedef char32_t DUTF32Char;
#else
    typedef char16_t DUTF16Char;
    typedef wchar_t  DUTF32Char;
#endif

typedef std::basic_string<DUTF8Char> UTF8String;
typedef UTF8String U8String;

typedef std::basic_string_view<DUTF8Char> UTF8StringView;
typedef UTF8StringView U8StringView;

typedef std::basic_string<DUTF16Char> UTF16String;
typedef UTF16String U16String;

typedef std::basic_string_view<DUTF16Char> UTF16StringView;
typedef UTF16StringView U16StringView;

typedef std::basic_string<DUTF32Char> UTF32String;
typedef UTF32String U32String;

typedef std::basic_string_view<DUTF32Char> UTF32StringView;
typedef UTF32StringView U32StringView;

//String type definition (Unicode on Windows, Ansi on Linux)

/** Unicode version of the string, UTF16 encoded
*/
typedef std::wstring DStringW;

/** Ansi version of the String, UTF8 encoded
*/
typedef std::string  DStringA;

/** String type macro definitions
*/
#ifdef DUILIB_UNICODE
    //Unicode version: the data is of type wchar_t
    //UTF16 encoding on Windows; UTF32 encoding on Linux/macOS
    typedef std::wstring  DString;
#else
    //Multibyte encoding: the data is UTF8 encoded
    typedef std::string   DString;
#endif

// macOS-specific string handling helper functions
#if defined(DUILIB_BUILD_FOR_MACOS)
#include <CoreFoundation/CoreFoundation.h>

namespace duilib {
    // Converts CFStringRef to a UTF8 string
    inline DStringA CFStringToUTF8(CFStringRef cfStr) {
        if (!cfStr) return DStringA();
        
        CFIndex length = CFStringGetLength(cfStr);
        CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
        DStringA result;
        result.resize(maxSize);
        
        if (CFStringGetCString(cfStr, &result[0], maxSize, kCFStringEncodingUTF8)) {
            result.resize(strlen(result.c_str()));
            return result;
        }
        return DStringA();
    }
    
    // Converts a UTF8 string to CFStringRef
    inline CFStringRef UTF8ToCFString(const DStringA& utf8Str) {
        return CFStringCreateWithCString(kCFAllocatorDefault, 
                                       utf8Str.c_str(), 
                                       kCFStringEncodingUTF8);
    }
}
#endif // DUILIB_BUILD_FOR_MACOS

#endif //DUILIB_STRING_H_
