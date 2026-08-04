#ifndef UI_CONTROL_LIST_CTRL_REPORT_VIEW_H_
#define UI_CONTROL_LIST_CTRL_REPORT_VIEW_H_

#include "dui/Control/ListCtrlDefs.h"

namespace ui
{
//Contains classes: ListCtrlReportView / ListCtrlReportLayout

/** List data display and layout functionality (Report type)
 *  The basic data structure of the ReportView view:
 *  <ListCtrlReportView>
 *      <ListCtrlItem> <ListCtrlSubItem/> ... <ListCtrlSubItem/> </ListCtrlItem>
 *      ...
 *      <ListCtrlItem> <ListCtrlSubItem/> ... <ListCtrlSubItem/> </ListCtrlItem>
 *  </ListCtrlReportView>
 */
class ListCtrl;
class ListCtrlData;
struct ListCtrlItemData;
class DUI_API ListCtrlReportView : public ListCtrlView, public IListCtrlView
{
    typedef ListCtrlView BaseClass;
    friend class ListCtrlReportLayout;
public:
    explicit ListCtrlReportView(Window* pWindow);
    virtual ~ListCtrlReportView() override;

    virtual DString GetType() const override { return _T("ListCtrlReportView"); }
    virtual void HandleEvent(const EventArgs& msg) override;

    /** Set the ListCtrl control interface
    */
    void SetListCtrl(ListCtrl* pListCtrl);

    /** Set the data provider object
    * @param[in] pProvider The data provider object
    */
    virtual void SetDataProvider(VirtualListBoxElement* pProvider) override;

    /** Refresh the interface to keep data and display in sync
    * @param [in] bSync true means synchronous refresh, immediately redrawing the current view; false means asynchronous refresh, where the refresh action is an asynchronous redraw that marks the dirty region and lets the system redraw the interface
    */
    virtual void Refresh(bool bSync = false) override;

    /** Get the width of the list control (the sum of the total widths of all Header columns)
    */
    int32_t GetListCtrlWidth() const;

    /** Set the index of the top element
    */
    void SetTopElementIndex(size_t nTopElementIndex);

    /** Get the index of the top element
    */
    virtual size_t GetTopElementIndex() const override;

    /** Set the list of currently displayed data items, in order from top to bottom
    * @param [in] itemIndexList The index list of the currently displayed data items
    */
    void SetDisplayDataItems(const std::vector<size_t>& itemIndexList);

    /** Get the list of currently displayed data items, in order from top to bottom
    * @param [in] itemIndexList The index list of the currently displayed data items
    */
    virtual void GetDisplayDataItems(std::vector<size_t>& itemIndexList) const override;

    /** Determine whether a data item is visible
    * @param [in] itemIndex The index of the data item
    */
    virtual bool IsDataItemDisplay(size_t itemIndex) const override;

    /** Ensure the data item is visible
    * @param [in] itemIndex The index of the data item
    * @param [in] bToTop Whether to ensure it is at the very top
    */
    virtual bool EnsureDataItemVisible(size_t itemIndex, bool bToTop) override;

public:
    /** Get the index of the first element within the visible range
    * @param [in] nScrollPosY The current Y scroll bar position
    * @return Returns the index of the element
    */
    size_t GetTopDataItemIndex(int64_t nScrollPosY) const;

    /** Get the height of the specified element (row height)
    * @param [in] itemIndex The index of the data item
    */
    int32_t GetDataItemHeight(size_t itemIndex) const;

    struct ShowItemInfo
    {
        size_t nItemIndex;      //Element index
        int32_t nItemHeight;    //Height of the element
    };

    /** Get the data to be displayed: pinned-to-top items first, and in order
    @param [in] nScrollPosY The current Y scroll bar position
    @param [in] maxCount The maximum number of records to take (including pinned-to-top and non-pinned-to-top)
    @param [out] itemIndexList Returns the indices of the elements to display (non-pinned-to-top)
    @param [out] atTopItemIndexList Returns the indices of the elements to display (pinned-to-top)
    @param [out] nPrevItemHeights The total height of all elements before the first visible element (not including pinned-to-top elements)
    */
    void GetDataItemsToShow(int64_t nScrollPosY, size_t maxCount, 
                            std::vector<ShowItemInfo>& itemIndexList,
                            std::vector<ShowItemInfo>& atTopItemIndexList,
                            int64_t& nPrevItemHeights) const;

