#include "duilib/Box/ListBox.h"
#include "duilib/Box/ListBoxHelper.h"
#include "duilib/Core/ScrollBar.h"
#include "duilib/Core/Keyboard.h"
#include "duilib/Layout/VirtualHTileLayout.h"
#include "duilib/Layout/VirtualVTileLayout.h"

namespace ui 
{

//When multi-selecting, whether to show the selection background color: 0 - default rule; 1 - show the background color; 2 - do not show the background color
enum ePaintSelectedColors
{
    PAINT_SELECTED_COLORS_DEFAULT = 0,
    PAINT_SELECTED_COLORS_YES = 1,
    PAINT_SELECTED_COLORS_NO = 2
};

ListBox::ListBox(Window* pWindow, Layout* pLayout) :
    ScrollBox(pWindow, (pLayout != nullptr) ? pLayout : new VLayout),
    m_iCurSel(Box::InvalidIndex),
    m_nLastNoShiftItem(0),
    m_pCompareFunc(nullptr),
    m_pCompareContext(nullptr),
    m_uPaintSelectedColors(PAINT_SELECTED_COLORS_DEFAULT),
    m_bScrollSelect(false),
    m_bSelectNextWhenActiveRemoved(false),
    m_bMultiSelect(false),
    m_bSelectLikeListCtrl(false),
    m_bSelectNoneWhenClickBlank(true)
{
}

ListBox::~ListBox()
{
    if (!IsAutoDestroyChild()) {
        const size_t itemCount = GetItemCount();
        for (size_t i = 0; i < itemCount; ++i) {
            Control* p = GetItemAt(i);
            OnListBoxItemRemoved(p);
        }
    }
}

DString ListBox::GetType() const { return _T("ListBox"); }

void ListBox::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("multi_select")) {
        SetMultiSelect(strValue == _T("true"));
    }
    else if (strName == _T("paint_selected_colors")) {
        if (strValue == _T("true")) {
            m_uPaintSelectedColors = PAINT_SELECTED_COLORS_YES;
        }
        else {
            m_uPaintSelectedColors = PAINT_SELECTED_COLORS_NO;
        }
    }
    else if ((strName == _T("scroll_select")) || (strName == _T("scrollselect"))) {
        SetScrollSelect(strValue == _T("true"));
    }
    else if (strName == _T("select_next_when_active_removed")) {
        SetSelectNextWhenActiveRemoved(strValue == _T("true"));
    }
    else if (strName == _T("frame_selection")) {
        SetEnableFrameSelection(strValue == _T("true"));
    }
    else if (strName == _T("frame_selection_color")) {
        SetFrameSelectionColor(strValue);
    }
    else if (strName == _T("frame_selection_alpha")) {
        SetframeSelectionAlpha((uint8_t)StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("frame_selection_border_size")) {
        SetFrameSelectionBorderSize(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("frame_selection_border_color")) {
        SetFrameSelectionBorderColor(strValue);
    }
    else if (strName == _T("select_none_when_click_blank")) {
        SetSelectNoneWhenClickBlank(strValue == _T("true"));
    }
    else if (strName == _T("select_like_list_ctrl")) {
        SetSelectLikeListCtrl(strValue == _T("true"));
    }
    else {
        ScrollBox::SetAttribute(strName, strValue);
    }
}

void ListBox::SetEnableFrameSelection(bool bEnable)
{
    if (m_pHelper == nullptr) {
        m_pHelper = std::make_unique<ListBoxHelper>(this);
    }
    m_pHelper->SetEnableFrameSelection(bEnable);
}

bool ListBox::IsEnableFrameSelection() const
{
    if (m_pHelper != nullptr) {
        return m_pHelper->IsEnableFrameSelection();
    }
    return false;
}

void ListBox::SetFrameSelectionColor(const DString& frameSelectionColor)
{
    if (m_pHelper == nullptr) {
        m_pHelper = std::make_unique<ListBoxHelper>(this);
    }
    m_pHelper->SetFrameSelectionColor(frameSelectionColor);
}

DString ListBox::GetFrameSelectionColor() const
{
    if (m_pHelper != nullptr) {
        return m_pHelper->GetFrameSelectionColor();
    }
    return DString();
}

void ListBox::SetframeSelectionAlpha(uint8_t frameSelectionAlpha)
{
    if (m_pHelper == nullptr) {
        m_pHelper = std::make_unique<ListBoxHelper>(this);
    }
    m_pHelper->SetframeSelectionAlpha(frameSelectionAlpha);
}

uint8_t ListBox::GetFrameSelectionAlpha() const
{
    if (m_pHelper != nullptr) {
        return m_pHelper->GetFrameSelectionAlpha();
    }
    return 255;
}

void ListBox::SetFrameSelectionBorderColor(const DString& frameSelectionBorderColor)
{
    if (m_pHelper == nullptr) {
        m_pHelper = std::make_unique<ListBoxHelper>(this);
    }
    m_pHelper->SetFrameSelectionBorderColor(frameSelectionBorderColor);
}

DString ListBox::GetFrameSelectionBorderColor() const
{
    if (m_pHelper != nullptr) {
        return m_pHelper->GetFrameSelectionBorderColor();
    }
    return DString();
}

void ListBox::SetFrameSelectionBorderSize(int32_t nBorderSize)
{
    if (m_pHelper == nullptr) {
        m_pHelper = std::make_unique<ListBoxHelper>(this);
    }
    m_pHelper->SetFrameSelectionBorderSize(nBorderSize);
}

int32_t ListBox::GetFrameSelectionBorderSize() const
{
    if (m_pHelper != nullptr) {
        return m_pHelper->GetFrameSelectionBorderSize();
    }
    return 0;
}

void ListBox::SetNormalItemTop(int32_t nNormalItemTop)
{
    if (m_pHelper == nullptr) {
        m_pHelper = std::make_unique<ListBoxHelper>(this);
    }
    m_pHelper->SetNormalItemTop(nNormalItemTop);
}

int32_t ListBox::GetNormalItemTop() const
{
    if (m_pHelper != nullptr) {
        return m_pHelper->GetNormalItemTop();
    }
    return -1;
}

void ListBox::SetSelectNoneWhenClickBlank(bool bSelectNoneWhenClickBlank)
{
    m_bSelectNoneWhenClickBlank = bSelectNoneWhenClickBlank;
}

bool ListBox::IsSelectNoneWhenClickBlank() const
{
    return m_bSelectNoneWhenClickBlank;
}

void ListBox::SetSelectLikeListCtrl(bool bSelectLikeListCtrl)
{
    m_bSelectLikeListCtrl = bSelectLikeListCtrl;
}

bool ListBox::IsSelectLikeListCtrl() const
{
    return m_bSelectLikeListCtrl;
}

void ListBox::HandleEvent(const EventArgs& msg)
{
    if (IsDisabledEvents(msg)) {
        //If it is a mouse or keyboard message and the control is Disabled, forward it to the parent control
        Box* pParent = GetParent();
        if (pParent != nullptr) {
            pParent->SendEventMsg(msg);
        }
        else {
            BaseClass::HandleEvent(msg);
        }
        return;
    }
    bool bHandled = false;
    if (msg.eventType == kEventKeyDown) {
        if (IsSelectLikeListCtrl()) {
            //Use the ListCtrl-style shortcut logic first
            bHandled = OnListCtrlKeyDown(msg);
        }        
        if (!bHandled) {
            bHandled = OnListBoxKeyDown(msg);
        }        
    }
    else if (msg.eventType == kEventMouseWheel) {
        bHandled = OnListBoxMouseWheel(msg);
    }    
    if(!bHandled) {
        BaseClass::HandleEvent(msg);
    }
}

bool ListBox::OnListBoxKeyDown(const EventArgs& msg)
{
    ASSERT(msg.eventType == kEventKeyDown);
    bool bHandled = false;
    bool bArrowKeyDown = (msg.eventType == kEventKeyDown) &&
                          ((msg.vkCode == kVK_UP)    || (msg.vkCode == kVK_DOWN) ||
                           (msg.vkCode == kVK_LEFT)  || (msg.vkCode == kVK_RIGHT) ||
                           (msg.vkCode == kVK_PRIOR) || (msg.vkCode == kVK_NEXT) ||
                           (msg.vkCode == kVK_HOME)  || (msg.vkCode == kVK_END));
    if (!bArrowKeyDown) {
        return bHandled;
    }

    bool bHasSelectItem = GetCurSel() < GetItemCount(); //Whether there is a single-selection item
    if (!IsMultiSelect() && (GetItemCount() > 0) && !bHasSelectItem) {
        //No selected item in the current UI; need to check whether a subclass (virtual list implementation) has a selected item
        size_t nDestItemIndex = Box::InvalidIndex;
        if (OnFindSelectable(GetCurSel(), SelectableMode::kSelect, 1, nDestItemIndex)) {
            bHasSelectItem = true;
            ASSERT(GetCurSel() == nDestItemIndex);
        }
    }
    if (IsMultiSelect() || (GetItemCount() == 0) || !bHasSelectItem) {
        //When there is no data, multi-selection is supported, or there is no selected item, the single-selection shortcut key logic is not supported, but the HOME and END key responses (scrolling) are supported
        if (msg.vkCode == kVK_HOME) {
            if (IsHorizontalScrollBar()) {
                HomeLeft();
            }
            else {
                HomeUp();
            }
            bHandled = true;
        }
        else if (msg.vkCode == kVK_END) {
            if (IsHorizontalScrollBar()) {
                EndRight();
            }
            else {
                EndDown(false);
            }
            bHandled = true;
        }
        return bHandled;
    }

    //The case of single selection, with data and a selected item
    bHandled = true;
    switch (msg.vkCode) {
    case kVK_UP:
        if (IsHorizontalScrollBar()) {
            //Horizontal scroll bar: up 1 item
            SelectItemPrevious(true, true);
        }
        else {
            //Not a horizontal scroll bar: up 1 row
            size_t nColumns = 0;
            size_t nRows = 0;
            GetDisplayItemCount(false, nColumns, nRows);
            if ((m_iCurSel < GetItemCount()) && IsSelectableItem(m_iCurSel) &&
                (GetItemCountBefore(m_iCurSel) >= nColumns)) {
                //Can scroll up 1 row
                SelectItemCountN(true, true, false, nColumns);
            }
        }        
        break;
    case kVK_DOWN:
        if (IsHorizontalScrollBar()) {
            //Horizontal scroll bar: down 1 item
            SelectItemNext(true, true);
        }
        else {
            //Not a horizontal scroll bar: down 1 row
            size_t nColumns = 0;
            size_t nRows = 0;
            GetDisplayItemCount(false, nColumns, nRows);
            if ((m_iCurSel < GetItemCount()) && IsSelectableItem(m_iCurSel) &&
                (GetItemCountAfter(m_iCurSel) >= nColumns)) {
                SelectItemCountN(true, true, true, nColumns);
            }
            else {
                PageDown();
                SelectItem(m_iCurSel, true, false);
            }
        }
        break;
    case kVK_LEFT:
        if (IsHorizontalScrollBar()) {
            //Horizontal scroll bar: up 1 column
            size_t nColumns = 0;
            size_t nRows = 0;
            GetDisplayItemCount(false, nColumns, nRows);
            if ((m_iCurSel < GetItemCount()) && IsSelectableItem(m_iCurSel) &&
                (GetItemCountBefore(m_iCurSel) >= nRows)) {
                SelectItemCountN(true, true, false, nRows);
            }
        }
        else {
            //Not a horizontal scroll bar: up 1 item
            SelectItemPrevious(true, true);
        }
        break;
    case kVK_RIGHT:
        if (IsHorizontalScrollBar()) {
            //Horizontal scroll bar: down 1 row
            size_t nColumns = 0;
            size_t nRows = 0;
            GetDisplayItemCount(false, nColumns, nRows);
            if ((m_iCurSel < GetItemCount()) && IsSelectableItem(m_iCurSel) &&
                (GetItemCountAfter(m_iCurSel) >= nRows)) {
                SelectItemCountN(true, true, true, nRows);
            }
            else {
                PageRight();
                SelectItem(m_iCurSel, true, false);
            }
        }
        else {
            //Not a horizontal scroll bar: down 1 item
            SelectItemNext(true, true);
        }
        break;
    case kVK_PRIOR:
        SelectItemPage(true, true, false, 0);
        break;
    case kVK_NEXT:
        SelectItemPage(true, true, true, 0);
        break;
    case kVK_HOME:
        SelectItemHome(true, true);
        break;
    case kVK_END:
        SelectItemEnd(true, true);
        break;
    default:
        bHandled = false;
        break;
    }
    if (!IsKeyDown(msg, ModifierKey::kShift)) {
        SetLastNoShiftItem(GetCurSel());
    }
    return bHandled;
}

int32_t ListBox::CalcRows() const
{
    HLayout* pHLayout = dynamic_cast<HLayout*>(GetLayout());
    if (pHLayout != nullptr) {
        //This layout is fixed at 1 row
        return 1;
    }
    HTileLayout* pHTileLayout = dynamic_cast<HTileLayout*>(GetLayout());
    return CalcHTileRows(pHTileLayout);
}

int32_t ListBox::CalcColumns() const
{
    VLayout* pVLayout = dynamic_cast<VLayout*>(GetLayout());
    if (pVLayout != nullptr) {
        //This layout is fixed at 1 column
        return 1;
    }
    VTileLayout* pVTileLayout = dynamic_cast<VTileLayout*>(GetLayout());
    return CalcVTileColumns(pVTileLayout);
}

bool ListBox::OnListCtrlKeyDown(const EventArgs& msg)
{
    //This function only implements the shortcut key logic for the non-virtual-list case; the corresponding logic for virtual list mode is implemented in the subclass
    ASSERT(msg.eventType == kEventKeyDown);
    bool bHandled = false;
    bool bCtrlADown = (msg.eventType == kEventKeyDown) && ((msg.vkCode == _T('A')) || (msg.vkCode == _T('a')));
    if (bCtrlADown) {
        //Ctrl + A select all operation
        bHandled = true;
        bool bRet = SetSelectAll();
        if (bRet) {
            OnSelectStatusChanged();
            SendEvent(kEventSelChanged);//bRet returns true to indicate there were changes
        }
        return bHandled;
    }

    //Arrow key operations
    bool bArrowKeyDown = (msg.eventType == kEventKeyDown) &&
                         ((msg.vkCode == kVK_UP) || (msg.vkCode == kVK_DOWN) ||
                          (msg.vkCode == kVK_LEFT) || (msg.vkCode == kVK_RIGHT) ||
                          (msg.vkCode == kVK_PRIOR) || (msg.vkCode == kVK_NEXT) ||
                          (msg.vkCode == kVK_HOME) || (msg.vkCode == kVK_END));
    const size_t nItemCount = GetItemCount();
    if (!bArrowKeyDown || !IsMultiSelect() || (nItemCount == 0)) {
        //When there is no arrow-key-down message, no data, or no multi-selection support, use the default handling flow
        return bHandled;
    }

    bool bShiftDown = Keyboard::IsKeyDown(kVK_SHIFT);
    bool bControlDown = Keyboard::IsKeyDown(kVK_CONTROL);
    bool bAltDown = Keyboard::IsKeyDown(kVK_MENU);

    if (bAltDown || bControlDown) {
        //If the Ctrl key or Alt key is held down, use the default flow
        return bHandled;
    }

    // The following flow handles arrow key operations
    // Handle arrow key operations in multi-selection mode; the flow is basically similar to single-selection mode, but GetCurSel() may not be correct in multi-selection mode and needs calibration
   // size_t nCurSel = GetCurSel();
    const bool bForward = (msg.vkCode == kVK_DOWN) || (msg.vkCode == kVK_RIGHT) ||
                          (msg.vkCode == kVK_NEXT) || (msg.vkCode == kVK_END);
    size_t nIndexCurSel = Box::InvalidIndex;
    if (bForward) {
        //Find the last selectable item in the selected region of the current view as the starting point
        for (int32_t index = (int32_t)nItemCount - 1; index >= 0; --index) {
            Control* pControl = GetItemAt(index);
            if ((pControl == nullptr) || !pControl->IsVisible() || !pControl->IsSelectableType()) {
                continue;
            }
            IListBoxItem* pItem = dynamic_cast<IListBoxItem*>(pControl);
            if ((pItem != nullptr) && pItem->IsSelected()) {
                nIndexCurSel = index;
                break;
            }
        }
    }
    else {
        //Find the first selectable item in the selected region of the current view as the starting point
        for (size_t index = 0; index < nItemCount; ++index) {
            Control* pControl = GetItemAt(index);
            if ((pControl == nullptr) || !pControl->IsVisible() || !pControl->IsSelectableType()) {
                continue;
            }
            IListBoxItem* pItem = dynamic_cast<IListBoxItem*>(pControl);
            if ((pItem != nullptr) && pItem->IsSelected()) {
                nIndexCurSel = index;
                break;
            }
        }
    }
    if (nIndexCurSel >= nItemCount) {
        if (bForward) {
            nIndexCurSel = 0;
        }
        else {
            nIndexCurSel = nItemCount - 1;
        }
    }
    
    if (nIndexCurSel < nItemCount) {
        //Match the selectable item
        nIndexCurSel = FindSelectable(nIndexCurSel, bForward);
    }
    if (nIndexCurSel >= nItemCount) {
        //No valid data selection item
        return bHandled;
    }

    const int32_t nRows = CalcRows();
    const int32_t nColumns = CalcColumns();

    size_t nIndexEnsureVisible = Box::InvalidIndex; //The element that needs to be kept visible
    size_t nIndexEnd = Box::InvalidIndex;
    //Implement the Shift key + arrow key selection logic
    switch (msg.vkCode) {
    case kVK_UP:
        if (IsHorizontalLayout()) {
            //Horizontal layout
            if (nIndexCurSel >= 1) {
                nIndexEnd = nIndexCurSel - 1;
            }
            else {
                nIndexEnsureVisible = 0;
            }
        }
        else {
            //Vertical layout
            if ((int32_t)nIndexCurSel >= nColumns) {
                nIndexEnd = nIndexCurSel - nColumns;
            }
            else {
                nIndexEnsureVisible = 0;
            }
        }
        break;
    case kVK_DOWN:
        if (IsHorizontalLayout()) {
            //Horizontal layout
            if ((nIndexCurSel + 1) < nItemCount) {
                nIndexEnd = nIndexCurSel + 1;
            }
            else {
                nIndexEnsureVisible = nItemCount - 1;
            }
        }
        else {
            //Vertical layout
            if ((nIndexCurSel + nColumns) < nItemCount) {
                nIndexEnd = nIndexCurSel + nColumns;
            }
            else {
                nIndexEnsureVisible = nItemCount - 1;
            }
        }
        break;
    case kVK_LEFT:
        if (IsHorizontalLayout()) {
            //Horizontal layout
            if ((int32_t)nIndexCurSel >= nRows) {
                nIndexEnd = nIndexCurSel - nRows;
            }
            else {
                nIndexEnsureVisible = 0;
            }
        }
        else {
            //Vertical layout
            if (nColumns <= 1) {
                //When there is only 1 column, handle as scrolling
                LineLeft();
                bHandled = true;
            }
            else {
                if (nIndexCurSel >= 1) {
                    nIndexEnd = nIndexCurSel - 1;
                }
                else {
                    nIndexEnsureVisible = 0;
                }
            }
        }
        break;
    case kVK_RIGHT:
        if (IsHorizontalLayout()) {
            //Horizontal layout
            if ((nIndexCurSel + nRows) < nItemCount) {
                nIndexEnd = nIndexCurSel + nRows;
            }
            else {
                nIndexEnsureVisible = nItemCount - 1;
            }
        }
        else {
            if (nColumns <= 1) {
                //When there is only 1 column, handle as scrolling
                LineRight();
                bHandled = true;
            }
            else {
                //Vertical layout
                if ((nIndexCurSel + 1) < nItemCount) {
                    nIndexEnd = nIndexCurSel + 1;
                }
                else {
                    nIndexEnsureVisible = nItemCount - 1;
                }
            }
        }
        break;
    case kVK_PRIOR:
    {
        size_t nShowColumns = 0;
        size_t nShowRows = 0;
        if (IsHorizontalLayout()) {
            //Horizontal layout
            GetDisplayItemCount(true, nShowColumns, nShowRows);
        }
        else {
            //Vertical layout            
            GetDisplayItemCount(false, nShowColumns, nShowRows);
        }
        size_t nScrollCount = nShowColumns * nShowRows;
        if (nIndexCurSel >= nScrollCount) {
            nIndexEnd = nIndexCurSel - nScrollCount;
        }
        else {
            if (IsHorizontalLayout()) {
                for (int32_t nColumn = (int32_t)nShowColumns - 1; nColumn >= 0; --nColumn) {
                    nScrollCount = (size_t)nColumn * nShowRows;
                    if (nIndexCurSel >= nScrollCount) {
                        //Jump to the first column, at the position of the same row
                        nIndexEnd = nIndexCurSel - nScrollCount;
                        break;
                    }
                }
            }
            else {
                for (int32_t nRow = (int32_t)nShowRows - 1; nRow >= 0; --nRow) {
                    nScrollCount = nShowColumns * (size_t)nRow;
                    if (nIndexCurSel >= nScrollCount) {
                        //Jump to the first row, at the position of the same column
                        nIndexEnd = nIndexCurSel - nScrollCount;
                        break;
                    }
                }
            }
        }
    }
    break;
    case kVK_NEXT:
    {
        size_t nShowColumns = 0;
        size_t nShowRows = 0;
        if (IsHorizontalLayout()) {
            //Horizontal layout
            GetDisplayItemCount(true, nShowColumns, nShowRows);
        }
        else {
            //Vertical layout            
            GetDisplayItemCount(false, nShowColumns, nShowRows);
        }
        size_t nScrollCount = nShowColumns * nShowRows;
        if ((nIndexCurSel + nScrollCount) < nItemCount) {
            nIndexEnd = nIndexCurSel + nScrollCount;
        }
        else {
            if (IsHorizontalLayout()) {
                for (int32_t nColumn = (int32_t)nShowColumns - 1; nColumn >= 0; --nColumn) {
                    nScrollCount = (size_t)nColumn * nShowRows;
                    if ((nIndexCurSel + nScrollCount) < nItemCount) {
                        //Jump to the last column, at the position of the same row
                        nIndexEnd = nIndexCurSel + nScrollCount;
                        nIndexEnsureVisible = nItemCount - 1;
                        break;
                    }
                }
            }
            else {
                for (int32_t nRow = (int32_t)nShowRows - 1; nRow >= 0; --nRow) {
                    nScrollCount = nShowColumns * (size_t)nRow;
                    if ((nIndexCurSel + nScrollCount) < nItemCount) {
                        //Jump to the last row, at the position of the same column
                        nIndexEnd = nIndexCurSel + nScrollCount;
                        nIndexEnsureVisible = nItemCount - 1;
                        break;
                    }
                }
            }
        }
    }
    break;
    case kVK_HOME:
        nIndexEnd = 0;
        break;
    case kVK_END:
        nIndexEnd = nItemCount - 1;
        break;
    default:
        break;
    }

    if (nIndexEnd >= nItemCount) {
        if (nIndexEnsureVisible != Box::InvalidIndex) {
            EnsureVisible(nIndexEnsureVisible);
        }
        return bHandled;
    }

    //Match the selectable item
    nIndexEnd = FindSelectable(nIndexEnd, bForward);
    if (nIndexEnd >= nItemCount) {
        return bHandled;
    }

    bHandled = true;
    std::set<size_t> selectedIndexs; //The list of items to be selected
    if (bShiftDown) {
        //Shift key held down: select all data in the range
        size_t nLastNoShiftItem = GetLastNoShiftItem();//The starting element index
        if (nLastNoShiftItem >= nItemCount) {
            nLastNoShiftItem = 0;
        }
        size_t nStartItemIndex = std::min(nLastNoShiftItem, nIndexEnd);
        size_t nEndItemIndex = std::max(nLastNoShiftItem, nIndexEnd);
        for (size_t i = nStartItemIndex; i <= nEndItemIndex; ++i) {
            if (IsSelectableItem(i)) {
                selectedIndexs.insert(i);
            }
        }
    }
    else {
        //Shift key not held down: only select the last data item
        selectedIndexs.insert(nIndexEnd);
    }

    //Select all elements in this range
    for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
        Control* pControl = GetItemAt(nItemIndex);
        if ((pControl == nullptr) || !pControl->IsVisible() || !pControl->IsEnabled()) {
            continue;
        }
        IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
        if (pListItem == nullptr) {
            continue;
        }
        if (selectedIndexs.find(nItemIndex) != selectedIndexs.end()) {
            //Selected set
            if (!pListItem->IsSelected()) {
                //If it was previously not selected, update it to selected
                pListItem->OptionSelected(true, false);
            }
        }
        else {
            //Unselect the others
            if (pListItem->IsSelected()) {
                //If it was previously selected, update it to not selected
                pListItem->OptionSelected(false, false);
            }
        }
    }
    if (nIndexEnsureVisible != Box::InvalidIndex) {
        EnsureVisible(nIndexEnsureVisible);
    }
    else {
        EnsureVisible(nIndexEnd);
    }
    size_t nCurSel = nIndexEnd;
    ASSERT(nCurSel < GetItemCount());
    bool bTriggerEvent = false;
    if (nCurSel < GetItemCount()) {
        SetCurSel(nCurSel);
        SelectItemSingle(nCurSel, true, false);
        if (!bShiftDown) {
            SetLastNoShiftItem(nCurSel);
        }
        bTriggerEvent = true;
        ASSERT(GetItemAt(nCurSel)->IsFocused());
    }
    OnSelectStatusChanged();
    if (bTriggerEvent) {
        SendEvent(kEventSelect, nCurSel, Box::InvalidIndex);
    }
    return bHandled;
}

