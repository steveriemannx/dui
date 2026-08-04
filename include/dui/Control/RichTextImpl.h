#ifndef UI_CONTROL_RICHTEXT_IMPL_H_
#define UI_CONTROL_RICHTEXT_IMPL_H_

#include "dui/Core/Control.h"
#include "dui/Render/IRender.h"

namespace ui 
{
/** A formatted text segment of RichText after XML parsing
*/
class DUI_API RichTextSlice
{
public:
    /** Node name
    */
    UiString m_nodeName;

    /** Text content (UTF16 encoded)
    */
    DStringW m_text;

    /** URL of the hyperlink: "href"
    */
    UiString m_linkUrl;

    /** Text color: "color"
    */
    UiString m_textColor;

    /** Background color: "color"
    */
    UiString m_bgColor;

    /** Font information
    */
    UiFont m_fontInfo;

public:
    /** Child nodes
    */
    std::vector<RichTextSlice> m_children;
};

/** Formatted text (HTML-like format)
*/
class DUI_API RichTextImpl
{
public:
    explicit RichTextImpl(Control* pOwner);
    RichTextImpl(const RichTextImpl& r) = delete;
    RichTextImpl& operator=(const RichTextImpl& r) = delete;
    ~RichTextImpl();

    bool SetAttribute(const DString& strName, const DString& strValue);
    void PaintText(IRender* pRender);

    /** The bound window has changed
    */
    void OnWindowChanged();

    /** DPI has changed; update the control size and layout
    * @param [in] nOldDpiScale the old DPI scale percentage
    * @param [in] nNewDpiScale the new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale);

    /** Calculate the text area size (width and height)
     *  @param [in] szAvailable available size, excluding padding and margin
     *  @return the estimated text size of the control, including padding (Box), excluding margin
     */
    UiSize EstimateText(UiSize szAvailable);

public:
    /** Get the text padding
     */
    UiPadding GetTextPadding() const;

    /** Set the text padding; DPI adaptation is performed inside the function
     * @param [in] padding the four edge values of the rectangle represent the four padding values respectively
     * @param [in] bNeedDpiScale whether to scale with DPI, the default is true
     */
    void SetTextPadding(UiPadding padding, bool bNeedDpiScale = true);

    /** Get the current font ID
     * @return the font ID, which is identified in global.xml
     */
    DString GetFontId() const;

    /** Set the current font ID
     * @param [in] strFontId the font ID to set, which can exist in global.xml
     */
    void SetFontId(const DString& strFontId);

    /** Get the default text color
     */
    DString GetTextColor() const;

    /** Set the default text color
     */
    void SetTextColor(const DString& sTextColor);

    /** Get the row spacing multiplier
    */
    float GetRowSpacingMul() const;

    /** Row spacing multiplier: the multiple ratio of the font size (the default is usually 1.0, i.e., 100% of the font size), used to adjust the row spacing proportionally
    */
    void SetRowSpacingMul(float fRowSpacingMul);

    /** Get the row spacing additional amount
    */
    float GetRowSpacingAdd() const;

    /** Row spacing additional amount: a fixed additional pixel value (the default is usually 0), used to add a fixed offset (in pixels) on top of the proportional adjustment
    */
    void SetRowSpacingAdd(float fRowSpacingAdd);

    /** Get how text is displayed when it exceeds the rectangle area
     * @return when it returns true and in multi-line mode, the content is wrapped; false means it is truncated
     */
    bool IsWordWrap() const;

    /** Set how text is displayed when it exceeds the rectangle area
     * @param[in] bWordWrap when true and in multi-line mode, the content is wrapped; false means it is truncated
     */
    void SetWordWrap(bool bWordWrap);

    /** Set the horizontal alignment of the text
    */
    void SetTextHAlignType(HorAlignType alignType);

    /** Get the horizontal alignment of the text
    */
    HorAlignType GetHAlignType() const;

    /** Set the vertical alignment of the text
    */
    void SetTextVAlignType(VerAlignType alignType);

    /** Get the vertical alignment of the text
    */
    VerAlignType GetVAlignType() const;

public:
    /** Set formatted text
    * @param [in] richText the text content with format
    * @param [in] bRedraw true means triggering a redraw, false means not triggering a redraw
    */
    bool SetText(const DString& richText, bool bRedraw = true);

    /** Set the formatted text ID
    * @param [in] richTextId the ID of the text content with format
    * @param [in] bRedraw true means triggering a redraw, false means not triggering a redraw
    */
    bool SetTextId(const DString& richTextId, bool bRedraw = true);

    /** Clear the original formatted text
    */
    void Clear();

    /** Append a text segment
    */
    void AppendTextSlice(const RichTextSlice&& textSlice);

    /** Append a text segment
    */
    void AppendTextSlice(const RichTextSlice& textSlice);

    /** Trim the text according to the Trim policy, removing extra spaces
    * @param [in,out] text the text to process as input, and the processed text as output
    * @return the reference of text
    */
    const DString& TrimText(DString& text);

