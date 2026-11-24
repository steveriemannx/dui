#include "dui/Control/Mask.h"
#include "dui/Core/ControlFinder.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Window.h"
#include "dui/Render/IRender.h"
#include "dui/Utils/StringUtil.h"

#include <algorithm>

namespace ui
{

Mask::Mask(Window* pWindow) :
    BaseClass(pWindow),
    m_strMaskColor("bg_overlay"),
    m_nHighlightPadding(0),
    m_nHighlightCorner(0),
    m_bClickThrough(false)
{
    SetHighlightPadding(4);
}

Mask::~Mask()
{
}

std::string Mask::GetType() const
{
    return DUI_CTR_MASK;
}

void Mask::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if ((strName == "mask_color") || (strName == "maskcolor")) {
        SetMaskColor(strValue);
    }
    else if ((strName == "highlight_control") || (strName == "highlightcontrol")) {
        SetHighlightControl(strValue);
    }
    else if ((strName == "highlight_padding") || (strName == "highlightpadding")) {
        SetHighlightPadding(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "highlight_corner") || (strName == "highlightcorner")) {
        SetHighlightCorner(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "click_through") || (strName == "clickthrough")) {
        //dui has no StringUtil::IsValueTrue; booleans are compared against "true", as in the other dui controls
        SetClickThrough(strValue == "true");
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void Mask::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    //The cut-out padding and the corner radius are stored as physical pixels after DPI scaling, so on a DPI change
    //they are converted from the old scale factor to the new one
    m_nHighlightPadding = Dpi().GetScaleInt(m_nHighlightPadding, nOldDpiScale);
    m_nHighlightCorner = Dpi().GetScaleInt(m_nHighlightCorner, nOldDpiScale);
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

void Mask::SetMaskColor(const std::string& strColor)
{
    if (m_strMaskColor != strColor) {
        m_strMaskColor = strColor;
        Invalidate();
    }
}

void Mask::SetHighlightControl(const std::string& strControlName)
{
    if (m_strHighlightControl != strControlName) {
        m_strHighlightControl = strControlName;
        Invalidate();
    }
}

void Mask::SetHighlightPadding(int32_t nPadding, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        nPadding = Dpi().GetScaleInt(nPadding);
    }
    if (nPadding < 0) {
        nPadding = 0;
    }
    if (m_nHighlightPadding != nPadding) {
        m_nHighlightPadding = nPadding;
        Invalidate();
    }
}

void Mask::SetHighlightCorner(int32_t nCorner, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        nCorner = Dpi().GetScaleInt(nCorner);
    }
    if (nCorner < 0) {
        nCorner = 0;
    }
    if (m_nHighlightCorner != nCorner) {
        m_nHighlightCorner = nCorner;
        Invalidate();
    }
}

void Mask::SetClickThrough(bool bThrough)
{
    m_bClickThrough = bThrough;
}

bool Mask::GetHighlightRect(UiRect& rcHighlight)
{
    rcHighlight.Clear();
    if (m_strHighlightControl.empty()) {
        return false;
    }
    //Note: Control::FindControl(name) cannot be used here, because it only searches its own subtree;
    //the highlight target is usually a sibling control outside the Mask, so the search starts at the window root
    Control* pControl = nullptr;
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        pControl = pWindow->FindControl(m_strHighlightControl);
    }
    if (pControl == nullptr) {
        return false;
    }
    UiRect rc = pControl->GetRect();
    if (rc.IsEmpty()) {
        return false;
    }
    rc.Inflate(m_nHighlightPadding, m_nHighlightPadding);
    //Clamp to the bounds of this control
    const UiRect rcSelf = GetRect();
    rc.left = std::max(rc.left, rcSelf.left);
    rc.top = std::max(rc.top, rcSelf.top);
    rc.right = std::min(rc.right, rcSelf.right);
    rc.bottom = std::min(rc.bottom, rcSelf.bottom);
    rc.Validate();
    if (rc.IsEmpty()) {
        return false;
    }
    rcHighlight = rc;
    return true;
}

