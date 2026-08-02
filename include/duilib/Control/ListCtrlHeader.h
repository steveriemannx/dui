#ifndef UI_CONTROL_LIST_CTRL_HEADER_H_
#define UI_CONTROL_LIST_CTRL_HEADER_H_

#include "duilib/Box/ListBoxItem.h"

namespace ui
{

/** The header control of ListCtrl
*/
class ListCtrl;
class ListCtrlHeaderItem;
struct ListCtrlColumn;

/*The basic structure of the header control is as follows:
*   <ListCtrlHeader>
*       <ListCtrlHeaderItem/>
*       <SplitBox> <Control/> </SplitBox> //SplitBox is used to implement the drag function, and the Control inside it determines its display shape
*       ...
*       <ListCtrlHeaderItem/>
*       <SplitBox> <Control/> </SplitBox>
*   </ListCtrlHeader>
*/
class DUILIB_API ListCtrlHeader : public ListBoxItemH
{
    typedef ListBoxItemH BaseClass;
    friend class ListCtrlHeaderItem;
public:
    explicit ListCtrlHeader(Window* pWindow);
    virtual ~ListCtrlHeader() override;

    /** Get the control type
    */
    virtual DString GetType() const override;

    /** Set attributes
    */
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

    /** DPI changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

    /** Determine whether the control type is selectable
     * @return Returns false by default
     */
    virtual bool IsSelectableType() const override;

public:
    /** Insert a column at the specified position
    * @param [in] columnIndex Insert the column after the given column index; if it is -1, append the column at the end
    * @param [in] columnInfo The basic properties of the column
    * @return Returns the header control interface of this column
    */
    ListCtrlHeaderItem* InsertColumn(int32_t columnIndex, const ListCtrlColumn& columnInfo);

    /** Get the number of columns
    */
    size_t GetColumnCount() const;

    /** Get the column width
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    */
    int32_t GetColumnWidth(size_t columnIndex) const;

    /** Adjust the column width (auto-fit based on the actual width of the column content)
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] nWidth The column width value
    * @param [in] bNeedDpiScale Whether the column width value needs DPI scaling
    */
    bool SetColumnWidth(size_t columnIndex, int32_t nWidth, bool bNeedDpiScale);

    /** Get the control interface of the list header
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    */
    ListCtrlHeaderItem* GetColumn(size_t columnIndex) const;

    /** Get the control interface of the list header
    * @param [in] columnId The ID value of the column, obtained via the ListCtrlHeaderItem::GetColumnId() function
    */
    ListCtrlHeaderItem* GetColumnById(size_t columnId) const;

    /** Get the column width and the column index
    * @param [in] columnId The ID value of the column, obtained via the ListCtrlHeaderItem::GetColumnId() function
    * @param [out] columnIndex The index of the column: [0, GetColumnCount()), which column it represents
    * @param [out] nColumnWidth The width value of the column
    */
    bool GetColumnInfo(size_t columnId, size_t& columnIndex, int32_t& nColumnWidth) const;

    /** Determine whether the column ID is valid
    * @param [in] columnId The ID value of the column, obtained via the ListCtrlHeaderItem::GetColumnId() function
    */
    bool IsValidColumnId(size_t columnId) const;

    /** Get the index of the column
    * @param [in] columnId The ID value of the column, obtained via the ListCtrlHeaderItem::GetColumnId() function
    * @return The index of the column: [0, GetColumnCount()), which column it represents
    */
    size_t GetColumnIndex(size_t columnId) const;

    /** Determine whether the column index is valid
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    */
    bool IsValidColumnIndex(size_t columnIndex) const;

    /** Get the index of the column
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @return The ID of the column; if no match is found, return Box::InvalidIndex
    */
    size_t GetColumnId(size_t columnIndex) const;

    /** Delete a column
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] columnId The column ID
    */
    bool DeleteColumn(size_t columnIndex);
    bool DeleteColumnById(size_t columnId);

    /** Set whether to show the CheckBox in the header
    * @param [in] bShow true means showing the CheckBox in the header, false means not showing it
    */
    bool SetShowCheckBox(bool bShow);

    /** Determine whether the CheckBox is shown
    */
    bool IsShowCheckBox() const;

    /** Disable/Enable the CheckBox check state change event
    * @return Returns the original state for restoration
    */
    bool SetEnableCheckChangeEvent(bool bEnable);

    /** Set the left Padding value, used to align with the data rows
    */
    void SetPaddingLeftValue(int32_t nPaddingLeft);

