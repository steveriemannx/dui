#ifndef UI_UTILS_ATTRIBUTUTIL_H_
#define UI_UTILS_ATTRIBUTUTIL_H_

#include "duilib/Core/UiTypes.h"
#include <string>
#include <vector>
#include <tuple>

namespace ui
{
class Window;
class DUILIB_API AttributeUtil
{
public:
    /** Configuration string parsing: skip a separator character
    */
    static void SkipSepChar(wchar_t*& pPtr);
    static void SkipSepChar(char*& pPtr);

    /** Parse a Size structure from the configuration; the input format is like: "800,500"
    */
    static void ParseSizeValue(const wchar_t* strValue, UiSize& size);
    static void ParseSizeValue(const char* strValue, UiSize& size);

    /** Parse a Point structure from the configuration; the input format is like: "800,500"
    */
    static void ParsePointValue(const wchar_t* strValue, UiPoint& size);
    static void ParsePointValue(const char* strValue, UiPoint& size);

    /** Parse a Rect structure from the configuration; the input format is like: "80,50,60,70"
    */
    static void ParseRectValue(const wchar_t* strValue, UiRect& rect, bool bCheckSize = true);
    static void ParsePaddingValue(const wchar_t* strValue, UiPadding& padding);
    static void ParseMarginValue(const wchar_t* strValue, UiMargin& margin);

    static void ParseRectValue(const char* strValue, UiRect& rect, bool bCheckSize = true);
    static void ParsePaddingValue(const char* strValue, UiPadding& padding);
    static void ParseMarginValue(const char* strValue, UiMargin& margin);

    /** Parse an attribute list; the format is like: font="system_bold_14" normaltextcolor="white" bkcolor="red"
    *                  or: color='black' offset='1,1' blur_radius='2' spread_radius='2'
    *   The separator can be double quotes or single quotes, passed in via the parameter
    * @param [in] seperateChar The string separator, which can be _T('\"') or _T('\')'
    */
    static void ParseAttributeList(const DString& strList,
                                   DString::value_type seperateChar,
                                   std::vector<std::pair<DString, DString>>& attributeList);

    /** Parse a string (the format is: "500," or "50%,"; the comma is optional and can also be other characters), and obtain an integer value or a floating-point number
    * @param [in] strValue The address of the string to be parsed
    * @param [out] pEndPtr After parsing, the end address of the string, used to continue parsing the remaining content
    * @return Returns the integer value or percentage; in the return value, 0 or 0.0f indicates an invalid value
    */
    static std::tuple<int32_t, float> ParseString(const wchar_t* strValue, wchar_t** pEndPtr);
    static std::tuple<int32_t, float> ParseString(const char* strValue, char** pEndPtr);

    /** Get the window size from the configuration
    * @param [in] pWindow The associated window, which can be nullptr
    * @param [in] strValue The window size string to be parsed
    * @param [out] size The parsed window size value (in pixels)
    * @param [out] pScaledCX Returns whether the window width size.cx value has been DPI-scaled
    * @param [out] pScaledCY Returns whether the window height size.cy value has been DPI-scaled
    * @param [out] pPercentCX Returns whether the window width size.cx value is configured as a screen percentage
    * @param [out] pPercentCY Returns whether the window height size.cy value is configured as a screen percentage
    */
    static void ParseWindowSize(const Window* pWindow, const DString::value_type* strValue,
                                UiSize& size,
                                bool* pScaledCX, bool* pScaledCY,
                                bool* pPercentCX, bool* pPercentCY);

    /** Validate the window size to avoid exceeding the area of the screen where the window is located
    * @param [in] pWindow The associated window, which can be nullptr
    * @param [in,out] nWindowWidth The window width
    * @param [in,out] nWindowHeight The window height
    */
    static void ValidateWindowSize(const Window* pWindow, int32_t& nWindowWidth, int32_t& nWindowHeight);
};

} //namespace ui

#endif // UI_UTILS_ATTRIBUTUTIL_H_
