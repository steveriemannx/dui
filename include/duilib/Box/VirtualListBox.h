#ifndef UI_BOX_VIRTUAL_LISTBOX_H_
#define UI_BOX_VIRTUAL_LISTBOX_H_

#include "duilib/Box/ListBox.h"
#include "duilib/Layout/VirtualHLayout.h"
#include "duilib/Layout/VirtualVLayout.h"
#include "duilib/Layout/VirtualHTileLayout.h"
#include "duilib/Layout/VirtualVTileLayout.h"
#include "duilib/Core/Callback.h"

namespace ui {

typedef std::function<void(size_t nStartIndex, size_t nEndIndex)> DataChangedNotify;
typedef std::function<void()> CountChangedNotify;

class VirtualListBox;
class DUILIB_API VirtualListBoxElement : public virtual SupportWeakCallback
{
public:
    VirtualListBoxElement();

    /** Create a data item
    * @param [in] pVirtualListBox The interface of the associated virtual list
    * @return Returns the pointer to the created data item
    */
    virtual ui::Control* CreateElement(ui::VirtualListBox* pVirtualListBox) = 0;

    /** Fill the specified data item
    * @param [in] pControl The data item control pointer
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    */
    virtual bool FillElement(ui::Control* pControl, size_t nElementIndex) = 0;

    /** Get the total number of data items
    * @return Returns the total number of data items
    */
    virtual size_t GetElementCount() const = 0;

    /** Set the selection state
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @param [in] bSelected true indicates selected, false indicates not selected
    */
    virtual void SetElementSelected(size_t nElementIndex, bool bSelected) = 0;

    /** Get the selection state
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @return true indicates selected, false indicates not selected
    */
    virtual bool IsElementSelected(size_t nElementIndex) const = 0;

    /** Get the list of selected elements
    * @param [in] selectedIndexs Returns the list of currently selected elements, valid range: [0, GetElementCount())
    */
    virtual void GetSelectedElements(std::vector<size_t>& selectedIndexs) const = 0;

    /** Whether multi-selection is supported
    */
    virtual bool IsMultiSelect() const = 0;

    /** Set whether multi-selection is supported, called by the UI layer to keep consistent with the UI control
    * @return bMultiSelect true indicates multi-selection is supported, false indicates it is not
    */
    virtual void SetMultiSelect(bool bMultiSelect) = 0;

public:
    /** Register the event notification callbacks
    * @param [in] pVirtualListBox The associated VirtualListBox object
    * @param [in] dcNotify The callback function for data content change notifications
    * @param [in] ccNotify The callback function for data item count change notifications
    */
    void RegNotifys(VirtualListBox* pVirtualListBox,
                    const DataChangedNotify& dcNotify,
                    const CountChangedNotify& ccNotify);

    /** Unregister the event notification callbacks
    * @param [in] pVirtualListBox The associated VirtualListBox object
    */
    void UnRegNotifys(VirtualListBox* pVirtualListBox);

protected:

    /** Send a notification: the data content has changed
    * @param [in] nStartElementIndex The start index of the data
    * @param [in] nEndElementIndex The end index of the data
    */
    void EmitDataChanged(size_t nStartElementIndex, size_t nEndElementIndex);

    /** Send a notification: the number of data items has changed
    */
    void EmitCountChanged();

private:
    /** The VirtualListBox object associated with the callbacks
    */
    VirtualListBox* m_pVirtualListBox;

    /** The callback function for data content changes
    */
    DataChangedNotify m_pfnDataChangedNotify;

    /** The callback function for data count changes
    */
    CountChangedNotify m_pfnCountChangedNotify;
};

/** A ListBox implemented with a virtual list, supporting large amounts of data and scroll bars
*/
class DUILIB_API VirtualListBox : public ListBox
{
    typedef ListBox BaseClass;
    friend class VirtualHLayout;
    friend class VirtualVLayout;    
    friend class VirtualHTileLayout;
    friend class VirtualVTileLayout;
public:
    VirtualListBox(Window* pWindow, Layout* pLayout = nullptr);

    /** Set the data provider object
    * @param[in] pProvider Developers need to override the VirtualListBoxElement interface to serve as the data provider object
    */
    virtual void SetDataProvider(VirtualListBoxElement* pProvider);

