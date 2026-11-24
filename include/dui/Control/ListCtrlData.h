#ifndef UI_CONTROL_LIST_CTRL_DATA_PROVIDER_H_
#define UI_CONTROL_LIST_CTRL_DATA_PROVIDER_H_

#include "dui/Box/VirtualListBox.h"
#include "dui/Control/ListCtrlDefs.h"
#include <unordered_map>
#include <array>
#include <memory>

namespace ui
{
/** Data manager of list items
*/
class ListCtrl;
class ListCtrlReportView;
struct ListCtrlSubItemData;
class DUI_API ListCtrlData : public ui::VirtualListBoxElement
{
    friend class ListCtrlReportView; //List data display UI control (Report view): needs access to the cell data storage to support per-cell refresh
    friend class ListCtrl;           //ListCtrl control: needs to map display indexes to row identities (RowId), decoupling cell hover state from display order
public:
    //Data structure used for storage
    typedef ListCtrlSubItemData2 Storage;
    typedef std::vector<ListCtrlItemData> RowDataList;      //Row attribute pool: indexed by row identity (RowId)

    /** Row identity: the unique identifier of a row; monotonically increasing, never reused after deletion.
        Used to decouple "the identity of a row" from "the physical position of a row": insert/delete/update only reorder the display order list, the data itself is not moved
    */
    typedef uint32_t RowId;

    /** Invalid row identity
    */
    static constexpr RowId kInvalidRowId = static_cast<RowId>(0xFFFFFFFF);

    /** Number of cells per page (paged contiguous storage)
        Page index = RowId / kPageCells, offset within the page = RowId % kPageCells
    */
    static constexpr size_t kPageCells = 64;

    /** Storage page: the cells within a page are stored contiguously, with a "written" marker
        Advantages: the array within a page is contiguous (cache-friendly for drawing/sorting scans, no shared_ptr pointer chasing);
              pages are lazily allocated (created on the first write to a page), so sparse or purely virtual table scenarios do not bloat memory
    */
    struct StoragePage
    {
        std::array<Storage, kPageCells> cells;  //Cells within the page (contiguous)
        uint8_t usedMask[kPageCells] = {};      //Whether each cell has been written (1 means written)
    };

    /** Storage page list of each column (stored by page index; pages are lazily allocated, so null pointers are allowed in the list)
    */
    typedef std::vector<std::unique_ptr<StoragePage>> StoragePageList;
    typedef std::unordered_map<size_t, StoragePageList> StorageMap;

public:
    ListCtrlData();

    /** Create a data item
    * @param [in] pVirtualListBox The interface of the associated virtual list
    * @return Returns the pointer of the created data item
    */
    virtual ui::Control* CreateElement(ui::VirtualListBox* pVirtualListBox) override;

    /** Fill the specified data item
    * @param [in] pControl Pointer to the data item control
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    */
    virtual bool FillElement(ui::Control* pControl, size_t nElementIndex) override;

    /** Get the total number of data items
    * @return Returns the total number of data items
    */
    virtual size_t GetElementCount() const override;

    /** Set the selection state
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @param [in] bSelected true means the selected state, false means the non-selected state
    */
    virtual void SetElementSelected(size_t nElementIndex, bool bSelected) override;

    /** Get the selection state
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @return true means the selected state, false means the non-selected state
    */
    virtual bool IsElementSelected(size_t nElementIndex) const override;

    /** Get the list of selected elements
    * @param [in] selectedIndexs Returns the list of currently selected elements, valid range: [0, GetElementCount())
    */
    virtual void GetSelectedElements(std::vector<size_t>& selectedIndexs) const override;

    /** Whether multi-select is supported
    */
    virtual bool IsMultiSelect() const override;

    /** Set whether multi-select is supported; called by the UI layer to keep consistent with the UI control
    * @return bMultiSelect true means multi-select is supported, false means it is not supported
    */
    virtual void SetMultiSelect(bool bMultiSelect) override;

public:
    /** Set the view interface
    */
    void SetListView(IListCtrlView* pListView);

    /** Set whether to automatically check the selected data items (applies to the Header and each row)
    */
    void SetAutoCheckSelect(bool bAutoCheckSelect);

    /** Get whether to automatically check the selected data items
    */
    bool IsAutoCheckSelect() const;

    /** Set the default text attributes
    */
    void SetDefaultTextStyle(int32_t nTextStyle);

