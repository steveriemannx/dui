#include "duilib/Layout/VirtualHLayout.h"
#include "duilib/Box/VirtualListBox.h"

namespace ui 
{
VirtualHLayout::VirtualHLayout():
    m_bAutoCalcItemHeight(false)
{
    // Align to the center by default
    SetChildVAlignType(VerAlignType::kAlignCenter);
}

bool VirtualHLayout::SetAttribute(const DString& strName, const DString& strValue, const DpiManager& dpiManager)
{
    bool hasAttribute = true;
    if ((strName == _T("item_size")) || (strName == _T("itemsize"))) {
        UiSize szItem;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szItem);
        dpiManager.ScaleSize(szItem);
        SetItemSize(szItem);
    }
    else if (strName == _T("auto_calc_item_size")) {
        SetAutoCalcItemHeight(strValue == _T("true"));
    }
    else {
        hasAttribute = BaseClass::SetAttribute(strName, strValue, dpiManager);
    }
    return hasAttribute;
}

VirtualListBox* VirtualHLayout::GetOwnerBox() const
{
    VirtualListBox* pList = dynamic_cast<VirtualListBox*>(GetOwner());
    ASSERT(pList != nullptr);
    return pList;
}

UiSize64 VirtualHLayout::ArrangeChildren(const std::vector<ui::Control*>& items, ui::UiRect rc, bool bEstimateOnly)
{
    VirtualListBox* pList = dynamic_cast<VirtualListBox*>(GetOwner());
    if ((pList == nullptr) || !pList->HasDataProvider()) {
        // If no data interface is set, fall back to the base class behavior
        return BaseClass::ArrangeChildren(items, rc, bEstimateOnly);
    }
    DeflatePadding(rc);
    if (IsAutoCalcItemHeight()) {
        // Auto-calculation of the child item height is set: fill the entire height
        UiSize szNewItemSize = GetItemSize();
        szNewItemSize.cy = rc.Height();
        SetItemSize(szNewItemSize, false);
    }
    int64_t nTotalWidth = GetElementsWidth(rc, Box::InvalidIndex);
    UiSize64 sz(rc.Width(), rc.Height());
    sz.cx = std::max(nTotalWidth, sz.cx);
    sz.cy = std::max((int64_t)GetItemSize().cy, sz.cy);// Supports a vertical scroll bar
    if (!bEstimateOnly) {
        LazyArrangeChild(rc);
    }    
    return sz;
}

UiSize64 VirtualHLayout::EstimateLayoutSize(const std::vector<Control*>& items, ui::UiSize szAvailable)
{
    // When estimating the control size (mainly for the case where the width or height is "auto"),
    // only the container itself is estimated, excluding the size of the list data,
    // because virtual list data can be large and is not suitable for estimating the size of "auto" controls
    VirtualListBox* pList = dynamic_cast<VirtualListBox*>(GetOwner());
    if ((pList == nullptr) || !pList->HasDataProvider()) {
        // If no data interface is set, fall back to the base class behavior
        return BaseClass::EstimateLayoutSize(items, szAvailable);
    }
    szAvailable.Validate();
    UiPadding rcPadding;
    if (GetOwner() != nullptr) {
        rcPadding = GetOwner()->GetPadding();
    }
    UiSize szAvailableLocal = szAvailable;
    szAvailableLocal.cx -= (rcPadding.left + rcPadding.right);
    szAvailableLocal.cy -= (rcPadding.top + rcPadding.bottom);
    szAvailableLocal.Validate();

    if (IsAutoCalcItemHeight()) {
        // Fixed rows are set, and auto-calculation of the child item height is enabled
        UiSize szNewItemSize = GetItemSize();
        szNewItemSize.cy = szAvailableLocal.cy;
        SetItemSize(szNewItemSize, false);
    }

    UiEstSize estSize;
    if (GetOwner() != nullptr) {
        estSize = GetOwner()->Control::EstimateSize(szAvailableLocal);
    }
    int32_t nTotalWidth = estSize.cx.GetInt32();
    int32_t nTotalHeight = estSize.cy.GetInt32();
    if (estSize.cx.IsStretch()) {
        nTotalWidth = CalcStretchValue(estSize.cx, szAvailableLocal.cx);
        if (nTotalWidth > 0) {
            nTotalWidth += (rcPadding.left + rcPadding.right);
        }
    }
    if (estSize.cy.IsStretch()) {
        nTotalHeight = CalcStretchValue(estSize.cy, szAvailableLocal.cy);
        if (nTotalHeight > 0) {
            nTotalHeight += (rcPadding.top + rcPadding.bottom);
        }
    }
    if (nTotalWidth == 0) {
        UiSize szItem = GetItemSize();
        if (szItem.cx > 0) {
            nTotalWidth = szItem.cx + (rcPadding.left + rcPadding.right);
        }
    }
    if (nTotalHeight == 0) {
        UiSize szItem = GetItemSize();
        if (szItem.cy > 0) {
            nTotalHeight = szItem.cy + (rcPadding.top + rcPadding.bottom);
        }
    }
    nTotalWidth = std::max(nTotalWidth, 0);
    nTotalHeight = std::max(nTotalHeight, 0);
    return UiSize64(nTotalWidth, nTotalHeight);
}

