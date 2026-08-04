#ifndef UI_BOX_LISTBOX_H_
#define UI_BOX_LISTBOX_H_

#include "dui/Box/ListBoxItem.h"
#include "dui/Box/ScrollBox.h"
#include "dui/Control/Option.h"

namespace ui 
{

/** User-defined sorting function
@param [in] pControl1 The first control interface
@param [in] pControl2 The second control interface
@param [in] pCompareContext User-defined context data
@return < 0 control1 is less than control2
        = 0 control1 is equal to control2
        > 0 control1 is greater than control2
*/
typedef int (CALLBACK *PFNCompareFunc)(Control* pControl1, Control* pControl2, void* pCompareContext);

/** Helper class for mouse and keyboard operations
*/
class ListBoxHelper;

/** List container, used to display a set of data
*   By changing the layout, it forms four subclasses: HListBox/VListBox/HTileListBox/VTileListBox
*/
class DUI_API ListBox : public ScrollBox, public IListBoxOwner
{
    typedef ScrollBox BaseClass;
public:
    ListBox(Window* pWindow, Layout* pLayout = nullptr);
    virtual ~ListBox() override;

    ListBox(const ListBox& r) = delete;
    ListBox& operator=(const ListBox& r) = delete;

    /// Overrides the parent class methods to provide customized functionality. Please refer to the parent class declarations
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;
    virtual void HandleEvent(const EventArgs& msg) override;
    virtual void SendEventMsg(const EventArgs& msg) override;

    //Implementation of the IListBoxOwner interface
    virtual void StopScroll() override;
    virtual bool CanPaintSelectedColors(bool bHasStateImages) const override;
    virtual void EnsureSingleSelection() override;

public:
    /** Whether multi-select is supported
    */
    virtual bool IsMultiSelect() const override;

    /** Sets whether multi-select is supported, called by the UI layer to keep it consistent with the UI control
    * @return bMultiSelect true means multi-select is supported, false means not supported
    */
    virtual void SetMultiSelect(bool bMultiSelect);

    /** Gets the index of the current selection (valid only in single-select mode)
    @return The index of the selection, or Box::InvalidIndex if there is no valid index
    */
    virtual size_t GetCurSel() const override;

    /** Sets the index of the current selection (valid only in single-select mode)
    * @param [in] iIndex The ID of the child item
    */
    virtual void SetCurSel(size_t iIndex) override;

    /** Gets whether the selected item changes with the mouse wheel scrolling (valid only in single-select mode)
    * @return true if the selection follows the scroll bar, otherwise false
    */
    bool IsScrollSelect() const;

    /** Sets whether the selected item changes with the mouse wheel scrolling (valid only in single-select mode)
     * @param[in] bScrollSelect true to change the selected item with the scroll bar, false to not follow
     */
    void SetScrollSelect(bool bScrollSelect);

    /** After removing a child item, whether to automatically select the next item if the removed item was selected (valid only in single-select mode)
     */
    bool IsSelectNextWhenActiveRemoved() const;

    /** After removing a child item, automatically selects the next item if the removed item was selected (valid only in single-select mode)
     * @param [in] bSelectNextItem true to automatically select the next item, false to not
     */
    void SetSelectNextWhenActiveRemoved(bool bSelectNextItem);

public:
    /** Finds the index of the next selectable control
     * @param[in] iIndex The index from which to start searching
     * @param[in] bForward true to search forward (increasing), false to search backward (decreasing)
     * @return The index of the next selectable control, or Box::InvalidIndex if there is no selectable control
     */
    virtual size_t FindSelectable(size_t iIndex, bool bForward = true) const;

    /** Gets the currently selected child items (supports both single-select and multi-select), excluding child items that are !IsVisible() and !IsEnabled()
    * @param [out] selectedIndexs Returns the IDs of the selected child items, range: [0, GetItemCount())
    */
    virtual void GetSelectedItems(std::vector<size_t>& selectedIndexs) const;

