#ifndef UI_CONTROL_LIST_CTRL_H_
#define UI_CONTROL_LIST_CTRL_H_

#include "dui/Control/ListCtrlDefs.h"
#include "dui/Control/ListCtrlHeader.h"
#include "dui/Control/ListCtrlHeaderItem.h"
#include "dui/Control/ListCtrlItem.h"
#include "dui/Control/ListCtrlSubItem.h"
#include "dui/Control/ListCtrlData.h"
#include "dui/Core/ImageList.h"

namespace ui
{
class RichEdit;
class DateTime;

class ListCtrlData;
class ListCtrlReportView;
class ListCtrlIconView;
class ListCtrlListView;
class ListCtrlHeader;
class ListCtrlHeaderItem;
class ListCtrlItem;
class ListCtrlSubItem;
class ListCtrlIconViewItem;
class ListCtrlListViewItem;
class ListCtrlComboFlyout;

/** ListCtrl control
*/
class DUI_API ListCtrl: public VBox
{
    typedef VBox BaseClass;
    friend class ListCtrlData;          //List data management container
    friend class ListCtrlReportView;    //UI control for displaying list data
    friend class ListCtrlIconView;      //UI control for displaying list data
    friend class ListCtrlListView;      //UI control for displaying list data
    friend class ListCtrlHeader;
    friend class ListCtrlHeaderItem;
    friend class ListCtrlItem;
    friend class ListCtrlSubItem;
    friend class ListCtrlIconViewItem;
    friend class ListCtrlListViewItem;
public:
    explicit ListCtrl(Window* pWindow);
    virtual ~ListCtrl() override;

    /** Get the control type
    */
    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual void HandleEvent(const EventArgs& msg) override;

    /** The DPI has changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:
    /** Set the list control type (defaults to Report type)
    */
    void SetListCtrlType(ListCtrlType type);

    /** Get the list control type
    */
    ListCtrlType GetListCtrlType() const;

    /** Set the image list; each view has an independent image list, and they can also share the same image list
    * @param [in] type The view type
    * @param [in] spImageList The image resource interface, smart pointer
    */
    void SetImageList(ListCtrlType type, const ImageListPtr& spImageList);

    /** Get the image list, to which image resources can be added for displaying icons in the list
    */
    ImageListPtr GetImageList(ListCtrlType type) const;

public:
    /** Get the number of columns
    */
    size_t GetColumnCount() const;

    /** Insert a column at the specified position
    * @param [in] columnIndex Insert the column after this column index; if it is -1, append the column at the end
    * @param [in] columnInfo The basic properties of the column
    * @return The header control interface of this column; the column ID can be obtained via the ListCtrlHeaderItem::GetColumnId() function
    */
    ListCtrlHeaderItem* InsertColumn(int32_t columnIndex, const ListCtrlColumn& columnInfo);

    /** Get the column ID
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @return The ID of the column corresponding to the column index, i.e., the column ID obtained via the ListCtrlHeaderItem::GetColumnId() function
    */
    size_t GetColumnId(size_t columnIndex) const;

    /** Check whether a column ID is valid
    * @param [in] columnId The column ID value
    */
    bool IsValidColumnId(size_t columnId) const;

    /** Get the column index
    * @param [in] columnId The column ID value, obtained via the ListCtrlHeaderItem::GetColumnId() function
    * @return The column index: [0, GetColumnCount()), indicating which column
    */
    size_t GetColumnIndex(size_t columnId) const;

    /** Get the header control interface
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] columnId The column ID value, obtained via the ListCtrlHeaderItem::GetColumnId() function
    */
    ListCtrlHeaderItem* GetColumn(size_t columnIndex) const;
    ListCtrlHeaderItem* GetColumnById(size_t columnId) const;

    /** Delete a column
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] columnId The column ID value, obtained via the ListCtrlHeaderItem::GetColumnId() function
    */
    bool DeleteColumn(size_t columnIndex);
    bool DeleteColumnById(size_t columnId);

    /** Get the column width
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] columnId The column ID value, obtained via the ListCtrlHeaderItem::GetColumnId() function
    */
    int32_t GetColumnWidth(size_t columnIndex) const;
    int32_t GetColumnWidthById(size_t columnId) const;

    /** Set the column width (auto-fit based on the actual width of the column content)
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] columnId The column ID value, obtained via the ListCtrlHeaderItem::GetColumnId() function
    * @param [in] nWidth The column width value
    * @param [in] bNeedDpiScale Whether the column width value needs DPI adaptation
    */
    bool SetColumnWidth(size_t columnIndex, int32_t nWidth, bool bNeedDpiScale);
    bool SetColumnWidthById(size_t columnId, int32_t nWidth, bool bNeedDpiScale);

    /** Automatically adjust the column width (auto-fit based on the actual width of the column content)
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] columnId The column ID value, obtained via the ListCtrlHeaderItem::GetColumnId() function
    */
    bool SetColumnWidthAuto(size_t columnIndex);
    bool SetColumnWidthAutoById(size_t columnId);

    /** Show/hide a column
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] columnId The column ID, obtained through ListCtrlHeaderItem::GetColumnId()
    * @param [in] bColumnVisible true means show the column, false means hide the column
    * @return Returns true if the operation succeeded, false if the parameters are invalid
    */
    bool SetColumnVisible(size_t columnIndex, bool bColumnVisible);
    bool SetColumnVisibleById(size_t columnId, bool bColumnVisible);

    /** Determine whether a column is visible
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] columnId The column ID, obtained through ListCtrlHeaderItem::GetColumnId()
    * @return Returns true if the column is visible, false if the column is hidden (or the parameters are invalid)
    */
    bool IsColumnVisible(size_t columnIndex) const;
    bool IsColumnVisibleById(size_t columnId) const;

    /** Set the widths of all columns according to the given width values
    * @param [in] columnWidthList The index of the container corresponds to the column index, and the value of the container represents the expected width of each column
    *             The column width supports three types: fixed value, auto-fit, and stretch.
    *             Fixed value: same behavior as the SetColumnWidth function above
    *             Auto-fit: same behavior as the SetColumnWidthAuto function above
    *             Stretch: operates according to the following rules:
    *             (1) Based on the total width of the list view, all stretch-type columns are allocated the available width according to the specified percentages (available width = total list view width - fixed width - auto width)
    *             (2) If the available width is insufficient (i.e., 0), keep the original widths of the related columns and do nothing
    *             (3) Example 1: custom ratios; assuming three columns in total, columnWidthList contains three values: UiFixedInt::MakeStretch(30),UiFixedInt::MakeStretch(30),UiFixedInt::MakeStretch(40)
    *                        meaning the widths of the three columns are 30%, 30%, and 40% of the total list view width respectively
    *             (4) Example 2: the widths of the columns are evenly distributed; stretch values can be set directly without calculating specific ratios: columnWidthList contains three values: UiFixedInt::MakeStretch(),UiFixedInt::MakeStretch(),UiFixedInt::MakeStretch()
    * @param [in] bNeedDpiScale Whether the column width value needs DPI adaptation (only used when the column width is set to a fixed value)
    */
    bool SetColumnWidth(const std::vector<UiFixedInt>& columnWidthList, bool bNeedDpiScale);
    
    /** Set whether dragging the header can change the order of the columns
    */
    void SetEnableHeaderDragOrder(bool bEnable);

    /** Whether dragging the header can change the order of the columns
    */
    bool IsEnableHeaderDragOrder() const;

    /** Set the number of frozen columns (the first N columns at the left of the list are fixed and do not move with horizontal scrolling)
    * @param [in] nCount The number of frozen columns; 0 means no column is frozen
    */
    void SetFrozenColumnCount(size_t nCount);

    /** Get the number of frozen columns
    */
    size_t GetFrozenColumnCount() const;

    /** Set whether to show the header control
    */
    void SetHeaderVisible(bool bVisible);

    /** Whether the header control is currently shown
    */
    bool IsHeaderVisible() const;

    /** Set the height of the header control
    * @param[in] bNeedDpiScale If true, the width needs DPI adaptation
    */
    void SetHeaderHeight(int32_t nHeaderHeight, bool bNeedDpiScale);

    /** Get the height of the header control
    */
    int32_t GetHeaderHeight() const;

    /** Set the sort flag of a column's data (by column index)
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] columnId The column ID value
    * @param [in] nSortFlag The sort method flag, see the ListCtrlSubItemSortFlag enum values
    */
    void SetColumnSortFlag(size_t columnIndex, uint8_t nSortFlag);
    void SetColumnSortFlagById(size_t columnId, uint8_t nSortFlag);

    /** Get the sort flag of a column's data (by column index)
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] columnId The column ID value
    * return The sort method flag, see the ListCtrlSubItemSortFlag enum values
    */
    uint8_t GetColumnSortFlag(size_t columnIndex);
    uint8_t GetColumnSortFlagById(size_t columnId);

    /** Get the header control interface; it has a value only after the control is initialized
    */
    ListCtrlHeader* GetHeaderCtrl() const;

public:
    /** Get the total number of data items (number of data rows)
    */
    size_t GetDataItemCount() const;

    /** Set the total number of data items (corresponding to the number of rows)
    * @param [in] itemCount The total number of data items
    */
    bool SetDataItemCount(size_t itemCount);

