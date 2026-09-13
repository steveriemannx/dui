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
    std::string m_displayName;      //The display name of the event
    std::string m_xmlName1;         //Name 1 of the event in XML
    std::string m_xmlName2;         //Name 2 of the event in XML
};

//Initialize the data container for converting between EventType and String
static void InitEventStringMap(std::unordered_map<EventType, std::string>* typeMap,
                               std::unordered_map<std::string, EventType>* nameMap)
{
    if ((typeMap == nullptr) && (nameMap == nullptr)) {
        return;
    }
    const std::vector<EventTypeString> eventTypeStringList = {
        {kEventAll, "kEventAll", "All", "all"},
        {kEventDestroy, "kEventDestroy", "Destroy", "destroy"},
        {kEventKeyDown, "kEventKeyDown", "KeyDown", "key_down"},
        {kEventKeyUp, "kEventKeyUp", "KeyUp", "key_up"},
        {kEventChar, "kEventChar", "Char", "char"},
        {kEventMouseEnter, "kEventMouseEnter", "MouseEnter", "mouse_enter"},
        {kEventMouseLeave, "kEventMouseLeave", "MouseLeave", "mouse_leave"},
        {kEventMouseMove, "kEventMouseMove", "MouseMove", "mouse_move"},
        {kEventMouseHover, "kEventMouseHover", "MouseHover", "mouse_hover"},
        {kEventMouseWheel, "kEventMouseWheel", "MouseWheel", "mouse_wheel"},
        {kEventMouseButtonDown, "kEventMouseButtonDown", "MouseButtonDown", "mouse_button_down"},
        {kEventMouseButtonUp, "kEventMouseButtonUp", "MouseButtonUp", "mouse_button_up"},
        {kEventMouseDoubleClick, "kEventMouseDoubleClick", "MouseDoubleClick", "mouse_double_click"},
        {kEventMouseRButtonDown, "kEventMouseRButtonDown", "MouseRButtonDown", "mouse_rbutton_down"},
        {kEventMouseRButtonUp, "kEventMouseRButtonUp", "MouseRButtonUp", "mouse_rbutton_up"},
        {kEventMouseRDoubleClick, "kEventMouseRDoubleClick", "MouseRDoubleClick", "mouse_rdouble_click"},
        {kEventMouseMButtonDown, "kEventMouseMButtonDown", "MouseMButtonDown", "mouse_mbutton_down"},
        {kEventMouseMButtonUp, "kEventMouseMButtonUp", "MouseMButtonUp", "mouse_mbutton_up"},
        {kEventMouseMDoubleClick, "kEventMouseMDoubleClick", "MouseMDoubleClick", "mouse_mdouble_click"},
        {kEventContextMenu, "kEventContextMenu", "ContextMenu", "context_menu"},
        {kEventSetFocus, "kEventSetFocus", "SetFocus", "set_focus"},
        {kEventKillFocus, "kEventKillFocus", "KillFocus", "kill_focus"},
        {kEventSetCursor, "kEventSetCursor", "SetCursor", "set_cursor"},
        {kEventCaptureChanged, "kEventCaptureChanged", "CaptureChanged", "capture_changed"},
        {kEventImeSetContext, "kEventImeSetContext", "ImeSetContext", "ime_set_context"},
        {kEventImeStartComposition, "kEventImeStartComposition", "ImeStartComposition", "ime_start_composition"},
        {kEventImeComposition, "kEventImeComposition", "ImeComposition", "ime_composition"},
        {kEventImeEndComposition, "kEventImeEndComposition", "ImeEndComposition", "ime_end_composition"},
        {kEventWindowSetFocus, "kEventWindowSetFocus", "WindowSetFocus", "window_set_focus"},
        {kEventWindowKillFocus, "kEventWindowKillFocus", "WindowKillFocus", "window_kill_focus"},
        {kEventWindowPosChanged, "kEventWindowPosChanged", "WindowPosChanged", "window_pos_changed"},
        {kEventWindowSize, "kEventWindowSize", "WindowSize", "window_size"},
        {kEventWindowMove, "kEventWindowMove", "WindowMove", "window_move"},
        {kEventWindowCreate, "kEventWindowCreate", "WindowCreate", "window_create"},
        {kEventWindowClose, "kEventWindowClose", "WindowClose", "window_close"},
        {kEventClick, "kEventClick", "Click", "click"},
        {kEventRClick, "kEventRClick", "RClick", "rclick"},
        {kEventMouseClickChanged, "kEventMouseClickChanged", "MouseClickChanged", "mouse_click_changed"},
        {kEventMouseClickEsc, "kEventMouseClickEsc", "MouseClickEsc", "mouse_click_esc"},
        {kEventSelect, "kEventSelect", "Select", "select"},
        {kEventUnSelect, "kEventUnSelect", "UnSelect", "unselect"},
        {kEventCheck, "kEventCheck", "Check", "check"},
        {kEventUnCheck, "kEventUnCheck", "UnCheck", "uncheck"},
        {kEventTabSelect, "kEventTabSelect", "TabSelect", "tab_select"},
        {kEventExpand, "kEventExpand", "Expand", "expand"},
        {kEventCollapse, "kEventCollapse", "Collapse", "collapse"},
        {kEventZoom, "kEventZoom", "Zoom", "zoom"},
        {kEventTextChanged, "kEventTextChanged", "TextChanged", "text_changed"},
        {kEventSelChanged, "kEventSelChanged", "SelChanged", "sel_changed"},
        {kEventReturn, "kEventReturn", "Return", "return"},
        {kEventEsc, "kEventEsc", "Esc", "esc"},
        {kEventTab, "kEventTab", "Tab", "tab"},
        {kEventLinkClick, "kEventLinkClick", "LinkClick", "link_click"},
        {kEventScrollPosChanged, "kEventScrollPosChanged", "ScrollPosChanged", "scroll_pos_changed"},
        {kEventValueChanged, "kEventValueChanged", "ValueChanged", "value_changed"},
        {kEventPosChanged, "kEventPosChanged", "PosChanged", "pos_changed"},
        {kEventSizeChanged, "kEventSizeChanged", "SizeChanged", "size_changed"},
        {kEventVisibleChanged, "kEventVisibleChanged", "VisibleChanged", "visible_changed"},
        {kEventStateChanged, "kEventStateChanged", "StateChanged", "state_changed"},
        {kEventSelectColor, "kEventSelectColor", "SelectColor", "select_color"},
        {kEventSplitDraged, "kEventSplitDraged", "SplitDraged", "split_draged"},
        {kEventElementFilled, "kEventElementFilled", "ElementFilled", "element_filled"},
        {kEventEnterEdit, "kEventEnterEdit", "EnterEdit", "enter_edit"},
        {kEventLeaveEdit, "kEventLeaveEdit", "LeaveEdit", "leave_edit"},
        {kEventDataItemCountChanged, "kEventDataItemCountChanged", "DataItemCountChanged", "data_item_count_changed"},
        {kEventItemMouseEnter, "kEventItemMouseEnter", "ItemMouseEnter", "item_mouse_enter"},
        {kEventItemMouseLeave, "kEventItemMouseLeave", "ItemMouseLeave", "item_mouse_leave"},
        {kEventSubItemMouseEnter, "kEventSubItemMouseEnter", "SubItemMouseEnter", "sub_item_mouse_enter"},
        {kEventSubItemMouseLeave, "kEventSubItemMouseLeave", "SubItemMouseLeave", "sub_item_mouse_leave"},
        {kEventReportViewItemFilled, "kEventReportViewItemFilled", "ReportViewItemFilled", "report_view_item_filled"},
        {kEventReportViewSubItemFilled, "kEventReportViewSubItemFilled", "ReportViewSubItemFilled", "report_view_sub_item_filled"},
        {kEventListViewItemFilled, "kEventListViewItemFilled", "ListViewItemFilled", "list_view_item_filled"},
        {kEventIconViewItemFilled, "kEventIconViewItemFilled", "IconViewItemFilled", "icon_view_item_filled"},
        {kEventViewTypeChanged, "kEventViewTypeChanged", "ViewTypeChanged", "view_type_changed"},
        {kEventViewPosChanged, "kEventViewPosChanged", "ViewPosChanged", "view_pos_changed"},
        {kEventViewSizeChanged, "kEventViewSizeChanged", "ViewSizeChanged", "view_size_changed"},
        {kEventPathChanged, "kEventPathChanged", "PathChanged", "path_changed"},
        {kEventPathClick, "kEventPathClick", "PathClick", "path_click"},
        {kEventDropEnter, "kEventDropEnter", "DropEnter", "drop_enter"},
        {kEventDropOver, "kEventDropOver", "DropOver", "drop_over"},
        {kEventDropLeave, "kEventDropLeave", "DropLeave", "drop_leave"},
        {kEventDropData, "kEventDropData", "DropData", "drop_data"},
        {kEventImageAnimationStart, "kEventImageAnimationStart", "ImageAnimationStart", "image_animation_start"},
        {kEventImageAnimationPlayFrame, "kEventImageAnimationPlayFrame", "ImageAnimationPlayFrame", "image_animation_play_frame"},
        {kEventImageAnimationStop, "kEventImageAnimationStop", "ImageAnimationStop", "image_animation_stop"},
        {kEventLoadingStart, "kEventLoadingStart", "LoadingStart", "loading_start"},
        {kEventLoading, "kEventLoading", "Loading", "loading"},
        {kEventLoadingStop, "kEventLoadingStop", "LoadingStop", "loading_stop"},
        {kEventImageLoad, "kEventImageLoad", "ImageLoad", "image_load"},
        {kEventImageDecode, "kEventImageDecode", "ImageDecode", "image_decode"}
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

EventType EventUtils::StringToEventType(const std::string& eventName)
{
    static std::unordered_map<std::string, EventType> nameMap;
    InitEventStringMap(nullptr, &nameMap);
    auto iter = nameMap.find(eventName);
    ASSERT(iter != nameMap.end());
    if (iter != nameMap.end()) {
        return iter->second;
    }
    return EventType::kEventNone;
}

std::string EventUtils::EventTypeToString(EventType eventType)
{
    static std::unordered_map<EventType, std::string> typeMap;
    InitEventStringMap(&typeMap, nullptr);
    auto iter = typeMap.find(eventType);
    ASSERT(iter != typeMap.end());
    if (iter != typeMap.end()) {
        return iter->second;
    }
    return std::string();
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