    /** Selects a child item
    * @param [in] iIndex The ID of the child item
    * @param [in] bTakeFocus Whether to make the child item control the focused control
    * @param [in] bTriggerEvent Whether to trigger the selection event; if true, a kEventSelect event will be triggered
    * @param [in] vkFlag Key flags, see the definition of enum VKFlag for the value range
    * @return true if the internal selection state changed, false if it did not
    */
    virtual bool SelectItem(size_t iIndex, bool bTakeFocus, 
                            bool bTriggerEvent, uint64_t vkFlag = 0) override;

    /** Unselects a child item
    * @param [in] iIndex The ID of the child item
    * @param [in] bTriggerEvent Whether to trigger the selection event; if true, a kEventUnSelect event will be triggered
    * @return true if the internal selection state changed, false if it did not
    */
    virtual bool UnSelectItem(size_t iIndex, bool bTriggerEvent) override;

    /** Selects the previous item and ensures the selected item is visible
    * @return The index of the selected child item, valid range: [0, GetItemCount())
    */
    virtual size_t SelectItemPrevious(bool bTakeFocus, bool bTriggerEvent);

    /** Selects the next item and ensures the selected item is visible
    * @return The index of the selected child item, valid range: [0, GetItemCount())
    */
    virtual size_t SelectItemNext(bool bTakeFocus, bool bTriggerEvent);

    /** Scrolls up one page, selects an item, and ensures the selected item is visible
    * @return The index of the selected child item, valid range: [0, GetItemCount())
    */
    virtual size_t SelectItemPageUp(bool bTakeFocus, bool bTriggerEvent);

    /** Scrolls down one page, selects an item, and ensures the selected item is visible
    * @return The index of the selected child item, valid range: [0, GetItemCount())
    */
    virtual size_t SelectItemPageDown(bool bTakeFocus, bool bTriggerEvent);

    /** Scrolls to the first page, selects the first item, and ensures the selected item is visible
    * @return The index of the selected child item, valid range: [0, GetItemCount())
    */
    virtual size_t SelectItemHome(bool bTakeFocus, bool bTriggerEvent);

    /** Scrolls to the last page, selects the last item, and ensures the selected item is visible
    * @return The index of the selected child item, valid range: [0, GetItemCount())
    */
    virtual size_t SelectItemEnd(bool bTakeFocus, bool bTriggerEvent);

public:    
    /** Ensures the rectangular area is visible
    * @param [in] rcItem The rect range of the visible area
    * @param [in] vVisibleType Additional flag for vertical visibility
    * @param [in] hVisibleType Additional flag for horizontal visibility
    */
    virtual void EnsureVisible(const UiRect& rcItem,
                               ListBoxVerVisible vVisibleType,
                               ListBoxHorVisible hVisibleType) override;

    /** Ensures a child item is visible
    * @param [in] iIndex The child item index, range: [0, GetItemCount())
    * @param [in] vVisibleType Additional flag for vertical visibility
    * @param [in] hVisibleType Additional flag for horizontal visibility
    * @return For a virtual table implementation, returns the new control index of the element, range: [0, GetItemCount())
    */
    virtual size_t EnsureVisible(size_t iIndex,
                                 ListBoxVerVisible vVisibleType = ListBoxVerVisible::kVisible,
                                 ListBoxHorVisible hVisibleType = ListBoxHorVisible::kVisible);

    /** Scrolls to the position of the specified child item
     * @param [in] iIndex The ID of the child item, range: [0, GetItemCount())
     */
    virtual bool ScrollItemToTop(size_t iIndex);

    /** Scrolls to the position of the specified child item
     * @param [in] itemName The name of the child item (i.e., Control::GetName())
     */
    virtual bool ScrollItemToTop(const DString& itemName);

    /** Gets the first child item within the current rect (Control::GetPos())
     */
    virtual Control* GetTopItem() const;

public:
    /** Sets the position index of a child item
     * @param [in] pControl Pointer to the child item
     * @param [in] iIndex The index, range: [0, GetItemCount())
     */
    virtual bool SetItemIndex(Control* pControl, size_t iIndex) override;

    /** Appends a child item to the end
     * @param [in] pControl Pointer to the child item
     */
    virtual bool AddItem(Control* pControl) override;

