#include "duilib/Control/ListCtrlIcon.h"
#include "duilib/Control/ListCtrlView.h"
#include "duilib/Core/Window.h"

namespace ui
{
ListCtrlIcon::ListCtrlIcon(Window* pWindow) :
    CheckBoxTemplate<HBox>(pWindow),
    m_pListBoxItem(nullptr)
{
    //By default, does not get focus, no keyboard messages
    SetNoFocus();
    SetKeyboardEnabled(false);
}

DString ListCtrlIcon::GetType() const { return _T("ListCtrlIcon"); }

void ListCtrlIcon::HandleEvent(const EventArgs& msg)
{
    if ((msg.eventType > kEventMouseBegin) && (msg.eventType < kEventMouseEnd) &&
        (msg.eventType != EventType::kEventMouseEnter) && (msg.eventType != EventType::kEventMouseLeave)) {
        //Mouse message
        if (!IsDisabledEvents(msg) && (m_pListBoxItem != nullptr)) {
            //The mouse message can be handled
            if (OnMouseEvent(msg)) {
                return;
            }
        }
    }
    //Hand over to the base class
    BaseClass::HandleEvent(msg);
}

bool ListCtrlIcon::OnMouseEvent(const EventArgs& msg)
{
    //Validation of the basic conditions required by the business logic
    ASSERT((msg.eventType > kEventMouseBegin) && (msg.eventType < kEventMouseEnd));
    if ((msg.eventType <= kEventMouseBegin) || (msg.eventType >= kEventMouseEnd)) {
        return false;
    }
    Window* pWindow = GetWindow();
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return false;
    }
    ASSERT(IsEnabled() && IsMouseEnabled());
    if (!IsEnabled() || !IsMouseEnabled()) {
        return false;
    }
    ASSERT(m_pListBoxItem != nullptr);
    if (m_pListBoxItem == nullptr) {
        return false;
    }
    IListCtrlViewItem* pViewItem = dynamic_cast<IListCtrlViewItem*>(m_pListBoxItem);
    ASSERT(pViewItem != nullptr);
    if (pViewItem == nullptr) {
        return false;
    }
    const bool bWindowFocused = pWindow->IsWindowFocused();
    auto iconFlag = GetWeakFlag();
    auto windowFlag = pWindow->GetWeakFlag();

    //First hand over to the child item of the view for business processing
    pViewItem->OnChildItemMouseEvent(msg);
    if (iconFlag.expired() || windowFlag.expired()) {
        return true;
    }
    //The window focus has changed, this message is no longer forwarded (e.g., a right-click menu was popped up)
    if (bWindowFocused != pWindow->IsWindowFocused()) {
        return true;
    }
    if (pWindow->GetFocusControl() != m_pListBoxItem) {
        //Ensure the current list item is the focus control, otherwise stop processing
        return true;
    }

    //Hand over to the base class
    BaseClass::HandleEvent(msg);

    if (iconFlag.expired() || windowFlag.expired() || msg.IsSenderExpired()) {
        return true;
    }

    //Finally trigger the message dispatch of the ListCtrl child item
    pViewItem->FireChildItemMouseEvent(msg);
    return true;
}

void ListCtrlIcon::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

void ListCtrlIcon::SetListBoxItem(Control* pListBoxItem)
{
    m_pListBoxItem = pListBoxItem;
}

}//namespace ui