    /** Get the maximum number of data items that can be displayed in a region of the specified height
    @param [in] nScrollPosY The current Y scroll bar position
    @param [in] nRectHeight The region height
    @param [out] pItemIndexList Returns the indices of the elements that can be displayed
    @param [out] pAtTopItemIndexList The indices of the pinned-to-top items
    @return Returns the number of data items that can be displayed
    */
    int32_t GetMaxDataItemsToShow(int64_t nScrollPosY, int32_t nRectHeight, 
                                  std::vector<size_t>* pItemIndexList = nullptr,
                                  std::vector<size_t>* pAtTopItemIndexList = nullptr) const;

    /** Get the total height of the display position of the specified element
    * @param [in] itemIndex The index of the data item
    * @parma [in] bIncludeAtTops Whether to include the height of pinned-to-top elements
    * @return The total height of the display position, not including the element itself
    */
    int64_t GetDataItemTotalHeights(size_t itemIndex, bool bIncludeAtTops) const;

    /** Listen for the Report view data item UI element fill event (virtual list, fills the data of a row)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container sub-item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it means no sub-item is associated
     *   lParam: The data item index corresponding to the associated UI container sub-item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, it means no data item is associated
     *   pEventData: The UI control interface pointer of the associated UI container sub-item, type: ListCtrlItem* pointer, data is filled by row
     */
    void AttachReportViewItemFilled(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventReportViewItemFilled, callback, callbackID); }

    /** Listen for the Report view data item UI element fill event (virtual list, fills the data of a row and column)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The index of the associated UI container sub-item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it means no sub-item is associated
     *   lParam: The data item index corresponding to the associated UI container sub-item index, valid range: [0, GetDataItemCount()); if the value is Box::InvalidIndex, it means no data item is associated
     *   pEventData: The UI control interface pointer of the associated UI container sub-item, type: ListCtrlSubItem* pointer, representing the UI element control of the data at a certain row and column
     *               Through this interface, you can get the data item index and which column's data it belongs to
     */
    void AttachReportViewSubItemFilled(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventReportViewSubItemFilled, callback, callbackID); }

public:
    /** Whether it is normal mode (all row heights are the default row height, no hidden rows, no pinned-to-top rows)
    */
    bool IsNormalMode() const;

    /** Set the UI control indices to be pinned to top
    */
    void SetAtTopControlIndex(const std::vector<size_t>& atTopControlList);

    /** Adjust the number of UI controls to ensure enough data is displayed
    */
    void AjustItemCount();

public:
    /** Width of the horizontal grid line
    * @param [in] nLineWidth The width of the grid line; if it is 0, the horizontal grid line is not displayed
    * @param [in] bNeedDpiScale If true, the width needs to be DPI-adaptive
    */
    void SetRowGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale);
    int32_t GetRowGridLineWidth() const;

    /** Color of the horizontal grid line
    * @param [in] color The color of the horizontal grid line
    */
    void SetRowGridLineColor(const DString& color);
    DString GetRowGridLineColor() const;

    /** Width of the vertical grid line
    * @param [in] nLineWidth The width of the grid line; if it is 0, the vertical grid line is not displayed
    * @param [in] bNeedDpiScale If true, the width needs to be DPI-adaptive
    */
    void SetColumnGridLineWidth(int32_t nLineWidth, bool bNeedDpiScale);
    int32_t GetColumnGridLineWidth() const;

    /** Color of the vertical grid line
    * @param [in] color The color of the vertical grid line
    */
    void SetColumnGridLineColor(const DString& color);
    DString GetColumnGridLineColor() const;

    /** Adjust the column widths: the 1st value in the array is the column index, and the 2nd value is the column width
    */
    void AdjustSubItemWidth(const std::map<size_t, int32_t>& subItemWidths);