    /** Inserts a child item after the specified position
     * @param [in] pControl Pointer to the child item
     * @param[in] iIndex The position index at which to insert, range: [0, GetItemCount())
     */
    virtual bool AddItemAt(Control* pControl, size_t  iIndex) override;

    /** Removes a child item by its pointer
     * @param [in] pControl Pointer to the child item
     */
    virtual bool RemoveItem(Control* pControl) override;

    /** Removes a child item by its index
     * @param [in] iIndex The child item index, range: [0, GetItemCount())
     */
    virtual bool RemoveItemAt(size_t iIndex) override;

    /** Removes all child items
     */
    virtual void RemoveAllItems() override;

public:
    /** Sets whether the mouse frame selection feature is supported
    */
    void SetEnableFrameSelection(bool bEnable);

    /** Gets whether the mouse frame selection feature is supported
    */
    bool IsEnableFrameSelection() const;

    /** Sets the fill color of the mouse frame selection
    */
    void SetFrameSelectionColor(const DString& frameSelectionColor);

    /** Gets the fill color of the mouse frame selection
    */
    DString GetFrameSelectionColor() const;

    /** Sets the Alpha value of the mouse frame selection fill color
    */
    void SetframeSelectionAlpha(uint8_t frameSelectionAlpha);

    /** Gets the Alpha value of the mouse frame selection fill color
    */
    uint8_t GetFrameSelectionAlpha() const;

    /** Sets the border color of the mouse frame selection
    */
    void SetFrameSelectionBorderColor(const DString& frameSelectionBorderColor);

    /** Gets the border color of the mouse frame selection
    */
    DString GetFrameSelectionBorderColor() const;

    /** Sets the border size of the mouse frame selection
    * @param [in] nBorderSize The border size (not DPI scaled)
    */
    void SetFrameSelectionBorderSize(int32_t nBorderSize);

    /** Gets the border size of the mouse frame selection (not DPI scaled)
    */
    int32_t GetFrameSelectionBorderSize() const;

    /** Sets the top coordinate of a normal list item (not Header, not pinned to top) (currently used by ListCtrl)
    */
    void SetNormalItemTop(int32_t nNormalItemTop);

    /** Gets the top coordinate of a normal list item (not Header, not pinned to top) (currently used by ListCtrl)
    */
    int32_t GetNormalItemTop() const;

    /** Sets whether the selection is canceled when the mouse clicks on a blank area (only valid when the mouse frame selection feature is enabled)
    */
    void SetSelectNoneWhenClickBlank(bool bSelectNoneWhenClickBlank);

    /** Gets whether the selection is canceled when the mouse clicks on a blank area
    */
    bool IsSelectNoneWhenClickBlank() const;

    /** Sets the selection mode: whether to use ListCtrl style (only valid in multi-select mode, i.e., similar to how files are operated on in Windows Explorer)
        Ctrl: only selects the clicked child item; clicking again cancels the selection
        Shift: selects all child items between two clicks
    */
    void SetSelectLikeListCtrl(bool bSelectLikeListCtrl);

    /** Gets the selection mode: whether ListCtrl style is used
    */
    bool IsSelectLikeListCtrl() const;

public:
    /** Selects all items, and updates the UI display as needed
    * @return true if the data changed, otherwise false
    */
    virtual bool SetSelectAll();

    /** Cancels all selections, and updates the UI display as needed
    * @return true if the data changed, otherwise false
    */
    virtual bool SetSelectNone();

    /** The selection state has changed (triggered by select all / select none)
    */
    virtual void OnSelectStatusChanged();

public:
    /** Sorts the child items
     * @param [in] pfnCompare Custom sort function
     * @param [in] pCompareContext User-defined data passed to the comparison function
     */
    virtual bool SortItems(PFNCompareFunc pfnCompare, void* pCompareContext);

    /** Listens for the event of selecting a child item
    * @param [in] callback The callback function invoked when a child item is selected
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    * Parameter description:
    *   wParam: The index of the newly selected child item
    *   lParam: The index of the previously selected child item, possibly the invalid value Box::InvalidIndex
    */
    void AttachSelect(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelect, callback, callbackID); }

