#include "dui/Control/ListCtrlView.h" 
#include "dui/Control/ListCtrl.h"
#include "dui/Control/ListCtrlData.h"
#include "dui/Core/Keyboard.h"

namespace ui
{
ListCtrlView::ListCtrlView(Window* pWindow, Layout* pLayout):
    VirtualListBox(pWindow, pLayout)
{
}

ListCtrlView::~ListCtrlView()
{
}

size_t ListCtrlView::GetTopElementIndex() const
{
    std::vector<size_t> itemIndexList;
    GetDisplayElements(itemIndexList);
    if (itemIndexList.empty()) {
        return Box::InvalidIndex;
    }
    else {
        return itemIndexList.front();
    }
}

bool ListCtrlView::IsDataItemDisplay(size_t itemIndex) const
{
    std::vector<size_t> itemIndexList;
    GetDisplayElements(itemIndexList);
    return std::find(itemIndexList.begin(), itemIndexList.end(), itemIndex) != itemIndexList.end();
}

void ListCtrlView::GetDisplayDataItems(std::vector<size_t>& itemIndexList) const
{
    itemIndexList.clear();
    GetDisplayElements(itemIndexList);
}

bool ListCtrlView::EnsureDataItemVisible(size_t itemIndex, bool bToTop)
{
    EnsureVisible(itemIndex, bToTop);
    return true;
}

void ListCtrlView::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();

    //Prohibit changing the selected item when scrolling with the mouse wheel
    SetScrollSelect(false);
}

void ListCtrlView::HandleEvent(const EventArgs& msg)
{
    if (IsDisabledEvents(msg)) {
        //If it is a mouse or keyboard message, and the control is disabled, forward it to the parent control
        Box* pParent = GetParent();
        if (pParent != nullptr) {
            pParent->SendEventMsg(msg);
        }
        else {
            BaseClass::HandleEvent(msg);
        }
        return;
    }
    bool bHandled = false;
    if (msg.eventType == kEventKeyDown) {
        //Skip the base class function switch and directly call ListCtrl's shortcut key handler function
        bHandled = OnListCtrlKeyDown(msg);
    }
    if (!bHandled) {
        BaseClass::HandleEvent(msg);
    }
}

bool ListCtrlView::SelectItem(size_t iIndex, bool bTakeFocus, bool bTriggerEvent, uint64_t vkFlag)
{
    //Skip the base class's IsSelectLikeListCtrl() functionality and directly use ListCtrl's selection mode
    return ListCtrlSelectItem(iIndex, bTakeFocus, bTriggerEvent, vkFlag);
}

}//namespace ui

