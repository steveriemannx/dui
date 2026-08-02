#ifndef UI_BOX_LISTBOX_ITEM_H_
#define UI_BOX_LISTBOX_ITEM_H_

#include "duilib/Box/VBox.h"
#include "duilib/Box/HBox.h"
#include "duilib/Control/Option.h"
#include "duilib/Core/Keyboard.h"

namespace ui 
{
class IListBoxItem;

/** Additional flags for ensuring visibility (vertical direction, vertical scroll bar)
*/
enum class ListBoxVerVisible
{
    kVisible,           //Ensures it is displayed in the visible area
    kVisibleAtTop,      //Ensures it is displayed at the top of the visible area
    kVisibleAtCenter,   //Ensures it is displayed in the middle of the visible area
    kVisibleAtBottom,   //Ensures it is displayed at the bottom of the visible area
};

/** Additional flags for ensuring visibility (horizontal direction, horizontal scroll bar)
*/
enum class ListBoxHorVisible
{
    kVisible,           //Ensures it is displayed in the visible area
    kVisibleAtLeft,     //Ensures it is displayed on the left side of the visible area
    kVisibleAtCenter,   //Ensures it is displayed in the middle of the visible area
    kVisibleAtRight,    //Ensures it is displayed on the right side of the visible area
};

/** ListBox owner interface
*/
class DUILIB_API IListBoxOwner
{
public:
    /** Triggers an event
    */
    virtual void SendEventMsg(const EventArgs& msg) = 0;

    /** The child item of a list item receives a mouse event
    * @return true means the message was intercepted and the child item will not process it; false means the child item continues to process the message
    */
    virtual bool OnListBoxItemMouseEvent(const EventArgs& msg) = 0;

    /** The child item of a list item receives the window lost focus event
    */
    virtual void OnListBoxItemWindowKillFocus() = 0;

    /** Gets the index of the current selection (returns Box::InvalidIndex if there is no valid index)
    */
    virtual size_t GetCurSel() const = 0;

    /** Sets the index of the current selection
    * @param [in] iIndex The ID of the child item
    */
    virtual void SetCurSel(size_t iIndex) = 0;

    /** Selects a child item
    * @param [in] iIndex The ID of the child item
    * @param [in] bTakeFocus Whether to make the child item control the focused control
    * @param [in] bTriggerEvent Whether to trigger the selection event; if true, a kEventSelect event will be triggered
    * @param [in] vkFlag Key flags, see the definition of enum VKFlag for the value range
    * @return true if the internal selection state changed, false if it did not
    */
    virtual bool SelectItem(size_t iIndex, bool bTakeFocus,
                            bool bTriggerEvent, uint64_t vkFlag = 0) = 0;

    /** Unselects a child item
    * @param [in] iIndex The ID of the child item
    * @param [in] bTriggerEvent Whether to trigger the selection event; if true, a kEventUnSelect event will be triggered
    * @return true if the internal selection state changed, false if it did not
    */
    virtual bool UnSelectItem(size_t iIndex, bool bTriggerEvent) = 0;

    /** The selection state change event of a child item, used for state synchronization
    * @param [in] iIndex The ID of the child item, range: [0, GetItemCount())
    * @param [in] pListBoxItem The associated list item interface
    */
    virtual void OnItemSelectedChanged(size_t iIndex, IListBoxItem* pListBoxItem) = 0;

    /** The checked state change event of a child item, used for state synchronization
    * @param [in] iIndex The ID of the child item, range: [0, GetItemCount())
    * @param [in] pListBoxItem The associated list item interface
    */
    virtual void OnItemCheckedChanged(size_t iIndex, IListBoxItem* pListBoxItem) = 0;

    /** Ensures the rectangular area is visible
    * @param [in] rcItem The rect range of the visible area
    * @param [in] vVisibleType Additional flag for vertical visibility
    * @param [in] hVisibleType Additional flag for horizontal visibility
    */
    virtual void EnsureVisible(const UiRect& rcItem,
                               ListBoxVerVisible vVisibleType,
                               ListBoxHorVisible hVisibleType) = 0;

    /** Stops the scroll bar animation
    */
    virtual void StopScroll() = 0;