bool ListBox::OnListBoxMouseWheel(const EventArgs& msg)
{
    ASSERT(msg.eventType == kEventMouseWheel);
    bool bHandled = false;
    if (m_bScrollSelect && (msg.eventType == kEventMouseWheel)) {
        int32_t deltaValue = msg.eventData;
        if (deltaValue != 0) {
            bool bForward = deltaValue > 0 ? false : true;
            SelectItemPage(true, true, bForward, std::abs(deltaValue));
            bHandled = true;
        }
    }
    return bHandled;
}

bool ListBox::SelectItem(size_t iIndex)
{
    return SelectItem(iIndex, true, true);
}

size_t ListBox::SelectItemPrevious(bool bTakeFocus, bool bTriggerEvent)
{
    return SelectItemCountN(bTakeFocus, bTriggerEvent, false, 1);
}

size_t ListBox::SelectItemNext(bool bTakeFocus, bool bTriggerEvent)
{
    return SelectItemCountN(bTakeFocus, bTriggerEvent, true, 1);
}

size_t ListBox::SelectItemPageUp(bool bTakeFocus, bool bTriggerEvent)
{
    return SelectItemPage(bTakeFocus, bTriggerEvent, false, 0);
}

size_t ListBox::SelectItemPageDown(bool bTakeFocus, bool bTriggerEvent)
{
    return SelectItemPage(bTakeFocus, bTriggerEvent, true, 0);
}

