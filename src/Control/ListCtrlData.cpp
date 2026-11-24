#include "dui/Control/ListCtrlData.h"
#include "dui/Control/ListCtrl.h"
#include "dui/Core/GlobalManager.h"
#include <set>
#include <algorithm>

namespace ui
{
ListCtrlData::ListCtrlData() :
    m_pListView(nullptr),
    m_pfnCompareFunc(nullptr),
    m_pUserData(nullptr),
    m_hideRowCount(0),
    m_heightRowCount(0),
    m_atTopRowCount(0),
    m_bMultiSelect(true),
    m_nSelectedRowId(kInvalidRowId),
    m_nextRowId(0),
    m_nDefaultTextStyle(0),
    m_nDefaultItemHeight(-1),
    m_bAutoCheckSelect(false)
{
}

Control* ListCtrlData::CreateElement(ui::VirtualListBox* pVirtualListBox)
{
    if (pVirtualListBox == nullptr) {
        return nullptr;
    }
    ASSERT(pVirtualListBox->GetWindow() != nullptr);
    Control* pItem = nullptr;
    ASSERT(m_pListView != nullptr);
    if (m_pListView != nullptr) {
        pItem = m_pListView->CreateDataItem();
    }
    return pItem;
}

bool ListCtrlData::FillElement(Control* pControl, size_t nElementIndex)
{
    ASSERT(pControl != nullptr);
    ASSERT(nElementIndex < m_displayOrder.size());
    if ((nElementIndex >= m_displayOrder.size()) || (pControl == nullptr)) {
        return false;
    }
    const RowId rowId = m_displayOrder[nElementIndex];
    ASSERT(rowId < m_rowDataList.size());
    if (rowId >= m_rowDataList.size()) {
        return false;
    }
    const ListCtrlItemData& itemData = m_rowDataList[rowId];
    //Reuse the member buffer so that filling a row does not reallocate a temporary container each time
    std::vector<ListCtrlSubItemData2Pair>& subItemList = m_subItemListBuffer;
    if (!GetSubItemStorageList(nElementIndex, subItemList)) {//clears subItemList internally
        return false;
    }

    bool bRet = false;
    ASSERT(m_pListView != nullptr);
    if (m_pListView != nullptr) {
        bRet = m_pListView->FillDataItem(pControl, nElementIndex, itemData, subItemList);
    }
    return bRet;
}

int32_t ListCtrlData::GetMaxColumnWidth(size_t columnId) const
{
    int32_t nMaxWidth = -1;
    std::vector<const ListCtrlSubItemData2*> subItemList;
    auto iter = m_dataMap.find(columnId);
    ASSERT(iter != m_dataMap.end());
    if (iter != m_dataMap.end()) {
        const StoragePageList& pageList = iter->second;
        const size_t nPageCount = pageList.size();
        for (size_t nPage = 0; nPage < nPageCount; ++nPage) {
            const StoragePage* pPage = pageList[nPage].get();
            if (pPage == nullptr) {
                continue;
            }
            for (size_t nSlot = 0; nSlot < kPageCells; ++nSlot) {
                if (pPage->usedMask[nSlot] != 0) {
                    subItemList.push_back(&pPage->cells[nSlot]);
                }
            }
        }
    }
    if (!subItemList.empty()) {
        ASSERT(m_pListView != nullptr);
        if (m_pListView != nullptr) {
            nMaxWidth = m_pListView->GetMaxDataItemWidth(subItemList);
        }
    }    
    return nMaxWidth;
}

size_t ListCtrlData::GetElementCount() const
{
    return GetDataItemCount();
}

void ListCtrlData::SetElementSelected(size_t nElementIndex, bool bSelected)
{
    if (nElementIndex == Box::InvalidIndex) {
        //If the selected one is the Header, ignore it
        return;
    }
    const RowId rowId = GetRowIdByDisplayIndex(nElementIndex);
    if (rowId == kInvalidRowId) {
        return;
    }
    ListCtrlItemData& rowData = m_rowDataList[rowId];
    if (rowData.bSelected != bSelected) {
        rowData.bSelected = bSelected;//Update in both multi-select and single-select modes
    }
    if (IsAutoCheckSelect() && (rowData.bChecked != rowData.bSelected)) {
        rowData.bChecked = rowData.bSelected;
    }

    if (!m_bMultiSelect) {
        //Single-select case (the row identity is recorded, so it still points to the same row after sorting/insert/delete)
        if (bSelected) {
            m_nSelectedRowId = rowId;
        }
        else if (m_nSelectedRowId == rowId) {
            m_nSelectedRowId = kInvalidRowId;
        }
    }
}

bool ListCtrlData::IsElementSelected(size_t nElementIndex) const
{
    if (nElementIndex == Box::InvalidIndex) {
        //If the selected one is the Header, ignore it
        return false;
    }
    bool bSelected = false;
    if (m_bMultiSelect) {
        //Multi-select
        const RowId rowId = GetRowIdByDisplayIndex(nElementIndex);
        if (rowId != kInvalidRowId) {
            const ListCtrlItemData& rowData = m_rowDataList[rowId];
            bSelected = rowData.bSelected;
        }
    }
    else {
        //Single-select
        const RowId rowId = GetRowIdByDisplayIndex(nElementIndex);
        bSelected = (rowId != kInvalidRowId) && (m_nSelectedRowId == rowId);
    }
    return bSelected;
}

void ListCtrlData::GetSelectedElements(std::vector<size_t>& selectedIndexs) const
{
    selectedIndexs.clear();
    if (m_bMultiSelect) {
        const size_t nCount = m_displayOrder.size();
        for (size_t nElementIndex = 0; nElementIndex < nCount; ++nElementIndex) {
            const ListCtrlItemData& rowData = m_rowDataList[m_displayOrder[nElementIndex]];
            if (rowData.bSelected) {
                selectedIndexs.push_back(nElementIndex);
            }
        }
    }
    else {
        const size_t nDisplayIndex = GetDisplayIndexByRowId(m_nSelectedRowId);
        if (nDisplayIndex != Box::InvalidIndex) {
            selectedIndexs.push_back(nDisplayIndex);
        }
    }
}

bool ListCtrlData::IsMultiSelect() const
{
    return m_bMultiSelect;
}

void ListCtrlData::SetMultiSelect(bool bMultiSelect)
{
    bool bChanged = m_bMultiSelect != bMultiSelect;
    m_bMultiSelect = bMultiSelect;
    if (bChanged && bMultiSelect) {
        //When switching from single-select to multi-select, clear the selections and keep only one selected item
        const size_t nItemCount = m_displayOrder.size();
        for (size_t itemIndex = 0; itemIndex < nItemCount; ++itemIndex) {
            const RowId rowId = m_displayOrder[itemIndex];
            ListCtrlItemData& rowData = m_rowDataList[rowId];
            if (rowData.bSelected) {
                if (m_nSelectedRowId != rowId) {
                    rowData.bSelected = false;
                }
            }
            if (IsAutoCheckSelect() && (rowData.bChecked != rowData.bSelected)) {
                rowData.bChecked = rowData.bSelected;
            }
        }
    }    
}

void ListCtrlData::SetListView(IListCtrlView* pListView)
{
    m_pListView = pListView;
}

void ListCtrlData::SetAutoCheckSelect(bool bAutoCheckSelect)
{
    m_bAutoCheckSelect = bAutoCheckSelect;
}

bool ListCtrlData::IsAutoCheckSelect() const
{
    return m_bAutoCheckSelect;
}

void ListCtrlData::SetDefaultTextStyle(int32_t nTextStyle)
{
    m_nDefaultTextStyle = nTextStyle;
}

void ListCtrlData::SetDefaultItemHeight(int32_t nItemHeight)
{
    m_nDefaultItemHeight = nItemHeight;
}

void ListCtrlData::ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale)
{
    for (ListCtrlItemData& data : m_rowDataList) {
        if (data.nItemHeight > 0) {
            data.nItemHeight = ui::TruncateToUInt16(dpiManager.GetScaleInt((int32_t)data.nItemHeight, nOldDpiScale));
        }
    }
}

void ListCtrlData::SubItemToStorage(const ListCtrlSubItemData& item, Storage& storage) const
{
    storage.text = item.text;
    if (item.nTextFormat >= 0) {
        storage.nTextFormat = TruncateToUInt16(item.nTextFormat);
    }
    else {
        storage.nTextFormat = 0;
    }
    storage.nImageId = item.nImageId;
    storage.textColor = item.textColor;
    storage.bkColor = item.bkColor;
    storage.bShowCheckBox = item.bShowCheckBox;
    storage.bChecked = item.bChecked;
    storage.bEditable = item.bEditable;

    storage.userDataN = item.userDataN;
    storage.userDataS = item.userDataS;
    storage.nSortGroup = item.nSortGroup;
    storage.nProgress = static_cast<int8_t>(item.nProgress);
    storage.bRichText = item.bRichText;
}

void ListCtrlData::StorageToSubItem(const Storage& storage, ListCtrlSubItemData& item) const
{
    item.text = storage.text.c_str();
    if (storage.nTextFormat == 0) {
        item.nTextFormat = -1;
    }
    else {
        item.nTextFormat = storage.nTextFormat;
    }
    item.nImageId = storage.nImageId;
    item.textColor = storage.textColor;
    item.bkColor = storage.bkColor;
    item.bShowCheckBox = storage.bShowCheckBox;
    item.bChecked = storage.bChecked;
    item.bEditable = storage.bEditable;

    item.userDataN = storage.userDataN;
    item.userDataS = storage.userDataS;
    item.nSortGroup = storage.nSortGroup;
    item.nProgress = static_cast<int32_t>(storage.nProgress);
    item.bRichText = storage.bRichText;
}

bool ListCtrlData::IsValidDataItemIndex(size_t itemIndex) const
{
    return itemIndex < m_displayOrder.size();
}

bool ListCtrlData::IsValidDataColumnId(size_t nColumnId) const
{
    return m_dataMap.find(nColumnId) != m_dataMap.end();
}

bool ListCtrlData::AddColumn(size_t columnId)
{
    if ((columnId == Box::InvalidIndex) || (columnId == 0)) {
        return false;
    }
    //Create the storage page list of this column (pages are lazily allocated: a page is created on the
    //first write to it, so nothing is preallocated here)
    if (m_dataMap.find(columnId) == m_dataMap.end()) {
        m_dataMap.emplace(columnId, StoragePageList());
    }
    EmitCountChanged();
    return true;
}