    /** Add a data item at the end (rows + 1), and the data is associated with the first column (column index 0)
    * @param [in] dataItem The content of the data item
    * @return The index of the data item on success, valid range: [0, GetDataItemCount()); returns Box::InvalidIndex on failure
    */
    size_t AddDataItem(const ListCtrlSubItemData& dataItem);

    /** Insert a data item at the specified row position (rows + 1)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] dataItem The content of the data item
    */
    bool InsertDataItem(size_t itemIndex, const ListCtrlSubItemData& dataItem);

    /** Delete the data item of the specified row (rows - 1)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    */
    bool DeleteDataItem(size_t itemIndex);

    /** Delete the data items of all rows (row count becomes 0)
    */
    bool DeleteAllDataItems();

    /** Delete the data items of the specified rows in batch (K rows)
    * @param [in] itemIndexs The list of indexes of the data items to delete, valid range: [0, GetDataItemCount());
    *                        duplicate indexes and out-of-range indexes are ignored
    * @return Returns the number of rows actually deleted
    */
    size_t DeleteDataItems(const std::vector<size_t>& itemIndexs);

    /** Compact the data storage: reorganize the row identities and the storage page layout, and reclaim the memory occupied by deleted rows
        Note: this is an explicit whole-table maintenance operation, which may only be called while no filling/painting is in progress; after the call, the raw cell pointers already handed out internally become invalid
    */
    void CompactStorage();

    /** Set the default height of data items (row height)
    * @param [in] nItemHeight The height value
    * @param [in] bNeedDpiScale If true, DPI adaptation is performed on the width
    */
    void SetDataItemHeight(int32_t nItemHeight, bool bNeedDpiScale);

    /** Get the default height of data items (row height)
    */
    int32_t GetDataItemHeight() const;

public:
    /** Set the row attribute data of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] itemData The associated data
    */
    bool SetDataItemData(size_t itemIndex, const ListCtrlItemData& itemData);

    /** Get the row attribute data of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] itemData The associated data
    */
    bool GetDataItemData(size_t itemIndex, ListCtrlItemData& itemData) const;

    /** Set the visibility of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] bVisible Whether it is visible
    */
    bool SetDataItemVisible(size_t itemIndex, bool bVisible);

    /** Get the visibility of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @return The visibility associated with the data item
    */
    bool IsDataItemVisible(size_t itemIndex) const;

    /** Set the selection attribute of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] bSelected Whether it is in the selected state
    */
    bool SetDataItemSelected(size_t itemIndex, bool bSelected);

    /** Get the selection attribute of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @return The selection state associated with the data item
    */
    bool IsDataItemSelected(size_t itemIndex) const;

    /** Set the icon at the beginning of the row
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] imageId The icon resource ID; if it is -1, no icon is displayed at the beginning of the row. The ID is generated by ImageList
    */
    bool SetDataItemImageId(size_t itemIndex, int32_t imageId);

    /** Get the icon at the beginning of the row
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    */
    int32_t GetDataItemImageId(size_t itemIndex) const;

    /** Set the always-on-top state of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] nAlwaysAtTop The always-on-top state: -1 means not on top, 0 or a positive number means on top; the larger the value, the higher the priority, displayed at the top first
    */
    bool SetDataItemAlwaysAtTop(size_t itemIndex, int8_t nAlwaysAtTop);

    /** Get the always-on-top state of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @return The always-on-top state associated with the data item: -1 means not on top, 0 or a positive number means on top; the larger the value, the higher the priority, displayed at the top first
    */
    int8_t GetDataItemAlwaysAtTop(size_t itemIndex) const;

    /** Set the row height of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] nItemHeight The row height: -1 means using the default row height set by ListCtrl, other values mean the row height set for this row
    * @param [in] bNeedDpiScale If true, the width needs DPI adaptation
    */
    bool SetDataItemHeight(size_t itemIndex, int32_t nItemHeight, bool bNeedDpiScale);

    /** Get the row height of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @return The row height associated with the data item: -1 means using the default row height set by ListCtrl, other values mean the row height set for this row
    */
    int32_t GetDataItemHeight(size_t itemIndex) const;

    /** Set the custom data of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] userData The custom data associated with the data item
    */
    bool SetDataItemUserData(size_t itemIndex, size_t userData);

    /** Get the custom data of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @return The custom data associated with the data item
    */
    size_t GetDataItemUserData(size_t itemIndex) const;

public:
    /** Set the data item of the specified <row, column>
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] subItemData The content of the specified data item; the column index is specified in dataItem.nColumnIndex
    */
    bool SetSubItemData(size_t itemIndex, size_t columnIndex, const ListCtrlSubItemData& subItemData);
    bool SetSubItemDataById(size_t itemIndex, size_t columnId, const ListCtrlSubItemData& subItemData);

    /** Get the data item of the specified <row, column>
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [out] subItemData The content of the specified data item
    */
    bool GetSubItemData(size_t itemIndex, size_t columnIndex, ListCtrlSubItemData& subItemData) const;
    bool GetSubItemDataById(size_t itemIndex, size_t columnId, ListCtrlSubItemData& subItemData) const;

public:
    /** Set the text of the specified data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] text The text content to set
    */
    bool SetSubItemText(size_t itemIndex, size_t columnIndex, const std::string& text);
    bool SetSubItemTextById(size_t itemIndex, size_t columnId, const std::string& text);

    /** Get the text of the specified data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @return The text content associated with the data item
    */
    std::string GetSubItemText(size_t itemIndex, size_t columnIndex) const;
    std::string GetSubItemTextById(size_t itemIndex, size_t columnId) const;

    /** Set the sort group of the specified data item (integer)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] nSortGroup The integer data of the sort group to set
    */
    bool SetSubItemSortGroup(size_t itemIndex, size_t columnIndex, int32_t nSortGroup);
    bool SetSubItemSortGroupById(size_t itemIndex, size_t columnId, int32_t nSortGroup);

    /** Get the sort group of the specified data item (integer)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @return The integer data of the sort group
    */
    int32_t GetSubItemGroup(size_t itemIndex, size_t columnIndex) const;
    int32_t GetSubItemGroupById(size_t itemIndex, size_t columnId) const;

    /** Set the associated user data of the specified data item (integer)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] userDataN The integer data associated with the data item to set
    */
    bool SetSubItemUserDataN(size_t itemIndex, size_t columnIndex, uint64_t userDataN);
    bool SetSubItemUserDataNById(size_t itemIndex, size_t columnId, uint64_t userDataN);

    /** Get the associated user data of the specified data item (integer)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @return The integer data associated with the data item
    */
    uint64_t GetSubItemUserDataN(size_t itemIndex, size_t columnIndex) const;
    uint64_t GetSubItemUserDataNById(size_t itemIndex, size_t columnId) const;

    /** Set the associated user data of the specified data item (string)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] userDataS The string data associated with the data item to set
    */
    bool SetSubItemUserDataS(size_t itemIndex, size_t columnIndex, const std::string& userDataS);
    bool SetSubItemUserDataSById(size_t itemIndex, size_t columnId, const std::string& userDataS);

    /** Get the associated user data of the specified data item (string)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @return The string data associated with the data item
    */
    std::string GetSubItemUserDataS(size_t itemIndex, size_t columnIndex) const;
    std::string GetSubItemUserDataSById(size_t itemIndex, size_t columnId) const;

public:
    /** Set the text color of the specified data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] textColor The text color to set
    */
    bool SetSubItemTextColor(size_t itemIndex, size_t columnIndex, const UiColor& textColor);
    bool SetSubItemTextColorById(size_t itemIndex, size_t columnId, const UiColor& textColor);

    /** Get the text color of the specified data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    */
    UiColor GetSubItemTextColor(size_t itemIndex, size_t columnIndex) const;
    UiColor GetSubItemTextColorById(size_t itemIndex, size_t columnId) const;

    /** Set the text attributes of the specified data item (text alignment, etc.)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] nTextFormat The text attributes to set, see the enum type: enum DrawStringFormat in IRender.h
    */
    bool SetSubItemTextFormat(size_t itemIndex, size_t columnIndex, int32_t nTextFormat);
    bool SetSubItemTextFormatById(size_t itemIndex, size_t columnId, int32_t nTextFormat);

    /** Get the text attributes of the specified data item (text alignment, etc.)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @return The text attributes associated with the data item, see the enum type: enum DrawStringFormat in IRender.h
    */
    int32_t GetSubItemTextFormat(size_t itemIndex, size_t columnIndex) const;
    int32_t GetSubItemTextFormatById(size_t itemIndex, size_t columnId) const;

    /** Set whether the text of the specified data item is displayed on a single line (Report view only)
        Note: the Report view is a virtual list and its cell controls are reused while scrolling, so this attribute is stored in the data model
              and the framework applies it to the reused control when the cell data is filled; after it is set to multi-line (false), the text is wrapped
              automatically according to the cell width, and in that case the row height must be adjusted by the caller through SetDataItemHeight, otherwise the overflow is clipped
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] bSingleLine true means single-line display (clipped with an ellipsis when overflowing), false means multi-line display (wrapped automatically)
    */
    bool SetSubItemSingleLine(size_t itemIndex, size_t columnIndex, bool bSingleLine);
    bool SetSubItemSingleLineById(size_t itemIndex, size_t columnId, bool bSingleLine);