    /** Get the data provider object
    */
    VirtualListBoxElement* GetDataProvider() const;

    /** Whether a data provider interface exists
    */
    bool HasDataProvider() const;

    /** Whether multi-selection is supported
    */
    virtual bool IsMultiSelect() const override;

    /** Set whether multi-selection is supported, called by the UI layer to keep consistent with the UI control
    * @return bMultiSelect true indicates multi-selection is supported, false indicates it is not
    */
    virtual void SetMultiSelect(bool bMultiSelect) override;

public:
    /** Get the total number of data elements
    * @return Returns the total number of data elements
    */
    size_t GetElementCount() const;

    /** Get the index of the currently selected data element (only valid in single-selection mode)
    @return Returns the index of the selected data element, range: [0, GetElementCount())
    */
    size_t GetCurSelElement() const;

    /** Set the selection state and update the UI display as needed
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @param [in] bSelected true indicates selected, false indicates not selected
    */
    void SetElementSelected(size_t nElementIndex, bool bSelected);

    /** Set selection for a batch of elements and update the UI display as needed
    * @param [in] selectedIndexs The list of elements to be selected, valid range: [0, GetElementCount())
    * @param [in] bClearOthers If true, clears the selection of other already-selected elements, keeping only those set in this call
    * @return Returns true if the data changed, otherwise false
    */
    bool SetSelectedElements(const std::vector<size_t>& selectedIndexs, bool bClearOthers);

    /** Set selection for a batch of elements without updating the UI display
    * @param [in] selectedIndexs The list of elements to be selected, valid range: [0, GetElementCount())
    * @param [in] bClearOthers If true, clears the selection of other already-selected elements, keeping only those set in this call
    * @param [out] refreshIndexs Returns the indices of elements whose display needs to be refreshed
    */
    void SetSelectedElements(const std::vector<size_t>& selectedIndexs, 
                             bool bClearOthers,
                             std::vector<size_t>& refreshIndexs);

    /** Get the selection state
    * @param [in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    * @return true indicates selected, false indicates not selected
    */
    bool IsElementSelected(size_t nElementIndex) const;

    /** Get the list of selected elements
    * @param [in] selectedIndexs Returns the list of currently selected elements, valid range: [0, GetElementCount())
    */
    void GetSelectedElements(std::vector<size_t>& selectedIndexs) const;

    /** Select all and update the UI display as needed
    * @return Returns true if the data changed, otherwise false
    */
    virtual bool SetSelectAll() override;

    /** Cancel all selections and update the UI display as needed
    * @return Returns true if the data changed, otherwise false
    */
    virtual bool SetSelectNone() override;

    /** Cancel all selections without updating the UI display; the display can be refreshed externally
    * @param [out] refreshIndexs Returns the indices of elements whose display needs to be refreshed
    */
    void SetSelectNone(std::vector<size_t>& refreshIndexs);

    /** Cancel all selections (excluding some elements) without updating the UI display; the display can be refreshed externally
    * @param [in] excludeIndexs The indices of elements to exclude; the selection state of these elements remains unchanged
    * @param [out] refreshIndexs Returns the indices of elements whose display needs to be refreshed
    */
    void SetSelectNoneExclude(const std::vector<size_t>& excludeIndexs, 
                              std::vector<size_t>& refreshIndexs);

    /** Get the data element indices of all currently visible controls
    * @param [out] collection The index list, valid range: [0, GetElementCount())
    */
    void GetDisplayElements(std::vector<size_t>& collection) const;

    /** Get the index of the display control corresponding to an element
    * @param [in] nElementIndex The element index, valid range: [0, GetElementCount())
    * @return Returns the index of the UI control corresponding to the element, valid range: [0, GetItemCount())
    */
    size_t GetDisplayItemIndex(size_t nElementIndex) const;

    /** Get the element index associated with a display control
    * @param [in] nItemIndex The UI control index, valid range: [0, GetItemCount())
    * @return The data element index associated with the UI control, valid range: [0, GetElementCount())
    */
    size_t GetDisplayItemElementIndex(size_t nItemIndex) const;

