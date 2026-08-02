#include "duilib/Core/ColorManager.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Utils/StringUtil.h"

namespace ui 
{
void ColorMap::AddColor(const DString& strName, const DString& strValue)
{
    ASSERT(!strName.empty() && !strValue.empty());
    if (strName.empty() || strValue.empty()) {
        return;
    }
    UiColor color = ColorManager::ConvertToUiColor(strValue);
    AddColor(strName, color);
}

void ColorMap::AddColor(const DString& strName, UiColor argb)
{
    ASSERT(!strName.empty() && (argb.GetARGB() != 0));
    if (strName.empty() || (argb.GetARGB() == 0)) {
        return;
    }
#ifdef _DEBUG
    // Check: avoid accidental modification
    auto iter = m_colorMap.find(strName);
    if (iter != m_colorMap.end()) {
        ASSERT(iter->second == argb);
    }
#endif
    m_colorMap[strName] = argb;
}

UiColor ColorMap::GetColor(const DString& strName) const
{
    auto it = m_colorMap.find(strName);
    if (it != m_colorMap.end()) {
        return it->second;
    }
    return UiColor();
}

void ColorMap::RemoveColor(const DString& strName)
{
    auto it = m_colorMap.find(strName);
    if (it != m_colorMap.end()) {
        m_colorMap.erase(it);
    }
}

void ColorMap::RemoveAllColors()
{
    m_colorMap.clear();
}

ColorManager::ColorManager()
{
    // Initialize the standard color table; strings are case-insensitive
    std::vector<std::pair<DString, int32_t>> uiColors;
    UiColors::GetUiColorsString(uiColors);
    for (auto iter : uiColors) {        
        m_standardColorMap.AddColor(StringUtil::MakeLowerString(iter.first), UiColor(iter.second));
    }
}

UiColor ColorManager::ConvertToUiColor(const DString& strColor)
{
    ASSERT(!strColor.empty());
    UiColor color;
    if (strColor.empty()) {
        return color;
    }
    if (strColor.at(0) != _T('#')) {
        // Get by the standard color value
        color = GlobalManager::Instance().Color().GetStandardColor(strColor);
        if (!color.IsEmpty()) {
            return color;
        }
    }

    // The specific color value, format like: #FFFFFFFF or #FFFFFF
    ASSERT((strColor.size() == 9) || (strColor.size() == 7));
    if ((strColor.size() != 9) && (strColor.size() != 7)) {
        return color;
    }
    ASSERT(strColor.at(0) == _T('#'));
    if (strColor.at(0) != _T('#')) {
        return color;
    }
    for (size_t i = 1; i < strColor.size(); ++i) {
        DString::value_type ch = strColor.at(i);
        bool isValid = (((ch >= _T('0')) && (ch <= _T('9'))) ||
            ((ch >= _T('a')) && (ch <= _T('f'))) ||
            ((ch >= _T('A')) && (ch <= _T('F'))));
        ASSERT(isValid);
        if (!isValid) {
            return color;
        }
    }
    DString colorValue = strColor.substr(1);
    if (colorValue.size() == 6) {
        // If it is in the #FFFFFF format, add the Alpha value automatically
        colorValue = _T("FF") + colorValue;
    }
    UiColor::ARGB argb = StringUtil::StringToUInt32(colorValue.c_str(), nullptr, 16);
    return UiColor(argb);
}

void ColorManager::AddColor(const DString& strName, const DString& strValue)
{
    m_colorMap.AddColor(strName, strValue);
}

void ColorManager::AddColor(const DString& strName, UiColor argb)
{
    m_colorMap.AddColor(strName, argb);
}

UiColor ColorManager::GetColor(const DString& strName) const
{
    return m_colorMap.GetColor(strName);
}

UiColor ColorManager::GetStandardColor(const DString& strName) const
{
    // The name is case-insensitive
    return m_standardColorMap.GetColor(StringUtil::MakeLowerString(strName));
}

void ColorManager::RemoveAllColors()
{
    m_colorMap.RemoveAllColors();
    m_defaultDisabledTextColor.clear();
    m_defaultTextColor.clear();
}

void ColorManager::Clear()
{
    RemoveAllColors();
    m_standardColorMap.RemoveAllColors();
}

const DString& ColorManager::GetDefaultDisabledTextColor()
{
    return m_defaultDisabledTextColor;
}

void ColorManager::SetDefaultDisabledTextColor(const DString& strColor)
{
    m_defaultDisabledTextColor = strColor;
}

const DString& ColorManager::GetDefaultTextColor()
{
    return m_defaultTextColor;
}

void ColorManager::SetDefaultTextColor(const DString& strColor)
{
    m_defaultTextColor = strColor;
}

} // namespace ui