    /** Get whether the text of the specified data item is displayed on a single line (Report view only)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @return true means single-line display, false means multi-line display (wrapped automatically)
    */
    bool IsSubItemSingleLine(size_t itemIndex, size_t columnIndex) const;
    bool IsSubItemSingleLineById(size_t itemIndex, size_t columnId) const;

    /** Set the background color of the specified data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] bkColor The background color to set
    */
    bool SetSubItemBkColor(size_t itemIndex, size_t columnIndex, const UiColor& bkColor);
    bool SetSubItemBkColorById(size_t itemIndex, size_t columnId, const UiColor& bkColor);

    /** Get the background color of the specified data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    */
    UiColor GetSubItemBkColor(size_t itemIndex, size_t columnIndex) const;
    UiColor GetSubItemBkColorById(size_t itemIndex, size_t columnId) const;

    /** Whether to show the CheckBox
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    */
    bool IsSubItemShowCheckBox(size_t itemIndex, size_t columnIndex) const;
    bool IsSubItemShowCheckBoxById(size_t itemIndex, size_t columnId) const;

    /** Set whether to show the CheckBox
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] bShowCheckBox true means show, false means hide
    */
    bool SetSubItemShowCheckBox(size_t itemIndex, size_t columnIndex, bool bShowCheckBox);
    bool SetSubItemShowCheckBoxById(size_t itemIndex, size_t columnId, bool bShowCheckBox);

    /** Set the checked state of the CheckBox
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] bChecked true means checked, false means unchecked
    */
    bool SetSubItemCheck(size_t itemIndex, size_t columnIndex, bool bChecked);
    bool SetSubItemCheckById(size_t itemIndex, size_t columnId, bool bChecked);

    /** Get the checked state of the CheckBox
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    */
    bool IsSubItemChecked(size_t itemIndex, size_t columnIndex) const;
    bool IsSubItemCheckedById(size_t itemIndex, size_t columnId) const;

    /** Set the icon of this column
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] imageId The icon resource ID; if it is -1, no icon is displayed at the beginning of the row. The ID is generated by ImageList
    */
    bool SetSubItemImageId(size_t itemIndex, size_t columnIndex, int32_t imageId);
    bool SetSubItemImageIdById(size_t itemIndex, size_t columnId, int32_t imageId);

    /** Get the icon of this column
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    */
    int32_t GetSubItemImageId(size_t itemIndex, size_t columnIndex) const;
    int32_t GetSubItemImageIdById(size_t itemIndex, size_t columnId) const;

    /** Set whether the text of this column is editable
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] bEditable true means editable, false means not editable
    */
    bool SetSubItemEditable(size_t itemIndex, size_t columnIndex, bool bEditable);
    bool SetSubItemEditableById(size_t itemIndex, size_t columnId, bool bEditable);

    /** Get whether the text of this column is editable
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    */
    bool IsSubItemEditable(size_t itemIndex, size_t columnIndex) const;
    bool IsSubItemEditableById(size_t itemIndex, size_t columnId) const;

    /** Set the column to be displayed as a progress bar
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] nProgress The progress value, valid range [0, 100]; -1 means the progress bar is not displayed (the text is displayed as normal text)
    */
    bool SetSubItemProgress(size_t itemIndex, size_t columnIndex, int32_t nProgress);
    bool SetSubItemProgressById(size_t itemIndex, size_t columnId, int32_t nProgress);

    /** Get the progress value of the column
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @return Returns the progress value, valid range [0, 100]; -1 means the progress bar is not displayed (the text is displayed as normal text)
    */
    int32_t GetSubItemProgress(size_t itemIndex, size_t columnIndex) const;
    int32_t GetSubItemProgressById(size_t itemIndex, size_t columnId) const;

    /** Set whether the text of the column is rendered as rich text (HTML-like format)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] bRichText true means render as rich text, false means render as normal text
    */
    bool SetSubItemRichText(size_t itemIndex, size_t columnIndex, bool bRichText);
    bool SetSubItemRichTextById(size_t itemIndex, size_t columnId, bool bRichText);

    /** Get whether the text of the column is rendered as rich text (HTML-like format)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    */
    bool IsSubItemRichText(size_t itemIndex, size_t columnIndex) const;
    bool IsSubItemRichTextById(size_t itemIndex, size_t columnId) const;

public:
    /** Sort the data
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] bSortedUp true means ascending, false means descending
    * @param [in] nSortFlag The sort method flag, see the enum values of ListCtrlSubItemSortFlag
    * @param [in] pfnCompareFunc The custom compare function; if nullptr, the default compare function is used
    * @param [in] pUserData User-defined data, passed back to the compare function through the parameter when the comparison function is called
    * Note: the sort is stable (std::stable_sort); elements whose comparison result is equal (same value) keep their original relative order,
    *       so rows in the same group (e.g. folders/files) or with the same value are not reordered or interleaved by sorting; this holds for both ascending and descending order.
    */
    bool SortDataItems(size_t columnIndex, bool bSortedUp, uint8_t nSortFlag = ListCtrlSubItemSortFlag::kDefault,
                       ListCtrlDataCompareFunc pfnCompareFunc = nullptr,
                       void* pUserData = nullptr);
    bool SortDataItemsById(size_t columnId, bool bSortedUp, uint8_t nSortFlag = ListCtrlSubItemSortFlag::kDefault,
                           ListCtrlDataCompareFunc pfnCompareFunc = nullptr,
                           void* pUserData = nullptr);

    /** Set an external custom sort function, replacing the default sort function
    * @param [in] pfnCompareFunc The data compare function
    * @param [in] pUserData User custom data, passed back to the compare function through the parameter when it is called
    */
    void SetSortCompareFunction(ListCtrlDataCompareFunc pfnCompareFunc, void* pUserData);

public:
    /** Whether multi-select is supported
    */
    bool IsMultiSelect() const;

    /** Set whether multi-select is supported
    */
    void SetMultiSelect(bool bMultiSelect);

    /** Set selected data items in batch (row selection only)
        Note: only effective in row selection mode (kRow); in cell selection mode (kCell), the row selection is not modified at all (no-op)
    * @param [in] selectedIndexs The indexes of the data items to be selected, valid range: [0, GetDataItemCount())
    * @param [in] bClearOthers If true, clear the selection of other already-selected items, keeping only the ones set this time
    */
    void SetSelectedDataItems(const std::vector<size_t>& selectedIndexs, bool bClearOthers);

    /** Get the list of selected data items (row selection only)
        Note: only returns a result in row selection mode (kRow); in cell selection mode (kCell) an empty list is returned
    * @param [in] itemIndexs Returns the indexes of the currently selected data items, valid range: [0, GetDataItemCount())
    */
    void GetSelectedDataItems(std::vector<size_t>& itemIndexs) const;

    /** Select all (dispatched according to the current selection mode: row selection mode selects all rows, cell selection mode selects all cells)
    */
    void SetSelectAll();

    /** Cancel all selections (dispatched according to the current selection mode: row selection mode clears the row selection, cell selection mode clears the cell selection)
    */
    void SetSelectNone();

public:
    /** Get the selection mode (row selection mode / cell selection mode)
    */
    ListCtrlSelectionMode GetSelectionMode() const;

    /** Set the selection mode (the default is row selection mode)
        Note: switching to cell selection mode automatically clears the current row selection; switching back to row selection mode automatically clears the cell selection
    * @param [in] mode The selection mode
    */
    void SetSelectionMode(ListCtrlSelectionMode mode);

    /** Get the current cell (only valid in cell selection mode; when invalid, ListCtrlCellId::IsValid() of the returned value is false)
    */
    ListCtrlCellId GetCurrentCell() const;

    /** Set the current cell (only valid in cell selection mode; cannot be used in row selection mode)
    * @param [in] nRowIndex The display index of the row, valid range: [0, GetDataItemCount())
    * @param [in] nColumnId The column ID
    * @return Returns whether the operation succeeded (returns false if the row index or the column ID is invalid, or the current mode is not cell selection mode)
    */
    bool SetCurrentCell(size_t nRowIndex, size_t nColumnId);

    /** Set the selection area of cells (anchor + focus, forming a normalized rectangle)
    * @param [in] anchorCell The anchor cell
    * @param [in] focusCell The focus cell (when invalid, only the anchor cell is set and the selection set is not changed)
    * @param [in] bAdditive true = append this rectangular area to the existing selection (without clearing it); false = clear the existing cell selection first and then set it
    * @return Returns whether the operation succeeded (returns false if the anchor is invalid or the current mode is not cell selection mode)
    */
    bool SetCellSelection(const ListCtrlCellId& anchorCell, const ListCtrlCellId& focusCell, bool bAdditive);

    /** Clear the selection area of cells
        The UI is refreshed and the kEventCellSelectionChanged event is dispatched only when a selection state really exists (there are selected cells or a current cell)
    */
    void ClearCellSelection();

    /** Get the selection area of cells (anchor + focus, not normalized)
    */
    void GetCellSelection(ListCtrlCellId& anchorCell, ListCtrlCellId& focusCell) const;

    /** Set selected cells in batch (the cell-selection equivalent API, corresponding to SetSelectedDataItems for row selection)
        Note: only effective in cell selection mode (kCell); in row selection mode (kRow) nothing is modified (no-op)
    * @param [in] cellList The list of cells to set as selected
    * @param [in] bClearOthers If true, clear the other selected cells first, keeping only the ones set this time
    */
    void SetSelectedCells(const std::vector<ListCtrlCellId>& cellList, bool bClearOthers);

