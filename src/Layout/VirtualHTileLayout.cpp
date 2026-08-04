#include "dui/Layout/VirtualHTileLayout.h"
#include "dui/Box/VirtualListBox.h"

namespace ui 
{

VirtualHTileLayout::VirtualHTileLayout()
{
    SetRows(0);
    SetAutoCalcRows(true);

    // Default: center-aligned in the vertical direction
    SetChildVAlignType(VerAlignType::kAlignCenter);
}

VirtualListBox* VirtualHTileLayout::GetOwnerBox() const
{
    VirtualListBox* pList = dynamic_cast<VirtualListBox*>(GetOwner());
    ASSERT(pList != nullptr);
    return pList;
}

int32_t VirtualHTileLayout::CalcTileRows(int32_t rcHeight) const
{
    int32_t nRows = GetRows();
    if (nRows < 0) {
        nRows = 0;
    }
    if (IsAutoCalcRows()) {
        nRows = 0;
    }
    if (nRows > 0) {
        return nRows;
    }
    int32_t totalHeight = rcHeight;
    int32_t tileHeight = GetItemSize().cy;
    int32_t childMarginY = GetChildMarginY();
    if (childMarginY < 0) {
        childMarginY = 0;
    }
    if (tileHeight > 0) {
        while (totalHeight > 0) {
            totalHeight -= tileHeight;
            if (nRows != 0) {
                totalHeight -= childMarginY;
            }
            if (totalHeight >= 0) {
                ++nRows;
            }
        }
    }
    if (nRows < 1) {
        nRows = 1;
    }
    return nRows;
}

UiSize64 VirtualHTileLayout::ArrangeChildren(const std::vector<ui::Control*>& items, ui::UiRect rc, bool bEstimateOnly)
{
    VirtualListBox* pList = dynamic_cast<VirtualListBox*>(GetOwner());
    if ((pList == nullptr) || !pList->HasDataProvider()) {
        // If no data provider is set, fall back to the base class behavior
        return BaseClass::ArrangeChildren(items, rc, bEstimateOnly);
    }
    DeflatePadding(rc);
    if ((GetRows() > 0) && IsAutoCalcItemHeight()) {
        // Fixed rows are configured, and auto-calculating the child item height is enabled
        int32_t nNewItemHeight = 0;
        if (AutoCalcItemHeight(GetRows(), GetChildMarginY(), rc.Height(), nNewItemHeight)) {
            UiSize szNewItemSize = GetItemSize();
            szNewItemSize.cy = nNewItemHeight;
            SetItemSize(szNewItemSize, false);
        }
    }

    // Calculate the total width
    int64_t nTotalWidth = GetElementsWidth(rc, Box::InvalidIndex);
    UiSize64 sz(rc.Width(), rc.Height());
    sz.cx = std::max(nTotalWidth, sz.cx);

    // Calculate the height needed in the vertical direction to support the vertical scroll bar
    int32_t nRows = CalcTileRows(rc.Height()); // Number of rows
    const UiSize szItem = GetItemSize();
    int64_t cyHeight = szItem.cy * nRows + GetChildMarginY() * (nRows - 1);
    sz.cy = std::max(cyHeight, sz.cy);

    if (!bEstimateOnly) {
        LazyArrangeChild(rc);
    }    
    return sz;
}

UiSize64 VirtualHTileLayout::EstimateLayoutSize(const std::vector<Control*>& items, ui::UiSize szAvailable)
{
    // When estimating the control size (mainly for the case where the width/height is of type "auto"), only estimate the container's own size, not including the size of the list data
    // Because the virtual list data can be very large, it is not suitable for estimating the size of "auto" controls
    VirtualListBox* pList = dynamic_cast<VirtualListBox*>(GetOwner());
    if ((pList == nullptr) || !pList->HasDataProvider()) {
        // If no data provider is set, fall back to the base class behavior
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

    if ((GetRows() > 0) && IsAutoCalcItemHeight()) {
        // Fixed rows are configured, and auto-calculating the child item height is enabled
        int32_t nNewItemHeight = 0;
        if (AutoCalcItemHeight(GetRows(), GetChildMarginY(), szAvailableLocal.cy, nNewItemHeight)) {
            UiSize szNewItemSize = GetItemSize();
            szNewItemSize.cy = nNewItemHeight;
            SetItemSize(szNewItemSize, false);
        }
    }
    UiSize szItem = GetItemSize();
    ASSERT((szItem.cx > 0) && (szItem.cy > 0));
    if ((szItem.cx <= 0) || (szItem.cy <= 0)) {
        return UiSize64();
    }    
    int32_t nRows = CalcTileRows(szAvailable.cy);
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
    if ((nTotalHeight == 0) && (nRows > 0)) {
        nTotalHeight = szItem.cy * nRows + GetChildMarginY() * (nRows - 1);
        if (nTotalHeight > 0) {
            nTotalHeight += (rcPadding.top + rcPadding.bottom);
        }
    }
    nTotalWidth = std::max(nTotalWidth, 0);
    nTotalHeight = std::max(nTotalHeight, 0);
    return UiSize64(nTotalWidth, nTotalHeight);
}

int64_t VirtualHTileLayout::GetElementsWidth(const UiRect& rc, size_t nCount) const
{
    UiSize szItem = GetItemSize();
    ASSERT((szItem.cx > 0) && (szItem.cy > 0));
    if ((szItem.cx <= 0) || (szItem.cy <= 0)) {
        return 0;
    }
    int32_t nRows = CalcTileRows(rc.Height());
    if (nCount <= (size_t)nRows && nCount != Box::InvalidIndex) {
        // Less than one column, or exactly one column
        return (int64_t)szItem.cx + GetChildMarginX();
    }
    if (!Box::IsValidItemIndex(nCount)) {
        VirtualListBox* pList = dynamic_cast<VirtualListBox*>(GetOwner());
        ASSERT(pList != nullptr);
        if (pList != nullptr) {
            nCount = pList->GetElementCount();
        }
    }
    if (!Box::IsValidItemIndex(nCount)) {
        ASSERT(0);
        return (int64_t)szItem.cx + GetChildMarginX();
    }

    int64_t cols = nCount / nRows;
    if (nCount % nRows != 0) {
        cols += 1;
    }
    int64_t iChildMargin = 0;
    if (GetChildMarginX() > 0) {
        iChildMargin = GetChildMarginX();
    }
    if (nCount > 0) {
        int64_t childMarginTotal = 0;
        if (nCount % nRows == 0) {
            childMarginTotal = ((int64_t)nCount / nRows - 1) * iChildMargin;
        }
        else {
            childMarginTotal = ((int64_t)nCount / nRows) * iChildMargin;
        }
        return szItem.cx * cols + childMarginTotal;
    }
    return 0;
}

void VirtualHTileLayout::LazyArrangeChild(UiRect rc) const
{
    UiSize szItem = GetItemSize();
    ASSERT((szItem.cx > 0) && (szItem.cy > 0));
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

    // Number of rows
    int32_t nRows = CalcTileRows(rc.Height());

    // Top start position of child items
    int32_t iPosTop = rc.top;

    // Determine the alignment
    int32_t cyTotal = nRows * szItem.cy;
    if (nRows > 1) {
        cyTotal += (nRows - 1) * GetChildMarginY();
    }
    if (cyTotal < rc.Height()) {
        VerAlignType vAlign = GetChildVAlignType();
        if (vAlign == VerAlignType::kAlignCenter) {
            // Center-aligned
            iPosTop = rc.CenterY() - cyTotal / 2;
        }
        else if (vAlign == VerAlignType::kAlignBottom) {
            // Bottom-aligned
            iPosTop = rc.bottom - cyTotal;
        }
    }

    // The X-axis offset must be preserved to avoid misalignment when the interface is refreshed after the scroll position changes
    int32_t xOffset = 0;
    int64_t itemWidth = GetElementsWidth(rc, 1);
    if (itemWidth > 0) {
        xOffset = TruncateToInt32(pOwnerBox->GetScrollPos().cx % itemWidth);
    }

    // Left start position of child items
    int32_t iPosLeft = rc.left - xOffset;

    // Set the virtual offset; otherwise, when the data volume is large, the height of the 32-bit rect rc will overflow, and a 64-bit integer is required to hold it
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

        // Fill data
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
            // ElementIndex needs to be cleared
            IListBoxItem* pListBoxItem = dynamic_cast<IListBoxItem*>(pControl);
            if (pListBoxItem != nullptr) {
                pListBoxItem->SetElementIndex(Box::InvalidIndex);
            }
        }

        if ((++iCount % nRows) == 0) {
            ptTile.y = iPosTop;
            ptTile.x += szItem.cx + GetChildMarginX();
        }
        else {
            ptTile.y += rcTile.Height() + GetChildMarginY();
        }
    }
    if (!refreshDataList.empty()) {
        pOwnerBox->OnRefreshElements(refreshDataList);
        pOwnerBox->OnFilledElements(refreshDataList);
    }
}