void VirtualHLayout::ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale)
{
    UiSize szItem = GetItemSize();
    szItem = dpiManager.GetScaleSize(szItem, nOldDpiScale);
    SetItemSize(szItem);
    BaseClass::ChangeDpiScale(dpiManager, nOldDpiScale);
}

void VirtualHLayout::SetItemSize(UiSize szItem, bool bArrange)
{
    szItem.cx = std::max(szItem.cx, 0);
    szItem.cy = std::max(szItem.cy, 0);
    ASSERT((szItem.cx > 0) && (szItem.cy > 0));
    if ((m_szItem.cx != szItem.cx) || (m_szItem.cy != szItem.cy)) {
        m_szItem = szItem;
        if (bArrange && (GetOwner() != nullptr)) {
            GetOwner()->Arrange();
        }
    }
}

const UiSize& VirtualHLayout::GetItemSize() const
{
    return m_szItem;
}

void VirtualHLayout::SetAutoCalcItemHeight(bool bAutoCalcItemHeight)
{
    if (m_bAutoCalcItemHeight != bAutoCalcItemHeight) {
        m_bAutoCalcItemHeight = bAutoCalcItemHeight;
        if (GetOwner() != nullptr) {
            GetOwner()->Arrange();
        }
    }
}

bool VirtualHLayout::IsAutoCalcItemHeight() const
{
    return m_bAutoCalcItemHeight;
}

int64_t VirtualHLayout::GetElementsWidth(UiRect /*rc*/, size_t nCount) const
{
    UiSize szItem = GetItemSize();
    ASSERT((szItem.cx > 0) && (szItem.cy > 0));
    if ((szItem.cx <= 0) || (szItem.cy <= 0)) {
        return 0;
    }
    if (nCount <= 1) {
        return (int64_t)szItem.cx + GetChildMarginX();
    }
    if (!Box::IsValidItemIndex(nCount)) {
        VirtualListBox* pList = GetOwnerBox();
        ASSERT(pList != nullptr);
        if (pList != nullptr) {
            nCount = pList->GetElementCount();
        }
    }
    if (!Box::IsValidItemIndex(nCount)) {
        ASSERT(0);
        return (int64_t)szItem.cx + GetChildMarginX();
    }

    int64_t cols = nCount;
    int64_t iChildMargin = 0;
    if (GetChildMarginX() > 0) {
        iChildMargin = GetChildMarginX();
    }
    int64_t childMarginTotal = ((int64_t)nCount - 1) * iChildMargin;
    return szItem.cx * cols + childMarginTotal;
}