    /** Set the default row height
    */
    void SetDefaultItemHeight(int32_t nItemHeight);

    /** The DPI has changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] dpiManager The DPI scale manager
    */
    void ChangeDpiScale(const DpiManager& dpiManager, uint32_t nOldDpiScale);

public:
    /** Add a column and refresh the UI display
    * @param [in] columnId The column ID
    */
    bool AddColumn(size_t columnId);

    /** Delete a column and refresh the UI display
    * @param [in] columnId The column ID
    */
    bool RemoveColumn(size_t columnId);

    /** Get the maximum width of a column
    * @return Returns the maximum width of this column, which is the value after DPI adaptive processing; returns -1 on failure
    */
    int32_t GetMaxColumnWidth(size_t columnId) const;

    /** Set the check state of a column (Checked or UnChecked)
    * @param [in] columnId The column ID
    * @param [in] bChecked true means selected, false means deselected
    * @param [in] bRefresh Whether to refresh the UI display
    */
    bool SetColumnCheck(size_t columnId, bool bChecked, bool bRefresh);

    /** Get the total number of data items
    */
    size_t GetDataItemCount() const;

    /** Set the total number of data items and refresh the UI display
    * @param [in] itemCount The total number of data items; the data of each data item is filled through callbacks (internally implemented as a virtual list)
    */
    bool SetDataItemCount(size_t itemCount);

    /** Add a data item at the end and refresh the UI display
    * @param [in] columnId The column ID
    * @param [in] dataItem The content of the data item
    * @return On success, returns the row index of the data item; on failure, returns Box::InvalidIndex
    */
    size_t AddDataItem(size_t columnId, const ListCtrlSubItemData& dataItem);

    /** Add a data item at the specified row position and refresh the UI display
    * @param [in] itemIndex The index of the data item
    * @param [in] columnId The column ID
    * @param [in] dataItem The content of the data item
    */
    bool InsertDataItem(size_t itemIndex, size_t columnId, const ListCtrlSubItemData& dataItem);

    /** Delete the data item of the specified row and refresh the UI display
    * @param [in] itemIndex The index of the data item
    */
    bool DeleteDataItem(size_t itemIndex);

    /** Delete the data items of all rows and refresh the UI display
    * @return If data was deleted, returns true; otherwise returns false
    */
    bool DeleteAllDataItems();

    /** Delete the data items of the specified rows in batch and refresh the UI display
        Implementation: first de-duplicate by row identity and mark tombstones, then compact the
        display order list in a single pass, avoiding the O(K*N) move cost of deleting row by row
    * @param [in] displayIndexs The display indexes of the data items to delete, valid range: [0, GetDataItemCount());
    *                            duplicate indexes are ignored; out-of-range indexes are ignored
    * @return Returns the number of rows actually deleted
    */
    size_t DeleteDataItems(const std::vector<size_t>& displayIndexs);

    /** Compact the data storage: tidy up the row identities and the storage page layout, and
        reclaim the memory occupied by deleted rows
        Note: this is an explicit whole-table maintenance operation (renumbering + full compaction);
              call it only while no filling/drawing is in progress. After the call, all internal
              cell pointers (raw pointers) handed out earlier become invalid
    */
    void CompactStorage();

    /** Set the row attribute data of a data item and refresh the UI display
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] itemData The associated data
    * @param [out] bChanged Returns whether the data has changed
    * @param [out] bCheckChanged Returns whether the bCheck flag has changed
    */
    bool SetDataItemData(size_t itemIndex, const ListCtrlItemData& itemData, 
                         bool& bChanged, bool& bCheckChanged);

    /** Get the row attribute data of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] itemData The associated data
    */
    bool GetDataItemData(size_t itemIndex, ListCtrlItemData& itemData) const;

    /** Set the visibility of a data item and refresh the UI display
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] bVisible Whether it is visible
    * @param [out] bChanged Returns whether the data has changed
    */
    bool SetDataItemVisible(size_t itemIndex, bool bVisible, bool& bChanged);

    /** Get the visibility of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @return Returns the visibility associated with the data item
    */
    bool IsDataItemVisible(size_t itemIndex) const;

    /** Set the selection attribute of a data item and refresh the UI display
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] bSelected Whether it is in the selected state
    * @param [out] bChanged Returns whether the data has changed
    */
    bool SetDataItemSelected(size_t itemIndex, bool bSelected, bool& bChanged);