bool ListCtrlData::RemoveColumn(size_t columnId)
{
    auto iter = m_dataMap.find(columnId);
    if (iter != m_dataMap.end()) {
        m_dataMap.erase(iter);
        //Discard the cell selection bitmap of this column as well, so that re-adding a column with the same ID does not inherit a stale selection state
        m_cellSelected.erase(columnId);
        if (m_dataMap.empty()) {
            //If all columns are deleted, the rows are also cleared to 0
            m_displayOrder.clear();
            m_rowDataList.clear();
            m_nextRowId = 0;
            m_nSelectedRowId = kInvalidRowId;
            //All row identities are now invalid, so the cell selection state is cleared as well
            ClearCellSelection();
            m_hideRowCount = 0;
            m_heightRowCount = 0;
            m_atTopRowCount = 0;
        }
        EmitCountChanged();
        return true;
    }
    return false;
}

bool ListCtrlData::SetColumnCheck(size_t columnId, bool bChecked, bool bRefresh)
{
    bool bRet = false;
    auto iter = m_dataMap.find(columnId);
    ASSERT(iter != m_dataMap.end());
    if (iter != m_dataMap.end()) {
        StoragePageList& pageList = iter->second;
        //Only walk the live rows (in display order) to avoid rebuilding data for deleted rows (tombstones)
        const size_t nCount = m_displayOrder.size();
        for (size_t index = 0; index < nCount; ++index) {
            Storage* pStorage = GetStorageAtForWrite(pageList, m_displayOrder[index]);
            if (pStorage != nullptr) {
                pStorage->bChecked = bChecked;
            }
        }
        bRet = true;
    }
    if (bRefresh && bRet) {
        EmitCountChanged();
    }
    return bRet;
}

const ListCtrlData::Storage* ListCtrlData::GetStorageAt(const StoragePageList& pageList, RowId rowId)
{
    const size_t nPage = static_cast<size_t>(rowId) / kPageCells;
    if (nPage >= pageList.size()) {
        //The page has not been allocated yet (lazy allocation), so it is treated as having no data
        return nullptr;
    }
    const StoragePage* pPage = pageList[nPage].get();
    if (pPage == nullptr) {
        return nullptr;
    }
    const size_t nSlot = static_cast<size_t>(rowId) % kPageCells;
    if (pPage->usedMask[nSlot] == 0) {
        //The cell has not been written (tombstone), so it is treated as having no data
        return nullptr;
    }
    return &pPage->cells[nSlot];
}

ListCtrlData::Storage* ListCtrlData::GetStorageAtForWrite(StoragePageList& pageList, RowId rowId)
{
    const size_t nPage = static_cast<size_t>(rowId) / kPageCells;
    const size_t nSlot = static_cast<size_t>(rowId) % kPageCells;
    if (nPage >= pageList.size()) {
        //Lazy allocation: grow the page list only on the first write to that page (pages in between stay null pointers)
        pageList.resize(nPage + 1);
    }
    if (pageList[nPage] == nullptr) {
        //Lazy allocation: create the storage page (the cells in a page are constructed at once, contiguous within the page)
        pageList[nPage] = std::make_unique<StoragePage>();
    }
    StoragePage* pPage = pageList[nPage].get();
    pPage->usedMask[nSlot] = 1;
    return &pPage->cells[nSlot];
}

void ListCtrlData::ResetStorageAt(StoragePageList& pageList, RowId rowId)
{
    const size_t nPage = static_cast<size_t>(rowId) / kPageCells;
    if (nPage >= pageList.size()) {
        return;
    }
    StoragePage* pPage = pageList[nPage].get();
    if (pPage == nullptr) {
        return;
    }
    const size_t nSlot = static_cast<size_t>(rowId) % kPageCells;
    if (pPage->usedMask[nSlot] != 0) {
        //Make it a tombstone: clear the data and mark it as not written
        pPage->cells[nSlot] = Storage();
        pPage->usedMask[nSlot] = 0;
    }
    //Page recycling: when the page is fully tombstones (no cell is written), release the page and trim released pages at the tail to shrink the page list
    for (size_t n = 0; n < kPageCells; ++n) {
        if (pPage->usedMask[n] != 0) {
            return;
        }
    }
    pageList[nPage].reset();
    while (!pageList.empty() && (pageList.back() == nullptr)) {
        pageList.pop_back();
    }
}

const ListCtrlData::Storage* ListCtrlData::GetSubItemStorage(
    size_t itemIndex, size_t nColumnId) const
{
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    if (rowId == kInvalidRowId) {
        return nullptr;
    }
    auto iter = m_dataMap.find(nColumnId);
    ASSERT(iter != m_dataMap.end());
    if (iter == m_dataMap.end()) {
        return nullptr;
    }
    //Associated column: read the data by page/slot (an unwritten cell returns nullptr)
    return GetStorageAt(iter->second, rowId);
}

ListCtrlData::Storage* ListCtrlData::GetSubItemStorageForWrite(
    size_t itemIndex, size_t nColumnId)
{
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    if (rowId == kInvalidRowId) {
        return nullptr;
    }
    auto iter = m_dataMap.find(nColumnId);
    ASSERT(iter != m_dataMap.end());
    if (iter == m_dataMap.end()) {
        return nullptr;
    }
    //Associated column: get a writable cell by page/slot (allocate a storage page when necessary)
    return GetStorageAtForWrite(iter->second, rowId);
}

bool ListCtrlData::GetSubItemStorageList(size_t itemIndex, std::vector<ListCtrlSubItemData2Pair>& subItemList) const
{
    subItemList.clear();
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    ASSERT(rowId != kInvalidRowId);
    if (rowId == kInvalidRowId) {
        return false;
    }
    ListCtrlSubItemData2Pair dataPair;
    for (auto iter = m_dataMap.begin(); iter != m_dataMap.end(); ++iter) {
        dataPair.nColumnId = iter->first;        
        dataPair.pSubItemData = GetStorageAt(iter->second, rowId);
        subItemList.push_back(dataPair);
    }
    return true;
}

const ListCtrlItemData* ListCtrlData::GetDisplayItemData(size_t itemIndex) const
{
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    if (rowId == kInvalidRowId) {
        return nullptr;
    }
    return &m_rowDataList[rowId];
}

ListCtrlData::RowId ListCtrlData::GetRowIdByDisplayIndex(size_t itemIndex) const
{
    if (itemIndex == Box::InvalidIndex) {
        return kInvalidRowId;
    }
    if (itemIndex >= m_displayOrder.size()) {
        return kInvalidRowId;
    }
    return m_displayOrder[itemIndex];
}

size_t ListCtrlData::GetDisplayIndexByRowId(RowId rowId) const
{
    if (rowId == kInvalidRowId) {
        return Box::InvalidIndex;
    }
    const size_t nCount = m_displayOrder.size();
    for (size_t index = 0; index < nCount; ++index) {
        if (m_displayOrder[index] == rowId) {
            return index;
        }
    }
    return Box::InvalidIndex;
}

ListCtrlData::RowId ListCtrlData::AllocRowId()
{
    const RowId rowId = m_nextRowId;
    ++m_nextRowId;
    //Only the row attribute pool grows to match the number of row identities; the column data pools are paged and lazily allocated, so nothing needs to grow here
    if (m_rowDataList.size() < m_nextRowId) {
        m_rowDataList.resize(m_nextRowId);
    }
    //The cell selection bitmap is indexed by row identity, so grow it in sync (new rows are unselected by default)
    for (auto iter = m_cellSelected.begin(); iter != m_cellSelected.end(); ++iter) {
        std::vector<uint8_t>& bitmap = iter->second;
        if (bitmap.size() < m_nextRowId) {
            bitmap.resize(m_nextRowId, static_cast<uint8_t>(0));
        }
    }
    return rowId;
}

void ListCtrlData::FreeRowId(RowId rowId)
{
    if (rowId >= m_rowDataList.size()) {
        return;
    }
    //Fix the counters of the personalized data
    const ListCtrlItemData oldData = m_rowDataList[rowId];
    if (!oldData.bVisible) {
        m_hideRowCount -= 1;
        ASSERT(m_hideRowCount >= 0);
    }
    if (oldData.nItemHeight >= 0) {
        m_heightRowCount -= 1;
        ASSERT(m_heightRowCount >= 0);
    }
    if (oldData.nAlwaysAtTop >= 0) {
        m_atTopRowCount -= 1;
        ASSERT(m_atTopRowCount >= 0);
    }
    //Make the row attributes a tombstone
    m_rowDataList[rowId] = ListCtrlItemData();
    //Make the cell a tombstone in each column (clear the cell and mark it as not written; the storage page itself is not released)
    for (auto iter = m_dataMap.begin(); iter != m_dataMap.end(); ++iter) {
        ResetStorageAt(iter->second, rowId);
    }
    //Fix the single-selection state
    if (m_nSelectedRowId == rowId) {
        m_nSelectedRowId = kInvalidRowId;
    }
    //Cell selection: clear the selected bits of the deleted row; if the anchor/current cell is in the deleted row, invalidate it
    for (auto iter = m_cellSelected.begin(); iter != m_cellSelected.end(); ++iter) {
        std::vector<uint8_t>& bitmap = iter->second;
        if (rowId < bitmap.size()) {
            bitmap[rowId] = 0;
        }
    }
    if (m_anchorCell.IsValid() && (m_anchorCell.nRowId == rowId)) {
        m_anchorCell = CellIdentity();
    }
    if (m_currentCell.IsValid() && (m_currentCell.nRowId == rowId)) {
        m_currentCell = CellIdentity();
    }
}