    /** Get the list of selected cells (the cells expanded inside the normalized area, in row-major order)
    * @param [in] cellList Returns the list of selected cells
    * @return Returns the number of selected cells
    * Note: all selected cells are returned; cell merging (spanning rows/columns) was decided not to be implemented after review
    *       (it would require an overall upgrade of the layout model of the header and the rows, which is too large a change), so there is no "cell covered by a merge".
    */
    size_t GetSelectedCells(std::vector<ListCtrlCellId>& cellList) const;

    /** Determine whether a cell is inside the selected area
    * @param [in] nRowIndex The display index of the row, valid range: [0, GetDataItemCount())
    * @param [in] nColumnId The column ID
    */
    bool IsCellSelected(size_t nRowIndex, size_t nColumnId) const;

    /** Determine whether a cell is the current cell
    * @param [in] nRowIndex The display index of the row, valid range: [0, GetDataItemCount())
    * @param [in] nColumnId The column ID
    */
    bool IsCellCurrent(size_t nRowIndex, size_t nColumnId) const;

public:
    /** Get the list of currently displayed data items, in order from top to bottom
    * @param [in] itemIndexList The list of indexes of the currently displayed data items; each element's valid range: [0, GetDataItemCount())
    */
    void GetDisplayDataItems(std::vector<size_t>& itemIndexList) const;

    /** Get the index of the first data item in the visible range
    * @return The index of the data item, valid range: [0, GetDataItemCount())
    */
    size_t GetTopDataItem() const;

    /** Check whether a data item is visible
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    */
    bool IsDataItemDisplay(size_t itemIndex) const;

    /** Ensure the data item is visible
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] bToTop Whether to ensure it is at the top
    */
    bool EnsureDataItemVisible(size_t itemIndex, bool bToTop);

    /** Refresh the UI to keep the data and display in sync
    * @param [in] bSync true means synchronous refresh, redrawing the current view immediately; false means asynchronous refresh, the refresh action is an async redraw, marking the dirty area and leaving the redraw to the system
    */
    void Refresh(bool bSync = false);

    /** Refresh the specified data items
    * @param [in] dataItemIndexs The indexes of the data items to refresh, valid range: [0, GetDataItemCount())
    */
    void RefreshDataItems(const std::vector<size_t>& dataItemIndexs);

    /** Refresh the specified cell (a single-cell refresh, valid for the Report view only; used for local updates of progress/state columns so that the whole row need not be refreshed)
    * @param [in] dataItemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @return Returns true if the update succeeded (the cell is currently in the visible range and has been refreshed);
    *         returning false means the parameters are invalid, or the cell is currently not in the visible range (the data itself has been updated, and the latest data is shown once the cell is scrolled into view)
    */
    bool RefreshSubItem(size_t dataItemIndex, size_t columnIndex);

    /** Refresh the specified cell (a single-cell refresh, valid for the Report view only; used for local updates of progress/state columns so that the whole row need not be refreshed)
    * @param [in] dataItemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnId The column ID, obtained through ListCtrlHeaderItem::GetColumnId()
    * @return Returns true if the update succeeded (the cell is currently in the visible range and has been refreshed);
    *         returning false means the parameters are invalid, or the cell is currently not in the visible range (the data itself has been updated, and the latest data is shown once the cell is scrolled into view)
    */
    bool RefreshSubItemById(size_t dataItemIndex, size_t columnId);

    /** Whether refreshing the UI is allowed
    * @param [in] bEnable If true, refreshing the UI through the Refresh family of functions is allowed; if false, refreshing the UI through the Refresh family of functions is disabled,
                          and the UI is not refreshed automatically when the data changes either
    * @return Returns the previous IsEnableRefresh() state
    */
    bool SetEnableRefresh(bool bEnable);

    /** Check whether refreshing the UI is allowed
    */
    bool IsEnableRefresh() const;

public:
    /** Get the list data display interface (Report view)
    */
    ListCtrlReportView* GetReportView() const;

    /** Get the list data display interface (Icon view)
    */
    ListCtrlIconView* GetIconView() const;

    /** Get the list data display interface (List view)
    */
    ListCtrlListView* GetListView() const;

public:
    /** Get the UI control interface of the first data item (Report view)
    * @return The UI control interface
              Note: after the UI is refreshed, the relationship between UI controls and data elements changes dynamically
    */
    ListCtrlItem* GetFirstDisplayItem() const;

    /** Get the UI control interface of the next data item (Report view)
    * @param [in] pItem The current UI control interface
    * @return The UI control interface next to the current one; if the current one is already the last, returns nullptr
    *         Note: after the UI is refreshed, the relationship between UI controls and data elements changes dynamically
    */
    ListCtrlItem* GetNextDisplayItem(ListCtrlItem* pItem) const;

public:
    /** Get the UI control interface of the first data item (Icon view)
    * @return The UI control interface
              Note: after the UI is refreshed, the relationship between UI controls and data elements changes dynamically
    */
    ListCtrlIconViewItem* GetFirstDisplayIconItem() const;

    /** Get the UI control interface of the next data item (Icon view)
    * @param [in] pItem The current UI control interface
    * @return The UI control interface next to the current one; if the current one is already the last, returns nullptr
    *         Note: after the UI is refreshed, the relationship between UI controls and data elements changes dynamically
    */
    ListCtrlIconViewItem* GetNextDisplayIconItem(ListCtrlIconViewItem* pItem) const;

public:
    /** Get the UI control interface of the first data item (List view)
    * @return The UI control interface
              Note: after the UI is refreshed, the relationship between UI controls and data elements changes dynamically
    */
    ListCtrlListViewItem* GetFirstDisplayListItem() const;

    /** Get the UI control interface of the next data item (List view)
    * @param [in] pItem The current UI control interface
    * @return The UI control interface next to the current one; if the current one is already the last, returns nullptr
    *         Note: after the UI is refreshed, the relationship between UI controls and data elements changes dynamically
    */
    ListCtrlListViewItem* GetNextDisplayListItem(ListCtrlListViewItem* pItem) const;

public:
    /** Set whether sub-item editing is supported
    */
    void SetEnableItemEdit(bool bEnableItemEdit);

    /** Check whether sub-item editing is supported
    */
    bool IsEnableItemEdit() const;

    /** Set whether copying the selected content to the clipboard is supported (Ctrl+C, TSV format; default true)
    */
    void SetEnableCopy(bool bEnableCopy);

    /** Determine whether copying the selected content to the clipboard is supported
    */
    bool IsEnableCopy() const;

    /** Set whether CSV export is supported (Ctrl+Shift+C copies as CSV, and ExportToCsvFile; default true)
    */
    void SetEnableCsvExport(bool bEnableCsvExport);

    /** Determine whether CSV export is supported
    */
    bool IsEnableCsvExport() const;

    /** Copy the currently selected content to the clipboard
    * @param [in] bCsvFormat true means the CSV format (controlled by the enable_csv_export switch),
    *                       false means the TSV format (controlled by the enable_copy switch)
    * @return Returns true if copying succeeded; returns false when the switch is off, there is no selected content, or writing to the clipboard fails
    */
    bool CopySelectionToClipboard(bool bCsvFormat = false);

    /** Export to a CSV file (UTF-8 with BOM encoding, so that Excel can open it directly)
    * @param [in] strFilePath The CSV file path (provided by the application, an absolute path)
    * @param [in] bSelectedOnly true means export only the currently selected content, false means export the whole table
    * @return Returns true if the export succeeded; returns false when the enable_csv_export switch is off, there is no data, or writing the file fails
    */
    bool ExportToCsvFile(const std::string& strFilePath, bool bSelectedOnly = true);

public:
    /** @name Cell editing (kCell mode; Report view only)
    * @{ */

    /** Set the editor type by column index (applies to all cells of that column)
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] editorType The editor type (text/number/date)
    */
    void SetColumnEditorType(size_t columnIndex, ListCtrlEditorType editorType);

    /** Set the editor type by column ID (recommended: the column ID is stable while the column order may change)
    * @param [in] columnId The column ID
    * @param [in] editorType The editor type
    */
    void SetColumnEditorTypeById(size_t columnId, ListCtrlEditorType editorType);

    /** Get the editor type of a column index (kEditText when it has not been set)
    */
    ListCtrlEditorType GetColumnEditorType(size_t columnIndex) const;

    /** Get the editor type of a column ID (kEditText when it has not been set)
    */
    ListCtrlEditorType GetColumnEditorTypeById(size_t columnId) const;

    /** Set the list of candidate values by column index (used by the kEditCombo editor)
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] candidates The list of candidate values (plain text, displayed in the order they are passed in); passing an empty list clears the candidates
    */
    void SetColumnCandidates(size_t columnIndex, const std::vector<std::string>& candidates);

    /** Set the list of candidate values by column ID (recommended: the column ID is stable while the column order may change)
    * @param [in] columnId The column ID
    * @param [in] candidates The list of candidate values (plain text, displayed in the order they are passed in); passing an empty list clears the candidates
    */
    void SetColumnCandidatesById(size_t columnId, const std::vector<std::string>& candidates);

    /** Get the list of candidate values of a column index (an empty list when it has not been set)
    */
    std::vector<std::string> GetColumnCandidates(size_t columnIndex) const;

