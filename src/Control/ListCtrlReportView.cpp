#include "dui/Control/ListCtrlReportView.h" 
#include "dui/Control/ListCtrl.h"
#include "dui/Render/AutoClip.h"

//Contains classes: ListCtrlReportView / ListCtrlReportLayout

namespace ui
{
ListCtrlReportView::ListCtrlReportView(Window* pWindow) :
    ListCtrlView(pWindow, new ListCtrlReportLayout),
    m_pListCtrl(nullptr),
    m_pData(nullptr),
    m_nTopElementIndex(0),
    m_nRowGridLineWidth(0),
    m_nColumnGridLineWidth(0),
    m_bFrozenHitTestPass(false)
{
    ListCtrlReportLayout* pDataLayout = dynamic_cast<ListCtrlReportLayout*>(GetLayout());
    ASSERT(pDataLayout != nullptr);

    VirtualLayout* pVirtualLayout = pDataLayout;
    ASSERT(pVirtualLayout != nullptr);
    SetVirtualLayout(pVirtualLayout);
    
    if (pDataLayout != nullptr) {
        pDataLayout->SetDataView(this);
    }
    SetRowGridLineWidth(1, true);
    SetColumnGridLineWidth(1, true);
}

ListCtrlReportView::~ListCtrlReportView() 
{
}

void ListCtrlReportView::HandleEvent(const EventArgs& msg)
{
    if ((msg.eventType == kEventKeyDown) && (m_pListCtrl != nullptr)) {
        //While the drop-down candidate flyout is open (common to row-selection mode and cell-selection mode): keyboard events are consumed by the flyout first
        if (m_pListCtrl->OnComboKeyDown(msg)) {
            return;
        }
        //In cell-selection mode, keyboard events are used first to move the current cell / extend the selection
        if ((m_pListCtrl->GetSelectionMode() == ListCtrlSelectionMode::kCell) &&
            m_pListCtrl->OnCellKeyDown(msg)) {
            return;
        }
    }
    //Clear the cell hover highlight when the mouse leaves the control, so no stale highlight remains
    if ((msg.eventType == kEventMouseLeave) && (m_pListCtrl != nullptr) &&
        (m_pListCtrl->GetSelectionMode() == ListCtrlSelectionMode::kCell)) {
        m_pListCtrl->ClearHoverCell();
    }
    BaseClass::HandleEvent(msg);
    if (m_pListCtrl != nullptr) {
        if ((msg.eventType > kEventKeyBegin) && (msg.eventType < kEventKeyEnd)) {
            m_pListCtrl->OnViewKeyboardEvents(msg);
        }
        else if ((msg.eventType > kEventMouseBegin) && (msg.eventType < kEventMouseEnd)) {
            m_pListCtrl->OnViewMouseEvents(msg);
        }
    }
}

bool ListCtrlReportView::OnListBoxItemMouseEvent(const EventArgs& msg)
{
    //In cell-selection mode, mouse events are intercepted for cell selection and range extension
    if ((m_pListCtrl != nullptr) && (m_pListCtrl->GetSelectionMode() == ListCtrlSelectionMode::kCell)) {
        //Row enter/leave events are not consumed and must keep bubbling to the parent control (the view ScrollBox):
        //only when the view receives MouseEnter/MouseLeave does it enter/leave the Hovered state and then trigger the fade-in/fade-out of the auto-hidden scroll bars;
        //row-level Hover/Pressed highlights are a visual exclusive to row-selection mode and are suppressed by ListCtrlItem's overridden MouseEnter/MouseLeave
        //(it does not enter the Hovered state or start the hover fade animation under kCell), with PaintStateColors
        if (msg.eventType == kEventMouseLeave) {
            //The mouse left this row (for example it moved to a blank area of the view): clear the cell hover state, so the hover highlight does not remain on the last cell the mouse passed over
            m_pListCtrl->ClearHoverCell();
        }
        if ((msg.eventType == kEventMouseEnter) || (msg.eventType == kEventMouseLeave)) {
            return BaseClass::OnListBoxItemMouseEvent(msg);
        }
        const bool bLButton = (msg.eventType == kEventMouseButtonDown) || (msg.eventType == kEventMouseButtonUp);
        const bool bRButton = (msg.eventType == kEventMouseRButtonDown) || (msg.eventType == kEventMouseRButtonUp);
        if (bLButton || bRButton || (msg.eventType == kEventMouseMove) || (msg.eventType == kEventMouseHover)) {
            //Forward the mouse event to the base class first: ListBoxHelper uses it to track the drag-selection state and paint the drag-selection overlay (the same implementation as row-selection mode)
            BaseClass::OnListBoxItemMouseEvent(msg);
            //When the mouse is on the CheckBox icon of a "data row/header" itself, do not consume this event (return false and let the item handle it):
            //if the event were consumed in cell-selection mode, ListBoxItemTemplate::HandleEvent would return early,
            //the item would not run its own ButtonDown/ButtonUp, the CheckBox check logic inside would not take effect,
            //and the row-level and header CheckBox would not respond to clicks; when let through, the item completes the check/uncheck itself,
            //and the whole-row selection logic is already skipped by ListCtrlItem::ButtonDown in cell-selection mode, so a whole-row selection is not triggered by mistake
            if (bLButton && IsMouseOnItemCheckBox(msg.GetSender(), msg.ptMouse)) {
                return false;
            }
            //Because the event is consumed below (preventing the item from triggering row selection and the row-level Hover/Pressed visuals), the item does not run Control::ButtonDown
            //and therefore does not set the "mouse focused" state automatically, so maintain it explicitly here to satisfy ListBoxHelper::OnMouseMove's requirement on pSender->IsMouseFocused()
            Control* pSender = msg.GetSender();
            if (pSender != nullptr) {
                if ((msg.eventType == kEventMouseButtonDown) || (msg.eventType == kEventMouseRButtonDown)) {
                    pSender->SetMouseFocused(true);
                }
                else if ((msg.eventType == kEventMouseButtonUp) || (msg.eventType == kEventMouseRButtonUp)) {
                    pSender->SetMouseFocused(false);
                }
            }
            //When the mouse is inside the "header/pinned-to-top row" area, do not perform cell selection or hover handling (button-up is excluded, since it is only used to end the dragging state):
            //this avoids "passing through" and selecting the cells covered below when a pinned-to-top row is clicked, and also avoids clearing the existing cell selection by mistake
            const bool bButtonUp = (msg.eventType == kEventMouseButtonUp) || (msg.eventType == kEventMouseRButtonUp);
            if (!bButtonUp && IsPointOnAtTopItem(msg.ptMouse)) {
                if ((msg.eventType == kEventMouseMove) || (msg.eventType == kEventMouseHover)) {
                    //Clear the cell hover state, so the hover highlight does not remain below the pinned-to-top row
                    m_pListCtrl->UpdateHoverCell(Box::InvalidIndex, Box::InvalidIndex);
                }
                else if ((msg.eventType == kEventMouseButtonDown) || (msg.eventType == kEventMouseRButtonDown)) {
                    //A cell CheckBox inside a pinned-to-top row (a non-selectable row) likewise requires "the current list item is the focused control" before it can be checked
                    //(see the focus precondition of ListCtrlLabel::OnMouseEvent), so, as with normal rows, set the hit pinned-to-top row as the focused control on button-down
                    ListCtrlItem* pTopItem = HitTestTopItem(msg.ptMouse);
                    if (pTopItem != nullptr) {
                        pTopItem->SetFocus();
                    }
                }
                return true;
            }
            //Then handle the hit and the selection with cell semantics
            size_t nRowIndex = Box::InvalidIndex;
            size_t nColumnId = Box::InvalidIndex;
            ListCtrlItem* pHitItem = nullptr;
            HitTestCell(msg.ptMouse, nRowIndex, nColumnId, &pHitItem);
            if ((pHitItem != nullptr) &&
                ((msg.eventType == kEventMouseButtonDown) || (msg.eventType == kEventMouseRButtonDown))) {
                //Explicitly set the hit list item as the keyboard focus control:
                //1) before checking/unchecking, a CheckBox inside a cell (ListCtrlLabel) requires "the current list item is the focused control";
                //2) the focused control is the entry point for keyboard events; the arrow keys/Ctrl+A can only bubble to the view and be handled by OnCellKeyDown after reaching the focused control first.
                //(in row-selection mode ListBox::SelectItemSingle performs the same focus setup)
                //Note: SetFocus() sets the row control state to Hovered through Control::OnSetFocus;
                //because the row-level Hovered visual is already suppressed by ListCtrlItem::PaintStateColors in cell-selection mode, no state restoration is needed here.
                pHitItem->SetFocus();
            }
            m_pListCtrl->OnCellMouseEvent(msg, nRowIndex, nColumnId);
            //Consume this event: do not trigger the row selection logic
            return true;
        }
        if (msg.eventType == kEventMouseRDoubleClick) {
            return true;
        }
    }
    return BaseClass::OnListBoxItemMouseEvent(msg);
}

bool ListCtrlReportView::OnListBoxItemKeyEvent(const EventArgs& msg)
{
    if (m_pListCtrl == nullptr) {
        return false;
    }
    //While the drop-down candidate flyout is open, keyboard events are consumed by the flyout first (Up/Down to move the candidate, Enter to submit, ESC to cancel):
    //if they are not intercepted at the sub-item level, Enter is consumed by ListBoxItem and converted into kEventReturn, so the flyout never receives Enter and cannot submit
    return m_pListCtrl->OnComboKeyDown(msg);
}

bool ListCtrlReportView::HitTestCell(const UiPoint& ptMouse, size_t& nRowIndex, size_t& nColumnId, ListCtrlItem** ppItem) const
{
    nRowIndex = Box::InvalidIndex;
    nColumnId = Box::InvalidIndex;
    if (ppItem != nullptr) {
        *ppItem = nullptr;
    }
    const size_t nItemCount = GetItemCount();
    //The first row is the header, so iterate the data rows starting from index 1
    for (size_t index = 1; index < nItemCount; ++index) {
        ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(GetItemAt(index));
        if ((pItem == nullptr) || !pItem->IsVisible()) {
            continue;
        }
        if (!pItem->IsSelectableType()) {
            //Pinned-to-top rows and other non-selectable rows do not participate in cell hit testing
            continue;
        }
        //Convert the mouse point to sub-item control coordinates (with frozen columns, the horizontal part follows the frozen-area convention)
        const UiPoint pt = MouseToCellSpace(ptMouse, pItem->GetScrollOffsetInScrollBox());
        //Find the cell containing this point among the sub-items: ListCtrlItem::GetSubItem cannot be reused here,
        //because it converts with a single uniform scroll amount, and when the mouse is inside the frozen area the hit column would be misaligned with the painting
        ListCtrlSubItem* pSubItem = nullptr;
        const size_t nSubItemCount = pItem->GetItemCount();
        for (size_t nSubItemIndex = 0; nSubItemIndex < nSubItemCount; ++nSubItemIndex) {
            ListCtrlSubItem* pSubItemTemp = dynamic_cast<ListCtrlSubItem*>(pItem->GetItemAt(nSubItemIndex));
            if ((pSubItemTemp != nullptr) && pSubItemTemp->IsVisible() && pSubItemTemp->GetRect().ContainsPt(pt)) {
                pSubItem = pSubItemTemp;
                break;
            }
        }
        if (pSubItem != nullptr) {
            nRowIndex = pItem->GetDataItemIndex();
            nColumnId = pSubItem->GetDataColumnId();
            if (ppItem != nullptr) {
                *ppItem = pItem;
            }
            return true;
        }
    }
    return false;
}

UiPoint ListCtrlReportView::MouseToCellSpace(const UiPoint& ptMouse, const UiPoint& scrollOffsetInBox) const
{
    int32_t nScrollX = scrollOffsetInBox.x;
    if ((nScrollX != 0) && (m_pListCtrl != nullptr)) {
        const ListCtrlCellGeometry cellGeometry = BuildCellGeometry();
        if (cellGeometry.bHasFrozen) {
            //When the mouse is inside the frozen area, the horizontal scroll amount counts as 0: frozen columns do not move with horizontal scrolling
            //(the horizontal window-origin offset is 0 while painting, so hit testing must use the same convention as painting)
            if (cellGeometry.IsFrozenX(ptMouse.x)) {
                nScrollX = 0;
            }
        }
    }
    UiPoint pt(ptMouse);
    pt.Offset(nScrollX, scrollOffsetInBox.y);
    return pt;
}

bool ListCtrlReportView::IsPointOnAtTopItem(const UiPoint& ptMouse) const
{
    const size_t nItemCount = GetItemCount();
    //The first row is the header, so iterate the data rows starting from index 1
    for (size_t index = 1; index < nItemCount; ++index) {
        ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(GetItemAt(index));
        if ((pItem == nullptr) || !pItem->IsVisible() || pItem->IsSelectableType()) {
            continue;
        }
        //Pinned-to-top row (a non-selectable row): test whether the mouse point is inside the row
        //(uses the same coordinate convention as GetSubItem, so scrolling cannot cause a wrong result; with frozen columns the horizontal part follows the frozen-area convention)
        const UiPoint pt = MouseToCellSpace(ptMouse, pItem->GetScrollOffsetInScrollBox());
        if (pItem->GetRect().ContainsPt(pt)) {
            return true;
        }
    }
    return false;
}

ListCtrlItem* ListCtrlReportView::HitTestTopItem(const UiPoint& ptMouse) const
{
    const size_t nItemCount = GetItemCount();
    //The first row is the header, so iterate the data rows starting from index 1
    for (size_t index = 1; index < nItemCount; ++index) {
        ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(GetItemAt(index));
        if ((pItem == nullptr) || !pItem->IsVisible() || pItem->IsSelectableType()) {
            continue;
        }
        //Pinned-to-top row (a non-selectable row): test whether the mouse point is inside the row (same coordinate convention as GetSubItem)
        const UiPoint pt = MouseToCellSpace(ptMouse, pItem->GetScrollOffsetInScrollBox());
        if (pItem->GetRect().ContainsPt(pt)) {
            return pItem;
        }
    }
    return nullptr;
}

bool ListCtrlReportView::IsMouseOnItemCheckBox(const Control* pSender, const UiPoint& ptMouse) const
{
    if (pSender == nullptr) {
        return false;
    }
    //The data row's (ListCtrlItem's) own CheckBox
    const ListCtrlItem* pListItem = dynamic_cast<const ListCtrlItem*>(pSender);
    if (pListItem != nullptr) {
        return pListItem->IsShowCheckBox() && pListItem->IsMouseOnCheckBoxIcon(ptMouse);
    }
    //The header's (ListCtrlHeader's) own CheckBox
    const ListCtrlHeader* pListHeader = dynamic_cast<const ListCtrlHeader*>(pSender);
    if (pListHeader != nullptr) {
        return pListHeader->IsShowCheckBox() && pListHeader->IsMouseOnCheckBoxIcon(ptMouse);
    }
    return false;
}

bool ListCtrlReportView::GetTopFixedItemsRect(UiRect& rcTopFixed) const
{
    bool bHasTopFixed = false;
    const size_t nItemCount = GetItemCount();
    for (size_t index = 0; index < nItemCount; ++index) {
        Control* pControl = GetItemAt(index);
        if ((pControl == nullptr) || !pControl->IsVisible()) {
            continue;
        }
        if (index > 0) {
            //Index 0 is the header; among the remaining elements, only pinned-to-top (non-selectable) rows are fixed at the top
            ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(pControl);
            if ((pItem == nullptr) || pItem->IsSelectableType()) {
                continue;
            }
        }
        if (!bHasTopFixed) {
            rcTopFixed = pControl->GetRect();
            bHasTopFixed = true;
        }
        else {
            rcTopFixed.Union(pControl->GetRect());
        }
    }
    return bHasTopFixed;
}

void ListCtrlReportView::SetListCtrl(ListCtrl* pListCtrl)
{
    m_pListCtrl = pListCtrl;
}

void ListCtrlReportView::SetDataProvider(VirtualListBoxElement* pProvider)
{
    BaseClass::SetDataProvider(pProvider);
    if (GetDataProvider() != nullptr) {
        m_pData = dynamic_cast<ListCtrlData*>(GetDataProvider());
        ASSERT(m_pData != nullptr);
    }
    else {
        m_pData = nullptr;
    }
}

void ListCtrlReportView::InvalidateRowElementIndexes()
{
    //Sub-item 0 is the header control; start from sub-item 1 (the data row controls)
    const size_t nItemCount = GetItemCount();
    for (size_t nItemIndex = 1; nItemIndex < nItemCount; ++nItemIndex) {
        Control* pControl = GetItemAt(nItemIndex);
        if (pControl == nullptr) {
            continue;
        }
        IListBoxItem* pListBoxItem = dynamic_cast<IListBoxItem*>(pControl);
        if (pListBoxItem != nullptr) {
            pListBoxItem->SetElementIndex(Box::InvalidIndex);
        }
    }
}

void ListCtrlReportView::Refresh(bool bSync)
{
    if ((m_pListCtrl != nullptr) && !m_pListCtrl->IsEnableRefresh()) {
        //The refresh feature has been disabled
        return;
    }
    if ((m_pListCtrl != nullptr) && !m_pListCtrl->IsInited()) {
        return;
    }
    if ((GetWindow() == nullptr) || !HasDataProvider()) {
        return;
    }
    //Adjust the number of UI controls to meet the display requirements
    AjustItemCount();

    //After a structural refresh (sorting, row insertion/removal, row height, pinned rows, hidden rows, ...) the binding between data elements and UI controls may be stale,
    //so clear the element-index binding of every row: the following relayout then refills data on demand (rows whose binding is still valid are not filled twice)
    InvalidateRowElementIndexes();

    //Refresh the display content of the UI controls
    if (GetElementCount() > 0) {
        ReArrangeChild(true);
        Arrange();
    }
    if (bSync) {
        //Re-layout immediately
        SetPos(GetPos());
    }
}

void ListCtrlReportView::OnModelDataChanged(size_t nStartElementIndex, size_t nEndElementIndex)
{
    if ((m_pListCtrl != nullptr) && !m_pListCtrl->IsEnableRefresh()) {
        //Refreshing is currently disabled
        return;
    }
    BaseClass::OnModelDataChanged(nStartElementIndex, nEndElementIndex);
}

void ListCtrlReportView::OnModelCountChanged()
{
    if ((m_pListCtrl != nullptr) && !m_pListCtrl->IsEnableRefresh()) {
        //Refreshing is currently disabled
        return;
    }
    BaseClass::OnModelCountChanged();
}

void ListCtrlReportView::AjustItemCount()
{
    VirtualLayout* pVirtualLayout = GetVirtualLayout();
    if (pVirtualLayout == nullptr) {
        return;
    }

    //Maximum number of sub-items
    size_t nMaxItemCount = pVirtualLayout->AjustMaxItem(GetPosWithoutPadding());
    if (nMaxItemCount == 0) {
        return;
    }

    //Current total data count: data count + Header control
    size_t nElementCount = GetElementCount() + 1;

    //Current number of sub-items
    size_t nItemCount = GetItemCount();

    //Number of sub-items after refresh
    size_t nNewItemCount = nElementCount;
    if (nNewItemCount > nMaxItemCount) {
        nNewItemCount = nMaxItemCount;
    }

    if (nItemCount > nNewItemCount) {
        //If the current total sub-item count is greater than the newly calculated one, remove the sub-items exceeding the data count (delete from the back)
        size_t n = nItemCount - nNewItemCount;
        for (size_t i = 0; i < n; ++i) {
            size_t itemCount = GetItemCount();
            if (itemCount > 1) {
                //Keep at least one Header control
                RemoveItemAt(itemCount - 1);
            }
        }
    }
    else if (nItemCount < nNewItemCount) {
        //If the current total sub-item count is less than the newly calculated one, add the sub-items missing from the data count
        size_t n = nNewItemCount - nItemCount;
        for (size_t i = 0; i < n; ++i) {
            Control* pControl = CreateElement();
            ASSERT(pControl != nullptr);
            if (pControl != nullptr) {
                AddItem(pControl);
            }
        }
    }
}

int32_t ListCtrlReportView::GetListCtrlWidth() const
{
    int32_t nToltalWidth = 0;
    if (m_pListCtrl == nullptr) {
        return nToltalWidth;
    }
    ListCtrlHeader* pHeaderCtrl = m_pListCtrl->GetHeaderCtrl();
    if (pHeaderCtrl == nullptr) {
        return nToltalWidth;
    }
    size_t nColumnCount = pHeaderCtrl->GetColumnCount();
    for (size_t index = 0; index < nColumnCount; ++index) {
        ListCtrlHeaderItem* pHeaderItem = pHeaderCtrl->GetColumn(index);
        if ((pHeaderItem != nullptr) && pHeaderItem->IsColumnVisible()) {
            nToltalWidth += pHeaderItem->GetColumnWidth();
        }
    }
    UiPadding rcReportViewPadding = GetPadding();
    nToltalWidth += rcReportViewPadding.left + rcReportViewPadding.right;
    nToltalWidth += pHeaderCtrl->GetPadding().left;
    return nToltalWidth;
}

void ListCtrlReportView::SetTopElementIndex(size_t nTopElementIndex)
{
    m_nTopElementIndex = nTopElementIndex;
}

size_t ListCtrlReportView::GetTopElementIndex() const
{
    return m_nTopElementIndex;
}

void ListCtrlReportView::SetDisplayDataItems(const std::vector<size_t>& itemIndexList)
{
    m_diplayItemIndexList = itemIndexList;
}

void ListCtrlReportView::GetDisplayDataItems(std::vector<size_t>& itemIndexList) const
{
    itemIndexList = m_diplayItemIndexList;
}

bool ListCtrlReportView::IsDataItemDisplay(size_t itemIndex) const
{
    auto iter = std::find(m_diplayItemIndexList.begin(), m_diplayItemIndexList.end(), itemIndex);
    return iter != m_diplayItemIndexList.end();
}

bool ListCtrlReportView::EnsureDataItemVisible(size_t itemIndex, bool bToTop)
{
    if (!Box::IsValidItemIndex(itemIndex) || (itemIndex >= GetElementCount())) {
        return false;
    }
    VirtualLayout* pVirtualLayout = GetVirtualLayout();
    if (pVirtualLayout != nullptr) {
        pVirtualLayout->EnsureVisible(GetRect(), itemIndex, bToTop);
        return true;
    }
    return false;
}

size_t ListCtrlReportView::GetTopDataItemIndex(int64_t nScrollPosY) const
{
    if (m_pListCtrl == nullptr) {
        return 0;
    }
    size_t itemIndex = 0;
    if (nScrollPosY <= 0) {
        return itemIndex;
    }
    ListCtrlData* pDataProvider = m_pData;
    if (pDataProvider == nullptr) {
        return itemIndex;
    }
    int64_t totalItemHeight = 0;
    int32_t nItemHeight = 0;
    const size_t dataItemCount = pDataProvider->GetDataItemCount();
    for (size_t index = 0; index < dataItemCount; ++index) {
        const ListCtrlItemData* pRowData = pDataProvider->GetDisplayItemData(index);
        if (pRowData == nullptr) {
            continue;
        }
        const ListCtrlItemData& rowData = *pRowData;
        nItemHeight = rowData.nItemHeight;
        if (nItemHeight < 0) {
            //Use the default row height
            nItemHeight = m_pListCtrl->GetDataItemHeight();
        }
        if (!rowData.bVisible || (nItemHeight  == 0)){
            continue;
        }
        totalItemHeight += nItemHeight;
        //If every row has the same height, this is equivalent to nScrollPosY / ItemHeight
        if (totalItemHeight > nScrollPosY) {
            itemIndex = index;
            break;
        }
    }
    return itemIndex;
}

int32_t ListCtrlReportView::GetDataItemHeight(size_t itemIndex) const
{
    int32_t nItemHeight = 0;
    if (m_pListCtrl == nullptr) {
        return 0;
    }
    nItemHeight = m_pListCtrl->GetDataItemHeight();
    ListCtrlData* pDataProvider = m_pData;
    if (pDataProvider == nullptr) {
        return 0;
    }
    const ListCtrlItemData* pRowData = pDataProvider->GetDisplayItemData(itemIndex);
    ASSERT(pRowData != nullptr);
    if ((pRowData != nullptr) && (pRowData->nItemHeight >= 0)) {
        nItemHeight = pRowData->nItemHeight;
    }
    return nItemHeight;
}

void ListCtrlReportView::GetDataItemsToShow(int64_t nScrollPosY, size_t maxCount, 
                                            std::vector<ShowItemInfo>& itemIndexList,
                                            std::vector<ShowItemInfo>& atTopItemIndexList,
                                            int64_t& nPrevItemHeights) const
{
    nPrevItemHeights = 0;
    itemIndexList.clear();
    atTopItemIndexList.clear();
    if (m_pListCtrl == nullptr) {
        return;
    }
    ASSERT(nScrollPosY >= 0);
    if (nScrollPosY < 0) {
        return;
    }
    //ASSERT(maxCount != 0);
    if (maxCount == 0) {
        return;
    }
    ListCtrlData* pDataProvider = m_pData;
    if (pDataProvider == nullptr) {
        return;
    }
    size_t nTopDataItemIndex = Box::InvalidIndex; //Index of the first element visible at the top
    const int32_t nDefaultItemHeight = m_pListCtrl->GetDataItemHeight(); //Default row height
    //Indices of the pinned-to-top elements
    struct AlwaysAtTopData
    {
        int8_t nAlwaysAtTop;    //Pinning priority
        size_t nItemIndex;      //Element index
        int32_t nItemHeight;    //Height of the element
    };
    std::vector<AlwaysAtTopData> alwaysAtTopItemList;
    
    int64_t totalItemHeight = 0;
    int32_t nItemHeight = 0;
    const size_t dataItemCount = pDataProvider->GetDataItemCount();
    for (size_t index = 0; index < dataItemCount; ++index) {
        const ListCtrlItemData* pRowData = pDataProvider->GetDisplayItemData(index);
        if (pRowData == nullptr) {
            continue;
        }
        const ListCtrlItemData& rowData = *pRowData;
        nItemHeight = (rowData.nItemHeight < 0) ? nDefaultItemHeight : rowData.nItemHeight;
        if (!rowData.bVisible || (nItemHeight == 0)) {
            //Not visible, skip
            continue;
        }

        if (rowData.nAlwaysAtTop >= 0) {
            //Pinned-to-top element
            if (alwaysAtTopItemList.size() < maxCount) {
                alwaysAtTopItemList.push_back({ rowData.nAlwaysAtTop, index, nItemHeight });
            }
            continue;
        }

        if (nTopDataItemIndex == Box::InvalidIndex) {
            totalItemHeight += nItemHeight;
            //If every row has the same height, this is equivalent to nScrollPosY / ItemHeight
            if (totalItemHeight > nScrollPosY) {
                nTopDataItemIndex = index;
                nPrevItemHeights = totalItemHeight - nItemHeight;
            }
        }

        if ((nTopDataItemIndex != Box::InvalidIndex) && (itemIndexList.size() < maxCount)){
            itemIndexList.push_back({ index, nItemHeight });
        }
    }

    //Sort the pinned-to-top items
    if (!alwaysAtTopItemList.empty()) {
        std::stable_sort(alwaysAtTopItemList.begin(), alwaysAtTopItemList.end(),
            [](const AlwaysAtTopData& a, const AlwaysAtTopData& b) {
                //The one with the larger nAlwaysAtTop value comes first
                return a.nAlwaysAtTop > b.nAlwaysAtTop;
            });
    }
    for (const AlwaysAtTopData& item : alwaysAtTopItemList) {
        atTopItemIndexList.push_back({ item.nItemIndex, item.nItemHeight });
    }
    if (atTopItemIndexList.size() >= maxCount) {
        atTopItemIndexList.resize(maxCount);
        itemIndexList.clear();
    }
    else {
        size_t nLeftCount = maxCount - atTopItemIndexList.size();
        if (itemIndexList.size() > nLeftCount) {
            itemIndexList.resize(nLeftCount);
        }
    }
    ASSERT((itemIndexList.size() + atTopItemIndexList.size()) <= maxCount);
}

int32_t ListCtrlReportView::GetMaxDataItemsToShow(int64_t nScrollPosY, int32_t nRectHeight, 
                                                std::vector<size_t>* pItemIndexList,
                                                std::vector<size_t>* pAtTopItemIndexList) const
{
    if (pItemIndexList) {
        pItemIndexList->clear();
    }
    if (m_pListCtrl == nullptr) {
        return 0;
    }
    ASSERT(nScrollPosY >= 0);
    if (nScrollPosY < 0) {
        return 0;
    }
    //ASSERT(nRectHeight > 0);
    if (nRectHeight <= 0) {
        return 0;
    }
    ListCtrlData* pDataProvider = m_pData;
    if (pDataProvider == nullptr) {
        return 0;
    }
    size_t nTopDataItemIndex = Box::InvalidIndex; //Index of the first element visible at the top
    const int32_t nDefaultItemHeight = m_pListCtrl->GetDataItemHeight(); //Default row height
    //Indices of the pinned-to-top elements
    struct AlwaysAtTopData
    {
        int8_t nAlwaysAtTop;
        size_t index;
    };
    std::vector<AlwaysAtTopData> alwaysAtTopItemList;
    std::vector<size_t> itemIndexList;

    int64_t totalItemHeight = 0;
    int32_t nItemHeight = 0;
    const size_t dataItemCount = pDataProvider->GetDataItemCount();
    for (size_t index = 0; index < dataItemCount; ++index) {
        const ListCtrlItemData* pRowData = pDataProvider->GetDisplayItemData(index);
        if (pRowData == nullptr) {
            continue;
        }
        const ListCtrlItemData& rowData = *pRowData;
        nItemHeight = (rowData.nItemHeight < 0) ? nDefaultItemHeight : rowData.nItemHeight;
        if (!rowData.bVisible || (nItemHeight == 0)) {
            //Not visible, skip
            continue;
        }

        if (rowData.nAlwaysAtTop >= 0) {
            //Pinned-to-top element
            alwaysAtTopItemList.push_back({ rowData.nAlwaysAtTop, index });
            continue;
        }

        if (nTopDataItemIndex == Box::InvalidIndex) {
            totalItemHeight += nItemHeight;
            //If every row has the same height, this is equivalent to nScrollPosY / ItemHeight
            if (totalItemHeight > nScrollPosY) {
                nTopDataItemIndex = index;
            }
        }

        if (nTopDataItemIndex != Box::InvalidIndex) {
            itemIndexList.push_back(index);
        }
    }

    //Sort the pinned-to-top items
    if (!alwaysAtTopItemList.empty()) {
        std::stable_sort(alwaysAtTopItemList.begin(), alwaysAtTopItemList.end(),
            [](const AlwaysAtTopData& a, const AlwaysAtTopData& b) {
                //The one with the larger nAlwaysAtTop value comes first
                return a.nAlwaysAtTop > b.nAlwaysAtTop;
            });
    }
    std::vector<size_t> atTopIndexList;
    std::vector<size_t> tempItemIndexList;
    tempItemIndexList.swap(itemIndexList);
    for (const AlwaysAtTopData& item : alwaysAtTopItemList) {
        itemIndexList.push_back(item.index);
        atTopIndexList.push_back(item.index);
    }
    for (size_t index : tempItemIndexList) {
        itemIndexList.push_back(index);
    }
    int32_t nShowItemCount = 0;
    int64_t nTotalHeight = 0;
    for (size_t index : itemIndexList) {
        if (index >= dataItemCount) {
            continue;
        }
        const ListCtrlItemData* pRowData = pDataProvider->GetDisplayItemData(index);
        if (pRowData == nullptr) {
            continue;
        }
        nItemHeight = pRowData->nItemHeight;
        if (nItemHeight < 0) {
            //Use the default row height
            nItemHeight = nDefaultItemHeight;
        }
        nTotalHeight += nItemHeight;
        if (nTotalHeight < nRectHeight) {
            if (pItemIndexList) {
                pItemIndexList->push_back(index);
            }
            if (pAtTopItemIndexList != nullptr) {
                if (std::find(atTopIndexList.begin(),
                              atTopIndexList.end(), index) != atTopIndexList.end()) {
                    pAtTopItemIndexList->push_back(index);
                }
            }
            ++nShowItemCount;
        }
        else {
            nShowItemCount += 2;
            break;
        }
    }
    return nShowItemCount;
}

int64_t ListCtrlReportView::GetDataItemTotalHeights(size_t itemIndex, bool bIncludeAtTops) const
{
    if (m_pListCtrl == nullptr) {
        return 0;
    }
    ListCtrlData* pDataProvider = m_pData;
    if (pDataProvider == nullptr) {
        return 0;
    }
    const int32_t nDefaultItemHeight = m_pListCtrl->GetDataItemHeight(); //Default row height
    int64_t totalItemHeight = 0;
    int32_t nItemHeight = 0;
    const size_t dataItemCount = pDataProvider->GetDataItemCount();
    for (size_t index = 0; index < dataItemCount; ++index) {
        const ListCtrlItemData* pRowData = pDataProvider->GetDisplayItemData(index);
        if (pRowData == nullptr) {
            continue;
        }
        const ListCtrlItemData& rowData = *pRowData;
        nItemHeight = (rowData.nItemHeight < 0) ? nDefaultItemHeight : rowData.nItemHeight;
        if (!rowData.bVisible || (nItemHeight == 0)) {
            //Not visible, skip
            continue;
        }

        if (rowData.nAlwaysAtTop >= 0) {
            //Pinned-to-top element, needs to be counted
            if (bIncludeAtTops) {
                totalItemHeight += nItemHeight;
            }            
        }
        else if (index < itemIndex) {
            //Element that meets the requirements
            totalItemHeight += nItemHeight;
        }
        else if (!bIncludeAtTops) {
            //Already done
            break;
        }
    }
    return totalItemHeight;
}

bool ListCtrlReportView::IsNormalMode() const
{
    bool bNormalMode = true;
    ListCtrlData* pDataProvider = m_pData;
    if (pDataProvider != nullptr) {
        bNormalMode = pDataProvider->IsNormalMode();
    }
    return bNormalMode;
}

void ListCtrlReportView::SetAtTopControlIndex(const std::vector<size_t>& atTopControlList)
{
    m_atTopControlList = atTopControlList;
}

void ListCtrlReportView::MoveTopItemsToLast(std::vector<Control*>& items, std::vector<Control*>& atTopItems) const
{
    atTopItems.clear();
    if (items.empty()) {
        return;
    }
    for (size_t index : m_atTopControlList) {
        if (index < items.size()) {
            atTopItems.push_back(items[index]);
        }
    }
    for (int32_t i = (int32_t)m_atTopControlList.size() - 1; i >= 0; --i) {
        size_t index = m_atTopControlList[i];
        if (index < items.size()) {
            items.erase(items.begin() + index);
        }
    }
    for (Control* pControl : atTopItems) {
        items.push_back(pControl);
    }
    ListCtrlHeader* pHeaderCtrl = dynamic_cast<ListCtrlHeader*>(GetItemAt(0));
    if ((pHeaderCtrl != nullptr) && pHeaderCtrl->IsVisible()) {
        //Draw the Header control last to avoid being covered by others
        Control* pHeader = items.front();
        items.erase(items.begin());
        items.push_back(pHeader);

        //Add the Header control to the list
        atTopItems.push_back(pHeader);
    }    
    ASSERT(items.size() == m_items.size());
}


void ListCtrlReportView::PaintChild(IRender* pRender, const UiRect& rcPaint)
{
    //Override the VirtualListBox::PaintChild / ScrollBox::PaintChild functions to ensure the Header is drawn correctly
    if (pRender == nullptr) {
        return;
    }
    UiRect rcTemp;
    if (!UiRect::Intersect(rcTemp, rcPaint, GetRect())) {
        return;
    }

    const size_t nItemCount = GetItemCount();
    if (nItemCount <= 1) {
        //The first row is the header, draw it directly
        BaseClass::PaintChild(pRender, rcPaint);
        return;
    }
    ListCtrlHeader* pHeaderCtrl = dynamic_cast<ListCtrlHeader*>(GetItemAt(0));
    if (pHeaderCtrl == nullptr) {
        BaseClass::PaintChild(pRender, rcPaint);
        return;
    }

    //The drawing strategy needs to be adjusted
    ReArrangeChild(false);
    std::vector<Control*> items = m_items;
    //Move the Header and pinned-to-top elements to the end
    std::vector<Control*> atTopItems;
    MoveTopItemsToLast(items, atTopItems);

    UiRect rcTopControls; //Rect region of all pinned-to-top controls
    for (size_t i = 0; i < atTopItems.size(); ++i) {
        const Control* pTopControl = atTopItems[i];
        if (pTopControl == nullptr) {
            continue;
        }
        if (i == 0) {
            rcTopControls = pTopControl->GetRect();
        }
        else {
            rcTopControls.Union(pTopControl->GetRect());
        }
    }

    //Draw the list item child controls
    //Frozen columns: they are always displayed on the left side of the list and do not move with horizontal scrolling
    //Painting, hit testing and grid lines all reuse the same cell geometry layer (BuildCellGeometry)
    const ListCtrlCellGeometry cellGeometry = BuildCellGeometry();

    //Cell-selection mode: paint the selection highlight first (before the data-item controls, so the cell text is not covered)
    if ((m_pListCtrl != nullptr) && (m_pListCtrl->GetSelectionMode() == ListCtrlSelectionMode::kCell)) {
        if (cellGeometry.bHasFrozen) {
            PaintCellSelection(pRender, cellGeometry, false);
            PaintCellSelection(pRender, cellGeometry, true);
        }
        else {
            PaintCellSelection(pRender, cellGeometry, false);
        }
    }

    //Paint a single control (bFrozenPass: whether the frozen-column part is being painted)
    auto paintControl = [&](Control* pControl, bool bFrozenPass) {
        if ((pControl == nullptr) || !pControl->IsVisible()) {
            return;
        }
        //The clip rectangle (screen coordinates), the paint rectangle (screen coordinates) and the window-origin offset are all derived from the coordinate transform
        UiRect rcClip;
        UiRect rcNewPaint;
        UiPoint ptOffset;
        cellGeometry.GetPaintPass(bFrozenPass, rcClip, rcNewPaint, ptOffset);
        if (cellGeometry.bHasFrozen && (rcClip.left >= rcClip.right)) {
            return;
        }
        AutoClip alphaClip(pRender, rcClip, IsClip());

        rcNewPaint.Offset(GetRenderOffset().x, GetRenderOffset().y);

        bool bHasClip = false;
        if (!atTopItems.empty() &&
            (std::find(atTopItems.begin(), atTopItems.end(), pControl) == atTopItems.end())) {
            UiRect rcControlRect = pControl->GetRect();
            UiRect rUnion;
            if (UiRect::Intersect(rUnion, rcTopControls, rcControlRect)) {
                //There is an intersection, clipping needs to be set to avoid drawing the overlapping region of the pinned-to-top element and other elements
                pRender->SetClip(rUnion, false);
                bHasClip = true;
            }
        }

        UiPoint ptOldOrg = pRender->OffsetWindowOrg(ptOffset);
        pControl->AlphaPaint(pRender, rcNewPaint);
        pRender->SetWindowOrg(ptOldOrg);
        if (bHasClip) {
            pRender->ClearClip();
        }
    };

    for (Control* pControl : items) {
        if (pControl == nullptr) {
            continue;
        }
        if (cellGeometry.bHasFrozen) {
            //Paint the scrollable area (non-frozen columns) first
            paintControl(pControl, false);
            //Then paint the frozen columns (they cover the top layer)
            paintControl(pControl, true);
        }
        else {
            //No frozen columns: paint in a single pass as before
            paintControl(pControl, false);
        }
    }

    //Drawing of the grid lines
    PaintGridLines(pRender);

    //Draw the scroll bar
    ScrollBar* pVScrollBar = GetVScrollBar();
    ScrollBar* pHScrollBar = GetHScrollBar();
    if ((pHScrollBar != nullptr) && pHScrollBar->IsVisible()) {
        pHScrollBar->AlphaPaint(pRender, rcPaint);
    }

    if ((pVScrollBar != nullptr) && pVScrollBar->IsVisible()) {
        pVScrollBar->AlphaPaint(pRender, rcPaint);
    }

    //Drawing of the frame selection box for the mouse frame selection feature
    PaintFrameSelection(pRender);
}

ListCtrlReportView::ListCtrlColumnLayout ListCtrlReportView::BuildColumnLayout(int32_t nViewLeft) const
{
    ListCtrlColumnLayout columnLayout;
    const size_t nItemCount = GetItemCount();
    //Derive the X interval of each column from the first visible data row when possible (the geometry aligns with the column boundaries and includes the row-leading CheckBox/padding placeholder widths)
    for (size_t index = 1; index < nItemCount; ++index) {
        ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(GetItemAt(index));
        if ((pItem == nullptr) || !pItem->IsVisible() || (pItem->GetWidth() <= 0)) {
            continue;
        }
        const size_t nSubItemCount = pItem->GetItemCount();
        for (size_t nSubItem = 0; nSubItem < nSubItemCount; ++nSubItem) {
            ListCtrlSubItem* pSubItem = dynamic_cast<ListCtrlSubItem*>(pItem->GetItemAt(nSubItem));
            if ((pSubItem == nullptr) || !pSubItem->IsVisible()) {
                continue;
            }
            ListCtrlColumnLayout::Column column;
            column.nColumnId = pSubItem->GetDataColumnId();
            column.nColumnIndex = columnLayout.m_columns.size();
            column.nXBegin = pSubItem->GetRect().left - nViewLeft;
            column.nXEnd = pSubItem->GetRect().right - nViewLeft;
            columnLayout.m_columns.push_back(column);
        }
        //Only the first visible data row is used
        break;
    }
    if (!columnLayout.m_columns.empty()) {
        return columnLayout;
    }

    //When no data row is visible, derive the column X intervals from the header
    ListCtrlHeader* pHeaderCtrl = dynamic_cast<ListCtrlHeader*>(GetItemAt(0));
    if (pHeaderCtrl != nullptr) {
        const size_t nHeaderItemCount = pHeaderCtrl->GetItemCount();
        for (size_t i = 0; i < nHeaderItemCount; ++i) {
            ListCtrlHeaderItem* pHeaderItem = dynamic_cast<ListCtrlHeaderItem*>(pHeaderCtrl->GetItemAt(i));
            if (pHeaderItem == nullptr) {
                continue;
            }
            ListCtrlColumnLayout::Column column;
            column.nColumnId = pHeaderItem->GetColumnId();
            column.nColumnIndex = columnLayout.m_columns.size();
            column.nXBegin = pHeaderItem->GetRect().left - nViewLeft;
            column.nXEnd = pHeaderItem->GetRect().right - nViewLeft;
            columnLayout.m_columns.push_back(column);
        }
    }
    return columnLayout;
}

ListCtrlReportView::ListCtrlCellGeometry ListCtrlReportView::BuildCellGeometry() const
{
    ListCtrlCellGeometry cellGeometry;
    cellGeometry.rcViewport = GetPosWithoutPadding();
    cellGeometry.scrollPos = GetScrollOffset();
    cellGeometry.columnLayout = BuildColumnLayout(cellGeometry.rcViewport.left);
    if ((m_pListCtrl != nullptr) && (m_pListCtrl->GetFrozenColumnCount() > 0)) {
        cellGeometry.nFrozenCount = m_pListCtrl->GetFrozenColumnCount();
        //Frozen width = total width of the first N columns (including the row-leading CheckBox/padding placeholder widths, so the frozen-area boundary aligns with the column boundary)
        cellGeometry.nFrozenWidth = cellGeometry.columnLayout.GetColumnsWidth(cellGeometry.nFrozenCount);
        //A frozen width of 0 means there are no frozen columns (keeps the previous behavior)
        cellGeometry.bHasFrozen = cellGeometry.nFrozenWidth > 0;
    }
    return cellGeometry;
}

void ListCtrlReportView::PaintGridLines(IRender* pRender)
{
    if (pRender == nullptr) {
        return;
    }

    UiRect viewRect = GetRect();
    viewRect.Deflate(GetPadding());
    const bool bRectClip = IsClip();
    const bool bRoundClip = IsClip() && ShouldBeRoundRectFill();
    std::unique_ptr<AutoClip> rectClip = CreateRectClip(pRender, viewRect, bRectClip);
    std::unique_ptr<AutoClip> roundClip = CreateRoundClip(pRender, viewRect, bRoundClip);

    int32_t nColumnLineWidth = GetColumnGridLineWidth();//Vertical border line width        
    int32_t nRowLineWidth = GetRowGridLineWidth();   //Horizontal border line width
    UiColor columnLineColor;
    UiColor rowLineColor;
    std::string color = GetColumnGridLineColor();
    if (!color.empty()) {
        columnLineColor = GetUiColor(color);
    }
    color = GetRowGridLineColor();
    if (!color.empty()) {
        rowLineColor = GetUiColor(color);
    }

    if ((nColumnLineWidth > 0) && !columnLineColor.IsEmpty()) {
        //Draw vertical grid lines        
        int32_t yTop = viewRect.top;
        std::vector<int32_t> xPosList;
        const size_t itemCount = GetItemCount();
        for (size_t index = 0; index < itemCount; ++index) {
            if (index == 0) {
                ListCtrlHeader* pHeader = dynamic_cast<ListCtrlHeader*>(GetItemAt(index));
                if (pHeader != nullptr) {
                    if (pHeader->IsVisible() && (pHeader->GetHeight() > 0)) {
                        //Start drawing the line from the bottom of the Header
                        yTop = pHeader->GetRect().bottom;

                        //Use the header's splitter control position for the vertical grid lines
                        std::vector<UiRect> rcSplitControls;
                        pHeader->GetHeaderSplitControlRect(rcSplitControls);
                        if (!rcSplitControls.empty()) {
                            for (const UiRect& rc : rcSplitControls) {
                                //Use the right side of each column's list item header splitter control as the X-axis starting coordinate of the line
                                xPosList.push_back(rc.right);
                            }
                            break;
                        }
                    }
                    continue;
                }
            }
            ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(GetItemAt(index));
            if ((pItem == nullptr) || !pItem->IsVisible() || (pItem->GetWidth() <= 0)) {
                continue;
            }
            size_t nSubItemCount = pItem->GetItemCount();
            for (size_t nSubItem = 0; nSubItem < nSubItemCount; ++nSubItem) {
                ListCtrlSubItem* pSubItem = dynamic_cast<ListCtrlSubItem*>(pItem->GetItemAt(nSubItem));
                if ((pSubItem == nullptr) || !pSubItem->IsVisible() || (pSubItem->GetWidth() <= 0)) {
                    continue;
                }
                UiPoint scrollBoxOffset = pSubItem->GetScrollOffsetInScrollBox();
                UiRect subItemRect = pSubItem->GetRect();
                subItemRect.Offset(-scrollBoxOffset.x, -scrollBoxOffset.y);
                //Use the rightmost side of each column's list item as the X-axis starting coordinate of the line
                xPosList.push_back(subItemRect.right);
            }
            break;
        }

        //Vertical grid lines of the frozen columns: frozen columns do not move with horizontal scrolling
        //Reuse the same coordinate transform as painting (viewRect uses the same convention as the viewport rectangle of the coordinate transform)
        const ListCtrlCellGeometry cellGeometry = BuildCellGeometry();
        const int32_t nFrozenRight = cellGeometry.FrozenRight();

        for (size_t i = 0; i < xPosList.size(); ++i) {
            int32_t xPos = xPosList[i];
            bool bHasClip = false;
            if (cellGeometry.bHasFrozen) {
                UiRect rcClip = viewRect;
                if (i < cellGeometry.nFrozenCount) {
                    //Frozen column: restore the screen position that does not move with horizontal scrolling
                    xPos = cellGeometry.GridLineScreenX(xPos, true);
                    rcClip.right = nFrozenRight;
                }
                else {
                    //Non-frozen column: exclude the frozen-column area from the paint region
                    rcClip.left = nFrozenRight;
                }
                if (rcClip.left >= rcClip.right) {
                    continue;
                }
                pRender->SetClip(rcClip, true);
                bHasClip = true;
            }
            //Place the horizontal coordinate at the right side of each sub-item control
            float fXPos = (float)xPos - (float)nColumnLineWidth / 2;
            UiPointF pt1(fXPos, (float)yTop);
            UiPointF pt2(fXPos, (float)viewRect.bottom);
            pRender->DrawLine(pt1, pt2, columnLineColor, (float)nColumnLineWidth);
            if (bHasClip) {
                pRender->ClearClip();
            }
        }
    }
    if ((nRowLineWidth > 0) && !rowLineColor.IsEmpty()) {
        //Draw horizontal grid lines
        const size_t itemCount = GetItemCount();
        for (size_t index = 0; index < itemCount; ++index) {
            ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(GetItemAt(index));
            if ((pItem == nullptr) || !pItem->IsVisible() || (pItem->GetHeight() <= 0)) {
                continue;
            }
            //Place the vertical coordinate at the bottom of each sub-item control (no line at the bottom of the Header control)
            int32_t yPos = pItem->GetRect().bottom;
            int32_t nChildMarginY = 0;
            Layout* pLayout = GetLayout();
            if (pLayout != nullptr) {
                nChildMarginY = pLayout->GetChildMarginY();
            }
            yPos += nChildMarginY / 2;

            UiPointF pt1(viewRect.left, yPos);
            UiPointF pt2(viewRect.right, yPos);
            pRender->DrawLine(pt1, pt2, rowLineColor, (float)nRowLineWidth);
        }
    }
}

void ListCtrlReportView::PaintCellSelection(IRender* pRender, const ListCtrlCellGeometry& cellGeometry, bool bFrozenPass)
{
    if ((pRender == nullptr) || (m_pListCtrl == nullptr) || (m_pData == nullptr)) {
        return;
    }
    if (m_pListCtrl->GetSelectionMode() != ListCtrlSelectionMode::kCell) {
        return;
    }

    //Reuse the same coordinate transform as painting the data-item controls (clip rectangle / paint rectangle / window-origin offset)
    UiRect rcClip;
    UiRect rcNewPaint;
    UiPoint ptOffset;
    cellGeometry.GetPaintPass(bFrozenPass, rcClip, rcNewPaint, ptOffset);
    if (cellGeometry.bHasFrozen && (rcClip.left >= rcClip.right)) {
        return;
    }
    AutoClip alphaClip(pRender, rcClip, IsClip());
    UiPoint ptOldOrg = pRender->OffsetWindowOrg(ptOffset);

    const UiColor fillColor = GetUiColor("#66AACCEE");
    const UiColor focusColor = GetUiColor("#FF0078D7");
    const UiColor hoverColor = GetUiColor("#33AACCEE");

    //The current cell, determined cell by cell
    const ListCtrlCellId currentCell = m_pListCtrl->GetCurrentCell();

    //The header and pinned-to-top rows are fixed at the top: the areas they cover do not get a cell highlight, so the highlight does not cover the pinned-row content
    UiRect rcTopFixed;
    const bool bHasTopFixed = GetTopFixedItemsRect(rcTopFixed);

    const size_t nItemCount = GetItemCount();
    //The first row is the header, so iterate the data rows starting from index 1
    for (size_t index = 1; index < nItemCount; ++index) {
        ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(GetItemAt(index));
        if ((pItem == nullptr) || !pItem->IsVisible() || (pItem->GetWidth() <= 0)) {
            continue;
        }
        if (!pItem->IsSelectableType()) {
            //Pinned-to-top rows and other non-selectable rows do not participate in cell selection highlighting
            continue;
        }
        const size_t nRowIndex = pItem->GetDataItemIndex();
        const size_t nSubItemCount = pItem->GetItemCount();
        for (size_t nSubItem = 0; nSubItem < nSubItemCount; ++nSubItem) {
            ListCtrlSubItem* pSubItem = dynamic_cast<ListCtrlSubItem*>(pItem->GetItemAt(nSubItem));
            if ((pSubItem == nullptr) || !pSubItem->IsVisible() || (pSubItem->GetWidth() <= 0)) {
                continue;
            }
            const size_t nColumnId = pSubItem->GetDataColumnId();
            const size_t nColumnIndex = m_pListCtrl->GetColumnIndex(nColumnId);
            if (nColumnIndex == Box::InvalidIndex) {
                continue;
            }
            const bool bFrozenColumn = cellGeometry.bHasFrozen && (nColumnIndex < cellGeometry.nFrozenCount);
            if (bFrozenColumn != bFrozenPass) {
                continue;
            }
            UiRect rcCell = pSubItem->GetRect();
            if (bHasTopFixed && (rcCell.top < rcTopFixed.bottom)) {
                //The cell is covered by the header/pinned row: paint only its visible part, so the highlight does not cover the pinned-row content
                rcCell.top = rcTopFixed.bottom;
                if (rcCell.bottom <= rcCell.top) {
                    continue;
                }
            }

            //Decide whether the cell is selected by row identity (O(1), independent of the display order)
            const bool bSelected = m_pData->IsCellSelected(nRowIndex, nColumnId);
            const bool bCurrent = currentCell.IsValid() &&
                                  (currentCell.nRowIndex == nRowIndex) &&
                                  (currentCell.nColumnId == nColumnId);
            const bool bHovered = m_pListCtrl->IsCellHovered(nRowIndex, nColumnId);
            if (!bSelected && !bCurrent && !bHovered) {
                continue;
            }
            if (bSelected) {
                pRender->FillRect(UiRectF::MakeFromRect(rcCell), fillColor);
            }
            else if (bHovered) {
                //Hovered cell: a light fill hint
                pRender->FillRect(UiRectF::MakeFromRect(rcCell), hoverColor);
            }
            if (bCurrent) {
                pRender->DrawRect(UiRectF::MakeFromRect(rcCell), focusColor, 1.0f, false);
            }
        }
    }
    pRender->SetWindowOrg(ptOldOrg);
}

Control* ListCtrlReportView::FindControl(FINDCONTROLPROC Proc, void* pProcData,
                                         uint32_t uFlags, const UiPoint& ptMouse,
                                         const UiPoint& scrollPos)
{
    //Override ScrollBox::FindControl so that the header/pinned-to-top items are found first
    //Frozen columns are handled too: when the mouse is inside the frozen area, hit testing must use the "does not move with horizontal scrolling" coordinates
    std::vector<Control*> newItems = m_items;
    if ((uFlags & UIFIND_TOP_FIRST) != 0) {
        //Move the header and pinned-to-top elements to the end
        std::vector<Control*> atTopItems;
        MoveTopItemsToLast(newItems, atTopItems);
    }

    // Check if this guy is valid
    if ((uFlags & UIFIND_VISIBLE) != 0 && !IsVisible()) {
        return nullptr;
    }
    if ((uFlags & UIFIND_ENABLED) != 0 && !IsEnabled()) {
        return nullptr;
    }
    UiPoint boxPt(ptMouse);
    boxPt.Offset(scrollPos);
#ifdef _DEBUG
    if (((uFlags & UIFIND_HITTEST) != 0) && ((uFlags & UIFIND_DRAG_DROP) == 0) && (pProcData != nullptr)) {
        UiPoint ptOrg(*(UiPoint*)pProcData);
        ptOrg.Offset(this->GetScrollOffsetInScrollBox());
        ASSERT(ptOrg == boxPt);
    }
#endif // _DEBUG
    ScrollBar* pVScrollBar = GetVScrollBar();
    ScrollBar* pHScrollBar = GetHScrollBar();
    if ((uFlags & UIFIND_HITTEST) != 0) {
        if (!GetRect().ContainsPt(boxPt)) {
            return nullptr;
        }
        if (!IsMouseChildEnabled() && ((uFlags & UIFIND_TOOLTIP) == 0)) {
            Control* pResult = nullptr;            
            if (pVScrollBar != nullptr) {
                pResult = pVScrollBar->FindControl(Proc, pProcData, uFlags, boxPt);
            }            
            if ((pResult == nullptr) && (pHScrollBar != nullptr)) {
                pResult = pHScrollBar->FindControl(Proc, pProcData, uFlags, boxPt);
            }
            if (pResult == nullptr) {
                pResult = Control::FindControl(Proc, pProcData, uFlags, boxPt);
            }
            return pResult;
        }
    }

    Control* pResult = nullptr;
    if (pVScrollBar != nullptr) {
        pResult = pVScrollBar->FindControl(Proc, pProcData, uFlags, boxPt);
    }
    if ((pResult == nullptr) && (pHScrollBar != nullptr)) {
        pResult = pHScrollBar->FindControl(Proc, pProcData, uFlags, boxPt);
    }
    if (pResult != nullptr) {
        return pResult;
    }

    UiSize boxScrollOffset = GetScrollOffset();
    UiPoint boxScrollPos(boxScrollOffset.cx, boxScrollOffset.cy);
    bool bFrozenHitTestPass = false;
    if ((boxScrollPos.x != 0) && (m_pListCtrl != nullptr) && (m_pListCtrl->GetFrozenColumnCount() > 0)) {
        //Frozen columns exist and the view is scrolled horizontally: test whether the mouse is inside the frozen area
        const ListCtrlCellGeometry cellGeometry = BuildCellGeometry();
        if (cellGeometry.IsFrozenX(boxPt.x)) {
            //The mouse is inside the frozen area: frozen columns do not move with horizontal scrolling, so the horizontal scroll amount is 0 during the hit-test traversal
            //(1) the scroll amount passed to the children is (0, scrollY), which matches the coordinate space of the frozen columns;
            //(2) at the same time the horizontal component of the children's GetScrollOffsetInScrollBox() is 0,
            //    which keeps the framework's coordinate convention (custom data + control scroll amount == passed-in coordinate) valid and removes the Debug assertion;
            //(3) the find function decides the hit as "custom data + control scroll amount", so once the two above agree,
            //    the hit result matches the frozen columns' display position (cell tooltips and context menus benefit as well)
            boxScrollPos.x = 0;
            bFrozenHitTestPass = true;
        }
    }
    m_bFrozenHitTestPass = bFrozenHitTestPass;
    pResult = FindControlInItems(newItems, Proc, pProcData, uFlags, boxPt, boxScrollPos);
    m_bFrozenHitTestPass = false;
    return pResult;
}

UiSize ListCtrlReportView::GetScrollOffset() const
{
    UiSize scrollOffset = BaseClass::GetScrollOffset();
    if (m_bFrozenHitTestPass) {
        //Frozen-column hit-test traversal: the horizontal scroll amount counts as 0, so the children's coordinate system matches the frozen columns' display position
        scrollOffset.cx = 0;
    }
    return scrollOffset;
}

UiSize ListCtrlReportView::GetScrollOffsetForChild(const PlaceHolder* pChild) const
{
    UiSize scrollOffset = BaseClass::GetScrollOffsetForChild(pChild);
    //Children inside the frozen area are fixed at the left side of the viewport and do not move with horizontal scrolling: the horizontal offset counts as 0
    if ((scrollOffset.cx != 0) && IsChildInFrozenColumn(pChild)) {
        scrollOffset.cx = 0;
    }
    return scrollOffset;
}

bool ListCtrlReportView::IsChildInFrozenColumn(const PlaceHolder* pChild) const
{
    if ((pChild == nullptr) || (m_pListCtrl == nullptr)) {
        return false;
    }
    const size_t nFrozenCount = m_pListCtrl->GetFrozenColumnCount();
    if (nFrozenCount == 0) {
        return false;
    }
    //Look upwards for the cell that owns this control (data-row cells are ListCtrlSubItem, header cells are ListCtrlHeaderItem)
    size_t nColumnId = Box::InvalidIndex;
    const PlaceHolder* pControl = pChild;
    while ((pControl != nullptr) && (pControl != this)) {
        const ListCtrlSubItem* pSubItem = dynamic_cast<const ListCtrlSubItem*>(pControl);
        if (pSubItem != nullptr) {
            nColumnId = pSubItem->GetDataColumnId();
            break;
        }
        const ListCtrlHeaderItem* pHeaderItem = dynamic_cast<const ListCtrlHeaderItem*>(pControl);
        if (pHeaderItem != nullptr) {
            nColumnId = pHeaderItem->GetColumnId();
            break;
        }
        pControl = pControl->GetParent();
    }
    if (nColumnId == Box::InvalidIndex) {
        //No owning cell was found (for example the row-wide ListCtrlItem): handle it with the default convention
        return false;
    }
    const size_t nColumnIndex = m_pListCtrl->GetColumnIndex(nColumnId);
    return (nColumnIndex != Box::InvalidIndex) && (nColumnIndex < nFrozenCount);
}

Control* ListCtrlReportView::CreateDataItem()
{
    if (m_pListCtrl == nullptr) {
        return nullptr;
    }
    ListCtrlItem* pItem = new ListCtrlItem(GetWindow());
    pItem->SetListCtrl(m_pListCtrl);
    pItem->SetClass(m_pListCtrl->GetDataItemClass());
    pItem->SetAutoCheckSelect(m_pListCtrl->IsAutoCheckSelect());
    return pItem;
}

bool ListCtrlReportView::FillDataItem(Control* pControl,
                                     size_t nElementIndex,
                                     const ListCtrlItemData& itemData,
                                     const std::vector<ListCtrlSubItemData2Pair>& subItemList)
{
    //This function controls the display of the data element data and UI, and controls the display width of each column
    if (m_pListCtrl == nullptr) {
        return false;
    }
    ListCtrlHeader* pHeaderCtrl = m_pListCtrl->GetHeaderCtrl();
    if (pHeaderCtrl == nullptr) {
        return false;
    }

    ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(pControl);
    if (pItem == nullptr) {
        return false;
    }

    //Whether the data item displays a CheckBox    
    bool bShowCheckBox = m_pListCtrl->IsDataItemShowCheckBox();
    pItem->SetShowCheckBox(bShowCheckBox);
    pItem->SetEnableControlPadding(false);

    ASSERT(itemData.bVisible);
    bool bItemChecked = itemData.bChecked;
    int32_t nImageId = itemData.nImageId;
    if (!bShowCheckBox) {
        bItemChecked = false;
    }
    pItem->SetChecked(bItemChecked, false);
    pItem->SetImageId(nImageId);

    //Set the left padding to avoid overlapping between the CheckBox display and the text display
    const int32_t nPaddingLeft = pItem->GetItemPaddingLeft();
    UiPadding rcPadding = pItem->GetPadding();
    if (nPaddingLeft != rcPadding.left) {
        rcPadding.left = nPaddingLeft;
        pItem->SetPadding(rcPadding, false);
    }

    //The padding of the Header control
    const UiPadding rcHeaderPadding = pHeaderCtrl->GetPadding();

    // Basic structure: <ListCtrlItem> <ListCtrlSubItem/> ... <ListCtrlSubItem/>  </ListCtrlItem>
    // Additional notes: 1. ListCtrlItem is a subclass of HBox;   
    //          2. For each column, place one ListCtrlSubItem control
    //          3. ListCtrlSubItem is a subclass of LabelBox

    // For detailed structure description, see: ListCtrlItem.h

    //Find the cell data of this row by column ID: the number of columns is usually small (N <= ~10), so a linear search is cheaper than building a std::map per row,
    //and it makes no heap allocation (subItemList is already a small contiguous container)
    auto findSubItemData = [&subItemList](size_t nColumnId) -> const ListCtrlSubItemData2* {
        for (const ListCtrlSubItemData2Pair& dataPair : subItemList) {
            if (dataPair.nColumnId == nColumnId) {
                return dataPair.pSubItemData;
            }
        }
        return nullptr;
        };

    //Get the attributes of each column to be displayed
    struct ElementData
    {
        size_t nColumnIndex = Box::InvalidIndex;
        size_t nColumnId = Box::InvalidIndex;
        int32_t nColumnWidth = 0;
        const ListCtrlSubItemData2* pStorage = nullptr;
    };
    std::vector<ElementData> elementDataList;
    const size_t nColumnCount = pHeaderCtrl->GetColumnCount();
    elementDataList.reserve(nColumnCount);
    for (size_t nColumnIndex = 0; nColumnIndex < nColumnCount; ++nColumnIndex) {
        ListCtrlHeaderItem* pHeaderItem = pHeaderCtrl->GetColumn(nColumnIndex);
        if ((pHeaderItem == nullptr) || !pHeaderItem->IsColumnVisible()) {
            continue;
        }
        int32_t nColumnWidth = pHeaderCtrl->GetColumnWidth(nColumnIndex);
        if (nColumnWidth < 0) {
            nColumnWidth = 0;
        }
        ElementData data;
        data.nColumnIndex = nColumnIndex;
        data.nColumnId = pHeaderCtrl->GetColumnId(nColumnIndex);        
        data.pStorage = findSubItemData(data.nColumnId);
        if (nColumnIndex == 0) {
            //Column 1: ensure the header and the data item are aligned on the right side
            data.nColumnWidth = nColumnWidth + rcHeaderPadding.left - nPaddingLeft;
            if (data.nColumnWidth < 0) {
                data.nColumnWidth = 0;
            }
        }
        else {
            //Other columns starting from column 2
            data.nColumnWidth = nColumnWidth;
        }
        elementDataList.push_back(data);
    }

    ASSERT(!elementDataList.empty());
    if (elementDataList.empty()) {
        return false;
    }

    const size_t showColumnCount = elementDataList.size(); //Number of displayed columns
    while (pItem->GetItemCount() > showColumnCount) {
        //Remove redundant columns
        if (!pItem->RemoveItemAt(pItem->GetItemCount() - 1)) {
            ASSERT(!"RemoveItemAt failed!");
            return false;
        }
    }

    //Default attributes
    std::string defaultSubItemClass = m_pListCtrl->GetDataSubItemClass();
    ListCtrlSubItem defaultSubItem(m_pListCtrl->GetWindow());
    defaultSubItem.SetClass(defaultSubItemClass);
    //Cache the row's default state text colors for reuse by every cell of the row, avoiding repeated lookups per cell
    std::string defaultTextColors[kControlStateCount];
    for (int32_t nState = kControlStateNormal; nState < kControlStateCount; ++nState) {
        defaultTextColors[nState] = defaultSubItem.GetStateTextColor((ControlStateType)nState);
    }

    for (size_t nColumn = 0; nColumn < showColumnCount; ++nColumn) {
        const ElementData& elementData = elementDataList[nColumn];
        ListCtrlSubItem* pSubItem = nullptr;
        if (nColumn < pItem->GetItemCount()) {
            pSubItem = static_cast<ListCtrlSubItem*>(pItem->GetItemAt(nColumn));
            ASSERT(pSubItem != nullptr);
            if (pSubItem == nullptr) {
                return false;
            }
        }
        else {
            pSubItem = new ListCtrlSubItem(GetWindow());
            pSubItem->SetListCtrlItem(pItem);
            pSubItem->SetListBoxItem(pItem);
            pItem->AddItem(pSubItem);
            if (!defaultSubItemClass.empty()) {
                pSubItem->SetClass(defaultSubItemClass);
            }
            pSubItem->AttachMouseEnter([this, pItem, pSubItem](const EventArgs& args) {
                if (m_pListCtrl != nullptr) {
                    EventArgs msg = args;
                    msg.SetSender(m_pListCtrl);
                    msg.eventType = kEventSubItemMouseEnter;
                    msg.listCtrlType = (int32_t)ListCtrlType::Report;
                    msg.wParam = (WPARAM)pItem->GetListBoxIndex();
                    msg.lParam = (LPARAM)pSubItem->GetDataItemIndex();
                    msg.pEventData = pSubItem;
                    m_pListCtrl->FireAllEvents(msg);
                }
                return true;
                });
            pSubItem->AttachMouseLeave([this, pItem, pSubItem](const EventArgs& args) {
                if (m_pListCtrl != nullptr) {
                    EventArgs msg = args;
                    msg.SetSender(m_pListCtrl);
                    msg.eventType = kEventSubItemMouseLeave;
                    msg.listCtrlType = (int32_t)ListCtrlType::Report;
                    msg.wParam = (WPARAM)pItem->GetListBoxIndex();
                    msg.lParam = (LPARAM)pSubItem->GetDataItemIndex();
                    msg.pEventData = pSubItem;
                    m_pListCtrl->FireAllEvents(msg);
                }
                return true;
                });
        }
        //Set to not get focus
        pSubItem->SetNoFocus();

        //Set the associated column ID
        const size_t nColumnId = elementData.nColumnId;
        pSubItem->SetDataColumnId(nColumnId);

        //Set the editable attribute
        const EventCallbackID callbackID = (EventCallbackID)this;
        bool bEditable = (elementData.pStorage != nullptr) ? elementData.pStorage->bEditable : false;
        if (bEditable && m_pListCtrl->IsEnableItemEdit()) {            
            pSubItem->SetEnableEdit(true);
            pSubItem->DetachEventByID(kEventEnterEdit, callbackID);
            pSubItem->AttachEvent(kEventEnterEdit, [this, nElementIndex, nColumnId, pItem, pSubItem](const EventArgs& /*args*/) {
                if (m_pListCtrl != nullptr) {
                    m_pListCtrl->OnItemEnterEditMode(nElementIndex, nColumnId, pItem, pSubItem);
                }
                return true;
                }, callbackID);
        }
        else {
            pSubItem->SetEnableEdit(false);
            pSubItem->DetachEventByID(kEventEnterEdit, callbackID);
        }

        //Fill data and set attributes        
        pSubItem->SetFixedWidth(UiFixedInt(elementData.nColumnWidth), true, false);
        FillSubItemData(pSubItem, elementData.pStorage, defaultSubItem, defaultTextColors, nColumnId, nElementIndex);
    }
    //Deliver the data-fill callback of the current row
    SendEventReportViewItemFilled(pItem->GetListBoxIndex(), pItem->GetDataItemIndex(), pItem, nullptr);
    return true;
}

void ListCtrlReportView::FillSubItemData(ListCtrlSubItem* pSubItem,
                                         const ListCtrlSubItemData2* pStorage,
                                         const ListCtrlSubItem& defaultSubItem,
                                         const std::string* pDefaultTextColors,
                                         size_t nColumnId,
                                         size_t nElementIndex)
{
    ASSERT(pSubItem != nullptr);
    if (pSubItem == nullptr) {
        return;
    }
    ASSERT(pDefaultTextColors != nullptr);
    if (pDefaultTextColors == nullptr) {
        return;
    }
    if (pStorage != nullptr) {
        //Set the rich-text property before the text content: the text must be parsed as rich text
        pSubItem->SetRichText(pStorage->bRichText);
        pSubItem->SetText(pStorage->text.c_str());
        if (pStorage->nTextFormat != 0) {
            pSubItem->SetTextStyle(pStorage->nTextFormat, false);
        }
        else {
            pSubItem->SetTextStyle(defaultSubItem.GetTextStyle(), false);
        }
        pSubItem->SetTextPadding(defaultSubItem.GetTextPadding(), false);
        for (int32_t nState = kControlStateNormal; nState < kControlStateCount; ++nState) {
            ControlStateType stateType = (ControlStateType)nState;
            pSubItem->SetStateTextColor(stateType, pDefaultTextColors[nState]);
        }
        if (!pStorage->textColor.IsEmpty()) {
            pSubItem->SetStateTextColor(kControlStateNormal, pSubItem->GetColorString(pStorage->textColor));
        }
        if (!pStorage->bkColor.IsEmpty()) {
            pSubItem->SetBkColor(pStorage->bkColor);
        }
        else {
            pSubItem->SetBkColor(defaultSubItem.GetBkColor());
        }
        if (pStorage->bShowCheckBox) {
            //The data declares a CheckBox
            AddSubItemCheckBox(pSubItem, nColumnId, nElementIndex, pStorage->bChecked);
        }
        else {
            RemoveSubItemCheckBox(pSubItem);
        }
        pSubItem->SetImageId(pStorage->nImageId);
        pSubItem->SetProgress(pStorage->nProgress);
    }
    else {
        pSubItem->SetRichText(false);
        pSubItem->SetTextStyle(defaultSubItem.GetTextStyle(), false);
        pSubItem->SetText(defaultSubItem.GetText());
        pSubItem->SetTextPadding(defaultSubItem.GetTextPadding(), false);
        for (int32_t nState = kControlStateNormal; nState < kControlStateCount; ++nState) {
            ControlStateType stateType = (ControlStateType)nState;
            pSubItem->SetStateTextColor(stateType, pDefaultTextColors[nState]);
        }
        pSubItem->SetBkColor(defaultSubItem.GetBkColor());
        RemoveSubItemCheckBox(pSubItem);
        pSubItem->SetImageId(-1);
        pSubItem->SetProgress(-1);
    }
}

void ListCtrlReportView::SendEventReportViewItemFilled(size_t nListBoxIndex, size_t nElementIndex,
                                                       ListCtrlItem* pItem, ListCtrlSubItem* pSubItem)
{
    EventArgs msg;
    msg.SetSender(this);
    msg.eventType = kEventReportViewItemFilled;
    msg.vkCode = VirtualKeyCode::kVK_None;
    msg.wParam = (WPARAM)nListBoxIndex;
    msg.lParam = (LPARAM)nElementIndex;
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        msg.ptMouse = pWindow->GetLastMousePos();
    }
    msg.modifierKey = 0;
    msg.eventData = 0;
    msg.pEventData = pItem;
    msg.pExtraData = pSubItem;
    msg.listCtrlType = (int32_t)ListCtrlType::Report;

