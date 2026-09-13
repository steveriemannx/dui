#include "dui/Core/ColorManager.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Utils/StringUtil.h"

namespace ui 
{
void ColorMap::AddColor(const std::string& strName, const std::string& strValue)
{
    ASSERT(!strName.empty() && !strValue.empty());
    if (strName.empty() || strValue.empty()) {
        return;
    }
    UiColor color = ColorManager::ConvertToUiColor(strValue);
    AddColor(strName, color);
}

void ColorMap::AddColor(const std::string& strName, UiColor argb)
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

UiColor ColorMap::GetColor(const std::string& strName) const
{
    auto it = m_colorMap.find(strName);
    if (it != m_colorMap.end()) {
        return it->second;
    }
    return UiColor();
}

void ColorMap::RemoveColor(const std::string& strName)
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
    std::vector<std::pair<std::string, int32_t>> uiColors;
    UiColors::GetUiColorsString(uiColors);
    for (auto iter : uiColors) {        
        m_standardColorMap.AddColor(StringUtil::MakeLowerString(iter.first), UiColor(iter.second));
    }
}

UiColor ColorManager::ConvertToUiColor(const std::string& strColor)
{
    ASSERT(!strColor.empty());
    UiColor color;
    if (strColor.empty()) {
        return color;
    }
    if (strColor.at(0) != '#') {
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
    ASSERT(strColor.at(0) == '#');
    if (strColor.at(0) != '#') {
        return color;
    }
    for (size_t i = 1; i < strColor.size(); ++i) {
        std::string::value_type ch = strColor.at(i);
        bool isValid = (((ch >= '0') && (ch <= '9')) ||
            ((ch >= 'a') && (ch <= 'f')) ||
            ((ch >= 'A') && (ch <= 'F')));
        ASSERT(isValid);
        if (!isValid) {
            return color;
        }
    }
    std::string colorValue = strColor.substr(1);
    if (colorValue.size() == 6) {
        // If it is in the #FFFFFF format, add the Alpha value automatically
        colorValue = "FF" + colorValue;
    }
    UiColor::ARGB argb = StringUtil::StringToUInt32(colorValue.c_str(), nullptr, 16);
    return UiColor(argb);
}

void ColorManager::AddColor(const std::string& strName, const std::string& strValue)
{
    m_colorMap.AddColor(strName, strValue);
}

void ColorManager::AddColor(const std::string& strName, UiColor argb)
{
    m_colorMap.AddColor(strName, argb);
}

UiColor ColorManager::GetColor(const std::string& strName) const
{
    return m_colorMap.GetColor(strName);
}

UiColor ColorManager::GetStandardColor(const std::string& strName) const
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

const std::string& ColorManager::GetDefaultDisabledTextColor()
{
    return m_defaultDisabledTextColor;
}

void ColorManager::SetDefaultDisabledTextColor(const std::string& strColor)
{
    m_defaultDisabledTextColor = strColor;
}

const std::string& ColorManager::GetDefaultTextColor()
{
    return m_defaultTextColor;
}

void ColorManager::SetDefaultTextColor(const std::string& strColor)
{
    m_defaultTextColor = strColor;
}

} // namespace ui