void VirtualHLayout::LazyArrangeChild(UiRect rc) const
{
    UiSize szItem = GetItemSize();
    ASSERT((szItem.cx > 0) || (szItem.cy > 0));
    if ((szItem.cx <= 0) || (szItem.cy <= 0)) {
        return;
    }
    VirtualListBox* pOwnerBox = GetOwnerBox();
    if (pOwnerBox == nullptr) {
        return;
    }
    if (!pOwnerBox->HasDataProvider()) {
        return;
    }

    // The X-axis coordinate offset must be preserved to avoid deviations when the interface is refreshed after the scroll position changes
    int32_t xOffset = 0;
    int64_t itemWidth = GetElementsWidth(rc, 1);
    if (itemWidth > 0) {
        xOffset = TruncateToInt32(pOwnerBox->GetScrollPos().cx % itemWidth);
    }

    // Left start position of the child items
    int32_t iPosLeft = rc.left - xOffset;

    // Top start position of the child items
    int32_t iPosTop = rc.top;

    // Determine the alignment
    if (szItem.cy < rc.Height()) {
        VerAlignType vAlign = GetChildVAlignType();
        if (vAlign == VerAlignType::kAlignCenter) {
            iPosTop = rc.CenterY() - szItem.cy / 2;
        }
        else if (vAlign == VerAlignType::kAlignBottom) {
            iPosTop = rc.bottom - szItem.cy;
        }
    }

    // Set the virtual offset; otherwise, when the data volume is large, the height of the 32-bit rect rc may overflow and would require a 64-bit integer
    pOwnerBox->SetScrollVirtualOffsetX(pOwnerBox->GetScrollPos().cx);

    // Top-left coordinates of the control
    ui::UiPoint ptTile(iPosLeft, iPosTop);

    VirtualListBox::RefreshDataList refreshDataList;
    VirtualListBox::RefreshData refreshData;
    // Top index
    size_t nTopIndex = GetTopElementIndex(rc);
    size_t iCount = 0;
    size_t nItemCount = pOwnerBox->m_items.size();
    for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
        Control* pControl = pOwnerBox->m_items[nItemIndex];
        if (pControl == nullptr) {
            continue;
        }
        // Determine size
        ui::UiRect rcTile(ptTile.x, ptTile.y, ptTile.x + szItem.cx, ptTile.y + szItem.cy);
        pControl->SetPos(rcTile);

        // Fill in the data
        size_t nElementIndex = nTopIndex + iCount;
        if (nElementIndex < pOwnerBox->GetElementCount()) {
            if (!pControl->IsVisible()) {
                pControl->SetVisible(true);
            }
            pOwnerBox->FillElementData(pControl, nElementIndex);
            refreshData.nItemIndex = nItemIndex;
            refreshData.pControl = pControl;
            refreshData.nElementIndex = nElementIndex;
            refreshDataList.push_back(refreshData);
        }
        else {
            if (pControl->IsVisible()) {
                pControl->SetVisible(false);
            }
            // The ElementIndex needs to be cleared
            IListBoxItem* pListBoxItem = dynamic_cast<IListBoxItem*>(pControl);
            if (pListBoxItem != nullptr) {
                pListBoxItem->SetElementIndex(Box::InvalidIndex);
            }
        }
        ++iCount;

        // Line wrap
        ptTile.y = iPosTop;
        ptTile.x += szItem.cx + GetChildMarginX();
    }
    if (!refreshDataList.empty()) {
        pOwnerBox->OnRefreshElements(refreshDataList);
        pOwnerBox->OnFilledElements(refreshDataList);
    }
}

size_t VirtualHLayout::AjustMaxItem(UiRect rc) const
{
    UiSize szItem = GetItemSize();
    ASSERT((szItem.cx > 0) || (szItem.cy > 0));
    if ((szItem.cx <= 0) || (szItem.cy <= 0)) {
        return 0;
    }
    if (rc.IsEmpty()) {
        return 0;
    }
    int32_t nColumns = rc.Width() / (szItem.cx + GetChildMarginX() / 2);
    // Verify and correct
    if (nColumns > 1) {
        int32_t calcWidth = nColumns * szItem.cx + (nColumns - 1) * GetChildMarginX();
        if (calcWidth < rc.Width()) {
            nColumns += 1;
        }
    }
    // Add one extra column to ensure the real controls fill the entire displayable area
    nColumns += 1;
    return nColumns;
}

size_t VirtualHLayout::GetTopElementIndex(UiRect rc) const
{
    VirtualListBox* pOwnerBox = GetOwnerBox();
    if (pOwnerBox == nullptr) {
        return 0;
    }
    int64_t nPos = pOwnerBox->GetScrollPos().cx;
    if (nPos < 0) {
        nPos = 0;
    }
    int64_t nWidth = GetElementsWidth(rc, 1);
    ASSERT(nWidth >= 0);
    if (nWidth <= 0) {
        return 0;
    }
    int64_t iIndex = nPos / nWidth;
    return static_cast<size_t>(iIndex);
}

