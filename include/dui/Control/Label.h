#ifndef UI_CONTROL_LABEL_H_
#define UI_CONTROL_LABEL_H_

#include "dui/Control/LabelImpl.h"
#include "dui/Core/DpiManager.h"
#include "dui/Box/HBox.h"
#include "dui/Box/VBox.h"

namespace ui
{
/** Label control (template), used to display text
*/
template<typename T = Control>
class LabelTemplate : public T, public LabelOwner
{
    typedef T BaseClass;
public:
    explicit LabelTemplate(Window* pWindow);
    virtual ~LabelTemplate() override;

    /// Override the parent class methods to provide customized functionality; refer to the parent class declarations
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void SetPos(UiRect rc) override;
    virtual void SetWindow(Window* pWindow) override;
    virtual void PaintText(IRender* pRender) override;
    virtual bool HasHotState() override;
    virtual DString GetToolTipText() const override;
    virtual void OnLanguageChanged() override;
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Calculate the size of the text area (width and height)
     *  @param [in] szAvailable The available size, excluding padding and margins
     *  @return The estimated text size of the control, including padding (Box), excluding margins
     */
    virtual UiSize EstimateText(UiSize szAvailable) override;

public:
    /** Get the text content
    */
    virtual DString GetText() const override;

    /** Set the text content
    * @param [in] strText The text content
    */
    virtual void SetText(const DString& strText) override;

    /** Get the text content ID (supports multiple languages)
    */
    virtual DString GetTextId() const override;

    /** Set the text content ID (supports multiple languages)
    * @param [in] strTextId The ID of the text content
    */
    virtual void SetTextId(const DString& strTextId) override;

    /** Get the text content (UTF8 format)
    */
    std::string GetUTF8Text() const;

    /** Set the text content (UTF8 format)
    * @param [in] strText The text content in UTF8 format
    */
    void SetUTF8Text(const std::string& strText);

    /** Get the text content ID (UTF8 format)
    */
    std::string GetUTF8TextId() const;

    /** Set the text content ID (UTF8 format)
    */
    void SetUTF8TextId(const std::string& strTextId);

    /** Set whether the text content is RichText
     * @param [in] bRichText Indicates whether RichText mode is supported; the set text content can be in RichText format
     *             Example: "a simple <b>window</b><br/>with a <u>title bar</u> and <u>regular buttons</u>, <b>bold, <font color='#FF0000'>red font</font></b>"
     *             Note: In RichText mode, the following features are not supported:
     *                  (1) Justified alignment is not supported
     *                  (2) The vertical_text attribute is not supported (nor other vertical text related attributes)
     *                  (3) The end_ellipsis attribute is not supported
     *                  (4) The path_ellipsis attribute is not supported
     *                  (5) The auto_tooltip attribute is not supported
     *                  (6) The word_spacing attribute is not supported
     */
    void SetRichText(bool bRichText);

    /** Get whether the text content is RichText
    */
    bool IsRichText() const;

    /** Check whether the text is equal
    */
    bool IsTextEquals(const DString& text) const;

public:
    /** Restore the default text style
    * @param [in] bRedraw true means redraw, false means no redraw
    */
    void SetDefaultTextStyle(bool bRedraw);

    /** Set the text style
     * @param [in] uStyle The style to set
     * @param [in] bRedraw true means redraw, false means no redraw
     */
    void SetTextStyle(uint32_t uStyle, bool bRedraw);

    /** Get the text style
     * @return The text style
     */
    uint32_t GetTextStyle() const;

    /** Get the text color in the specified state
     * @param [in] stateType The state flag to get
     * @return The text color in the specified state
     */
    DString GetStateTextColor(ControlStateType stateType) const;

    /** Set the text color in the specified state
     * @param [in] stateType The state flag to set
     * @param [in] dwTextColor The state color string to set; the value must exist in global.xml
     * @return None
     */
    void SetStateTextColor(ControlStateType stateType, const DString& dwTextColor);

    /** Get the actually rendered text color in the specified state
     * @param [in] buttonStateType The state for which to get the color
     * @param [out] stateType The state actually rendered
     * @return The color string, defined in global.xml
     */
    DString GetPaintStateTextColor(ControlStateType buttonStateType, ControlStateType& stateType);

    /** Get the current font ID
     * @return The font ID, identified in global.xml
     */
    DString GetFontId() const;

    /** Set the current font ID
     * @param[in] strFontId The font ID to set; the font ID must exist in global.xml
     */
    void SetFontId(const DString& strFontId);

    /** Get the text padding
     * @return The text padding information
     */
    UiPadding GetTextPadding() const;