    /** Listens for the event of unselecting a child item
    * @param [in] callback The callback function invoked when a child item is unselected
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    * Parameter description:
    *   wParam: The index of the unselected child item
    *   lParam: The invalid value Box::InvalidIndex
    */
    void AttachUnSelect(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventUnSelect, callback, callbackID); }

    /** Listens for the event of the selection changing
     * @param [in] callback The callback function invoked when a child item is selected
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachSelChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelChanged, callback, callbackID); }

    /** Listens for the event of the mouse entering a ListBoxItem control
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The associated child item index, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, no child item is associated
     */
    void AttachItemMouseEnter(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventItemMouseEnter, callback, callbackID); }

    /** Listens for the event of the mouse leaving a ListBoxItem control
     * @param [in] callback The callback function for event handling, please refer to the EventCallback declaration
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The associated child item index, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, no child item is associated
     */
    void AttachItemMouseLeave(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventItemMouseLeave, callback, callbackID); }

    /** Listens for the double-click event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The index of the double-clicked child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, no child item is associated
     */
    void AttachDoubleClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventMouseDoubleClick, callback, callbackID); }

    /** Binds the mouse click handler
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    *  Parameter description:
    *    wParam: The index of the clicked child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, no child item is associated
    */
    void AttachClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventClick, callback, callbackID); }

    /** Binds the mouse right-click handler
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    *  Parameter description:
    *    wParam: The index of the clicked child item, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, no child item is associated
    */
    void AttachRClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventRClick, callback, callbackID); }

    /** Listens for the Enter key event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The associated child item index, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, no child item is associated
     */
    void AttachReturn(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventReturn, callback, callbackID); }

    /** Listens for the keyboard key-down event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The associated child item index, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, no child item is associated
     */
    void AttachKeyDown(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventKeyDown, callback, callbackID); }

    /** Listens for the keyboard key-up event
     * @param [in] callback The callback function to bind
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     *  Parameter description:
     *    wParam: The associated child item index, valid range: [0, GetItemCount()); if the value is Box::InvalidIndex, no child item is associated
     */
    void AttachKeyUp(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventKeyUp, callback, callbackID); }

protected:
    /** When switching from multi-select to single-select mode, ensures that the list has only one selected item
    * @return true if there was a change, otherwise false
    */
    virtual bool OnSwitchToSingleSelect();

    /** Synchronizes the selection state of the current selected item
    * @return true if there was a change, otherwise false
    */
    bool UpdateCurSelItemSelectStatus();

    /** The search mode
    */
    enum class SelectableMode
    {
        kForward,    //Searches forward from the current element nCurSel for the nCount-th data item
        kBackward,    //Searches backward from the current element nCurSel for the nCount-th data item
        kSelect,    //Locates the currently selected data (in virtual table mode, the selected data may not be in the visible area)
        kHome,        //Locates the first data item
        kEnd        //Locates the last data item
    };
    /** About to search for the specified element (provides the virtual table an opportunity to load data)
    * @param [in] nCurSel The index of the currently selected display control
    * @param [in] mode  The search mode
    * @param [in] nCount The nCount-th data item of the searched control
    * @param [out] nDestItemIndex Returns the target control index after loading, valid range: [0, GetItemCount())
    * @return true if data was loaded, false if no action was taken
    */
    virtual bool OnFindSelectable(size_t nCurSel, SelectableMode mode, 
                                  size_t nCount, size_t& nDestItemIndex);

    /** Queries how many data items precede this data item (supports virtual table data)
    * @param [in] nCurSel The index of the currently selected display control
    */
    virtual size_t GetItemCountBefore(size_t nCurSel);

    /** Queries how many data items follow this data item (supports virtual table data)
    * @param [in] nCurSel The index of the currently selected display control
    */
    virtual size_t GetItemCountAfter(size_t nCurSel);

    /** The selection state change event of a child item, used for state synchronization
    * @param [in] iIndex The ID of the child item, range: [0, GetItemCount())
    * @param [in] pListBoxItem The associated list item interface
    */
    virtual void OnItemSelectedChanged(size_t iIndex, IListBoxItem* pListBoxItem) override;

    /** The checked state change event of a child item, used for state synchronization
    * @param [in] iIndex The ID of the child item, range: [0, GetItemCount())
    * @param [in] pListBoxItem The associated list item interface
    */
    virtual void OnItemCheckedChanged(size_t iIndex, IListBoxItem* pListBoxItem) override;

    /** Handles the KeyDown message (implements shortcut key handling for the ListBox standard mode)
    * @return true if the message was handled successfully, false if it was not handled
    */
    virtual bool OnListBoxKeyDown(const EventArgs& msg);

    /** Handles the KeyDown message (implements ListCtrl-like shortcut key logic: Shift + arrow keys)
    * @return true if the message was handled successfully, false if it was not handled
    */
    virtual bool OnListCtrlKeyDown(const EventArgs& msg);

    /** Handles the MouseWheel message
    @return true if the message was handled successfully, false if it was not handled
    */
    bool OnListBoxMouseWheel(const EventArgs& msg);