size_t ListBox::SelectItemHome(bool bTakeFocus, bool bTriggerEvent)
{
    if (GetItemCount() == 0) {
        return Box::InvalidIndex;
    }
    size_t iIndex = 0;
    size_t nDestItemIndex = Box::InvalidIndex;
    if (OnFindSelectable(m_iCurSel, SelectableMode::kHome, 1, nDestItemIndex)) {
        iIndex = nDestItemIndex;
    }
    size_t itemIndex = FindSelectable(iIndex, true);
    if (Box::IsValidItemIndex(itemIndex)) {        
        SelectItem(itemIndex, false, bTriggerEvent);
        itemIndex = SelectEnsureVisible(itemIndex, bTakeFocus);
        if (bTriggerEvent) {
            SendEvent(kEventSelect, itemIndex, Box::InvalidIndex);
        }
    }
    return itemIndex;
}

size_t ListBox::SelectItemEnd(bool bTakeFocus, bool bTriggerEvent)
{
    if (GetItemCount() == 0) {
        return Box::InvalidIndex;
    }
    size_t iIndex = GetItemCount() - 1;
    size_t nDestItemIndex = Box::InvalidIndex;
    if (OnFindSelectable(m_iCurSel, SelectableMode::kEnd, 1, nDestItemIndex)) {
        iIndex = nDestItemIndex;
    }
    size_t itemIndex = FindSelectable(iIndex, false);
    if (Box::IsValidItemIndex(itemIndex)) {
        SelectItem(itemIndex, false, bTriggerEvent);
        itemIndex = SelectEnsureVisible(itemIndex, bTakeFocus);
        if (bTriggerEvent) {
            SendEvent(kEventSelect, itemIndex, Box::InvalidIndex);
        }
    }
    return itemIndex;
}

size_t ListBox::SelectItemPage(bool bTakeFocus, bool bTriggerEvent, bool bForward, int32_t nDeltaValue)
{
    //The page-turning logic for the Page Up / Page Down keys
    size_t itemIndex = Box::InvalidIndex;
    const size_t itemCount = GetItemCount();
    if (itemCount == 0) {
        return itemIndex;
    }

    bool bIsHorizontal = IsHorizontalScrollBar(); //Whether it is a horizontal scroll bar
    if (nDeltaValue == 0) {
        //Calculate the nDeltaValue value
        nDeltaValue = bIsHorizontal ? GetRect().Width() : GetRect().Height();
    }
    if (nDeltaValue == 0) {
        if (m_iCurSel < itemCount) {
            EnsureVisible(m_iCurSel);
        }
        return itemIndex;
    }
    size_t nCountPerPage = 1;
    size_t nColumns = 0;
    size_t nRows = 0;
    size_t nTotalDisplayCount = GetDisplayItemCount(bIsHorizontal, nColumns, nRows);
    if (nTotalDisplayCount < 1) {
        nTotalDisplayCount = 1;
    }
    if (bIsHorizontal) {
        //Only a horizontal scroll bar exists, handle as horizontal
        nCountPerPage = nTotalDisplayCount * std::abs(nDeltaValue) / GetRect().Width();
        if (nCountPerPage > nRows) {
            nCountPerPage -= nRows; //Subtract 1 row
        }
        if ((m_iCurSel < GetItemCount()) && IsSelectableItem(m_iCurSel)) {
            //If page turning cannot be satisfied, stay on the last column
            while (nCountPerPage > nRows) {
                if (bForward) {
                    if (GetItemCountAfter(m_iCurSel) >= nCountPerPage) {
                        break;
                    }
                }
                else {
                    if (GetItemCountBefore(m_iCurSel) >= nCountPerPage) {
                        break;
                    }
                }
                nCountPerPage -= nRows;
            }
        }
    }
    else {
        //Otherwise, handle as vertical scrolling
        nCountPerPage = nTotalDisplayCount * std::abs(nDeltaValue) / GetRect().Height();
        if (nCountPerPage > nColumns) {
            nCountPerPage -= nColumns; //Subtract 1 column
        }
        if ((m_iCurSel < GetItemCount()) && IsSelectableItem(m_iCurSel)) {
            //If page turning cannot be satisfied, stay on the last row
            while (nCountPerPage > nColumns) {
                if (bForward) {
                    if (GetItemCountAfter(m_iCurSel) >= nCountPerPage) {
                        break;
                    }
                }
                else {
                    if (GetItemCountBefore(m_iCurSel) >= nCountPerPage) {
                        break;
                    }
                }
                nCountPerPage -= nColumns;
            }
        }
    }
    if (nCountPerPage < 1) {
        nCountPerPage = 1;
    }
    
    itemIndex = Box::InvalidIndex;
    if ((m_iCurSel < GetItemCount()) && IsSelectableItem(m_iCurSel)) {
        if (bForward) {
            if (GetItemCountAfter(m_iCurSel) >= nCountPerPage) {
                itemIndex = SelectItemCountN(bTakeFocus, bTriggerEvent, bForward, nCountPerPage);
            }
        }
        else {
            if (GetItemCountBefore(m_iCurSel) >= nCountPerPage) {
                itemIndex = SelectItemCountN(bTakeFocus, bTriggerEvent, bForward, nCountPerPage);
            }
        }
    }
    if (itemIndex == Box::InvalidIndex) {
        if (IsHorizontalScrollBar()) {
            if (bForward) {
                PageRight();
            }
            else {
                PageLeft();
            }            
        }
        else {
            if (bForward) {
                PageDown();
            }
            else {
                PageUp();
            }
        }
        SelectItem(m_iCurSel, true, false);
    }
    return itemIndex;
}

