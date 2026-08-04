#ifndef UI_CONTROL_RICHTEXT_H_
#define UI_CONTROL_RICHTEXT_H_

#include "dui/Control/RichTextImpl.h"
#include "dui/Box/HBox.h"
#include "dui/Box/VBox.h"

namespace ui 
{
/** Formatted text (HTML-like format)
*/
template<typename T = Control>
class RichTextT: public T
{
    typedef T BaseClass;
public:
    explicit RichTextT(Window* pWindow);
    RichTextT(const RichTextT& r) = delete;
    RichTextT& operator=(const RichTextT& r) = delete;
    virtual ~RichTextT() override;

    /// Override parent class methods to provide personalized features, please refer to the parent class declaration
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void PaintText(IRender* pRender) override;

    /** Set the window to which the container belongs
     * @param [in] pWindow Window pointer
     */
    virtual void SetWindow(Window* pWindow) override;

    /** DPI changed, update control size and layout
    * @param [in] nOldDpiScale Old DPI scaling percentage
    * @param [in] nNewDpiScale New DPI scaling percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Language changed, refresh the content related to UI text display
    */
    virtual void OnLanguageChanged() override;

    /** Calculate the size of the text area (width and height)
     *  @param [in] szAvailable Available size, not including inner padding, not including outer margin
     *  @return Estimated text size of the control, including inner padding (Box), not including outer margin
     */
    virtual UiSize EstimateText(UiSize szAvailable) override;

public:
    /** Get text inner padding
     */
    UiPadding GetTextPadding() const;

    /** Set text inner padding information, DPI adaptive operation will be performed inside the function
     * @param [in] padding The four edge values of the rectangle represent the four corresponding inner padding values
     * @param [in] bNeedDpiScale Compatible with DPI scaling, defaults to true
     */
    void SetTextPadding(const UiPadding& padding, bool bNeedDpiScale = true);

    /** Get the current font ID
     * @return Returns the font ID, this ID is identified in global.xml
     */
    DString GetFontId() const;

    /** Set the current font ID
     * @param [in] strFontId The font ID to set, this ID can exist in global.xml
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

    /** Row spacing multiplier: multiple ratio of the font size (default is usually 1.0, i.e. 100% of the font size), used to adjust row spacing proportionally
    */
    void SetRowSpacingMul(float fRowSpacingMul);

    /** Get the row spacing addition
    */
    float GetRowSpacingAdd() const;

    /** Row spacing addition: a fixed additional pixel value (default is usually 0), used to add a fixed offset (pixels) on top of the proportional adjustment
    */
    void SetRowSpacingAdd(float fRowSpacingAdd);

    /** Get how text overflowing the rectangle area is displayed
     * @return When it returns true and in multi-line mode the content is displayed wrapped, false means truncated display
     */
    bool IsWordWrap() const;

    /** Set how text overflowing the rectangle area is displayed
     * @param[in] bWordWrap When it is true and in multi-line mode the content is displayed wrapped, false means truncated display
     */
    void SetWordWrap(bool bWordWrap);

    /** Set text horizontal alignment
    */
    void SetTextHAlignType(HorAlignType alignType);

    /** Get text horizontal alignment
    */
    HorAlignType GetHAlignType() const;

    /** Set text vertical alignment
    */
    void SetTextVAlignType(VerAlignType alignType);

    /** Get text vertical alignment
    */
    VerAlignType GetVAlignType() const;

public:
    /** Set formatted text
    * @param [in] richText Text content with formatting
    * @param [in] bRedraw true triggers a redraw, false does not trigger a redraw
    */
    bool SetText(const DString& richText, bool bRedraw = true);

    /** Set the formatted text ID
    * @param [in] richTextId The ID of the formatted text content
    * @param [in] bRedraw true triggers a redraw, false does not trigger a redraw
    */
    bool SetTextId(const DString& richTextId, bool bRedraw = true);

    /** Clear the original formatted text
    */
    void Clear();

    /** Append a text slice
    */
    void AppendTextSlice(const RichTextSlice&& textSlice);

    /** Append a text slice
    */
    void AppendTextSlice(const RichTextSlice& textSlice);

