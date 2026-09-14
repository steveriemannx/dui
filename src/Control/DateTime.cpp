#include "dui/Control/DateTime.h"
#include "dui/Core/Window.h"
#include <sstream>
#include <iomanip>

#if defined (DUI_BUILD_FOR_WIN)
    #include "dui/Control/DateTimeWnd_Windows.h"
#elif defined (DUI_BUILD_FOR_WAYLAND) || defined (DUI_BUILD_FOR_X11)
    #include "dui/Control/DateTimeWnd_Native.h"
#elif defined (DUI_BUILD_FOR_MACOS)
    #include "dui/Control/DateTimeWnd_MacOS.h"
#endif

namespace ui
{
DateTime::DateTime(Window* pWindow):
    LabelTemplate<HBox>(pWindow),
    m_dateTime({0,}),
    m_pDateWindow(nullptr),
    m_editFormat(EditFormat::kDateCalendar),
    m_dateSeparator('-')
{
    //Set the default attributes
    SetAttribute("border_size", "1");
    SetAttribute("border_color", "gray");
    SetAttribute("text_align", "vcenter");
    SetAttribute("text_padding", "2,0,0,0");
#if defined(DUI_BUILD_FOR_WAYLAND) || defined(DUI_BUILD_FOR_X11)
    SetAttribute("padding", "1,1,1,1");
    SetAttribute("spin_class", "rich_edit_spin_box,rich_edit_spin_btn_up,rich_edit_spin_btn_down");
#endif
}

DateTime::~DateTime()
{
}

std::string DateTime::GetType() const { return DUI_CTR_DATETIME; }

void DateTime::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if (strName == "format") {
        SetStringFormat(strValue);
    }
    else if (strName == "edit_format") {
        if (strValue == "date_calendar") {
            SetEditFormat(EditFormat::kDateCalendar);
        }
        else if (strValue == "date_up_down") {
            SetEditFormat(EditFormat::kDateUpDown);
        }
        else if (strValue == "date_time_up_down") {
            SetEditFormat(EditFormat::kDateTimeUpDown);
        }
        else if (strValue == "date_minute_up_down") {
            SetEditFormat(EditFormat::kDateMinuteUpDown);
        }
        else if (strValue == "time_up_down") {
            SetEditFormat(EditFormat::kTimeUpDown);
        }
        else if (strValue == "minute_up_down") {
            SetEditFormat(EditFormat::kMinuteUpDown);
        }
        else {
            ASSERT(0);
        }
    }
    else if (strName == "spin_class") {
        SetSpinClass(strValue);
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void DateTime::InitLocalTime()
{
    time_t timeNow = std::time(nullptr);
    struct tm dateTime = {0, };
#if defined (_WIN32) || defined (_WIN64)
    ::localtime_s(&dateTime, &timeNow);
#else
    ::localtime_r(&timeNow, &dateTime);
#endif
    SetDateTime(dateTime);
}

void DateTime::ClearTime()
{
    struct tm dateTime = { 0, };
    SetDateTime(dateTime);
}

const struct tm& DateTime::GetDateTime() const
{
    return m_dateTime;
}

void DateTime::SetDateTime(const struct tm& dateTime)
{
    if (!IsEqual(m_dateTime, dateTime)) {
        m_dateTime = dateTime;

        //Update the displayed text
        SetText(GetDateTimeString());
        //Trigger the value change event
        SendEvent(kEventValueChanged);
    }
}

std::string DateTime::GetDateTimeString() const
{
    std::string dateTime;
    if (IsValidDateTime()) {
        struct tm tmSystemDate = m_dateTime;
        std::stringstream ss;
        ss << std::put_time(&tmSystemDate, GetStringFormat().c_str());
        dateTime = ss.str();
    }
    return dateTime;
}

bool DateTime::SetDateTimeString(const std::string& dateTime)
{
    bool bRet = false;
    std::string sFormat = GetStringFormat();
    ASSERT(!sFormat.empty());
    struct tm t = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
    std::istringstream ss(dateTime);
    ss >> std::get_time(&t, sFormat.c_str());
    if (ss.fail()) {
        //After failure, intelligently recognize the separators of the year, month and day
        if (dateTime.find('-') != std::string::npos) {
            StringUtil::ReplaceAll("/", "-", sFormat);
            std::istringstream ss2(dateTime);
            ss2 >> std::get_time(&t, sFormat.c_str());
            if (!ss2.fail()) {
                m_dateTime = t;
                bRet = true;
                m_dateSeparator = '-';
            }
        }
        else if (dateTime.find('/') != std::string::npos) {
            StringUtil::ReplaceAll("-", "/", sFormat);
            std::istringstream ss2(dateTime);
            ss2 >> std::get_time(&t, sFormat.c_str());
            if (!ss2.fail()) {
                m_dateTime = t;
                bRet = true;
                m_dateSeparator = '/';
            }
        }
    }
    else {
        m_dateTime = t;
        bRet = true;
    }
    if (bRet) {
        //If the year, month and day are not included, update them to today's values; otherwise the date is considered invalid when editing
        time_t timeNow = std::time(nullptr);
        struct tm tmTime = { 0, };
#if defined (_WIN32) || defined (_WIN64)
        ::localtime_s(&tmTime, &timeNow);
#else
        ::localtime_r(&timeNow, &tmTime);
#endif
        if (m_dateTime.tm_year < 0) {
            m_dateTime.tm_year = tmTime.tm_year;
        }        
        if (m_dateTime.tm_mon < 0) {
            m_dateTime.tm_mon = tmTime.tm_mon;
        }
        if (m_dateTime.tm_mday < 0) {
            m_dateTime.tm_mday = tmTime.tm_mday;
        }
        if (m_dateTime.tm_hour < 0) {
            m_dateTime.tm_hour = tmTime.tm_hour;
        }
        if (m_dateTime.tm_min < 0) {
            m_dateTime.tm_min = tmTime.tm_min;
        }
        if (m_dateTime.tm_sec < 0) {
            m_dateTime.tm_sec = tmTime.tm_sec;
        }
        time_t timeValue = std::mktime(&m_dateTime);
        ASSERT(timeValue != 0);
        if (timeValue != 0) {
#if defined (_WIN32) || defined (_WIN64)
            ::localtime_s(&m_dateTime, &timeValue);
#else
            ::localtime_r(&timeValue, &m_dateTime);
#endif
        }
    }
    ASSERT(bRet);
    return bRet;
}

bool DateTime::IsEqual(const struct tm& a, const struct tm& b) const
{
    if (a.tm_sec == b.tm_sec   &&
        a.tm_min == b.tm_min   &&
        a.tm_hour == b.tm_hour &&
        a.tm_mday == b.tm_mday &&
        a.tm_mon == b.tm_mon   &&
        a.tm_year == b.tm_year &&
        a.tm_wday == b.tm_wday &&
        a.tm_yday == b.tm_yday &&
        a.tm_isdst == b.tm_isdst) {
        return true;
    }
    return false;
}

bool DateTime::IsValidDateTime() const
{
    const struct tm& a = m_dateTime;
    if (a.tm_sec == 0  &&
        a.tm_min == 0  &&
        a.tm_hour == 0 &&
        a.tm_mday == 0 &&
        a.tm_mon == 0  &&
        a.tm_year == 0 &&
        a.tm_wday == 0 &&
        a.tm_yday == 0 &&
        a.tm_isdst == 0) {
        return false;
    }
    return true;
}

void DateTime::SetStringFormat(const std::string& sFormat)
{
    if (!IsInited()) {
        m_sFormat = sFormat;
    }
    else if (m_sFormat != sFormat) {
        m_sFormat = sFormat;

        //Update the displayed text
        SetText(GetDateTimeString());
        //Trigger the value change event
        SendEvent(kEventValueChanged);
    }
}

std::string DateTime::GetStringFormat() const
{
    std::string sFormat = m_sFormat.c_str();
    if (sFormat.empty()) {
        EditFormat editFormat = GetEditFormat();
        switch (editFormat) {
        case EditFormat::kDateCalendar:
        case EditFormat::kDateUpDown:
            sFormat = "%Y-%m-%d";
            break;
        case EditFormat::kDateTimeUpDown:
            sFormat = "%Y-%m-%d %H:%M:%S";
            break;
        case EditFormat::kDateMinuteUpDown:
            sFormat = "%Y-%m-%d %H:%M";
            break;
        case EditFormat::kTimeUpDown:
            sFormat = "%H:%M:%S";
            break;
        case EditFormat::kMinuteUpDown:
            sFormat = "%H:%M";
            break;
        default:
            sFormat = "%Y-%m-%d";
            break;
        }
        if (m_dateSeparator != '-') {
            std::string separator;
            separator = m_dateSeparator;
            StringUtil::ReplaceAll("-", separator, sFormat);
        }        
    }
    return sFormat;
}

void DateTime::SetEditFormat(EditFormat editFormat)
{
    if (!IsInited()) {
        m_editFormat = editFormat;
    }
    else if (m_editFormat != editFormat) {
        std::string oldFormat = GetStringFormat();
        m_editFormat = editFormat;
        if (oldFormat != GetStringFormat()) {
            //Update the displayed text
            SetText(GetDateTimeString());
            //Trigger the value change event
            SendEvent(kEventValueChanged);
        }
    }
}

DateTime::EditFormat DateTime::GetEditFormat() const
{
    return m_editFormat;
}

std::string::value_type DateTime::GetDateSeparator() const
{
    return m_dateSeparator;
}

void DateTime::UpdateEditWndPos()
{
    if (m_pDateWindow != nullptr) {
        m_pDateWindow->UpdateWndPos();
    }
}

void DateTime::HandleEvent(const EventArgs& msg)
{
    if (IsDisabledEvents(msg)) {
        //If it is a mouse or keyboard message and the control is disabled, forward it to the parent control
        Box* pParent = GetParent();
        if (pParent != nullptr) {
            pParent->SendEventMsg(msg);
        }
        else {
            BaseClass::HandleEvent(msg);
        }
        return;
    }
    if ((msg.eventType == kEventSetCursor)) {
        SetCursor(CursorType::kCursorIBeam);
        return;
    }
    else if (msg.eventType == kEventWindowSize) {
        if (m_pDateWindow != nullptr) {
            return;
        }
    }
    else if (msg.eventType == kEventScrollPosChanged) {
        if (m_pDateWindow != nullptr) {
            return;
        }
    }
    else if (msg.eventType == kEventSetFocus) {
        if (m_pDateWindow != nullptr) {
            return;
        }
        if (GetRect().IsZero() && (GetWindow() != nullptr)) {
            //Not yet displayed; refresh the window once to ensure the control position is determined first, then show the edit window
            GetWindow()->UpdateWindow();
        }
        if (IsFocused() && IsEnabled()) {
            m_pDateWindow = new DateTimeWnd(this);
            if (m_pDateWindow->Init(this)) {
                m_pDateWindow->ShowWindow();
            }
            else {
                delete m_pDateWindow;
                m_pDateWindow = nullptr;
            }
        }
    }
    else if (msg.eventType == kEventKillFocus) {
        Invalidate();
    }
    else if ((msg.eventType == kEventMouseButtonDown) ||
             (msg.eventType == kEventMouseDoubleClick) ||
             (msg.eventType == kEventMouseRButtonDown)) {
        if (GetWindow() != nullptr) {
            GetWindow()->ReleaseCapture();
        }
        if (IsFocused() && IsEnabled() && (m_pDateWindow == nullptr)) {
            m_pDateWindow = new DateTimeWnd(this);
        }
        if (m_pDateWindow != nullptr) {
            if (m_pDateWindow->Init(this)) {
                m_pDateWindow->ShowWindow();
            }
            else {
                delete m_pDateWindow;
                m_pDateWindow = nullptr;
            }
        }
    }
    else if (msg.eventType == kEventMouseMove) {
        return;
    }
    else if (msg.eventType == kEventMouseButtonUp) {
        return;
    }
    else if (msg.eventType == kEventContextMenu) {
        return;
    }
    else if (msg.eventType == kEventMouseEnter) {
        return;
    }
    else if (msg.eventType == kEventMouseLeave) {
        return;
    }
    else {
        BaseClass::HandleEvent(msg);
    }
}

void DateTime::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();

    if (!IsValidDateTime()) {
        std::string text = GetText();
        //Convert the displayed text into a date time format
        if (!text.empty()) {
            SetDateTimeString(text);
        }
    }
}

void DateTime::SetSpinClass(const std::string& spinClass)
{
    m_spinClass = spinClass;
}

std::string DateTime::GetSpinClass() const
{
    return m_spinClass.c_str();
}

void DateTime::SendEventMsg(const EventArgs& msg)
{
    if ((msg.GetSender() == this) && (msg.eventType == kEventKillFocus)) {
        Control* pNewFocus = (Control*)msg.wParam;
        if ((pNewFocus != nullptr) && (GetItemIndex(pNewFocus) != Box::InvalidIndex)) {
            //The focus switches to a child control; do not send the KillFocus event
            return;
        }
    }
    BaseClass::SendEventMsg(msg);
}

void DateTime::EndEditDateTime()
{
    SendEvent(kEventKillFocus);
}

}//namespace ui