    //Dispatch the event
    SendEventMsg(msg);
}

void ListCtrlReportView::AddSubItemCheckBox(ListCtrlSubItem* pSubItem, size_t nColumnId, size_t nElementIndex, bool bChecked)
{
    ASSERT(pSubItem != nullptr);
    if (pSubItem == nullptr) {
        return;
    }
    //Add a CheckBox
    pSubItem->SetShowCheckBox(true);
    //Attach the CheckBox event handlers (detach first, so nothing is attached twice)
    pSubItem->DetachEvent(kEventCheck);
    pSubItem->DetachEvent(kEventUnCheck);
    pSubItem->SetChecked(bChecked, false);
    pSubItem->AttachCheck([this, nColumnId, nElementIndex](const EventArgs& /*args*/) {
        OnSubItemColumnChecked(nElementIndex, nColumnId, true);
        return true;
        });
    pSubItem->AttachUnCheck([this, nColumnId, nElementIndex](const EventArgs& /*args*/) {
        OnSubItemColumnChecked(nElementIndex, nColumnId, false);
        return true;
        });
}

void ListCtrlReportView::RemoveSubItemCheckBox(ListCtrlSubItem* pSubItem)
{
    ASSERT(pSubItem != nullptr);
    if (pSubItem == nullptr) {
        return;
    }
    if (!pSubItem->IsShowCheckBox()) {
        //No CheckBox is currently shown, so nothing needs removing (avoids a pointless ClearStateImages and event detach)
        return;
    }
    pSubItem->SetShowCheckBox(false);
    pSubItem->DetachEvent(kEventCheck);
    pSubItem->DetachEvent(kEventUnCheck);
    pSubItem->SetChecked(false, false);
}