    /** Perform Trim processing on the text according to the Trim scheme, removing extra spaces
    * @param [in,out] text The text to be processed is passed in, and the processed text is passed out
    * @return Returns the reference to text
    */
    const DString& TrimText(DString& text);

    /** Perform Trim processing on the text according to the Trim scheme, removing extra spaces
    * @param [in] text The text to be processed
    * @return Returns the processed string
    */
    DString TrimText(const DString::value_type* text);

    /** Set whether redrawing is allowed
    * @param [in] bEnable true means redrawing is allowed, false means redrawing is prohibited
    */
    void SetEnableRedraw(bool bEnable);

    /** Get whether redrawing is allowed
    */
    bool IsEnableRedraw() const;

public:
    /** Output formatted text
    */
    DString ToString() const;

    /** Listen for the hyperlink click event
     * @param [in] callback The callback function after the hyperlink is clicked
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     */
    void AttachLinkClick(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventLinkClick, callback, callbackID); }

    /** Get the implementation interface
    */
    RichTextImpl* GetRichTextImpl() const { return m_impl.get(); }

private:
    //Mouse message (return true: the message has been handled; return false: the message has not been handled and needs to be forwarded to the parent control)
    virtual bool ButtonDown(const EventArgs& msg) override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual bool MouseMove(const EventArgs& msg) override;
    virtual bool MouseHover(const EventArgs& msg) override;
    virtual bool MouseLeave(const EventArgs& msg) override;
    virtual bool OnSetCursor(const EventArgs& msg) override;

private:

    //Internal implementation of the functionality
    std::unique_ptr<RichTextImpl> m_impl;
};

template<typename T>
RichTextT<T>::RichTextT(Window* pWindow) :
    T(pWindow)
{
    m_impl = std::make_unique<RichTextImpl>(this);
}

template<typename T>
RichTextT<T>::~RichTextT()
{
    m_impl.reset();
}

template<typename T>
inline DString RichTextT<T>::GetType() const { return DUI_CTR_RICHTEXT; }

template<>
inline DString RichTextT<Box>::GetType() const { return DUI_CTR_RICHTEXT_BOX; }

template<>
inline DString RichTextT<HBox>::GetType() const { return DUI_CTR_RICHTEXT_HBOX; }

template<>
inline DString RichTextT<VBox>::GetType() const { return DUI_CTR_RICHTEXT_VBOX; }

template<typename T>
void RichTextT<T>::SetAttribute(const DString& strName, const DString& strValue)
{
    if (!m_impl->SetAttribute(strName, strValue)) {
        BaseClass::SetAttribute(strName, strValue);
    }
}

template<typename T>
void RichTextT<T>::SetWindow(Window* pWindow)
{
    BaseClass::SetWindow(pWindow);
    m_impl->OnWindowChanged();
}

template<typename T>
void RichTextT<T>::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    m_impl->ChangeDpiScale(nOldDpiScale, nNewDpiScale);
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

template<typename T>
void RichTextT<T>::OnLanguageChanged()
{
    BaseClass::OnLanguageChanged();
    m_impl->Redraw();
}

template<typename T>
UiSize RichTextT<T>::EstimateText(UiSize szAvailable)
{
    return m_impl->EstimateText(szAvailable);
}

template<typename T>
void RichTextT<T>::PaintText(IRender* pRender)
{
    m_impl->PaintText(pRender);
}

template<typename T>
UiPadding RichTextT<T>::GetTextPadding() const
{
    return m_impl->GetTextPadding();
}

template<typename T>
void RichTextT<T>::SetTextPadding(const UiPadding& padding, bool bNeedDpiScale)
{
    m_impl->SetTextPadding(padding, bNeedDpiScale);
}

template<typename T>
const DString& RichTextT<T>::TrimText(DString& text)
{
    return m_impl->TrimText(text);
}

template<typename T>
DString RichTextT<T>::TrimText(const DString::value_type* text)
{
    return m_impl->TrimText(text);
}

template<typename T>
void RichTextT<T>::SetEnableRedraw(bool bEnable)
{
    m_impl->SetEnableRedraw(bEnable);
}

template<typename T>
bool RichTextT<T>::IsEnableRedraw() const
{
    return m_impl->IsEnableRedraw();
}