size_t ListBox::SelectItemCountN(bool bTakeFocus, bool bTriggerEvent, bool bForward, size_t nCount)
{
    if (m_iCurSel >= GetItemCount()) {
        //No valid selection item currently, cannot operate
        return Box::InvalidIndex;
    }
    if (!IsSelectableItem(m_iCurSel)) {
        //If the currently selected item is not selectable, cannot operate
        return Box::InvalidIndex;
    }
    if ((nCount == 0) || (nCount == Box::InvalidIndex)){
        nCount = 1;
    }
    bool bExceedFirst = false; //Already reached the first item
    size_t iIndex = Box::InvalidIndex;
    if (!bForward) {
        //Page Up
        if (m_iCurSel > nCount) {
            iIndex = m_iCurSel - nCount;
        }
        else {
            bExceedFirst = true;
        }
    }
    else {
        //Page Down
        iIndex = m_iCurSel + nCount;
    }
    if (IsSelectableItem(iIndex)) {
        //The target child item exists, return directly
        size_t itemIndex = iIndex;
        if (itemIndex < GetItemCount()) {
            SelectItem(itemIndex, false, bTriggerEvent);
            itemIndex = SelectEnsureVisible(itemIndex, bTakeFocus);
            if (bTriggerEvent) {
                SendEvent(kEventSelect, itemIndex, Box::InvalidIndex);
            }
        }
        return itemIndex;
    }

    //Data may need to be preloaded; if preloading occurs, the value of m_iCurSel may change
    size_t nDestItemIndex = Box::InvalidIndex;
    SelectableMode mode = bForward ? SelectableMode::kForward : SelectableMode::kBackward;
    if (OnFindSelectable(m_iCurSel, mode, nCount, nDestItemIndex)) {
        iIndex = nDestItemIndex;
        ASSERT(iIndex < GetItemCount());
        if (iIndex >= GetItemCount()) {
            return Box::InvalidIndex;
        }
    }
    else {
        if (bExceedFirst) {
            iIndex = 0;
        }
    }
    const size_t itemCount = GetItemCount();
    if (iIndex >= itemCount) {
        iIndex = itemCount - 1;
    }    
    size_t itemIndex = FindSelectable(iIndex, bForward);
    if (itemIndex < itemCount) {        
        SelectItem(itemIndex, false, bTriggerEvent);
        itemIndex = SelectEnsureVisible(itemIndex, bTakeFocus);
        if (bTriggerEvent) {
            SendEvent(kEventSelect, itemIndex, Box::InvalidIndex);
        }
    }
    return itemIndex;
}

size_t ListBox::SelectEnsureVisible(size_t itemIndex, bool bTakeFocus)
{
    itemIndex = EnsureVisible(itemIndex);
    if (bTakeFocus) {
        Control* pSelectedControl = GetItemAt(itemIndex);
        if ((pSelectedControl != nullptr) && pSelectedControl->IsVisible()) {
            pSelectedControl->SetFocus();
        }
        ASSERT(pSelectedControl != nullptr);
        ASSERT(pSelectedControl->IsVisible());
        ASSERT(GetWindow()->GetFocusControl() == pSelectedControl);
    }
    return itemIndex;
}

bool ListBox::IsSelectableItem(size_t itemIndex) const
{
    bool bSelectable = false;
    Control* pControl = GetItemAt(itemIndex);
    if ((pControl != nullptr) &&
        pControl->IsSelectableType() &&
        pControl->IsVisible() &&
        pControl->IsEnabled()) {
        bSelectable = true;
    }
    return bSelectable;
}

bool ListBox::IsItemSelected(size_t nIndex) const
{
    bool bSelected = false;
    Control* pControl = GetItemAt(nIndex);
    if (pControl != nullptr) {
        IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
        if ((pListItem != nullptr) && pListItem->IsSelected()) {
            bSelected = true;
        }
    }    
    return bSelected;
}

size_t ListBox::GetDisplayItemCount(bool bIsHorizontal, size_t& nColumns, size_t& nRows) const
{
    nColumns = 1;
    nRows = 1;
    size_t nCount = 1;
    bool bRet = false;
    HTileLayout* pHTileLayout = dynamic_cast<HTileLayout*>(GetLayout());
    if ((pHTileLayout != nullptr) && pHTileLayout->IsFreeLayout()) {
        pHTileLayout = nullptr;
    }
    VTileLayout* pVTileLayout = dynamic_cast<VTileLayout*>(GetLayout());
    if ((pVTileLayout != nullptr) && pVTileLayout->IsFreeLayout()) {
        pVTileLayout = nullptr;
    }
    if (pHTileLayout != nullptr) {
        nRows = CalcHTileRows(pHTileLayout);
        nColumns = CalcHTileColumns(pHTileLayout);
        nCount = nColumns * nRows;
        bRet = true;
    }
    else if (pVTileLayout != nullptr) {        
        nRows = CalcVTileRows(pVTileLayout);
        nColumns = CalcVTileColumns(pVTileLayout);
        nCount = nColumns * nRows;
        bRet = true;
    }
    if(!bRet) {
        std::map<int32_t, int32_t> rows;
        std::map<int32_t, int32_t> columns;        
        UiRect boxRect = GetRect();
        const size_t nItemCount = GetItemCount();
        for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
            Control* pControl = GetItemAt(nItemIndex);
            if ((pControl == nullptr) || !pControl->IsVisible() || pControl->IsFloat()) {
                continue;
            }

            bool bDisplayItem = false;
            const UiRect& rc = pControl->GetRect();
            if (bIsHorizontal) {
                if ((rc.left >= boxRect.left) &&
                    (rc.right <= boxRect.right)) {
                    if ((rc.top >= boxRect.top) && (rc.top < boxRect.bottom)) {
                        bDisplayItem = true;
                    }
                    else if ((rc.bottom >= boxRect.top) && (rc.top < boxRect.bottom)) {
                        bDisplayItem = true;
                    }
                }
            }
            else {
                if ((rc.top >= boxRect.top) &&
                    (rc.bottom <= boxRect.bottom)) {
                    if ((rc.left >= boxRect.left) && (rc.left < boxRect.right)) {
                        bDisplayItem = true;
                    }
                    else if ((rc.right >= boxRect.left) && (rc.right < boxRect.right)) {
                        bDisplayItem = true;
                    }
                }
            }
            if (bDisplayItem) {
                rows[pControl->GetRect().top] = 0;
                columns[pControl->GetRect().left] = 0;
                ++nCount;
            }
        }
        nColumns = columns.size();
        nRows = rows.size();
        if (nCount >= (nRows * nColumns)) {
            nCount = nRows * nColumns;
        }
    }
    return nCount;
}

int32_t ListBox::CalcHTileRows(HTileLayout* pHTileLayout) const
{
    int32_t nRows = 1;
    if (pHTileLayout == nullptr) {
        return nRows;
    }
    nRows = pHTileLayout->GetRows();
    bool bAutoRows = pHTileLayout->IsAutoCalcRows();
    if (bAutoRows) {
        nRows = 0;
    }
    if (nRows <= 0) {
        UiSize szItem = pHTileLayout->GetItemSize();
        if (szItem.cy <= 0) {
            return nRows;
        }
        int32_t childMarginY = pHTileLayout->GetChildMarginY();
        if (childMarginY < 0) {
            childMarginY = 0;
        }

        UiRect rc = GetRect();
        rc.Deflate(GetControlPadding());
        int32_t totalHeight = rc.Height();
        while (totalHeight > 0) {
            totalHeight -= szItem.cy;
            if (nRows != 0) {
                totalHeight -= childMarginY;
            }
            if (totalHeight >= 0) {
                ++nRows;
            }
        }
    }
    if (nRows <= 0) {
        nRows = 1;
    }
    return nRows;
}

int32_t ListBox::CalcHTileColumns(HTileLayout* pHTileLayout) const
{
    int32_t nColumns = 1;
    if (pHTileLayout == nullptr) {
        return nColumns;
    }
    UiSize szItem = pHTileLayout->GetItemSize();
    if (szItem.cx <= 0) {
        return nColumns;
    }
    int32_t childMarginX = pHTileLayout->GetChildMarginX();
    if (childMarginX < 0) {
        childMarginX = 0;
    }
    nColumns = 0;
    UiRect rc = GetRect();
    rc.Deflate(GetControlPadding());
    int32_t totalWidth = rc.Width();
    while (totalWidth > 0) {
        totalWidth -= szItem.cx;
        if (nColumns != 0) {
            totalWidth -= childMarginX;
        }
        if (totalWidth >= 0) {
            ++nColumns;
        }
    }
    if (nColumns <= 0) {
        nColumns = 1;
    }
    return nColumns;
}

int32_t ListBox::CalcVTileColumns(VTileLayout* pVTileLayout) const
{
    int32_t nColumns = 1;
    if (pVTileLayout == nullptr) {
        return nColumns;
    }
    
    nColumns = pVTileLayout->GetColumns();
    bool bAutoColumns = pVTileLayout->IsAutoCalcColumns();
    if (bAutoColumns) {
        nColumns = 0;
    }
    if (nColumns <= 0) {
        UiSize szItem = pVTileLayout->GetItemSize();
        if (szItem.cx <= 0) {
            return nColumns;
        }
        int32_t childMarginX = pVTileLayout->GetChildMarginX();
        if (childMarginX < 0) {
            childMarginX = 0;
        }

        UiRect rc = GetRect();
        rc.Deflate(GetControlPadding());
        int32_t totalWidth = rc.Width();
        while (totalWidth > 0) {
            totalWidth -= szItem.cx;
            if (nColumns != 0) {
                totalWidth -= childMarginX;
            }
            if (totalWidth >= 0) {
                ++nColumns;
            }
        }
    }
    if (nColumns <= 0) {
        nColumns = 1;
    }
    return nColumns;
}

int32_t ListBox::CalcVTileRows(VTileLayout* pVTileLayout) const
{
    int32_t nRows = 1;
    if (pVTileLayout == nullptr) {
        return nRows;
    }
    UiSize szItem = pVTileLayout->GetItemSize();
    if (szItem.cy <= 0) {
        return nRows;
    }
    int32_t childMarginY = pVTileLayout->GetChildMarginY();
    if (childMarginY < 0) {
        childMarginY = 0;
    }
    nRows = 0;
    UiRect rc = GetRect();
    rc.Deflate(GetControlPadding());
    int32_t totalHeight = rc.Height();
    while (totalHeight > 0) {
        totalHeight -= szItem.cy;
        if (nRows != 0) {
            totalHeight -= childMarginY;
        }
        if (totalHeight >= 0) {
            ++nRows;
        }
    }
    if (nRows <= 0) {
        nRows = 1;
    }
    return nRows;
}

void ListBox::SendEventMsg(const EventArgs& msg)
{
    auto msgFlag = GetWeakFlag();
    ScrollBox::SendEventMsg(msg);
    if (!msgFlag.expired() && ((msg.eventType == kEventSelect) || (msg.eventType == kEventUnSelect))) {
        //Trigger the selection change event
        SendEvent(kEventSelChanged);
    }
}

size_t ListBox::GetCurSel() const
{
    return m_iCurSel;
}

void ListBox::SetCurSel(size_t iIndex)
{
    if (Box::IsValidItemIndex(iIndex)) {
        ASSERT(iIndex < GetItemCount());
        if (iIndex > GetItemCount()) {
            return;
        }
    }    
    m_iCurSel = iIndex;
}

bool ListBox::IsScrollSelect() const
{
    return m_bScrollSelect;
}