    /** Whether to draw the background color in the selected state, provided as an optional virtual function
        (For example, when ListBox/TreeView nodes are multi-selected, since there are check options, the selected state background color does not need to be drawn)
        @param [in] bHasStateImages Whether the current list item has a CheckBox check option
    */
    virtual bool CanPaintSelectedColors(bool bHasStateImages) const = 0;

    /** Whether multi-select is allowed
    */
    virtual bool IsMultiSelect() const = 0;

    /** Event after a child item is selected, used to ensure only one item is selected in single-select mode
    */
    virtual void EnsureSingleSelection() = 0;
};

/** ListBoxItem interface
*/
class DUILIB_API IListBoxItem
{
public:
    virtual ~IListBoxItem() = default;

    /** Calls the selection function of the Option class, only updating the UI selection state
    * @param [in] bSelected true means the selected state, false means the unselected state
     * @param [in] bTriggerEvent Whether to send the state change event; true to send, otherwise false.
    */
    virtual void OptionSelected(bool bSelect, bool bTriggerEvent) = 0;

    /** Sets the selection state, handling internal state synchronization but not triggering any events (suitable for state synchronization)
    */
    virtual void SetItemSelected(bool bSelected) = 0;

    /** Determines whether it is currently in the selected state
     * @return true if in the selected state, otherwise false
     */
    virtual bool IsSelected() const = 0;

    /** Gets the parent container
     */
    virtual IListBoxOwner* GetOwner() = 0;

    /** Sets the parent container
     * @param[in] pOwner Pointer to the parent container
     */
    virtual void SetOwner(IListBoxOwner* pOwner) = 0;

    /** Gets the container index, range: [0, GetItemCount())
     */
    virtual size_t GetListBoxIndex() const = 0;

    /** Sets the container child item index
     * @param[in] iIndex The index, range: [0, GetItemCount())
     */
    virtual void SetListBoxIndex(size_t iIndex) = 0;

    /** Gets the virtual table data element index, used to support the virtual table, range: [0, GetElementCount())
     */
    virtual size_t GetElementIndex() const = 0;

    /** Sets the virtual table data element index
     * @param[in] iIndex The index, used to support the virtual table, range: [0, GetElementCount())
     */
    virtual void SetElementIndex(size_t iIndex) = 0;
};

/** Data child item of a list item, used to display data in a list
*/
template<typename InheritType = Box>
class ListBoxItemTemplate:
    public OptionTemplate<InheritType>,
    public IListBoxItem
{
    typedef OptionTemplate<InheritType> BaseClass;
public:
    explicit ListBoxItemTemplate(Window* pWindow);

    /// Overrides the parent class methods to provide customized functionality. Please refer to the parent class declarations
    virtual DString GetType() const override;    
    virtual void HandleEvent(const EventArgs& msg) override;

    /** Whether to draw the background color in the selected state, provided as an optional virtual function
       (For example, when ListBox/TreeView nodes are multi-selected, since there are check options, the selected state background color does not need to be drawn)
    */
    virtual bool CanPaintSelectedColors() const override;

    /** Determines whether the control type is selectable
     * @return true
     */
    virtual bool IsSelectableType() const override;

    /** Sets whether the control is in the selected state
     * @param [in] bSelected true means the selected state, false means the unselected state
     * @param [in] bTriggerEvent Whether to send the state change event; true to send, otherwise false
     * @param [in] vkFlag Key flags, see the definition of enum VKFlag for the value range
     */
    virtual void Selected(bool bSelect, bool bTriggerEvent, uint64_t vkFlag) override;

    /** Calls the selection function of the Option class, only updating the UI selection state
    * @param [in] bSelected true means the selected state, false means the unselected state
     * @param [in] bTriggerEvent Whether to send the state change event; true to send, otherwise false.
    */
    virtual void OptionSelected(bool bSelect, bool bTriggerEvent) override;

    /** Sets the selection state, handling internal state synchronization but not triggering any events (suitable for state synchronization)
    */
    virtual void SetItemSelected(bool bSelected) override;

    /** Determines whether it is currently in the selected state
     * @return true if in the selected state, otherwise false
     */
    virtual bool IsSelected() const override;

    /** Gets the parent container
     */
    virtual IListBoxOwner* GetOwner() override;

    /** Sets the parent container
     * @param[in] pOwner Pointer to the parent container
     */
    virtual void SetOwner(IListBoxOwner* pOwner) override;

    /** Gets the container index, range: [0, GetItemCount())
     */
    virtual size_t GetListBoxIndex() const override;

    /** Sets the container child item index
     * @param[in] iIndex The index, range: [0, GetItemCount())
     */
    virtual void SetListBoxIndex(size_t iIndex) override;

    /** Gets the virtual table data element index, used to support the virtual table, range: [0, GetElementCount())
     */
    virtual size_t GetElementIndex() const override;

    /** Sets the virtual table data element index
     * @param[in] iIndex The index, used to support the virtual table, range: [0, GetElementCount())
     */
    virtual void SetElementIndex(size_t iIndex) override;

public:
    /** Binds the mouse click handler
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachClick(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventClick, callback, callbackID); }

    /** Binds the mouse right-click handler
    * @param [in] callback The callback function to bind
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
    */
    void AttachRClick(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventRClick, callback, callbackID); }