    /** Make the control visible within the viewport
    * @param [in] nElementIndex The element index, valid range: [0, GetElementCount())
    * @param [in] bToTop Whether to place it at the very top
    */
    void EnsureVisible(size_t nElementIndex, bool bToTop);

    /** Refresh the data in the specified range, keeping the data and UI display in sync, data range: [nStartElementIndex, nEndElementIndex]
    * @param [in] nStartElementIndex The start index of the data
    * @param [in] nEndElementIndex The end index of the data
    */
    void RefreshElements(size_t nStartElementIndex, size_t nEndElementIndex);

    /** Refresh the specified data, keeping the data and UI display in sync
    * @param [elementIndexs] The list contains element indices, valid range: [0, GetElementCount())
    */
    void RefreshElements(const std::vector<size_t>& elementIndexs);

    /** Refresh the UI, keeping the data and display in sync
    * @param [in] bSync true indicates a synchronous refresh, redrawing the current view immediately; false indicates an asynchronous refresh, where the refresh action redraws asynchronously, marks the dirty region, and leaves the system to redraw the UI
    */
    virtual void Refresh(bool bSync = false);

    /** Ensure the given rect region is visible
    * @param [in] rcItem The rect range of the visible region
    * @param [in] vVisibleType The additional visibility flag in the vertical direction
    * @param [in] hVisibleType The additional visibility flag in the horizontal direction
    */
    virtual void EnsureVisible(const UiRect& rcItem,
                               ListBoxVerVisible vVisibleType,
                               ListBoxHorVisible hVisibleType) override;

    /** Ensure the child item is visible
    * @param [in] iIndex The child item index, range: [0, GetItemCount())
    * @param [in] vVisibleType The additional visibility flag in the vertical direction
    * @param [in] hVisibleType The additional visibility flag in the horizontal direction
    * @return For a virtual list implementation, returns the new control index corresponding to the element, range: [0, GetItemCount())
    */
    virtual size_t EnsureVisible(size_t iIndex,
                                 ListBoxVerVisible vVisibleType = ListBoxVerVisible::kVisible,
                                 ListBoxHorVisible hVisibleType = ListBoxHorVisible::kVisible) override;

    /** Select the child item
    *  @param [in] iIndex The ID of the child item
    *  @param [in] bTakeFocus Whether to make the child item control the focus control
    *  @param [in] bTriggerEvent Whether to trigger the selection event; if true, a kEventSelect event is triggered
    *  @param [in] vkFlag The key flag, see the definition of enum VKFlag for valid values
    */
    virtual bool SelectItem(size_t iIndex, bool bTakeFocus,
                            bool bTriggerEvent, uint64_t vkFlag) override;

public:
    /** Listen for the child item selection event
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    * Parameter description:
    *   wParam: The index of the newly selected child item, valid range: [0, GetItemCount())
    *   lParam: The index of the previously selected child item, valid range: [0, GetItemCount()), may be the invalid value Box::InvalidIndex
    *   The associated data element index can be obtained via the GetDisplayItemElementIndex function
    */
    void AttachSelect(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelect, callback, callbackID); }