    /** Get the list of candidate values of a column ID (an empty list when it has not been set)
    */
    std::vector<std::string> GetColumnCandidatesById(size_t columnId) const;

    /** Enter the edit state of the specified cell (equivalent to pressing F2 on the current cell)
    * Preconditions: enable_item_edit is true, kCell mode, Report view, the cell is "editable" and its row is selectable;
    *           if the row is not in the visible area, it is scrolled into the visible area first
    * @param [in] nRowIndex The display index of the row, valid range: [0, GetDataItemCount())
    * @param [in] nColumnId The column ID
    * @return Returns true if the edit state has been entered (or is already being edited)
    */
    bool EnterCellEdit(size_t nRowIndex, size_t nColumnId);

    /** Enter the edit state of the "current cell" (the implementation entry point of F2)
    * @return Returns true if the edit state has been entered
    */
    bool EnterCurrentCellEdit();

    /** Determine whether the control is currently in the cell edit state (kCell mode)
    */
    bool IsCellEditing() const;

    /** Get the cell currently being edited
    * @param [out] cell Returns the cell location (nRowIndex / nColumnId are Box::InvalidIndex when there is no edit state)
    * @return Returns whether the cell edit state is active
    */
    bool GetEditingCell(ListCtrlCellId& cell) const;

    /** @} */

public:
    /** Get the interface of the edit control (it has a value only after an edit operation is performed)
    */
    RichEdit* GetRichEdit() const;

public:
    /** @name CheckBox (row-level) related functions
    * @{ */

    /** Set whether to automatically check the selected data items (applies to the Header and each row)
    */
    void SetAutoCheckSelect(bool bAutoCheckSelect);

    /** Get whether to automatically check the selected data items
    */
    bool IsAutoCheckSelect() const;

    /** Set whether to show the CheckBox at the far left of the header
    * @param [in] bShow true means show the CheckBox, false means hide it
    */
    bool SetHeaderShowCheckBox(bool bShow);

    /** Check whether the CheckBox is shown at the beginning of each row
    */
    bool IsHeaderShowCheckBox() const;

    /** Set whether to show the CheckBox at the beginning of each row
    * @param [in] bShow true means show the CheckBox, false means hide it
    */
    void SetDataItemShowCheckBox(bool bShow);

    /** Check whether the CheckBox is shown at the beginning of each row
    */
    bool IsDataItemShowCheckBox() const;

    /** Set the check attribute of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] bCheck Whether it is in the checked state
    */
    bool SetDataItemCheck(size_t itemIndex, bool bCheck);

    /** Get the check attribute of a data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @return The checked state associated with the data item
    */
    bool IsDataItemCheck(size_t itemIndex) const;

    /** Set checked data items in batch (the data checked via the CheckBox at the beginning of each row)
    * @param [in] itemIndexs The indexes of the data items to be checked, valid range: [0, GetDataItemCount())
    * @param [in] bClearOthers If true, clear the selection of other already-selected items, keeping only the ones set this time
    */
    void SetCheckedDataItems(const std::vector<size_t>& itemIndexs, bool bClearOthers);

    /** Get the list of checked data items (the data checked via the CheckBox at the beginning of each row)
    * @param [in] itemIndexs Returns the indexes of the currently checked data items, valid range: [0, GetDataItemCount())
    */
    void GetCheckedDataItems(std::vector<size_t>& itemIndexs) const;

    /** @} */

public:
    /** The width of the horizontal grid lines
    * @param [in] nLineWidth The width of the grid lines; if it is 0, the horizontal grid lines are not shown
    * @param [in] bNeedDpiScale If true, the width needs DPI adaptation
    */
    void SetRowGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale);
    int32_t GetRowGridLineWidth() const;

    /** The color of the horizontal grid lines
    * @param [in] color The color of the horizontal grid lines
    */
    void SetRowGridLineColor(const std::string& color);
    std::string GetRowGridLineColor() const;

    /** The width of the vertical grid lines
    * @param [in] nLineWidth The width of the grid lines; if it is 0, the vertical grid lines are not shown
    * @param [in] bNeedDpiScale If true, the width needs DPI adaptation
    */
    void SetColumnGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale);
    int32_t GetColumnGridLineWidth() const;

    /** The color of the vertical grid lines
    * @param [in] color The color of the vertical grid lines
    */
    void SetColumnGridLineColor(const std::string& color);
    std::string GetColumnGridLineColor() const;

    /** Whether double-clicking the Header's splitter automatically adjusts the column width
    */
    void SetEnableColumnWidthAuto(bool bEnable);
    bool IsEnableColumnWidthAuto() const;

    /** Get the ID and sort order of the currently sorted column
    * @param [out] nSortColumnId The ID of the sorted column
    * @param [out] bSortUp Whether the current sort is ascending; true means ascending, false means descending
    * @return true means there is a sorted column, false means there is none
    */
    bool GetSortColumn(size_t& nSortColumnId, bool& bSortUp) const;