void ListCtrlData::AssertDataConsistency() const
{
#ifdef _DEBUG
    //1. All row identities in the display order list are valid and unique, and their row attributes are reachable
    std::set<RowId> rowIdSet;
    for (RowId rowId : m_displayOrder) {
        ASSERT(rowId < m_nextRowId);
        ASSERT(rowId < m_rowDataList.size());
        ASSERT(rowIdSet.find(rowId) == rowIdSet.end());
        rowIdSet.insert(rowId);
    }
    //2. In the paged storage of each column, a written cell must belong to a live row identity; and the page index/offset conversion must be consistent
    for (auto iter = m_dataMap.begin(); iter != m_dataMap.end(); ++iter) {
        const StoragePageList& pageList = iter->second;
        const size_t nPageCount = pageList.size();
        for (size_t nPage = 0; nPage < nPageCount; ++nPage) {
            const StoragePage* pPage = pageList[nPage].get();
            if (pPage == nullptr) {
                continue;
            }
            for (size_t nSlot = 0; nSlot < kPageCells; ++nSlot) {
                if (pPage->usedMask[nSlot] != 0) {
                    const RowId rowId = static_cast<RowId>(nPage * kPageCells + nSlot);
                    ASSERT(rowId < m_nextRowId);
                    ASSERT(rowIdSet.find(rowId) != rowIdSet.end());
                }
            }
        }
    }
    //3. The length of the row attribute pool matches the number of allocated row identities
    ASSERT(m_rowDataList.size() == m_nextRowId);
    //4. When the single-selection state is valid, its row identity must exist in the display order list
    if (!m_bMultiSelect && (m_nSelectedRowId != kInvalidRowId)) {
        ASSERT(rowIdSet.find(m_nSelectedRowId) != rowIdSet.end());
    }
    //5. The counters of the personalized data match the result of scanning the live rows
    int32_t nHideCount = 0;
    int32_t nHeightCount = 0;
    int32_t nAtTopCount = 0;
    for (RowId rowId : rowIdSet) {
        const ListCtrlItemData& rowData = m_rowDataList[rowId];
        if (!rowData.bVisible) {
            ++nHideCount;
        }
        if (rowData.nItemHeight >= 0) {
            ++nHeightCount;
        }
        if (rowData.nAlwaysAtTop >= 0) {
            ++nAtTopCount;
        }
    }
    ASSERT(nHideCount == m_hideRowCount);
    ASSERT(nHeightCount == m_heightRowCount);
    ASSERT(nAtTopCount == m_atTopRowCount);
#endif
}

bool ListCtrlData::IsNormalMode() const
{
    ASSERT((m_hideRowCount >= 0) && (m_heightRowCount >= 0) && (m_atTopRowCount >= 0));
    return (m_hideRowCount == 0) && (m_heightRowCount == 0) && (m_atTopRowCount == 0);
}

const std::vector<uint8_t>* ListCtrlData::FindCellBitmap(size_t nColumnId) const
{
    auto iter = m_cellSelected.find(nColumnId);
    if (iter == m_cellSelected.end()) {
        return nullptr;
    }
    return &(iter->second);
}

std::vector<uint8_t>& ListCtrlData::GetCellBitmapForWrite(size_t nColumnId, RowId rowId)
{
    std::vector<uint8_t>& bitmap = m_cellSelected[nColumnId];
    const size_t nNeed = static_cast<size_t>(rowId) + 1;
    if (bitmap.size() < nNeed) {
        bitmap.resize(nNeed, static_cast<uint8_t>(0));
    }
    return bitmap;
}

void ListCtrlData::SetCellSelectedByRowId(RowId rowId, size_t nColumnId, bool bSelected)
{
    if ((rowId == kInvalidRowId) || !IsValidDataColumnId(nColumnId)) {
        return;
    }
    std::vector<uint8_t>& bitmap = GetCellBitmapForWrite(nColumnId, rowId);
    bitmap[rowId] = bSelected ? 1 : 0;
}

bool ListCtrlData::IsCellSelectedByRowId(RowId rowId, size_t nColumnId) const
{
    if (rowId == kInvalidRowId) {
        return false;
    }
    const std::vector<uint8_t>* pBitmap = FindCellBitmap(nColumnId);
    if ((pBitmap == nullptr) || (rowId >= pBitmap->size())) {
        return false;
    }
    return (*pBitmap)[rowId] != 0;
}

ListCtrlCellId ListCtrlData::GetCurrentCell() const
{
    ListCtrlCellId cell;
    if (m_currentCell.IsValid()) {
        const size_t nDisplayIndex = GetDisplayIndexByRowId(m_currentCell.nRowId);
        if (nDisplayIndex != Box::InvalidIndex) {
            cell.nRowIndex = nDisplayIndex;
            cell.nColumnId = m_currentCell.nColumnId;
        }
    }
    return cell;
}

ListCtrlCellId ListCtrlData::GetAnchorCell() const
{
    ListCtrlCellId cell;
    if (m_anchorCell.IsValid()) {
        const size_t nDisplayIndex = GetDisplayIndexByRowId(m_anchorCell.nRowId);
        if (nDisplayIndex != Box::InvalidIndex) {
            cell.nRowIndex = nDisplayIndex;
            cell.nColumnId = m_anchorCell.nColumnId;
        }
    }
    return cell;
}

bool ListCtrlData::SetCurrentCell(const ListCtrlCellId& cell)
{
    if (!IsValidDataItemIndex(cell.nRowIndex) || !IsValidDataColumnId(cell.nColumnId)) {
        return false;
    }
    //Convert the "display index" to a "row identity" for storage, so the selection state is decoupled from the display order (it still follows the original data row after sorting/insert/delete)
    const RowId rowId = GetRowIdByDisplayIndex(cell.nRowIndex);
    if (rowId == kInvalidRowId) {
        return false;
    }
    m_currentCell.nRowId = rowId;
    m_currentCell.nColumnId = cell.nColumnId;
    return true;
}

bool ListCtrlData::SetAnchorCell(const ListCtrlCellId& cell)
{
    if (!IsValidDataItemIndex(cell.nRowIndex) || !IsValidDataColumnId(cell.nColumnId)) {
        return false;
    }
    const RowId rowId = GetRowIdByDisplayIndex(cell.nRowIndex);
    if (rowId == kInvalidRowId) {
        return false;
    }
    m_anchorCell.nRowId = rowId;
    m_anchorCell.nColumnId = cell.nColumnId;
    return true;
}

void ListCtrlData::GetCellSelection(ListCtrlCellId& anchorCell, ListCtrlCellId& focusCell) const
{
    anchorCell = GetAnchorCell();
    focusCell = GetCurrentCell();
}

bool ListCtrlData::IsCellSelected(size_t nRowIndex, size_t nColumnId) const
{
    return IsCellSelectedByRowId(GetRowIdByDisplayIndex(nRowIndex), nColumnId);
}

bool ListCtrlData::SetCellSelected(size_t nRowIndex, size_t nColumnId, bool bSelected)
{
    if (!IsValidDataItemIndex(nRowIndex) || !IsValidDataColumnId(nColumnId)) {
        return false;
    }
    if (!IsSelectableElement(nRowIndex)) {
        //Always-on-top rows and invisible rows do not participate in cell selection
        return false;
    }
    const RowId rowId = GetRowIdByDisplayIndex(nRowIndex);
    if (rowId == kInvalidRowId) {
        return false;
    }
    if (IsCellSelectedByRowId(rowId, nColumnId) == bSelected) {
        //The selection state did not change: return false so the caller does not refresh again or dispatch a selection change event
        return false;
    }
    SetCellSelectedByRowId(rowId, nColumnId, bSelected);
    return true;
}

void ListCtrlData::SetCellSelectionRect(size_t nRowBegin, size_t nRowEnd,
                                        const std::vector<size_t>& columnIdList, bool bAdditive)
{
    if (!bAdditive) {
        //Replace mode: only clear the selected set; the anchor/current cell are maintained by the caller (the anchor must stay unchanged while dragging/extending a range)
        m_cellSelected.clear();
    }
    const size_t nCount = m_displayOrder.size();
    if ((nCount == 0) || columnIdList.empty() ||
        (nRowBegin == Box::InvalidIndex) || (nRowEnd == Box::InvalidIndex)) {
        return;
    }
    if (nRowBegin > nRowEnd) {
        const size_t nTemp = nRowBegin;
        nRowBegin = nRowEnd;
        nRowEnd = nTemp;
    }
    if (nRowEnd >= nCount) {
        nRowEnd = nCount - 1;
    }
    //Collect the selected cells by row identity: the selection state still follows the original data row after sorting/insert/delete
    for (size_t nRowIndex = nRowBegin; nRowIndex <= nRowEnd; ++nRowIndex) {
        if (!IsSelectableElement(nRowIndex)) {
            //Always-on-top rows and invisible rows do not participate in cell selection
            continue;
        }
        const RowId rowId = m_displayOrder[nRowIndex];
        for (size_t nColumnId : columnIdList) {
            SetCellSelectedByRowId(rowId, nColumnId, true);
        }
    }
}

void ListCtrlData::SelectAllCells(const std::vector<size_t>& columnIdList)
{
    const size_t nCount = m_displayOrder.size();
    if ((nCount == 0) || columnIdList.empty()) {
        ClearCellSelection();
        return;
    }
    SetCellSelectionRect(0, nCount - 1, columnIdList, false);
}

bool ListCtrlData::ClearCellSelection()
{
    //Determine whether a selection state really exists before clearing: the bitmap may be stale (a cell was selected and then deselected, so the bitmap still exists but all its bits are 0),
    //so the bitmap content must be walked to confirm it; a non-empty bitmap container alone is not enough
    bool bHasSelectedCell = false;
    for (auto iter = m_cellSelected.begin(); (iter != m_cellSelected.end()) && !bHasSelectedCell; ++iter) {
        const std::vector<uint8_t>& bitmap = iter->second;
        for (size_t nIndex = 0; nIndex < bitmap.size(); ++nIndex) {
            if (bitmap[nIndex] != 0) {
                bHasSelectedCell = true;
                break;
            }
        }
    }
    //The anchor is only used for range extension and has no standalone visual, so it does not count as a change by itself; the current cell has a focus frame, so it is a state change
    const bool bChanged = bHasSelectedCell || m_currentCell.IsValid();

    m_anchorCell = CellIdentity();
    m_currentCell = CellIdentity();
    m_cellSelected.clear();
    return bChanged;
}

size_t ListCtrlData::GetDataItemCount() const
{
    return m_displayOrder.size();
}