bool ListCtrlReportView::RefreshSubItemData(size_t nElementIndex, size_t nColumnId)
{
    if ((m_pListCtrl == nullptr) || (m_pData == nullptr)) {
        return false;
    }
    if (!Box::IsValidItemIndex(nElementIndex) || (nElementIndex >= m_pData->GetDataItemCount())) {
        return false;
    }
    if (!m_pData->IsValidDataColumnId(nColumnId)) {
        return false;
    }
    //Get the data of this cell
    const ListCtrlData::Storage* pStorage = m_pData->GetSubItemStorage(nElementIndex, nColumnId);

    //Get the UI control currently bound to this data element
    size_t nItemIndex = GetDisplayItemIndex(nElementIndex);
    if (!Box::IsValidItemIndex(nItemIndex)) {
        //The data element is not in the visible range, so the UI needs no update (the data itself is already updated)
        return false;
    }
    ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(GetItemAt(nItemIndex));
    if (pItem == nullptr) {
        return false;
    }

    //Find the sub-item control of this column
    ListCtrlSubItem* pSubItem = nullptr;
    const size_t nSubItemCount = pItem->GetItemCount();
    for (size_t nIndex = 0; nIndex < nSubItemCount; ++nIndex) {
        ListCtrlSubItem* pItemSubItem = dynamic_cast<ListCtrlSubItem*>(pItem->GetItemAt(nIndex));
        if ((pItemSubItem != nullptr) && (pItemSubItem->GetDataColumnId() == nColumnId)) {
            pSubItem = pItemSubItem;
            break;
        }
    }
    if (pSubItem == nullptr) {
        //The control of this column does not exist (for example the column is hidden), or the cell is not in the visible range
        return false;
    }

    //Default attributes
    std::string defaultSubItemClass = m_pListCtrl->GetDataSubItemClass();
    ListCtrlSubItem defaultSubItem(m_pListCtrl->GetWindow());
    defaultSubItem.SetClass(defaultSubItemClass);
    //Cache the default state text colors for reuse by this cell
    std::string defaultTextColors[kControlStateCount];
    for (int32_t nState = kControlStateNormal; nState < kControlStateCount; ++nState) {
        defaultTextColors[nState] = defaultSubItem.GetStateTextColor((ControlStateType)nState);
    }

    //Update only this cell's data, without changing its width or event handlers
    FillSubItemData(pSubItem, pStorage, defaultSubItem, defaultTextColors, nColumnId, nElementIndex);
    pSubItem->Invalidate();
    //Deliver the cell's data-fill callback (consistent with the whole-row fill, so applications can customize the cell content dynamically)
    SendEventReportViewItemFilled(pItem->GetListBoxIndex(), nElementIndex, pItem, pSubItem);
    return true;
}