size_t VirtualHTileLayout::AjustMaxItem(UiRect rc) const
{
    UiSize szItem = GetItemSize();
    ASSERT((szItem.cx > 0) && (szItem.cy > 0));
    if ((szItem.cx <= 0) || (szItem.cy <= 0)) {
        return 0;
    }
    if (rc.IsEmpty()) {
        return 0;
    }
    int32_t nRows = CalcTileRows(rc.Height());
    int32_t nColumns = rc.Width() / (szItem.cx + GetChildMarginX() / 2);
    // Validate and correct
    if (nColumns > 1) {
        int32_t calcWidth = nColumns * szItem.cx + (nColumns - 1) * GetChildMarginX();
        if (calcWidth < rc.Width()) {
            nColumns += 1;
        }
    }
    // Add one extra column to ensure that the real controls fill the entire display area
    nColumns += 1;
    return (size_t)nRows * nColumns;
}

size_t VirtualHTileLayout::GetTopElementIndex(UiRect rc) const
{
    VirtualListBox* pOwnerBox = GetOwnerBox();
    if (pOwnerBox == nullptr) {
        return 0;
    }
    int64_t nPos = pOwnerBox->GetScrollPos().cx;
    if (nPos < 0) {
        nPos = 0;
    }

    int64_t nRows = (int64_t)CalcTileRows(rc.Height());
    if (nRows < 0) {
        nRows = 0;
    }
    int64_t nWidth = GetElementsWidth(rc, 1);
    ASSERT(nWidth >= 0);
    if (nWidth <= 0) {
        return 0;
    }
    int64_t iIndex = (nPos / nWidth) * nRows;
    return static_cast<size_t>(iIndex);
}

