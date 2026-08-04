#ifndef UI_CONTROL_LIST_CTRL_DEFS_H_
#define UI_CONTROL_LIST_CTRL_DEFS_H_

#include "dui/Control/ListCtrlView.h"
#include "dui/Control/ListCtrlLabel.h"
#include "dui/Control/ListCtrlIcon.h"

namespace ui
{
/** Table type
*/
enum class ListCtrlType: int8_t
{
    Report = 0, //Report type
    Icon   = 1, //Icon type
    List   = 2  //List type
};

/** Basic information of a column (used to add a column)
*/
struct ListCtrlColumn
{
    DString text;                   //Text of the header
    int32_t nColumnWidth = 100;     //Column width; if bNeedDpiScale is true, DPI adaptive processing is performed
    int32_t nColumnWidthMin = 0;    //Minimum column width; 0 means using the default setting; if bNeedDpiScale is true, DPI adaptive processing is performed
    int32_t nColumnWidthMax = 0;    //Maximum column width; 0 means using the default setting; if bNeedDpiScale is true, DPI adaptive processing is performed
    int32_t nTextFormat = -1;       //Text alignment and other attributes; this attribute only applies to the Header, see DrawStringFormat in IRender.h for the value; if it is -1, the alignment of the default configuration is used
    int32_t nImageId = -1;          //Icon resource ID; if -1, no icon is displayed; the icon is displayed in front of the text
    bool bSortable = true;          //Whether sorting is supported
    bool bResizeable = true;        //Whether the column width can be adjusted by dragging
    bool bShowCheckBox = false;     //Whether to show the CheckBox (supports showing the CheckBox in the header and data columns)
    bool bNeedDpiScale = true;      //Whether to perform DPI adaptation on the value
};

/** Attribute data structure of a row (row data, one data item per row, data storage)
*/
struct ListCtrlItemData
{
    bool bVisible = true;           //Whether it is visible
    bool bSelected = false;         //Whether it is in the selected state (whether this row is selected)
    bool bChecked = false;          //Whether it is in the checked state (whether the CheckBox at the beginning of this row is checked)
    int8_t nAlwaysAtTop = -1;       //Whether to display on top; -1 means not on top, 0 or a positive number means on top; the larger the value, the higher the priority, displayed at the top first
    int16_t nItemHeight = -1;       //Row height; -1 means using the default row height set by ListCtrl, which is the value after DPI adaptive processing
    int32_t nImageId = -1;          //Icon resource ID; if -1, no icon is displayed at the beginning of the row
    size_t nUserData = 0;           //User-defined data
};

/** Basic information of a list data item (column data, used to add data and pass parameters)
*/
struct ListCtrlSubItemData
{
    DString text;                   //Text content
    int32_t nTextFormat = -1;       //Text alignment and other attributes; this attribute only applies to the Header, see DrawStringFormat in IRender.h for the value; if it is -1, the alignment of the default configuration is used
    int32_t nImageId = -1;          //Icon resource ID; if -1, no icon is displayed
    UiColor textColor;              //Text color
    UiColor bkColor;                //Background color
    uint64_t userDataN = 0;         //User-defined data (integer)
    UiString userDataS;             //User-defined data (string type)
    int32_t nSortGroup = 0;         //The group it belongs to (e.g., folders and files can be divided into two groups; after sorting, folders and files are separated)
    bool bShowCheckBox = false;     //Whether to show the CheckBox
    bool bChecked = false;          //Whether it is in the checked state (CheckBox checked state)
    bool bEditable = false;         //Whether it is editable
};

/** Data structure for internal storage of list data items (column data, one data item per <row, column>, data storage)
*/
struct ListCtrlSubItemData2
{
    UiString text;                  //Text content
    int32_t nImageId = -1;          //Icon resource ID; if -1, no icon is displayed
    UiColor textColor;              //Text color
    UiColor bkColor;                //Background color
    uint16_t nTextFormat = 0;       //Text alignment and other attributes; this attribute only applies to the Header, see DrawStringFormat in IRender.h for the value; if it is -1, the alignment of the default configuration is used
    bool bShowCheckBox = false;     //Whether to show the CheckBox
    bool bChecked = false;          //Whether it is in the checked state (CheckBox checked state)
    uint64_t userDataN = 0;         //User-defined data (integer)
    UiString userDataS;             //User-defined data (string type)
    int32_t nSortGroup = 0;         //The group it belongs to (e.g., folders and files can be divided into two groups; after sorting, folders and files are separated)
    bool bEditable = false;         //Whether it is editable
};

//Smart pointer of column data
typedef std::shared_ptr<ListCtrlSubItemData2> ListCtrlSubItemData2Ptr;

/** Data sorting flags in the list (bitwise AND operation)
*/
enum ListCtrlSubItemSortFlag : uint8_t
{
    kDefault            = 0,    //Empty value
    kSortByGroup        = 1,    //When sorting, use group sorting, grouping by the ListCtrlSubItemData2.nSortGroup field
    kSortNoCase         = 2,    //When the sorting target is a string, the comparison is case-insensitive
    kSortByText         = 4,    //Sort by the ListCtrlSubItemData2.text field (default)
    kSortByUserDataN    = 8,    //Sort by the ListCtrlSubItemData2.userDataN field
    kSortByUserDataS    = 16,    //Sort by the ListCtrlSubItemData2.userDataS field
};

struct ListCtrlSubItemData2Pair
{
    size_t nColumnId = 0; //Column ID
    ListCtrlSubItemData2Ptr pSubItemData; //Data of the column
};

/** Additional information for comparing data
*/
struct ListCtrlCompareParam
{
    size_t nColumnIndex = 0;   //Which column the data is associated with, valid range: [0, GetColumnCount())
    size_t nColumnId = 0;      //The ID of the column the data is associated with
    uint8_t nSortFlag = ListCtrlSubItemSortFlag::kDefault; //Sort flag, see the enum values of ListCtrlSubItemSortFlag
    void* pUserData = nullptr; //User-defined data, passed in together when setting the compare function
};

/** Prototype of the compare function for stored data, implementing ascending comparison (a < b)
* @param [in] a The first data to compare
* @param [in] b The second data to compare
* @param [in] param The parameter associated with the data
* @return If (a < b), returns true; otherwise returns false
*/
typedef std::function<bool(const ListCtrlSubItemData2& a, 
                           const ListCtrlSubItemData2& b, 
                           const ListCtrlCompareParam& param)> ListCtrlDataCompareFunc;

/** Related interface for the view to fill data into UI controls
*/
class DUI_API IListCtrlView
{
public:
    /** Create a data item
    * @return Returns the pointer of the created data item
    */
    virtual Control* CreateDataItem() = 0;

