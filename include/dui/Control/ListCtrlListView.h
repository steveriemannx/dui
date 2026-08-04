#ifndef UI_CONTROL_LIST_CTRL_LIST_VIEW_H_
#define UI_CONTROL_LIST_CTRL_LIST_VIEW_H_

#include "dui/Control/ListCtrlDefs.h"

namespace ui
{
/** List data display and layout functionality (List type)
*   Basic structure:
*       <ListCtrlListView>
*           <ListCtrlListViewItem> <ListCtrlIcon/><ListCtrlLabel/> </ListCtrlListViewItem>
*           ...
*           <ListCtrlListViewItem> <ListCtrlIcon/><ListCtrlLabel/> </ListCtrlListViewItem>
*       </ListCtrlListView>
*   The properties of ListCtrlIcon and ListCtrlLabel support reading from the configuration file
*/
class ListCtrl;
class ListCtrlData;
class DUI_API ListCtrlListView : public ListCtrlView, public IListCtrlView
{
    typedef ListCtrlView BaseClass;
public:
    explicit ListCtrlListView(Window* pWindow);
    virtual ~ListCtrlListView() override;

    virtual DString GetType() const override { return _T("ListCtrlListView"); }
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void HandleEvent(const EventArgs& msg) override;

public:
    /** Set the ListCtrl control interface
    */
    void SetListCtrl(ListCtrl* pListCtrl);

    /** Set the data provider object
    * @param[in] pProvider data provider object
    */
    virtual void SetDataProvider(VirtualListBoxElement* pProvider) override;

    /** Refresh the interface to keep the data in sync with the display
    * @param [in] bSync true means a synchronous refresh that immediately redraws the current view; false means an asynchronous refresh, where the refresh action is an asynchronous redraw that marks the area as dirty and lets the system redraw the interface
    */
    virtual void Refresh(bool bSync = false) override;

public:
    /** Set the layout direction (by default, the List view uses a vertical layout)
    * @param [in] bHorizontal true means a horizontal layout, false means a vertical layout
    */
    void SetHorizontalLayout(bool bHorizontal);

    /** Determine whether the current layout direction is horizontal
    */
    bool IsHorizontalLayout() const;

    /** Get the item size; the width and height include the margin and padding of the control
    */
    UiSize GetItemSize() const;

    /** Set the item size
     * @param [in] szItem item size data; the width and height include the margin and padding of the control
     */
    void SetItemSize(const UiSize& szItem);

    /** Set whether to use single-line text
    */
    void SetTextSingleLine(bool bSingleLine);

    /** Get whether to use single-line text
    */
    bool IsTextSingleLine(bool& bSingleLine) const;

    /** Listen for the List view data item UI element filling event (virtual table)
     * @param [in] callback the callback function to bind
     * @param [in] callbackID the ID corresponding to the callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: the item index of the associated UI container, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it means no item is associated
     *   lParam: the data item index corresponding to the item index of the associated UI container, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, it means there is no associated data item
     *   pEventData: the interface pointer of the UI control of the associated UI container item, type: ListCtrlListViewItem* pointer
     */
    void AttachListViewItemFilled(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventListViewItemFilled, callback, callbackID); }

protected:
    /** Create a data item
    * @return the pointer of the created data item
    */
    virtual Control* CreateDataItem() override;

    /** Fill the specified data item
    * @param [in] pControl data item control pointer
    * @param [in] nElementIndex the index ID of the data element, range: [0, GetElementCount())
    * @param [in] itemData data item (representing the row properties)
    * @param [in] subItemList data sub-items (representing the data of each column; the first is the column ID, the second is the column data)
    */
    virtual bool FillDataItem(Control* pControl,
                              size_t nElementIndex,
                              const ListCtrlItemData& itemData,
                              const std::vector<ListCtrlSubItemData2Pair>& subItemList) override;


    /** Get the maximum width of a column
    * @param [in] subItemList data sub-items (representing the data of each column)
    * @return the maximum width of the column, which is the value after DPI adaptation; returns -1 on failure
    */
    virtual int32_t GetMaxDataItemWidth(const std::vector<ListCtrlSubItemData2Ptr>& subItemList) override;

private:
    /** ListCtrl control interface
    */
    ListCtrl* m_pListCtrl;

    /** Data interface
    */
    ListCtrlData* m_pData;

    /** Whether to use single-line text
    */
    bool m_bSingleLine;
    bool m_bSingleLineFlag;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_LIST_VIEW_H_
