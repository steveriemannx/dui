#ifndef UI_CONTROL_GROUPBOX_H_
#define UI_CONTROL_GROUPBOX_H_

#include "dui/Control/Label.h"
#include "dui/Box/HBox.h"
#include "dui/Box/VBox.h"

namespace ui
{

/** Group container
*/
template<typename InheritType>
class GroupBoxTemplate : public LabelTemplate<InheritType>
{
    typedef LabelTemplate<InheritType> BaseClass;
public:
    explicit GroupBoxTemplate(Window* pWindow);
    virtual ~GroupBoxTemplate() override;
        
    /// Override the base class method to provide customized functionality; refer to the base class declaration
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void PaintText(IRender* pRender) override;

    /** When the DPI changes, update the control size and layout
    * @param [in] nOldDpiScale Old DPI scale percentage
    * @param [in] nNewDpiScale New DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Set the corner size
     */
    void SetCornerSize(UiSize cxyRound, bool bNeedDpiScale);

    /** Get the corner size
    */
    const UiSize& GetCornerSize() const;

    /** Set the line width
    */
    void SetLineWidth(int32_t nLineWidth, bool bNeedDpiScale);

    /** Get the line width
    */
    int32_t GetLineWidth() const;

    /** Set the line color
    */
    void SetLineColor(const DString& lineColor);

private:
    /** Get a color with a certain transparency
    */
    UiColor GetFadeColor(UiColor color, uint8_t nFade) const;

private:
    //Line width
    int32_t m_nLineWidth;

    //Corner size (defaults to square corners, no rounding)
    UiSize m_cornerSize;