protected:
    /** Draw child controls
    */
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;

    /** Find a child control
    */
    virtual Control* FindControl(FINDCONTROLPROC Proc, void* pProcData, uint32_t uFlags,
                                 const UiPoint& ptMouse = UiPoint(),
                                 const UiPoint& scrollPos = UiPoint()) override;

    /** Create a data item
    * @return Returns the pointer to the created data item
    */
    virtual Control* CreateDataItem() override;

    /** Fill the specified data item
    * @param [in] pControl The pointer to the data item control
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @param [in] itemData The data item (the attributes representing the row)
    * @param [in] subItemList The data sub-items (representing the data of each column; the 1st is the column ID, and the 2nd is the column data)
    */
    virtual bool FillDataItem(Control* pControl,
                              size_t nElementIndex,
                              const ListCtrlItemData& itemData,
                              const std::vector<ListCtrlSubItemData2Pair>& subItemList) override;


    /** Get the maximum width of a column
    * @param [in] subItemList The data sub-items (representing the data of each column)
    * @return Returns the maximum width of the column, which is the value after DPI adaptation; returns -1 on failure
    */
    virtual int32_t GetMaxDataItemWidth(const std::vector<ListCtrlSubItemData2Ptr>& subItemList) override;

    /** Calculate how many sub-items are displayed on this page
    * @param [in] bIsHorizontal Whether the current layout is a horizontal layout
    * @param [out] nColumns Returns the number of columns
    * @param [out] nRows Returns the number of rows
    * @return Returns the number of records displayed in the visible region
    */
    virtual size_t GetDisplayItemCount(bool bIsHorizontal, size_t& nColumns, size_t& nRows) const override;

    /** Determine whether a data element is selectable
    * @param [in] nElementIndex The element index, valid range: [0, GetElementCount())
    */
    virtual bool IsSelectableElement(size_t nElementIndex) const override;

    /** Get the next selectable data element
    * @param [in] nElementIndex The element index, valid range: [0, GetElementCount())
    * @param [in] bForward true means searching forward, false means searching backward
    */
    virtual size_t FindSelectableElement(size_t nElementIndex, bool bForward) const override;

    /** The selection state change event of a sub-item, used for state synchronization
    * @param [in] iIndex The ID of the sub-item, range: [0, GetItemCount())
    * @param [in] pListBoxItem The associated list item interface
    */
    virtual void OnItemSelectedChanged(size_t iIndex, IListBoxItem* pListBoxItem) override;

    /** The check state change event of a sub-item, used for state synchronization
    * @param [in] iIndex The ID of the sub-item, range: [0, GetItemCount())
    * @param [in] pListBoxItem The associated list item interface
    */
    virtual void OnItemCheckedChanged(size_t iIndex, IListBoxItem* pListBoxItem) override;

    /** The selection state has changed
    */
    virtual void OnSelectStatusChanged() override;

    /** Get the scroll amount of the scroll view
    */
    virtual void GetScrollDeltaValue(int32_t& nHScrollValue, int32_t& nVScrollValue) const override;

    /** A mouse frame selection operation has been performed
    * @param [in] left The X coordinate left value of the frame selection
    * @param [in] right The X coordinate right value of the frame selection
    * @param [in] top The Y coordinate top value of the frame selection
    * @param [in] bottom The Y coordinate bottom value of the frame selection
    * @return Returns true if there is a selection change, otherwise returns false
    */
    virtual bool OnFrameSelection(int64_t left, int64_t right, int64_t top, int64_t bottom) override;

    /** The Check state change of a data item (column level)
    * @param [in] nElementIndex The index of the data item, valid range: [0, GetDataItemCount())
    * @param [in] nColumnId The column ID
    * @param [in] bChecked Whether it is checked
    */
    void OnSubItemColumnChecked(size_t nElementIndex, size_t nColumnId, bool bChecked);

private:
    /** Draw grid lines
    */
    void PaintGridLines(IRender* pRender);

    /** Move the header and pinned-to-top items to the end
    * @param [in,out] items The list of controls to move
    * @param [out] atTopItems Returns the list of pinned-to-top controls
    */
    void MoveTopItemsToLast(std::vector<Control*>& items, std::vector<Control*>& atTopItems) const;

private:
    /** ListCtrl control interface
    */
    ListCtrl* m_pListCtrl;

    /** Data interface
    */
    ListCtrlData* m_pData;

    /** The index of the top element (used for drawing grid lines)
    */
    size_t m_nTopElementIndex;

    /** The currently visible element list
    */
    std::vector<size_t> m_diplayItemIndexList;

    /** The UI control indices pinned to top
    */
    std::vector<size_t> m_atTopControlList;

private:
    /** Width of the horizontal grid line
    */
    int32_t m_nRowGridLineWidth;

    /** Color of the horizontal grid line
    */
    UiString m_rowGridLineColor;

    /** Width of the vertical grid line
    */
    int32_t m_nColumnGridLineWidth;

    /** Color of the vertical grid line
    */
    UiString m_columnGridLineColor;
};

/** Layout management interface of the list data display control
*/
class DUI_API ListCtrlReportLayout : public Layout, public VirtualLayout
{
    typedef Layout BaseClass;
public:
    ListCtrlReportLayout();

    /** Layout type
    */
    virtual LayoutType GetLayoutType() const override { return LayoutType::ListCtrlReportLayout; }

    /** Adjust the position and size of all internal child controls according to the layout policy
     * @param [in] items The list of child controls
     * @param [in] rc The current container position and size information, including padding, but not including margin
     * @param [in] bEstimateOnly true means only estimating without adjusting the control positions, false means adjusting the control positions
     * @return Returns the width and height information of the final layout after arrangement, including the padding of the Box container, but not including the margin of the Box container itself (use this return value when the container supports a scroll bar)
     */
    virtual UiSize64 ArrangeChildren(const std::vector<Control*>& items, UiRect rc, bool bEstimateOnly = false) override;

