#include "duilib/Control/ListCtrlReportView.h" 
#include "duilib/Control/ListCtrl.h"
#include "duilib/Render/AutoClip.h"

//Contains classes: ListCtrlReportView / ListCtrlReportLayout

namespace ui
{
ListCtrlReportView::ListCtrlReportView(Window* pWindow) :
    ListCtrlView(pWindow, new ListCtrlReportLayout),
    m_pListCtrl(nullptr),
    m_pData(nullptr),
    m_nTopElementIndex(0),
    m_nRowGridLineWidth(0),
    m_nColumnGridLineWidth(0)
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
    ASSERT(m_pListCtrl != nullptr);
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
    ASSERT(m_pListCtrl != nullptr);
    if (m_pListCtrl == nullptr) {
        return 0;
    }
    size_t itemIndex = 0;
    if (nScrollPosY <= 0) {
        return itemIndex;
    }
    ListCtrlData* pDataProvider = m_pData;
    ASSERT(pDataProvider != nullptr);
    if (pDataProvider == nullptr) {
        return itemIndex;
    }
    const ListCtrlData::RowDataList& itemDataList = pDataProvider->GetItemDataList();
    int64_t totalItemHeight = 0;
    int32_t nItemHeight = 0;
    const size_t dataItemCount = itemDataList.size();
    for (size_t index = 0; index < dataItemCount; ++index) {
        const ListCtrlItemData& rowData = itemDataList[index];
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
    ASSERT(m_pListCtrl != nullptr);
    if (m_pListCtrl == nullptr) {
        return 0;
    }
    nItemHeight = m_pListCtrl->GetDataItemHeight();
    ListCtrlData* pDataProvider = m_pData;
    ASSERT(pDataProvider != nullptr);
    if (pDataProvider == nullptr) {
        return 0;
    }
    const ListCtrlData::RowDataList& itemDataList = pDataProvider->GetItemDataList();
    ASSERT(itemIndex < itemDataList.size());
    if (itemIndex < itemDataList.size()) {
        if (itemDataList[itemIndex].nItemHeight >= 0) {
            nItemHeight = itemDataList[itemIndex].nItemHeight;
        }        
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
    ASSERT(m_pListCtrl != nullptr);
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
    ASSERT(pDataProvider != nullptr);
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
    
    const ListCtrlData::RowDataList& itemDataList = pDataProvider->GetItemDataList();
    int64_t totalItemHeight = 0;
    int32_t nItemHeight = 0;
    const size_t dataItemCount = itemDataList.size();
    for (size_t index = 0; index < dataItemCount; ++index) {
        const ListCtrlItemData& rowData = itemDataList[index];
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
    ASSERT(m_pListCtrl != nullptr);
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
    ASSERT(pDataProvider != nullptr);
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

    const ListCtrlData::RowDataList& itemDataList = pDataProvider->GetItemDataList();
    int64_t totalItemHeight = 0;
    int32_t nItemHeight = 0;
    const size_t dataItemCount = itemDataList.size();
    for (size_t index = 0; index < dataItemCount; ++index) {
        const ListCtrlItemData& rowData = itemDataList[index];
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
        const ListCtrlItemData& rowData = itemDataList[index];
        nItemHeight = rowData.nItemHeight;
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
    ASSERT(m_pListCtrl != nullptr);
    if (m_pListCtrl == nullptr) {
        return 0;
    }
    ListCtrlData* pDataProvider = m_pData;
    ASSERT(pDataProvider != nullptr);
    if (pDataProvider == nullptr) {
        return 0;
    }
    const int32_t nDefaultItemHeight = m_pListCtrl->GetDataItemHeight(); //Default row height
    const ListCtrlData::RowDataList& itemDataList = pDataProvider->GetItemDataList();
    int64_t totalItemHeight = 0;
    int32_t nItemHeight = 0;
    const size_t dataItemCount = itemDataList.size();
    for (size_t index = 0; index < dataItemCount; ++index) {
        const ListCtrlItemData& rowData = itemDataList[index];
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
    ASSERT(pRender != nullptr);
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
    ASSERT(pHeaderCtrl != nullptr);
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
    for (Control* pControl : items) {
        if (pControl == nullptr) {
            continue;
        }
        if (!pControl->IsVisible()) {
            continue;
        }

        UiSize scrollPos = GetScrollOffset();
        UiRect rcNewPaint = GetPosWithoutPadding();
        AutoClip alphaClip(pRender, rcNewPaint, IsClip());
        rcNewPaint.Offset(scrollPos.cx, scrollPos.cy);
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

        UiPoint ptOffset(scrollPos.cx, scrollPos.cy);
        UiPoint ptOldOrg = pRender->OffsetWindowOrg(ptOffset);
        pControl->AlphaPaint(pRender, rcNewPaint);
        pRender->SetWindowOrg(ptOldOrg);
        if (bHasClip) {
            pRender->ClearClip();
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

void ListCtrlReportView::PaintGridLines(IRender* pRender)
{
    ASSERT(pRender != nullptr);
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
    DString color = GetColumnGridLineColor();
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

        for (int32_t xPos : xPosList) {
            //Place the horizontal coordinate at the right side of each sub-item control
            float fXPos = (float)xPos - (float)nColumnLineWidth/2;
            UiPointF pt1(fXPos, (float)yTop);
            UiPointF pt2(fXPos, (float)viewRect.bottom);
            pRender->DrawLine(pt1, pt2, columnLineColor, (float)nColumnLineWidth);
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

Control* ListCtrlReportView::FindControl(FINDCONTROLPROC Proc, void* pProcData,
                                         uint32_t uFlags, const UiPoint& ptMouse,
                                         const UiPoint& scrollPos)
{
    //Override the ScrollBox::FindControl function so that the Header/pinned-to-top Items are found first; only handle the case where the UIFIND_TOP_FIRST flag is present
    if ((uFlags & UIFIND_TOP_FIRST) == 0) {
        return BaseClass::FindControl(Proc, pProcData, uFlags, ptMouse, scrollPos);
    }

    std::vector<Control*> newItems = m_items;
    //Move the Header and pinned-to-top elements to the end
    std::vector<Control*> atTopItems;
    MoveTopItemsToLast(newItems, atTopItems);

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
    return FindControlInItems(newItems, Proc, pProcData, uFlags, boxPt, boxScrollPos);
}

Control* ListCtrlReportView::CreateDataItem()
{
    ASSERT(m_pListCtrl != nullptr);
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
    ASSERT(m_pListCtrl != nullptr);
    if (m_pListCtrl == nullptr) {
        return false;
    }
    ListCtrlHeader* pHeaderCtrl = m_pListCtrl->GetHeaderCtrl();
    ASSERT(pHeaderCtrl != nullptr);
    if (pHeaderCtrl == nullptr) {
        return false;
    }

    ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(pControl);
    ASSERT(pItem != nullptr);
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

    std::map<size_t, ListCtrlSubItemData2Ptr> subItemDataMap;
    for (const ListCtrlSubItemData2Pair& dataPair : subItemList) {
        subItemDataMap[dataPair.nColumnId] = dataPair.pSubItemData;
    }

    //Get the attributes of each column to be displayed
    struct ElementData
    {
        size_t nColumnIndex = Box::InvalidIndex;
        size_t nColumnId = Box::InvalidIndex;
        int32_t nColumnWidth = 0;
        ListCtrlSubItemData2Ptr pStorage;
    };
    std::vector<ElementData> elementDataList;
    const size_t nColumnCount = pHeaderCtrl->GetColumnCount();
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
        data.pStorage = subItemDataMap[data.nColumnId];
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
    DString defaultSubItemClass = m_pListCtrl->GetDataSubItemClass();
    ListCtrlSubItem defaultSubItem(m_pListCtrl->GetWindow());
    defaultSubItem.SetClass(defaultSubItemClass);

    std::vector<ControlPtrT<ListCtrlSubItem>> subItemPtrList;
    for (size_t nColumn = 0; nColumn < showColumnCount; ++nColumn) {
        const ElementData& elementData = elementDataList[nColumn];
        ListCtrlSubItem* pSubItem = nullptr;
        if (nColumn < pItem->GetItemCount()) {
            pSubItem = dynamic_cast<ListCtrlSubItem*>(pItem->GetItemAt(nColumn));
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
        subItemPtrList.push_back(ControlPtrT<ListCtrlSubItem>(pSubItem));
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
        const ListCtrlSubItemData2Ptr& pStorage = elementData.pStorage;
        if (pStorage != nullptr) {
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
                pSubItem->SetStateTextColor(stateType, defaultSubItem.GetStateTextColor(stateType));
            }
            if (!pStorage->textColor.IsEmpty()) {
                pSubItem->SetStateTextColor(kControlStateNormal, pSubItem->GetColorString(pStorage->textColor));
            }
            else {
                pSubItem->SetStateTextColor(kControlStateNormal, defaultSubItem.GetStateTextColor(kControlStateNormal));
            }
            if (!pStorage->bkColor.IsEmpty()) {
                pSubItem->SetBkColor(pStorage->bkColor);
            }
            else {
                pSubItem->SetBkColor(defaultSubItem.GetBkColor());
            }
            if (pStorage->bShowCheckBox) {
                //Add a CheckBox
                pSubItem->SetShowCheckBox(true);
                //Attach the CheckBox event handling
                pSubItem->DetachEvent(kEventCheck);
                pSubItem->DetachEvent(kEventUnCheck);
                pSubItem->SetChecked(pStorage->bChecked, false);
                pSubItem->AttachCheck([this, nColumnId, nElementIndex](const EventArgs& /*args*/) {
                    OnSubItemColumnChecked(nElementIndex, nColumnId, true);
                    return true;
                    });
                pSubItem->AttachUnCheck([this, nColumnId, nElementIndex](const EventArgs& /*args*/) {
                    OnSubItemColumnChecked(nElementIndex, nColumnId, false);
                    return true;
                    });
            }
            else {
                pSubItem->SetShowCheckBox(false);
                pSubItem->DetachEvent(kEventCheck);
                pSubItem->DetachEvent(kEventUnCheck);
                pSubItem->SetChecked(false, false);
            }
            pSubItem->SetImageId(pStorage->nImageId);
        }
        else {
            pSubItem->SetTextStyle(defaultSubItem.GetTextStyle(), false);
            pSubItem->SetText(defaultSubItem.GetText());
            pSubItem->SetTextPadding(defaultSubItem.GetTextPadding(), false);
            for (int32_t nState = kControlStateNormal; nState < kControlStateCount; ++nState) {
                ControlStateType stateType = (ControlStateType)nState;
                pSubItem->SetStateTextColor(stateType, defaultSubItem.GetStateTextColor(stateType));
            }
            pSubItem->SetBkColor(defaultSubItem.GetBkColor());
            pSubItem->SetShowCheckBox(false);
            pSubItem->DetachEvent(kEventCheck);
            pSubItem->DetachEvent(kEventUnCheck);
            pSubItem->SetChecked(false, false);
            pSubItem->SetImageId(-1);
        }
    }
    auto viewFlag = GetWeakFlag();
    //First fire the data fill callbacks for each column
    for (size_t nIndex = 0; nIndex < subItemPtrList.size(); ++nIndex) {
        const ControlPtrT<ListCtrlSubItem>& pSubItem = subItemPtrList[nIndex];
        if (viewFlag.expired() || pSubItem.expired()) {
            break;
        }
        SendEvent(kEventReportViewSubItemFilled, (WPARAM)pItem->GetListBoxIndex(), (LPARAM)pItem->GetDataItemIndex(), pSubItem.get());
    }

    //Fire the data fill callback for the current row
    if (!viewFlag.expired()) {
        SendEvent(kEventReportViewItemFilled, (WPARAM)pItem->GetListBoxIndex(), (LPARAM)pItem->GetDataItemIndex(), pItem);
    }
    return true;
}

int32_t ListCtrlReportView::GetMaxDataItemWidth(const std::vector<ListCtrlSubItemData2Ptr>& subItemList)
{
    int32_t nMaxWidth = -1;
    if (m_pListCtrl == nullptr) {
        return nMaxWidth;
    }

    //Default attributes
    ListCtrlItem defaultItem(m_pListCtrl->GetWindow());
    defaultItem.SetListCtrl(m_pListCtrl);
    defaultItem.SetClass(m_pListCtrl->GetDataItemClass());

    DString defaultSubItemClass = m_pListCtrl->GetDataSubItemClass();
    ListCtrlSubItem defaultSubItem(m_pListCtrl->GetWindow());
    defaultSubItem.SetClass(defaultSubItemClass);
    defaultSubItem.SetListCtrlItem(&defaultItem);

    ListCtrlSubItem subItem(m_pListCtrl->GetWindow());
    subItem.SetClass(defaultSubItemClass);
    subItem.SetListCtrlItem(&defaultItem);

    for (const ListCtrlSubItemData2Ptr& pStorage : subItemList) {
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
    ASSERT(pDataProvider != nullptr);
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

bool ListCtrlReportView::OnFrameSelection(int64_t left, int64_t /*right*/, int64_t top, int64_t bottom)
{
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
    ASSERT(m_pListCtrl != nullptr);
    if (m_pListCtrl == nullptr) {
        return false;
    }
    ListCtrlData* pDataProvider = m_pData;
    ASSERT(pDataProvider != nullptr);
    if (pDataProvider == nullptr) {
        return false;
    }
    const ListCtrlData::RowDataList& itemDataList = pDataProvider->GetItemDataList();
    const size_t dataItemCount = itemDataList.size();
    if (dataItemCount == 0) {
        return false;
    }

    const int32_t nDefaultItemHeight = m_pListCtrl->GetDataItemHeight(); //Default row height
    int32_t nTopItemHeights = m_pListCtrl->GetHeaderHeight(); //Height occupied by the Header and pinned-to-top elements

    std::vector<size_t> itemIndexList;

    int64_t totalItemHeight = 0;
    int32_t nItemHeight = 0;    
    for (size_t index = 0; index < dataItemCount; ++index) {
        const ListCtrlItemData& rowData = itemDataList[index];
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
        const ListCtrlItemData& rowData = itemDataList[index];
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
    ASSERT(pListBoxItem != nullptr);
    if (pListBoxItem == nullptr) {
        return;
    }
    ListCtrlItem* pItem = dynamic_cast<ListCtrlItem*>(pListBoxItem);
    if (pItem == nullptr) {
        return;
    }

    ListCtrlData* pDataProvider = m_pData;
    ASSERT(pDataProvider != nullptr);
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

void ListCtrlReportView::SetRowGridLineColor(const DString& color)
{
    if (m_rowGridLineColor != color) {
        m_rowGridLineColor = color;
        Invalidate();
    }
}

DString ListCtrlReportView::GetRowGridLineColor() const
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

void ListCtrlReportView::SetColumnGridLineColor(const DString& color)
{
    if (m_columnGridLineColor != color) {
        m_columnGridLineColor = color;
        Invalidate();
    }
}

DString ListCtrlReportView::GetColumnGridLineColor() const
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
    VirtualListBox::RefreshData refreshData;

    UiSize szItem;
    size_t iCount = 0;
    std::vector<size_t> atTopUiItemIndexList;
    //The first element is the header control; skip filling data, so start from 1
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
            // Fill data
            //TODO: Optimize the code to avoid filling on every refresh
            pDataView->FillElementData(pControl, nElementIndex);
            diplayItemIndexList.push_back(nElementIndex);

            refreshData.nItemIndex = index;
            refreshData.pControl = pControl;
            refreshData.nElementIndex = nElementIndex;
            refreshDataList.push_back(refreshData);

            ListCtrlItem* pListCtrlItem = dynamic_cast<ListCtrlItem*>(pControl);
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
            pDataView->FillElementData(pControl, nElementIndex);
            diplayItemIndexList.push_back(nElementIndex);

            refreshData.nItemIndex = index;
            refreshData.pControl = pControl;
            refreshData.nElementIndex = nElementIndex;
            refreshDataList.push_back(refreshData);
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