    //Line color
    UiString m_lineColor;
};

template<typename InheritType>
GroupBoxTemplate<InheritType>::GroupBoxTemplate(Window* pWindow):
    LabelTemplate<InheritType>(pWindow),
    m_nLineWidth(0)
{
    SetAttribute(_T("text_align"), _T("top,left"));
    SetAttribute(_T("text_padding"), _T("8,0,0,0"));
}

template<typename InheritType>
GroupBoxTemplate<InheritType>::~GroupBoxTemplate()
{
}

template<typename InheritType>
inline DString GroupBoxTemplate<InheritType>::GetType() const { return _T("GroupBoxTemplate"); }

template<>
inline DString GroupBoxTemplate<Box>::GetType() const { return DUI_CTR_GROUP_BOX; }

template<>
inline DString GroupBoxTemplate<HBox>::GetType() const { return DUI_CTR_GROUP_HBOX; }

template<>
inline DString GroupBoxTemplate<VBox>::GetType() const { return DUI_CTR_GROUP_VBOX; }

template<typename InheritType>
void GroupBoxTemplate<InheritType>::SetCornerSize(UiSize cxyRound, bool bNeedDpiScale)
{
    int32_t cx = cxyRound.cx;
    int32_t cy = cxyRound.cy;
    ASSERT(cx >= 0);
    ASSERT(cy >= 0);
    if ((cx < 0) || (cy < 0)) {
        return;
    }
    //The two parameters must either both be 0 or both be greater than 0; otherwise the parameters are invalid
    ASSERT(((cx > 0) && (cy > 0)) || ((cx == 0) && (cy == 0)));
    if (cx == 0) {
        if (cy != 0) {
            return;
        }
    }
    else {
        if (cy == 0) {
            return;
        }
    }
    if (bNeedDpiScale) {
        this->Dpi().ScaleSize(cxyRound);
    }
    if (m_cornerSize != cxyRound) {
        m_cornerSize = cxyRound;
        this->Invalidate();
    }
}

template<typename InheritType>
const UiSize& GroupBoxTemplate<InheritType>::GetCornerSize() const
{
    return m_cornerSize;
}

template<typename InheritType>
void GroupBoxTemplate<InheritType>::SetLineWidth(int32_t nLineWidth, bool bNeedDpiScale)
{
    if (nLineWidth < 0) {
        nLineWidth = 0;
    }
    if (bNeedDpiScale) {
        this->Dpi().ScaleInt(nLineWidth);
    }
    if (m_nLineWidth != nLineWidth) {
        m_nLineWidth = nLineWidth;
        this->Invalidate();
    }
}

template<typename InheritType>
int32_t GroupBoxTemplate<InheritType>::GetLineWidth() const
{
    return m_nLineWidth;
}

template<typename InheritType>
void GroupBoxTemplate<InheritType>::SetLineColor(const DString& lineColor)
{
    if (m_lineColor != lineColor) {
        m_lineColor = lineColor;
        this->Invalidate();
    }
}

template<typename InheritType>
UiColor GroupBoxTemplate<InheritType>::GetFadeColor(UiColor color, uint8_t nFade) const
{
    color = UiColor(nFade, color.GetR() * nFade / 255, color.GetG() * nFade / 255, color.GetB() * nFade / 255);
    return color;
}

template<typename InheritType>
void GroupBoxTemplate<InheritType>::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("corner_size")) {
        //Corner size
        UiSize cxyRound;
        AttributeUtil::ParseSizeValue(strValue.c_str(), cxyRound);
        this->SetCornerSize(cxyRound, true);
    }
    else if (strName == _T("line_width")) {
        //Line width
        ASSERT(StringUtil::StringToInt32(strValue) >= 0);
        this->SetLineWidth(StringUtil::StringToInt32(strValue), true);
    }
    else if (strName == _T("line_color")) {
        //Line color
        this->SetLineColor(strValue);
    }
    else if (strName == _T("text")) {
        //Set the text content
        BaseClass::SetAttribute(strName, strValue);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

template<typename InheritType>
void GroupBoxTemplate<InheritType>::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!this->Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    UiSize cxyRound = this->GetCornerSize();
    cxyRound = this->Dpi().GetScaleSize(cxyRound, nOldDpiScale);
    this->SetCornerSize(cxyRound, false);

    int32_t iValue = this->GetLineWidth();
    iValue = this->Dpi().GetScaleInt(iValue, nOldDpiScale);
    this->SetLineWidth(iValue, false);
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

template<typename InheritType>
void GroupBoxTemplate<InheritType>::PaintText(IRender* pRender)
{
    //Draw the text first
    BaseClass::PaintText(pRender);

    if (pRender == nullptr) {
        return;
    }

    UiPadding rcPadding = this->GetControlPadding();
    DString textValue = this->GetText();//text content
    UiRect drawTextRect;//draw area of the text
    bool hasClip = false;
    if (!textValue.empty()) {
        MeasureStringParam measureParam;
        measureParam.pFont = this->GetIFontById(this->GetFontId());
        UiRect textRect = pRender->MeasureString(textValue, measureParam);
        drawTextRect = this->GetRect();
        drawTextRect.Deflate(rcPadding);
        drawTextRect.Deflate(this->GetTextPadding());

        int32_t nTextRight = drawTextRect.left + textRect.Width();
        int32_t nTextBottom = drawTextRect.top + textRect.Height();

        drawTextRect.right = std::min(drawTextRect.right, nTextRight);
        drawTextRect.bottom = std::min(drawTextRect.bottom, nTextBottom);

        //Set the clip region to avoid drawing over the text area
        pRender->SetClip(drawTextRect, false);
        hasClip = true;
    }

    //Draw the border below the text
    int32_t nLineWidth = m_nLineWidth;
    if (nLineWidth <= 0) {
        nLineWidth = this->Dpi().GetScaleInt(1);
    }
    UiSize cornerSize = m_cornerSize;
    UiColor lineColor;
    if (!m_lineColor.empty()) {
        lineColor = this->GetUiColor(m_lineColor.c_str());
    }
    if (lineColor.GetARGB() == 0) {
        lineColor = GetFadeColor(UiColor(UiColors::Gray), 96);
    }

    int32_t nShadowOffset = 1;//shadow offset
    UiRect rc = this->GetRect();
    rc.Deflate(rcPadding);
    rc.Deflate(nLineWidth / 2 + nShadowOffset, nLineWidth/2 + nShadowOffset);

    if (drawTextRect.Height() > 0) {
        //Center the line vertically relative to the text
        rc.top += (drawTextRect.Height() / 2 - nLineWidth / 2);
    }

    if ((cornerSize.cx > 0) && (cornerSize.cy > 0)) {
        //Draw a shadow effect first
        if (nShadowOffset > 0) {
            UiColor fadeColor = GetFadeColor(lineColor, 24);
            UiRect fadeRect = rc;
            fadeRect.Inflate(nShadowOffset, nShadowOffset);
            pRender->DrawRoundRect(UiRectF::MakeFromRect(fadeRect), (float)cornerSize.cx, (float)cornerSize.cy, fadeColor, (float)nLineWidth);
        }
        //Draw the rounded-rectangle border
        pRender->DrawRoundRect(UiRectF::MakeFromRect(rc), (float)cornerSize.cx, (float)cornerSize.cy, lineColor, (float)nLineWidth);
    }
    else {
        //Draw a shadow effect first
        if (nShadowOffset > 0) {
            UiColor fadeColor = GetFadeColor(lineColor, 24);
            UiRect fadeRect = rc;
            fadeRect.Inflate(nShadowOffset, nShadowOffset);
            pRender->DrawRect(UiRectF::MakeFromRect(fadeRect), fadeColor, (float)nLineWidth);
        }
        //Draw the rectangle border
        pRender->DrawRect(UiRectF::MakeFromRect(rc), lineColor, (float)nLineWidth);
    }

    if (hasClip) {
        //Restore the clip region
        pRender->ClearClip();
    }    
}

/** Group container / vertical group container / horizontal group container
*/
typedef GroupBoxTemplate<Box>  GroupBox;
typedef GroupBoxTemplate<HBox> GroupHBox;
typedef GroupBoxTemplate<VBox> GroupVBox;

}

#endif // UI_CONTROL_GROUPBOX_H_