template<typename T>
bool RichTextT<T>::SetText(const DString& richText, bool bRedraw)
{
    return m_impl->SetText(richText, bRedraw);
}

template<typename T>
bool RichTextT<T>::SetTextId(const DString& richTextId, bool bRedraw)
{
    return m_impl->SetTextId(richTextId, bRedraw);
}

template<typename T>
void RichTextT<T>::Clear()
{
    m_impl->Clear();
}

template<typename T>
DString RichTextT<T>::GetFontId() const
{
    return m_impl->GetFontId();
}

template<typename T>
void RichTextT<T>::SetFontId(const DString& strFontId)
{
    m_impl->SetFontId(strFontId);
}

template<typename T>
DString RichTextT<T>::GetTextColor() const
{
    return m_impl->GetTextColor();
}

template<typename T>
void RichTextT<T>::SetTextColor(const DString& sTextColor)
{
    m_impl->SetTextColor(sTextColor);
}

template<typename T>
float RichTextT<T>::GetRowSpacingMul() const
{
    return m_impl->GetRowSpacingMul();
}

template<typename T>
void RichTextT<T>::SetRowSpacingMul(float fRowSpacingMul)
{
    m_impl->SetRowSpacingMul(fRowSpacingMul);
}

template<typename T>
float RichTextT<T>::GetRowSpacingAdd() const
{
    return m_impl->GetRowSpacingAdd();
}

template<typename T>
void RichTextT<T>::SetRowSpacingAdd(float fRowSpacingAdd)
{
    m_impl->SetRowSpacingAdd(fRowSpacingAdd);
}

template<typename T>
bool RichTextT<T>::IsWordWrap() const
{
    return m_impl->IsWordWrap();
}

template<typename T>
void RichTextT<T>::SetWordWrap(bool bWordWrap)
{
    m_impl->SetWordWrap(bWordWrap);
}

template<typename T>
void RichTextT<T>::SetTextHAlignType(HorAlignType alignType)
{
    m_impl->SetTextHAlignType(alignType);
}

template<typename T>
HorAlignType RichTextT<T>::GetHAlignType() const
{
    return m_impl->GetHAlignType();
}

template<typename T>
void RichTextT<T>::SetTextVAlignType(VerAlignType alignType)
{
    m_impl->SetTextVAlignType(alignType);
}

template<typename T>
VerAlignType RichTextT<T>::GetVAlignType() const
{
    return m_impl->GetVAlignType();
}

template<typename T>
void RichTextT<T>::AppendTextSlice(const RichTextSlice&& textSlice)
{
    m_impl->AppendTextSlice(textSlice);
}

template<typename T>
void RichTextT<T>::AppendTextSlice(const RichTextSlice& textSlice)
{
    m_impl->AppendTextSlice(textSlice);
}

template<typename T>
DString RichTextT<T>::ToString() const
{
    return m_impl->ToString();
}

template<typename T>
bool RichTextT<T>::ButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonDown(msg);
    m_impl->ButtonDown(msg);
    return bRet;
}

template<typename T>
bool RichTextT<T>::ButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonUp(msg);
    m_impl->ButtonUp(msg);
    return bRet;
}

template<typename T>
bool RichTextT<T>::MouseMove(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseMove(msg);
    m_impl->MouseMove(msg);
    return bRet;
}

template<typename T>
bool RichTextT<T>::MouseHover(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseHover(msg);
    m_impl->MouseHover(msg);
    return bRet;
}

template<typename T>
bool RichTextT<T>::MouseLeave(const EventArgs& msg)
{
    m_impl->MouseLeave(msg);
    return BaseClass::MouseLeave(msg);
}

template<typename T>
bool RichTextT<T>::OnSetCursor(const EventArgs& msg)
{
    if (!m_impl->OnSetCursor(msg)) {
        return BaseClass::OnSetCursor(msg);
    }
    return true;
}

typedef RichTextT<Control> RichText;
typedef RichTextT<Box>     RichTextBox;
typedef RichTextT<HBox>    RichTextHBox;
typedef RichTextT<VBox>    RichTextVBox;

} // namespace ui

#endif // UI_CONTROL_RICHTEXT_H_
