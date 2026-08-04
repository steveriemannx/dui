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
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
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

    /** Set the default height of data items (row height)
    * @param [in] nItemHeight The height value
    * @param [in] bNeedDpiScale If true, the width needs DPI adaptation
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
    bool SetSubItemText(size_t itemIndex, size_t columnIndex, const DString& text);
    bool SetSubItemTextById(size_t itemIndex, size_t columnId, const DString& text);

    /** Get the text of the specified data item
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @return The text content associated with the data item
    */
    DString GetSubItemText(size_t itemIndex, size_t columnIndex) const;
    DString GetSubItemTextById(size_t itemIndex, size_t columnId) const;

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
    bool SetSubItemUserDataS(size_t itemIndex, size_t columnIndex, const DString& userDataS);
    bool SetSubItemUserDataSById(size_t itemIndex, size_t columnId, const DString& userDataS);

    /** Get the associated user data of the specified data item (string)
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @return The string data associated with the data item
    */
    DString GetSubItemUserDataS(size_t itemIndex, size_t columnIndex) const;
    DString GetSubItemUserDataSById(size_t itemIndex, size_t columnId) const;

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

public:
    /** Sort the data
    * @param [in] columnIndex The column index, valid range: [0, GetColumnCount())
    * @param [in] columnId The column ID
    * @param [in] bSortedUp true means ascending, false means descending
    * @param [in] nSortFlag The sort method flag, see the ListCtrlSubItemSortFlag enum values
    * @param [in] pfnCompareFunc The custom compare function; if nullptr, the default compare function is used
    * @param [in] pUserData User custom data, passed back to the compare function through the parameter when it is called
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

    /** Set selected data items in batch
    * @param [in] selectedIndexs The indexes of the data items to be selected, valid range: [0, GetDataItemCount())
    * @param [in] bClearOthers If true, clear the selection of other already-selected items, keeping only the ones set this time
    */
    void SetSelectedDataItems(const std::vector<size_t>& selectedIndexs, bool bClearOthers);

    /** Get the list of selected data items
    * @param [in] itemIndexs Returns the indexes of the currently selected data items, valid range: [0, GetDataItemCount())
    */
    void GetSelectedDataItems(std::vector<size_t>& itemIndexs) const;

    /** Select all
    */
    void SetSelectAll();

    /** Cancel all selections
    */
    void SetSelectNone();

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

    /** Set whether refreshing the UI is allowed
    * @param [in] bEnable If true, refreshing the UI via the Refresh() function is allowed
    * @return Returns the old IsEnableRefresh() state
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
    void SetRowGridLineColor(const DString& color);
    DString GetRowGridLineColor() const;

    /** The width of the vertical grid lines
    * @param [in] nLineWidth The width of the grid lines; if it is 0, the vertical grid lines are not shown
    * @param [in] bNeedDpiScale If true, the width needs DPI adaptation
    */
    void SetColumnGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale);
    int32_t GetColumnGridLineWidth() const;

    /** The color of the vertical grid lines
    * @param [in] color The color of the vertical grid lines
    */
    void SetColumnGridLineColor(const DString& color);
    DString GetColumnGridLineColor() const;

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

    /** Listen to the event of filling the UI elements of a Report view data item (virtual list, filling the data of a row)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item
     *   pEventData: The pointer to the UI control interface of the associated UI container child item; type: ListCtrlItem* pointer, the data is filled row by row
     */
    void AttachReportViewItemFilled(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventReportViewItemFilled, callback, callbackID); }

    /** Listen to the event of filling the UI elements of a Report view data item (virtual list, filling the data of a row and column)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The index of the data item corresponding to the UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, there is no associated data item
     *   pEventData: The pointer to the UI control interface of the associated UI container child item; type: ListCtrlSubItem* pointer, representing the UI element control of the data of a row and a column
     *               Through this interface, the data item index and which column the data belongs to can be obtained
     */
    void AttachReportViewSubItemFilled(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventReportViewSubItemFilled, callback, callbackID); }

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
    void SetHeaderClass(const DString& className);

    /** The attribute Class of ListCtrlHeaderItem
    */
    void SetHeaderItemClass(const DString& className);
    DString GetHeaderItemClass() const;

    /** The attribute Class of ListCtrlHeader/SplitBox
    */
    void SetHeaderSplitBoxClass(const DString& className);
    DString GetHeaderSplitBoxClass() const;

    /** The attribute Class of ListCtrlHeader/SplitBox/Control
    */
    void SetHeaderSplitControlClass(const DString& className);
    DString GetHeaderSplitControlClass() const;

    /** The Class attribute of CheckBox (applied to the Header and ListCtrl data)
    */
    void SetCheckBoxClass(const DString& className);
    DString GetCheckBoxClass() const;

    /** The Class attribute of the ListBox in the data Report view
    */
    void SetReportViewClass(const DString& className);
    DString GetReportViewClass() const;

    /** The Class attribute of ListCtrlItem
    */
    void SetDataItemClass(const DString& className);
    DString GetDataItemClass() const;

    /** The Class attribute of ListCtrlItem/ListCtrlSubItem
    */
    void SetDataSubItemClass(const DString& className);
    DString GetDataSubItemClass() const;

    /** The Class attribute of the ListBox in the data Icon view
    */
    void SetIconViewClass(const DString& className);
    DString GetIconViewClass() const;

    /** The Class attribute of the child items of the ListBox in the data Icon view
    */
    void SetIconViewItemClass(const DString& className);
    DString GetIconViewItemClass() const;

    /** The Class attribute of the images in the child items of the ListBox in the data Icon view
    */
    void SetIconViewItemImageClass(const DString& className);
    DString GetIconViewItemImageClass() const;

    /** The Class attribute of the Labels in the child items of the ListBox in the data Icon view
    */
    void SetIconViewItemLabelClass(const DString& className);
    DString GetIconViewItemLabelClass() const;

    /** The Class attribute of the ListBox in the data List view
    */
    void SetListViewClass(const DString& className);
    DString GetListViewClass() const;

    /** The Class attribute of the child items of the ListBox in the data List view
    */
    void SetListViewItemClass(const DString& className);
    DString GetListViewItemClass() const;

    /** The Class attribute of the images of the child items of the ListBox in the data List view
    */
    void SetListViewItemImageClass(const DString& className);
    DString GetListViewItemImageClass() const;

    /** The Class attribute of the Labels of the child items of the ListBox in the data List view
    */
    void SetListViewItemLabelClass(const DString& className);
    DString GetListViewItemLabelClass() const;

    /** The Class attribute of the edit control
    */
    void SetRichEditClass(const DString& richEditClass);
    DString GetRichEditClass() const;

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
    */
    void OnHeaderColumnVisibleChanged();

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

    /** Sub-item edit event
    */
    void OnItemEdited(const ListCtrlEditParam& editParam, const DString& newItemText);

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

    /** Image lists
    */
    ImageListPtr m_imageList[3];

    /** The edit control
    */
    RichEdit* m_pRichEdit;

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

    /** Whether the header control is shown
    */
    bool m_bShowHeaderCtrl;

    /** Whether refreshing the UI is allowed
    */
    bool m_bEnableRefresh;

    /** Whether multi-select is supported (single-select by default)
    */
    bool m_bMultiSelect;

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

    /** Whether sub-item editing is supported
    */
    bool m_bEnableItemEdit;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_H_