public:
    /** Listen to the event of selecting an item
     * @param [in] callback The callback function for event handling
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item     
     *   listCtrlType: The list type, valid values: ListCtrlType::Report, ListCtrlType::Icon, ListCtrlType::List
     *                 If listCtrlType is any other value, the event is not associated with child item data
     *   pEventData: The pointer to the UI control interface of the selected item; possible types: ListCtrlItem* pointer, ListCtrlIconViewItem* pointer, ListCtrlListViewItem* pointer
     *               The value of listCtrlType can be converted to ListCtrlType, and then determine which specific type to convert to based on the enum value
     */
    void AttachSelect(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelect, callback, callbackID); }

    /** Listen to the event of the selected item changing
     * @param[in] callback The callback function when an item is selected
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     */
    void AttachSelChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelChanged, callback, callbackID); }

    /** Listen to the double-click event
     * @param [in] callback The callback function for event handling
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item     
     *   listCtrlType: The list type, valid values: ListCtrlType::Report, ListCtrlType::Icon, ListCtrlType::List
     *                 If listCtrlType is any other value, the event is not associated with child item data
     *   pEventData: The pointer to the UI control interface of the selected item; possible types: ListCtrlItem* pointer, ListCtrlIconViewItem* pointer, ListCtrlListViewItem* pointer
     *               The value of listCtrlType can be converted to ListCtrlType, and then determine which specific type to convert to based on the enum value
     */
    void AttachDoubleClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseDoubleClick, callback, callbackID); }

    /** Bind the mouse click handler
     * @param [in] callback The callback function for event handling
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item     
     *   listCtrlType: The list type, valid values: ListCtrlType::Report, ListCtrlType::Icon, ListCtrlType::List
     *                 If listCtrlType is any other value, the event is not associated with child item data
     *   pEventData: The pointer to the UI control interface of the selected item; possible types: ListCtrlItem* pointer, ListCtrlIconViewItem* pointer, ListCtrlListViewItem* pointer
     *               The value of listCtrlType can be converted to ListCtrlType, and then determine which specific type to convert to based on the enum value
     */
    void AttachClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventClick, callback, callbackID); }

    /** Bind the mouse right-click handler
     * @param [in] callback The callback function for event handling
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item     
     *   listCtrlType: The list type, valid values: ListCtrlType::Report, ListCtrlType::Icon, ListCtrlType::List
     *                 If listCtrlType is any other value, the event is not associated with child item data
     *   pEventData: The pointer to the UI control interface of the selected item; possible types: ListCtrlItem* pointer, ListCtrlIconViewItem* pointer, ListCtrlListViewItem* pointer
     *               The value of listCtrlType can be converted to ListCtrlType, and then determine which specific type to convert to based on the enum value
     */
    void AttachRClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventRClick, callback, callbackID); }

    /** Bind the handler for the mouse entering the ListCtrlItem/ListCtrlListViewItem/ListCtrlIconViewItem control
     * @param [in] callback The callback function for event handling
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item     
     *   listCtrlType: The list type, valid values: ListCtrlType::Report, ListCtrlType::Icon, ListCtrlType::List
     *                 If listCtrlType is any other value, the event is not associated with child item data
     *   pEventData: The pointer to the UI control interface of the selected item; possible types: ListCtrlItem* pointer, ListCtrlIconViewItem* pointer, ListCtrlListViewItem* pointer
     *               The value of listCtrlType can be converted to ListCtrlType, and then determine which specific type to convert to based on the enum value
     */
    void AttachItemMouseEnter(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventItemMouseEnter, callback, callbackID); }

    /** Bind the handler for the mouse leaving the ListCtrlItem/ListCtrlListViewItem/ListCtrlIconViewItem control
     * @param [in] callback The callback function for event handling
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item
     *   listCtrlType: The list type, valid values: ListCtrlType::Report, ListCtrlType::Icon, ListCtrlType::List
     *                 If listCtrlType is any other value, the event is not associated with child item data
     *   pEventData: The pointer to the UI control interface of the selected item; possible types: ListCtrlItem* pointer, ListCtrlIconViewItem* pointer, ListCtrlListViewItem* pointer
     *               The value of listCtrlType can be converted to ListCtrlType, and then determine which specific type to convert to based on the enum value
     */
    void AttachItemMouseLeave(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventItemMouseLeave, callback, callbackID); }

    /** Bind the handler for the mouse entering the ListCtrlSubItem control
     * @param [in] callback The callback function for event handling
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item
     *   listCtrlType: The list type, the valid value should be: ListCtrlType::Report
     *   pEventData: The pointer to the UI control interface of the selected item; type: ListCtrlSubItem* pointer
     */
    void AttachSubItemMouseEnter(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSubItemMouseEnter, callback, callbackID); }

    /** Bind the handler for the mouse leaving the ListCtrlSubItem control
     * @param [in] callback The callback function for event handling
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item
     *   listCtrlType: The list type, the valid value should be: ListCtrlType::Report
     *   pEventData: The pointer to the UI control interface of the selected item; type: ListCtrlSubItem* pointer
     */
    void AttachSubItemMouseLeave(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSubItemMouseLeave, callback, callbackID); }

    /** Listen to the return key event
     * @param [in] callback The callback function for event handling
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item     
     *   listCtrlType: The list type, valid values: ListCtrlType::Report, ListCtrlType::Icon, ListCtrlType::List
     *                 If listCtrlType is any other value, the event is not associated with child item data
     *   pEventData: The pointer to the UI control interface of the selected item; possible types: ListCtrlItem* pointer, ListCtrlIconViewItem* pointer, ListCtrlListViewItem* pointer
     *               The value of listCtrlType can be converted to ListCtrlType, and then determine which specific type to convert to based on the enum value
     */
    void AttachReturn(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventReturn, callback, callbackID); }

    /** Listen to the keyboard key down event
     * @param [in] callback The callback function for event handling
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item     
     *   listCtrlType: The list type, valid values: ListCtrlType::Report, ListCtrlType::Icon, ListCtrlType::List
     *                 If listCtrlType is any other value, the event is not associated with child item data
     *   pEventData: The pointer to the UI control interface of the selected item; possible types: ListCtrlItem* pointer, ListCtrlIconViewItem* pointer, ListCtrlListViewItem* pointer
     *               The value of listCtrlType can be converted to ListCtrlType, and then determine which specific type to convert to based on the enum value
     */
    void AttachKeyDown(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventKeyDown, callback, callbackID); }

    /** Listen to the keyboard key up event
     * @param [in] callback The callback function for event handling
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item     
     *   listCtrlType: The list type, valid values: ListCtrlType::Report, ListCtrlType::Icon, ListCtrlType::List
     *                 If listCtrlType is any other value, the event is not associated with child item data
     *   pEventData: The pointer to the UI control interface of the selected item; possible types: ListCtrlItem* pointer, ListCtrlIconViewItem* pointer, ListCtrlListViewItem* pointer
     *               The value of listCtrlType can be converted to ListCtrlType, and then determine which specific type to convert to based on the enum value
     */
    void AttachKeyUp(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventKeyUp, callback, callbackID); }

    /** Listen to the start editing event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: an interface pointer: ListCtrlEditParam*, the operation can be cancelled by setting bCancelled
     */
    void AttachEnterEdit(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventEnterEdit, callback, callbackID); }

    /** Listen to the end editing event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: an interface pointer: ListCtrlEditParam*, the operation can be cancelled by setting bCancelled
     */
    void AttachLeaveEdit(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventLeaveEdit, callback, callbackID); }

    /** Listen to the event of the total number of data items changing
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: the new count (size_t)
     *   lParam: the old count (size_t)
     */
    void AttachDataItemCountChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventDataItemCountChanged, callback, callbackID); }

    /** Listen to the event of filling the UI elements of a Report view data item (virtual list)
     *  Triggered: once when a whole row is filled; it is also triggered once when RefreshSubItem refreshes a single cell
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item
     *   pEventData: The pointer to the UI control interface of the associated UI container child item; type: ListCtrlItem* pointer, the data is filled row by row
     *   pExtraData: The pointer to the associated cell, type: ListCtrlSubItem* pointer; nullptr means all cells of this row were filled, non-nullptr means only that cell was filled (single-cell refresh)
     */
    void AttachReportViewItemFilled(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventReportViewItemFilled, callback, callbackID); }

    /** Listen to the event of filling the UI elements of a List view data item (virtual list)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item
     *   pEventData: The pointer to the UI control interface of the associated UI container child item; type: ListCtrlListViewItem* pointer
     */
    void AttachListViewItemFilled(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventListViewItemFilled, callback, callbackID); }

    /** Listen to the event of filling the UI elements of an Icon view data item (virtual list)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item
     *   pEventData: The pointer to the UI control interface of the associated UI container child item; type: ListCtrlIconViewItem* pointer
     */
    void AttachIconViewItemFilled(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventIconViewItemFilled, callback, callbackID); }

    /** Listen to the list view type change event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The current list view type, see the listCtrlType enum values
     *   lParam: The original list view type, see the listCtrlType enum values
     *   pEventData: The pointer to the current list view: ListCtrlReportView*, ListCtrlIconView*, ListCtrlListView*
     */
    void AttachViewTypeChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventViewTypeChanged, callback, callbackID); }

    /** Listen to the list position change event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   listCtrlType: The list type, valid values: ListCtrlType::Report, ListCtrlType::Icon, ListCtrlType::List
     *   pEventData: The pointer to the current list view: ListCtrlReportView*, ListCtrlIconView*, ListCtrlListView*
     */
    void AttachViewPosChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventViewPosChanged, callback, callbackID); }

    /** Listen to the list size change event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   listCtrlType: The list type, valid values: ListCtrlType::Report, ListCtrlType::Icon, ListCtrlType::List
     *   pEventData: The pointer to the current list view: ListCtrlReportView*, ListCtrlIconView*, ListCtrlListView*
     */
    void AttachViewSizeChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventViewSizeChanged, callback, callbackID); }

    /** Listen to the column width changed event (triggered when the column width is adjusted by dragging the header, or when functions such as SetColumnWidth are called)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The column ID (size_t)
     *   lParam: The other associated column ID (size_t); when the width is adjusted by dragging it may be the adjacent column; Box::InvalidIndex means there is no associated column
     */
    void AttachColumnWidthChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventColumnWidthChanged, callback, callbackID); }

    /** Listen to the column order changed event (triggered when the column order is adjusted by dragging the header)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam/lParam: Not used; the latest column order can be obtained through functions such as GetColumnId
     */
    void AttachColumnOrderChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventColumnOrderChanged, callback, callbackID); }

    /** Listen to the column show/hide state changed event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The column ID (size_t)
     *   lParam: The new state (1 means visible, 0 means hidden)
     */
    void AttachColumnVisibleChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventColumnVisibleChanged, callback, callbackID); }

    /** Listen to the column sorting changed event (triggered when sorting by clicking the header, or when functions such as SortDataItems are called)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The column ID (size_t)
     *   lParam: The new state (1 means ascending, 0 means descending)
     */
    void AttachColumnSorted(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventColumnSorted, callback, callbackID); }

    /** Bind the event of the current cell changing (cell selection mode)
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
    * Parameter description:
    *   wParam: The display index of the row (size_t)
    *   lParam: The column ID (size_t)
    */
    void AttachCurrentCellChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventCurrentCellChanged, callback, callbackID); }

    /** Bind the event of the cell selection area changing (cell selection mode)
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
    * Parameter description:
    *   wParam: The number of selected areas (size_t); 0 means there is currently no selection
    *   lParam: 0 (reserved)
    *   pEventData: Points to std::vector<ui::ListCtrlCellRange>* (the list of selected areas, possibly several items; nullptr when there is no selection)
    */
    void AttachCellSelectionChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventCellSelectionChanged, callback, callbackID); }

protected:
    /** Control initialization
    */
    virtual void OnInit() override;

    /** Initialize the Report view
    */
    void InitReportView();

    /** Initialize the Icon view
    */
    void InitIconView();

    /** Initialize the List view
    */
    void InitListView();

    /** Set the attribute Class of ListCtrlHeader
    */
    void SetHeaderClass(const std::string& className);

    /** The attribute Class of ListCtrlHeaderItem
    */
    void SetHeaderItemClass(const std::string& className);
    std::string GetHeaderItemClass() const;

    /** The attribute Class of ListCtrlHeader/SplitBox
    */
    void SetHeaderSplitBoxClass(const std::string& className);
    std::string GetHeaderSplitBoxClass() const;

    /** The attribute Class of ListCtrlHeader/SplitBox/Control
    */
    void SetHeaderSplitControlClass(const std::string& className);
    std::string GetHeaderSplitControlClass() const;

    /** The Class attribute of CheckBox (applied to the Header and ListCtrl data)
    */
    void SetCheckBoxClass(const std::string& className);
    std::string GetCheckBoxClass() const;

    /** The Class attribute of the ListBox in the data Report view
    */
    void SetReportViewClass(const std::string& className);
    std::string GetReportViewClass() const;

    /** The Class attribute of ListCtrlItem
    */
    void SetDataItemClass(const std::string& className);
    std::string GetDataItemClass() const;

    /** The Class attribute of ListCtrlItem/ListCtrlSubItem
    */
    void SetDataSubItemClass(const std::string& className);
    std::string GetDataSubItemClass() const;

    /** The Class attribute of the ListBox in the data Icon view
    */
    void SetIconViewClass(const std::string& className);
    std::string GetIconViewClass() const;

    /** The Class attribute of the child items of the ListBox in the data Icon view
    */
    void SetIconViewItemClass(const std::string& className);
    std::string GetIconViewItemClass() const;

    /** The Class attribute of the images in the child items of the ListBox in the data Icon view
    */
    void SetIconViewItemImageClass(const std::string& className);
    std::string GetIconViewItemImageClass() const;

    /** The Class attribute of the Labels in the child items of the ListBox in the data Icon view
    */
    void SetIconViewItemLabelClass(const std::string& className);
    std::string GetIconViewItemLabelClass() const;

    /** The Class attribute of the ListBox in the data List view
    */
    void SetListViewClass(const std::string& className);
    std::string GetListViewClass() const;

    /** The Class attribute of the child items of the ListBox in the data List view
    */
    void SetListViewItemClass(const std::string& className);
    std::string GetListViewItemClass() const;

    /** The Class attribute of the images of the child items of the ListBox in the data List view
    */
    void SetListViewItemImageClass(const std::string& className);
    std::string GetListViewItemImageClass() const;

    /** The Class attribute of the Labels of the child items of the ListBox in the data List view
    */
    void SetListViewItemLabelClass(const std::string& className);
    std::string GetListViewItemLabelClass() const;

    /** The Class attribute of the edit control
    */
    void SetRichEditClass(const std::string& richEditClass);
    std::string GetRichEditClass() const;

    /** The Class attribute of the date edit box (the DateTime control used by the kEditDate editor)
    */
    void SetDateTimeClass(const std::string& dateTimeClass);
    std::string GetDateTimeClass() const;