void ListBox::SetScrollSelect(bool bScrollSelect)
{
    m_bScrollSelect = bScrollSelect;
}

bool ListBox::IsSelectNextWhenActiveRemoved() const
{
    return m_bSelectNextWhenActiveRemoved;
}

void ListBox::SetSelectNextWhenActiveRemoved(bool bSelectNextItem)
{
    m_bSelectNextWhenActiveRemoved = bSelectNextItem;
}

void ListBox::GetSelectedItems(std::vector<size_t>& selectedIndexs) const
{
    selectedIndexs.clear();
    const size_t itemCount = GetItemCount();
    for (size_t iIndex = 0; iIndex < itemCount; ++iIndex) {
        Control* pControl = m_items[iIndex];
        if ((pControl == nullptr) || !pControl->IsVisible() || !pControl->IsEnabled()) {
            continue;
        }
        IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
        if (pListItem != nullptr) {
            if (pListItem->IsSelected()) {
                selectedIndexs.push_back(iIndex);
            }
        }
    }
}

size_t ListBox::FindSelectable(size_t iIndex, bool bForward) const
{
    return BaseClass::FindSelectable(iIndex, bForward);
}

bool ListBox::OnFindSelectable(size_t /*nCurSel*/, SelectableMode /*mode*/,
                               size_t /*nCount*/, size_t& /*nDestItemIndex*/)
{
    return false;
}

size_t ListBox::GetItemCountBefore(size_t nCurSel)
{
    if (nCurSel < GetItemCount()) {
        return nCurSel;
    }
    else {
        return 0;
    }
}

size_t ListBox::GetItemCountAfter(size_t nCurSel)
{
    size_t nCount = 0;
    if (nCurSel < GetItemCount()) {
        nCount = GetItemCount() - nCurSel - 1;
    }
    return nCount;
}

bool ListBox::SelectItem(size_t iIndex, bool bTakeFocus, bool bTriggerEvent, uint64_t vkFlag)
{
    bool bRet = false;
    if (!IsSelectLikeListCtrl()) {
        //This feature is not enabled
        if (IsMultiSelect()) {
            //Multi-selection
            bRet = SelectItemMulti(iIndex, bTakeFocus, bTriggerEvent);
        }
        else {
            //Single selection
            bRet = SelectItemSingle(iIndex, bTakeFocus, bTriggerEvent);
        }
        if (bRet) {
            OnSelectStatusChanged();
        }
    }
    else {
        return ListCtrlSelectItem(iIndex, bTakeFocus, bTriggerEvent, vkFlag);
    }
    return bRet;
}

bool ListBox::ListCtrlSelectItem(size_t iIndex, bool bTakeFocus,
                                 bool bTriggerEvent, uint64_t vkFlag)
{
    if (!IsVisible()) {
        //When hidden, avoid taking focus
        bTakeFocus = false;
    }
    if (IsVisible()) {
        if (!IsSelectableItem(iIndex)) {
            //The value of iIndex is invalid, or the current child item is not selectable
            return false;
        }
    }
    else {
        Control* pControl = GetItemAt(iIndex);
        if ((pControl == nullptr) || !pControl->IsSelectableType() || !pControl->IsEnabled()) {
            //The value of iIndex is invalid, or the current child item is not selectable
            return false;
        }
    }

    //Event triggering must be placed before the function returns, not in the middle of the code
    bool bSelectStatusChanged = false;
    bool bRet = false;
    if (IsMultiSelect()) {
        //Multi-selection mode        
        bool bRbuttonDown = vkFlag & kVkRButton;
        bool bShiftDown = vkFlag & kVkShift;
        bool bControlDown = vkFlag & kVkControl;
        if (bShiftDown && bControlDown) {
            //Shift and Ctrl keys pressed simultaneously, ignore
            bShiftDown = false;
            bControlDown = false;
        }
        if (bRbuttonDown || (!bShiftDown && !bControlDown)) {
            //When the right button is pressed: if the current item is not selected, use single-selection logic to keep only one option;
            //            if it is already selected, keep the original selection and leave the selection state of all items unchanged (to provide the opportunity to operate on the selected items via the right-click menu)
            //When neither the Control key nor the Shift key is pressed: use single-selection logic to keep only one option            
            if (bRbuttonDown && IsItemSelected(iIndex)) {
                bRet = true;
            }
            else {                             
                //Unselect the other selection items
                size_t nItemCount = GetItemCount();
                for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
                    if (nItemIndex == iIndex) {
                        continue;
                    }
                    Control* pControl = GetItemAt(nItemIndex);
                    if ((pControl == nullptr) || !pControl->IsVisible() || !pControl->IsEnabled()) {
                        continue;
                    }
                    IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
                    if (pListItem == nullptr) {
                        continue;
                    }
                    if (pListItem->IsSelected()) {
                        //If it was previously selected, update it to not selected
                        pListItem->OptionSelected(false, false);
                    }
                }
                SetLastNoShiftItem(iIndex);
                SetCurSel(iIndex);
                bRet = SelectItemSingle(iIndex, bTakeFocus, false);
                bSelectStatusChanged = true;
            }
        }
        else {
            if (bShiftDown) {
                //Left button pressed: Shift key also pressed
                size_t nIndexStart = GetLastNoShiftItem();
                if (nIndexStart >= GetItemCount()) {
                    nIndexStart = 0;
                }
                if (iIndex < GetItemCount()) {
                    std::set<size_t> selectedIndexs;
                    size_t iStart = std::min(nIndexStart, iIndex);
                    size_t iEnd = std::max(nIndexStart, iIndex);
                    for (size_t i = iStart; i <= iEnd; ++i) {
                        if (IsSelectableItem(i)) {
                            selectedIndexs.insert(i);
                        }
                    }
                    size_t nItemCount = GetItemCount();
                    for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
                        Control* pControl = GetItemAt(nItemIndex);
                        if ((pControl == nullptr) || !pControl->IsVisible() || !pControl->IsEnabled()) {
                            continue;
                        }
                        IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
                        if (pListItem == nullptr) {
                            continue;
                        }
                        if (selectedIndexs.find(nItemIndex) != selectedIndexs.end()) {
                            //Selected set
                            if (!pListItem->IsSelected()) {
                                //If it was previously not selected, update it to selected
                                pListItem->OptionSelected(true, false);
                            }
                        }
                        else {
                            //Unselect the others
                            if (pListItem->IsSelected()) {
                                //If it was previously selected, update it to not selected
                                pListItem->OptionSelected(false, false);
                            }
                        }
                    }
                    SetCurSel(iIndex);
                    bRet = SelectItemSingle(iIndex, bTakeFocus, false);
                    bSelectStatusChanged = true;
                }
                else {
                    //Unknown situation; this should normally never be reached
                    bRet = SelectItemMulti(iIndex, bTakeFocus, false);
                }
            }
            else {
                //Left button pressed: Control key also pressed, keep multi-selection
                bRet = SelectItemMulti(iIndex, bTakeFocus, false);
                if (bRet) {
                    SetLastNoShiftItem(iIndex);
                }
            }
        }
    }
    else {
        //Single selection
        bRet = SelectItemSingle(iIndex, bTakeFocus, false);
    }
    if (bSelectStatusChanged) {
        OnSelectStatusChanged();
    }
    if (bTriggerEvent && bRet) {
        SendEvent(kEventSelect, iIndex, Box::InvalidIndex);
    }
    return bRet;
}

bool ListBox::UnSelectItem(size_t iIndex, bool bTriggerEvent)
{
    bool bHasEvent = false;
    Control* pControl = GetItemAt(iIndex);
    if (pControl != nullptr) {
        IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
        if ((pListItem != nullptr) && pListItem->IsSelected()) {
            pListItem->OptionSelected(false, bTriggerEvent);
            bHasEvent = true;            
            //Only redraw when the state changes
            Invalidate();
        }
    }
    if (iIndex == m_iCurSel) {
        m_iCurSel = Box::InvalidIndex;
    }
    if (bTriggerEvent && bHasEvent) {
        //Event triggering must be placed before the function returns, not in the middle of the code
        SendEvent(kEventUnSelect, iIndex, Box::InvalidIndex);
    }
    return bHasEvent;
}

void ListBox::OnItemSelectedChanged(size_t /*iIndex*/, IListBoxItem* /*pListBoxItem*/)
{
}

void ListBox::OnItemCheckedChanged(size_t /*iIndex*/, IListBoxItem* /*pListBoxItem*/)
{
}

bool ListBox::SelectItemSingle(size_t iIndex, bool bTakeFocus, bool bTriggerEvent)
{
    if (!IsVisible()) {
        //When hidden, avoid taking focus
        bTakeFocus = false;
    }
    //Single selection
    if (iIndex == m_iCurSel) {
        Control* pControl = GetItemAt(iIndex);
        if (pControl == nullptr) {
            m_iCurSel = Box::InvalidIndex;
            return false;
        }        
        //Ensure it is visible, then return        
        if (bTakeFocus) {
            pControl->SetFocus();
        }
        bool bChanged = false;
        IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
        if ((pListItem != nullptr) && !pListItem->IsSelected()) {
            bChanged = true;
            pListItem->OptionSelected(true, bTriggerEvent);
        }
        Invalidate();
        if (bChanged && bTriggerEvent) {
            SendEvent(kEventSelect, m_iCurSel, Box::InvalidIndex);
        }
        return bChanged;
    }
    bool hasUnSelectEvent = false;
    const size_t iOldSel = m_iCurSel;
    if (Box::IsValidItemIndex(iOldSel)) {
        //Clear the selection state of the old selected item
        Control* pControl = GetItemAt(iOldSel);
        if (pControl != nullptr) {
            IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
            if ((pListItem != nullptr) && pListItem->IsSelected()) {
                pListItem->OptionSelected(false, bTriggerEvent);
                hasUnSelectEvent = true;                
            }
        }
        m_iCurSel = Box::InvalidIndex;
    }
    if (!Box::IsValidItemIndex(iIndex)) {
        Invalidate();
        if (hasUnSelectEvent && bTriggerEvent) {
            SendEvent(kEventUnSelect, iOldSel, Box::InvalidIndex);
        }
        return hasUnSelectEvent;
    }

    Control* pControl = GetItemAt(iIndex);
    if ((pControl == nullptr) || (IsVisible() && !pControl->IsVisible()) || !pControl->IsEnabled()) {
        Invalidate();
        if (hasUnSelectEvent && bTriggerEvent) {
            SendEvent(kEventUnSelect, iOldSel, Box::InvalidIndex);
        }
        return hasUnSelectEvent;
    }
    IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
    if (pListItem == nullptr) {
        Invalidate();
        if (hasUnSelectEvent && bTriggerEvent) {
            SendEvent(kEventUnSelect, iOldSel, Box::InvalidIndex);
        }
        return hasUnSelectEvent;
    }
    m_iCurSel = iIndex;
    //Set the selection state
    pListItem->OptionSelected(true, bTriggerEvent);
    pControl = GetItemAt(m_iCurSel);
    if (pControl != nullptr) {        
        if (bTakeFocus) {
            pControl->SetFocus();
        }
    }

    Invalidate();
    if (hasUnSelectEvent && bTriggerEvent) {
        SendEvent(kEventUnSelect, iOldSel, Box::InvalidIndex);
    }
    if (bTriggerEvent) {
        SendEvent(kEventSelect, m_iCurSel, iOldSel);
    }    
    return true;
}