    /** Set the text padding information
     * @param [in] padding The padding information
     * @param [in] bNeedDpiScale Whether DPI scaling is supported
     */
    void SetTextPadding(UiPadding padding, bool bNeedDpiScale);

    /** Check whether it is single-line mode
     * @return Returns true for single-line mode, otherwise false
     */
    bool IsSingleLine() const;

    /** Set to single-line input mode
     * @param [in] bSingleLine true for single-line mode, otherwise false
     */
    void SetSingleLine(bool bSingleLine);

    /** Set whether the tooltip text shown when the mouse hovers over the control is displayed only when the ellipsis appears
    * @param [in] bAutoShow true means show only when the ellipsis appears, false means no control
    */
    void SetAutoToolTip(bool bAutoShow);

    /** Get whether the tooltip text shown when the mouse hovers over the control is displayed only when the ellipsis appears
    */
    bool IsAutoToolTip() const;

    /** Set whether to replace newline characters (replace the string "\\n" with the newline character "\n", so the two characters in parentheses (\n) can be used in XML as newline characters, thereby supporting multi-line text)
    * @param [in] bReplaceNewline true means replace, false means no replacement
    */
    void SetReplaceNewline(bool bReplaceNewline);

    /** Get whether to replace newline characters (replace the string "\\n" with the newline character "\n", so the two characters in parentheses (\n) can be used in XML as newline characters, thereby supporting multi-line text)
    * @return true means replace, false means no replacement
    */
    bool IsReplaceNewline() const;

    /** Set the line spacing (valid for both horizontal and vertical text)
     *  After setting, the actual line spacing is: font size * mul + add
     * @param [in] mul The line spacing multiplier, a ratio of the font size (the default is usually 1.0, i.e., 100% of the font size), used to adjust the line spacing proportionally
     * @param [in] add The line spacing additional amount, a fixed additional pixel value (the default is usually 0), used to add a fixed offset (pixels) on top of the proportional adjustment
     * @param [in] bNeedDpiScale Whether DPI scaling is supported
     */
    void SetLineSpacing(float mul, float add, bool bNeedDpiScale);

    /** Get the line spacing (valid for both horizontal and vertical text)
     * @param [out] mul Returns the line spacing multiplier
     * @param [out] add Returns the line spacing additional amount (pixels, already DPI scaled)
     */
    void GetLineSpacing(float* mul, float* add) const;

    /** Set the spacing between two adjacent characters (pixels)
    */
    void SetWordSpacing(float fWordSpacing, bool bNeedDpiScale);

    /** Set the spacing between two adjacent characters (pixels, already DPI scaled)
    */
    float GetWordSpacing() const;

public:
    /** Set the text direction: true for vertical text, false for horizontal text
     *    Horizontal text: from left to right, from top to bottom
     *    Vertical text: from top to bottom, from right to left
     */
    void SetVerticalText(bool bVerticalText);

    /** Get the text direction
    @return true for vertical text, false for horizontal text
    */
    bool IsVerticalText() const;

    /** Set whether, when drawing text vertically, the character spacing uses the font's default height instead of each character's actual height (all fonts are displayed at equal height)
    */
    void SetUseFontHeight(bool bUseFontHeight);

    /** Get whether, when drawing text vertically, the character spacing uses the font's default height instead of each character's actual height (all fonts are displayed at equal height)
    */
    bool IsUseFontHeight() const;

    /** Set whether, when drawing text vertically, characters such as letters and digits are displayed rotated 90 degrees clockwise
    */
    void SetRotate90ForAscii(bool bRotate90ForAscii);

    /** Get whether, when drawing text vertically, characters such as letters and digits are displayed rotated 90 degrees clockwise
    */
    bool IsRotate90ForAscii() const;

public:
    /** Get the current parameters for measuring drawn text
    * @return The currently set parameters, excluding the value of the rectSize field
    */
    MeasureStringParam GetMeasureParam() const;

    /** Get the current parameters for drawing text
    * @return The currently set parameters, excluding the values of the textRect/dwTextColor/uFade fields
    */
    DrawStringParam GetDrawParam() const;

