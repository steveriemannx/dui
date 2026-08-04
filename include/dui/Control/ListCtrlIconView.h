#ifndef UI_CONTROL_LIST_CTRL_ICON_VIEW_H_
#define UI_CONTROL_LIST_CTRL_ICON_VIEW_H_

#include "dui/Control/ListCtrlDefs.h"

namespace ui
{
/** List data display and layout functionality (Icon type)
*   Basic structure:
*       <ListCtrlIconView>
*           <ListCtrlIconViewItem> <ListCtrlIcon/><ListCtrlLabel/> </ListCtrlIconViewItem>
*           ...
*           <ListCtrlIconViewItem> <ListCtrlIcon/><ListCtrlLabel/> </ListCtrlIconViewItem>
*       </ListCtrlIconView>
*   The attributes of ListCtrlIcon and ListCtrlLabel can be read from the configuration file
*/
class ListCtrl;
class ListCtrlData;
class DUI_API ListCtrlIconView : public ListCtrlView, public IListCtrlView
{
    typedef ListCtrlView BaseClass;
public:
    explicit ListCtrlIconView(Window* pWindow);
    virtual ~ListCtrlIconView() override;

    virtual DString GetType() const override { return _T("ListCtrlIconView"); }
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void HandleEvent(const EventArgs& msg) override;

public:
    /** Set the ListCtrl control interface
    */
    void SetListCtrl(ListCtrl* pListCtrl);

    /** Set the data provider object
    * @param[in] pProvider The data provider object
    */
    virtual void SetDataProvider(VirtualListBoxElement* pProvider) override;

    /** Refresh the view, keep the data and display synchronized
    * @param [in] bSync true means synchronous refresh, redraw the current view immediately; false means asynchronous refresh, the refresh action is an async redraw that marks the region as dirty and lets the system redraw the view
    */
    virtual void Refresh(bool bSync = false) override;

public:
    /** Set the layout direction (by default, the Icon view uses a vertical layout)
    * @param [in] bHorizontal true means horizontal layout, false means vertical layout
    */
    void SetHorizontalLayout(bool bHorizontal);

    /** Determine whether the current layout direction is horizontal
    */
    bool IsHorizontalLayout() const;

    /** Get the child item size; the width and height include the margins and paddings of the control
     */
    UiSize GetItemSize() const;

    /** Set the child item size
     * @param [in] szItem The child item size data; the width and height include the margins and paddings of the control
     */
    void SetItemSize(const UiSize& szItem);

    /** Set whether to use single-line text
    */
    void SetTextSingleLine(bool bSingleLine);

    /** Get whether to use single-line text
    */
    bool IsTextSingleLine(bool& bSingleLine) const;

    /** Listen for the UI element fill event of Icon view data items (virtual list)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to delete the callback)
     * Parameter description:
     *   wParam: The index of the associated UI container child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, no child item is associated
     *   lParam: The data item index corresponding to the associated UI container child item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, no associated data item
     *   pEventData: The control interface pointer of the associated UI container child item, type: ListCtrlIconViewItem*
     */
    void AttachIconViewItemFilled(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventIconViewItemFilled, callback, callbackID); }

protected:
    /** Create a data item
    * @return Returns the pointer of the created data item
    */
    virtual Control* CreateDataItem() override;

    /** Fill the specified data item
    * @param [in] pControl The pointer of the data item control
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @param [in] itemData The data item (attributes of the row)
    * @param [in] subItemList The data sub-items (data of each column; the first is the column ID, the second is the column data)
    */
    virtual bool FillDataItem(Control* pControl,
                              size_t nElementIndex,
                              const ListCtrlItemData& itemData,
                              const std::vector<ListCtrlSubItemData2Pair>& subItemList) override;


    /** Get the maximum width of a column
    * @param [in] subItemList The data sub-items (data of each column)
    * @return Returns the maximum width of the column, DPI-scaled; returns -1 on failure
    */
    virtual int32_t GetMaxDataItemWidth(const std::vector<ListCtrlSubItemData2Ptr>& subItemList) override;

private:
    /** The ListCtrl control interface
    */
    ListCtrl* m_pListCtrl;

    /** The data interface
    */
    ListCtrlData* m_pData;

    /** Whether to use single-line text
    */
    bool m_bSingleLine;
    bool m_bSingleLineFlag;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_ICON_VIEW_H_