bool ListCtrlData::SetDataItemCount(size_t itemCount)
{
    if (itemCount == Box::InvalidIndex) {
        return false;
    }
    const size_t nOldCount = m_displayOrder.size();
    if (itemCount == nOldCount) {
        //No change
        return true;
    }
    if (itemCount < nOldCount) {
        //The number of rows decreased: release the row identities being cut (the counters are fixed
        //inside FreeRowId)
        for (size_t index = itemCount; index < nOldCount; ++index) {
            FreeRowId(m_displayOrder[index]);
        }
        m_displayOrder.resize(itemCount);
    }
    else {
        //The number of rows increased: allocate a row identity for each new display position
        m_displayOrder.reserve(itemCount);
        for (size_t index = nOldCount; index < itemCount; ++index) {
            m_displayOrder.push_back(AllocRowId());
        }
    }
    AssertDataConsistency();
    EmitCountChanged();
    return true;
}

void ListCtrlData::UpdateNormalMode()
{
    m_hideRowCount = 0;
    m_heightRowCount = 0;
    m_atTopRowCount = 0;
    const size_t nCount = m_displayOrder.size();
    for (size_t index = 0; index < nCount; ++index) {
        const ListCtrlItemData& data = m_rowDataList[m_displayOrder[index]];
        if (!data.bVisible) {
            m_hideRowCount += 1;
        }
        if (data.nItemHeight >= 0) {
            m_heightRowCount += 1;
        }
        if (data.nAlwaysAtTop >= 0) {
            m_atTopRowCount += 1;
        }
    }
}

size_t ListCtrlData::AddDataItem(size_t columnId, const ListCtrlSubItemData& dataItem)
{
    if (!IsValidDataColumnId(columnId)) {
        return Box::InvalidIndex;
    }

    Storage storage;
    SubItemToStorage(dataItem, storage);

    //Allocate a new row identity (the row attribute pool grows with it, the column data itself is not moved)
    const RowId rowId = AllocRowId();

    //Associated column: write the data (the other columns stay empty)
    auto iter = m_dataMap.find(columnId);
    if (iter != m_dataMap.end()) {
        Storage* pStorage = GetStorageAtForWrite(iter->second, rowId);
        if (pStorage != nullptr) {
            *pStorage = storage;
        }
    }

    //Append to the end of the display order list
    m_displayOrder.push_back(rowId);
    AssertDataConsistency();
    EmitCountChanged();
    return (m_displayOrder.size() - 1);
}

bool ListCtrlData::InsertDataItem(size_t itemIndex, size_t columnId, const ListCtrlSubItemData& dataItem)
{
    if (!IsValidDataColumnId(columnId)) {
        return false;
    }
    if (!IsValidDataItemIndex(itemIndex)) {
        //If the index is invalid, add the data by appending
        return (AddDataItem(columnId, dataItem) != Box::InvalidIndex);
    }

    Storage storage;
    SubItemToStorage(dataItem, storage);

    //Allocate a new row identity (the row attribute pool grows with it, the column data itself is not moved)
    const RowId rowId = AllocRowId();

    //Associated column: write the data
    auto iter = m_dataMap.find(columnId);
    if (iter != m_dataMap.end()) {
        Storage* pStorage = GetStorageAtForWrite(iter->second, rowId);
        if (pStorage != nullptr) {
            *pStorage = storage;
        }
    }

    //Insert the new row identity only into the display order list (the single-selection state follows the row identity and needs no adjustment)
    ASSERT(itemIndex <= m_displayOrder.size());
    m_displayOrder.insert(m_displayOrder.begin() + itemIndex, rowId);
    AssertDataConsistency();
    EmitCountChanged();
    return true;
}

bool ListCtrlData::DeleteDataItem(size_t itemIndex)
{
    if (!IsValidDataItemIndex(itemIndex)) {
        //Invalid index
        return false;
    }

    //Release the data associated with this row identity (column pools become tombstones, the counters and
    //the single-selection state are fixed), then remove it from the display order list
    const RowId rowId = m_displayOrder[itemIndex];
    FreeRowId(rowId);
    m_displayOrder.erase(m_displayOrder.begin() + itemIndex);
    AssertDataConsistency();
    EmitCountChanged();
    return true;
}

bool ListCtrlData::DeleteAllDataItems()
{
    bool bDeleted = !m_displayOrder.empty();
    //Clear the display order list
    m_displayOrder.clear();
    //Clear the paged storage of each column (release all storage pages)
    for (auto iter = m_dataMap.begin(); iter != m_dataMap.end(); ++iter) {
        iter->second.clear();
    }
    //Clear the row attribute pool and reset the row identity generator
    m_rowDataList.clear();
    m_nextRowId = 0;
    m_nSelectedRowId = kInvalidRowId;
    //All row identities are now invalid, so the cell selection state is cleared as well
    ClearCellSelection();
    m_hideRowCount = 0;
    m_heightRowCount = 0;
    m_atTopRowCount = 0;

    if (bDeleted) {
        EmitCountChanged();
    }
    return bDeleted;
}

size_t ListCtrlData::DeleteDataItems(const std::vector<size_t>& displayIndexs)
{
    const size_t nCount = m_displayOrder.size();
    if ((nCount == 0) || displayIndexs.empty()) {
        return 0;
    }
    //Collect the rows to delete by row identity (de-duplicated; out-of-range indexes are ignored)
    std::set<RowId> rowIdSet;
    for (size_t index : displayIndexs) {
        if (index < nCount) {
            rowIdSet.insert(m_displayOrder[index]);
        }
    }
    if (rowIdSet.empty()) {
        return 0;
    }

    //Release the data associated with the row identities to delete (column pools become tombstones, personalized counters and the single-selection state are fixed)
    for (RowId rowId : rowIdSet) {
        FreeRowId(rowId);
    }

    //Single-pass compaction of the display order list: move the live row identities forward, O(N) 4-byte moves (avoiding the O(K*N) move cost of erasing row by row)
    size_t nWriteIndex = 0;
    for (size_t nReadIndex = 0; nReadIndex < nCount; ++nReadIndex) {
        const RowId rowId = m_displayOrder[nReadIndex];
        if (rowIdSet.find(rowId) == rowIdSet.end()) {
            m_displayOrder[nWriteIndex] = rowId;
            ++nWriteIndex;
        }
    }
    m_displayOrder.resize(nWriteIndex);

    AssertDataConsistency();
    EmitCountChanged();
    return rowIdSet.size();
}

void ListCtrlData::CompactStorage()
{
    const size_t nCount = m_displayOrder.size();
    //Record the display position of the selected row before compaction (it must be remapped after the row identities are renumbered)
    size_t nSelectedIndex = Box::InvalidIndex;
    if (m_nSelectedRowId != kInvalidRowId) {
        nSelectedIndex = GetDisplayIndexByRowId(m_nSelectedRowId);
    }
    //Also record the display positions of the cell selection anchor/current cell, and restore them with the new row identities after renumbering
    size_t nAnchorIndex = Box::InvalidIndex;
    if (m_anchorCell.IsValid()) {
        nAnchorIndex = GetDisplayIndexByRowId(m_anchorCell.nRowId);
    }
    size_t nCurrentIndex = Box::InvalidIndex;
    if (m_currentCell.IsValid()) {
        nCurrentIndex = GetDisplayIndexByRowId(m_currentCell.nRowId);
    }

    //1. Rebuild the row attribute pool: the new row identity = display position (packed in display order, tombstone slots dropped)
    RowDataList newRowDataList;
    newRowDataList.resize(nCount);
    for (size_t index = 0; index < nCount; ++index) {
        newRowDataList[index] = m_rowDataList[m_displayOrder[index]];
    }
    m_rowDataList.swap(newRowDataList);

    //2. Rebuild the storage pages of each column: pack them by the new row identity and move only the written cells (unwritten ones do not allocate a page)
    for (auto iter = m_dataMap.begin(); iter != m_dataMap.end(); ++iter) {
        StoragePageList& pageList = iter->second;
        StoragePageList newPageList;
        for (size_t index = 0; index < nCount; ++index) {
            const Storage* pOld = GetStorageAt(pageList, m_displayOrder[index]);
            if (pOld == nullptr) {
                continue;
            }
            Storage* pNew = GetStorageAtForWrite(newPageList, static_cast<RowId>(index));
            *pNew = *pOld;
        }
        //After the swap, the old page list is destroyed together with newPageList (reclaiming the memory occupied by deleted rows)
        pageList.swap(newPageList);
    }

    //2b. Remap the cell selection bitmap: the new row identity = display position; move the selected bits in display order
    for (auto iter = m_cellSelected.begin(); iter != m_cellSelected.end(); ++iter) {
        const std::vector<uint8_t>& oldBits = iter->second;
        std::vector<uint8_t> newBits(nCount, static_cast<uint8_t>(0));
        for (size_t index = 0; index < nCount; ++index) {
            const RowId oldRowId = m_displayOrder[index];
            if ((oldRowId < oldBits.size()) && (oldBits[oldRowId] != 0)) {
                newBits[index] = static_cast<uint8_t>(1);
            }
        }
        iter->second.swap(newBits);
    }

    //3. The display order list becomes an identity mapping; the row identity generator is reset
    for (size_t index = 0; index < nCount; ++index) {
        m_displayOrder[index] = static_cast<RowId>(index);
    }
    m_nextRowId = static_cast<RowId>(nCount);

    //4. Restore the selected row identity (the display position is unchanged, the new row identity = that display position)
    if (nSelectedIndex != Box::InvalidIndex) {
        m_nSelectedRowId = static_cast<RowId>(nSelectedIndex);
    }
    //Restore the cell selection anchor/current cell (the row identities were renumbered)
    if (nAnchorIndex != Box::InvalidIndex) {
        m_anchorCell.nRowId = static_cast<RowId>(nAnchorIndex);
    }
    else {
        m_anchorCell = CellIdentity();
    }
    if (nCurrentIndex != Box::InvalidIndex) {
        m_currentCell.nRowId = static_cast<RowId>(nCurrentIndex);
    }
    else {
        m_currentCell = CellIdentity();
    }

    AssertDataConsistency();
}

