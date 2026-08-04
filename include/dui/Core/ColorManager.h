#ifndef UI_CORE_COLOR_MANAGER_H_
#define UI_CORE_COLOR_MANAGER_H_

#include "dui/Core/UiColor.h"
#include <unordered_map>
#include <string>

namespace ui 
{
/** Color value management container
*/
class DUI_API ColorMap
{
public:
    /** Add a color value
    * @param[in] strName The color name (e.g., white)
    * @param[in] strValue The specific color value (e.g., #FFFFFFFF)
    */
    void AddColor(const DString& strName, const DString& strValue);

    /** Add a color value
    * @param[in] strName The color name (e.g., white)
    * @param[in] argb The specific color value, represented in ARGB format
    */
    void AddColor(const DString& strName, UiColor argb);

    /** Get the specific value of a color by name
    * @param[in] strName The color name to get
    * @return Returns the color description value in ARGB format
    */
    UiColor GetColor(const DString& strName) const;

    /** Remove the specified color attribute
    * @param [in] strName The color name to remove
    */
    void RemoveColor(const DString& strName);

    /** Remove all color attributes
    */
    void RemoveAllColors();

private:
    /** Mapping between color names and color values
    */
    std::unordered_map<DString, UiColor> m_colorMap;
};

/** Color value management class
*/
class DUI_API ColorManager
{
public:
    ColorManager();

    /** Convert a color value string to an ARGB color class
     * @param[in] strColor Supports two kinds of color values:
                  (1) The specific color value (e.g., #FFFFFFFF)
                  (2) The built-in standard color value, such as "blue", see the definition in the ui::UiColors::UiColorConsts function
     */
    static UiColor ConvertToUiColor(const DString& strColor);

public:
    /** Add a global color value
     * @param[in] strName The color name (e.g., white)
     * @param[in] strValue The specific color value (e.g., #FFFFFFFF)
     */
    void AddColor(const DString& strName, const DString& strValue);

    /** Add a global color value
     * @param[in] strName The color name (e.g., white)
     * @param[in] argb The specific color value, represented in ARGB format
     */
    void AddColor(const DString& strName, UiColor argb);

    /** Get the specific value of a color by name
     * @param[in] strName The color name to get
     * @return Returns the color description value in ARGB format
     */
    UiColor GetColor(const DString& strName) const;

    /** Get the specific value of a standard color by name
     * @param[in] strName The color name to get, such as "blue", see the detailed list in the definition of the ui::UiColors::UiColorConsts function
     * @return Returns the color description value in ARGB format
     */
    UiColor GetStandardColor(const DString& strName) const;

    /** Remove all color attributes
     */
    void RemoveAllColors();

    /** Clear caches such as all color attributes
     */
    void Clear();

public:
    /** Get the font color in the default disabled state
     * @return The string representation of the default disabled state color, corresponding to the color value specified in global.xml
     */
    const DString& GetDefaultDisabledTextColor();

    /** Set the font color in the default disabled state
     */
    void SetDefaultDisabledTextColor(const DString& strColor);

    /** Get the default font color
     */
    const DString& GetDefaultTextColor();

    /** Set the default font color
     */
    void SetDefaultTextColor(const DString& strColor);

private:
    /** Mapping between color names and color values
    */
    ColorMap m_colorMap;

    /** Standard color value mapping table, see the list in the UiColors::GetUiColorsString function
    */
    ColorMap m_standardColorMap;

    /** The font color in the default disabled state
    */
    DString m_defaultDisabledTextColor;

    /** The font color in the default normal state
    */
    DString m_defaultTextColor;
};

} // namespace ui

#endif // UI_CORE_COLOR_MANAGER_H_