bool VirtualHLayout::IsElementDisplay(UiRect rc, size_t iIndex) const
{
    if (!Box::IsValidItemIndex(iIndex)) {
        return false;
    }
    VirtualListBox* pOwnerBox = GetOwnerBox();
    if (pOwnerBox == nullptr) {
        return false;
    }

    int64_t nScrollPos = pOwnerBox->GetScrollPos().cx;
    int64_t nElementPos = GetElementsWidth(rc, iIndex + 1);
    int64_t nElementWidth = GetElementsWidth(rc, 1);
    if ((nElementPos - nElementWidth) > nScrollPos) { // Left position of the rect
        int64_t nBoxWidth = pOwnerBox->GetWidth();
        if (nElementPos <= (nScrollPos + nBoxWidth)) {// Right position of the rect
            return true;
        }
    }
    return false;
}

bool VirtualHLayout::NeedReArrange() const
{
    VirtualListBox* pOwnerBox = GetOwnerBox();
    if (pOwnerBox == nullptr) {
        return false;
    }
    if (!pOwnerBox->HasDataProvider()) {
        return false;
    }
    size_t nCount = pOwnerBox->GetItemCount();
    if (nCount == 0) {
        return false;
    }

    if (pOwnerBox->GetElementCount() <= nCount) {
        return false;
    }

    ui::UiRect rcThis = pOwnerBox->GetPos();
    if (rcThis.IsEmpty()) {
        return false;
    }

    int64_t nScrollPosX = pOwnerBox->GetScrollPos().cx;
    int64_t nVirtualOffsetX = pOwnerBox->GetScrollVirtualOffset().cx;
    return nVirtualOffsetX != nScrollPosX;
}

void VirtualHLayout::GetDisplayElements(UiRect rc, std::vector<size_t>& collection) const
{
    collection.clear();
    VirtualListBox* pOwnerBox = GetOwnerBox();
    if (pOwnerBox == nullptr) {
        return;
    }

    if (pOwnerBox->GetItemCount() == 0) {
        return;
    }

    int64_t nEleWidth = GetElementsWidth(rc, 1);
    if (nEleWidth <= 0) {
        return;
    }

    size_t min = (size_t)(pOwnerBox->GetScrollPos().cx / nEleWidth);
    size_t max = min + (size_t)(rc.Width() / nEleWidth);

    size_t nCount = pOwnerBox->GetElementCount();
    if (nCount > 0) {
        if (max >= nCount) {
            max = nCount - 1;
        }
    }
    else {
        return;
    }

    for (size_t i = min; i <= max; ++i) {
        collection.push_back(i);
    }
}

void VirtualHLayout::EnsureVisible(UiRect rc, size_t iIndex, bool bToTop) const
{
    VirtualListBox* pOwnerBox = GetOwnerBox();
    if (pOwnerBox == nullptr) {
        return;
    }
    if (!Box::IsValidItemIndex(iIndex) || iIndex >= pOwnerBox->GetElementCount()) {
        return;
    }
    if (pOwnerBox->GetHScrollBar() == nullptr) {
        return;
    }
    int64_t nPos = pOwnerBox->GetScrollPos().cx;
    int64_t elementWidth = GetElementsWidth(rc, 1);
    if (elementWidth <= 0) {
        return;
    }
    int64_t nTopIndex = (nPos / elementWidth);
    int64_t nNewPos = 0;

    if (bToTop) {
        nNewPos = GetElementsWidth(rc, iIndex);
        if (nNewPos >= elementWidth) {
            nNewPos -= elementWidth;
        }
    }
    else {
        if (IsElementDisplay(rc, iIndex)) {
            return;
        }

        if ((int64_t)iIndex > nTopIndex) {
            // Downward
            int64_t nWidth = GetElementsWidth(rc, iIndex + 1);
            nNewPos = nWidth - pOwnerBox->GetRect().Width();
        }
        else {
            // Upward
            nNewPos = GetElementsWidth(rc, iIndex + 1);
            if (nNewPos >= elementWidth) {
                nNewPos -= elementWidth;
            }
        }
    }
    if (nNewPos < 0) {
        nNewPos = 0;
    }
    if (nNewPos > pOwnerBox->GetHScrollBar()->GetScrollRange()) {
        nNewPos = pOwnerBox->GetHScrollBar()->GetScrollRange();
    }
    ui::UiSize64 sz(nNewPos, 0);
    pOwnerBox->SetScrollPos(sz);
}
} // namespace ui
