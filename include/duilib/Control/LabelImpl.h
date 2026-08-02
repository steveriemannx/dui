#ifndef UI_CONTROL_LABEL_IMPL_H_
#define UI_CONTROL_LABEL_IMPL_H_

#include "duilib/Core/Control.h"
#include "duilib/Render/IRender.h"

namespace ui
{
class TextDrawer;

/** Interface for getting text, supporting virtual functions
*/
class DUILIB_API LabelOwner
{
public:
    virtual ~LabelOwner() = default;

    /** Get the text content
    */
    virtual DString GetText() const = 0;

    /** Set the text content
    * @param [in] strText The text content
    */
    virtual void SetText(const DString& strText) = 0;

    /** Get the text content ID (supports multiple languages)
    */
    virtual DString GetTextId() const = 0;

    /** Set the text content ID (supports multiple languages)
    * @param [in] strTextId The ID of the text content
    */
    virtual void SetTextId(const DString& strTextId) = 0;
};

/** Internal implementation of the label control, used to display text
*/
class DUILIB_API LabelImpl
{
public:
    explicit LabelImpl(Control* pOwner);
    ~LabelImpl();

    /// Override the parent class methods to provide customized functionality; refer to the parent class declarations
    bool HasHotColorState();
    bool OnSetAttribute(const DString& strName, const DString& strValue);
    void OnPaintText(IRender* pRender);

    /** The bound window has changed
    */
    void OnWindowChanged();

    /** The language has changed
    */
    void OnLanguageChanged();

    /** The DPI has changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    void OnDpiScaleChanged(uint32_t nOldDpiScale, uint32_t nNewDpiScale);
    
    /** Calculate the size of the text area (width and height)
     *  @param [in] szAvailable The available size, excluding padding and margins
     *  @return The estimated text size of the control, including padding (Box), excluding margins
     */
    UiSize OnEstimateText(UiSize szAvailable);

public:
    /** Get the text content
    */
    DString GetText() const;

    /** Set the text content
    * @param [in] strText The text content
    */
    void SetText(const DString& strText);

    /** Get the text content ID (supports multiple languages)
    */
    DString GetTextId() const;

    /** Set the text content ID (supports multiple languages)
    * @param [in] strTextId The ID of the text content
    */
    void SetTextId(const DString& strTextId);

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
    void SetAutoShowToolTipEnabled(bool bAutoShow);

    /** Get whether the tooltip text shown when the mouse hovers over the control is displayed only when the ellipsis appears
    */
    bool IsAutoShowToolTipEnabled() const;

    /** Get the automatically displayed tooltip text information
    */
    DString GetAutoToolTipText() const;

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

    /** Get the spacing between two adjacent characters (pixels, already DPI scaled)
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

    /** Check whether the ToolTip needs to be displayed automatically
    */
    void CheckShowToolTip();

    /** Implementation function for drawing text
    * @param [in] rc The actual drawing area, excluding padding (the caller must clip out the padding)
    * @param [in] pRender The render interface
    */
    void DoPaintText(const UiRect& rc, IRender* pRender);

private:
    /** Get the text from the Owner (supports virtual functions)
    */
    DString GetOwnerText() const;

private:
    /** The associated control
    */
    Control* m_pOwner;

    /** The implementation of text drawing (supports RichText text drawing)
    */
    std::unique_ptr<TextDrawer> m_pTextDrawer;

    //Text content
    UiString m_sText;

    //Text ID, used to support multiple languages
    UiString m_sTextId;

    //Font ID
    UiString m_sFontId;

    //Text color mapping table for each state (normal/hot/pushed/disabled)
    std::unique_ptr<StateColorMap> m_pTextColorMap;

    //Text padding
    UiPadding16 m_rcTextPadding;

    //Text alignment attribute
    uint32_t m_uTextStyle;

    //Actual line spacing = font size x m_fSpacingMul + m_fSpacingAdd
    //Line spacing multiplier: a ratio of the font size (the default is usually 1.0, i.e., 100% of the font size), used to adjust the line spacing proportionally
    float m_fSpacingMul;

    //Line spacing additional amount: a fixed additional pixel value (the default is usually 0), used to add a fixed offset (pixels) on top of the proportional adjustment
    float m_fSpacingAdd;

    //The spacing between each two characters (pixels)
    float m_fWordSpacing;

    //Whether it is single-line text: true for single-line text, false for multi-line text
    bool m_bSingleLine;

    //Whether the auto-display Tooltip feature is enabled
    bool m_bAutoShowToolTipEnabled;

    //Whether the Tooltip should currently be displayed automatically
    bool m_bAutoShowTooltip;

    //Whether to replace newline characters (replace the string "\\n" with the newline character "\n", so the two characters in parentheses (\n) can be used in XML as newline characters, thereby supporting multi-line text)
    bool m_bReplaceNewline;

    //Text direction: true for vertical text, false for horizontal text
    //    Horizontal text: from left to right, from top to bottom
    //    Vertical text: from top to bottom, from right to left
    bool m_bVerticalText;

    //When drawing vertically, use the font's default height instead of each font's height (all fonts are displayed at equal height)
    bool m_bUseFontHeight;

    //When drawing vertically, characters such as letters and digits are displayed rotated 90 degrees clockwise
    bool m_bRotate90ForAscii;

    //Whether the text content is RichText
    bool m_bRichText;
};

}

#endif // UI_CONTROL_LABEL_IMPL_H_
