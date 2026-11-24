#include "dui/Control/StatusBar.h"
#include "dui/Render/IRender.h"
#include "dui/Core/DpiManager.h"
#include "dui/Core/Window.h"
#include "dui/Control/LabelImpl.h"
#include "dui/Utils/StringUtil.h"
#include <vector>

namespace ui
{

StatusBar::StatusBar(Window* pWindow) :
    HBox(pWindow),
    m_nSeparatorWidth(1),
    m_separatorColor("border_window"),
    m_bShowSizeGrip(false),
    m_bSizeGripResize(true),
    m_sizeGripColor("border_window"),
    m_bSizeBoxBackupValid(false)
{
}

std::string StatusBar::GetType() const { return DUI_CTR_STATUS_BAR; }

void StatusBar::SetAttribute(const std::string& strName, const std::string& strValue2)
{
    //dui does not support variable expansion in attribute values, so the value is used as is
    const std::string& strValue = strValue2;
    if ((strName == "separator_width") || (strName == "separatorwidth")) {
        SetSeparatorWidth(StringUtil::StringToInt32(strValue));
    }
    else if ((strName == "separator_color") || (strName == "separatorcolor")) {
        SetSeparatorColor(strValue);
    }
    else if ((strName == "size_grip") || (strName == "sizegrip")) {
        //dui has no StringUtil::IsValueTrue; booleans are compared against "true", as in the other dui controls
        SetShowSizeGrip(strValue == "true");
    }
    else if ((strName == "size_grip_resize") || (strName == "sizegripresize")) {
        SetSizeGripResize(strValue == "true");
    }
    else if ((strName == "size_grip_color") || (strName == "sizegripcolor")) {
        SetSizeGripColor(strValue);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

size_t StatusBar::GetPanelCount() const
{
    size_t nCount = 0;
    const size_t nItemCount = GetItemCount();
    for (size_t nIndex = 0; nIndex < nItemCount; ++nIndex) {
        Control* pItem = GetItemAt(nIndex);
        if ((pItem != nullptr) && pItem->IsVisible()) {
            ++nCount;
        }
    }
    return nCount;
}

Control* StatusBar::GetPanel(size_t nIndex) const
{
    size_t nSeen = 0;
    const size_t nItemCount = GetItemCount();
    for (size_t i = 0; i < nItemCount; ++i) {
        Control* pItem = GetItemAt(i);
        if ((pItem == nullptr) || !pItem->IsVisible()) {
            continue;
        }
        if (nSeen == nIndex) {
            return pItem;
        }
        ++nSeen;
    }
    return nullptr;
}

int32_t StatusBar::GetPanelIndex(Control* pPanel) const
{
    if (pPanel == nullptr) {
        return -1;
    }
    int32_t nIndex = 0;
    const size_t nItemCount = GetItemCount();
    for (size_t i = 0; i < nItemCount; ++i) {
        Control* pItem = GetItemAt(i);
        if ((pItem == nullptr) || !pItem->IsVisible()) {
            continue;
        }
        if (pItem == pPanel) {
            return nIndex;
        }
        ++nIndex;
    }
    return -1;
}

Control* StatusBar::FindPanel(const std::string& strName) const
{
    if (strName.empty()) {
        return nullptr;
    }
    const size_t nItemCount = GetItemCount();
    for (size_t i = 0; i < nItemCount; ++i) {
        Control* pItem = GetItemAt(i);
        if ((pItem != nullptr) && pItem->IsVisible() && (pItem->GetName() == strName)) {
            return pItem;
        }
    }
    return nullptr;
}

bool StatusBar::SetPanelText(size_t nIndex, const std::string& strText)
{
    Control* pPanel = GetPanel(nIndex);
    if (pPanel == nullptr) {
        return false;
    }
    LabelOwner* pTextPanel = dynamic_cast<LabelOwner*>(pPanel);
    if (pTextPanel == nullptr) {
        //The panel is not a text control (such as Progress or a custom self-drawn panel), so no text can be set
        return false;
    }
    pTextPanel->SetText(strText);
    return true;
}

std::string StatusBar::GetPanelText(size_t nIndex) const
{
    Control* pPanel = GetPanel(nIndex);
    if (pPanel == nullptr) {
        return std::string();
    }
    LabelOwner* pTextPanel = dynamic_cast<LabelOwner*>(pPanel);
    if (pTextPanel == nullptr) {
        return std::string();
    }
    return pTextPanel->GetText();
}

void StatusBar::SetSeparatorWidth(int32_t nWidth)
{
    if (m_nSeparatorWidth != nWidth) {
        m_nSeparatorWidth = nWidth;
        Invalidate();
    }
}

int32_t StatusBar::GetSeparatorWidth() const
{
    return m_nSeparatorWidth;
}

void StatusBar::SetSeparatorColor(const std::string& strColor)
{
    if (m_separatorColor != strColor) {
        m_separatorColor = strColor;
        Invalidate();
    }
}

std::string StatusBar::GetSeparatorColor() const
{
    return m_separatorColor.c_str();
}

void StatusBar::SetShowSizeGrip(bool bShow)
{
    if (m_bShowSizeGrip != bShow) {
        m_bShowSizeGrip = bShow;
        Invalidate();
        //The draggable resize area of the window has to be refreshed after the grip switch changed
        UpdateSizeGripHitArea();
    }
}

bool StatusBar::IsShowSizeGrip() const
{
    return m_bShowSizeGrip;
}

void StatusBar::SetSizeGripResize(bool bResize)
{
    if (m_bSizeGripResize != bResize) {
        m_bSizeGripResize = bResize;
        if (!bResize) {
            //When the drag resize is turned off, restore the window size_box that was enlarged before immediately
            RestoreSizeBox();
        }
        else {
            UpdateSizeGripHitArea();
        }
    }
}

bool StatusBar::IsSizeGripResize() const
{
    return m_bSizeGripResize;
}

void StatusBar::SetSizeGripColor(const std::string& strColor)
{
    if (m_sizeGripColor != strColor) {
        m_sizeGripColor = strColor;
        Invalidate();
    }
}

std::string StatusBar::GetSizeGripColor() const
{
    return m_sizeGripColor.c_str();
}

void StatusBar::PaintChild(IRender* pRender, const UiRect& rcPaint)
{
    //Paint all child controls first (the individual status panels)
    BaseClass::PaintChild(pRender, rcPaint);

    if (pRender == nullptr) {
        return;
    }
    UiRect rcContent = GetRect();
    if (rcContent.IsEmpty()) {
        return;
    }
    rcContent.Deflate(GetPadding());

    //The separators are painted on top of the child controls, so they are not covered by the panel backgrounds
    PaintSeparator(pRender, rcContent);
    if (m_bShowSizeGrip) {
        PaintSizeGrip(pRender, rcContent);
    }
}

void StatusBar::PaintSeparator(IRender* pRender, const UiRect& rcContent)
{
    const int32_t nWidth = Dpi().GetScaleInt(m_nSeparatorWidth);
    if (nWidth <= 0) {
        return;
    }

    //Collect the rectangles of the visible child controls in layout order
    std::vector<UiRect> itemRects;
    const size_t nCount = GetItemCount();
    for (size_t nIndex = 0; nIndex < nCount; ++nIndex) {
        Control* pItem = GetItemAt(nIndex);
        if ((pItem != nullptr) && pItem->IsVisible() && !pItem->GetRect().IsEmpty()) {
            itemRects.push_back(pItem->GetRect());
        }
    }
    if (itemRects.size() < 2) {
        //No separator when there are fewer than two panels
        return;
    }

    const UiColor color = GetUiColor(m_separatorColor.c_str());

    //Leave a small gap above and below the separator, so it does not fill the whole status bar
    const int32_t nInset = Dpi().GetScaleInt(2);
    int32_t nTop = rcContent.top + nInset;
    int32_t nBottom = rcContent.bottom - nInset;
    if (nBottom <= nTop) {
        nTop = rcContent.top;
        nBottom = rcContent.bottom;
    }

    for (size_t nIndex = 1; nIndex < itemRects.size(); ++nIndex) {
        //The middle position between two adjacent panels is used as the separator position
        const int32_t x = (itemRects[nIndex - 1].right + itemRects[nIndex].left) / 2;
        UiRect rcLine;
        rcLine.left = x;
        rcLine.right = x + nWidth;
        rcLine.top = nTop;
        rcLine.bottom = nBottom;
        if ((rcLine.right <= rcContent.left) || (rcLine.left >= rcContent.right)) {
            //The separator falls outside the content area, skip it
            continue;
        }
        pRender->FillRect(UiRectF::MakeFromRect(rcLine), color);
    }
}

void StatusBar::PaintSizeGrip(IRender* pRender, const UiRect& rcContent)
{
    const int32_t nMargin = Dpi().GetScaleInt(2);
    const int32_t nStep = Dpi().GetScaleInt(3);
    //When even the first diagonal line does not fit, the whole grip is not painted
    if ((rcContent.right - nMargin - nStep <= rcContent.left) ||
        (rcContent.bottom - nMargin - nStep <= rcContent.top)) {
        return;
    }

    const UiColor color = GetUiColor(m_sizeGripColor.c_str());
    const float fLineWidth = Dpi().GetScaleFloat(1.0f);

    const int32_t xRight = rcContent.right - nMargin;
    const int32_t yBottom = rcContent.bottom - nMargin;
    for (int32_t i = 0; i < 3; ++i) {
        const int32_t nOffset = i * nStep;
        const int32_t x1 = xRight - nOffset - nStep;
        const int32_t y1 = yBottom;
        const int32_t x2 = xRight;
        const int32_t y2 = yBottom - nOffset - nStep;
        if ((x1 < rcContent.left) || (y2 < rcContent.top)) {
            //Not enough room left, stop painting
            break;
        }
        pRender->DrawLine(UiPointF((float)x1, (float)y1), UiPointF((float)x2, (float)y2), color, fLineWidth);
    }
}

UiRect StatusBar::GetSizeGripRect(const UiRect& rcContent) const
{
    const int32_t nMargin = Dpi().GetScaleInt(2);
    const int32_t nStep = Dpi().GetScaleInt(3);
    //The three diagonal lines form a 3*nStep square area; the bottom-right anchor is described in PaintSizeGrip
    UiRect rcGrip;
    rcGrip.left = rcContent.right - nMargin - 3 * nStep;
    rcGrip.top = rcContent.bottom - nMargin - 3 * nStep;
    rcGrip.right = rcContent.right - nMargin;
    rcGrip.bottom = rcContent.bottom - nMargin;
    if ((rcGrip.right <= rcGrip.left) || (rcGrip.bottom <= rcGrip.top)) {
        //Not enough room, the grip is not painted
        rcGrip.Clear();
    }
    return rcGrip;
}

int32_t StatusBar::GetSizeGripHitSize() const
{
    const int32_t nDefault = Dpi().GetScaleInt(12);
    UiRect rcContent = GetRect();
    if (rcContent.IsEmpty()) {
        return nDefault > 0 ? nDefault : 1;
    }
    rcContent.Deflate(GetPadding());

    const UiRect rcGrip = GetSizeGripRect(rcContent);
    if (rcGrip.IsEmpty()) {
        return nDefault > 0 ? nDefault : 1;
    }

    //The hit area uses the distance from the grip to the bottom-right corner of the content area, plus 1 logical
    //pixel of margin
    const int32_t nDx = rcContent.right - rcGrip.left;
    const int32_t nDy = rcContent.bottom - rcGrip.top;
    int32_t nNeed = (nDx > nDy) ? nDx : nDy;
    nNeed += Dpi().GetScaleInt(1);
    return nNeed > 0 ? nNeed : 1;
}

void StatusBar::SetPos(UiRect rc)
{
    BaseClass::SetPos(rc);
    //After the layout position changed, the grip may have moved into or out of the bottom-right corner of the
    //window, so the drag hit area has to be refreshed
    UpdateSizeGripHitArea();
}

void StatusBar::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
    //After a DPI change the window scales size_box by itself, so the previous backup value is no longer valid; the
    //value is recomputed directly with the new scale
    m_bSizeBoxBackupValid = false;
    UpdateSizeGripHitArea();
}

void StatusBar::UpdateSizeGripHitArea()
{
    Window* pWindow = GetWindow();
    if (pWindow == nullptr) {
        return;
    }

    //Decide whether this status bar is docked in the bottom-right corner of the window client area (a layout
    //tolerance of 1 logical pixel is allowed)
    bool bAtRightBottom = false;
    if (m_bShowSizeGrip && m_bSizeGripResize && pWindow->IsWindow()) {
        const UiRect rcSelf = GetRect();
        if (!rcSelf.IsEmpty()) {
            UiRect rcClient;
            pWindow->GetClientRect(rcClient);
            const int32_t nTolerance = Dpi().GetScaleInt(1);
            const int32_t nDx = rcSelf.right > rcClient.right ? (rcSelf.right - rcClient.right)
                                                             : (rcClient.right - rcSelf.right);
            const int32_t nDy = rcSelf.bottom > rcClient.bottom ? (rcSelf.bottom - rcClient.bottom)
                                                               : (rcClient.bottom - rcSelf.bottom);
            bAtRightBottom = (nDx <= nTolerance) && (nDy <= nTolerance);
        }
    }

    if (!bAtRightBottom) {
        //Not in the bottom-right corner (or the grip is turned off): restore the window size_box enlarged before
        RestoreSizeBox();
        return;
    }

    const int32_t nNeed = GetSizeGripHitSize();
    UiRect rcSizeBox = pWindow->GetSizeBox();
    if ((rcSizeBox.right >= nNeed) && (rcSizeBox.bottom >= nNeed)) {
        //The resizable area of the window already covers the grip, nothing to change
        return;
    }

    if (!m_bSizeBoxBackupValid) {
        //Remember the original value, so it can be restored once this control is no longer docked in the
        //bottom-right corner
        m_rcSizeBoxBackup = rcSizeBox;
        m_bSizeBoxBackupValid = true;
    }
    if (rcSizeBox.right < nNeed) {
        rcSizeBox.right = nNeed;
    }
    if (rcSizeBox.bottom < nNeed) {
        rcSizeBox.bottom = nNeed;
    }
    m_rcSizeBoxApplied = rcSizeBox;
    //The value passed in is already DPI scaled, so it must not be scaled again
    pWindow->SetSizeBox(rcSizeBox, false);
}

void StatusBar::RestoreSizeBox()
{
    if (!m_bSizeBoxBackupValid) {
        return;
    }
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        const UiRect rcCurrent = pWindow->GetSizeBox();
        bool bUnchanged = (rcCurrent.left == m_rcSizeBoxApplied.left) &&
                          (rcCurrent.top == m_rcSizeBoxApplied.top) &&
                          (rcCurrent.right == m_rcSizeBoxApplied.right) &&
                          (rcCurrent.bottom == m_rcSizeBoxApplied.bottom);
        if (bUnchanged) {
            //Restore only when the current value is still the value written by this control, so the application's own
            //settings are not overwritten
            pWindow->SetSizeBox(m_rcSizeBoxBackup, false);
        }
    }
    m_bSizeBoxBackupValid = false;
}

} // namespace ui