void Mask::AddRoundRectPath(IPath* pPath, const UiRectF& rc, float rx, float ry)
{
    ASSERT(pPath != nullptr);
    if (pPath == nullptr) {
        return;
    }
    rx = std::min(rx, rc.Width() / 2);
    ry = std::min(ry, rc.Height() / 2);
    rx = std::max(rx, 0.0f);
    ry = std::max(ry, 0.0f);
    if ((rx <= 0.01f) || (ry <= 0.01f)) {
        pPath->AddRect(rc);
        return;
    }
    //Draw clockwise: top-left arc -> top edge -> top-right arc -> right edge -> bottom-right arc -> bottom edge -> bottom-left arc -> left edge
    pPath->AddArc(UiRectF(rc.left, rc.top, rc.left + rx * 2, rc.top + ry * 2), 180.0f, 90.0f);
    pPath->AddLine(rc.left + rx, rc.top, rc.right - rx, rc.top);
    pPath->AddArc(UiRectF(rc.right - rx * 2, rc.top, rc.right, rc.top + ry * 2), 270.0f, 90.0f);
    pPath->AddLine(rc.right, rc.top + ry, rc.right, rc.bottom - ry);
    pPath->AddArc(UiRectF(rc.right - rx * 2, rc.bottom - ry * 2, rc.right, rc.bottom), 0.0f, 90.0f);
    pPath->AddLine(rc.right - rx, rc.bottom, rc.left + rx, rc.bottom);
    pPath->AddArc(UiRectF(rc.left, rc.bottom - ry * 2, rc.left + rx * 2, rc.bottom), 90.0f, 90.0f);
    pPath->AddLine(rc.left, rc.bottom - ry, rc.left, rc.top + ry);
    pPath->Close();
}

void Mask::PaintBkColor(IRender* pRender)
{
    if (pRender == nullptr) {
        return;
    }
    UiColor maskColor = GetUiColor(m_strMaskColor);
    if (maskColor.IsEmpty()) {
        return;
    }
    const UiRect rc = GetRect();
    if (rc.IsEmpty()) {
        return;
    }

    UiRect rcHighlight;
    if (!GetHighlightRect(rcHighlight)) {
        //No cut-out: fill the whole rectangle
        pRender->FillRect(UiRectF::MakeFromRect(rc), maskColor);
        return;
    }

    //With a cut-out: build an even-odd fill path from the outer rectangle plus the inner rounded rectangle to punch out the highlight area
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory == nullptr) {
        pRender->FillRect(UiRectF::MakeFromRect(rc), maskColor);
        return;
    }
    std::unique_ptr<IPath> pPath(pRenderFactory->CreatePath());
    if (pPath == nullptr) {
        pRender->FillRect(UiRectF::MakeFromRect(rc), maskColor);
        return;
    }
    pPath->SetFillType(IPath::FillType::kEvenOdd);
    pPath->AddRect(UiRectF::MakeFromRect(rc));
    const float fRadius = (float)m_nHighlightCorner;
    AddRoundRectPath(pPath.get(), UiRectF::MakeFromRect(rcHighlight), fRadius, fRadius);

    std::unique_ptr<IBrush> pBrush(pRenderFactory->CreateBrush(maskColor));
    if (pBrush != nullptr) {
        pRender->FillPath(pPath.get(), pBrush.get());
    }
}

Control* Mask::FindControl(FINDCONTROLPROC Proc, void* pProcData,
                           uint32_t uFlags, const UiPoint& ptMouse, const UiPoint& scrollPos)
{
    if (((uFlags & UIFIND_HITTEST) != 0) && m_bClickThrough) {
        UiRect rcHighlight;
        if (GetHighlightRect(rcHighlight) && rcHighlight.ContainsPt(ptMouse)) {
            //The click landed in the cut-out area: return nullptr so the outer container keeps searching the controls below (this implements click-through)
            return nullptr;
        }
    }
    return BaseClass::FindControl(Proc, pProcData, uFlags, ptMouse, scrollPos);
}

Mask* Mask::ShowOverWindow(Window* pWindow)
{
    if (pWindow == nullptr) {
        return nullptr;
    }
    Box* pRoot = pWindow->GetXmlRoot();
    if (pRoot == nullptr) {
        return nullptr;
    }
    Mask* pMask = new Mask(pWindow);
    pMask->SetFloat(true);
    pMask->SetFixedWidth(UiFixedInt::MakeStretch(), true, false);
    pMask->SetFixedHeight(UiFixedInt::MakeStretch(), true, false);
    pMask->SetPaintOrder(255);
    if (!pRoot->AddItem(pMask)) {
        delete pMask;
        return nullptr;
    }
    return pMask;
}

void Mask::Remove()
{
    Box* pParent = GetParent();
    if (pParent == nullptr) {
        return;
    }
    //Note: when the parent container destroys its children automatically, RemoveItem destroys this object and the pointer must not be used afterwards
    pParent->RemoveItem(this);
}

} // namespace ui