protected:
    /**
     * @brief Default comparison method for child items
     * @param[in] pvlocale Stores the List pointer
     * @param[in] item1 Child item 1
     * @param[in] item2 Child item 2
     * @return The comparison result
     */
    static int ItemComareFuncWindows(void* pvlocale, const void* item1, const void* item2);
    static int ItemComareFuncLinux(const void* item1, const void* item2, void* pvlocale);
    static int ItemComareFuncMacOS(void* context, const void* item1, const void* item2);

    int ItemComareFunc(const void* item1, const void* item2);

    /** Selects a child item (single-select)
    *  @param [in] iIndex The ID of the child item, range: [0, GetItemCount())
    *  @param [in] bTakeFocus Whether to make the child item control the focused control
    *  @param [in] bTriggerEvent Whether to trigger the selection event; if true, a kEventSelect event will be triggered
    *  @return true if the internal selection state changed, false if it did not
    */
    bool SelectItemSingle(size_t iIndex, bool bTakeFocus, bool bTriggerEvent);

    /** Selects a child item (multi-select)
    *  @param [in] iIndex The ID of the child item, range: [0, GetItemCount())
    *  @param [in] bTakeFocus Whether to make the child item control the focused control
    *  @param [in] bTriggerEvent Whether to trigger the selection event; if true, a kEventSelect event will be triggered
    *  @return true if the internal selection state changed, false if it did not
    */
    bool SelectItemMulti(size_t iIndex, bool bTakeFocus, bool bTriggerEvent);

    /** Selects a child item; after selection, makes the child item control the focused control and triggers a kEventSelect event
    *   It is a secondary wrapper function, equivalent to: SelectItem(iIndex, true, true);
    *  @param [in] iIndex The ID of the child item, range: [0, GetItemCount())
    *  @return true if the internal selection state changed, false if it did not
    */
    bool SelectItem(size_t iIndex);

    /** Selects a child item page by page
    */
    size_t SelectItemPage(bool bTakeFocus, bool bTriggerEvent, bool bForward, int32_t nDeltaValue);

    /** Selects a child item by skipping nCount child items
    */
    size_t SelectItemCountN(bool bTakeFocus, bool bTriggerEvent, bool bForward, size_t nCount);

    /** Gets the current layout direction
    * @return true if it is a horizontal layout, otherwise a vertical layout
    */
    bool IsHorizontalLayout() const;

    /** Determines whether the current layout has a horizontal scroll bar
    */
    bool IsHorizontalScrollBar() const;

    /** Ensures the selected item is visible
    * @param [in] itemIndex The ID of the child item, range: [0, GetItemCount())
    * @param [in] bTakeFocus Whether to make the child item control the focused control
    * return Returns the new control index (the index may change in the case of a virtual table), range: [0, GetItemCount())
    */
    size_t SelectEnsureVisible(size_t itemIndex, bool bTakeFocus);

    /** Determines whether a child item is selectable
    * @param [in] itemIndex The ID of the child item, range: [0, GetItemCount())
    */
    bool IsSelectableItem(size_t itemIndex) const;

    /** Determines whether a list item is in the selected state (UI control)
    * @param [in] nIndex The child item index of the UI control
    * @return true if it is in the selected state, false if not
    */
    bool IsItemSelected(size_t nIndex) const;

    /** Calculates how many child items are displayed on the current page
    * @param [in] bIsHorizontal Whether the current layout is a horizontal layout
    * @param [out] nColumns Returns the number of columns
    * @param [out] nRows Returns the number of rows
    * @return The number of records displayed in the visible area
    */
    virtual size_t GetDisplayItemCount(bool bIsHorizontal, size_t& nColumns, size_t& nRows) const;

    /** Horizontal tile layout: calculates the number of displayable rows
    * @return Return value: >= 1
    */
    int32_t CalcHTileRows(HTileLayout* pHTileLayout) const;

    /** Horizontal tile layout: calculates the number of displayable columns
    * @return Return value: >= 1
    */
    int32_t CalcHTileColumns(HTileLayout* pHTileLayout) const;

    /** Vertical tile layout: calculates the number of columns
    * @return Return value: >= 1
    */
    int32_t CalcVTileColumns(VTileLayout* pVTileLayout) const;

    /** Vertical tile layout: calculates the number of displayable rows
    * @return Return value: >= 1
    */
    int32_t CalcVTileRows(VTileLayout* pVTileLayout) const;

    /** Sets the index of the last selected item when the Shift key was not pressed, i.e., the UI control index (used for the Shift key selection logic)
    */
    virtual void SetLastNoShiftItem(size_t nLastNoShiftItem);

    /** Sets the index of the last selected item when the Shift key was not pressed (used for the Shift key selection logic)
    */
    size_t GetLastNoShiftItem() const;