bool VirtualHTileLayout::IsElementDisplay(UiRect rc, size_t iIndex) const
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
    UiSize szItem = GetItemSize();
    if ((nElementPos - szItem.cx) > nScrollPos) { // Left position of the rect
        int64_t nBoxWidth = pOwnerBox->GetWidth();
        if (nElementPos <= (nScrollPos + nBoxWidth)) {// Right position of the rect
            // Fully displayed
            return true;
        }
    }
    return false;
}

bool VirtualHTileLayout::NeedReArrange() const
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

void VirtualHTileLayout::GetDisplayElements(UiRect rc, std::vector<size_t>& collection) const
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
    if (nEleWidth == 0) {
        return;
    }

    int32_t nRows = CalcTileRows(rc.Height());
    size_t min = (size_t)(pOwnerBox->GetScrollPos().cx / nEleWidth) * nRows;
    size_t max = min + (size_t)(rc.Width() / nEleWidth) * nRows;

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

void VirtualHTileLayout::EnsureVisible(UiRect rc, size_t iIndex, bool bToTop) const
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
    const int32_t nRows = CalcTileRows(rc.Height());
    int64_t nTopIndex = 0;
    if (elementWidth > 0) {
        nTopIndex = (nPos / elementWidth) * nRows;
    }
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

        int64_t nTopColumns = nTopIndex / nRows;
        int64_t nDestColumns = iIndex / nRows;
        if ((int64_t)nDestColumns > nTopColumns) {
            // Scroll right: ensure it is at the far right
            int64_t width = GetElementsWidth(rc, iIndex + 1);
            nNewPos = width - pOwnerBox->GetRect().Width();
        }
        else {
            // Scroll left: ensure it is at the far left
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