int32_t ListCtrlReportView::GetMaxDataItemWidth(const std::vector<const ListCtrlSubItemData2*>& subItemList)
{
    int32_t nMaxWidth = -1;
    if (m_pListCtrl == nullptr) {
        return nMaxWidth;
    }

    //Default attributes
    ListCtrlItem defaultItem(m_pListCtrl->GetWindow());
    defaultItem.SetListCtrl(m_pListCtrl);
    defaultItem.SetClass(m_pListCtrl->GetDataItemClass());

    std::string defaultSubItemClass = m_pListCtrl->GetDataSubItemClass();
    ListCtrlSubItem defaultSubItem(m_pListCtrl->GetWindow());
    defaultSubItem.SetClass(defaultSubItemClass);
    defaultSubItem.SetListCtrlItem(&defaultItem);

    ListCtrlSubItem subItem(m_pListCtrl->GetWindow());
    subItem.SetClass(defaultSubItemClass);
    subItem.SetListCtrlItem(&defaultItem);

    for (const ListCtrlSubItemData2* pStorage : subItemList) {
        if (pStorage == nullptr) {
            continue;
        }
        if (pStorage->text.empty()) {
            continue;
        }

        subItem.SetText(pStorage->text.c_str());
        if (pStorage->nTextFormat != 0) {
            subItem.SetTextStyle(pStorage->nTextFormat, false);
        }
        else {
            subItem.SetTextStyle(defaultSubItem.GetTextStyle(), false);
        }
        subItem.SetTextPadding(defaultSubItem.GetTextPadding(), false);
        subItem.SetShowCheckBox(pStorage->bShowCheckBox);
        subItem.SetImageId(pStorage->nImageId);
        subItem.SetFixedWidth(UiFixedInt::MakeAuto(), false, false);
        subItem.SetFixedHeight(UiFixedInt::MakeAuto(), false, false);
        subItem.SetReEstimateSize(true);
        UiEstSize sz = subItem.EstimateSize(UiSize(0, 0));
        nMaxWidth = std::max(nMaxWidth, sz.cx.GetInt32());
    }

    if (nMaxWidth <= 0) {
        nMaxWidth = -1;
    }
    else {
        //Add a little margin
        nMaxWidth += Dpi().GetScaleInt(4);
    }
    return nMaxWidth;
}