    /** Helper function to get the valid text style, keeping the supported styles, including alignment, etc. (see the DrawStringFormat definition for details)
    * @param [in] nTextFormat The text style
    * @return The supported text style flags
    */
    static uint32_t GetValidTextStyle(uint32_t nTextFormat);

protected:
    /** Implementation function for drawing text
    * @param [in] rc The actual drawing area, excluding padding (the caller must clip out the padding)
    * @param [in] pRender The render interface
    */
    void DoPaintText(const UiRect& rc, IRender* pRender);

private:
    //Internal implementation of the functionality
    std::unique_ptr<LabelImpl> m_impl;
};

template<typename T>
LabelTemplate<T>::LabelTemplate(Window* pWindow) :
    T(pWindow)
{
    m_impl = std::make_unique<LabelImpl>(this);
}

template<typename T>
LabelTemplate<T>::~LabelTemplate()
{
    m_impl.reset();
}

template<typename T>
inline DString LabelTemplate<T>::GetType() const { return DUI_CTR_LABEL; }

template<>
inline DString LabelTemplate<Box>::GetType() const { return DUI_CTR_LABELBOX; }

template<>
inline DString LabelTemplate<HBox>::GetType() const { return DUI_CTR_LABELHBOX; }

template<>
inline DString LabelTemplate<VBox>::GetType() const { return DUI_CTR_LABELVBOX; }

template<typename T>
void LabelTemplate<T>::SetAttribute(const DString& strName, const DString& strValue)
{
    if (!m_impl->OnSetAttribute(strName, strValue)) {
        BaseClass::SetAttribute(strName, strValue);
    }
}

template<typename T>
void LabelTemplate<T>::SetWindow(Window* pWindow)
{
    Window* pOldWindow = this->GetWindow();
    BaseClass::SetWindow(pWindow);
    if (pOldWindow != pWindow) {
        m_impl->OnWindowChanged();
    }
}

template<typename T>
void LabelTemplate<T>::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!this->Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    m_impl->OnDpiScaleChanged(nOldDpiScale, nNewDpiScale);
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

template<typename T>
void LabelTemplate<T>::OnLanguageChanged()
{
    BaseClass::OnLanguageChanged();
    //The language has changed, so the string length may have changed; the layout needs to be recalculated and the ToolTip data updated
    this->RelayoutOrRedraw();
    m_impl->OnLanguageChanged();
}

template<typename T>
uint32_t LabelTemplate<T>::GetValidTextStyle(uint32_t nTextFormat)
{
    return LabelImpl::GetValidTextStyle(nTextFormat);
}

template<typename T>
void LabelTemplate<T>::SetAutoToolTip(bool bAutoShow)
{
    m_impl->SetAutoShowToolTipEnabled(bAutoShow);
}

template<typename T>
bool LabelTemplate<T>::IsAutoToolTip() const
{
    return m_impl->IsAutoShowToolTipEnabled();
}

template<typename T>
void LabelTemplate<T>::SetReplaceNewline(bool bReplaceNewline)
{
    m_impl->SetReplaceNewline(bReplaceNewline);
}

template<typename T>
bool LabelTemplate<T>::IsReplaceNewline() const
{
    return m_impl->IsReplaceNewline();
}

template<typename T>
void LabelTemplate<T>::SetVerticalText(bool bVerticalText)
{
    m_impl->SetVerticalText(bVerticalText);
}

template<typename T>
bool LabelTemplate<T>::IsVerticalText() const
{
    return m_impl->IsVerticalText();
}

template<typename T>
void LabelTemplate<T>::SetLineSpacing(float mul, float add, bool bNeedDpiScale)
{
    m_impl->SetLineSpacing(mul, add, bNeedDpiScale);
}

template<typename T>
void LabelTemplate<T>::GetLineSpacing(float* mul, float* add) const
{
    m_impl->GetLineSpacing(mul, add);
}

template<typename T>
void LabelTemplate<T>::SetWordSpacing(float fWordSpacing, bool bNeedDpiScale)
{
    m_impl->SetWordSpacing(fWordSpacing, bNeedDpiScale);
}

template<typename T>
float LabelTemplate<T>::GetWordSpacing() const
{
    return m_impl->GetWordSpacing();
}

template<typename T>
void LabelTemplate<T>::SetUseFontHeight(bool bUseFontHeight)
{
    m_impl->SetUseFontHeight(bUseFontHeight);
}

template<typename T>
bool LabelTemplate<T>::IsUseFontHeight() const
{
    return m_impl->IsUseFontHeight();
}

template<typename T>
void LabelTemplate<T>::SetRotate90ForAscii(bool bRotate90ForAscii)
{
    m_impl->SetRotate90ForAscii(bRotate90ForAscii);
}

template<typename T>
bool LabelTemplate<T>::IsRotate90ForAscii() const
{
    return m_impl->IsRotate90ForAscii();
}

template<typename T>
MeasureStringParam LabelTemplate<T>::GetMeasureParam() const
{
    return m_impl->GetMeasureParam();
}

template<typename T>
DrawStringParam LabelTemplate<T>::GetDrawParam() const
{
    return m_impl->GetDrawParam();
}

template<typename T /*= Control*/>
void ui::LabelTemplate<T>::SetPos(UiRect rc)
{
    BaseClass::SetPos(rc);
    m_impl->CheckShowToolTip();
}

template<typename T>
DString LabelTemplate<T>::GetToolTipText() const
{
    DString toolTip = BaseClass::GetToolTipText();
    if (toolTip.empty()) {
        toolTip = m_impl->GetAutoToolTipText();
    }
    return toolTip;
}

template<typename T>
DString LabelTemplate<T>::GetText() const
{
    return m_impl->GetText();
}

template<typename T>
void LabelTemplate<T>::SetText(const DString& strText)
{
    m_impl->SetText(strText);
}

template<typename T>
DString LabelTemplate<T>::GetTextId() const
{
    return m_impl->GetTextId();
}

template<typename T>
void LabelTemplate<T>::SetTextId(const DString& strTextId)
{
    m_impl->SetTextId(strTextId);
}

template<typename T>
std::string LabelTemplate<T>::GetUTF8Text() const
{
    return m_impl->GetUTF8Text();
}

template<typename T>
void LabelTemplate<T>::SetUTF8Text(const std::string& strText)
{
    m_impl->SetUTF8Text(strText);
}

template<typename T>
void LabelTemplate<T>::SetUTF8TextId(const std::string& strTextId)
{
    m_impl->SetUTF8TextId(strTextId);
}

template<typename T>
std::string LabelTemplate<T>::GetUTF8TextId() const
{
    return m_impl->GetUTF8TextId();
}

template<typename T>
void LabelTemplate<T>::SetRichText(bool bRichText)
{
    m_impl->SetRichText(bRichText);
}

template<typename T>
bool LabelTemplate<T>::IsRichText() const
{
    return m_impl->IsRichText();
}

template<typename T>
bool LabelTemplate<T>::IsTextEquals(const DString& text) const
{
    return m_impl->IsTextEquals(text);
}

template<typename T>
bool LabelTemplate<T>::HasHotState()
{
    if (BaseClass::HasHotState()) {
        return true;
    }
    return m_impl->HasHotColorState();
}

template<typename T>
UiSize LabelTemplate<T>::EstimateText(UiSize szAvailable)
{
    return m_impl->OnEstimateText(szAvailable);
}

template<typename T>
void LabelTemplate<T>::PaintText(IRender* pRender)
{
    m_impl->OnPaintText(pRender);
}

template<typename T>
void LabelTemplate<T>::SetTextStyle(uint32_t uStyle, bool bRedraw)
{
    m_impl->SetTextStyle(uStyle, bRedraw);
}

template<typename T>
void LabelTemplate<T>::SetDefaultTextStyle(bool bRedraw)
{
    m_impl->SetDefaultTextStyle(bRedraw);
}

template<typename T>
uint32_t LabelTemplate<T>::GetTextStyle() const
{
    return m_impl->GetTextStyle();
}

template<typename T>
DString LabelTemplate<T>::GetStateTextColor(ControlStateType stateType) const
{
    return m_impl->GetStateTextColor(stateType);
}

template<typename T>
void LabelTemplate<T>::SetStateTextColor(ControlStateType stateType, const DString& dwTextColor)
{
    m_impl->SetStateTextColor(stateType, dwTextColor);
}

template<typename T /*= Control*/>
DString ui::LabelTemplate<T>::GetPaintStateTextColor(ControlStateType buttonStateType, ControlStateType& stateType)
{
    return m_impl->GetPaintStateTextColor(buttonStateType, stateType);
}

template<typename T>
DString LabelTemplate<T>::GetFontId() const
{
    return m_impl->GetFontId();
}

template<typename T>
void LabelTemplate<T>::SetFontId(const DString& strFontId)
{
    m_impl->SetFontId(strFontId);
}

template<typename T>
UiPadding LabelTemplate<T>::GetTextPadding() const
{
    return m_impl->GetTextPadding();
}

template<typename T>
void LabelTemplate<T>::SetTextPadding(UiPadding padding, bool bNeedDpiScale)
{
    m_impl->SetTextPadding(padding, bNeedDpiScale);   
}

template<typename T>
bool LabelTemplate<T>::IsSingleLine() const
{
    return m_impl->IsSingleLine();
}

template<typename T>
void LabelTemplate<T>::SetSingleLine(bool bSingleLine)
{
    m_impl->SetSingleLine(bSingleLine);
}

template<typename T>
void LabelTemplate<T>::DoPaintText(const UiRect& rc, IRender* pRender)
{
    m_impl->DoPaintText(rc, pRender);
}

typedef LabelTemplate<Control> Label;
typedef LabelTemplate<Box> LabelBox;
typedef LabelTemplate<HBox> LabelHBox;
typedef LabelTemplate<VBox> LabelVBox;

}

#endif // UI_CONTROL_LABEL_H_