    /** Listen for the event when the selection changes
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachSelChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelChanged, callback, callbackID); }

    /** Listen for the event when the mouse enters the ListBoxItem control
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The associated child item index, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *    lParam: The data element index corresponding to the associated child item index, valid range: [0, GetElementCount()); if the value is Box::InvalidIndex, there is no associated data element
     */
    void AttachItemMouseEnter(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventItemMouseEnter, callback, callbackID); }

    /** Listen for the event when the mouse leaves the ListBoxItem control
     * @param [in] callback The callback function for event handling, see the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The associated child item index, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *    lParam: The data element index corresponding to the associated child item index, valid range: [0, GetElementCount()); if the value is Box::InvalidIndex, there is no associated data element
     */
    void AttachItemMouseLeave(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventItemMouseLeave, callback, callbackID); }

    /** Listen for the double-click event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The index of the double-clicked child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *    lParam: The data element index corresponding to the associated child item index, valid range: [0, GetElementCount()); if the value is Box::InvalidIndex, there is no associated data element
     */
    void AttachDoubleClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseDoubleClick, callback, callbackID); }

    /** Bind the mouse click handler
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    *  Parameter description:
    *    wParam: The index of the clicked child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
    *    lParam: The data element index corresponding to the associated child item index, valid range: [0, GetElementCount()); if the value is Box::InvalidIndex, there is no associated data element
    */
    void AttachClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventClick, callback, callbackID); }

    /** Bind the mouse right-click handler
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    *  Parameter description:
    *    wParam: The index of the right-clicked child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
    *    lParam: The data element index corresponding to the associated child item index, valid range: [0, GetElementCount()); if the value is Box::InvalidIndex, there is no associated data element
    */
    void AttachRClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventRClick, callback, callbackID); }

    /** Listen for the Return key event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The associated child item index, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *    lParam: The data element index corresponding to the associated child item index, valid range: [0, GetElementCount()); if the value is Box::InvalidIndex, there is no associated data element
     */
    void AttachReturn(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventReturn, callback, callbackID); }

    /** Listen for the keyboard key-down event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The associated child item index, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *    lParam: The data element index corresponding to the associated child item index, valid range: [0, GetElementCount()); if the value is Box::InvalidIndex, there is no associated data element
     */
    void AttachKeyDown(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventKeyDown, callback, callbackID); }

    /** Listen for the keyboard key-up event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The associated child item index, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *    lParam: The data element index corresponding to the associated child item index, valid range: [0, GetElementCount()); if the value is Box::InvalidIndex, there is no associated data element
     */
    void AttachKeyUp(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventKeyUp, callback, callbackID); }

    /** Listen for the data item UI element fill event (virtual list, filling element data)
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     * Parameter description:
     *   wParam: The associated child item index, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, it is not associated with any child item
     *   lParam: The data element index corresponding to the associated child item index, valid range: [0, GetElementCount()); if the value is Box::InvalidIndex, there is no associated data element
     *   pEventData: The UI control interface pointer of the associated child item, type: Control* pointer; the control is a subclass of IListBoxItem/ListBoxItem/ListBoxItemH/ListBoxItemV
     */
    void AttachElementFilled(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventElementFilled, callback, callbackID); }

public:
    /// Override the parent class interface to provide customized functionality
    virtual void SetScrollPos(UiSize64 szPos) override;
    virtual void SetPos(UiRect rc) override;
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;
    virtual void SendEventMsg(const EventArgs& msg) override;

protected:
    //Override the deletion functions; external calls are prohibited
    virtual bool RemoveItem(Control* pControl) override;
    virtual bool RemoveItemAt(size_t iIndex) override;
    virtual void RemoveAllItems() override;

    /** About to find the specified element (provides the virtual list with an opportunity to load data)
    * @param [in] nCurSel The index of the currently selected display control
    * @param [in] mode  The search mode
    * @param [in] nCount The n-th data of the control being searched
    * @param [out] nDestItemIndex Returns the target control index after loading, valid range: [0, GetItemCount())
    * @return Returns true if there was data loading behavior, false if no action was taken
    */
    virtual bool OnFindSelectable(size_t nCurSel, SelectableMode mode, 
                                  size_t nCount, size_t& nDestItemIndex) override;

    /** Query how many data items precede this data item (supports virtual list data)
    * @param [in] nCurSel The index of the currently selected display control
    */
    virtual size_t GetItemCountBefore(size_t nCurSel) override;

    /** Query how many data items follow this data item (supports virtual list data)
    * @param [in] nCurSel The index of the currently selected display control
    */
    virtual size_t GetItemCountAfter(size_t nCurSel) override;

    /** Determine whether a data element is selectable
    * @param [in] nElementIndex The element index, valid range: [0, GetElementCount())
    */
    virtual bool IsSelectableElement(size_t nElementIndex) const;

    /** Get the next selectable data element
    * @param [in] nElementIndex The element index, valid range: [0, GetElementCount())
    * @param [in] bForward true searches forward, false searches backward
    */
    virtual size_t FindSelectableElement(size_t nElementIndex, bool bForward) const;

    /** Sort the child items
     * @param [in] pfnCompare The custom sort function
     * @param [in] pCompareContext The user-defined data passed to the comparison function
     */
    virtual bool SortItems(PFNCompareFunc pfnCompare, void* pCompareContext) override;

    /** The child item selection state change event, used for state synchronization
    * @param [in] iIndex The ID of the child item, range: [0, GetItemCount())
    * @param [in] pListBoxItem The associated list item interface
    */
    virtual void OnItemSelectedChanged(size_t iIndex, IListBoxItem* pListBoxItem) override;

    /** A mouse frame selection operation was performed
    * @param [in] left The left value of the X coordinate of the frame selection
    * @param [in] right The right value of the X coordinate of the frame selection
    * @param [in] top The top value of the Y coordinate of the frame selection
    * @param [in] bottom The bottom value of the Y coordinate of the frame selection
    * @return Returns true if the selection changed, otherwise false
    */
    virtual bool OnFrameSelection(int64_t left, int64_t right, int64_t top, int64_t bottom) override;

    /** Select a child item (ListCtrl style)
    *  @param [in] iIndex The ID of the child item
    *  @param [in] bTakeFocus Whether to make the child item control the focus control
    *  @param [in] bTriggerEvent Whether to trigger the selection event; if true, a kEventSelect event is triggered
    *  @param [in] vkFlag The key flag, see the definition of enum VKFlag for valid values
    *  @return Returns true if the internal selection state changed, false if the internal state is unchanged
    */
    virtual bool ListCtrlSelectItem(size_t iIndex, bool bTakeFocus,
                                    bool bTriggerEvent, uint64_t vkFlag) override;

    /** Respond to the KeyDown message (implements ListCtrl-like shortcut key logic)
    * @return Returns true if handled successfully, false if the message was not handled
    */
    virtual bool OnListCtrlKeyDown(const EventArgs& msg) override;