void ListCtrlReportView::AdjustSubItemWidth(const std::map<size_t, int32_t>& subItemWidths)
{
    if (subItemWidths.empty()) {
        return;
    }
    size_t itemCount = GetItemCount();
    for (size_t index = 1; index < itemCount; ++index) {
        ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(GetItemAt(index));
        if (pItem == nullptr) {
            continue;
        }
        size_t columnCount = pItem->GetItemCount();
        for (size_t nColumn = 0; nColumn < columnCount; ++nColumn) {
            auto iter = subItemWidths.find(nColumn);
            if (iter != subItemWidths.end()) {
                int32_t nColumnWidth = iter->second;
                if (nColumnWidth < 0) {
                    nColumnWidth = 0;
                }
                ListCtrlSubItem* pSubItem = dynamic_cast<ListCtrlSubItem*>(pItem->GetItemAt(nColumn));
                if (pSubItem != nullptr) {
                    pSubItem->SetFixedWidth(UiFixedInt(nColumnWidth), true, false);
                }
            }
        }
    }
}

void ListCtrlReportView::OnSubItemColumnChecked(size_t nElementIndex, size_t nColumnId, bool bChecked)
{
    ListCtrlData* pDataProvider = m_pData;
    ASSERT(pDataProvider != nullptr);
    if (pDataProvider != nullptr) {
        pDataProvider->SetSubItemCheck(nElementIndex, nColumnId, bChecked, false);
        //Update the check state of the corresponding header column
        if (m_pListCtrl != nullptr) {
            m_pListCtrl->UpdateHeaderColumnCheckBox(nColumnId);
        }
    }
}