bool ListBox::SelectItemMulti(size_t iIndex, bool bTakeFocus, bool bTriggerEvent)
{
    if (!IsVisible()) {
        //When hidden, avoid taking focus
        bTakeFocus = false;
    }
    //Multi-selection: m_iCurSel always points to the last selected item
    size_t iOldSel = m_iCurSel;
    m_iCurSel = Box::InvalidIndex;
    if (!Box::IsValidItemIndex(iIndex)) {
        Invalidate();
        return false;
    }
    Control* pControl = GetItemAt(iIndex);
    if ((pControl == nullptr) || (IsVisible() && !pControl->IsVisible()) || !pControl->IsEnabled()){
        Invalidate();
        return false;
    }
    IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
    if (pListItem == nullptr) {
        Invalidate();
        return false;
    }
    
    if (pListItem->IsSelected()) {
        //In multi-selection mode, selecting again is treated as unselecting
        pListItem->OptionSelected(false, false);
        Invalidate();
        if (bTriggerEvent) {
            SendEvent(kEventUnSelect, iIndex, Box::InvalidIndex);
        }
    }
    else {
        //If it was previously not selected, update it to selected
        m_iCurSel = iIndex;
        pListItem->OptionSelected(true, false);        
        if (bTakeFocus) {            
            pControl->SetFocus();
        }
        Invalidate();
        if (bTriggerEvent) {
            SendEvent(kEventSelect, iIndex, iOldSel);
        }
    }    
    return true;
}

void ListBox::EnsureVisible(const UiRect& rcItem,
                            ListBoxVerVisible vVisibleType,
                            ListBoxHorVisible hVisibleType)
{
    UiRect rcNewItem = rcItem;
    UiSize scrollOffset = GetScrollOffset();
    rcNewItem.Offset(-scrollOffset.cx, -scrollOffset.cy);
    UiRect rcList = GetPos();
    rcList.Deflate(GetPadding());

    ScrollBar* pVScrollBar = GetVScrollBar();
    if (pVScrollBar && pVScrollBar->IsValid()) {
        if (IsVScrollBarAtLeft()) {
            ASSERT(pVScrollBar->GetFixedWidth().GetInt32() > 0);
            rcList.left += pVScrollBar->GetFixedWidth().GetInt32();
        }
        else {
            ASSERT(pVScrollBar->GetFixedWidth().GetInt32() > 0);
            rcList.right -= pVScrollBar->GetFixedWidth().GetInt32();
        }
    }

    ScrollBar* pHScrollBar = GetHScrollBar();
    if (pHScrollBar && pHScrollBar->IsValid()) {
        ASSERT(pHScrollBar->GetFixedHeight().GetInt32() > 0);
        rcList.bottom -= pHScrollBar->GetFixedHeight().GetInt32();
    }

    if ((rcNewItem.left >= rcList.left) && (rcNewItem.top >= rcList.top) && 
        (rcNewItem.right <= rcList.right) && (rcNewItem.bottom <= rcList.bottom)) {
        IListBoxItem* listBoxElement = dynamic_cast<IListBoxItem*>(GetParent());
        IListBoxOwner* lisBoxOwner = nullptr;
        if (listBoxElement != nullptr) {
            lisBoxOwner = listBoxElement->GetOwner();
        }
        if (lisBoxOwner != nullptr) {
            lisBoxOwner->EnsureVisible(rcNewItem, vVisibleType, hVisibleType);
        }        
        return;
    }
    //Horizontal scroll bar
    int32_t dx = 0;
    if (hVisibleType == ListBoxHorVisible::kVisibleAtCenter) {
        //Display centered
        if (rcNewItem.left < rcList.CenterX()) {
            dx = rcNewItem.left - rcList.CenterX();
        }
        if (rcNewItem.right > rcList.CenterX()) {
            dx = rcNewItem.right - rcList.CenterX();
        }
    }
    else if (hVisibleType == ListBoxHorVisible::kVisibleAtLeft) {
        //Display aligned to the left
        dx = rcNewItem.left - rcList.left;
    }
    else if (hVisibleType == ListBoxHorVisible::kVisibleAtRight) {
        //Display aligned to the right
        dx = rcNewItem.right - rcList.right;
    }
    else {
        if (rcNewItem.left < rcList.left) {
            dx = rcNewItem.left - rcList.left;
        }
        if (rcNewItem.right > rcList.right) {
            dx = rcNewItem.right - rcList.right;
        }
        UiRect rcNewList = rcList;
        rcNewList.top = rcNewItem.top;
        rcNewList.bottom = rcNewItem.bottom;
        if (rcNewItem.ContainsRect(rcNewList)) {
            //The child item's region is already within the visible region; do not adjust, to avoid horizontal jitter after mouse clicks
            dx = 0;
        }
    }
    //Vertical scroll bar
    int32_t dy = 0;
    if (vVisibleType == ListBoxVerVisible::kVisibleAtCenter) {
        //Display centered
        if (rcNewItem.top < rcList.CenterY()) {
            dy = rcNewItem.top - rcList.CenterY();
        }
        if (rcNewItem.bottom > rcList.CenterY()) {
            dy = rcNewItem.bottom - rcList.CenterY();
        }
    }
    else if (vVisibleType == ListBoxVerVisible::kVisibleAtTop) {
        //Align to the top
        dy = rcNewItem.top - rcList.top;
    }
    else if (vVisibleType == ListBoxVerVisible::kVisibleAtBottom) {
        //Align to the bottom
        dy = rcNewItem.bottom - rcList.bottom;
    }
    else {
        //Just make it visible
        if (rcNewItem.top < rcList.top) {
            dy = rcNewItem.top - rcList.top;
        }
        if (rcNewItem.bottom > rcList.bottom) {
            dy = rcNewItem.bottom - rcList.bottom;
        }
        UiRect rcNewList = rcList;
        rcNewList.left = rcNewItem.left;
        rcNewList.right = rcNewItem.right;
        if (rcNewItem.ContainsRect(rcNewList)) {
            //The child item's region is already within the visible region; do not adjust, to avoid vertical jitter after mouse clicks
            dy = 0;
        }
    }
    if ((dx != 0) || (dy != 0)) {
        UiSize64 sz = GetScrollPos();
        SetScrollPos(UiSize64(sz.cx + dx, sz.cy + dy));
        Invalidate();
    }
}

void ListBox::StopScroll()
{
}

bool ListBox::CanPaintSelectedColors(bool bHasStateImages) const
{
    if (m_uPaintSelectedColors == PAINT_SELECTED_COLORS_YES) {
        return true;
    }
    else if (m_uPaintSelectedColors == PAINT_SELECTED_COLORS_NO) {
        return false;
    }
    if (bHasStateImages && IsMultiSelect()) {
        //If there is a CheckBox, the selection background color is not shown by default in multi-selection mode
        return false;
    }
    return true;
}

bool ListBox::IsHorizontalLayout() const
{
    return GetLayout()->IsHLayout();
}

bool ListBox::IsHorizontalScrollBar() const
{
    bool bHasVScrollBar = false;
    ScrollBar* pVScrollBar = GetVScrollBar();
    if (pVScrollBar && pVScrollBar->IsValid()) {
        bHasVScrollBar = true;
    }

    bool bHasHScrollBar = false;
    ScrollBar* pHScrollBar = GetHScrollBar();
    if (pHScrollBar && pHScrollBar->IsValid()) {
        bHasHScrollBar = true;
    }

    bool bIsHorizontal = (bHasHScrollBar && !bHasVScrollBar) ? true : false;
    if (GetLayout()->IsHLayout()) {
        //It is confirmed to be a horizontal layout
        bIsHorizontal = true;
    }
    else if (GetLayout()->IsVLayout()) {
        bIsHorizontal = false;
    }
    return bIsHorizontal;
}

bool ListBox::ScrollItemToTop(size_t iIndex)
{
    Control* pControl = GetItemAt(iIndex);
    if ((pControl == nullptr) || !pControl->IsVisible()) {
        return false;
    }
    if (IsHorizontalLayout()) {
        //Horizontal layout
        if (GetScrollRange().cx != 0) {
            UiSize64 scrollPos = GetScrollPos();
            scrollPos.cx = (int64_t)pControl->GetPos().left - GetPosWithoutPadding().left;
            if (scrollPos.cx >= 0) {
                SetScrollPos(scrollPos);
                return true;
            }
        }
    }
    else {
        //Vertical layout
        if (GetScrollRange().cy != 0) {
            UiSize64 scrollPos = GetScrollPos();
            scrollPos.cy = (int64_t)pControl->GetPos().top - GetPosWithoutPadding().top;
            if (scrollPos.cy >= 0) {
                SetScrollPos(scrollPos);
                return true;
            }
        }
    }
    return false;
}

bool ListBox::ScrollItemToTop(const DString& itemName)
{
    const size_t itemCount = m_items.size();
    for (size_t iIndex = 0; iIndex < itemCount; ++iIndex) {
        Control* pControl = m_items[iIndex];
        if ((pControl == nullptr) || !pControl->IsVisible()) {
            continue;
        }
        if (pControl->IsNameEquals(itemName)) {
            return ScrollItemToTop(iIndex);
        }
    }
    return false;
}

Control* ListBox::GetTopItem() const
{
    if (IsHorizontalLayout()) {
        //Horizontal layout
        int32_t listLeft = GetPos().left + GetPadding().left + GetScrollOffset().cx;
        for (Control* pControl : m_items) {
            ASSERT(pControl != nullptr);
            if (pControl->IsVisible() && !pControl->IsFloat() && pControl->GetPos().right >= listLeft) {
                return pControl;
            }
        }
    }
    else {
        //Vertical layout
        int32_t listTop = GetPos().top + GetPadding().top + GetScrollOffset().cy;
        for (Control* pControl : m_items) {
            ASSERT(pControl != nullptr);
            if (pControl->IsVisible() && !pControl->IsFloat() && pControl->GetPos().bottom >= listTop) {
                return pControl;
            }
        }
    }
    return nullptr;
}

bool ListBox::SetItemIndex(Control* pControl, size_t iIndex)
{
    size_t iOrginIndex = GetItemIndex(pControl);
    if (!Box::IsValidItemIndex(iOrginIndex)) {
        return false;
    }
    if (iOrginIndex == iIndex) {
        return true;
    }

    IListBoxItem* pSelectedListItem = nullptr;
    if (Box::IsValidItemIndex(m_iCurSel)) {
        pSelectedListItem = dynamic_cast<IListBoxItem*>(GetItemAt(m_iCurSel));
    }
    if (!ScrollBox::SetItemIndex(pControl, iIndex)) {
        return false;
    }
    size_t iMinIndex = std::min(iOrginIndex, iIndex);
    size_t iMaxIndex = std::max(iOrginIndex, iIndex);
    for(size_t i = iMinIndex; i < iMaxIndex + 1; ++i) {
        Control* pItemControl = GetItemAt(i);
        IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pItemControl);
        if( pListItem != nullptr ) {
            pListItem->SetListBoxIndex(i);
        }
    }
    if (Box::IsValidItemIndex(m_iCurSel) && pSelectedListItem != nullptr) {
        m_iCurSel = pSelectedListItem->GetListBoxIndex();
    }
    return true;
}