protected:

    /** Set the virtual layout interface
    */
    void SetVirtualLayout(VirtualLayout* pVirtualLayout);

    /** Get the virtual layout interface
    */
    VirtualLayout* GetVirtualLayout() const;

public:
    struct RefreshData
    {
        size_t nItemIndex = Box::InvalidIndex;      //The index of the UI control
        Control* pControl = nullptr;                //The interface of the UI control
        size_t nElementIndex = Box::InvalidIndex;   //The index of the data element
    };
    typedef std::vector<RefreshData> RefreshDataList;

    /** A UI refresh operation was performed; the number of UI controls may change
    */
    virtual void OnRefreshElements(const RefreshDataList& /*refreshDataList*/) {}

    /** A UI control fill operation was performed, triggering the UI control fill event
    */
    virtual void OnFilledElements(const RefreshDataList& refreshDataList);

protected:
    /** Create a child item
    * @return Returns the pointer to the created child item
    */
    Control* CreateElement();

    /** Fill the specified data item: the caller is responsible for triggering the kEventElementFilled event
    * @param[in] pControl The data item control pointer
    * @param[in] nElementIndex The index ID of the data element, range: [0, GetElementCount())
    */
    void FillElementData(Control* pControl, size_t nElementIndex);

    /** Re-layout the child items
    * @param[in] bForce Whether to force a re-layout
    */
    void ReArrangeChild(bool bForce);

    /** The data content has changed; the displayed data needs to be reloaded in the event
    */
    void OnModelDataChanged(size_t nStartElementIndex, size_t nEndElementIndex);

    /** The number of data items has changed; the displayed data needs to be reloaded in the event
    */
    void OnModelCountChanged();

    /** Whether syncing from the UI state to the stored state is allowed
    */
    bool IsEnableUpdateProvider() const;

    /** Calculate the rect of an element
    */
    void CalcTileElementRectV(size_t nElemenetIndex, const UiSize& szItem,
                             int32_t nColumns, int32_t childMarginX, int32_t childMarginY,
                             int64_t& iLeft, int64_t& iTop, int64_t& iRight, int64_t& iBottom) const;

    /** Calculate the rect of an element
    */
    void CalcTileElementRectH(size_t nElemenetIndex, const UiSize& szItem,
                             int32_t nRows, int32_t childMarginX, int32_t childMarginY,
                             int64_t& iLeft, int64_t& iTop, int64_t& iRight, int64_t& iBottom) const;

    /** Set the index of the last selected item when the Shift key was not pressed, i.e., the UI control index (used for Shift-key selection logic)
    */
    virtual void SetLastNoShiftItem(size_t nLastNoShiftItem) override;

    /** Set the data index of the last selected item when the Shift key was not pressed (used for Shift-key selection logic)
    */
    void SetLastNoShiftIndex(size_t nLastNoShiftIndex);

    /** Set the data index of the last selected item when the Shift key was not pressed (used for Shift-key selection logic)
    */
    size_t GetLastNoShiftIndex() const;

    /** A new child item is added to the container; operations such as attaching events can be performed in this function
    * @param [in] pControl The interface of the newly added child item
    */
    virtual void OnListBoxItemAdded(Control* pControl) override;

    /** A child item is removed from the container; operations such as detaching the attached events can be performed in this function
    * @param [in] pControl The child item interface
    */
    virtual void OnListBoxItemRemoved(Control* pControl) override;