protected:
    /** Add a column
    * @param [in] nColumnId The column ID
    */
    void OnHeaderColumnAdded(size_t nColumnId);

    /** Delete a column
    * @param [in] nColumnId The column ID
    */
    void OnHeaderColumnRemoved(size_t nColumnId);

    /** Adjust the column widths (dragging to adjust the column width, adjusting the widths of two columns each time)
    * @param [in] nColumnId1 The ID of the first column
    * @param [in] nColumnId2 The ID of the second column
    */
    void OnColumnWidthChanged(size_t nColumnId1, size_t nColumnId2);

    /** Sort the columns
    * @param [in] nColumnId The column ID
    * @param [in] bSortedUp If true, ascending; if false, descending
    */
    void OnColumnSorted(size_t nColumnId, bool bSortedUp);

    /** The order of the columns has been adjusted by dragging the header
    */
    void OnHeaderColumnOrderChanged();

    /** CheckBox check operation of the header (column level)
    * @param [in] nColumnId The column ID
    * @param [in] bChecked true means checked (Checked state), false means unchecked (UnChecked state)
    */
    void OnHeaderColumnCheckStateChanged(size_t nColumnId, bool bChecked);

    /** CheckBox check operation of the header (row level)
    * @param [in] bChecked true means checked (Checked state), false means unchecked (UnChecked state)
    */
    void OnHeaderCheckStateChanged(bool bChecked);

    /** The display attribute of a header column has changed
    * @param [in] nColumnId The column ID
    */
    void OnHeaderColumnVisibleChanged(size_t nColumnId);

    /** Double-click event of the header column splitter
    * @param [in] pHeaderItem The header control interface
    */
    void OnHeaderColumnSplitDoubleClick(ListCtrlHeaderItem* pHeaderItem);

    /** Sync the UI check state (column-level CheckBox)
    * @param [in] nColumnId The column ID; if it is Box::InvalidIndex, update all columns
    */
    void UpdateHeaderColumnCheckBox(size_t nColumnId);

    /** Sync the UI check state (row-level CheckBox)
    */
    void UpdateHeaderCheckBox();

private:
    /** List event, forwarded to the application layer
    */
    void OnListCtrlViewEvent(ListCtrlType listCtrlType, const EventArgs& args);

    /** Enter the edit state
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] nColumnId The column ID
    * @param [in] pItem The interface of the list item
    * @param [in] pSubItem The interface of the list item's sub-item
    */
    void OnItemEnterEditMode(size_t itemIndex, size_t nColumnId,
                             IListBoxItem* pItem, ListCtrlLabel* pSubItem);

    /** Timer cancellation mechanism for entering the edit state
    */
    WeakCallbackFlag m_editModeFlag;

    /** Enter the edit state from the timer
    */
    void OnItemEditMode(ListCtrlEditParam editParam);

    /** Validate whether the input parameters of the edit state are valid
    */
    bool IsValidItemEditParam(const ListCtrlEditParam& editParam) const;

    /** Validate whether the editable state is satisfied
    */
    bool IsValidItemEditState(const ListCtrlEditParam& editParam) const;

    /** Update the position and size of the RichEdit control
    */
    void UpdateRichEditSize(ListCtrlLabel* pSubItem);

    /** Update the position and size of the DateTime control (kEditDate editor)
    */
    void UpdateDateTimeSize(ListCtrlLabel* pSubItem);

    /** Sub-item edit event
    * @return Returns true if the data has been written back (the user did not cancel and the input passed the editor type validation)
    */
    bool OnItemEdited(const ListCtrlEditParam& editParam, const std::string& newItemText);

    /** Get the data row control corresponding to the specified display index (Report view; only the "currently visible" rows, which cannot be cached because they are reused after scrolling)
    * @param [in] nItemIndex The display index of the row
    * @return Returns the data row control (returns nullptr when it is not visible)
    */
    ListCtrlItem* GetDisplayItemByIndex(size_t nItemIndex) const;

    /** Determine whether the text entered in the editor conforms to the editor type declared for the column (an empty string is always considered valid and means clearing the cell)
    */
    static bool IsValidCellEditText(const std::string& strNewText, ListCtrlEditorType editorType);

    /** Enter the combo candidate edit state (kEditCombo editor): pop up a candidate popup below the cell
    * @param [in] editParam The edit parameters (created when the edit starts)
    * @param [in] pSubItem The text control of the hit cell (used as the anchor of the popup)
    */
    void EnterComboEditMode(const ListCtrlEditParam& editParam, ListCtrlLabel* pSubItem);

    /** Enter the date edit state (kEditDate editor): float a DateTime control over the cell and pop up a month calendar once it has focus
    * @param [in] editParam The edit parameters (created when the edit starts)
    * @param [in] pSubItem The text control of the hit cell
    */
    void EnterDateEditMode(const ListCtrlEditParam& editParam, ListCtrlLabel* pSubItem);

    /** Combo candidate editing: commit after a candidate item is selected (by mouse click or keyboard Return)
    * @param [in] nIndex The candidate item index
    * @return Returns true if it has been committed and written back
    */
    bool OnComboCandidateSelected(size_t nIndex);

    /** Close the combo candidate popup (no-op when it is not open)
    */
    void CloseComboFlyout();

    /** Determine whether the combo candidate popup is open
    */
    bool IsComboEditOpened() const;

    /** After a successful commit, move the current cell down by one row (kCell mode; nothing happens when it is already the last row)
    */
    void MoveCurrentCellToNextRow();

    /** The view received a mouse message
    */
    void OnViewMouseEvents(const EventArgs& msg);

    /** The view received a mouse message
    */
    void OnViewKeyboardEvents(const EventArgs& msg);

    /** Leave the edit state
    */
    void LeaveEditMode();

    /** Clear all event listeners of the edit state
    */
    void ClearEditEvents();

    /** Set the list control type (defaults to Report type)
    * @param [in] type The view type
    * @param [in] bInit true means it is called during the initialization phase
    */
    void DoSetListCtrlType(ListCtrlType type, bool bInit);

    /** Cell selection mode: collect the list of currently selected cell areas (possibly several items, used for event reporting)
        Merging rule: within the same display row, areas whose "column indexes are consecutive" are merged into a horizontal segment, and the same segments of vertically adjacent rows are merged into a rectangle
    * @param [out] rangeList Returns the list of selected areas (empty when there is no selection)
    */
    void GetSelectedCellRanges(std::vector<ListCtrlCellRange>& rangeList) const;

    /** Generate the text of the selected content (or of the whole table)
    * @param [in] bCsvFormat true means the CSV format (conforming to RFC4180), false means the TSV format (tab separated)
    * @param [in] bSelectedOnly true means export only the currently selected content, false means export the whole table (used by the CSV export path only)
    * @return Returns the generated text (an empty string when there is no content)
    */
    std::string BuildSelectionText(bool bCsvFormat, bool bSelectedOnly) const;

    /** Escape the content of a single field
    * @param [in] strText The raw field text
    * @param [in] bCsvFormat true means the CSV format (double quotes are added when needed, and the inner double quotes are doubled);
    *                         false means the TSV format (tabs/returns/newlines are replaced with spaces)
    * @return Returns the escaped text
    */
    static std::string EscapeText(const std::string& strText, bool bCsvFormat);

    /** Cell selection mode: dispatch the "current cell changed" event
    */
    void NotifyCurrentCellChanged(const ListCtrlCellId& cell);

    /** Cell selection mode: dispatch the "selection area changed" event
    */
    void NotifyCellSelectionChanged();

    /** Cell selection mode: refresh the view display (repaint the cell highlight and the focus frame of the current cell)
    */
    void RefreshCellSelectionView();

    /** Cell selection mode: update the cell the mouse is hovering over (used to paint the cell hover highlight)
    * @param [in] nRowIndex The display index of the row (Box::InvalidIndex when no cell is hit)
    * @param [in] nColumnId The column ID (Box::InvalidIndex when no cell is hit)
    */
    void UpdateHoverCell(size_t nRowIndex, size_t nColumnId);

    /** Cell selection mode: clear the cell the mouse is hovering over (called when the mouse leaves the control, to avoid a leftover hover highlight)
    */
    void ClearHoverCell();

    /** Cell selection mode: determine whether the specified cell is the cell the mouse is hovering over
    * @param [in] nRowIndex The display index of the row
    * @param [in] nColumnId The column ID
    */
    bool IsCellHovered(size_t nRowIndex, size_t nColumnId) const;

    /** Cell selection mode: handle the mouse events coming from ListCtrlItem (set/extend the cell selection)
    * @param [in] msg The mouse event message
    * @param [in] nRowIndex The display index of the row
    * @param [in] nColumnId The column ID (Box::InvalidIndex when no cell is hit)
    * @return Returns whether it has been handled (true means it has been handled as a cell selection and the caller should not perform row selection)
    */
    bool OnCellMouseEvent(const EventArgs& msg, size_t nRowIndex, size_t nColumnId);

    /** Cell selection mode: start a drag selection when the mouse button is pressed (shared by the left and right buttons)
        A plain click selects the cell (clearing the others); Shift extends to the clicked cell; Ctrl toggles the selection; Ctrl+Shift extends additively;
        when no cell is hit (blank area) the cell selection is cleared; afterwards the drag-selection state is set (the rubber-band rectangle drives the subsequent selection)
    * @param [in] msg The mouse button down event message
    * @param [in] nRowIndex The display index of the row (Box::InvalidIndex when no cell is hit)
    * @param [in] nColumnId The column ID (Box::InvalidIndex when no cell is hit)
    */
    void BeginCellDrag(const EventArgs& msg, size_t nRowIndex, size_t nColumnId);

    /** Cell selection mode: handle keyboard events (the arrow keys move the current cell, Shift extends the area, Ctrl+A selects all, etc.)
    * @param [in] msg The keyboard event message
    * @return Returns whether it has been handled
    */
    bool OnCellKeyDown(const EventArgs& msg);

    /** Handle the keyboard events of the combo candidate popup (shared by row selection mode and cell selection mode)
    *  When the popup is open: the up/down arrow keys move the candidate item, Return commits, Esc cancels (all of these keys are consumed)
    * @param [in] msg The keyboard event message
    * @return Returns whether it has been handled (returns false when the popup is not open, and the key continues through the normal flow)
    */
    bool OnComboKeyDown(const EventArgs& msg);