    /** Get the selection attribute of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @return Returns the selection state associated with the data item
    */
    bool IsDataItemSelected(size_t itemIndex) const;

    /** Set the icon at the beginning of the row; do not refresh the UI, the caller is responsible for refreshing the UI
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] imageId The icon resource ID; if -1, no icon is displayed at the beginning of the row; the ID is generated by ImageList
    * @param [out] bChanged Returns whether the data has changed
    */
    bool SetDataItemImageId(size_t itemIndex, int32_t imageId, bool& bChanged);

    /** Get the icon at the beginning of the row
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    */
    int32_t GetDataItemImageId(size_t itemIndex) const;

    /** Get the selection state (bSelect)
    * @param [out] bSelected Whether it is selected
    * @param [out] bPartSelected Whether it is partially selected
    */
    void GetDataItemsSelectStatus(bool& bSelected, bool& bPartSelected) const;

    /** Set the check attribute of a data item (the CheckBox at the beginning of each row); do not refresh the UI, the caller is responsible for refreshing the UI
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] bChecked Whether it is in the checked state
    * @param [out] bChanged Returns whether the data has changed
    */
    bool SetDataItemChecked(size_t itemIndex, bool bChecked, bool& bChanged);

    /** Get the check attribute of a data item (the CheckBox at the beginning of each row)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @return Returns the checked state associated with the data item
    */
    bool IsDataItemChecked(size_t itemIndex) const;

    /** Set the check state of all rows (Checked or UnChecked) and refresh the UI display
    * @param [in] bChecked true means checked, false means unchecked
    */
    bool SetAllDataItemsCheck(bool bChecked);

    /** Set checked data items in batch (the data checked via the CheckBox at the beginning of each row); do not refresh the UI, the caller is responsible for refreshing the UI
    * @param [in] itemIndexs The indexes of the data items to be checked, valid range: [0, GetDataItemCount())
    * @param [in] bClearOthers If true, clear the selection of other already-selected items, keeping only the ones set this time
    * @param [out] refreshIndexs Returns the indexes of the elements that need to be refreshed and displayed
    */
    void SetCheckedDataItems(const std::vector<size_t>& itemIndexs,
                             bool bClearOthers,
                             std::vector<size_t>& refreshIndexs);

    /** Get the list of checked elements (the data checked via the CheckBox at the beginning of each row)
    * @param [in] itemIndexs Returns the indexes of the currently checked data items, valid range: [0, GetDataItemCount())
    */
    void GetCheckedDataItems(std::vector<size_t>& itemIndexs) const;

    /** Get the check state (bChecked)
    * @param [out] bChecked Whether it is checked
    * @param [out] bPartChecked Whether it is partially checked
    */
    void GetDataItemsCheckStatus(bool& bChecked, bool& bPartChecked) const;

    /** Set the always-on-top state of a data item; do not refresh the UI, the caller is responsible for refreshing the UI
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] nAlwaysAtTop The always-on-top state: -1 means not on top, 0 or a positive number means on top; the larger the value, the higher the priority, displayed at the top first
    * @param [out] bChanged Returns whether the data has changed
    */
    bool SetDataItemAlwaysAtTop(size_t itemIndex, int8_t nAlwaysAtTop, bool& bChanged);

    /** Get the always-on-top state of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @return Returns the always-on-top state associated with the data item: -1 means not on top, 0 or a positive number means on top; the larger the value, the higher the priority, displayed at the top first
    */
    int8_t GetDataItemAlwaysAtTop(size_t itemIndex) const;

    /** Set the row height of a data item; do not refresh the UI, the caller is responsible for refreshing the UI
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] nItemHeight The row height; -1 means using the default row height set by ListCtrl, other values mean the row height set for this row
    * @param [out] bChanged Returns whether the data has changed
    */
    bool SetDataItemHeight(size_t itemIndex, int32_t nItemHeight, bool& bChanged);

    /** Get the row height of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @return Returns the row height associated with the data item: -1 means using the default row height set by ListCtrl, other values mean the row height set for this row
    */
    int32_t GetDataItemHeight(size_t itemIndex) const;

    /** Set the custom data of a data item; do not refresh the UI
    * @param [in] itemIndex The index of the data item
    * @param [in] userData The custom data associated with the data item
    */
    bool SetDataItemUserData(size_t itemIndex, size_t userData);

