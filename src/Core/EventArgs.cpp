#include "dui/Core/EventArgs.h"
#include "dui/Core/Control.h"
#include <unordered_map>
#include <vector>

namespace ui
{
EventArgs::EventArgs() :
    eventType(EventType::kEventNone),
    vkCode(VirtualKeyCode::kVK_None),
    wParam(0),
    lParam(0),
    pSender(nullptr),
    modifierKey(0),
    eventData(0),
    pEventData(nullptr),
    listCtrlType(-1)
{
    ptMouse.x = 0;
    ptMouse.y = 0;
}

void EventArgs::SetSender(Control* pControl)
{
    if (pControl != nullptr) {
        pSender = pControl;
        m_senderFlag = pControl->GetWeakFlag();
    }
    else {
        pSender = nullptr;
        m_senderFlag.reset();
    }
}

Control* EventArgs::GetSender() const
{
    if (m_senderFlag.expired()) {
        return nullptr;
    }
    else {
        return pSender;
    }
}

void EventArgs::SetSenderWeakFlag(std::weak_ptr<WeakFlag> senderFlag)
{
    m_senderFlag = senderFlag;
}

bool EventArgs::IsSenderExpired() const
{
    return m_senderFlag.expired();
}

//The data structure for converting between EventType and String
struct EventTypeString
{
    EventType m_eventType;      //The event type
    DString m_displayName;      //The display name of the event
    DString m_xmlName1;         //Name 1 of the event in XML
    DString m_xmlName2;         //Name 2 of the event in XML
};

//Initialize the data container for converting between EventType and String
static void InitEventStringMap(std::unordered_map<EventType, DString>* typeMap,
                               std::unordered_map<DString, EventType>* nameMap)
{
    if ((typeMap == nullptr) && (nameMap == nullptr)) {
        return;
    }
    const std::vector<EventTypeString> eventTypeStringList = {
        {kEventAll, DUI_T("kEventAll"), DUI_T("All"), DUI_T("all")},
        {kEventDestroy, DUI_T("kEventDestroy"), DUI_T("Destroy"), DUI_T("destroy")},
        {kEventKeyDown, DUI_T("kEventKeyDown"), DUI_T("KeyDown"), DUI_T("key_down")},
        {kEventKeyUp, DUI_T("kEventKeyUp"), DUI_T("KeyUp"), DUI_T("key_up")},
        {kEventChar, DUI_T("kEventChar"), DUI_T("Char"), DUI_T("char")},
        {kEventMouseEnter, DUI_T("kEventMouseEnter"), DUI_T("MouseEnter"), DUI_T("mouse_enter")},
        {kEventMouseLeave, DUI_T("kEventMouseLeave"), DUI_T("MouseLeave"), DUI_T("mouse_leave")},
        {kEventMouseMove, DUI_T("kEventMouseMove"), DUI_T("MouseMove"), DUI_T("mouse_move")},
        {kEventMouseHover, DUI_T("kEventMouseHover"), DUI_T("MouseHover"), DUI_T("mouse_hover")},
        {kEventMouseWheel, DUI_T("kEventMouseWheel"), DUI_T("MouseWheel"), DUI_T("mouse_wheel")},
        {kEventMouseButtonDown, DUI_T("kEventMouseButtonDown"), DUI_T("MouseButtonDown"), DUI_T("mouse_button_down")},
        {kEventMouseButtonUp, DUI_T("kEventMouseButtonUp"), DUI_T("MouseButtonUp"), DUI_T("mouse_button_up")},
        {kEventMouseDoubleClick, DUI_T("kEventMouseDoubleClick"), DUI_T("MouseDoubleClick"), DUI_T("mouse_double_click")},
        {kEventMouseRButtonDown, DUI_T("kEventMouseRButtonDown"), DUI_T("MouseRButtonDown"), DUI_T("mouse_rbutton_down")},
        {kEventMouseRButtonUp, DUI_T("kEventMouseRButtonUp"), DUI_T("MouseRButtonUp"), DUI_T("mouse_rbutton_up")},
        {kEventMouseRDoubleClick, DUI_T("kEventMouseRDoubleClick"), DUI_T("MouseRDoubleClick"), DUI_T("mouse_rdouble_click")},
        {kEventMouseMButtonDown, DUI_T("kEventMouseMButtonDown"), DUI_T("MouseMButtonDown"), DUI_T("mouse_mbutton_down")},
        {kEventMouseMButtonUp, DUI_T("kEventMouseMButtonUp"), DUI_T("MouseMButtonUp"), DUI_T("mouse_mbutton_up")},
        {kEventMouseMDoubleClick, DUI_T("kEventMouseMDoubleClick"), DUI_T("MouseMDoubleClick"), DUI_T("mouse_mdouble_click")},
        {kEventContextMenu, DUI_T("kEventContextMenu"), DUI_T("ContextMenu"), DUI_T("context_menu")},
        {kEventSetFocus, DUI_T("kEventSetFocus"), DUI_T("SetFocus"), DUI_T("set_focus")},
        {kEventKillFocus, DUI_T("kEventKillFocus"), DUI_T("KillFocus"), DUI_T("kill_focus")},
        {kEventSetCursor, DUI_T("kEventSetCursor"), DUI_T("SetCursor"), DUI_T("set_cursor")},
        {kEventCaptureChanged, DUI_T("kEventCaptureChanged"), DUI_T("CaptureChanged"), DUI_T("capture_changed")},
        {kEventImeSetContext, DUI_T("kEventImeSetContext"), DUI_T("ImeSetContext"), DUI_T("ime_set_context")},
        {kEventImeStartComposition, DUI_T("kEventImeStartComposition"), DUI_T("ImeStartComposition"), DUI_T("ime_start_composition")},
        {kEventImeComposition, DUI_T("kEventImeComposition"), DUI_T("ImeComposition"), DUI_T("ime_composition")},
        {kEventImeEndComposition, DUI_T("kEventImeEndComposition"), DUI_T("ImeEndComposition"), DUI_T("ime_end_composition")},
        {kEventWindowSetFocus, DUI_T("kEventWindowSetFocus"), DUI_T("WindowSetFocus"), DUI_T("window_set_focus")},
        {kEventWindowKillFocus, DUI_T("kEventWindowKillFocus"), DUI_T("WindowKillFocus"), DUI_T("window_kill_focus")},
        {kEventWindowPosChanged, DUI_T("kEventWindowPosChanged"), DUI_T("WindowPosChanged"), DUI_T("window_pos_changed")},
        {kEventWindowSize, DUI_T("kEventWindowSize"), DUI_T("WindowSize"), DUI_T("window_size")},
        {kEventWindowMove, DUI_T("kEventWindowMove"), DUI_T("WindowMove"), DUI_T("window_move")},
        {kEventWindowCreate, DUI_T("kEventWindowCreate"), DUI_T("WindowCreate"), DUI_T("window_create")},
        {kEventWindowClose, DUI_T("kEventWindowClose"), DUI_T("WindowClose"), DUI_T("window_close")},
        {kEventClick, DUI_T("kEventClick"), DUI_T("Click"), DUI_T("click")},
        {kEventRClick, DUI_T("kEventRClick"), DUI_T("RClick"), DUI_T("rclick")},
        {kEventMouseClickChanged, DUI_T("kEventMouseClickChanged"), DUI_T("MouseClickChanged"), DUI_T("mouse_click_changed")},
        {kEventMouseClickEsc, DUI_T("kEventMouseClickEsc"), DUI_T("MouseClickEsc"), DUI_T("mouse_click_esc")},
        {kEventSelect, DUI_T("kEventSelect"), DUI_T("Select"), DUI_T("select")},
        {kEventUnSelect, DUI_T("kEventUnSelect"), DUI_T("UnSelect"), DUI_T("unselect")},
        {kEventCheck, DUI_T("kEventCheck"), DUI_T("Check"), DUI_T("check")},
        {kEventUnCheck, DUI_T("kEventUnCheck"), DUI_T("UnCheck"), DUI_T("uncheck")},
        {kEventTabSelect, DUI_T("kEventTabSelect"), DUI_T("TabSelect"), DUI_T("tab_select")},
        {kEventExpand, DUI_T("kEventExpand"), DUI_T("Expand"), DUI_T("expand")},
        {kEventCollapse, DUI_T("kEventCollapse"), DUI_T("Collapse"), DUI_T("collapse")},
        {kEventZoom, DUI_T("kEventZoom"), DUI_T("Zoom"), DUI_T("zoom")},
        {kEventTextChanged, DUI_T("kEventTextChanged"), DUI_T("TextChanged"), DUI_T("text_changed")},
        {kEventSelChanged, DUI_T("kEventSelChanged"), DUI_T("SelChanged"), DUI_T("sel_changed")},
        {kEventReturn, DUI_T("kEventReturn"), DUI_T("Return"), DUI_T("return")},
        {kEventEsc, DUI_T("kEventEsc"), DUI_T("Esc"), DUI_T("esc")},
        {kEventTab, DUI_T("kEventTab"), DUI_T("Tab"), DUI_T("tab")},
        {kEventLinkClick, DUI_T("kEventLinkClick"), DUI_T("LinkClick"), DUI_T("link_click")},
        {kEventScrollPosChanged, DUI_T("kEventScrollPosChanged"), DUI_T("ScrollPosChanged"), DUI_T("scroll_pos_changed")},
        {kEventValueChanged, DUI_T("kEventValueChanged"), DUI_T("ValueChanged"), DUI_T("value_changed")},
        {kEventPosChanged, DUI_T("kEventPosChanged"), DUI_T("PosChanged"), DUI_T("pos_changed")},
        {kEventSizeChanged, DUI_T("kEventSizeChanged"), DUI_T("SizeChanged"), DUI_T("size_changed")},
        {kEventVisibleChanged, DUI_T("kEventVisibleChanged"), DUI_T("VisibleChanged"), DUI_T("visible_changed")},
        {kEventStateChanged, DUI_T("kEventStateChanged"), DUI_T("StateChanged"), DUI_T("state_changed")},
        {kEventSelectColor, DUI_T("kEventSelectColor"), DUI_T("SelectColor"), DUI_T("select_color")},
        {kEventSplitDraged, DUI_T("kEventSplitDraged"), DUI_T("SplitDraged"), DUI_T("split_draged")},
        {kEventElementFilled, DUI_T("kEventElementFilled"), DUI_T("ElementFilled"), DUI_T("element_filled")},
        {kEventEnterEdit, DUI_T("kEventEnterEdit"), DUI_T("EnterEdit"), DUI_T("enter_edit")},
        {kEventLeaveEdit, DUI_T("kEventLeaveEdit"), DUI_T("LeaveEdit"), DUI_T("leave_edit")},
        {kEventDataItemCountChanged, DUI_T("kEventDataItemCountChanged"), DUI_T("DataItemCountChanged"), DUI_T("data_item_count_changed")},
        {kEventItemMouseEnter, DUI_T("kEventItemMouseEnter"), DUI_T("ItemMouseEnter"), DUI_T("item_mouse_enter")},
        {kEventItemMouseLeave, DUI_T("kEventItemMouseLeave"), DUI_T("ItemMouseLeave"), DUI_T("item_mouse_leave")},
        {kEventSubItemMouseEnter, DUI_T("kEventSubItemMouseEnter"), DUI_T("SubItemMouseEnter"), DUI_T("sub_item_mouse_enter")},
        {kEventSubItemMouseLeave, DUI_T("kEventSubItemMouseLeave"), DUI_T("SubItemMouseLeave"), DUI_T("sub_item_mouse_leave")},
        {kEventReportViewItemFilled, DUI_T("kEventReportViewItemFilled"), DUI_T("ReportViewItemFilled"), DUI_T("report_view_item_filled")},
        {kEventReportViewSubItemFilled, DUI_T("kEventReportViewSubItemFilled"), DUI_T("ReportViewSubItemFilled"), DUI_T("report_view_sub_item_filled")},
        {kEventListViewItemFilled, DUI_T("kEventListViewItemFilled"), DUI_T("ListViewItemFilled"), DUI_T("list_view_item_filled")},
        {kEventIconViewItemFilled, DUI_T("kEventIconViewItemFilled"), DUI_T("IconViewItemFilled"), DUI_T("icon_view_item_filled")},
        {kEventViewTypeChanged, DUI_T("kEventViewTypeChanged"), DUI_T("ViewTypeChanged"), DUI_T("view_type_changed")},
        {kEventViewPosChanged, DUI_T("kEventViewPosChanged"), DUI_T("ViewPosChanged"), DUI_T("view_pos_changed")},
        {kEventViewSizeChanged, DUI_T("kEventViewSizeChanged"), DUI_T("ViewSizeChanged"), DUI_T("view_size_changed")},
        {kEventPathChanged, DUI_T("kEventPathChanged"), DUI_T("PathChanged"), DUI_T("path_changed")},
        {kEventPathClick, DUI_T("kEventPathClick"), DUI_T("PathClick"), DUI_T("path_click")},
        {kEventDropEnter, DUI_T("kEventDropEnter"), DUI_T("DropEnter"), DUI_T("drop_enter")},
        {kEventDropOver, DUI_T("kEventDropOver"), DUI_T("DropOver"), DUI_T("drop_over")},
        {kEventDropLeave, DUI_T("kEventDropLeave"), DUI_T("DropLeave"), DUI_T("drop_leave")},
        {kEventDropData, DUI_T("kEventDropData"), DUI_T("DropData"), DUI_T("drop_data")},
        {kEventImageAnimationStart, DUI_T("kEventImageAnimationStart"), DUI_T("ImageAnimationStart"), DUI_T("image_animation_start")},
        {kEventImageAnimationPlayFrame, DUI_T("kEventImageAnimationPlayFrame"), DUI_T("ImageAnimationPlayFrame"), DUI_T("image_animation_play_frame")},
        {kEventImageAnimationStop, DUI_T("kEventImageAnimationStop"), DUI_T("ImageAnimationStop"), DUI_T("image_animation_stop")},
        {kEventLoadingStart, DUI_T("kEventLoadingStart"), DUI_T("LoadingStart"), DUI_T("loading_start")},
        {kEventLoading, DUI_T("kEventLoading"), DUI_T("Loading"), DUI_T("loading")},
        {kEventLoadingStop, DUI_T("kEventLoadingStop"), DUI_T("LoadingStop"), DUI_T("loading_stop")},
        {kEventImageLoad, DUI_T("kEventImageLoad"), DUI_T("ImageLoad"), DUI_T("image_load")},
        {kEventImageDecode, DUI_T("kEventImageDecode"), DUI_T("ImageDecode"), DUI_T("image_decode")}
    };

    for (const EventTypeString& typeString : eventTypeStringList) {
        if (typeMap != nullptr) {
            (*typeMap)[typeString.m_eventType] = typeString.m_displayName;
        }
        if (nameMap != nullptr) {
            (*nameMap)[typeString.m_xmlName1] = typeString.m_eventType;
            (*nameMap)[typeString.m_xmlName2] = typeString.m_eventType;
        }
    }
}

EventType EventUtils::StringToEventType(const DString& eventName)
{
    static std::unordered_map<DString, EventType> nameMap;
    InitEventStringMap(nullptr, &nameMap);
    auto iter = nameMap.find(eventName);
    ASSERT(iter != nameMap.end());
    if (iter != nameMap.end()) {
        return iter->second;
    }
    return EventType::kEventNone;
}

DString EventUtils::EventTypeToString(EventType eventType)
{
    static std::unordered_map<EventType, DString> typeMap;
    InitEventStringMap(&typeMap, nullptr);
    auto iter = typeMap.find(eventType);
    ASSERT(iter != typeMap.end());
    if (iter != typeMap.end()) {
        return iter->second;
    }
    return DString();
}

void EventSource::AddEventCallback(const EventCallback& callback, EventCallbackID callbackID)
{
    ASSERT(callback != nullptr);
    if (callback != nullptr) {
        m_callbackList.push_back({ callback, callbackID });
    }
}

bool EventSource::RemoveEventCallbackByID(EventCallbackID callbackID)
{
    ASSERT(callbackID > 0);
    if (callbackID == 0) {
        return false;
    }
    bool bRet = false;
    auto iter = m_callbackList.begin();
    while (iter != m_callbackList.end()) {
        if (iter->m_callbackID == callbackID) {
            iter = m_callbackList.erase(iter);
            bRet = true;
        }
        else {
            ++iter;
        }
    }
    return bRet;
}

bool EventSource::HasEventCallbackByID(EventCallbackID callbackID) const
{
    for (auto iter = m_callbackList.begin(); iter != m_callbackList.end(); ++iter) {
        if (iter->m_callbackID == callbackID) {
            return true;
        }
    }
    return false;
}

bool EventSource::IsEmpty() const
{
    return m_callbackList.empty();
}

bool EventSource::operator() (const ui::EventArgs& args) const
{
    //Supports operating on this container inside the callback function
    const size_t nMaxCallbackCount = m_callbackList.size(); //The maximum callback count for this invocation
    for (size_t nIndex = 0; nIndex < nMaxCallbackCount; ++nIndex) {
        if (args.IsSenderExpired()) {
            //The Sender control has expired, no more callback events are generated
            return false;
        }
        //Need to copy a copy, to avoid operating on this container inside the callback function, which would invalidate the container contents and cause a crash
        EventCallback callback = m_callbackList.at(nIndex).m_callback;
        if ((callback == nullptr) || !callback(args)) {
            return false;
        }
        if (nIndex >= m_callbackList.size()) {
            //Avoid removing the callback function from the container inside the callback, which would cause an out-of-bounds index access
            break;
        }
    }
    return true;
}

bool EventUtils::RemoveEventCallbackByID(EventMap& eventMap, EventCallbackID callbackID)
{
    bool bRet = false;
    auto iter = eventMap.begin();
    while (iter != eventMap.end()) {
        if (iter->second.RemoveEventCallbackByID(callbackID)) {
            bRet = true;
        }
        if (iter->second.IsEmpty()) {
            iter = eventMap.erase(iter);
        }
        else {
            ++iter;
        }
    }
    return bRet;
}

bool EventUtils::RemoveEventCallbackByID(EventMap& eventMap, EventType eventType, EventCallbackID callbackID)
{
    bool bRet = false;
    auto iter = eventMap.find(eventType);
    if (iter != eventMap.end()) {
        if (iter->second.RemoveEventCallbackByID(callbackID)) {
            bRet = true;
        }
        if (iter->second.IsEmpty()) {
            iter = eventMap.erase(iter);
        }
    }
    return bRet;
}

bool EventUtils::HasEventCallbackByID(const EventMap& eventMap, EventCallbackID callbackID)
{
    bool bRet = false;
    auto iter = eventMap.begin();
    while (iter != eventMap.end()) {
        if (iter->second.HasEventCallbackByID(callbackID)) {
            bRet = true;
            break;
        }
        ++iter;
    }
    return bRet;
}

bool EventUtils::HasEventCallbackByID(const EventMap& eventMap, EventType eventType, EventCallbackID callbackID)
{
    bool bRet = false;
    auto iter = eventMap.find(eventType);
    if (iter != eventMap.end()) {
        if (iter->second.HasEventCallbackByID(callbackID)) {
            bRet = true;
        }
    }
    return bRet;
}

} //namespace ui
