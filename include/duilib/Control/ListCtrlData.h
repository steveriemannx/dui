#ifndef UI_CONTROL_LIST_CTRL_DATA_PROVIDER_H_
#define UI_CONTROL_LIST_CTRL_DATA_PROVIDER_H_

#include "duilib/Box/VirtualListBox.h"
#include "duilib/Control/ListCtrlDefs.h"
#include <unordered_map>

namespace ui
{
/** Data manager of list items
*/
class ListCtrl;
struct ListCtrlSubItemData;
class DUILIB_API ListCtrlData : public ui::VirtualListBoxElement
{
public:
    //Data structure used for storage
    typedef ListCtrlSubItemData2 Storage;
    typedef std::shared_ptr<Storage> StoragePtr;
    typedef std::vector<StoragePtr> StoragePtrList;
    typedef std::unordered_map<size_t, StoragePtrList> StorageMap;
    typedef std::vector<ListCtrlItemData> RowDataList;

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
    bool SetSubItemText(size_t itemIndex, size_t columnId, const DString& text);

    /** Get the text of the specified data item
    * @param [in] itemIndex The index of the data item
    * @param [in] columnId The column ID
    * @return The text content associated with the data item
    */
    DString GetSubItemText(size_t itemIndex, size_t columnId) const;

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
    bool SetSubItemUserDataS(size_t itemIndex, size_t columnId, const DString& userDataS);

    /** Get the associated user data of the specified data item (string)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @return The string data associated with the data item
    */
    DString GetSubItemUserDataS(size_t itemIndex, size_t columnId) const;

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

    /** Sort the data and refresh the UI display
    * @param [in] columnId The column ID
    * @param [in] nColumnIndex The ordinal number of the column
    * @param [in] bSortedUp true means ascending, false means descending
    * @param [in] nSortFlag The sort method flag, see the enum values of ListCtrlSubItemSortFlag
    * @param [in] pfnCompareFunc The data compare function
    * @param [in] pUserData User custom data, passed back to the compare function through the parameter when it is called
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
    * @return Returns nullptr on failure
    */
    StoragePtr GetSubItemStorage(size_t itemIndex, size_t nColumnId) const;

    /** Get the data of the specified data item, for writing
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID
    * @return Returns nullptr on failure
    */
    StoragePtr GetSubItemStorageForWrite(size_t itemIndex, size_t nColumnId);

    /** Get the data of each column for UI display
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [out] subItemList Returns the data list of all columns of this row
    */
    bool GetSubItemStorageList(size_t itemIndex, std::vector<ListCtrlSubItemData2Pair>& subItemList) const;

public:
    /** Get the row attribute data
    */
    const RowDataList& GetItemDataList() const;

    /** Whether it is the normal mode (all row heights are the default row height, no hidden rows, no always-on-top rows)
    */
    bool IsNormalMode() const;

private:
    /** Sort data
    */
    struct StorageData
    {
        size_t index;       //The original data index
        StoragePtr pStorage;
    };

    /** Sort the data
    * @param [in] dataList The data to be sorted
    * @param [in] nColumnId The column ID
    * @param [in] nColumnIndex The ordinal number of the column
    * @param [in] bSortedUp true means ascending, false means descending
    * @param [in] nSortFlag The sort method flag, see the enum values of ListCtrlSubItemSortFlag
    * @param [in] pfnCompareFunc The data compare function
    * @param [in] pUserData User custom data, passed back to the compare function through the parameter when it is called
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

    /** Data, saved by column, one array per column
    */
    StorageMap m_dataMap;

    /** Row attribute data
    */
    RowDataList m_rowDataList;

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

    /** The index of the selected element in single-select mode
    */
    size_t m_nSelectedIndex;

    /** The current default text attributes
    */
    int32_t m_nDefaultTextStyle;

    /** The current default row height
    */
    int32_t m_nDefaultItemHeight;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_DATA_PROVIDER_H_
