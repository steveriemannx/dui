#include "dui/Utils/AttributeUtil.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Core/Window.h"
#include "dui/Core/Control.h"
#include "dui/Core/GlobalManager.h"

namespace ui
{

void AttributeUtil::SkipSepChar(wchar_t*& pPtr)
{
    if ((pPtr != nullptr) && (*pPtr != L'\0')) {
        // Skip the separator character, but avoid skipping the trailing null character to prevent going out of bounds
        ++pPtr;
    }
}

void AttributeUtil::SkipSepChar(char*& pPtr)
{
    if ((pPtr != nullptr) && (*pPtr != '\0')) {
        // Skip the separator character, but avoid skipping the trailing null character to prevent going out of bounds
        ++pPtr;
    }
}

void AttributeUtil::ParseSizeValue(const wchar_t* strValue, UiSize& size)
{
    size.Clear();
    if ((strValue == nullptr) || (*strValue == L'\0')) {
        return;
    }
    wchar_t* pstr = nullptr;
    int32_t cx = StringUtil::StringToInt32(strValue, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    int32_t cy = StringUtil::StringToInt32(pstr, &pstr, 10);
    size.cx = cx;
    size.cy = cy;
}

void AttributeUtil::ParseSizeValue(const char* strValue, UiSize& size)
{
    size.Clear();
    if ((strValue == nullptr) || (*strValue == '\0')) {
        return;
    }
    char* pstr = nullptr;
    int32_t cx = StringUtil::StringToInt32(strValue, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    int32_t cy = StringUtil::StringToInt32(pstr, &pstr, 10);
    size.cx = cx;
    size.cy = cy;
}

void AttributeUtil::ParsePointValue(const wchar_t* strValue, UiPoint& pt)
{
    UiSize size;
    AttributeUtil::ParseSizeValue(strValue, size);
    pt.x = size.cx;
    pt.y = size.cy;
}

void AttributeUtil::ParsePointValue(const char* strValue, UiPoint& pt)
{
    UiSize size;
    AttributeUtil::ParseSizeValue(strValue, size);
    pt.x = size.cx;
    pt.y = size.cy;
}

void AttributeUtil::ParsePaddingValue(const wchar_t* strValue, UiPadding& padding)
{
    padding.Clear();
    if ((strValue == nullptr) || (*strValue == L'\0')) {
        return;
    }
    wchar_t* pstr = nullptr;
    padding.left = StringUtil::StringToInt32(strValue, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    padding.top = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    padding.right = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    padding.bottom = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    ASSERT((padding.left >= 0) && (padding.top >= 0) && (padding.right >= 0) && (padding.bottom >= 0));
    padding.Validate();
}

void AttributeUtil::ParsePaddingValue(const char* strValue, UiPadding& padding)
{
    padding.Clear();
    if ((strValue == nullptr) || (*strValue == '\0')) {
        return;
    }
    char* pstr = nullptr;
    padding.left = StringUtil::StringToInt32(strValue, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    padding.top = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    padding.right = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    padding.bottom = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    ASSERT((padding.left >= 0) && (padding.top >= 0) && (padding.right >= 0) && (padding.bottom >= 0));
    padding.Validate();
}

void AttributeUtil::ParseMarginValue(const wchar_t* strValue, UiMargin& margin)
{
    margin.Clear();
    if ((strValue == nullptr) || (*strValue == L'\0')) {
        return;
    }
    wchar_t* pstr = nullptr;
    margin.left = StringUtil::StringToInt32(strValue, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    margin.top = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    margin.right = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    margin.bottom = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    ASSERT((margin.left >= 0) && (margin.top >= 0) && (margin.right >= 0) && (margin.bottom >= 0));
    margin.Validate();
}

void AttributeUtil::ParseMarginValue(const char* strValue, UiMargin& margin)
{
    margin.Clear();
    if ((strValue == nullptr) || (*strValue == '\0')) {
        return;
    }
    char* pstr = nullptr;
    margin.left = StringUtil::StringToInt32(strValue, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    margin.top = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    margin.right = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    margin.bottom = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    ASSERT((margin.left >= 0) && (margin.top >= 0) && (margin.right >= 0) && (margin.bottom >= 0));
    margin.Validate();
}

void AttributeUtil::ParseRectValue(const wchar_t* strValue, UiRect& rect, bool bCheckSize)
{
    rect.Clear();
    if ((strValue == nullptr) || (*strValue == L'\0')) {
        return;
    }
    wchar_t* pstr = nullptr;
    rect.left = StringUtil::StringToInt32(strValue, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    rect.top = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    rect.right = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    rect.bottom = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    if (bCheckSize) {
        ASSERT((rect.Width() >= 0) && (rect.Height() >= 0));
    }    
}

void AttributeUtil::ParseRectValue(const char* strValue, UiRect& rect, bool bCheckSize)
{
    rect.Clear();
    if ((strValue == nullptr) || (*strValue == '\0')) {
        return;
    }
    char* pstr = nullptr;
    rect.left = StringUtil::StringToInt32(strValue, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    rect.top = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    rect.right = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    SkipSepChar(pstr);
    rect.bottom = StringUtil::StringToInt32(pstr, &pstr, 10); ASSERT(pstr);
    if (bCheckSize) {
        ASSERT((rect.Width() >= 0) && (rect.Height() >= 0));
    }
}

void AttributeUtil::ParseAttributeList(const DString& strList,
                                       DString::value_type seperateChar,
                                       std::vector<std::pair<DString, DString>>& attributeList)
{
    // Example: normal_image="file='public/button/window-minimize.svg' width='24' height='24' valign='center' halign='center'" hot_color="AliceBlue" pushed_color="Lavender"
    DString sName;
    DString sValue;
    const DString::value_type* pstrList = strList.c_str();
    if (pstrList == nullptr) {
        return;
    }
    while (*pstrList != _T('\0')) {
        sName.clear();
        sValue.clear();
        // Read the content before the equals sign as the Name
        while (*pstrList != _T('\0') && *pstrList != _T('=')) {
            sName += *pstrList++;
        }
        // The current character should be an equals sign
        ASSERT(*pstrList == _T('='));
        if (*pstrList != _T('=')) {
            return;
        }
        // Skip to the character after the equals sign; this character should be a separator character
        pstrList++;
        ASSERT(*pstrList == seperateChar);
        if (*pstrList != seperateChar) {
            return;
        }

        // Skip to the character after the first separator character and read the attribute value
        pstrList++;
        while (*pstrList != _T('\0') && *pstrList != seperateChar) {
            sValue += *pstrList++;
        }
        ASSERT(*pstrList == seperateChar);
        if (*pstrList != seperateChar) {
            return;
        }

        // An attribute has been parsed; add it to the list (attribute names are not allowed to contain spaces)
        sName = StringUtil::Trim(sName);
        attributeList.push_back(std::make_pair(sName, sValue));

        // Skip to the character after the separator character; it should be a space. If it is not a space, the parsing is considered finished
        pstrList++;
        if (*pstrList != _T(' ')) {
            return;
        }

        // Skip to the character after the space
        pstrList++;
    }
}

std::tuple<int32_t, float> AttributeUtil::ParseString(const wchar_t* strValue, wchar_t** pEndPtr)
{
    wchar_t* pstr = nullptr;
    if ((strValue == nullptr) || (*strValue == L'\0')) {
        if (pEndPtr != nullptr) {
            *pEndPtr = pstr;
        }
        return std::tuple<int32_t, float>(0, 0.0f);
    }
    int32_t xValue = 0;
    float xPercent = StringUtil::StringToFloat(strValue, &pstr);
    ASSERT(pstr != nullptr);
    if ((pstr != nullptr) && (*pstr == L'%')) {
        // The value is a percentage; skip the '%' character
        pstr++;
    }
    else {
        // Not a percentage, but an integer value
        xPercent = 0.0f;
        xValue = StringUtil::StringToInt32(strValue, &pstr, 10);
        ASSERT(pstr != nullptr);
    }
    if (pEndPtr != nullptr) {
        *pEndPtr = pstr;
    }
    return std::tuple<int32_t, float>(xValue, xPercent);
}

std::tuple<int32_t, float> AttributeUtil::ParseString(const char* strValue, char** pEndPtr)
{
    char* pstr = nullptr;
    if ((strValue == nullptr) || (*strValue == '\0')) {
        if (pEndPtr != nullptr) {
            *pEndPtr = pstr;
        }
        return std::tuple<int32_t, float>(0, 0.0f);
    }
    int32_t xValue = 0;
    float xPercent = StringUtil::StringToFloat(strValue, &pstr);
    ASSERT(pstr != nullptr);
    if ((pstr != nullptr) && (*pstr == '%')) {
        // The value is a percentage; skip the '%' character
        pstr++;
    }
    else {
        // Not a percentage, but an integer value
        xPercent = 0.0f;
        xValue = StringUtil::StringToInt32(strValue, &pstr, 10);
        ASSERT(pstr != nullptr);
    }
    if (pEndPtr != nullptr) {
        *pEndPtr = pstr;
    }
    return std::tuple<int32_t, float>(xValue, xPercent);
}

void AttributeUtil::ParseWindowSize(const Window* pWindow, const DString::value_type* strValue,
                                    UiSize& size,
                                    bool* pScaledCX, bool* pScaledCY,
                                    bool* pPercentCX, bool* pPercentCY)
{
    // Supported formats: size="1200,800", or size="50%,50%", or size="1200,50%", size="50%,800"
    // The percentage refers to the percentage of the screen width or height
    UiRect rcWork;
    if (pWindow != nullptr) {
        pWindow->GetMonitorWorkRect(rcWork);
    }
    else {
        WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    }
    
    DString::value_type* pstr = nullptr;
    std::tuple<int32_t, float> x = ParseString(strValue, &pstr);
    AttributeUtil::SkipSepChar(pstr);
    std::tuple<int32_t, float> y = ParseString(pstr, &pstr);

    // Get the valid percentage value
    auto GetValidPercent = [](const std::tuple<int32_t, float>& xy) -> float {
        float fPercent = std::get<1>(xy);
        fPercent /= 100.0f;
        ASSERT((fPercent >= 0.0001f) && (fPercent < 1.0001f));
        if ((fPercent < 0.0001f) || (fPercent > 1.0001f)) {
            // If an invalid percentage is encountered, default to 75% of the window size
            fPercent = 0.75f;
        }
        return fPercent;
    };
    if (pPercentCX) {
        *pPercentCX = false;
    }
    if (pPercentCY) {
        *pPercentCY = false;
    }

    bool needScaleCX = true;
    bool needScaleCY = true;
    int cx = std::get<0>(x);
    if (cx <= 0) {
        float fPercent = GetValidPercent(x);
        cx = (int)(rcWork.Width() * fPercent);
        needScaleCX = false;
        if (pPercentCX) {
            *pPercentCX = true;
        }
    }
    int cy = std::get<0>(y);
    if (cy <= 0) {
        float fPercent = GetValidPercent(y);
        cy = (int)(rcWork.Height() * fPercent);
        needScaleCY = false;
        if (pPercentCY) {
            *pPercentCY = true;
        }
    }

    ASSERT((cx > 0) && (cy > 0));
    if (cx < 0) {
        cx = 0;
    }
    if (cy < 0) {
        cy = 0;
    }
    if (needScaleCX && (pWindow != nullptr)) {
        pWindow->Dpi().ScaleWindowSize(cx);
    }
    if (needScaleCY && (pWindow != nullptr)) {
        pWindow->Dpi().ScaleWindowSize(cy);
    }
    if (!rcWork.IsEmpty()) {
        if (cx > rcWork.Width()) {
            cx = rcWork.Width();
        }
        if (cy > rcWork.Height()) {
            cy = rcWork.Height();
        }
    }
    size.cx = cx;
    size.cy = cy;
    if (pScaledCX) {
        *pScaledCX = (pWindow != nullptr) ? true : !needScaleCX;
    }
    if (pScaledCY) {
        *pScaledCY = (pWindow != nullptr) ? true : !needScaleCY;
    }
}

void AttributeUtil::ValidateWindowSize(const Window* pWindow, int32_t& nWindowWidth, int32_t& nWindowHeight)
{
    UiRect rcWork;
    if (pWindow != nullptr) {
        pWindow->GetMonitorWorkRect(rcWork);
    }
    else {
        WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    }
    // If the width is exceeded, use 95% of the screen to determine the size (to avoid filling the entire screen, which would make this state indistinguishable from the maximized state)
    if (nWindowWidth > rcWork.Width()) {
        nWindowWidth = (int32_t)(rcWork.Width() * 0.95);
    }
    if (nWindowHeight > rcWork.Height()) {
        nWindowHeight = (int32_t)(rcWork.Height() * 0.95);
    }
}

}//namespace ui