    /** Trim the text according to the Trim policy, removing extra spaces
    * @param [in] text the text to process
    * @return the processed string
    */
    DString TrimText(const DString::value_type* text);

    /** Set whether redraw is allowed
    * @param [in] bEnable true means redraw is allowed, false means redraw is prohibited
    */
    void SetEnableRedraw(bool bEnable);

    /** Get whether redraw is allowed
    */
    bool IsEnableRedraw() const;

public:
    /** Output the formatted text
    */
    DString ToString() const;

public:
    /** Redraw
    */
    void Redraw();

    /** Redraw
    */
    void Invalidate();

    //Mouse messages (returning true: the message has been handled; returning false: the message has not been handled and needs to be forwarded to the parent control)
    void ButtonDown(const EventArgs& msg);
    void ButtonUp(const EventArgs& msg);
    void MouseMove(const EventArgs& msg);
    void MouseHover(const EventArgs& msg);
    void MouseLeave(const EventArgs& msg);
    bool OnSetCursor(const EventArgs& msg);

private:
    /** Formatted text, the parsed structure (for internal use)
    */
    class RichTextDataEx :
        public RichTextData
    {
    public:
        /** Drawing area of the object (output parameter)
        */
        std::vector<UiRect> m_textRects;

        /** URL of the hyperlink
        */
        UiString m_linkUrl;

        /** Whether the mouse button is pressed
        */
        bool m_bMouseDown = false;

        /** Whether it is in the mouse hover state
        */
        bool m_bMouseHover = false;
    };

private:
    /** Set formatted text, but do not redraw
    * @param [in] richText the text content with format
    */
    bool DoSetText(const DString& richText);

    /** Parse the formatted text and generate the parsed data structure
    */
    bool ParseText(std::vector<RichTextDataEx>& outTextData) const;

    /** Check and parse the text as needed
    */
    void CheckParseText();

    /** Parse a text segment into a drawing structure
    * @param [in] textSlice text segment
    * @param [in] parentTextData parent object information
    * @param [out] textData the parsed text structure
    */
    bool ParseTextSlice(const RichTextSlice& textSlice, 
                        const RichTextDataEx& parentTextData,
                        std::vector<RichTextDataEx>& textData) const;

    /** Output the formatted text
    */
    DString ToString(const RichTextSlice& textSlice, const DString& indent) const;

    /** Get the attributes of the currently drawn text
    */
    uint16_t GetTextStyle() const;

    /** Calculate the target area size after drawing
    */
    void CalcDestRect(IRender* pRender, const UiRect& rc, UiRect& rect);

private:
    /** Associated control
    */
    Control* m_pOwner;

    /** Drawing cache
    */
    std::shared_ptr<DrawRichTextCache> m_spDrawRichTextCache;

    /** Padding for text drawing (corresponding to the padding sizes of the four edges respectively)
    */
    UiPadding16 m_rcTextPadding;

    /** Default font
    */
    UiString m_sFontId;

    /** Default text color
    */
    UiString m_sTextColor;

    /** Horizontal alignment of the text
    */
    HorAlignType m_hAlignType;

    /** Vertical alignment of the text
    */
    VerAlignType m_vAlignType;

    /** Row spacing multiplier
    */
    float m_fRowSpacingMul;

    /** Row spacing additional amount
    */
    float m_fRowSpacingAdd;

    /** Drawn text content (before parsing)
    */
    std::vector<RichTextSlice> m_textSlice;

    /** Drawn text content (after parsing)
    */
    std::vector<RichTextDataEx> m_textData;

    /** The UI scale factor corresponding to the parsed text
    */
    uint32_t m_nTextDataDisplayScaleFactor;

    /** Hyperlink text: normal text color
    */
    UiString m_linkNormalTextColor;

    /** Hyperlink text: hover text color
    */
    UiString m_linkHoverTextColor;

    /** Hyperlink text: mouse pressed text color
    */
    UiString m_linkMouseDownTextColor;

    /** Text ID
    */
    UiString m_richTextId;

    /** Language file
    */
    UiString m_langFileName;

    /** Whether to show the underline font style
    */
    bool m_bLinkUnderlineFont;

    /** Trim policy of the text
    */
    enum class TrimPolicy: int8_t {
        kNone    = 0, //do nothing
        kAll     = 1, //remove all spaces
        kKeepOne = 2, //remove extra spaces, keep only one space
    };

    /** Trim policy of the text
    */
    TrimPolicy m_trimPolicy = TrimPolicy::kAll;

    /** Whether to wrap automatically (default is true)
    */
    bool m_bWordWrap;

    /** Whether to replace curly braces when setting the text attribute
    */
    bool m_bReplaceBrace;

    /** Whether redraw is allowed
    */
    bool m_bEnableRedraw;
};

} // namespace ui

#endif // UI_CONTROL_RICHTEXT_IMPL_H_