    /** Get the custom data of a data item
    * @param [in] itemIndex The index of the data item
    * @return Returns the custom data associated with the data item
    */
    size_t GetDataItemUserData(size_t itemIndex) const;

public:
    /** Set the data item of the specified <row, column> and refresh the UI display
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] subItemData The content of the specified data item; the column index is specified in dataItem.nColumnIndex
    * @param [out] bCheckChanged Whether the bChecked state has changed
    */
    bool SetSubItemData(size_t itemIndex, size_t columnId,
                        const ListCtrlSubItemData& subItemData, bool& bCheckChanged);

    /** Get the data item of the specified <row, column>
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [out] subItemData The content of the specified data item
    */
    bool GetSubItemData(size_t itemIndex, size_t columnId, ListCtrlSubItemData& subItemData) const;

public:
    /** Set the text of the specified data item and refresh the UI display
    * @param [in] itemIndex The index of the data item
    * @param [in] columnId The column ID
    * @param [in] text The text content to set
    */
    bool SetSubItemText(size_t itemIndex, size_t columnId, const std::string& text);

    /** Get the text of the specified data item
    * @param [in] itemIndex The index of the data item
    * @param [in] columnId The column ID
    * @return The text content associated with the data item
    */
    std::string GetSubItemText(size_t itemIndex, size_t columnId) const;

    /** Set the sort group of the specified data item (integer)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] nSortGroup The integer data of the sort group to set
    */
    bool SetSubItemSortGroup(size_t itemIndex, size_t columnId, int32_t nSortGroup);

    /** Get the sort group of the specified data item (integer)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @return The integer data of the sort group
    */
    int32_t GetSubItemSortGroup(size_t itemIndex, size_t columnId) const;

    /** Set the associated user data of the specified data item (integer)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] userDataN The integer data associated with the data item to set
    */
    bool SetSubItemUserDataN(size_t itemIndex, size_t columnId, uint64_t userDataN);

    /** Get the associated user data of the specified data item (integer)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @return The integer data associated with the data item
    */
    uint64_t GetSubItemUserDataN(size_t itemIndex, size_t columnId) const;

    /** Set the associated user data of the specified data item (integer)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] userDataS The string data associated with the data item to set
    */
    bool SetSubItemUserDataS(size_t itemIndex, size_t columnId, const std::string& userDataS);

    /** Get the associated user data of the specified data item (string)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @return The string data associated with the data item
    */
    std::string GetSubItemUserDataS(size_t itemIndex, size_t columnId) const;

public:
    /** Set the text color of the specified data item and refresh the UI display
    * @param [in] itemIndex The index of the data item
    * @param [in] columnId The column ID
    * @param [in] textColor The text color to set
    */
    bool SetSubItemTextColor(size_t itemIndex, size_t columnId, const UiColor& textColor);

    /** Get the text color of the specified data item and refresh the UI display
    * @param [in] itemIndex The index of the data item
    * @param [in] columnId The column ID
    * @param [out] textColor The text color associated with the data item
    */
    bool GetSubItemTextColor(size_t itemIndex, size_t columnId, UiColor& textColor) const;

    /** Set the text attributes of the specified data item (text alignment, etc.) and refresh the UI display
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] nTextFormat The text attributes to set
    */
    bool SetSubItemTextFormat(size_t itemIndex, size_t columnId, int32_t nTextFormat);

    /** Get the text attributes of the specified data item (text alignment, etc.)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @return The text attributes associated with the data item
    */
    int32_t GetSubItemTextFormat(size_t itemIndex, size_t columnId) const;

    /** Set whether the text of the specified data item is displayed on a single line and refresh the UI display
        Note: when set to multi-line (false), the text wraps automatically when it exceeds the cell width;
              the row height must be adjusted by the caller through SetDataItemHeight, otherwise the overflow is clipped
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] bSingleLine true means single-line display (clipped with an ellipsis when it overflows), false means multi-line display (automatic wrapping)
    */
    bool SetSubItemSingleLine(size_t itemIndex, size_t columnId, bool bSingleLine);

    /** Get whether the text of the specified data item is displayed on a single line
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @return true means single-line display, false means multi-line display (automatic wrapping)
    */
    bool IsSubItemSingleLine(size_t itemIndex, size_t columnId) const;

    /** Set the background color of the specified data item and refresh the UI display
    * @param [in] itemIndex The index of the data item
    * @param [in] columnId The column ID
    * @param [in] bkColor The background color to set
    */
    bool SetSubItemBkColor(size_t itemIndex, size_t columnId, const UiColor& bkColor);

