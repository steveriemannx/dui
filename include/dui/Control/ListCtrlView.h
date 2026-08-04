#ifndef UI_CONTROL_LIST_CTRL_VIEW_H_
#define UI_CONTROL_LIST_CTRL_VIEW_H_

#include "dui/Box/VirtualListBox.h"

namespace ui
{
/** Base class of the ListCtrl list view UI control (implements child item selection, including arrow keys, shortcut keys, mouse selection logic, etc.)
*   When using this class, the ListCtrlItemTemplate template must be used to form a subclass as the ListBoxItem
*/
class DUI_API ListCtrlView : public VirtualListBox
{
    typedef VirtualListBox BaseClass;
public:
    ListCtrlView(Window* pWindow, Layout* pLayout);
    virtual ~ListCtrlView() override;

    virtual DString GetType() const override { return _T("ListCtrlView"); }
    virtual void HandleEvent(const EventArgs& msg) override;

    /** Select a child item
    *  @param [in] iIndex The ID of the child item
    *  @param [in] bTakeFocus Whether to make the child item control the focus control
    *  @param [in] bTriggerEvent Whether to trigger the selection event; if true, a kEventSelect event will be triggered
    *  @param [in] vkFlag The key flag; refer to the definition of enum VKFlag for the value range
    */
    virtual bool SelectItem(size_t iIndex, bool bTakeFocus,
                            bool bTriggerEvent, uint64_t vkFlag) override;

public:
    /** Listen for the event that the selected item changes
     * @param [in] callback The callback function called when a child item is selected
     * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function)
     */
    void AttachSelChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventSelChanged, callback, callbackID); }

public:
    /** Get the index of the top element
    */
    virtual size_t GetTopElementIndex() const;

    /** Determine whether a data item is visible
    * @param [in] itemIndex The index of the data item, valid range: [0, GetDataItemCount())
    */
    virtual bool IsDataItemDisplay(size_t itemIndex) const;

    /** Get the list of currently displayed data items, in order from top to bottom
    * @param [in] itemIndexList The list of indexes of the currently displayed data items
    */
    virtual void GetDisplayDataItems(std::vector<size_t>& itemIndexList) const;

    /** Ensure that the data index item is visible
    * @param [in] itemIndex The index of the data item
    * @param [in] bToTop Whether to ensure it is at the top
    */
    virtual bool EnsureDataItemVisible(size_t itemIndex, bool bToTop);

protected:
    /** Control initialization
    */
    virtual void OnInit() override;
};

/** Functional interface of the child item data in ListCtrlView
*/
class DUI_API IListCtrlViewItem
{
public:
    virtual ~IListCtrlViewItem() = default;

    /** Mouse event (from the message forwarding of the child control)
    * @param [in] msg The content of the mouse event
    */
    virtual void OnChildItemMouseEvent(const EventArgs& msg) = 0;

    /** Fire the mouse-related callback event (from the message forwarding of the child control)
    * @param [in] msg The content of the mouse event
    */
    virtual void FireChildItemMouseEvent(const EventArgs& msg) = 0;
};

/** Base class template of the data items in each view of ListCtrl
*/
template<typename InheritType>
class ListCtrlItemTemplate : public ListBoxItemTemplate<InheritType>, public IListCtrlViewItem
{
    typedef ListBoxItemTemplate<InheritType> BaseClass;
public:
    explicit ListCtrlItemTemplate(Window* pWindow):
        ListBoxItemTemplate<InheritType>(pWindow)
    {
    }
    virtual ~ListCtrlItemTemplate() override = default;