    /** Listens for the control double-click event
     * @param [in] callback The callback function invoked when a double-click message is received
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachDoubleClick(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventMouseDoubleClick, callback, callbackID); }

    /** Listens for the Enter key event
     * @param [in] callback The callback function invoked when the Enter key is received
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachReturn(const EventCallback& callback, EventCallbackID callbackID = 0) { this->AttachEvent(kEventReturn, callback, callbackID); }

protected:
    /** Selection state change event (the m_bSelected variable has changed)
    */
    virtual void OnPrivateSetSelected() override;

    /** Checked state change event (the m_bChecked variable has changed)
    */
    virtual void OnPrivateSetChecked() override;

private:
    /** The child item index in the ListBox container, range: [0, GetItemCount())
    */
    size_t m_iListBoxIndex;

    /** The virtual table data element child item index, used to support the virtual table, range: [0, GetElementCount())
    */
    size_t m_iElementIndex;

    /** The ListBox container interface
    */
    IListBoxOwner* m_pOwner;
};

/////////////////////////////////////////////////////////////////////////////////////

template<typename InheritType>
ListBoxItemTemplate<InheritType>::ListBoxItemTemplate(Window* pWindow):
    OptionTemplate<InheritType>(pWindow),
    m_iListBoxIndex(Box::InvalidIndex),
    m_iElementIndex(Box::InvalidIndex),
    m_pOwner(nullptr)
{
    this->SetTextStyle(TEXT_LEFT | TEXT_VCENTER | TEXT_END_ELLIPSIS | TEXT_NOCLIP | TEXT_SINGLELINE, false);
}

template<typename InheritType>
DString ListBoxItemTemplate<InheritType>::GetType() const { return DUI_CTR_LISTBOX_ITEM; }

template<typename InheritType>
void ListBoxItemTemplate<InheritType>::SetItemSelected(bool bSelected)
{
    if (BaseClass::IsSelected() == bSelected) {
        return;
    }
    //Modify the internal state directly
    BaseClass::SetSelected(bSelected);
    if (m_pOwner == nullptr) {
        return;
    }

    //Synchronize the selection ID of the ListBox
    bool bChanged = false;
    if (bSelected) {
        m_pOwner->SetCurSel(m_iListBoxIndex);
        bChanged = true;
    }
    else {
        if (m_pOwner->GetCurSel() == m_iListBoxIndex) {
            m_pOwner->SetCurSel(Box::InvalidIndex);
            bChanged = true;
        }
    }

    if (bChanged && !m_pOwner->IsMultiSelect()) {
        //Single-select: the selection function needs to be called to ensure only one item is selected
        m_pOwner->EnsureSingleSelection();
    }
}

template<typename InheritType>
void ListBoxItemTemplate<InheritType>::OptionSelected(bool bSelect, bool bTriggerEvent)
{
    return BaseClass::Selected(bSelect, bTriggerEvent);
}