    /** Fill the specified data item
    * @param [in] pControl Pointer to the data item control
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @param [in] itemData The data item (representing the row attributes)
    * @param [in] subItemList The data sub-items (representing the data of each column; the 1st is the column ID, the 2nd is the column data)
    */
    virtual bool FillDataItem(ui::Control* pControl,
                              size_t nElementIndex,
                              const ListCtrlItemData& itemData,
                              const std::vector<ListCtrlSubItemData2Pair>& subItemList) = 0;


    /** Get the maximum width of a column
    * @param [in] subItemList The data sub-items (representing the data of each column)
    * @return Returns the maximum width of this column, which is the value after DPI adaptive processing; returns -1 on failure
    */
    virtual int32_t GetMaxDataItemWidth(const std::vector<ListCtrlSubItemData2Ptr>& subItemList) = 0;
};

/** Input parameters of the edit state
*/
struct ListCtrlEditParam
{
    ListCtrlType listCtrlType = ListCtrlType::Report;
    size_t nItemIndex = 0;          //The index of the data item, valid range: [0, GetDataItemCount())
    size_t nColumnId = 0;           //Column ID
    size_t nColumnIndex = 0;        //The ordinal number of the column, valid range: [0, GetColumnCount())
    IListBoxItem* pItem = nullptr;  //Interface of the data sub-item
    ListCtrlLabel* pSubItem = nullptr;    //Interface of the text control (including the text content before modification)

    UiString sNewText;          //The text content after modification
    bool bCancelled = false;    //Whether to cancel the operation; if set to true, the edit operation is cancelled
};

/** List item type of the Icon view (vertical layout)
*   Basic structure: <ListCtrlIconViewItem> <ListCtrlIcon/><ListCtrlLabel/> </ListCtrlListViewItem>
*   The attributes of the Control and Label inside support reading from the configuration file
*/
class ListCtrl;
class DUI_API ListCtrlIconViewItem : public ListCtrlItemBaseV
{
    typedef ListCtrlItemBaseV BaseClass;
public:
    explicit ListCtrlIconViewItem(Window* pWindow):
        ListCtrlItemBaseV(pWindow)
    {
    }
    /** Get the control type
    */
    virtual DString GetType() const override { return _T("ListCtrlIconViewItem"); }

    /** Event handling function
    */
    virtual void HandleEvent(const EventArgs& msg) override;

    /** Set the associated ListCtrl interface
    */
    void SetListCtrl(ListCtrl* pListCtrl) { m_pListCtrl = pListCtrl; }