    /** Get the background color of the specified data item
    * @param [in] itemIndex The index of the data item
    * @param [in] columnId The column ID
    * @param [out] bkColor The background color associated with the data item
    */
    bool GetSubItemBkColor(size_t itemIndex, size_t columnId, UiColor& bkColor) const;

    /** Whether to show the CheckBox
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    */
    bool IsSubItemShowCheckBox(size_t itemIndex, size_t columnId) const;

    /** Set whether to show the CheckBox and refresh the UI display
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] bShowCheckBox true means show, false means hide
    */
    bool SetSubItemShowCheckBox(size_t itemIndex, size_t columnId, bool bShowCheckBox);

    /** Set the checked state of the CheckBox
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] bChecked true means checked, false means unchecked
    * @param [in] bRefresh true means refresh the UI display of this element, false means no UI refresh is needed
    */
    bool SetSubItemCheck(size_t itemIndex, size_t columnId, bool bChecked, bool bRefresh);

    /** Get the checked state of the CheckBox
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [out] bChecked true means checked, false means unchecked
    */
    bool GetSubItemCheck(size_t itemIndex, size_t columnId, bool& bChecked) const;

    /** Get the checked state of a column (bChecked)
    * @param [in] columnId The column ID
    * @param [out] bChecked Whether it is selected
    * @param [out] bPartChecked Whether it is partially selected
    */
    void GetColumnCheckStatus(size_t columnId, bool& bChecked, bool& bPartChecked) const;

    /** Set the icon of this column and refresh the UI display
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] imageId The icon resource ID; if -1, no icon is displayed at the beginning of the row; the ID is generated by ImageList
    */
    bool SetSubItemImageId(size_t itemIndex, size_t columnId, int32_t imageId);

    /** Get the icon of this column
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    */
    int32_t GetSubItemImageId(size_t itemIndex, size_t columnId) const;

    /** Set whether the text of this column is editable
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] bEditable true means editable, false means not editable
    */
    bool SetSubItemEditable(size_t itemIndex, size_t columnId, bool bEditable);

    /** Get whether the text of this column is editable
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    */
    bool IsSubItemEditable(size_t itemIndex, size_t columnId) const;

    /** Set the column to be displayed as a progress bar and refresh the UI display
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] nProgress The progress value, valid range [0, 100]; -1 means no progress bar is shown (displayed as normal text)
    */
    bool SetSubItemProgress(size_t itemIndex, size_t columnId, int32_t nProgress);

    /** Get the progress value of the column
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @return Returns the progress value, valid range [0, 100]; -1 means no progress bar is shown (displayed as normal text)
    */
    int32_t GetSubItemProgress(size_t itemIndex, size_t columnId) const;

    /** Set whether the text of the column is rendered as rich text (HTML-like format) and refresh the UI display
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @param [in] bRichText true means render as rich text, false means render as normal text
    */
    bool SetSubItemRichText(size_t itemIndex, size_t columnId, bool bRichText);

    /** Get whether the text of the column is rendered as rich text (HTML-like format)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    */
    bool IsSubItemRichText(size_t itemIndex, size_t columnId) const;

    /** Sort the data and refresh the UI display
    * @param [in] columnId The column ID
    * @param [in] nColumnIndex The ordinal number of the column
    * @param [in] bSortedUp true means ascending, false means descending
    * @param [in] nSortFlag The sort method flag, see the enum values of ListCtrlSubItemSortFlag
    * @param [in] pfnCompareFunc The data compare function
    * @param [in] pUserData User custom data, passed back to the compare function through the parameter when it is called
    * Note: stable sort (equal elements keep their original relative order), for both ascending and descending order
    */
    bool SortDataItems(size_t nColumnId, size_t nColumnIndex,
                       bool bSortedUp, uint8_t nSortFlag,
                       ListCtrlDataCompareFunc pfnCompareFunc, void* pUserData);

    /** Set an external custom sort function, replacing the default sort function
    * @param [in] pfnCompareFunc The data compare function
    * @param [in] pUserData User custom data, passed back to the compare function through the parameter when it is called
    */
    void SetSortCompareFunction(ListCtrlDataCompareFunc pfnCompareFunc, void* pUserData);

public:
    /** Set selected elements in batch; do not update the UI display
    * @param [in] selectedIndexs The list of elements to be selected, valid range: [0, GetElementCount())
    * @param [in] bClearOthers If true, clear the selection of other already-selected items, keeping only the ones set this time
    * @param [out] refreshIndexs Returns the indexes of the elements that need to be refreshed and displayed
    */
    void SetSelectedElements(const std::vector<size_t>& selectedIndexs,
                             bool bClearOthers,
                             std::vector<size_t>& refreshIndexs);

