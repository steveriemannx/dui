#include "dui/Control/ListCtrlLabel.h"
#include "dui/Control/ListCtrlView.h"
#include "dui/Core/Window.h"

namespace ui
{
ListCtrlLabel::ListCtrlLabel(Window* pWindow) :
    CheckBoxTemplate<HBox>(pWindow),
    m_pListBoxItem(nullptr),
    m_bMouseDown(false),
    m_bEnableEdit(false)
{
    //No focus by default; no keyboard messages
    SetNoFocus();
    SetKeyboardEnabled(false);
}

DString ListCtrlLabel::GetType() const { return _T("ListCtrlLabel"); }

void ListCtrlLabel::HandleEvent(const EventArgs& msg)
{
    if ((msg.eventType > kEventMouseBegin) && (msg.eventType < kEventMouseEnd) &&
        (msg.eventType != EventType::kEventMouseEnter) && (msg.eventType != EventType::kEventMouseLeave)) {
        //Mouse message
        if (!IsDisabledEvents(msg) && (m_pListBoxItem != nullptr)) {
            //Can handle mouse messages
            if (OnMouseEvent(msg)) {
                return;
            }
        }
    }
    //Passes to the base class
    BaseClass::HandleEvent(msg);
}

bool ListCtrlLabel::OnMouseEvent(const EventArgs& msg)
{
    if (msg.IsSenderExpired()) {
        return false;
    }
    //Validation of the basic conditions the business logic depends on
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
    //When the control is disabled for mouse messages, forward mouse messages to the upper layer
    auto labelFlag = GetWeakFlag();
    auto windowFlag = pWindow->GetWeakFlag();
    bool bButtonUpEvent = false;
    if (IsEnableEdit()) {
        if (msg.eventType == kEventMouseButtonDown) {
            m_bMouseDown = false;
            if (pWindow->GetFocusControl() == m_pListBoxItem) { //Ensure the current list item is the focus control
                //Avoid entering edit mode on every click
                m_bMouseDown = true;
            }
        }
        else if (msg.eventType == kEventMouseButtonUp) {
            if (m_bMouseDown) {
                m_bMouseDown = false;
                bButtonUpEvent = true;
            }
        }
    }
    else {
        m_bMouseDown = false;
    }

    //First dispatch to the sub-items of the view for business processing
    pViewItem->OnChildItemMouseEvent(msg);
    if (labelFlag.expired() || windowFlag.expired()) {
        return true;
    }
    if ((msg.eventType != kEventMouseEnter) &&
        (msg.eventType != kEventMouseLeave) &&
        (msg.eventType != kEventMouseMove)  &&
        (msg.eventType != kEventMouseHover)) {
        //The window focus changed; the message is no longer passed (e.g. after popping up a context menu)
        if (bWindowFocused != pWindow->IsWindowFocused()) {
            return true;
        }
        if (pWindow->GetFocusControl() != m_pListBoxItem) {
            //Ensure the current list item is the focus control; otherwise stop processing
            return true;
        }

        if (bButtonUpEvent) {
            //Enters the edit state
            OnItemEnterEditMode();
        }
    }

    //Passes to the base class
    BaseClass::HandleEvent(msg);

    if (labelFlag.expired() || windowFlag.expired() || msg.IsSenderExpired()) {
        return true;
    }

    //Finally trigger the message dispatch of the ListCtrl sub-items
    pViewItem->FireChildItemMouseEvent(msg);
    return true;
}

void ListCtrlLabel::OnItemEnterEditMode()
{
    SendEvent(kEventEnterEdit, (WPARAM)this);
}

void ListCtrlLabel::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    if (!m_textRect.IsZero()) {
        m_textRect = Dpi().GetScaleRect(m_textRect, nOldDpiScale);
    }
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

void ListCtrlLabel::SetTextRect(const UiRect& rect)
{
    m_textRect = rect;
}

UiRect ListCtrlLabel::GetTextRect() const
{
    UiRect rect = m_textRect;
    if (rect.IsZero()) {
        rect = GetRect();
        rect.Deflate(GetControlPadding());
        rect.Deflate(GetTextPadding());
    }
    return rect;
}

void ListCtrlLabel::SetListBoxItem(Control* pListBoxItem)
{
    m_pListBoxItem = pListBoxItem;
}

void ListCtrlLabel::SetEnableEdit(bool bEnableEdit)
{
    m_bEnableEdit = bEnableEdit;
}

bool ListCtrlLabel::IsEnableEdit() const
{
    return m_bEnableEdit;
}

}//namespace ui