protected:
    //Mouse messages (returning true means the message has been handled; returning false means it has not been handled and needs to be forwarded to the parent control)
    virtual bool ButtonDown(const EventArgs& msg) override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual bool RButtonDown(const EventArgs& msg) override;
    virtual bool RButtonUp(const EventArgs& msg) override;
    virtual bool MouseMove(const EventArgs& msg) override;
    virtual bool OnWindowKillFocus(const EventArgs& msg) override;//The window that owns the control has lost focus

    /** Draws the child controls
    */
    virtual void PaintChild(IRender* pRender, const UiRect& rcPaint) override;

    /** Draws the border and fill color of the mouse frame selection
    */
    virtual void PaintFrameSelection(IRender* pRender);

    /** The child item of a list item receives a mouse event
    * @return true means the message was intercepted and the child item will not process it; false means the child item continues to process the message
    */
    virtual bool OnListBoxItemMouseEvent(const EventArgs& msg) override;

    /** The child item of a list item receives the window lost focus event
    */
    virtual void OnListBoxItemWindowKillFocus() override;

    /** Gets the scroll amount of the scroll view (for the mouse frame selection feature)
    */
    virtual void GetScrollDeltaValue(int32_t& nHScrollValue, int32_t& nVScrollValue) const;

    /** A mouse frame selection operation was performed (for the mouse frame selection feature; the coordinate values are relative to the top-left corner of the ListBox)
    * @param [in] left The left value of the X coordinate of the frame selection
    * @param [in] right The right value of the X coordinate of the frame selection
    * @param [in] top The top value of the Y coordinate of the frame selection
    * @param [in] bottom The bottom value of the Y coordinate of the frame selection
    * @return true if the selection changed, otherwise false
    */
    virtual bool OnFrameSelection(int64_t left, int64_t right, int64_t top, int64_t bottom);

    /** Selects a child item (ListCtrl style)
    *  @param [in] iIndex The ID of the child item
    *  @param [in] bTakeFocus Whether to make the child item control the focused control
    *  @param [in] bTriggerEvent Whether to trigger the selection event; if true, a kEventSelect event will be triggered
    *  @param [in] vkFlag Key flags, see the definition of enum VKFlag for the value range
    *  @return true if the internal selection state changed, false if it did not
    */
    virtual bool ListCtrlSelectItem(size_t iIndex, bool bTakeFocus,
                                    bool bTriggerEvent, uint64_t vkFlag);

    /** The left mouse button was clicked on a blank area of the view (for the mouse frame selection feature)
    * @return true if the selection changed, in which case a kEventSelChanged event will be triggered; otherwise false
    */
    virtual bool OnLButtonClickedBlank();

    /** The right mouse button was clicked on a blank area of the view (for the mouse frame selection feature)
    * @return true if the selection changed, in which case a kEventSelChanged event will be triggered; otherwise false
    */
    virtual bool OnRButtonClickedBlank();

    /** A new child item is added to the container; events and other operations can be attached in this function
    * @param [in] pControl The interface of the newly added child item
    */
    virtual void OnListBoxItemAdded(Control* pControl);

    /** A child item is removed from the container; attached events and other operations can be detached in this function
    * @param [in] pControl The interface of the child item
    */
    virtual void OnListBoxItemRemoved(Control* pControl);