    /** Select all (excluding invisible elements and always-on-top elements); do not update the UI display
    * @return If the data has changed, returns true; otherwise returns false
    * @param [out] refreshIndexs Returns the indexes of the elements that need to be refreshed and displayed
    */
    bool SelectAll(std::vector<size_t>& refreshIndexs);

    /** Cancel all selections; do not update the UI display, the caller can refresh the UI display
    * @param [out] refreshIndexs Returns the indexes of the elements that need to be refreshed and displayed
    */
    void SelectNone(std::vector<size_t>& refreshIndexs);

    /** Cancel all selections (except some excluded elements); do not update the UI display, the caller can refresh the UI display
    * @param [in] excludeIndexs The indexes of the elements to exclude; the selection state of these elements remains unchanged
    * @param [out] refreshIndexs Returns the indexes of the elements that need to be refreshed and displayed
    */
    void SelectNoneExclude(const std::vector<size_t>& excludeIndexs,
                           std::vector<size_t>& refreshIndexs);

    /** Determine whether a row data is selectable
    */
    bool IsSelectableRowData(const ListCtrlItemData& rowData) const;

    /** Determine whether a data element is a selectable item
    * @param [in] nElementIndex The element index, valid range: [0, GetElementCount())
    */
    bool IsSelectableElement(size_t nElementIndex) const;

private:
    /** Convert data to the storage data structure
    */
    void SubItemToStorage(const ListCtrlSubItemData& item, Storage& storage) const;

    /** Convert the storage data to the structure data
    */
    void StorageToSubItem(const Storage& storage, ListCtrlSubItemData& item) const;

    /** Determine whether a data item index is valid
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    */
    bool IsValidDataItemIndex(size_t itemIndex) const;

    /** Determine whether a column ID is valid in the data storage
    */
    bool IsValidDataColumnId(size_t nColumnId) const;

    /** Get the data of the specified data item, for reading
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @return Returns nullptr on failure, or when the cell has no data
    */
    const Storage* GetSubItemStorage(size_t itemIndex, size_t nColumnId) const;

    /** Get the data of the specified data item, for writing (allocates a storage page when necessary)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @return Returns nullptr on failure
    */
    Storage* GetSubItemStorageForWrite(size_t itemIndex, size_t nColumnId);

    /** Read the cell of a column by row identity (read-only); returns nullptr when the cell has not been written
    */
    static const Storage* GetStorageAt(const StoragePageList& pageList, RowId rowId);

    /** Get a writable cell of a column by row identity; allocates a storage page when necessary and marks it as written
    */
    static Storage* GetStorageAtForWrite(StoragePageList& pageList, RowId rowId);

    /** Reset the cell of a given row identity in a column to a tombstone (clear the data and mark it as not written); does not allocate a storage page,
        and releases the whole page once all of its cells are tombstones (page recycling)
    */
    static void ResetStorageAt(StoragePageList& pageList, RowId rowId);

    /** Get the data of each column for UI display
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [out] subItemList Returns the data list of all columns of this row
    */
    bool GetSubItemStorageList(size_t itemIndex, std::vector<ListCtrlSubItemData2Pair>& subItemList) const;

    /** Display index -> row identity; returns kInvalidRowId when the index is invalid
    */
    RowId GetRowIdByDisplayIndex(size_t itemIndex) const;

    /** Row identity -> display index; returns Box::InvalidIndex when not found
    */
    size_t GetDisplayIndexByRowId(RowId rowId) const;

    /** Allocate a new row identity and grow the column data pools and the row attribute pool up to that position
    * @return Returns the newly allocated row identity
    */
    RowId AllocRowId();

    /** Release the data associated with a row identity (column pools become tombstones, row attributes are reset) and fix the counters and the single-selection state
    */
    void FreeRowId(RowId rowId);

    /** Cell selection: find the selection bitmap of a column (returns nullptr when it does not exist)
    */
    const std::vector<uint8_t>* FindCellBitmap(size_t nColumnId) const;

    /** Cell selection: find (creating if necessary) the selection bitmap of a column and make sure its capacity can hold rowId
    */
    std::vector<uint8_t>& GetCellBitmapForWrite(size_t nColumnId, RowId rowId);