bool ListCtrlData::SetDataItemData(size_t itemIndex, const ListCtrlItemData& itemData,
                                   bool& bChanged, bool& bCheckChanged)
{
    bChanged = false;
    bCheckChanged = false;
    bool bCountChanged = false;
    bool bItemChanged = false;
    bool bRet = false;
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    ASSERT(rowId != kInvalidRowId);
    if (rowId != kInvalidRowId) {
        const ListCtrlItemData oldItemData = m_rowDataList[rowId];
        m_rowDataList[rowId] = itemData;
        if (m_nDefaultItemHeight == m_rowDataList[rowId].nItemHeight) {
            //If it equals the default height, set it to the flag value
            m_rowDataList[rowId].nItemHeight = -1;
        }
        const ListCtrlItemData& newItemData = m_rowDataList[rowId];
        if (newItemData.bSelected != oldItemData.bSelected) {
            bChanged = true;
            bItemChanged = true;
        }
        else if (newItemData.bChecked != oldItemData.bChecked) {
            bCheckChanged = true;
            bChanged = true;
            bItemChanged = true;
        }
        else if (newItemData.bVisible != oldItemData.bVisible) {
            bChanged = true;
            bCountChanged = true;
        }
        else if (newItemData.nAlwaysAtTop != oldItemData.nAlwaysAtTop) {
            bChanged = true;
            bCountChanged = true;
        }
        else if (newItemData.nItemHeight != oldItemData.nItemHeight) {
            bChanged = true;
            bCountChanged = true;
        }
        else if (newItemData.nUserData != oldItemData.nUserData) {
            bChanged = true;
        }

        //Update the count
        if (!oldItemData.bVisible && newItemData.bVisible) {
            m_hideRowCount -= 1;            
        }
        else if (oldItemData.bVisible && !newItemData.bVisible) {
            m_hideRowCount += 1;
        }
        ASSERT(m_hideRowCount >= 0);

        if ((oldItemData.nItemHeight >= 0) && (newItemData.nItemHeight < 0)) {
            m_heightRowCount -= 1;
        }
        else if ((oldItemData.nItemHeight < 0) && (newItemData.nItemHeight >= 0)) {
            m_heightRowCount += 1;
        }
        ASSERT(m_heightRowCount >= 0);

        if ((oldItemData.nAlwaysAtTop >= 0) && (newItemData.nAlwaysAtTop < 0)) {
            m_atTopRowCount -= 1;
        }
        else if ((oldItemData.nAlwaysAtTop < 0) && (newItemData.nAlwaysAtTop >= 0)) {
            m_atTopRowCount += 1;
        }
        ASSERT(m_atTopRowCount >= 0);
        bRet = true;
    }
    AssertDataConsistency();
    if (bCountChanged) {
        EmitCountChanged();
    }
    else if (bItemChanged) {
        EmitDataChanged(itemIndex, itemIndex);
    }
    return bRet;
}

bool ListCtrlData::GetDataItemData(size_t itemIndex, ListCtrlItemData& itemData) const
{
    bool bRet = false;
    itemData = ListCtrlItemData();
    const ListCtrlItemData* pRowData = GetDisplayItemData(itemIndex);
    ASSERT(pRowData != nullptr);
    if (pRowData != nullptr) {
        itemData = *pRowData;
        bRet = true;
    }
    return bRet;
}

bool ListCtrlData::SetDataItemVisible(size_t itemIndex, bool bVisible, bool& bChanged)
{
    bChanged = false;
    bool bRet = false;
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    ASSERT(rowId != kInvalidRowId);
    if (rowId != kInvalidRowId) {
        ListCtrlItemData& rowData = m_rowDataList[rowId];
        bool bOldVisible = rowData.bVisible;
        bChanged = rowData.bVisible != bVisible;
        rowData.bVisible = bVisible;

        if (!bOldVisible && bVisible) {
            m_hideRowCount -= 1;
        }
        else if (bOldVisible && !bVisible) {
            m_hideRowCount += 1;
        }
        ASSERT(m_hideRowCount >= 0);
        bRet = true;
    }
    AssertDataConsistency();
    if (bChanged) {
        EmitCountChanged();
    }
    return bRet;
}

bool ListCtrlData::IsDataItemVisible(size_t itemIndex) const
{
    bool bValue = false;
    const ListCtrlItemData* pRowData = GetDisplayItemData(itemIndex);
    ASSERT(pRowData != nullptr);
    if (pRowData != nullptr) {
        bValue = pRowData->bVisible;
    }
    return bValue;
}

bool ListCtrlData::SetDataItemSelected(size_t itemIndex, bool bSelected, bool& bChanged)
{
    bChanged = false;
    if (itemIndex >= m_displayOrder.size()) {
        return false;
    }
    bChanged = IsDataItemSelected(itemIndex) != bSelected;
    SetElementSelected(itemIndex, bSelected);
    if (bChanged) {
        EmitDataChanged(itemIndex, itemIndex);
    }
    return true;
}

bool ListCtrlData::IsDataItemSelected(size_t itemIndex) const
{
    return IsElementSelected(itemIndex);
}

bool ListCtrlData::SetDataItemChecked(size_t itemIndex, bool bChecked, bool& bChanged)
{
    bChanged = false;
    bool bRet = false;
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    ASSERT(rowId != kInvalidRowId);
    if (rowId != kInvalidRowId) {
        ListCtrlItemData& rowData = m_rowDataList[rowId];
        if (rowData.bChecked != bChecked) {
            bChanged = true;
            rowData.bChecked = bChecked;
        }        
        bRet = true;
    }
    //Do not refresh; the caller determines whether a refresh is needed
    return bRet;
}

bool ListCtrlData::IsDataItemChecked(size_t itemIndex) const
{
    bool bChecked = false;
    const ListCtrlItemData* pRowData = GetDisplayItemData(itemIndex);
    ASSERT(pRowData != nullptr);
    if (pRowData != nullptr) {
        bChecked = pRowData->bChecked;
    }
    return bChecked;
}

bool ListCtrlData::SetAllDataItemsCheck(bool bChecked)
{
    bool bChanged = false;
    size_t nCount = m_displayOrder.size();
    for (size_t itemIndex = 0; itemIndex < nCount; ++itemIndex) {
        ListCtrlItemData& rowData = m_rowDataList[m_displayOrder[itemIndex]];
        if (rowData.bChecked != bChecked) {
            rowData.bChecked = bChecked;
            bChanged = true;
        }
    }
    if (bChanged) {
        EmitCountChanged();
    }
    return bChanged;
}

void ListCtrlData::SetCheckedDataItems(const std::vector<size_t>& itemIndexs,
                                               bool bClearOthers,
                                               std::vector<size_t>& refreshIndexs)
{
    refreshIndexs.clear();
    const size_t nCount = m_displayOrder.size();
    if (!bClearOthers) {
        for (size_t itemIndex : itemIndexs) {
            if (itemIndex < nCount) {
                ListCtrlItemData& rowData = m_rowDataList[m_displayOrder[itemIndex]];
                if (!rowData.bChecked) {
                    rowData.bChecked = true;
                    refreshIndexs.push_back(itemIndex);
                }
            }
        }
    }
    else {
        std::set<size_t> indexSet;
        for (size_t itemIndex : itemIndexs) {
            if (itemIndex < nCount) {
                indexSet.insert(itemIndex);
            }
        }

        for (size_t itemIndex = 0; itemIndex < nCount; ++itemIndex) {
            ListCtrlItemData& rowData = m_rowDataList[m_displayOrder[itemIndex]];
            if (indexSet.find(itemIndex) != indexSet.end()) {
                if (!rowData.bChecked) {
                    rowData.bChecked = true;
                    refreshIndexs.push_back(itemIndex);
                }
            }
            else if (rowData.bChecked) {
                rowData.bChecked = false;
                refreshIndexs.push_back(itemIndex);
            }
        }
    }
}

void ListCtrlData::GetCheckedDataItems(std::vector<size_t>& itemIndexs) const
{
    itemIndexs.clear();
    const size_t nCount = m_displayOrder.size();
    for (size_t itemIndex = 0; itemIndex < nCount; ++itemIndex) {
        const ListCtrlItemData& rowData = m_rowDataList[m_displayOrder[itemIndex]];
        if (rowData.bChecked) {
            itemIndexs.push_back(itemIndex);
        }
    }
}

void ListCtrlData::GetDataItemsCheckStatus(bool& bChecked, bool& bPartChecked) const
{
    bChecked = false;
    bPartChecked = false;
    size_t nCheckCount = 0;
    size_t nUnCheckCount = 0;
    const size_t nCount = m_displayOrder.size();
    if (nCount == 0) {
        return;
    }
    for (size_t itemIndex = 0; itemIndex < nCount; ++itemIndex) {
        const ListCtrlItemData& rowData = m_rowDataList[m_displayOrder[itemIndex]];
        if (!rowData.bVisible) {
            continue;
        }
        if (rowData.bChecked) {
            nCheckCount++;
        }
        else {
            nUnCheckCount++;
        }
        if ((nCheckCount > 0) && (nUnCheckCount > 0)){
            //Confirmed to be a partial selection
            bChecked = true;
            bPartChecked = true;
            return;
        }
    }
    if ((nCheckCount > 0) && (nUnCheckCount == 0)) {
        bChecked = true;
    }
    else if ((nCheckCount == 0) && (nUnCheckCount > 0)) {
        bChecked = false;
    }
    else {
        ASSERT(0);
    }
}

void ListCtrlData::GetDataItemsSelectStatus(bool& bSelected, bool& bPartSelected) const
{
    bSelected = false;
    bPartSelected = false;
    size_t nSelectCount = 0;
    size_t nUnSelectCount = 0;
    const size_t nCount = m_displayOrder.size();
    if (nCount == 0) {
        return;
    }
    for (size_t itemIndex = 0; itemIndex < nCount; ++itemIndex) {
        const ListCtrlItemData& rowData = m_rowDataList[m_displayOrder[itemIndex]];
        if (!rowData.bVisible) {
            continue;
        }
        if (rowData.bSelected) {
            nSelectCount++;
        }
        else {
            nUnSelectCount++;
        }
        if ((nSelectCount > 0) && (nUnSelectCount > 0)) {
            //Confirmed to be a partial selection
            bSelected = true;
            bPartSelected = true;
            return;
        }
    }
    if ((nSelectCount > 0) && (nUnSelectCount == 0)) {
        bSelected = true;
    }
    else if ((nSelectCount == 0) && (nUnSelectCount > 0)) {
        bSelected = false;
    }
    else {
        ASSERT(0);
    }
}