size_t ListBox::EnsureVisible(size_t iIndex, ListBoxVerVisible vVisibleType, ListBoxHorVisible hVisibleType)
{
    Control* pControl = GetItemAt(iIndex);
    ASSERT(pControl != nullptr);
    if (pControl != nullptr) {
        UiRect rcItem = pControl->GetPos();
        EnsureVisible(rcItem, vVisibleType, hVisibleType);
        ASSERT(GetItemAt(iIndex) == pControl);
    }
    return iIndex;
}

bool ListBox::AddItem(Control* pControl)
{
    IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
    if( pListItem != nullptr) {
        pListItem->SetOwner(this);
        pListItem->SetListBoxIndex(GetItemCount());
        if (!IsMultiSelect()) {
            pListItem->OptionSelected(false, false);
        }
    }
    bool bRet = ScrollBox::AddItem(pControl);
    OnListBoxItemAdded(pControl);
    return bRet;
}

bool ListBox::AddItemAt(Control* pControl, size_t iIndex)
{
    if (!ScrollBox::AddItemAt(pControl, iIndex)) {
        return false;
    }

    IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
    if( pListItem != nullptr ) {
        pListItem->SetOwner(this);
        pListItem->SetListBoxIndex(iIndex);
        if (!IsMultiSelect()) {
            pListItem->OptionSelected(false, false);
        }
    }

    const size_t itemCount = GetItemCount();
    for(size_t i = iIndex + 1; i < itemCount; ++i) {
        Control* p = GetItemAt(i);
        pListItem = dynamic_cast<IListBoxItem*>(p);
        if( pListItem != nullptr ) {
            pListItem->SetListBoxIndex(i);
        }
    }
    if (Box::IsValidItemIndex(m_iCurSel) && (m_iCurSel >= iIndex)) {
        m_iCurSel += 1;
    }
    OnListBoxItemAdded(pControl);
    return true;
}

bool ListBox::RemoveItem(Control* pControl)
{
    size_t iIndex = GetItemIndex(pControl);
    if (!Box::IsValidItemIndex(iIndex)) {
        return false;
    }
    return RemoveItemAt(iIndex);
}

bool ListBox::RemoveItemAt(size_t iIndex)
{
    if (!IsAutoDestroyChild()) {
        Control* p = GetItemAt(iIndex);
        IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(p);
        if (pListItem != nullptr) {
            pListItem->SetOwner(nullptr);
        }
    }
    if (!IsAutoDestroyChild()) {
        OnListBoxItemRemoved(GetItemAt(iIndex));
    }
    if (!ScrollBox::RemoveItemAt(iIndex)) {
        return false;
    }
    const size_t itemCount = GetItemCount();
    for(size_t i = iIndex; i < itemCount; ++i) {
        Control* p = GetItemAt(i);
        IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(p);
        if (pListItem != nullptr) {
            pListItem->SetListBoxIndex(i);
        }
    }

    if (Box::IsValidItemIndex(m_iCurSel)) {
        if (iIndex == m_iCurSel) {
            if (!IsMultiSelect() && m_bSelectNextWhenActiveRemoved) {
                SelectItem(FindSelectable(m_iCurSel--, false));
            }
            else {
                m_iCurSel = Box::InvalidIndex;
            }
        }
        else if (iIndex < m_iCurSel) {
            m_iCurSel -= 1;
        }
    }    
    return true;
}

void ListBox::RemoveAllItems()
{
    if (!IsAutoDestroyChild()) {
        const size_t itemCount = GetItemCount();
        for (size_t i = 0; i < itemCount; ++i) {
            Control* p = GetItemAt(i);
            IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(p);
            if (pListItem != nullptr) {
                pListItem->SetOwner(nullptr);
            }
            OnListBoxItemRemoved(p);
        }
    }
    m_iCurSel = Box::InvalidIndex;
    ScrollBox::RemoveAllItems();
}

bool ListBox::SortItems(PFNCompareFunc pfnCompare, void* pCompareContext)
{
    if (pfnCompare == nullptr) {
        return false;
    }        
    if (m_items.empty()) {
        return true;
    }

    m_pCompareFunc = pfnCompare;
    m_pCompareContext = pCompareContext;
#if defined (_WIN32) || defined (_WIN64)
    //Windows system
    qsort_s(&(*m_items.begin()), m_items.size(), sizeof(Control*), ListBox::ItemComareFuncWindows, this);
#elif defined(__APPLE__)
    // macOS
    qsort_r(&(*m_items.begin()), m_items.size(), sizeof(Control*), this, ListBox::ItemComareFuncMacOS);   
#else
    //Linux system
    qsort_r(&(*m_items.begin()), m_items.size(), sizeof(Control*), ListBox::ItemComareFuncLinux, this);
#endif    
    IListBoxItem* pItem = nullptr;
    const size_t itemCount = m_items.size();
    for (size_t i = 0; i < itemCount; ++i) {
        pItem = dynamic_cast<IListBoxItem*>(m_items[i]);
        if (pItem != nullptr) {
            pItem->SetListBoxIndex(i);
            //Unselect all selection items on the UI
            pItem->OptionSelected(false, false);
        }
    }
    SelectItem(Box::InvalidIndex);
    SetPos(GetPos());
    Invalidate();
    return true;
}

int ListBox::ItemComareFuncWindows(void* pvlocale, const void* item1, const void* item2)
{
    ListBox* pThis = (ListBox*)pvlocale;
    if (!pThis || !item1 || !item2) {
        return 0;
    }
    return pThis->ItemComareFunc(item1, item2);
}

int ListBox::ItemComareFuncLinux(const void *item1, const void *item2, void* pvlocale)
{
    ListBox *pThis = (ListBox*)pvlocale;
    if (!pThis || !item1 || !item2) {
        return 0;
    }
    return pThis->ItemComareFunc(item1, item2);
}

int ListBox::ItemComareFuncMacOS(void* context, const void* item1, const void* item2)
{
    ListBox* pThis = (ListBox*)context;
    if (!pThis || !item1 || !item2) {
        return 0;
    }
    return pThis->ItemComareFunc(item1, item2);
}

int ListBox::ItemComareFunc(const void *item1, const void *item2)
{
    if (!item1 || !item2) {
        return 0;
    }
    Control *pControl1 = *(Control**)item1;
    Control *pControl2 = *(Control**)item2;
    return m_pCompareFunc(pControl1, pControl2, m_pCompareContext);
}

bool ListBox::IsMultiSelect() const
{
    return m_bMultiSelect;
}

void ListBox::SetMultiSelect(bool bMultiSelect)
{
    m_bMultiSelect = bMultiSelect;
    if (!bMultiSelect) {
        //As long as bMultiSelect is false, execute the logic to cancel the existing multi-selection
        //Here, whether to execute the code below must not be decided by comparing with the old value; otherwise it may affect the subclass (TreeView logic)
        if (OnSwitchToSingleSelect()) {
            Invalidate();
        }
    }
}

void ListBox::EnsureSingleSelection()
{
    if (!IsMultiSelect()) {
        OnSwitchToSingleSelect();
    }    
}

bool ListBox::OnSwitchToSingleSelect()
{
    bool bChanged = false;
    IListBoxItem* pItem = nullptr;
    const size_t itemCount = m_items.size();
    if (m_iCurSel > itemCount) { 
        //If the single-selection state is not synchronized, use the first selected item as the final single selection
        for (size_t i = 0; i < itemCount; ++i) {
            pItem = dynamic_cast<IListBoxItem*>(m_items[i]);
            if ((pItem != nullptr) && pItem->IsSelected()) {
                m_iCurSel = i;
                break;
            }
        }
    }
    for (size_t i = 0; i < itemCount; ++i) {
        pItem = dynamic_cast<IListBoxItem*>(m_items[i]);
        if ((pItem != nullptr) && pItem->IsSelected()) {
            if (m_iCurSel != i) {
                pItem->OptionSelected(false, false); //Do not trigger the Select event
                bChanged = true;
            }
        }
    }
    if (UpdateCurSelItemSelectStatus()) {
        bChanged = true;
    }
    return bChanged;
}

bool ListBox::UpdateCurSelItemSelectStatus()
{
    //Synchronize the state of the currently selected item
    bool bChanged = false;
    size_t curSelIndex = GetCurSel();
    if (Box::IsValidItemIndex(curSelIndex)) {
        bool bSelectItem = false;
        IListBoxItem* pItem = dynamic_cast<IListBoxItem*>(GetItemAt(curSelIndex));
        if (pItem != nullptr) {
            bSelectItem = pItem->IsSelected();
        }
        if (!bSelectItem) {
            SetCurSel(Box::InvalidIndex);
            bChanged = true;
        }
    }
    return bChanged;
}