template<typename InheritType>
void ListBoxItemTemplate<InheritType>::Selected(bool bSelected, bool bTriggerEvent, uint64_t vkFlag)
{
    //UI clicks and other operations trigger the selection operation
    if (!this->IsEnabled()) {
        return;
    }
    if (m_pOwner != nullptr) {
        if (bSelected) {
            m_pOwner->SelectItem(m_iListBoxIndex, false, bTriggerEvent, vkFlag);
        }
        else {
            m_pOwner->UnSelectItem(m_iListBoxIndex, bTriggerEvent);
        }
    }
}

template<typename InheritType>
bool ListBoxItemTemplate<InheritType>::IsSelected() const
{
    return BaseClass::IsSelected();
}

template<typename InheritType>
void ListBoxItemTemplate<InheritType>::OnPrivateSetSelected()
{
    if (m_pOwner != nullptr) {
        m_pOwner->OnItemSelectedChanged(m_iListBoxIndex, this);
    }
}

template<typename InheritType>
void ListBoxItemTemplate<InheritType>::OnPrivateSetChecked()
{
    if (m_pOwner != nullptr) {
        m_pOwner->OnItemCheckedChanged(m_iListBoxIndex, this);
    }
}

template<typename InheritType>
bool ListBoxItemTemplate<InheritType>::CanPaintSelectedColors() const
{
    bool bHasStateImages = this->HasStateImages();
    if (m_pOwner != nullptr) {
        return m_pOwner->CanPaintSelectedColors(bHasStateImages);
    }
    return BaseClass::CanPaintSelectedColors();
}

template<typename InheritType>
void ListBoxItemTemplate<InheritType>::HandleEvent(const EventArgs& msg)
{
    if (this->IsDisabledEvents(msg)) {
        //If it is a mouse or keyboard message and the control is disabled, forward it to the Owner control
        if (m_pOwner != nullptr) {
            m_pOwner->SendEventMsg(msg);
        }
        else {
            BaseClass::HandleEvent(msg);
        }
        return;
    }
    if ((msg.eventType > kEventMouseBegin) && (msg.eventType < kEventMouseEnd)) {
        //Forward mouse-related messages to the parent container for priority processing
        if ((m_pOwner != nullptr) && m_pOwner->OnListBoxItemMouseEvent(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventWindowKillFocus) {
        if (m_pOwner != nullptr) {
            m_pOwner->OnListBoxItemWindowKillFocus();
        }
    }

    if (msg.eventType == kEventMouseDoubleClick) {
        if (!this->IsActivatable()) {
            return;
        }
    }
    else if (msg.eventType == kEventKeyDown && this->IsEnabled()) {
        if (msg.vkCode == kVK_RETURN) {
            if (this->IsActivatable()) {
                this->SendEvent(kEventReturn);
            }
            return;
        }
    }
    BaseClass::HandleEvent(msg);
}

template<typename InheritType>
IListBoxOwner* ListBoxItemTemplate<InheritType>::GetOwner()
{
    return m_pOwner;
}

template<typename InheritType>
void ListBoxItemTemplate<InheritType>::SetOwner(IListBoxOwner* pOwner)
{
    m_pOwner = pOwner;
}

template<typename InheritType>
size_t ListBoxItemTemplate<InheritType>::GetListBoxIndex() const
{
    return m_iListBoxIndex;
}

template<typename InheritType>
void ListBoxItemTemplate<InheritType>::SetListBoxIndex(size_t iIndex)
{
    m_iListBoxIndex = iIndex;
}

template<typename InheritType>
size_t ListBoxItemTemplate<InheritType>::GetElementIndex() const
{
    return m_iElementIndex;
}

template<typename InheritType>
void ListBoxItemTemplate<InheritType>::SetElementIndex(size_t iIndex)
{
    m_iElementIndex = iIndex;
}

template<typename InheritType>
bool ListBoxItemTemplate<InheritType>::IsSelectableType() const
{
    return true;
}

/** Type definitions
*/
typedef ListBoxItemTemplate<Box> ListBoxItem;
typedef ListBoxItemTemplate<HBox> ListBoxItemH;
typedef ListBoxItemTemplate<VBox> ListBoxItemV;

} // namespace ui

#endif // UI_BOX_LISTBOX_ITEM_H_