private:
    /** Horizontal layout: calculate the number of rows
    */
    int32_t CalcRows() const;

    /** Vertical layout: calculate the number of columns
    */
    int32_t CalcColumns() const;

    /** The function for sending events
    * @param [in] msg The event content
    * @param [in] bFromItem true indicates the event comes from a child control, false indicates it comes from this control itself
    * @param [in] bFireEventOnly If true, only dispatches the event without processing it
    */
    void VSendEvent(const EventArgs& msg, bool bFromItem, bool bFireEventOnly = false);

    /** The function for sending events (mouse enter and leave events)
    * @param [in] msg The event content
    */
    void VFireMouseEnterLeaveEvent(const EventArgs& msg);

private:
    /** The data provider interface, providing the displayed data
    */
    VirtualListBoxElement* m_pDataProvider;

    /** The virtual layout interface
    */
    VirtualLayout* m_pVirtualLayout;

    /** The data index of the last selected item when the Shift key was not pressed (not the UI control index; used for Shift-key selection logic)
    */
    size_t m_nLastNoShiftIndex;

    /** Whether syncing from the UI state to the stored state is allowed
    */
    bool m_bEnableUpdateProvider;
};

/** A virtual ListBox with horizontal layout
*/
class DUILIB_API VirtualHListBox : public VirtualListBox
{
public:
    explicit VirtualHListBox(Window* pWindow) :
        VirtualListBox(pWindow, new VirtualHLayout)
    {
        VirtualLayout* pVirtualLayout = dynamic_cast<VirtualHLayout*>(GetLayout());
        SetVirtualLayout(pVirtualLayout);
    }

    virtual DString GetType() const override { return DUI_CTR_VIRTUAL_HLISTBOX; }
};

/** A virtual ListBox with vertical layout
*/
class DUILIB_API VirtualVListBox : public VirtualListBox
{
public:
    explicit VirtualVListBox(Window* pWindow) :
        VirtualListBox(pWindow, new VirtualVLayout)
    {
        VirtualLayout* pVirtualLayout = dynamic_cast<VirtualVLayout*>(GetLayout());
        SetVirtualLayout(pVirtualLayout);
    }

    virtual DString GetType() const override { return DUI_CTR_VIRTUAL_VLISTBOX; }
};

/** A virtual ListBox with tile layout (horizontal layout)
*/
class DUILIB_API VirtualHTileListBox : public VirtualListBox
{
public:
    explicit VirtualHTileListBox(Window* pWindow) :
        VirtualListBox(pWindow, new VirtualHTileLayout)
    {
        VirtualLayout* pVirtualLayout = dynamic_cast<VirtualHTileLayout*>(GetLayout());
        SetVirtualLayout(pVirtualLayout);
    }

    virtual DString GetType() const override { return DUI_CTR_VIRTUAL_HTILE_LISTBOX; }
};

/** A virtual ListBox with tile layout (vertical layout)
*/
class DUILIB_API VirtualVTileListBox : public VirtualListBox
{
public:
    explicit VirtualVTileListBox(Window* pWindow) :
        VirtualListBox(pWindow, new VirtualVTileLayout)
    {
        VirtualLayout* pVirtualLayout = dynamic_cast<VirtualVTileLayout*>(GetLayout());
        SetVirtualLayout(pVirtualLayout);
    }

    virtual DString GetType() const override { return DUI_CTR_VIRTUAL_VTILE_LISTBOX; }
};

}

#endif //UI_BOX_VIRTUAL_LISTBOX_H_