    /** Cell selection: set the selection state of a single cell by row identity
    */
    void SetCellSelectedByRowId(RowId rowId, size_t nColumnId, bool bSelected);

    /** Cell selection: determine whether a single cell is selected by row identity
    */
    bool IsCellSelectedByRowId(RowId rowId, size_t nColumnId) const;

    /** Check the consistency of the internal data (assertion checks only in Debug builds)
    */
    void AssertDataConsistency() const;

public:
    /** Get the row attribute data of the data item at the specified display position (accessed in display order)
    * @param [in] itemIndex The display index of the data item, valid range: [0, GetDataItemCount())
    * @return Returns the pointer to the row attribute data, or nullptr when the index is invalid; the pointer is valid until the next data change
    */
    const ListCtrlItemData* GetDisplayItemData(size_t itemIndex) const;

    /** Whether it is the normal mode (all row heights are the default row height, no hidden rows, no always-on-top rows)
    */
    bool IsNormalMode() const;

public:
    /** Get the current cell (the current cell in cell-selection mode; returns an invalid value when not set)
        The returned row index is the current display index (stored internally by row identity, so it follows the data row after sorting/insert/delete)
    */
    ListCtrlCellId GetCurrentCell() const;

    /** Get the anchor cell (the start point of a range selection; returns an invalid value when not set)
    */
    ListCtrlCellId GetAnchorCell() const;

    /** Set the current cell (updates the current cell only, does not change the selected set)
    * @param [in] cell The cell identity (row display index + column ID)
    * @return Returns whether it was set successfully (returns false without any change when the row index or column ID is invalid)
    */
    bool SetCurrentCell(const ListCtrlCellId& cell);

    /** Set the anchor cell (updates the anchor only, does not change the selected set)
    * @param [in] cell The cell identity (row display index + column ID)
    * @return Returns whether it was set successfully (returns false without any change when the row index or column ID is invalid)
    */
    bool SetAnchorCell(const ListCtrlCellId& cell);

    /** Get the selection range of cells (anchor + current cell, not normalized; used for queries and event reporting)
    */
    void GetCellSelection(ListCtrlCellId& anchorCell, ListCtrlCellId& focusCell) const;

    /** Determine whether a single cell is selected (O(1), decided by row identity, decoupled from display order)
    * @param [in] nRowIndex The display index of the row
    * @param [in] nColumnId The column ID
    */
    bool IsCellSelected(size_t nRowIndex, size_t nColumnId) const;

    /** Set the selection state of a single cell (does not clear other selected cells)
    * @return Returns whether the selection state changed (returns false when the row is invalid/not selectable or the state is unchanged)
    */
    bool SetCellSelected(size_t nRowIndex, size_t nColumnId, bool bSelected);

    /** Select the region "display row range x specified column set"
    * @param [in] nRowBegin/nRowEnd The display index range (already normalized, begin<=end); an invalid value means the rows are not changed
    * @param [in] columnIdList The list of column IDs participating in the selection (order does not matter)
    * @param [in] bAdditive true = append to the existing selection; false = clear all cell selection first and then set
    */
    void SetCellSelectionRect(size_t nRowBegin, size_t nRowEnd,
                              const std::vector<size_t>& columnIdList, bool bAdditive);

    /** Select all cells
    * @param [in] columnIdList The list of column IDs participating in the selection
    */
    void SelectAllCells(const std::vector<size_t>& columnIdList);

    /** Clear all cell selection (the selected set, the anchor and the current cell are cleared together)
    * @return Whether an actual selection state existed before clearing (there were selected cells or a current cell);
    *         returning false means there was no selection at all, so the caller does not need to refresh the UI or dispatch selection change events
    */
    bool ClearCellSelection();

private:
    /** Sort data
    */
    struct StorageData
    {
        size_t index;             //The original data index
        const Storage* pStorage;  //The cell of this row in the sort column (may be nullptr)
    };