size_t ListCtrlReportView::GetDisplayItemCount(bool /*bIsHorizontal*/, size_t& nColumns, size_t& nRows) const
{
    nColumns = 1;
    size_t nDiplayItemCount = m_diplayItemIndexList.size();
    size_t nAtTopItemCount = m_atTopControlList.size();
    nRows = nDiplayItemCount;
    if (nRows > nAtTopItemCount) {
        nRows -= nAtTopItemCount;//Subtract the pinned-to-top items
    }
    if (nRows > 1) {
        if ((m_pListCtrl != nullptr) && (m_pListCtrl->GetHeaderHeight() > 0)) {
            nRows -= 1;//Subtract the Header
        }
    }
    return nRows * nColumns;
}

bool ListCtrlReportView::IsSelectableElement(size_t nElementIndex) const
{
    bool bSelectable = true;
    ListCtrlData* pDataProvider = m_pData;
    ASSERT(pDataProvider != nullptr);
    if (pDataProvider != nullptr) {
        bSelectable = pDataProvider->IsSelectableElement(nElementIndex);
    }
    return bSelectable;
}

size_t ListCtrlReportView::FindSelectableElement(size_t nElementIndex, bool bForward) const
{
    ListCtrlData* pDataProvider = m_pData;
    if (pDataProvider == nullptr) {
        return nElementIndex;
    }
    const size_t nElementCount = pDataProvider->GetElementCount();
    if ((nElementCount == 0) || (nElementIndex >= nElementCount)) {
        return Box::InvalidIndex;
    }
    if (!pDataProvider->IsSelectableElement(nElementIndex)) {
        size_t nStartIndex = nElementIndex;
        nElementIndex = Box::InvalidIndex;
        if (bForward) {
            //Search forward for the next one that is not pinned to top
            for (size_t i = nStartIndex + 1; i < nElementCount; ++i) {
                if (pDataProvider->IsSelectableElement(i)) {
                    nElementIndex = i;
                    break;
                }
            }
        }
        else {
            //Search backward for the next one that is not pinned to top
            for (int32_t i = (int32_t)nStartIndex - 1; i >= 0; --i) {
                if (pDataProvider->IsSelectableElement(i)) {
                    nElementIndex = i;
                    break;
                }
            }
        }        
    }
    return nElementIndex;
}

void ListCtrlReportView::GetScrollDeltaValue(int32_t& nHScrollValue, int32_t& nVScrollValue) const
{
    nHScrollValue = DUI_NOSET_VALUE;
    nVScrollValue = DUI_NOSET_VALUE;
    int32_t deltaValue = DUI_NOSET_VALUE;
    if (m_pListCtrl != nullptr) {
        deltaValue = m_pListCtrl->GetDataItemHeight() * 2;
    }
    if (deltaValue > 0) {
        deltaValue = std::max(GetRect().Height() / 3, deltaValue);
        nVScrollValue = deltaValue;
    }
}

bool ListCtrlReportView::CanPaintSelectedColors(bool bHasStateImages) const
{
    //The row-selected background color is a visual exclusive to row-selection mode; it is suppressed in cell-selection mode,
    //so the row selection state (whichever entry point changed it: mouse/keyboard/API) is never shown, and the row's selectable state is not visible
    if ((m_pListCtrl != nullptr) && (m_pListCtrl->GetSelectionMode() == ListCtrlSelectionMode::kCell)) {
        return false;
    }
    return BaseClass::CanPaintSelectedColors(bHasStateImages);
}

bool ListCtrlReportView::OnLButtonClickedBlank()
{
    if ((m_pListCtrl != nullptr) && (m_pListCtrl->GetSelectionMode() == ListCtrlSelectionMode::kCell)) {
        //Cell-selection mode: clicking a blank area clears the cell selection (row selection is not involved, so return false to avoid a spurious row-selection-changed event)
        m_pListCtrl->ClearCellSelection();
        return false;
    }
    return BaseClass::OnLButtonClickedBlank();
}

bool ListCtrlReportView::OnRButtonClickedBlank()
{
    if ((m_pListCtrl != nullptr) && (m_pListCtrl->GetSelectionMode() == ListCtrlSelectionMode::kCell)) {
        //Cell-selection mode: clicking a blank area clears the cell selection (row selection is not involved, so return false to avoid a spurious row-selection-changed event)
        m_pListCtrl->ClearCellSelection();
        return false;
    }
    return BaseClass::OnRButtonClickedBlank();
}

bool ListCtrlReportView::OnFrameSelection(int64_t left, int64_t right, int64_t top, int64_t bottom)
{
    if ((m_pListCtrl != nullptr) && (m_pListCtrl->GetSelectionMode() == ListCtrlSelectionMode::kCell)) {
        //Cell-selection mode: hit cells with the drag-selection rectangle
        return OnFrameSelectionForCells(left, right, top, bottom);
    }
    if (left > GetListCtrlWidth()) {
        //In the blank area, no frame selection is performed; just clear all selections
        bool bRet = SetSelectNone();
        if (bRet) {
            OnSelectStatusChanged();
        }        
        return bRet;
    }
    ASSERT(top <= bottom);
    if (top > bottom) {
        return false;
    }
    if (m_pListCtrl == nullptr) {
        return false;
    }
    ListCtrlData* pDataProvider = m_pData;
    if (pDataProvider == nullptr) {
        return false;
    }
    const size_t dataItemCount = pDataProvider->GetDataItemCount();
    if (dataItemCount == 0) {
        return false;
    }

    const int32_t nDefaultItemHeight = m_pListCtrl->GetDataItemHeight(); //Default row height
    int32_t nTopItemHeights = m_pListCtrl->GetHeaderHeight(); //Height occupied by the Header and pinned-to-top elements

    std::vector<size_t> itemIndexList;

    int64_t totalItemHeight = 0;
    int32_t nItemHeight = 0;    
    for (size_t index = 0; index < dataItemCount; ++index) {
        const ListCtrlItemData* pRowData = pDataProvider->GetDisplayItemData(index);
        if (pRowData == nullptr) {
            continue;
        }
        const ListCtrlItemData& rowData = *pRowData;
        nItemHeight = (rowData.nItemHeight < 0) ? nDefaultItemHeight : rowData.nItemHeight;
        if (!rowData.bVisible || (nItemHeight == 0)) {
            //Not visible, skip
            continue;
        }

        if (rowData.nAlwaysAtTop >= 0) {
            //Pinned-to-top element
            nTopItemHeights += nItemHeight;
            continue;
        }
    }

    top -= nTopItemHeights;
    bottom -= nTopItemHeights;
    if (top < 0) {
        top = 0;
    }
    if (bottom < 0) {
        bottom = 0;
    }
    for (size_t index = 0; index < dataItemCount; ++index) {
        const ListCtrlItemData* pRowData = pDataProvider->GetDisplayItemData(index);
        if (pRowData == nullptr) {
            continue;
        }
        const ListCtrlItemData& rowData = *pRowData;
        nItemHeight = (rowData.nItemHeight < 0) ? nDefaultItemHeight : rowData.nItemHeight;
        if (!rowData.bVisible || (nItemHeight == 0)) {
            //Not visible, skip
            continue;
        }

        if (rowData.nAlwaysAtTop >= 0) {
            //Pinned-to-top element, exclude it
            continue;
        }
        totalItemHeight += nItemHeight;
        if (totalItemHeight > top) {
            //Start
            itemIndexList.push_back(index);
        }
        if (totalItemHeight > bottom) {
            //End
            break;
        }
    }

    //Select the frame-selected data
    bool bRet = SetSelectedElements(itemIndexList, true);
    if (bRet) {
        OnSelectStatusChanged();
    }    
    return bRet;
}

bool ListCtrlReportView::OnFrameSelectionForCells(int64_t left, int64_t right, int64_t top, int64_t bottom)
{
    if ((m_pListCtrl == nullptr) || (m_pData == nullptr)) {
        return false;
    }
    if (left > right) {
        const int64_t nTemp = left;
        left = right;
        right = nTemp;
    }
    if (top > bottom) {
        const int64_t nTemp = top;
        top = bottom;
        bottom = nTemp;
    }
    //Convert the drag-selection rectangle into the same coordinate system as the sub-item control rectangles:
    //the coordinates passed in by ListBoxHelper are "content coordinates" (including the ListBox's own scroll amount), while the cell sub-item rectangles are in "control coordinates".
    //vertically the layout is a virtual scroll layout (reused controls are placed at visible positions), so a control's rectangle position is its screen position and does not move with vertical scrolling;
    //the vertical scroll amount must therefore be subtracted before hit testing, otherwise after scrolling the actual drag-selection position differs from the mouse position by one scroll amount.
    //(horizontally this is the real scrollbar offset: painting shifts by -offset and hit testing restores by +offset, so both conventions agree)
    const UiRect rcView = GetRect();
    const int64_t nScrollPosY = GetScrollPos().cy;
    const int64_t frameLeft = left + (int64_t)rcView.left;
    const int64_t frameRight = right + (int64_t)rcView.left;
    const int64_t frameTop = top + (int64_t)rcView.top - nScrollPosY;
    const int64_t frameBottom = bottom + (int64_t)rcView.top - nScrollPosY;

    //Horizontal convention fix for frozen columns: the horizontal coordinates given by ListBoxHelper are always "window coordinate + horizontal scroll amount"
    //but the frozen columns (the first N columns) are painted with a horizontal window-origin offset of 0 (that is, content coordinate == window coordinate) and do not move with horizontal scrolling.
    //So for the frozen columns the drag-selection rectangle must be shifted left by one scroll amount; each of the two areas is clipped to its own range,
    //so a drag-selection region crossing the frozen boundary does not hit columns on the other side (without frozen columns both equal frameLeft/frameRight)
    const ListCtrlCellGeometry cellGeometry = BuildCellGeometry();
    int64_t frozenFrameLeft = frameLeft;
    int64_t frozenFrameRight = frameRight;
    int64_t scrollFrameLeft = frameLeft;
    int64_t scrollFrameRight = frameRight;
    if (cellGeometry.bHasFrozen) {
        const int64_t nScrollX = cellGeometry.scrollPos.cx;
        const int64_t nFrozenLeft = cellGeometry.rcViewport.left;
        const int64_t nFrozenRight = cellGeometry.FrozenRight();
        //Frozen column: shift the rectangle left by the scroll amount and clip it to the horizontal content range of the frozen area
        frozenFrameLeft = std::max<int64_t>(frameLeft - nScrollX, nFrozenLeft);
        frozenFrameRight = std::min<int64_t>(frameRight - nScrollX, nFrozenRight);
        //Non-frozen column: clip the rectangle to the horizontal range to the right of "frozen-area right edge + scroll amount"
        scrollFrameLeft = std::max<int64_t>(frameLeft, nFrozenRight + nScrollX);
        scrollFrameRight = frameRight;
    }

    //The header and pinned-to-top rows are fixed at the top and cover the normal rows scrolling below: those areas do not participate in cell drag-selection
    UiRect rcTopFixed;
    const bool bHasTopFixed = GetTopFixedItemsRect(rcTopFixed);
    if (bHasTopFixed &&
        (frameBottom <= (int64_t)rcTopFixed.bottom) &&
        (frameLeft < (int64_t)rcTopFixed.right) && (frameRight > (int64_t)rcTopFixed.left)) {
        //The drag-selection rectangle lies entirely inside the header/pinned-row area: no cell selection state is changed
        return false;
    }

    //Ctrl+drag: add only (hits become selected, non-hits keep their state); normal drag: replace (hits become selected, visible non-hits are deselected)
    const bool bAdditive = m_pListCtrl->m_bCellDragAdditive;
    bool bChanged = false;
    const size_t nItemCount = GetItemCount();
    //The first row is the header, so iterate the data rows starting from index 1
    for (size_t index = 1; index < nItemCount; ++index) {
        ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(GetItemAt(index));
        if ((pItem == nullptr) || !pItem->IsVisible()) {
            continue;
        }
        if (!pItem->IsSelectableType()) {
            //Pinned-to-top rows and other non-selectable rows do not participate in cell drag-selection
            continue;
        }
        const size_t nRowIndex = pItem->GetDataItemIndex();
        const size_t nSubItemCount = pItem->GetItemCount();
        for (size_t nSubItem = 0; nSubItem < nSubItemCount; ++nSubItem) {
            ListCtrlSubItem* pSubItem = dynamic_cast<ListCtrlSubItem*>(pItem->GetItemAt(nSubItem));
            if ((pSubItem == nullptr) || !pSubItem->IsVisible() || (pSubItem->GetWidth() <= 0)) {
                continue;
            }
            const size_t nColumnId = pSubItem->GetDataColumnId();
            const size_t nColumnIndex = m_pListCtrl->GetColumnIndex(nColumnId);
            if (nColumnIndex == Box::InvalidIndex) {
                continue;
            }
            UiRect rcCell = pSubItem->GetRect();
            if (bHasTopFixed && (rcCell.top < rcTopFixed.bottom)) {
                //The part of a cell covered by the header/pinned row does not participate in hit testing, so the covered area below a pinned row is not selected
                rcCell.top = rcTopFixed.bottom;
                if (rcCell.bottom <= rcCell.top) {
                    continue;
                }
            }
            //Choose the horizontal hit range of the corresponding area depending on whether the column is frozen (both are the same without frozen columns)
            const bool bFrozenColumn = cellGeometry.bHasFrozen && (nColumnIndex < cellGeometry.nFrozenCount);
            const int64_t nCellFrameLeft = bFrozenColumn ? frozenFrameLeft : scrollFrameLeft;
            const int64_t nCellFrameRight = bFrozenColumn ? frozenFrameRight : scrollFrameRight;
            const bool bHit = (rcCell.right > nCellFrameLeft) && (rcCell.left < nCellFrameRight) &&
                              (rcCell.bottom > frameTop) && (rcCell.top < frameBottom);
            if (bAdditive && !bHit) {
                //Add semantics: cells that are not hit keep their selection state
                continue;
            }
            if (m_pData->SetCellSelected(nRowIndex, nColumnId, bHit)) {
                bChanged = true;
            }
        }
    }
    if (bChanged) {
        m_pListCtrl->NotifyCellSelectionChanged();
        m_pListCtrl->RefreshCellSelectionView();
    }
    return bChanged;
}

void ListCtrlReportView::OnItemSelectedChanged(size_t iIndex, IListBoxItem* pListBoxItem)
{
    if (!IsEnableUpdateProvider()) {
        return;
    }
    BaseClass::OnItemSelectedChanged(iIndex, pListBoxItem);
    OnSelectStatusChanged();
}

void ListCtrlReportView::OnItemCheckedChanged(size_t /*iIndex*/, IListBoxItem* pListBoxItem)
{
    if (!IsEnableUpdateProvider()) {
        return;
    }
    if (pListBoxItem == nullptr) {
        return;
    }
    ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(pListBoxItem);
    if (pItem == nullptr) {
        return;
    }

    ListCtrlData* pDataProvider = m_pData;
    if (pDataProvider == nullptr) {
        return;
    }
    size_t nElementIndex = pListBoxItem->GetElementIndex();
    bool bCheckChanged = false;
    bool bChecked = pItem->IsChecked();
    pDataProvider->SetDataItemChecked(nElementIndex, bChecked, bCheckChanged);
    if (bCheckChanged) {
        //Update the check state of the header
        if (m_pListCtrl != nullptr) {
            m_pListCtrl->UpdateHeaderCheckBox();
        }
    }
}