    /** Get the associated ListCtrl interface
    */
    ListCtrl* GetListCtrl() const { return m_pListCtrl; }

public:
    /** Get the index of the associated data item, representing which row of data it is associated with
    * @return Returns the index of the data item, valid range: [0, ListCtrl::GetDataItemCount())
    */
    size_t GetDataItemIndex() const { return GetElementIndex(); }

    /** Get the interface of the icon control
    */
    ListCtrlIcon* GetListCtrlIcon() const { return dynamic_cast<ListCtrlIcon*>(GetItemAt(0)); }

    /** Get the interface of the text control
    */
    ListCtrlLabel* GetListCtrlLabel() const { return dynamic_cast<ListCtrlLabel*>(GetItemAt(1)); }

    /** Get the text inside the text control
    */
    DString GetLabelText() const
    {
        ListCtrlLabel* pLabel = GetListCtrlLabel();
        if (pLabel != nullptr) {
            return pLabel->GetText();
        }
        return DString();
    }

    /** Get the child control at the mouse position
    * @param [in] ptMouse The position of the mouse, a screen coordinate point
    */
    Control* GetSubItem(const UiPoint& ptMouse) const
    {
        UiPoint pt(ptMouse);
        pt.Offset(GetScrollOffsetInScrollBox());
        Control* pFoundSubItem = nullptr;
        size_t nItemCount = GetItemCount();
        for (size_t index = 0; index < nItemCount; ++index) {
            Control* pSubItem = GetItemAt(index);
            if (pSubItem != nullptr) {
                if (pSubItem->IsVisible() && pSubItem->GetRect().ContainsPt(pt)) {
                    pFoundSubItem = pSubItem;
                    break;
                }
            }
        }
        return pFoundSubItem;
    }

private:
    /** The associated ListCtrl interface
    */
    ListCtrl* m_pListCtrl = nullptr;
};

/** List item type of the List view (horizontal layout)
*   Basic structure: <ListCtrlListViewItem> <ListCtrlIcon/><ListCtrlLabel/> </ListCtrlListViewItem>
*   The attributes of the Control and Label inside support reading from the configuration file
*/
class DUI_API ListCtrlListViewItem : public ListCtrlItemBaseH
{
    typedef ListCtrlItemBaseH BaseClass;
public:
    explicit ListCtrlListViewItem(Window* pWindow) :
        ListCtrlItemBaseH(pWindow)
    {
    }
    /** Get the control type
    */
    virtual DString GetType() const override { return _T("ListCtrlListViewItem"); }

    /** Event handling function
    */
    virtual void HandleEvent(const EventArgs& msg) override;

    /** Set the associated ListCtrl interface
    */
    void SetListCtrl(ListCtrl* pListCtrl) { m_pListCtrl = pListCtrl; }

    /** Get the associated ListCtrl interface
    */
    ListCtrl* GetListCtrl() const { return m_pListCtrl; }

public:
    /** Get the index of the associated data item, representing which row of data it is associated with
    * @return Returns the index of the data item, valid range: [0, ListCtrl::GetDataItemCount())
    */
    size_t GetDataItemIndex() const { return GetElementIndex(); }

    /** Get the interface of the icon control
    */
    ListCtrlIcon* GetListCtrlIcon() const { return dynamic_cast<ListCtrlIcon*>(GetItemAt(0)); }

    /** Get the interface of the text control
    */
    ListCtrlLabel* GetListCtrlLabel() const { return dynamic_cast<ListCtrlLabel*>(GetItemAt(1)); }

    /** Get the text inside the text control
    */
    DString GetLabelText() const
    {
        ListCtrlLabel* pLabel = GetListCtrlLabel();
        if (pLabel != nullptr) {
            return pLabel->GetText();
        }
        return DString();
    }

    /** Get the child control at the mouse position
    * @param [in] ptMouse The position of the mouse, a screen coordinate point
    */
    Control* GetSubItem(const UiPoint& ptMouse) const
    {
        UiPoint pt(ptMouse);
        pt.Offset(GetScrollOffsetInScrollBox());
        Control* pFoundSubItem = nullptr;
        size_t nItemCount = GetItemCount();
        for (size_t index = 0; index < nItemCount; ++index) {
            Control* pSubItem = GetItemAt(index);
            if (pSubItem != nullptr) {
                if (pSubItem->IsVisible() && pSubItem->GetRect().ContainsPt(pt)) {
                    pFoundSubItem = pSubItem;
                    break;
                }
            }
        }
        return pFoundSubItem;
    }

private:
    /** The associated ListCtrl interface
    */
    ListCtrl* m_pListCtrl = nullptr;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_DEFS_H_