    /** Set the spacing between text and icon (in pixels)
    */
    void SetIconSpacing(int32_t nIconSpacing, bool bNeedDpiScale);

    /** Get the spacing between text and icon (in pixels)
    */
    int32_t GetIconSpacing() const;

    /** Set the current sort column (by index)
    * @param [in] columnIndex The column index: [0, GetColumnCount())
    * @param [in] bSortUp true means ascending order, false means descending order
    * @param [in] bTriggerEvent true means triggering the sort change event to re-sort the data; otherwise the event is not triggered
    */
    void SetSortColumnIndex(size_t columnIndex, bool bSortUp, bool bTriggerEvent = false);

    /** Set the current sort column (by column ID)
    * @param [in] columnId The ID value of the column
    * @param [in] bSortUp true means ascending order, false means descending order
    * @param [in] bTriggerEvent true means triggering the sort change event to re-sort the data; otherwise the event is not triggered
    */
    void SetSortColumnId(size_t columnId, bool bSortUp, bool bTriggerEvent = false);

    /** Get the rectangle ranges of the column split controls, in client area coordinates
    */
    void GetHeaderSplitControlRect(std::vector<UiRect>& rcSplitControls) const;

public:

    /** Set the associated ListCtrl interface
    */
    void SetListCtrl(ListCtrl* pListCtrl);

    /** Get the associated ListCtrl interface
    */
    ListCtrl* GetListCtrl() const;

public:
    /** Event handler for right-clicking the list header
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to the callback function (used to delete the callback)
    *   Parameter description: if wParam is not 0, its value is: ListCtrlHeaderItem*
    *           The clicked column can be obtained via: ListCtrlHeaderItem* pHeaderItem = (ListCtrlHeaderItem*)wParam;
    *           If wParam is 0, it means the click was on the blank area of the ListCtrlHeader itself, not on any column
    */
    void AttachRClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventRClick, callback, callbackID); }

protected:
    /** Whether dragging the list header to change the column order is supported
    */
    bool IsEnableHeaderDragOrder() const;

    /** Event handler for dragging the list header to change the column width
    * @param [in] pLeftHeaderItem The control interface of the left header
    * @param [in] pRightHeaderItem The control interface of the right header
    */
    void OnHeaderColumnResized(Control* pLeftHeaderItem, Control* pRightHeaderItem);

    /** Event handler for clicking the list header to trigger sorting
    * @param [in] pHeaderItem The control interface of the header
    */
    void OnHeaderColumnSorted(ListCtrlHeaderItem* pHeaderItem);

    /** Adjust the column order by dragging the list header
    */
    void OnHeaderColumnOrderChanged();

    /** CheckBox check state operation
    * @param [in] pHeaderItem The control interface of the header
    * @param [in] bChecked true means checked (Checked state), false means unchecked (UnChecked state)
    */
    void OnHeaderColumnCheckStateChanged(ListCtrlHeaderItem* pHeaderItem, bool bChecked);

    /** The display properties of the header columns have changed
    */
    void OnHeaderColumnVisibleChanged();

    /** Double-click event on the splitter of a header column
    * @param [in] pHeaderItem The control interface of the header
    */
    void OnHeaderColumnSplitDoubleClick(ListCtrlHeaderItem* pHeaderItem);

    /** Whether the check mode is supported (currently used by TreeView/ListCtrl)
        The check mode means:
        (1) Only when clicking on the CheckBox image is the check box image in the selected state (in non-check mode, clicking anywhere within the control rect selects it)
        (2) The checked state and the selected state are separated, two different states
    */
    virtual bool SupportCheckMode() const override;

    /** Check state change event (the m_bChecked variable has changed)
    */
    virtual void OnPrivateSetChecked() override;

    /** Get the image width of the CheckBox
    */
    int32_t GetCheckBoxImageWidth();

    /** Update the Padding to ensure text and icon do not overlap
    */
    void UpdatePaddingLeft();

private:
    /** The associated ListCtrl interface
    */
    ListCtrl* m_pListCtrl;

    /** The left Padding value, used to align with the data rows
    */
    int32_t m_nPaddingLeftValue;

    /** The spacing between text and icon (when the icon is displayed to the right or left of the text)
    */
    int16_t m_nIconSpacing;

    /** Whether the CheckBox check state change event is enabled
    */
    bool m_bEnableCheckChangeEvent;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_HEADER_H_