    /** Set whether the control is in the selected state
  * @param [in] bSelected true means the selected state, false means the deselected state
  * @param [in] bTriggerEvent Whether to send the state change event; true to send, otherwise false. The default is false
  * @param [in] vkFlag The key flag; refer to the definition of enum VKFlag for the value range
  */
    virtual void Selected(bool bSelect, bool bTriggerEvent, uint64_t vkFlag) override
    {
        if (BaseClass::IsSelected() != bSelect) {
            BaseClass::Selected(bSelect, bTriggerEvent, vkFlag);
        }
    }

protected:
    /** Activate function
    */
    virtual void Activate(const EventArgs* pMsg) override
    {
        //Override the implementation logic of the base class; here only a Click event is fired
        if (this->IsActivatable()) {
            if (pMsg != nullptr) {
                EventArgs newMsg = *pMsg;
                newMsg.eventData = pMsg->eventType;
                newMsg.eventType = kEventNone;
                this->SendEvent(kEventClick, newMsg);
            }
            else {
                this->SendEvent(kEventClick);
            }            
        }
    }

    /** Mouse left button down event: triggers the select child item event
    */
    virtual bool ButtonDown(const EventArgs& msg) override
    {
        if (this->IsEnabled() && this->IsActivatable() && this->IsPointInWithScrollOffset(msg.ptMouse)) {
            uint64_t vkFlag = kVkLButton;
            if (this->IsKeyDown(msg, ModifierKey::kControl)) {
                vkFlag |= kVkControl;
            }
            if (this->IsKeyDown(msg, ModifierKey::kShift)) {
                vkFlag |= kVkShift;
            }
            //Select when the left button is pressed
            SelectItem(vkFlag);
        }
        return BaseClass::ButtonDown(msg);
    }

    /** Mouse right button down event: triggers the select child item event
    */
    virtual bool RButtonDown(const EventArgs& msg) override
    {
        if (this->IsEnabled() && this->IsActivatable() && this->IsPointInWithScrollOffset(msg.ptMouse)) {
            uint64_t vkFlag = kVkRButton;            
            if (this->IsKeyDown(msg, ModifierKey::kControl)) {
                vkFlag |= kVkControl;
            }
            if (this->IsKeyDown(msg, ModifierKey::kShift)) {
                vkFlag |= kVkShift;
            }
            //Select when the right button is pressed
            SelectItem(vkFlag);
        }
        return BaseClass::RButtonDown(msg);
    }

    /** Execute the select function
    * @param [in] vkFlag The key flag; refer to the definition of enum VKFlag for the value range
    */
    void SelectItem(uint64_t vkFlag)
    {
        IListBoxOwner* pOwner = this->GetOwner();
        ASSERT(pOwner != nullptr);
        if (pOwner != nullptr) {
            size_t nListBoxIndex = this->GetListBoxIndex();
            pOwner->SelectItem(nListBoxIndex, true, true, vkFlag);
        }
    }

    /** Mouse event (from the message forwarding of the child control)
    * @param [in] msg The content of the mouse event
    */
    virtual void OnChildItemMouseEvent(const EventArgs& msg) override
    {
        if ((msg.eventType == EventType::kEventMouseEnter) || (msg.eventType == EventType::kEventMouseLeave)) {
            //These two events do not need to be forwarded; the child control automatically forwards them to the parent control
            return;
        }
        BaseClass::HandleEvent(msg);
    }

    /** Fire the mouse-related callback event (from the message forwarding of the child control)
    * @param [in] msg The content of the mouse event
    */
    virtual void FireChildItemMouseEvent(const EventArgs& msg) override
    {
        if ((msg.eventType == EventType::kEventMouseEnter) || (msg.eventType == EventType::kEventMouseLeave)) {
            //These two events do not need to be forwarded; the child control automatically forwards them to the parent control and triggers the event corresponding to the message
            return;
        }
        EventArgs newMsg = msg;
        newMsg.SetSender(this);//Fire the callback by the Item control
        this->FireAllEvents(newMsg);
    }
};

typedef ListCtrlItemTemplate<Box> ListCtrlItemBase;   //Base class: ListBoxItem
typedef ListCtrlItemTemplate<HBox> ListCtrlItemBaseH; //Base class: ListBoxItemH
typedef ListCtrlItemTemplate<VBox> ListCtrlItemBaseV; //Base class: ListBoxItemV

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_VIEW_H_