void ListCtrlReportView::OnSelectStatusChanged()
{
    if ((m_pListCtrl != nullptr) && m_pListCtrl->IsAutoCheckSelect()) {
        //Update the check state of the header
        m_pListCtrl->UpdateHeaderCheckBox();
    }
}

void ListCtrlReportView::SetRowGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nLineWidth);
    }
    if (nLineWidth < 0) {
        nLineWidth = 0;
    }
    if (m_nRowGridLineWidth != nLineWidth) {
        m_nRowGridLineWidth = nLineWidth;
        Invalidate();
    }
}

int32_t ListCtrlReportView::GetRowGridLineWidth() const
{   
    return m_nRowGridLineWidth;
}

void ListCtrlReportView::SetRowGridLineColor(const std::string& color)
{
    if (m_rowGridLineColor != color) {
        m_rowGridLineColor = color;
        Invalidate();
    }
}

std::string ListCtrlReportView::GetRowGridLineColor() const
{
    return m_rowGridLineColor.c_str();
}

void ListCtrlReportView::SetColumnGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nLineWidth);
    }
    if (nLineWidth < 0) {
        nLineWidth = 0;
    }
    if (m_nColumnGridLineWidth != nLineWidth) {
        m_nColumnGridLineWidth = nLineWidth;
        Invalidate();
    }
}

int32_t ListCtrlReportView::GetColumnGridLineWidth() const
{
    return m_nColumnGridLineWidth;
}

void ListCtrlReportView::SetColumnGridLineColor(const std::string& color)
{
    if (m_columnGridLineColor != color) {
        m_columnGridLineColor = color;
        Invalidate();
    }
}

std::string ListCtrlReportView::GetColumnGridLineColor() const
{
    return m_columnGridLineColor.c_str();
}

////////////////////////////////////////////////////////////////////////
/// Implementation of ListCtrlReportLayout

ListCtrlReportLayout::ListCtrlReportLayout():
    m_pDataView(nullptr),
    m_bReserveSet(false)
{
}

void ListCtrlReportLayout::SetDataView(ListCtrlReportView* pDataView)
{
    ASSERT(pDataView != nullptr);
    m_pDataView = pDataView;
}

UiSize64 ListCtrlReportLayout::ArrangeChildren(const std::vector<ui::Control*>& /*items*/, ui::UiRect rc, bool bEstimateOnly)
{
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView == nullptr) || !pDataView->HasDataProvider()) {
        ASSERT(0);
        return UiSize64();
    }
    DeflatePadding(rc);
    const int32_t nHeaderHeight = GetHeaderHeight();
    int64_t nTotalHeight = GetElementsHeight(Box::InvalidIndex, true) + nHeaderHeight;
    UiSize64 sz(rc.Width(), rc.Height());
    sz.cy = std::max(nTotalHeight, sz.cy);
    m_bReserveSet = false;
    if (nTotalHeight > rc.Height()) {
        //A scroll bar is needed, reserve space at the bottom
        sz.cy += GetReserveHeight();
        if (pDataView->GetHScrollBar() != nullptr) {
            sz.cy += pDataView->GetHScrollBar()->GetHeight();
        }
        m_bReserveSet = true;
    }
    sz.cx = std::max(GetItemWidth(), rc.Width()); //Allow a horizontal scroll bar to appear
    if (!bEstimateOnly) {
        LazyArrangeChild(rc);
    }    
    return sz;
}

UiSize64 ListCtrlReportLayout::EstimateLayoutSize(const std::vector<Control*>& items, ui::UiSize szAvailable)
{
    //When estimating the control size (mainly for the case where the width or height is "auto"), only estimate the size of the container itself, not including the size of the list data
    //Because the list is implemented as a virtual list with a large data scale, it is not suitable for estimating the size of "auto" controls
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView == nullptr) || !pDataView->HasDataProvider()) {
        ASSERT(0);
        //If the data interface is not set, be compatible with the base class functionality
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
        nTotalWidth = GetItemWidth();
        if (nTotalWidth > 0) {
            nTotalWidth += (rcPadding.left + rcPadding.right);
        }
    }
    if (nTotalHeight == 0) {
        nTotalHeight = szAvailable.cy;
    }
    return UiSize64(nTotalWidth, nTotalHeight);
}

void ListCtrlReportLayout::LazyArrangeChild(UiRect rc) const
{
    rc.Validate();
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView == nullptr) || !pDataView->HasDataProvider()) {
        ASSERT(0);
        return;
    }
    UiRect orgRect = rc;

    //Clear the cached data
    pDataView->SetAtTopControlIndex(std::vector<size_t>());
    pDataView->SetTopElementIndex(Box::InvalidIndex);
    pDataView->SetDisplayDataItems(std::vector<size_t>());
    pDataView->SetNormalItemTop(-1);

    if (pDataView->IsNormalMode()) {
        //Normal mode
        LazyArrangeChildNormal(rc);
        return;
    }

    const size_t nItemCount = pDataView->GetItemCount();
    if (nItemCount > 0) {
        //The first element is the header control, set its position and size
        ListCtrlHeader* pHeaderCtrl = dynamic_cast<ListCtrlHeader*>(pDataView->GetItemAt(0));
        if ((pHeaderCtrl != nullptr) && pHeaderCtrl->IsVisible()) {
            int32_t nHeaderHeight = pHeaderCtrl->GetFixedHeight().GetInt32();
            if (nHeaderHeight < 0) {
                nHeaderHeight = 0;
            }
            //Width of the header
            int32_t nHeaderWidth = std::max(GetItemWidth(), rc.Width());
            if (nHeaderWidth <= 0) {
                nHeaderWidth = rc.Width();
            }
            ui::UiRect rcTile(rc.left, rc.top, rc.left + nHeaderWidth, rc.top + nHeaderHeight);
            pHeaderCtrl->SetPos(rcTile);
            rc.top += nHeaderHeight;
        }
    }
    int32_t nNormalItemTop = rc.top; //The top coordinate of normal list items (not Header, not pinned to top)

    //Record the list of visible element indices
    std::vector<size_t> diplayItemIndexList;
    diplayItemIndexList.reserve(nItemCount);

    //Y coordinate position of the scroll bar
    int64_t nScrollPosY = pDataView->GetScrollPos().cy;

    //Calculate how many data items the current region can display
    int32_t nCalcItemCount = 0;
    if (!rc.IsEmpty()) {
        pDataView->GetMaxDataItemsToShow(nScrollPosY, rc.Height());
    }
    if (nCalcItemCount > (int32_t)(nItemCount - 1)) {
        //The number of UI controls is insufficient, adjust again
        pDataView->AjustItemCount();
    }

    //Get the list of data element indices to display
    std::vector<ListCtrlReportView::ShowItemInfo> showItemIndexList;
    std::vector<ListCtrlReportView::ShowItemInfo> atTopItemIndexList;
    int64_t nPrevItemHeights = 0;
    pDataView->GetDataItemsToShow(nScrollPosY, nItemCount - 1, 
                                  showItemIndexList, atTopItemIndexList, nPrevItemHeights);
    if (showItemIndexList.empty() && atTopItemIndexList.empty()) {
        //No data to display
        pDataView->SetScrollVirtualOffsetY(nScrollPosY);
        pDataView->SetTopElementIndex(0);
        pDataView->SetAtTopControlIndex(std::vector<size_t>());
        pDataView->SetDisplayDataItems(diplayItemIndexList);
        pDataView->SetNormalItemTop(nNormalItemTop);
        return;
    }

    // Index of the top element
    size_t nTopElementIndex = Box::InvalidIndex;
    if (!atTopItemIndexList.empty()) {
        nTopElementIndex = atTopItemIndexList.front().nItemIndex;
    }
    else if (!showItemIndexList.empty()) {
        nTopElementIndex = showItemIndexList.front().nItemIndex;
    }
    pDataView->SetTopElementIndex(nTopElementIndex); 

    //Set the virtual offset; otherwise, when the data volume is large, the height of the 32-bit rc rectangle will overflow and requires a 64-bit integer to hold it
    pDataView->SetScrollVirtualOffsetY(nScrollPosY);

    //The Y-axis offset of the first data item needs to be maintained to avoid deviation when the interface is refreshed after the scroll position changes, which would cause the last data item to be displayed incompletely
    int32_t yOffset = 0;
    if ((nScrollPosY > 0) && !showItemIndexList.empty()) {
        int32_t nFirstHeight = showItemIndexList.front().nItemHeight;
        if (nFirstHeight > 0) {            
            yOffset = std::abs(nScrollPosY - nPrevItemHeights) % nFirstHeight;
        }
    }
    if ((nScrollPosY > 0) && (nScrollPosY == pDataView->GetScrollRange().cy)) {
        //The scroll bar has reached the bottom
        if (!showItemIndexList.empty()) {
            size_t nLastItemIndex = showItemIndexList[showItemIndexList.size() - 1].nItemIndex;
            if (nLastItemIndex == pDataView->GetElementCount() - 1) {
                //It is already the last record, ensure the bottom is displayed completely
                int32_t rcHeights = rc.Height();
                if (m_bReserveSet && (rcHeights > GetReserveHeight())) {
                    rcHeights -= GetReserveHeight();
                    if (pDataView->GetHScrollBar() != nullptr) {
                        rcHeights -= pDataView->GetHScrollBar()->GetHeight();
                    }                    
                }
                for (const auto& info : atTopItemIndexList) {
                    rcHeights -= info.nItemHeight;
                }
                for (int32_t nIndex = (int32_t)showItemIndexList.size() - 1; nIndex >= 0; --nIndex) {
                    const auto& info = showItemIndexList[nIndex];
                    if ((rcHeights - info.nItemHeight) > 0) {
                        rcHeights -= info.nItemHeight;
                    }
                }
                if (rcHeights > 0) {
                    int32_t nFirstHeight = 0;
                    if (!showItemIndexList.empty()) {
                        nFirstHeight = showItemIndexList.front().nItemHeight;
                    }
                    if (nFirstHeight > 0) {
                        yOffset = nFirstHeight - (rcHeights % nFirstHeight);
                    }
                }
            }
        }
    }

    struct ShowItemIndex
    {
        size_t nElementIndex;   //Element index 
        int32_t nItemHeight;    //Height of the element
        bool bAtTop;            //Whether it is pinned to top
        int32_t yOffset;        //Y-axis offset
    };
    std::vector<ShowItemIndex> itemIndexList;
    itemIndexList.reserve(atTopItemIndexList.size() + showItemIndexList.size());
    for (const auto& info : atTopItemIndexList) {
        itemIndexList.push_back({ info.nItemIndex, info.nItemHeight, true, 0});
    }
    for (const auto& info : showItemIndexList) {
        itemIndexList.push_back({ info.nItemIndex, info.nItemHeight, false, yOffset });
        yOffset = 0;    //Only the first element sets the offset
    }
    //Width of the elements: all elements have the same width
    const int32_t cx = GetItemWidth(); 
    ASSERT(cx > 0);

    //The top-left corner coordinate of the control
    ui::UiPoint ptTile(rc.left, rc.top);

    VirtualListBox::RefreshDataList refreshDataList;
    refreshDataList.reserve(nItemCount);
    VirtualListBox::RefreshData refreshData;

    UiSize szItem;
    size_t iCount = 0;
    std::vector<size_t> atTopUiItemIndexList;
    atTopUiItemIndexList.reserve(nItemCount);
    //The first element is the header control, whose data is not filled, so start from 1
    for (size_t index = 1; index < nItemCount; ++index) {
        Control* pControl = pDataView->GetItemAt(index);
        if (pControl == nullptr) {
            continue;
        }

        bool bAlwaysAtTop = false;
        size_t nElementIndex = Box::InvalidIndex;
        bool bFillElement = true;
        if (iCount < itemIndexList.size()) {
            //Index of the current data element
            const ShowItemIndex& showItemIndex = itemIndexList[iCount];
            nElementIndex = showItemIndex.nElementIndex;
            bAlwaysAtTop = showItemIndex.bAtTop;
            szItem.cx = cx;
            szItem.cy = showItemIndex.nItemHeight;
            ASSERT(szItem.cy > 0);

            //Set the size and position of the current control
            if (showItemIndex.yOffset > 0) {
                ptTile.y = ptTile.y - showItemIndex.yOffset;
            }
            UiRect rcTile(ptTile.x, ptTile.y, ptTile.x + szItem.cx, ptTile.y + szItem.cy);
            pControl->SetPos(rcTile);
            if (rcTile.top > orgRect.bottom) {
                //If the element is no longer visible, stop filling data
                bFillElement = false;                
            }
        }
        else {
            //Data display is complete
            bFillElement = false;
        }

        if (nElementIndex >= pDataView->GetElementCount()) {
            bFillElement = false;
        }
        
        if (bFillElement) {
            if (!pControl->IsVisible()) {
                pControl->SetVisible(true);
            }
            //Fill data on demand: if the control is already bound to the same data element, there is no need to fill its data again
            //(this avoids a "whole-table refill" of unchanged rows while scrolling, for skeleton screens, theme refreshes or repeated layouts)
            //ListCtrlItem also implements IListBoxItem, so cast once here instead of repeating dynamic_cast for every row
            ListCtrlItem* pListCtrlItem = dynamic_cast<ListCtrlItem*>(pControl);
            const bool bNeedFillData = (pListCtrlItem == nullptr) || (pListCtrlItem->GetElementIndex() != nElementIndex);
            if (bNeedFillData) {
                pDataView->FillElementData(pControl, nElementIndex);

                refreshData.nItemIndex = index;
                refreshData.pControl = pControl;
                refreshData.nElementIndex = nElementIndex;
                refreshDataList.push_back(refreshData);
            }
            diplayItemIndexList.push_back(nElementIndex);

            if (pListCtrlItem != nullptr) {
                //Pinned-to-top items are not selectable
                pListCtrlItem->SetSelectableType(bAlwaysAtTop ? false : true);
            }
            if (bAlwaysAtTop) {
                //Record the pinned-to-top items
                atTopUiItemIndexList.push_back(index);
                //Record the bottom coordinate of the pinned-to-top items
                nNormalItemTop = pControl->GetRect().bottom;
            }
        }
        else {
            if (pControl->IsVisible()) {
                pControl->SetVisible(false);
            }
            ListCtrlItem* pListCtrlItem = dynamic_cast<ListCtrlItem*>(pControl);
            if (pListCtrlItem != nullptr) {
                pListCtrlItem->SetElementIndex(Box::InvalidIndex);
            }
        }

        //Switch to the next row
        ptTile.y += szItem.cy + GetChildMarginY();
        ++iCount;
    }

    pDataView->SetAtTopControlIndex(atTopUiItemIndexList);
    pDataView->SetDisplayDataItems(diplayItemIndexList);    
    pDataView->SetNormalItemTop(nNormalItemTop);

    if (!refreshDataList.empty()) {
        pDataView->OnRefreshElements(refreshDataList);
        pDataView->OnFilledElements(refreshDataList);
    }
}