void ListCtrlData::GetColumnCheckStatus(size_t columnId, bool& bChecked, bool& bPartChecked) const
{
    bChecked = false;
    bPartChecked = false;
    auto iter = m_dataMap.find(columnId);
    if (iter == m_dataMap.end()) {
        return;
    }
    const StoragePageList& pageList = iter->second;
    size_t nCheckCount = 0;
    size_t nUnCheckCount = 0;
    //Walk the live rows in display order
    const size_t nCount = m_displayOrder.size();
    if (nCount == 0) {
        return;
    }

    for (size_t itemIndex = 0; itemIndex < nCount; ++itemIndex) {
        const RowId rowId = m_displayOrder[itemIndex];
        const ListCtrlItemData& rowData = m_rowDataList[rowId];
        if (!rowData.bVisible) {
            continue;
        }
        const Storage* pStorage = GetStorageAt(pageList, rowId);
        if (pStorage == nullptr) {
            continue;
        }
        if (!pStorage->bShowCheckBox) {
            continue;
        }
        if (pStorage->bChecked) {
            nCheckCount++;
        }
        else {
            nUnCheckCount++;
        }
        if ((nCheckCount > 0) && (nUnCheckCount > 0)) {
            //Confirmed to be a partial selection
            bChecked = true;
            bPartChecked = true;
            return;
        }
    }
    if ((nCheckCount > 0) && (nUnCheckCount == 0)) {
        bChecked = true;
    }
    else if ((nCheckCount == 0) && (nUnCheckCount > 0)) {
        bChecked = false;
    }
}

bool ListCtrlData::SetDataItemImageId(size_t itemIndex, int32_t imageId, bool& bChanged)
{
    bChanged = false;
    bool bRet = false;
    if (imageId < -1) {
        imageId = -1;
    }
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    ASSERT(rowId != kInvalidRowId);
    if (rowId != kInvalidRowId) {
        ListCtrlItemData& rowData = m_rowDataList[rowId];
        if (rowData.nImageId != imageId) {
            rowData.nImageId = imageId;
            bChanged = true;
        }
        bRet = true;
    }
    //Do not refresh; the caller determines whether a refresh is needed
    return bRet;
}

int32_t ListCtrlData::GetDataItemImageId(size_t itemIndex) const
{
    int32_t imageId = -1;
    const ListCtrlItemData* pRowData = GetDisplayItemData(itemIndex);
    ASSERT(pRowData != nullptr);
    if (pRowData != nullptr) {
        imageId = pRowData->nImageId;
    }
    return imageId;
}

bool ListCtrlData::SetDataItemAlwaysAtTop(size_t itemIndex, int8_t nAlwaysAtTop, bool& bChanged)
{
    bChanged = false;
    bool bRet = false;
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    ASSERT(rowId != kInvalidRowId);
    if (rowId != kInvalidRowId) {
        ListCtrlItemData& rowData = m_rowDataList[rowId];
        int8_t nOldAlwaysAtTop = rowData.nAlwaysAtTop;
        bChanged = rowData.nAlwaysAtTop != nAlwaysAtTop;
        rowData.nAlwaysAtTop = nAlwaysAtTop;
        if ((nOldAlwaysAtTop >= 0) && (nAlwaysAtTop < 0)) {
            m_atTopRowCount -= 1;
        }
        else if ((nOldAlwaysAtTop < 0) && (nAlwaysAtTop >= 0)) {
            m_atTopRowCount += 1;
        }
        ASSERT(m_atTopRowCount >= 0);
        bRet = true;
    }
    AssertDataConsistency();
    //Do not refresh; the caller determines whether a refresh is needed
    return bRet;
}

int8_t ListCtrlData::GetDataItemAlwaysAtTop(size_t itemIndex) const
{
    int8_t nValue = -1;
    const ListCtrlItemData* pRowData = GetDisplayItemData(itemIndex);
    ASSERT(pRowData != nullptr);
    if (pRowData != nullptr) {
        nValue = pRowData->nAlwaysAtTop;
    }
    return nValue;
}

bool ListCtrlData::SetDataItemHeight(size_t itemIndex, int32_t nItemHeight, bool& bChanged)
{
    bChanged = false;
    if (nItemHeight < 0) {
        nItemHeight = -1;
    }
    if (m_nDefaultItemHeight == nItemHeight) {
        //If it equals the default height, set it to the flag value
        nItemHeight = -1;
    }

    bool bRet = false;
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    ASSERT(rowId != kInvalidRowId);
    if (rowId != kInvalidRowId) {
        ListCtrlItemData& rowData = m_rowDataList[rowId];
        int16_t nOldItemHeight = rowData.nItemHeight;
        bChanged = rowData.nItemHeight != nItemHeight;
        ASSERT(nItemHeight <= INT16_MAX);
        rowData.nItemHeight = (int16_t)nItemHeight;
        if ((nOldItemHeight >= 0) && (nItemHeight < 0)) {
            m_heightRowCount -= 1;
        }
        else if ((nOldItemHeight < 0) && (nItemHeight >= 0)) {
            m_heightRowCount += 1;
        }
        ASSERT(m_heightRowCount >= 0);
        bRet = true;
    }
    AssertDataConsistency();
    //Do not refresh; the caller determines whether a refresh is needed
    return bRet;
}

int32_t ListCtrlData::GetDataItemHeight(size_t itemIndex) const
{
    int32_t nValue = 0;
    const ListCtrlItemData* pRowData = GetDisplayItemData(itemIndex);
    ASSERT(pRowData != nullptr);
    if (pRowData != nullptr) {
        nValue = pRowData->nItemHeight;
        if ((nValue < 0) && (m_nDefaultItemHeight > 0)) {
            //Use the default height
            nValue = m_nDefaultItemHeight;
        }
    }
    return nValue;
}

bool ListCtrlData::SetDataItemUserData(size_t itemIndex, size_t itemData)
{
    bool bRet = false;
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    ASSERT(rowId != kInvalidRowId);
    if (rowId != kInvalidRowId) {
        ListCtrlItemData& rowData = m_rowDataList[rowId];
        rowData.nUserData = itemData;
        bRet = true;
    }
    return bRet;
}

size_t ListCtrlData::GetDataItemUserData(size_t itemIndex) const
{
    size_t nItemData = 0;
    const ListCtrlItemData* pRowData = GetDisplayItemData(itemIndex);
    ASSERT(pRowData != nullptr);
    if (pRowData != nullptr) {
        nItemData = pRowData->nUserData;
    }
    return nItemData;
}


bool ListCtrlData::SetSubItemData(size_t itemIndex, size_t columnId,
                                  const ListCtrlSubItemData& subItemData, bool& bCheckChanged)
{
    bCheckChanged = false;
    Storage storage;
    SubItemToStorage(subItemData, storage);

    bool bRet = false;
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    if (rowId == kInvalidRowId) {
        return false;
    }
    auto iter = m_dataMap.find(columnId);
    ASSERT(iter != m_dataMap.end());
    if (iter != m_dataMap.end()) {
        //Associated column: update the data (allocate a storage page first when it has not been written)
        StoragePageList& pageList = iter->second;
        const Storage* pOld = GetStorageAt(pageList, rowId);
        Storage* pStorage = GetStorageAtForWrite(pageList, rowId);
        if (pStorage != nullptr) {
            if (pOld == nullptr) {
                *pStorage = storage;
                if (storage.bChecked) {
                    bCheckChanged = true;
                }
            }
            else {
                if (storage.bChecked != pOld->bChecked) {
                    bCheckChanged = true;
                }
                *pStorage = storage;
            }
            bRet = true;
        }
    }

    if (bRet) {
        EmitDataChanged(itemIndex, itemIndex);
    }
    return bRet;
}

bool ListCtrlData::GetSubItemData(size_t itemIndex, size_t columnId, ListCtrlSubItemData& subItemData) const
{
    subItemData = ListCtrlSubItemData();

    bool bRet = false;
    const RowId rowId = GetRowIdByDisplayIndex(itemIndex);
    if (rowId == kInvalidRowId) {
        return false;
    }
    auto iter = m_dataMap.find(columnId);
    ASSERT(iter != m_dataMap.end());
    if (iter != m_dataMap.end()) {
        const Storage* pStorage = GetStorageAt(iter->second, rowId);
        if (pStorage != nullptr) {
            StorageToSubItem(*pStorage, subItemData);
        }
        bRet = true;
    }
    return bRet;
}

bool ListCtrlData::SetSubItemText(size_t itemIndex, size_t columnId, const std::string& text)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    if (pStorage->text != text) {
        pStorage->text = text;
        EmitDataChanged(itemIndex, itemIndex);
    }    
    return true;
}

std::string ListCtrlData::GetSubItemText(size_t itemIndex, size_t columnId) const
{
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return std::string();
    }
    return pStorage->text.c_str();
}

bool ListCtrlData::SetSubItemSortGroup(size_t itemIndex, size_t columnId, int32_t nSortGroup)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    if (pStorage->nSortGroup != nSortGroup) {
        pStorage->nSortGroup = nSortGroup;
        EmitDataChanged(itemIndex, itemIndex);
    }
    return true;
}

int32_t ListCtrlData::GetSubItemSortGroup(size_t itemIndex, size_t columnId) const
{
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return 0;
    }
    return pStorage->nSortGroup;
}

bool ListCtrlData::SetSubItemUserDataN(size_t itemIndex, size_t columnId, uint64_t userDataN)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    if (pStorage->userDataN != userDataN) {
        pStorage->userDataN = userDataN;
        EmitDataChanged(itemIndex, itemIndex);
    }
    return true;
}

uint64_t ListCtrlData::GetSubItemUserDataN(size_t itemIndex, size_t columnId) const
{
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return 0;
    }
    return pStorage->userDataN;
}

bool ListCtrlData::SetSubItemUserDataS(size_t itemIndex, size_t columnId, const std::string& userDataS)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    if (pStorage->userDataS != userDataS) {
        pStorage->userDataS = userDataS;
        EmitDataChanged(itemIndex, itemIndex);
    }
    return true;
}

std::string ListCtrlData::GetSubItemUserDataS(size_t itemIndex, size_t columnId) const
{
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return std::string();
    }
    return pStorage->userDataS.c_str();
}

bool ListCtrlData::SetSubItemTextColor(size_t itemIndex, size_t columnId, const UiColor& textColor)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    if (pStorage->textColor != textColor) {
        pStorage->textColor = textColor;
        EmitDataChanged(itemIndex, itemIndex);
    }    
    return true;
}

