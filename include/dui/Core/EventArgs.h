#ifndef UI_CORE_EVENTARGS_H_
#define UI_CORE_EVENTARGS_H_

#include "dui/Core/UiPoint.h"
#include "dui/Core/Keycode.h"
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace ui
{
class Control;
class WeakFlag;

/** The parameters of the control event notification
*/
struct DUI_API EventArgs
{
public:
    /** The event type
    */
    EventType eventType;

    /** Parameter 1 when the event is generated (can be used to pass a pointer)
    */
    WPARAM wParam;

    /** Parameter 2 when the event is generated (can be used to pass a pointer)
    */
    LPARAM lParam;

    /** The key associated with the message
    */
    VirtualKeyCode vkCode;

    /** The mouse coordinates associated with the message
    */
    UiPoint ptMouse;

    /** The key modifier flags associated with the message, see the enum ModifierKey definition in Keyboard.h
    */
    uint32_t modifierKey;

    /** The integer data associated with the message
    */
    int32_t eventData;

    /** The pointer data associated with the message
    */
    void* pEventData;

    /** Data dedicated to the ListCtrl control, used to indicate the view type corresponding to the event
    */
    int32_t listCtrlType;

public:
    /** Constructor
    */
    EventArgs();

    /** Set the control that sends the event
    */
    void SetSender(Control* pControl);

    /** Get the control that sends the event
    */
    Control* GetSender() const;

    /** Set the WeakFlag of the sender (used when the Sender is not set)
    */
    void SetSenderWeakFlag(std::weak_ptr<WeakFlag> senderFlag);

    /** Check whether the control that sends the event has expired
    */
    bool IsSenderExpired() const;

private:
    /** The control that sends the event
    */
    Control* pSender;

    /** The lifecycle flag of the control
    */
    std::weak_ptr<WeakFlag> m_senderFlag;
};

/** The prototype definition of the event callback function
*/
typedef std::function<bool (const ui::EventArgs&)> EventCallback;

/** The ID of the event callback function
*/
typedef size_t EventCallbackID;

/** The container of the event callback functions
*/
class DUI_API EventSource
{
public:
    /** Add a callback function
    * @param [in] callback The callback function
    * @param [in] callbackID The ID corresponding to the callback function (may not be unique)
    */
    void AddEventCallback(const EventCallback& callback, EventCallbackID callbackID);

    /** Remove the callback function (may correspond to multiple callback functions)
    * @param [in] callbackID The ID corresponding to the callback function (used to remove the callback function), must be greater than 0
    * @return Returns true if the callback function was removed; otherwise returns false, and returns false when no function matches the ID
    */
    bool RemoveEventCallbackByID(EventCallbackID callbackID);

    /** Check whether the callback function exists
    * @param [in] callbackID The ID corresponding to the callback function
    */
    bool HasEventCallbackByID(EventCallbackID callbackID) const;

    /** Whether the container is empty
    */
    bool IsEmpty() const;

    /** Trigger the callback event (corresponding to each callback function in the container)
     * @param [in] args The parameters of the callback function
     */
    bool operator() (const ui::EventArgs& args) const;

private:
    /** The data corresponding to the event
    */
    struct EventCallbackData
    {
        //The callback function
        EventCallback m_callback;

        //The ID corresponding to the callback function, used to remove the callback
        EventCallbackID m_callbackID;
    };

private:
    /** The container of the event callbacks
    */
    std::vector<EventCallbackData> m_callbackList;
};

/** The map container of event type callbacks
*/
typedef std::unordered_map<EventType, EventSource> EventMap;

/** Helper functions
*/
class DUI_API EventUtils
{
public:
    /** Remove the event callback function with the specified ID from the event type callback map container
    * @param [in] eventMap The container to operate on
    * @param [in] callbackID The ID of the callback function, must be greater than 0
    */
    static bool RemoveEventCallbackByID(EventMap& eventMap, EventCallbackID callbackID);

    /** Remove the event callback function with the specified ID from the event type callback map container
    * @param [in] eventMap The container to operate on
    * @param [in] eventType The event type
    * @param [in] callbackID The ID of the callback function, must be greater than 0
    */
    static bool RemoveEventCallbackByID(EventMap& eventMap, EventType eventType, EventCallbackID callbackID);

    /** Check whether the event type callback map container contains the event callback function with the specified ID
    * @param [in] eventMap The container to operate on
    * @param [in] callbackID The ID of the callback function, must be greater than 0
    */
    static bool HasEventCallbackByID(const EventMap& eventMap, EventCallbackID callbackID);

    /** Check whether the event type callback map container contains the event callback function with the specified ID
    * @param [in] eventMap The container to operate on
    * @param [in] eventType The event type
    * @param [in] callbackID The ID of the callback function, must be greater than 0
    */
    static bool HasEventCallbackByID(const EventMap& eventMap, EventType eventType, EventCallbackID callbackID);

    /** Convert a string to an event type
    */
    static EventType StringToEventType(const DString& eventName);

    /** Convert an event type to a string
    */
    static DString EventTypeToString(EventType eventType);
};

}// namespace ui

#endif // UI_CORE_EVENTARGS_H_