    /** Estimate the container layout size based on the internal child control sizes (used to evaluate the size of controls whose width or height is "auto"; stretched child controls are not counted in the size estimation)
     * @param [in] items The list of child controls
     * @param [in] szAvailable The available width and height of the container, including the padding assigned to the container, but not including the margin assigned to the container
     * @return Returns the size information (width and height) of the final layout after arrangement, including the padding of the Box container itself, but not including the margin of the Box container itself;
     */
    virtual UiSize64 EstimateLayoutSize(const std::vector<Control*>& items, UiSize szAvailable) override;

public:
    /** Lazy-load and display data
    * @param [in] rc The current container size information; when called externally, the padding needs to be removed first
    */
    virtual void LazyArrangeChild(UiRect rc) const override;

    /** Get the maximum number of real data items to display (i.e., real data items that have corresponding Control objects)
    * @param [in] rc The current container size information; when called externally, the padding needs to be removed first
    */
    virtual size_t AjustMaxItem(UiRect rc) const override;

    /** Get the index of the previous element of the first element within the visible range
    * @param [in] rc The rectangle of the current display region, not including padding
    * @return Returns the index of the element
    */
    virtual size_t GetTopElementIndex(UiRect rc) const override;

    /** Determine whether an element is within the visible range
    * @param[in] iIndex The element index, range: [0, GetElementCount())
    * @param [in] rc The rectangle of the current display region, not including padding
    * @return Returns true if visible, otherwise not visible
    */
    virtual bool IsElementDisplay(UiRect rc, size_t iIndex) const override;

    /** Determine whether re-layout is needed
    */
    virtual bool NeedReArrange() const override;

    /** Get the data element indices of all currently visible controls
    * @param [in] rc The rectangle of the current display region, not including padding
    * @param[out] collection The index list, range: [0, GetElementCount())
    */
    virtual void GetDisplayElements(UiRect rc, std::vector<size_t>& collection) const override;

    /** Make the control within the visible range
    * @param [in] rc The rectangle of the current display region, not including padding
    * @param[in] iIndex The element index, range: [0, GetElementCount())
    * @param[in] bToTop Whether it is at the very top
    */
    virtual void EnsureVisible(UiRect rc, size_t iIndex, bool bToTop) const override;

public:
    /** Set the associated Box interface
    */
    void SetDataView(ListCtrlReportView* pDataView);

private:
    /** Get the associated Box interface
    */
    ListCtrlReportView* GetDataView() const { return m_pDataView; }

    /** Get the height of data items, not including the header
    * @param [in] nCount The number of data items; if it is Box::InvalidIndex, get the total height of all data items
    * @parma [in] bIncludeAtTops Whether to include the height of pinned-to-top elements
    * @return Returns the total height of nCount data items, not including the height of the header
    */
    int64_t GetElementsHeight(size_t nCount, bool bIncludeAtTops) const;

    /** Get the height and width of a data item
    * @param [in] rcWidth The current container width; when called externally, the padding needs to be removed first
    * @param [in] nElementIndex The index of the data element
    * @return Returns the height and width of the data element
    */
    UiSize GetElementSize(int32_t rcWidth, size_t nElementIndex) const;

    /** Get the row width
    */
    int32_t GetItemWidth() const;

    /** Get the row height (currently only supports the case where all rows have equal height)
    */
    int32_t GetItemHeight() const;

    /** Get the height of the header control
    */
    int32_t GetHeaderHeight() const;

    /** Lazy-load and display data (normal mode, same row heights, no hidden items, no pinned-to-top items)
    * @param [in] rc The current container size information; when called externally, the padding needs to be removed first
    */
    void LazyArrangeChildNormal(UiRect rc) const;

    /** Get the data element indices of all currently visible controls
    * @param [in] rc The rectangle of the current display region, not including padding
    * @param[out] collection The index list, range: [0, GetElementCount())
    */
    void GetDisplayElements(UiRect rc, std::vector<size_t>& collection, 
                            std::vector<size_t>* pAtTopItemIndexList) const;

    /** The reserved space at the bottom, ensuring the last data item is easy to view completely when scrolled to the very bottom
    */
    int32_t GetReserveHeight() const;

private:
    /** The associated ListBox interface
    */
    ListCtrlReportView* m_pDataView;

    /** Whether the bottom reserved space is set
    */
    bool m_bReserveSet;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_REPORT_VIEW_H_
