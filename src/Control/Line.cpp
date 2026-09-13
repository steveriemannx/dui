#include "dui/Control/Line.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Render/IRender.h"
#include "dui/Utils/StringUtil.h"

namespace ui
{

Line::Line(Window* pWindow):
    Control(pWindow),
    m_bLineVertical(false),
    m_dashStyle((int8_t)IPen::kDashStyleDashDot),
    m_fLineWidth(0)
{
    SetLineWidth(1.0f, true);
}

std::string Line::GetType() const { return DUI_CTR_LINE; }

void Line::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if (strName == "vertical") {
        SetLineVertical(strValue == "true");
    }
    else if (strName == "line_color") {
        SetLineColor(strValue);
    }
    else if (strName == "line_width") {
        if (!strValue.empty()) {
            SetLineWidth((float)StringUtil::StringToInt32(strValue), true);
        }
    }
    else if (strName == "dash_style") {
        SetLineDashStyle(strValue);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void Line::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    float fValue = GetLineWidth();
    fValue = Dpi().GetScaleFloat(fValue, nOldDpiScale);
    SetLineWidth(fValue, false);

    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

void Line::SetLineWidth(float fLineWidth, bool bNeedDpiScale)
{
    if (fLineWidth < 0) {
        fLineWidth = 1.0f;
    }
    if (bNeedDpiScale) {
        fLineWidth = Dpi().GetScaleFloat(fLineWidth);
    }
    if (m_fLineWidth != fLineWidth) {
        m_fLineWidth = fLineWidth;
        Invalidate();
    }
}

float Line::GetLineWidth() const
{
    return m_fLineWidth;
}

void Line::SetLineVertical(bool bVertical)
{
    if (m_bLineVertical != bVertical) {
        m_bLineVertical = bVertical;
        Invalidate();
    }    
}

bool Line::IsLineVertical() const
{
    return m_bLineVertical;
}

void Line::SetLineColor(const std::string& lineColor)
{
    if (m_lineColor != lineColor) {
        m_lineColor = lineColor;
        Invalidate();
    }    
}

std::string Line::GetLineColor() const
{
    return m_lineColor.c_str();
}

void Line::SetLineDashStyle(const std::string& dashStyle)
{
    int32_t oldDashStyle = m_dashStyle;
    if (dashStyle == "solid") {
        m_dashStyle = IPen::kDashStyleSolid;
    }
    else if (dashStyle == "dash") {
        m_dashStyle = IPen::kDashStyleDash;
    }
    else if (dashStyle == "dot") {
        m_dashStyle = IPen::kDashStyleDot;
    }
    else if (dashStyle == "dash_dot") {
        m_dashStyle = IPen::kDashStyleDashDot;
    }
    else if (dashStyle == "dash_dot_dot") {
        m_dashStyle = IPen::kDashStyleDashDotDot;
    }
    else {
        m_dashStyle = IPen::kDashStyleDashDot;
    }
    if (oldDashStyle != m_dashStyle) {
        Invalidate();
    }
}

std::string Line::GetLineDashStyle() const
{
    if (m_dashStyle == IPen::kDashStyleSolid) {
        return "solid";
    }
    else if (m_dashStyle == IPen::kDashStyleDash) {
        return "dash";
    }
    else if (m_dashStyle == IPen::kDashStyleDot) {
        return "dot";
    }
    else if (m_dashStyle == IPen::kDashStyleDashDot) {
        return "dash_dot";
    }
    else if (m_dashStyle == IPen::kDashStyleDashDotDot) {
        return "dash_dot_dot";
    }
    else {
        return "dash_dot";
    }
}

void Line::Paint(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::Paint(pRender, rcPaint);
    if (pRender == nullptr) {
        return;
    }
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory == nullptr) {
        return;
    }

    std::string sLineColor = m_lineColor.c_str();
    if (sLineColor.empty()) {
        sLineColor = GlobalManager::Instance().Color().GetDefaultTextColor();
    }
    UiColor lineColor = GetUiColor(sLineColor);
    float fLineWidth = GetLineWidth();
    if (fLineWidth <= 0) {
        fLineWidth = this->Dpi().GetScaleFloat(1);
    }
    IPen* pLinePen = pRenderFactory->CreatePen(lineColor, fLineWidth);
    if (pLinePen == nullptr) {
        return;
    }
    std::unique_ptr<IPen> spLinePen(pLinePen);
    if (spLinePen == nullptr) {
        return;
    }

    pLinePen->SetDashStyle(static_cast<IPen::DashStyle>(m_dashStyle));

    UiRect rc = GetRect();
    rc.Deflate(GetControlPadding());
    if (!m_bLineVertical) {
        //Horizontal
        UiPointF pt1(rc.left, rc.CenterY());
        UiPointF pt2(rc.right, rc.CenterY());
        pRender->DrawLine(pt1, pt2, pLinePen);
    }
    else {
        //Vertical
        UiPointF pt1(rc.CenterX(), rc.top);
        UiPointF pt2(rc.CenterX(), rc.bottom);
        pRender->DrawLine(pt1, pt2, pLinePen);
    }
}

}//namespace ui