void ListCtrlReportLayout::LazyArrangeChildNormal(UiRect rc) const
{    
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView == nullptr) || !pDataView->HasDataProvider()) {
        ASSERT(0);
        return;
    }

    UiRect orgRect = rc;
    const size_t nItemCount = pDataView->GetItemCount();
    if (nItemCount > 0) {
        //The first element is the header control, set its position and size
        Control* pHeaderCtrl = pDataView->GetItemAt(0);
        if ((pHeaderCtrl != nullptr) && pHeaderCtrl->IsVisible()) {
            int32_t nHeaderHeight = pHeaderCtrl->GetFixedHeight().GetInt32();
            if (nHeaderHeight < 0) {
                nHeaderHeight = 0;
            }
            int32_t nHeaderWidth = GetElementSize(rc.Width(), 0).cx;
            if (nHeaderWidth <= 0) {
                nHeaderWidth = rc.Width();
            }
            ui::UiRect rcTile(rc.left, rc.top, rc.left + nHeaderWidth, rc.top + nHeaderHeight);
            pHeaderCtrl->SetPos(rcTile);
            rc.top += nHeaderHeight;
            //Record the bottom value of the header
            pDataView->SetNormalItemTop(rc.top);
        }
    }

    //Width of the elements: all elements have the same width
    const int32_t cx = GetItemWidth();
    if (cx <= 0) {
        return;
    }

    //Height of the elements: all elements have the same height
    const int32_t cy = GetItemHeight();
    ASSERT(cy > 0);
    if (cy <= 0) {
        return;
    }

    //Record the list of visible element indices
    std::vector<size_t> diplayItemIndexList;
    diplayItemIndexList.reserve(nItemCount);

    // Index of the top element
    const size_t nTopElementIndex = GetTopElementIndex(orgRect);
    pDataView->SetTopElementIndex(nTopElementIndex);

    //Y coordinate position of the scroll bar
    int64_t nScrollPosY = pDataView->GetScrollPos().cy;

    //The Y-axis offset needs to be maintained to avoid deviation when the interface is refreshed after the scroll position changes
    int32_t yOffset = 0;
    if (cy > 0) {
        yOffset = TruncateToInt32(nScrollPosY % cy);
    }

    //Set the virtual offset; otherwise, when the data volume is large, the height of the 32-bit rc rectangle will overflow and requires a 64-bit integer to hold it
    pDataView->SetScrollVirtualOffsetY(nScrollPosY);

    //The top-left corner coordinate of the control
    ui::UiPoint ptTile(rc.left, rc.top - yOffset);

    VirtualListBox::RefreshDataList refreshDataList;
    refreshDataList.reserve(nItemCount);
    VirtualListBox::RefreshData refreshData;

    size_t iCount = 0;
    //The first element is the header control; skip filling data, so start from 1
    for (size_t index = 1; index < nItemCount; ++index) {
        Control* pControl = pDataView->GetItemAt(index);
        if (pControl == nullptr) {
            continue;
        }
        //Index of the current data element
        const size_t nElementIndex = nTopElementIndex + iCount;

        //Set the size and position of the current control
        ui::UiRect rcTile(ptTile.x, ptTile.y, ptTile.x + cx, ptTile.y + cy);
        pControl->SetPos(rcTile);

        // Fill data        
        if (nElementIndex < pDataView->GetElementCount()) {
            if (!pControl->IsVisible()) {
                pControl->SetVisible(true);
            }
            //Fill data on demand: if the control is already bound to the same data element, there is no need to fill its data again
            //(avoids a "whole-table refill" of unchanged rows on repeated layouts, theme refreshes, and so on)
            //ListCtrlItem also implements IListBoxItem, so cast once here instead of repeating dynamic_cast for every row
            ListCtrlItem* pListCtrlItem = dynamic_cast<ListCtrlItem*>(pControl);
            const bool bNeedFillData = (pListCtrlItem == nullptr) || (pListCtrlItem->GetElementIndex() != nElementIndex);
            if (bNeedFillData) {
                pDataView->FillElementData(pControl, nElementIndex);

                refreshData.nItemIndex = index;
                refreshData.pControl = pControl;
                refreshData.nElementIndex = nElementIndex;
                refreshDataList.push_back(refreshData);
            }
            diplayItemIndexList.push_back(nElementIndex);
        }
        else {
            if (pControl->IsVisible()) {
                pControl->SetVisible(false);
            }
            ListCtrlItem* pListCtrlItem = dynamic_cast<ListCtrlItem*>(pControl);
            if (pListCtrlItem != nullptr) {
                pListCtrlItem->SetElementIndex(Box::InvalidIndex);
            }
        }

        ptTile.y += cy + GetChildMarginY();
        ++iCount;
    }
    pDataView->SetDisplayDataItems(diplayItemIndexList);
    if (!refreshDataList.empty()) {
        pDataView->OnRefreshElements(refreshDataList);
        pDataView->OnFilledElements(refreshDataList);
    }
}

size_t ListCtrlReportLayout::AjustMaxItem(UiRect rc) const
{
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView == nullptr) || !pDataView->HasDataProvider()) {
        ASSERT(0);
        return 1;
    }
    int32_t nItemHeight = GetItemHeight();
    ASSERT(nItemHeight > 0);
    if (nItemHeight <= 0) {
        return 1;
    }
    if (rc.IsEmpty()) {
        return 1;
    }
    int32_t nRows = 0;
    int32_t nHeaderHeight = GetHeaderHeight();
    if (nHeaderHeight > 0) {
        nRows += 1;
        rc.top += nHeaderHeight;
        rc.Validate();
    }

    nRows += rc.Height() / (nItemHeight + GetChildMarginY() / 2);
    //Validate and correct
    if (nRows > 1) {
        int32_t calcHeight = nRows * nItemHeight + (nRows - 1) * GetChildMarginY();
        if (calcHeight < rc.Height()) {
            nRows += 1;
        }
    }
    
    if (!pDataView->IsNormalMode()) {
        //Non-normal mode, requires complex calculation to get the result
        int64_t nScrollPosY = pDataView->GetScrollPos().cy;
        int32_t nCalcRows = pDataView->GetMaxDataItemsToShow(nScrollPosY, rc.Height());
        if (nCalcRows > 0) {
            nCalcRows += 1;
            if (nCalcRows > nRows) {
                nRows = nCalcRows;
            }
        }
    }

    //Add 1 extra row to ensure the real controls fill the entire displayable region
    nRows += 1;
    return nRows;
}

size_t ListCtrlReportLayout::GetTopElementIndex(UiRect /*rc*/) const
{
    size_t nTopElementIndex = 0;
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView == nullptr) || !pDataView->HasDataProvider()) {
        ASSERT(0);
        return nTopElementIndex;
    }
    int64_t nScrollPosY = pDataView->GetScrollPos().cy;
    if (!pDataView->IsNormalMode()) {
        //Non-normal mode
        nTopElementIndex = pDataView->GetTopDataItemIndex(nScrollPosY);
    }
    else {
        int32_t nItemHeight = GetItemHeight();
        ASSERT(nItemHeight > 0);
        if (nItemHeight > 0) {
            nTopElementIndex = static_cast<size_t>(nScrollPosY / nItemHeight);
        }
    }
    return nTopElementIndex;
}

bool ListCtrlReportLayout::IsElementDisplay(UiRect rc, size_t iIndex) const
{
    if (!Box::IsValidItemIndex(iIndex)) {
        return false;
    }
    std::vector<size_t> itemIndexList;
    GetDisplayElements(rc, itemIndexList);
    return std::find(itemIndexList.begin(), itemIndexList.end(), iIndex) != itemIndexList.end();
}

void ListCtrlReportLayout::GetDisplayElements(UiRect rc, std::vector<size_t>& collection) const
{
    GetDisplayElements(rc, collection, nullptr);
}

void ListCtrlReportLayout::GetDisplayElements(UiRect rc, std::vector<size_t>& collection,
                                            std::vector<size_t>* pAtTopItemIndexList) const
{
    collection.clear();
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView == nullptr) || !pDataView->HasDataProvider()) {
        ASSERT(0);
        return;
    }
    if (pDataView->GetItemCount() <= 1) {
        return;
    }
    int64_t nScrollPosY = pDataView->GetScrollPos().cy;
    if (!pDataView->IsNormalMode()) {
        //Non-normal mode
        pDataView->GetMaxDataItemsToShow(nScrollPosY, rc.Height(), 
                                         &collection, pAtTopItemIndexList);
        return;
    }

    int32_t nItemHeight = GetItemHeight();
    ASSERT(nItemHeight >= 0);
    if (nItemHeight <= 0) {
        return;
    }
    rc.top += GetHeaderHeight();
    rc.Validate();

    size_t min = (size_t)(nScrollPosY / nItemHeight);
    size_t max = min + (size_t)(rc.Height() / nItemHeight);
    size_t nCount = pDataView->GetElementCount();
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

bool ListCtrlReportLayout::NeedReArrange() const
{
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView == nullptr) || !pDataView->HasDataProvider()) {
        ASSERT(0);
        return false;
    }

    UiRect rcThis = pDataView->GetPos();
    if (rcThis.IsEmpty()) {
        return false;
    }

    int64_t nScrollPosY = pDataView->GetScrollPos().cy;//New scroll bar position
    int64_t nVirtualOffsetY = pDataView->GetScrollVirtualOffset().cy;//Original scroll bar position
    //As long as the scroll position changes, re-layout is needed
    return (nScrollPosY != nVirtualOffsetY);
}

void ListCtrlReportLayout::EnsureVisible(UiRect rc, size_t iIndex, bool bToTop) const
{
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView == nullptr) || !pDataView->HasDataProvider()) {
        ASSERT(0);
        return;
    }
    if (!Box::IsValidItemIndex(iIndex) || (iIndex >= pDataView->GetElementCount())) {
        return;
    }
    ScrollBar* pVScrollBar = pDataView->GetVScrollBar();
    if (pVScrollBar == nullptr) {
        return;
    }
    if (!bToTop) {
        std::vector<size_t> atTopItemIndexList;
        std::vector<size_t> itemIndexList;
        GetDisplayElements(rc, itemIndexList, &atTopItemIndexList);
        bool bDisplay = std::find(itemIndexList.begin(), itemIndexList.end(), iIndex) != itemIndexList.end();
        bool bFirst = false;
        bool bLast = false;
        if (!itemIndexList.empty()) {
            for (size_t i = 0; i < itemIndexList.size(); ++i) {
                if (std::find(atTopItemIndexList.begin(), 
                              atTopItemIndexList.end(), 
                              itemIndexList[i]) == atTopItemIndexList.end()) {
                    //The first index that is not pinned to top
                    bFirst = itemIndexList[i] == iIndex;
                    break;
                }
            }            
            bLast = itemIndexList[itemIndexList.size() - 1] == iIndex;
        }
        if (bDisplay && !bLast && !bFirst) {
            //Already in the displayed state
            return;
        }
    }
    const UiSize szElementSize = GetElementSize(0, iIndex); //Size of the target element
    int64_t nNewTopPos = 0;     //Position when top-aligned
    int64_t nNewBottomPos = 0;  //Position when bottom-aligned
    if (iIndex > 0) {
        nNewTopPos = GetElementsHeight(iIndex, false);
        if (!bToTop) {
            //Bottom aligned
            nNewBottomPos = GetElementsHeight(iIndex, false);
            int64_t nNewPosWithTop = GetElementsHeight(iIndex, true);
            int64_t nTopHeights = GetHeaderHeight();
            if (nNewPosWithTop > nNewBottomPos) {
                nTopHeights += (nNewPosWithTop - nNewBottomPos);
            }
            //Deduct the height of the pinned-to-top items, the height of the Header, and its own height
            nNewBottomPos -= rc.Height();
            nNewBottomPos += nTopHeights;
            nNewBottomPos += szElementSize.cy;
        }
    }

    if (nNewTopPos < 0) {
        nNewTopPos = 0;
    }
    if (nNewTopPos > pVScrollBar->GetScrollRange()) {
        nNewTopPos = pVScrollBar->GetScrollRange();
    }
    if (nNewBottomPos < 0) {
        nNewBottomPos = 0;
    }
    if (nNewBottomPos > pVScrollBar->GetScrollRange()) {
        nNewBottomPos = pVScrollBar->GetScrollRange();
    }
    ui::UiSize64 scrollPos = pDataView->GetScrollPos();
    int64_t nScrollPosY = scrollPos.cy;
    int64_t nNewPos = nScrollPosY;
    if (bToTop) {
        //Top aligned
        nNewPos = nNewTopPos;
    }
    else {
        //No alignment specified, determine intelligently
        int64_t diffTop = nNewTopPos - nScrollPosY;
        int64_t diffBottom = nNewBottomPos - nScrollPosY;
        bool bFullDisplay = false; //Whether it can already be fully displayed (vertically)
        if ((nScrollPosY >= nNewBottomPos) && (nScrollPosY <= nNewTopPos)) {
            if ((std::abs(diffTop) >= szElementSize.cy) && (std::abs(diffBottom) >= szElementSize.cy)) {
                //Currently fully displayed, no scrolling needed
                bFullDisplay = true;
            }
        }
        if (!bFullDisplay) {
            if (std::abs(diffTop) > std::abs(diffBottom)) {
                //Scroll up: bottom aligned
                nNewPos = nNewBottomPos;
            }
            else {
                //Scroll down: top aligned
                nNewPos = nNewTopPos;
            }
        }
    }
    if (scrollPos.cy != nNewPos) {
        scrollPos.cy = nNewPos;
        pDataView->SetScrollPos(scrollPos);
    }    
}

int64_t ListCtrlReportLayout::GetElementsHeight(size_t nCount, bool bIncludeAtTops) const
{
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView == nullptr) || !pDataView->HasDataProvider()) {
        ASSERT(0);
        return 0;
    }
    if (!Box::IsValidItemIndex(nCount)) {
        nCount = pDataView->GetElementCount();
    }
    if ((nCount == 0) || !Box::IsValidItemIndex(nCount)){
        return 0;
    }
    int64_t nTotalHeight = 0;
    if (!pDataView->IsNormalMode()) {
        //Non-normal mode
        nTotalHeight = pDataView->GetDataItemTotalHeights(nCount, bIncludeAtTops);
    }
    else {
        int32_t nItemHeight = GetItemHeight();
        ASSERT(nItemHeight > 0);
        if (nItemHeight <= 0) {
            return 0;
        }
        if (nCount <= 1) {
            //Only 1 row
            nTotalHeight = (int64_t)nItemHeight + GetChildMarginY();
        }
        else {
            int64_t iChildMargin = 0;
            if (GetChildMarginY() > 0) {
                iChildMargin = GetChildMarginY();
            }
            int64_t childMarginTotal = ((int64_t)nCount - 1) * iChildMargin;
            nTotalHeight = nItemHeight * nCount + childMarginTotal;
        }
    }
    return nTotalHeight;
}

UiSize ListCtrlReportLayout::GetElementSize(int32_t rcWidth, size_t nElementIndex) const
{
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView == nullptr) || !pDataView->HasDataProvider()) {
        ASSERT(0);
        return UiSize();
    }
    UiSize szElementSize;
    szElementSize.cx = std::max(GetItemWidth(), rcWidth);
    if (!pDataView->IsNormalMode()) {
        //Non-normal mode, row heights may differ
        szElementSize.cy = pDataView->GetDataItemHeight(nElementIndex);
    }
    else {
        //Normal mode, all rows have the same height
        szElementSize.cy = GetItemHeight();
    }    
    return szElementSize;
}

int32_t ListCtrlReportLayout::GetItemWidth() const
{
    //The width is the same as the header width
    int32_t nItemWidth = 0;
    ListCtrlReportView* pDataView = GetDataView();
    if (pDataView != nullptr) {
        nItemWidth = pDataView->GetListCtrlWidth();
    }
    return nItemWidth;
}

int32_t ListCtrlReportLayout::GetItemHeight() const
{
    //All rows have the same height, read from the configuration
    int32_t nItemHeight = 0;
    ListCtrlReportView* pDataView = GetDataView();
    if ((pDataView != nullptr) && (pDataView->m_pListCtrl != nullptr)) {
        nItemHeight = pDataView->m_pListCtrl->GetDataItemHeight();
    }
    return nItemHeight;
}

int32_t ListCtrlReportLayout::GetHeaderHeight() const
{
    int32_t nHeaderHeight = 0;
    ListCtrlReportView* pDataView = GetDataView();
    if (pDataView != nullptr) {
        Control* pHeaderCtrl = nullptr;
        size_t nItemCount = pDataView->GetItemCount();
        if (nItemCount > 0) {
            //The first element is the header control, set its position and size
            pHeaderCtrl = pDataView->GetItemAt(0);            
        }
        if ((pHeaderCtrl != nullptr) && pHeaderCtrl->IsVisible()) {
            nHeaderHeight = pHeaderCtrl->GetFixedHeight().GetInt32();
        }
    }
    return nHeaderHeight;
}

int32_t ListCtrlReportLayout::GetReserveHeight() const
{
    int32_t nReserveHeight = 8;
    if (m_pDataView != nullptr) {
        m_pDataView->Dpi().ScaleInt(nReserveHeight);
    }
    return nReserveHeight;
}

}//namespace ui

