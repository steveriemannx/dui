#ifndef UI_RENDER_SKIA_FONT_H_
#define UI_RENDER_SKIA_FONT_H_

#include "dui/Render/IRender.h"

class SkFont;
class SkFontMgr;

namespace ui 
{

/** Implementation of the Skia font interface
*/
class Font_Skia: public IFont
{
public:
    explicit Font_Skia(std::shared_ptr<IFontMgr>& spFontMgr);
    Font_Skia(const Font_Skia&) = delete;
    Font_Skia& operator=(const Font_Skia&) = delete;
    virtual ~Font_Skia() override;

    /** Initialize the font (the font size is not DPI-adaptive internally)
    */
    virtual bool InitFont(const UiFont& fontInfo) override;

    /**@brief Get the font name
     */
    virtual DString FontName() const override { return m_uiFont.m_fontName.c_str(); }

    /**@brief Get the font size
     */
    virtual int32_t FontSize() const override { return m_uiFont.m_fontSize; }

    /**@brief Whether it is bold
     */
    virtual bool IsBold() const override { return m_uiFont.m_bBold; }

    /**@brief Font underline state
     */
    virtual bool IsUnderline() const override { return m_uiFont.m_bUnderline; }

    /**@brief Font italic state
     */
    virtual bool IsItalic() const override { return m_uiFont.m_bItalic; }

    /**@brief Font strikethrough state
     */
    virtual bool IsStrikeOut() const override { return m_uiFont.m_bStrikeOut; }

public:
    /** Get the font handle
    */
    const SkFont* GetFontHandle();

private:
    /** Delete the Skia font
    */
    void ClearSkFont();

private:
    //Font information
    UiFont m_uiFont;

    //Font handle
    SkFont* m_skFont;

    //Font manager
    std::shared_ptr<IFontMgr> m_spFontMgr;
};

} // namespace ui

#endif // UI_RENDER_SKIA_FONT_H_
