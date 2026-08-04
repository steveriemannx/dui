#ifndef UI_RENDER_SKIA_FONT_MGR_H_
#define UI_RENDER_SKIA_FONT_MGR_H_

#include "dui/Render/IRender.h"

/** Skia font type
*/
class SkFont;

namespace ui 
{

/** Implementation of the font manager interface
*/
class FontMgr_Skia: public IFontMgr
{
public:
    explicit FontMgr_Skia();
    FontMgr_Skia(const FontMgr_Skia&) = delete;
    FontMgr_Skia& operator=(const FontMgr_Skia&) = delete;
    virtual ~FontMgr_Skia() override;

public:
    /** Get the number of fonts
    * @return Returns the number of fonts
    */
    virtual uint32_t GetFontCount() const override;

    /** Get the font name
    * @param [in] nIndex The index of the font: [0, GetFontCount())
    * @param [out] fontName Returns the font name
    * @return Returns true on success, false on failure
    */
    virtual bool GetFontName(uint32_t nIndex, DString& fontName) const override;

    /** Determine whether the font exists
    * @param [int] fontName The font name
    * @return Returns true if a font matching the font name exists, otherwise false
    */
    virtual bool HasFontName(const DString& fontName) const override;

    /** Set the default font name (used when the font to be loaded does not exist)
    * @param [in] fontName The default font name
    */
    virtual void SetDefaultFontName(const DString& fontName) override;

    /** Load the specified font file
    * @param [in] fontFilePath The path of the font file (local absolute path)
    * @return Returns true on success, false on failure
    */
    virtual bool LoadFontFile(const DString& fontFilePath) override;

    /** Load the specified font data
    * @param [in] data The in-memory data of the font file
    * @param [in] length The length of the in-memory font file data
    * @return Returns true on success, false on failure
    */
    virtual bool LoadFontFileData(const void* data, size_t length) override;

    /** Clear the loaded font files
    */
    virtual void ClearFontFiles() override;

    /** Clear the font cache
    */
    virtual void ClearFontCache() override;

public:
    /** Create a Skia font
    * @param [in] fontInfo The font attributes
    * @return On success, returns the Skia font pointer; DeleteSkFont must be called to delete the font the pointer points to
    */
    SkFont* CreateSkFont(const UiFont& fontInfo);

    /** Delete a Skia font
    */
    void DeleteSkFont(SkFont* pSkFont);

public:
    /** Returns a pointer to sk_sp<SkFontMgr> (&sk_sp<SkFontMgr>)
    */
    void* GetSkiaFontMgrPtr() const;

private:
    /** Internal implementation class
    */
    class TImpl;
    TImpl* m_impl;
};

} // namespace ui

#endif // UI_RENDER_SKIA_FONT_MGR_H_