private:
    /** Horizontal layout: calculates the number of rows
    */
    int32_t CalcRows() const;

    /** Vertical layout: calculates the number of columns
    */
    int32_t CalcColumns() const;

    /** Function for sending events
    * @param [in] msg The event content
    * @param [in] bFromItem true if the event comes from a child control, false if from itself
    * @param [in] bFireEventOnly If true, only dispatches the event without processing it
    */
    void ListBoxSendEvent(const EventArgs& msg, bool bFromItem, bool bFireEventOnly = false);

    /** Function for sending events (mouse enter and leave events)
    * @param [in] msg The event content
    */
    void ListBoxFireMouseEnterLeaveEvent(const EventArgs& msg);

private:
    //Helper type, can access all data
    friend class ListBoxHelper;

    //ID of the currently selected child item; in multi-select mode, points to the last selected item
    size_t m_iCurSel;

    //UI control index of the last selected item when the Shift key was not pressed (used for the Shift key selection logic)
    size_t m_nLastNoShiftItem;

    //User-defined sort comparison function
    PFNCompareFunc m_pCompareFunc;

    //Context data for the user-defined sort comparison function
    void* m_pCompareContext;

    //Helper class for mouse and keyboard operations
    std::unique_ptr<ListBoxHelper> m_pHelper;

    //Whether to show the selection background color in multi-select mode: 0 - default rule; 1 - show background color; 2: do not show background color
    uint8_t m_uPaintSelectedColors;

    //Whether the selected item changes with scrolling
    bool m_bScrollSelect;

    //Automatically selects the next item after removing a child item
    bool m_bSelectNextWhenActiveRemoved;

    //Whether multi-select is allowed (single-select by default)
    bool m_bMultiSelect;

    //Selection mode: ListCtrl style (only valid in multi-select mode)
    //Ctrl: only selects the clicked child item; clicking again cancels the selection
    //Shift: selects all child items between two clicks
    bool m_bSelectLikeListCtrl;

    //Whether the selection is canceled when the mouse clicks on a blank area (only valid when the mouse frame selection feature is enabled)
    bool m_bSelectNoneWhenClickBlank;
};

/////////////////////////////////////////////////////////////////////////////////////
//

/** ListBox with a horizontal layout
*/
class DUI_API HListBox : public ListBox
{
public:
    explicit HListBox(Window* pWindow) :
        ListBox(pWindow, new HLayout)
    {
    }

    virtual DString GetType() const override { return DUI_CTR_HLISTBOX; }
};

/** ListBox with a vertical layout
*/
class DUI_API VListBox : public ListBox
{
public:
    explicit VListBox(Window* pWindow) :
        ListBox(pWindow, new VLayout)
    {
    }

    virtual DString GetType() const override { return DUI_CTR_VLISTBOX; }
};

/** ListBox with a tile layout (horizontal layout)
*/
class DUI_API HTileListBox : public ListBox
{
public:
    explicit HTileListBox(Window* pWindow) :
        ListBox(pWindow, new HTileLayout)
    {
    }

    virtual DString GetType() const override { return DUI_CTR_HTILE_LISTBOX; }
};

/** ListBox with a tile layout (vertical layout)
*/
class DUI_API VTileListBox : public ListBox
{
public:
    explicit VTileListBox(Window* pWindow) :
        ListBox(pWindow, new VTileLayout)
    {
    }

    virtual DString GetType() const override { return DUI_CTR_VTILE_LISTBOX; }
};

} // namespace ui

#endif // UI_BOX_LISTBOX_H_