bool ListCtrlData::GetSubItemTextColor(size_t itemIndex, size_t columnId, UiColor& textColor) const
{
    textColor = UiColor();
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    textColor = pStorage->textColor;
    return true;
}

bool ListCtrlData::SetSubItemTextFormat(size_t itemIndex, size_t columnId, int32_t nTextFormat)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    int32_t nValidTextFormat = (int32_t)Label::GetValidTextStyle(nTextFormat);
    if (pStorage->nTextFormat != nValidTextFormat) {
        pStorage->nTextFormat = ui::TruncateToUInt16(nValidTextFormat);
        EmitDataChanged(itemIndex, itemIndex);
    }
    return true;
}

int32_t ListCtrlData::GetSubItemTextFormat(size_t itemIndex, size_t columnId) const
{
    int32_t nTextFormat = 0;
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    ASSERT(pStorage != nullptr);
    if (pStorage != nullptr) {
        nTextFormat = pStorage->nTextFormat;
        if (nTextFormat <= 0) {
            nTextFormat = m_nDefaultTextStyle;
        }
    }    
    return nTextFormat;
}

bool ListCtrlData::SetSubItemSingleLine(size_t itemIndex, size_t columnId, bool bSingleLine)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    //Switch the single-line/multi-line attribute based on the currently effective text format (including the default format):
    //in multi-line mode the text wraps automatically according to the available cell width (the row height must be adjusted by the caller through SetDataItemHeight)
    int32_t nTextFormat = GetSubItemTextFormat(itemIndex, columnId);
    if (bSingleLine) {
        nTextFormat |= TEXT_SINGLELINE;
    }
    else {
        nTextFormat &= ~TEXT_SINGLELINE;
    }
    return SetSubItemTextFormat(itemIndex, columnId, nTextFormat);
}

bool ListCtrlData::IsSubItemSingleLine(size_t itemIndex, size_t columnId) const
{
    int32_t nTextFormat = GetSubItemTextFormat(itemIndex, columnId);
    return (nTextFormat & TEXT_SINGLELINE) != 0;
}

bool ListCtrlData::SetSubItemBkColor(size_t itemIndex, size_t columnId, const UiColor& bkColor)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    if (pStorage->bkColor != bkColor) {
        pStorage->bkColor = bkColor;
        EmitDataChanged(itemIndex, itemIndex);
    }    
    return true;
}

bool ListCtrlData::GetSubItemBkColor(size_t itemIndex, size_t columnId, UiColor& bkColor) const
{
    bkColor = UiColor();
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    bkColor = pStorage->bkColor;
    return true;
}

bool ListCtrlData::IsSubItemShowCheckBox(size_t itemIndex, size_t columnId) const
{
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    return pStorage->bShowCheckBox;
}

bool ListCtrlData::SetSubItemShowCheckBox(size_t itemIndex, size_t columnId, bool bShowCheckBox)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    if (pStorage->bShowCheckBox != bShowCheckBox) {
        pStorage->bShowCheckBox = bShowCheckBox;
        EmitDataChanged(itemIndex, itemIndex);
    }    
    return true;
}

bool ListCtrlData::SetSubItemCheck(size_t itemIndex, size_t columnId, bool bChecked, bool bRefresh)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    ASSERT(pStorage->bShowCheckBox);
    if (pStorage->bShowCheckBox) {
        if (pStorage->bChecked != bChecked) {
            pStorage->bChecked = bChecked;
            if (bRefresh) {
                EmitDataChanged(itemIndex, itemIndex);
            }            
        }        
        return true;
    }
    return false;
}

bool ListCtrlData::GetSubItemCheck(size_t itemIndex, size_t columnId, bool& bChecked) const
{
    bChecked = false;
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    ASSERT(pStorage->bShowCheckBox);
    if (pStorage->bShowCheckBox) {
        bChecked = pStorage->bChecked;
        return true;
    }
    return false;
}

bool ListCtrlData::SetSubItemImageId(size_t itemIndex, size_t columnId, int32_t imageId)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    if (imageId < -1) {
        imageId = -1;
    }
    if (pStorage->nImageId != imageId) {
        pStorage->nImageId = imageId;
        EmitDataChanged(itemIndex, itemIndex);
    }
    return true;
}

int32_t ListCtrlData::GetSubItemImageId(size_t itemIndex, size_t columnId) const
{
    int32_t nImageId = -1;
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    ASSERT(pStorage != nullptr);
    if (pStorage != nullptr) {
        nImageId = pStorage->nImageId;
    }
    return nImageId;
}

bool ListCtrlData::SetSubItemEditable(size_t itemIndex, size_t columnId, bool bEditable)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    if (pStorage->bEditable != bEditable) {
        pStorage->bEditable = bEditable;
        EmitDataChanged(itemIndex, itemIndex);
    }
    return true;
}

bool ListCtrlData::IsSubItemEditable(size_t itemIndex, size_t columnId) const
{
    bool bEditable = false;
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    ASSERT(pStorage != nullptr);
    if (pStorage != nullptr) {
        bEditable = pStorage->bEditable;
    }
    return bEditable;
}

bool ListCtrlData::SetSubItemProgress(size_t itemIndex, size_t columnId, int32_t nProgress)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    ASSERT(pStorage != nullptr);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    if (nProgress > 100) {
        nProgress = 100;
    }
    if (nProgress < -1) {
        nProgress = -1;
    }
    if (pStorage->nProgress != static_cast<int8_t>(nProgress)) {
        pStorage->nProgress = static_cast<int8_t>(nProgress);
        EmitDataChanged(itemIndex, itemIndex);
    }
    return true;
}

int32_t ListCtrlData::GetSubItemProgress(size_t itemIndex, size_t columnId) const
{
    int32_t nProgress = -1;
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    ASSERT(pStorage != nullptr);
    if (pStorage != nullptr) {
        nProgress = static_cast<int32_t>(pStorage->nProgress);
    }
    return nProgress;
}

bool ListCtrlData::SetSubItemRichText(size_t itemIndex, size_t columnId, bool bRichText)
{
    Storage* pStorage = GetSubItemStorageForWrite(itemIndex, columnId);
    ASSERT(pStorage != nullptr);
    if (pStorage == nullptr) {
        //Invalid index
        return false;
    }
    if (pStorage->bRichText != bRichText) {
        pStorage->bRichText = bRichText;
        EmitDataChanged(itemIndex, itemIndex);
    }
    return true;
}

bool ListCtrlData::IsSubItemRichText(size_t itemIndex, size_t columnId) const
{
    bool bRichText = false;
    const Storage* pStorage = GetSubItemStorage(itemIndex, columnId);
    ASSERT(pStorage != nullptr);
    if (pStorage != nullptr) {
        bRichText = pStorage->bRichText;
    }
    return bRichText;
}

bool ListCtrlData::SortDataItems(size_t nColumnId, size_t nColumnIndex,
                                 bool bSortedUp, uint8_t nSortFlag,
                                 ListCtrlDataCompareFunc pfnCompareFunc, void* pUserData)
{
    StorageMap::iterator iter = m_dataMap.find(nColumnId);
    if (iter == m_dataMap.end()) {
        return false;
    }
    const StoragePageList& pageList = iter->second;
    if (m_displayOrder.empty()) {
        return false;
    }

    //Sort the display order list only: the comparison reads the cells of the sort column by row
    //identity, so the data itself is not moved
    const size_t dataCount = m_displayOrder.size();
    std::vector<StorageData> sortedDataList;
    sortedDataList.reserve(dataCount);
    for (size_t index = 0; index < dataCount; ++index) {
        const RowId rowId = m_displayOrder[index];
        sortedDataList.push_back({index, GetStorageAt(pageList, rowId)});
    }
    if (!SortStorageData(sortedDataList, nColumnId, nColumnIndex, bSortedUp, nSortFlag, pfnCompareFunc, pUserData)) {
        return false;
    }

    //Reorder the display order list according to the sort result (only the 4-byte row identities are
    //moved; each column is no longer deep-copied as a whole)
    ASSERT(sortedDataList.size() == dataCount);
    std::vector<RowId> newDisplayOrder;
    newDisplayOrder.reserve(dataCount);
    for (size_t index = 0; index < dataCount; ++index) {
        newDisplayOrder.push_back(m_displayOrder[sortedDataList[index].index]);
    }
    m_displayOrder.swap(newDisplayOrder);

    //The single-selection state stores a row identity, so it follows the corresponding row after
    //sorting and needs no adjustment
    AssertDataConsistency();
    EmitCountChanged();
    return true;
}

namespace
{
/** Stable sort of the data list, implementing the "(a < b)" ascending semantics through fnLess
* @param [in] first/last The range to be sorted
* @param [in] bDescending true means descending, false means ascending
* @param [in] fnLess The ascending compare function, implementing the (a < b) semantics
* Note:
* 1) std::stable_sort is used instead of std::sort: elements that compare equal (same value) keep
*    their original relative order, so rows in the same group (e.g. folders/files) or with the same
*    value are not reordered/interleaved by sorting;
* 2) descending order is sorted directly by "transposing the comparison" (swapping a/b) instead of
*    "sort ascending then std::reverse the whole list" - a whole-list reversal would also reverse
*    the relative order of equal elements, causing the same out-of-order problem;
* 3) rows without cell data (pStorage is nullptr) are treated as the minimum value: they come first
*    in ascending order and last in descending order, consistent with the behaviour before transposing.
*/
template <typename IterType, typename LessFunc>
void StableSortDisplayData(IterType first, IterType last, bool bDescending, LessFunc fnLess)
{
    if (!bDescending) {
        std::stable_sort(first, last, fnLess);
    }
    else {
        std::stable_sort(first, last, [&fnLess](const auto& a, const auto& b) {
                return fnLess(b, a);
            });
    }
}

}//namespace