    /** Sort the data
    * @param [in] dataList The data to be sorted
    * @param [in] nColumnId The column ID
    * @param [in] nColumnIndex The ordinal number of the column
    * @param [in] bSortedUp true means ascending, false means descending
    * @param [in] nSortFlag The sort method flag, see the enum values of ListCtrlSubItemSortFlag
    * @param [in] pfnCompareFunc The data compare function
    * @param [in] pUserData User custom data, passed back to the compare function through the parameter when it is called
    * Note: stable sort (equal elements keep their original relative order); descending order is implemented
    *       by transposing the comparison, not by "sort ascending then reverse the whole list" (which
    *       would reverse the relative order of equal elements)
    */
    bool SortStorageData(std::vector<StorageData>& dataList, size_t nColumnId, size_t nColumnIndex,
                         bool bSortedUp, uint8_t nSortFlag,
                         ListCtrlDataCompareFunc pfnCompareFunc, void* pUserData);

    /** The default data compare function
    * @param [in] a The first data to compare
    * @param [in] b The second data to compare
    * @param [in] nSortFlag The sort method flag, see the enum values of ListCtrlSubItemSortFlag
    * @return If (a < b), returns true; otherwise returns false
    */
    bool SortDataCompareFunc(const ListCtrlSubItemData2& a, const ListCtrlSubItemData2& b, uint8_t nSortFlag) const;

    /** Update the personalized data (hidden rows, row height, always-on-top, etc.)
    */
    void UpdateNormalMode();

private:
    /** The interface of the view control
    */
    IListCtrlView* m_pListView;

    /** Whether to automatically check the selected data items
    */
    bool m_bAutoCheckSelect;

    /** The data, saved by column; each column has one paged contiguous storage
        Stored by page index (page = RowId / kPageCells), indexed inside a page by the row identity offset;
        pages are lazily allocated; deleting a row only marks that cell as not written (tombstone), so the
        data itself is not moved; the whole page is released when all of its cells become tombstones (page
        recycling); after long periods of frequent insertion/deletion, CompactStorage can be called for a
        full compaction
    */
    StorageMap m_dataMap;

    /** Row attribute data, indexed by row identity (RowId); when a row is deleted its slot is reset to the default value (tombstone)
    */
    RowDataList m_rowDataList;

    /** Display order list: m_displayOrder[i] is the row identity of the i-th current display position.
        It is the only structure that changes on insert/delete/update/sort (4 bytes per element),
        and the data itself is not moved with it
    */
    std::vector<RowId> m_displayOrder;

    /** The next row identity to allocate (monotonically increasing, never reused after deletion)
    */
    RowId m_nextRowId;

    /** A reusable buffer for filling elements, so that each row fill does not reallocate a temporary container
        (only used during the synchronous FillElement call; no state is held across calls)
    */
    std::vector<ListCtrlSubItemData2Pair> m_subItemListBuffer;

    /** The sort function set externally
    */
    ListCtrlDataCompareFunc m_pfnCompareFunc;

    /** The additional data of the sort function set externally
    */
    void* m_pUserData;

    /** The number of hidden rows
    */
    int32_t m_hideRowCount;

    /** The number of rows with non-default row height
    */
    int32_t m_heightRowCount;

    /** The number of always-on-top rows
    */
    int32_t m_atTopRowCount;

    /** Whether multi-select is supported
    */
    bool m_bMultiSelect;

    /** The row identity of the selected element in single-select mode (kInvalidRowId when none is selected)
    */
    RowId m_nSelectedRowId;

    /** The current default text attributes
    */
    int32_t m_nDefaultTextStyle;

    /** The current default row height
    */
    int32_t m_nDefaultItemHeight;

    /** Cell identity: row identity (RowId) + column ID
        Decoupled from the display order, so it still points to the same data row after rows are sorted/inserted/deleted
    */
    struct CellIdentity
    {
        RowId nRowId = kInvalidRowId;
        size_t nColumnId = Box::InvalidIndex;
        bool IsValid() const { return (nRowId != kInvalidRowId) && (nColumnId != Box::InvalidIndex); }
    };

    /** The anchor cell in cell-selection mode (the start point of dragging/range selection)
    */
    CellIdentity m_anchorCell;

    /** The current cell in cell-selection mode (the range focus)
    */
    CellIdentity m_currentCell;

    /** Cell selection set: one bitmap per column (key = column ID, value = bitmap indexed by row identity RowId, 1 means selected)
        Independent of the paged lazily allocated cell storage: selecting all / selecting rows not yet materialized does not force cell entities to be allocated;
        memory usage is "column count x row count / 8 bytes"; indexed by row identity, so the selection follows the original data row after sorting/insert/delete
    */
    std::unordered_map<size_t, std::vector<uint8_t>> m_cellSelected;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_DATA_PROVIDER_H_
