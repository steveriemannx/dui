#ifndef UI_CORE_FONTMANAGER_H_
#define UI_CORE_FONTMANAGER_H_

#include "dui/Core/UiFont.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace ui 
{
class IFont;
class DpiManager;

/** Font size information
*/
struct FontSizeInfo
{
    DString fontSizeName;   //The display name of the font size, e.g. "No.5"
    float fFontSize = 0;    //Unit: pixels, without DPI adaptation
    float fDpiFontSize = 0; //Unit: pixels, with DPI adaptation
};

/** The font manager
*/
class DUI_API FontManager
{
public:
    FontManager();
    ~FontManager();
    FontManager(const FontManager&) = delete;
    FontManager& operator = (const FontManager&) = delete;

public:
    /** Add a font info, the font size is not DPI processed
     * @param [in] fontId The ID marker of the specified font
     * @param [in] fontInfo The font attribute info, the font size is the original size, not DPI scaled
     * @param [in] bDefault Whether to set it as the default font
     */
    bool AddFont(const DString& fontId, const UiFont& fontInfo, bool bDefault);

    /** Get the font interface; if no font interface is found by fontId, the m_defaultFontId font interface is searched instead
    * @param [in] fontId The font ID
    * @param [in] dpi The DPI scaling manager, used to scale the font size
    * @return Returns the font interface on success, external callers do not need to release the resource; returns nullptr on failure
    */
    IFont* GetIFont(const DString& fontId, const DpiManager& dpi);

    /** Get the font interface; if no font interface is found by fontId, the m_defaultFontId font interface is searched instead
    * @param [in] fontId The font ID
    * @param [in] nZoomPercent The font size scale percentage, used to scale the font size, e.g. 100 means 100%, 200 means 200%
    * @return Returns the font interface on success, external callers do not need to release the resource; returns nullptr on failure
    */
    IFont* GetIFont(const DString& fontId, uint32_t nZoomPercent);

    /** Whether the font ID is present
    * @param [in] fontId The ID marker of the specified font
    */
    bool HasFontId(const DString& fontId) const;

    /** Remove a font ID
    * @param [in] fontId The ID marker of the specified font
    */
    bool RemoveFontId(const DString& fontId);

    /** Remove a font cache entry to release memory
    * @param [in] fontId The font ID
    * @param [in] nZoomPercent The font size scale percentage, used to scale the font size, e.g. 100 means 100%, 200 means 200%
    */
    bool RemoveIFont(const DString& fontId, uint32_t nZoomPercent);

    /** Remove all fonts, not including the already loaded font files
     */
    void RemoveAllFonts();

    /** Get the default font ID
    */
    const DString& GetDefaultFontId() const;

    /** Set the default font family list
    * @param [in] defaultFontFamilyNames The font list, different fonts separated by commas, e.g. "Microsoft YaHei,SimSun"
    */
    void SetDefaultFontFamilyNames(const DString& defaultFontFamilyNames);

public:
    /** @brief Add a font file, after which it can be used as a normal font
      * @param[in] strFontFile The font file name, relative path, the font file is saved in the directory: "<resource path>\font\"
      * @param[in] strFontDesc The font description info
      * @return No return value
      */
    bool AddFontFile(const DString& strFontFile, const DString& strFontDesc);

    /** @brief Clear all added font files
      * @return No return value
      */
    void RemoveAllFontFiles();

public:
    /** Get the list of available font names
    * @param [out] fontNameList Returns the list of available font names
    */
    void GetFontNameList(std::vector<DString>& fontNameList) const;

    /** Get the font size list
    * @param [in] dpi The DPI scaling manager, used to scale the font size
    * @param [out] fontSizeList The font size info
    */
    void GetFontSizeList(const DpiManager& dpi, std::vector<FontSizeInfo>& fontSizeList) const;

private:
    /** Get the actual font ID after DPI scaling
    */
    DString GetDpiFontId(const DString& fontId, uint32_t nZoomPercent) const;

private:
    /** Custom font data: Key is FontID, Value is the font description info
    */
    std::unordered_map<DString, UiFont> m_fontIdMap;

    /** Custom font info: Key is FontId
    */
    std::unordered_map<DString, IFont*> m_fontMap;

    /** The default font ID
    */
    DString m_defaultFontId;

    /** The default font family list
    */
    std::vector<DString> m_defaultFontFamilyNames;

    /** Whether the default font family list has been initialized
    */
    bool m_bDefaultFontInited;
};

}
#endif //UI_CORE_FONTMANAGER_H_