bool ListCtrlData::SortStorageData(std::vector<StorageData>& dataList, size_t nColumnId, size_t nColumnIndex,
                                   bool bSortedUp, uint8_t nSortFlag,
                                   ListCtrlDataCompareFunc pfnCompareFunc, void* pUserData)
{
    if (dataList.empty()) {
        return false;
    }

    if (pfnCompareFunc == nullptr) {
        //If there is no valid parameter, use the externally set sort function
        pfnCompareFunc = m_pfnCompareFunc;
        pUserData = m_pUserData;
    }

    if (pfnCompareFunc != nullptr) {
        //Sort using the custom compare function (stable sort: equal elements keep their original relative order)
        ListCtrlCompareParam param;
        param.nColumnId = nColumnId;
        param.nColumnIndex = nColumnIndex;
        param.nSortFlag = nSortFlag;
        param.pUserData = pUserData;
        auto fnLess = [&pfnCompareFunc, &param](const StorageData& a, const StorageData& b) -> bool {
            //Implement the comparison logic of (a < b)
            if (b.pStorage == nullptr) {
                return false;
            }
            if (a.pStorage == nullptr) {
                return true;
            }
            return pfnCompareFunc(*a.pStorage, *b.pStorage, param);
        };
        StableSortDisplayData(dataList.begin(), dataList.end(), !bSortedUp, fnLess);
    }
    else {
        //Sort: ascending, using the default sort function (stable sort: equal elements keep their original relative order)
        auto fnLess = [this, nSortFlag](const StorageData& a, const StorageData& b) -> bool {
            //Implement the comparison logic of (a < b)
            if (b.pStorage == nullptr) {
                return false;
            }
            if (a.pStorage == nullptr) {
                return true;
            }
            return SortDataCompareFunc(*a.pStorage, *b.pStorage, nSortFlag);
        };
        StableSortDisplayData(dataList.begin(), dataList.end(), !bSortedUp, fnLess);
    }
    return true;
}

namespace
{
/** Parse a date/time text into a comparable 64-bit key (used by kSortByTextAsDate sorting)
*  Supported forms: fields separated by any non-digit characters, taking the numeric fields in the
*  order "year, month, day, hour, minute, second", for example "2024-3-7", "2024/03/07",
*  "2024-03-07 13:45:00" (month, day, hour, etc. may omit leading zeros).
*  At least the three fields "year, month, day" are required, otherwise the date is invalid
*  (an invalid value is smaller than any valid value).
* @param [in] text The text to be parsed
* @param [out] nKey The parsed key value; unchanged when parsing fails
* @return Returns true on success, otherwise returns false
*/
bool ParseTextAsDateKey(const UiString& text, int64_t& nKey)
{
    const std::string::value_type* pStr = text.c_str();
    if (pStr == nullptr) {
        return false;
    }
    //Take the six numeric fields "year, month, day, hour, minute, second" in order
    constexpr int32_t kFieldCount = 6;
    int64_t nFields[kFieldCount] = { 0, 0, 0, 0, 0, 0 };
    int32_t nFoundCount = 0;
    bool bInNumber = false;
    for (; *pStr != 0; ++pStr) {
        const std::string::value_type ch = *pStr;
        if ((ch >= '0') && (ch <= '9')) {
            if (!bInNumber) {
                if (nFoundCount >= kFieldCount) {
                    //Numeric fields beyond the supported range are ignored
                    break;
                }
                ++nFoundCount;    //Start a new numeric field
                bInNumber = true;
            }
            nFields[nFoundCount - 1] = nFields[nFoundCount - 1] * 10 + (ch - '0');
        }
        else {
            bInNumber = false;
        }
    }
    if (nFoundCount < 3) {
        //Fewer than the three fields "year, month, day": a valid date cannot be formed
        return false;
    }
    //Encode with a per-level carry in the order "year, month, day, hour, minute, second", so that the
    //magnitude order of the key matches the chronological order of the date/time
    int64_t nRetKey = 0;
    for (int32_t i = 0; i < kFieldCount; ++i) {
        nRetKey = nRetKey * 100 + nFields[i];
    }
    nKey = nRetKey;
    return true;
}
}//namespace

bool ListCtrlData::SortDataCompareFunc(const ListCtrlSubItemData2& a, const ListCtrlSubItemData2& b, uint8_t nSortFlag) const
{
    if (nSortFlag & ListCtrlSubItemSortFlag::kSortByGroup) {
        //Group sorting is supported
        if (a.nSortGroup != b.nSortGroup) {
            return a.nSortGroup < b.nSortGroup;
        }
    }
    if (nSortFlag & ListCtrlSubItemSortFlag::kSortByUserDataN) {
        //Sort by the .userDataN field (integer value)
        return a.userDataN < b.userDataN;
    }
    else if (nSortFlag & ListCtrlSubItemSortFlag::kSortByUserDataS) {
        //Sort by the .userDataS field (string value)
        if (nSortFlag & ListCtrlSubItemSortFlag::kSortNoCase) {
            //Case-insensitive
            return StringUtil::StringICompare(a.userDataS.c_str(), b.userDataS.c_str()) < 0;
        }
        else {
            //Case-sensitive
            return StringUtil::StringCompare(a.userDataS.c_str(), b.userDataS.c_str()) < 0;
        }
    }
    else if (nSortFlag & ListCtrlSubItemSortFlag::kSortByProgress) {
        //Sort by the progress value (explicitly declared at column level, replacing the earlier behaviour of "inferring from the cell nProgress")
        //Note: cells that do not show a progress bar (nProgress is -1) are treated as the minimum value, so the comparison remains a strict total order
        return a.nProgress < b.nProgress;
    }
    else if (nSortFlag & ListCtrlSubItemSortFlag::kSortByTextAsNumber) {
        //Compare text as numbers (e.g. "10" > "9") instead of by string lexicographic order
        double dValueA = StringUtil::StringToDouble(a.text.c_str());
        double dValueB = StringUtil::StringToDouble(b.text.c_str());
        return dValueA < dValueB;
    }
    else if (nSortFlag & ListCtrlSubItemSortFlag::kSortByTextAsDate) {
        //Compare text as date/time (e.g. "2024-3-7" < "2024-10-1") instead of by string lexicographic order
        //Note: text that cannot be parsed as a date is treated as the minimum value (consistent with the "invalid value is smallest" convention of the other sort flags);
        //    when the keys are equal, fall back to comparing the text, so the comparison remains a strict total order
        int64_t nKeyA = -1;
        int64_t nKeyB = -1;
        ParseTextAsDateKey(a.text, nKeyA);
        ParseTextAsDateKey(b.text, nKeyB);
        if (nKeyA != nKeyB) {
            return nKeyA < nKeyB;
        }
        return StringUtil::StringCompare(a.text.c_str(), b.text.c_str()) < 0;
    }
    else {
        //Sort by the .text field (string value)
        if (nSortFlag & ListCtrlSubItemSortFlag::kSortNoCase) {
            //Case-insensitive
            return StringUtil::StringICompare(a.text.c_str(), b.text.c_str()) < 0;
        }
        else {
            //Case-sensitive
            return StringUtil::StringCompare(a.text.c_str(), b.text.c_str()) < 0;
        }
    }
}

void ListCtrlData::SetSortCompareFunction(ListCtrlDataCompareFunc pfnCompareFunc, void* pUserData)
{
    m_pfnCompareFunc = pfnCompareFunc;
    m_pUserData = pUserData;
}

void ListCtrlData::SetSelectedElements(const std::vector<size_t>& selectedIndexs,
                                       bool bClearOthers,
                                       std::vector<size_t>& refreshIndexs)
{
    refreshIndexs.clear();
    if (!IsMultiSelect()) {
        return;
    }
    std::set<size_t> selectSet;
    for (auto index : selectedIndexs) {
        selectSet.insert(index);
    }
    std::set<size_t> refreshSet;
    std::vector<size_t> oldSelectedIndexs;
    if (bClearOthers) {
        GetSelectedElements(oldSelectedIndexs);
        if (!oldSelectedIndexs.empty()) {
            for (size_t nElementIndex : oldSelectedIndexs) {
                if (selectSet.find(nElementIndex) != selectSet.end()) {
                    //Filter out the ones about to be selected
                    continue;
                }
                SetElementSelected(nElementIndex, false);
                refreshSet.insert(nElementIndex);
            }
        }
    }
    for (size_t nElementIndex : selectedIndexs) {
        if (IsElementSelected(nElementIndex)) {
            continue;
        }
        SetElementSelected(nElementIndex, true);
        refreshSet.insert(nElementIndex);
    }
    for (auto index : refreshSet) {
        refreshIndexs.push_back(index);
    }
}

bool ListCtrlData::SelectAll(std::vector<size_t>& refreshIndexs)
{
    if (!IsMultiSelect()) {
        return false;
    }
    std::vector<size_t> selectedIndexs;
    size_t nCount = GetElementCount();
    for (size_t nElementIndex = 0; nElementIndex < nCount; ++nElementIndex) {
        if (IsSelectableElement(nElementIndex) && !IsElementSelected(nElementIndex)) {
            SetElementSelected(nElementIndex, true);
            selectedIndexs.push_back(nElementIndex);
        }
    }
    refreshIndexs.swap(selectedIndexs);
    return !refreshIndexs.empty();
}

bool ListCtrlData::IsSelectableRowData(const ListCtrlItemData& rowData) const
{
    //Visible and not displayed on top
    return rowData.bVisible && (rowData.nAlwaysAtTop < 0);
}

bool ListCtrlData::IsSelectableElement(size_t nElementIndex) const
{
    bool bSelectable = true;
    const ListCtrlItemData* pRowData = GetDisplayItemData(nElementIndex);
    if (pRowData != nullptr) {
        bSelectable = IsSelectableRowData(*pRowData);
    }
    return bSelectable;
}

void ListCtrlData::SelectNone(std::vector<size_t>& refreshIndexs)
{
    SelectNoneExclude(std::vector<size_t>(), refreshIndexs);
}

void ListCtrlData::SelectNoneExclude(const std::vector<size_t>& excludeIndexs,
                                     std::vector<size_t>& refreshIndexs)
{
    refreshIndexs.clear();
    std::vector<size_t> selectedIndexs;
    GetSelectedElements(selectedIndexs);
    if (!selectedIndexs.empty()) {
        std::set<size_t> indexSet;
        for (size_t nElementIndex : excludeIndexs) {
            indexSet.insert(nElementIndex);
        }
        for (size_t nElementIndex : selectedIndexs) {
            if (!indexSet.empty()) {
                if (indexSet.find(nElementIndex) != indexSet.end()) {
                    //Exclude
                    continue;
                }
            }
            SetElementSelected(nElementIndex, false);
        }
        refreshIndexs.swap(selectedIndexs);
    }
}

}//namespace ui