bool ListBox::ButtonDown(const EventArgs& msg)
{
    bool ret = BaseClass::ButtonDown(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    //Stop scrolling
    StopScroll();
    if (m_pHelper != nullptr) {
        m_pHelper->OnButtonDown(msg.ptMouse, msg.GetSender());
    }    
    return ret;
}

bool ListBox::ButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonUp(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    Control* pSender = msg.GetSender();
    if (IsEnableFrameSelection()) {
        //When Ctrl or Shift is held down, do not trigger the clear-selection operation to avoid accidental operations
        if (IsKeyDown(msg, ModifierKey::kControl)) {
            pSender = nullptr;
        }
        else if (this->IsKeyDown(msg, ModifierKey::kShift)) {
            pSender = nullptr;
        }
    }
    if (m_pHelper != nullptr) {
        m_pHelper->OnButtonUp(msg.ptMouse, pSender);
    }
    return bRet;
}

bool ListBox::RButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::RButtonDown(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (m_pHelper != nullptr) {
        m_pHelper->OnRButtonDown(msg.ptMouse, msg.GetSender());
    }
    return bRet;
}

bool ListBox::RButtonUp(const EventArgs& msg)
{
    bool bRet = BaseClass::RButtonUp(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (m_pHelper != nullptr) {
        m_pHelper->OnRButtonUp(msg.ptMouse, msg.GetSender());
    }
    return bRet;
}

bool ListBox::MouseMove(const EventArgs& msg)
{
    bool bRet = BaseClass::MouseMove(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (m_pHelper != nullptr) {
        m_pHelper->OnMouseMove(msg.ptMouse, msg.GetSender());
    }
    return bRet;
}

bool ListBox::OnWindowKillFocus(const EventArgs& msg)
{
    bool bRet = BaseClass::OnWindowKillFocus(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }
    if (m_pHelper != nullptr) {
        m_pHelper->OnWindowKillFocus();
    }
    return bRet;
}

bool ListBox::OnListBoxItemMouseEvent(const EventArgs& msg)
{
    if (m_pHelper != nullptr) {
        if (msg.eventType == kEventMouseButtonDown) {
            m_pHelper->OnButtonDown(msg.ptMouse, msg.GetSender());
        }
        else if (msg.eventType == kEventMouseButtonUp) {
            m_pHelper->OnButtonUp(msg.ptMouse, msg.GetSender());
        }
        else if (msg.eventType == kEventMouseRButtonDown) {
            m_pHelper->OnRButtonDown(msg.ptMouse, msg.GetSender());
        }
        else if (msg.eventType == kEventMouseRButtonUp) {
            m_pHelper->OnRButtonUp(msg.ptMouse, msg.GetSender());
        }
        else if (msg.eventType == kEventMouseMove) {
            m_pHelper->OnMouseMove(msg.ptMouse, msg.GetSender());
        }
    }
    //Must return false; otherwise the message will be intercepted, affecting normal functionality
    return false;
}

void ListBox::OnListBoxItemWindowKillFocus()
{
    if (m_pHelper != nullptr) {
        m_pHelper->OnWindowKillFocus();
    }
}

void ListBox::PaintChild(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::PaintChild(pRender, rcPaint);
    PaintFrameSelection(pRender);
}

void ListBox::PaintFrameSelection(IRender* pRender)
{
    if (m_pHelper != nullptr) {
        m_pHelper->PaintFrameSelection(pRender);
    }
}

void ListBox::GetScrollDeltaValue(int32_t& nHScrollValue, int32_t& nVScrollValue) const
{
    nHScrollValue = DUI_NOSET_VALUE;
    nVScrollValue = DUI_NOSET_VALUE;
    if (IsHorizontalLayout()) {
        //Horizontal layout
        int32_t deltaValue = 0;
        VirtualHTileLayout* pVirtualHTileLayout = dynamic_cast<VirtualHTileLayout*>(GetLayout());
        if (pVirtualHTileLayout != nullptr) {
            deltaValue = pVirtualHTileLayout->GetItemSize().cx * 2;
        }
        HTileLayout* pHTileLayout = dynamic_cast<HTileLayout*>(GetLayout());
        if (pHTileLayout != nullptr) {
            deltaValue = pHTileLayout->GetItemSize().cx * 2;
        }
        if (deltaValue > 0) {
            deltaValue = std::max(GetRect().Width() / 3, deltaValue);
            nHScrollValue = deltaValue;
        }
    }
    else {
        //Vertical layout
        int32_t deltaValue = 0;
        VirtualVTileLayout* pVirtualVTileLayout = dynamic_cast<VirtualVTileLayout*>(GetLayout());
        if (pVirtualVTileLayout != nullptr) {
            deltaValue = pVirtualVTileLayout->GetItemSize().cy * 2;            
        }
        VTileLayout* pVTileLayout = dynamic_cast<VirtualVTileLayout*>(GetLayout());
        if (pVTileLayout != nullptr) {
            deltaValue = pVTileLayout->GetItemSize().cy * 2;
        }
        if (deltaValue > 0) {
            deltaValue = std::max(GetRect().Height() / 3, deltaValue);
            nHScrollValue = deltaValue;
        }
    }
}

bool ListBox::OnLButtonClickedBlank()
{
    if (IsEnableFrameSelection() && IsSelectNoneWhenClickBlank()) {
        //Click the mouse left button on a blank area to cancel all selections
        bool bRet = SetSelectNone();
        OnSelectStatusChanged();
        return bRet;
    }
    return false;
}

bool ListBox::OnRButtonClickedBlank()
{
    if (IsEnableFrameSelection() && IsSelectNoneWhenClickBlank()) {
        //Click the mouse right button on a blank area to cancel all selections
        bool bRet = SetSelectNone();
        OnSelectStatusChanged();
        return bRet;
    }
    return false;
}

bool ListBox::SetSelectAll()
{
    bool bChanged = false;
    if (IsMultiSelect()) {
        size_t nItemCount = GetItemCount();
        for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
            Control* pControl = GetItemAt(nItemIndex);
            if ((pControl == nullptr) || !pControl->IsVisible() || !pControl->IsEnabled()) {
                continue;
            }
            IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
            if (pListItem == nullptr) {
                continue;
            }
            if (!pListItem->IsSelected()) {
                //If it was previously not selected, update it to selected
                pListItem->OptionSelected(true, false);
                bChanged = true;
            }
        }
    }
    if (bChanged) {
        Invalidate();
    }
    return bChanged;
}

bool ListBox::SetSelectNone()
{
    bool bChanged = false;
    if (IsMultiSelect()) {
        size_t nItemCount = GetItemCount();
        for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
            Control* pControl = GetItemAt(nItemIndex);
            if ((pControl == nullptr) || !pControl->IsVisible() || !pControl->IsEnabled()) {
                continue;
            }
            IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
            if (pListItem == nullptr) {
                continue;
            }
            if (pListItem->IsSelected()) {
                //If it was previously selected, update it to not selected
                pListItem->OptionSelected(false, false);
                bChanged = true;
            }
        }
    }
    if (bChanged) {
        SetCurSel(Box::InvalidIndex);
        Invalidate();
    }
    return bChanged;
}

void ListBox::OnSelectStatusChanged()
{
}

bool ListBox::OnFrameSelection(int64_t left, int64_t right, int64_t top, int64_t bottom)
{
    bool bChanged = false;
    if (IsEnableFrameSelection() && IsMultiSelect()) {
        UiRect rcListBox = GetRect();
        size_t nItemCount = GetItemCount();
        for (size_t nItemIndex = 0; nItemIndex < nItemCount; ++nItemIndex) {
            Control* pControl = GetItemAt(nItemIndex);
            if ((pControl == nullptr) || !pControl->IsVisible() || !pControl->IsEnabled()) {
                continue;
            }
            IListBoxItem* pListItem = dynamic_cast<IListBoxItem*>(pControl);
            if (pListItem == nullptr) {
                continue;
            }
            UiRect rc = pControl->GetRect();
            rc.Offset(-rcListBox.left, -rcListBox.top);
            int64_t nNewLeft = (std::max)(left, (int64_t)rc.left);
            int64_t nNewTop = (std::max)(top, (int64_t)rc.top);
            int64_t nNewRight = (std::min)(right, (int64_t)rc.right);
            int64_t nNewBottom = (std::min)(bottom, (int64_t)rc.bottom);
            bool bContains = (nNewBottom > nNewTop) && (nNewRight > nNewLeft);
            if (bContains) {
                //If it was previously not selected, update it to selected
                if (!pListItem->IsSelected()) {
                    pListItem->OptionSelected(true, false);
                    bChanged = true;
                }
            }
            else {
                //If it was previously selected, update it to not selected
                if (pListItem->IsSelected()) {
                    pListItem->OptionSelected(false, false);
                    bChanged = true;
                }
            }
        }
    }
    return bChanged;
}

void ListBox::SetLastNoShiftItem(size_t nLastNoShiftItem)
{
    m_nLastNoShiftItem = nLastNoShiftItem;
}

size_t ListBox::GetLastNoShiftItem() const
{
    return m_nLastNoShiftItem;
}

void ListBox::OnListBoxItemAdded(Control* pControl)
{
    if (pControl == nullptr) {
        return;
    }
    if (dynamic_cast<IListBoxItem*>(pControl) == nullptr) {
        return;
    }
    const EventCallbackID callbackID = (EventCallbackID)(Control*)this;
    Control* pListBoxItem = pControl;

    //Attach the mouse events, relay them to ListBox itself, and dispatch the events to the application layer
    pListBoxItem->AttachMouseEnter([this](const EventArgs& args) {
        ListBoxFireMouseEnterLeaveEvent(args);
        return true;
        }, callbackID);
    pListBoxItem->AttachMouseLeave([this](const EventArgs& args) {
        ListBoxFireMouseEnterLeaveEvent(args);
        return true;
        }, callbackID);
    pListBoxItem->AttachDoubleClick([this](const EventArgs& args) {
        ListBoxSendEvent(args, true);
        return true;
        }, callbackID);
    pListBoxItem->AttachClick([this](const EventArgs& args) {
        ListBoxSendEvent(args, true);
        return true;
        }, callbackID);
    pListBoxItem->AttachRClick([this](const EventArgs& args) {
        ListBoxSendEvent(args, true);
        return true;
        }, callbackID);
    pListBoxItem->AttachEvent(kEventReturn, [this](const EventArgs& args) {
        ListBoxSendEvent(args, true);
        return true;
        }, callbackID);
    pListBoxItem->AttachEvent(kEventKeyDown, [this](const EventArgs& args) {
        ListBoxSendEvent(args, true, true); //Keyboard messages only trigger the event but are not processed here, to avoid duplicate handling
        return true;
        }, callbackID);
    pListBoxItem->AttachEvent(kEventKeyUp, [this](const EventArgs& args) {
        ListBoxSendEvent(args, true, true); //Keyboard messages only trigger the event but are not processed here, to avoid duplicate handling
        return true;
        }, callbackID);
}

void ListBox::OnListBoxItemRemoved(Control* pControl)
{
    if (pControl == nullptr) {
        return;
    }
    if (dynamic_cast<IListBoxItem*>(pControl) == nullptr) {
        return;
    }
    const EventCallbackID callbackID = (EventCallbackID)(Control*)this;
    pControl->DetachEventByID(callbackID);
}

void ListBox::ListBoxSendEvent(const EventArgs& msg, bool bFromItem, bool bFireEventOnly)
{
    EventArgs newMsg = msg;
    if (bFromItem) {
        newMsg.SetSender(this);
        size_t nItemIndex = GetItemIndex(msg.GetSender());
        if (nItemIndex < GetItemCount()) {
            newMsg.wParam = nItemIndex;
            newMsg.lParam = 0;
        }
        else {
            newMsg.wParam = Box::InvalidIndex;
            newMsg.lParam = 0;
        }
    }
    else if ((msg.eventType == kEventMouseDoubleClick) ||
             (msg.eventType == kEventClick) ||
             (msg.eventType == kEventRClick) ||
             (msg.eventType == kEventKeyDown) ||
             (msg.eventType == kEventKeyUp)) {
        //wParam and lParam need to be set; set these two parameter values according to the Attach function corresponding to the interface
        if (msg.GetSender() == this) {
            newMsg.wParam = Box::InvalidIndex;
            newMsg.lParam = 0;
        }
    }
    if (bFireEventOnly) {
        BaseClass::FireAllEvents(newMsg);
    }
    else {
        BaseClass::SendEventMsg(newMsg);
    }
}

void ListBox::ListBoxFireMouseEnterLeaveEvent(const EventArgs& msg)
{
    EventArgs newMsg = msg;
    newMsg.SetSender(this);
    if (msg.eventType == kEventMouseEnter) {
        newMsg.eventType = kEventItemMouseEnter;
    }
    else if (msg.eventType == kEventMouseLeave) {
        newMsg.eventType = kEventItemMouseLeave;
    }
    else {
        ASSERT(0);
        return;
    }
    size_t nItemIndex = GetItemIndex(msg.GetSender());
    if (nItemIndex < GetItemCount()) {
        newMsg.wParam = nItemIndex;
        newMsg.lParam = 0;
    }
    else {
        newMsg.wParam = Box::InvalidIndex;
        newMsg.lParam = 0;
    }
    BaseClass::FireAllEvents(newMsg);
}

} // namespace ui