private:
    /** The header control
    */
    ListCtrlHeader* m_pHeaderCtrl;

    /** List data display (Report view)
    */
    ListCtrlReportView* m_pReportView;

    /** List data display (Icon view)
    */
    ListCtrlIconView* m_pIconView;

    /** List data display (List view)
    */
    ListCtrlListView* m_pListView;

    /** List data management
    */
    ListCtrlData* m_pData;

    /** The attribute Class of ListCtrlHeader
    */
    UiString m_headerClass;

    /** The attribute Class of ListCtrlHeaderItem
    */
    UiString m_headerItemClass;

    /** The attribute Class of ListCtrlHeader/SplitBox
    */
    UiString m_headerSplitBoxClass;

    /** The attribute Class of ListCtrlHeader/SplitBox/Control
    */
    UiString m_headerSplitControlClass;

    /** The Class of CheckBox
    */
    UiString m_checkBoxClass;

    /** The Class attribute of ListCtrlItem
    */
    UiString m_dataItemClass;

    /** The Class attribute of ListCtrlItem/ListCtrlSubItem
    */
    UiString m_dataSubItemClass;

    /** The Report view Class attribute of ListBox
    */
    UiString m_reportViewClass;

    /** The Icon view Class attribute of ListBox (Icon view)
    */
    UiString m_iconViewClass;

    /** The Icon view child item Class attribute of ListBox (Icon view)
    */
    UiString m_iconViewItemClass;

    /** The Icon view icon child item Class attribute of ListBox (Icon view)
    */
    UiString m_iconViewItemImageClass;

    /** The Icon view text child item Class attribute of ListBox (Icon view)
    */
    UiString m_iconViewItemLabelClass;

    /** The List view Class attribute of ListBox (List view)
    */
    UiString m_listViewClass;

    /** The List view Class attribute of ListBox (List view)
    */
    UiString m_listViewItemClass;

    /** The List view icon child item Class attribute of ListBox (List view)
    */
    UiString m_listViewItemImageClass;

    /** The List view text child item Class attribute of ListBox (List view)
    */
    UiString m_listViewItemLabelClass;

    /** The edit control Class attribute of ListCtrl
    */
    UiString m_listCtrlRichEditClass;

    /** The Class attribute of the date edit box of ListCtrl (kEditDate editor)
    */
    UiString m_listCtrlDateTimeClass;

    /** The character whitelist (limit_chars) declared in the rich text edit box class
    *  Note: the edit box control is reused, and the number editor temporarily rewrites the whitelist when it enters the edit state;
    *  to avoid erasing the whitelist configured on the control class, this value is recorded when the edit box is created for the first time and restored when switching back to a non-number editor.
    */
    std::string m_richEditLimitChars;

    /** Image list
    */
    ImageListPtr m_imageList[3];

    /** The edit control
    */
    RichEdit* m_pRichEdit;

    /** Date edit box control (kEditDate editor)
    */
    DateTime* m_pDateTime;

    /** The height of the header
    */
    int32_t m_nHeaderHeight;

    /** The height of the data items
    */
    int32_t m_nItemHeight;

    /** The ID of the currently sorted column
    */
    size_t m_nSortedColumnId;

    /** Sort flag mapping table for each column (column ID -> sort flag)
    */
    std::map<size_t, uint8_t> m_columnSortFlagMap;

    /** Whether the current sort is ascending
    */
    bool m_bSortedUp;

    /** The list control type (defaults to Report type)
    */
    ListCtrlType m_listCtrlType;

    /** Whether dragging can change the order of the columns
    */
    bool m_bEnableHeaderDragOrder;

    /** The number of frozen columns (the first N columns at the left of the list are fixed and do not move with horizontal scrolling)
    */
    size_t m_nFrozenColumnCount;

    /** Whether the header control is shown
    */
    bool m_bShowHeaderCtrl;

    /** Whether refreshing the UI is allowed
    */
    bool m_bEnableRefresh;

    /** Whether multi-select is supported (single-select by default)
    */
    bool m_bMultiSelect;

    /** The selection mode (the default is row selection mode)
    */
    ListCtrlSelectionMode m_selectionMode;

    /** Whether the selection area is currently being extended by mouse dragging in cell selection mode
    */
    bool m_bCellDragSelecting;

    /** Whether this drag is in "additive" mode in cell selection mode (Ctrl+drag: append the rectangular area without clearing the existing selection)
    */
    bool m_bCellDragAdditive;

    /** Internal identifier of the cell hover state: stored by row identity (RowId) and decoupled from the display order
        (after sorting/adding/removing rows the hover highlight still follows the original data row, and together with the column ID it locates a unique cell)
    */
    struct HoverCellId
    {
        ListCtrlData::RowId nRowId;     //Row identity (ListCtrlData::kInvalidRowId when invalid)
        size_t nColumnId;               //Column ID (Box::InvalidIndex when invalid)

        HoverCellId() : nRowId(ListCtrlData::kInvalidRowId), nColumnId(Box::InvalidIndex) {}

        /** Whether this is a valid cell
        */
        bool IsValid() const { return (nRowId != ListCtrlData::kInvalidRowId) && (nColumnId != Box::InvalidIndex); }

        bool operator==(const HoverCellId& r) const { return (nRowId == r.nRowId) && (nColumnId == r.nColumnId); }
        bool operator!=(const HoverCellId& r) const { return !(*this == r); }
    };

    /** The cell the mouse is hovering over in cell selection mode (used to paint the cell hover highlight)
    */
    HoverCellId m_hoverCell;

    /** Whether double-clicking the Header's splitter automatically adjusts the column width
    */
    bool m_bEnableColumnWidthAuto;

    /** Whether to automatically check the selected data items (similar to the LVS_EX_AUTOCHECKSELECT attribute of ListCtrl on Windows)
    */
    bool m_bAutoCheckSelect;

    /** Whether the Header's CheckBox is shown (row level)
    */
    bool m_bHeaderShowCheckBox;

    /** Whether the data items' CheckBox is shown (row level)
    */
    bool m_bDataItemShowCheckBox;

    /** Whether copying the selected content to the clipboard is supported (default true)
    */
    bool m_bEnableCopy;

    /** Whether CSV export is supported (default true)
    */
    bool m_bEnableCsvExport;

    /** Whether sub-item editing is supported
    */
    bool m_bEnableItemEdit;

    /** Column ID -> editor type (stored by column ID, unaffected by column order changes)
    */
    std::map<size_t, ListCtrlEditorType> m_columnEditorTypes;

    /** The cell currently being edited (only valid in kCell mode; an invalid value when not editing)
    */
    ListCtrlCellId m_editingCell;

    /** Column ID -> candidate value list (stored by column ID, used by the kEditCombo editor)
    */
    std::map<size_t, std::vector<std::string>> m_columnCandidates;

    /** The current candidate popup (kEditCombo editor; destroyed automatically by the framework after it is closed, and the pointer is set to null at the same time)
    */
    ListCtrlComboFlyout* m_pComboFlyout;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_H_
