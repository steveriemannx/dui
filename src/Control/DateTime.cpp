#include "dui/Control/DateTime.h"
#include "dui/Core/Window.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Utils/AttributeUtil.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Control/CalendarFlyout.h"
#include "dui/Control/Calendar.h"
#include <sstream>
#include <iomanip>

#include "dui/Control/DateTimeWnd.h"

namespace ui
{
DateTime::DateTime(Window* pWindow):
    LabelTemplate<HBox>(pWindow),
    m_dateTime({0,}),
    m_pDateWindow(nullptr),
    m_pCalendarFlyout(nullptr),
    m_bSuppressCalendarReopen(false),
    m_editFormat(EditFormat::kDateCalendar),
    m_dateSeparator('-')
{
    //Set the default attributes
    SetAttribute("border_size", "1");
    SetAttribute("border_color", "border_richedit_normal");
    SetAttribute("text_align", "vcenter");
    SetAttribute("text_padding", "2,0,0,0");
    SetAttribute("padding", "1,1,1,1");
    SetAttribute("spin_class", "rich_edit_spin_box,rich_edit_spin_btn_up,rich_edit_spin_btn_down");
}

DateTime::~DateTime()
{
    //Release any leftover DateTimeWnd object (for example when the control is destroyed before editing ends),
    //avoiding an object leak and a dangling reference. Deleting nullptr is safe when m_pDateWindow is empty.
    if (m_pDateWindow != nullptr) {
        DateTimeWnd* pWnd = m_pDateWindow.get();
        m_pDateWindow = nullptr;
        delete pWnd;
    }
}

std::string DateTime::GetType() const { return DUI_CTR_DATETIME; }

void DateTime::SetAttribute(const std::string& strName, const std::string& strValue2)
{
    //dui does not support variable expansion in attribute values
    const std::string& strValue = strValue2;
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
    else if (strName == "current_time") {
        //Initialize to the current local time for convenience: there is no need to call InitLocalTime() manually
        //Enabled when the value is "true" or "1" (dui has no StringUtil::IsValueTrue)
        if ((strValue == "true") || (strValue == "1")) {
            InitLocalTime();
        }
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

void DateTime::ResetCalendarFlyoutState()
{
    m_bSuppressCalendarReopen = false;
}

void DateTime::CloseCalendarFlyout()
{
    if (m_pCalendarFlyout != nullptr) {
        //Close the flyout (the internal close callback clears m_pCalendarFlyout)
        m_pCalendarFlyout->Dismiss();
    }
}

const struct tm& DateTime::GetDateTime() const
{
    return m_dateTime;
}

void DateTime::SetDateTime(const struct tm& dateTime)
{
    if (!IsEqual(m_dateTime, dateTime)) {
        m_dateTime = dateTime;

        //Update the display text
        SetText(GetDateTimeString());
        //Fire the value-changed event
        SendEvent(kEventValueChanged);
    }
}

std::string DateTime::GetDateTimeString() const
{
    std::string dateTime;
    if (IsValidDateTime()) {
        struct tm tmSystemDate = m_dateTime;
#ifdef DUI_UNICODE
        std::wstringstream ss;
#else
        std::stringstream ss;
#endif
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
#ifdef DUI_UNICODE
    std::wistringstream ss(dateTime);
#else
    std::istringstream ss(dateTime);
#endif
    ss >> std::get_time(&t, sFormat.c_str());
    if (ss.fail()) {
        //If that fails, detect the separator between year, month and day automatically
        if (dateTime.find('-') != std::string::npos) {
            StringUtil::ReplaceAll("/", "-", sFormat);
#ifdef DUI_UNICODE
            std::wistringstream ss2(dateTime);
#else
            std::istringstream ss2(dateTime);
#endif
            ss2 >> std::get_time(&t, sFormat.c_str());
            if (!ss2.fail()) {
                m_dateTime = t;
                bRet = true;
                m_dateSeparator = '-';
            }
        }
        else if (dateTime.find('/') != std::string::npos) {
            StringUtil::ReplaceAll("-", "/", sFormat);
#ifdef DUI_UNICODE
            std::wistringstream ss2(dateTime);
#else
            std::istringstream ss2(dateTime);
#endif
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
        //If the text contains no date part, update it to today's value; otherwise editing treats it as an invalid date
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

        //Update the display text
        SetText(GetDateTimeString());
        //Fire the value-changed event
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
            //Update the display text
            SetText(GetDateTimeString());
            //Fire the value-changed event
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
        //Mouse and keyboard messages are forwarded to the parent while the control is disabled
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
        if (m_pDateWindow != nullptr || m_pCalendarFlyout != nullptr) {
            return;
        }
    }
    else if (msg.eventType == kEventScrollPosChanged) {
        if (m_pDateWindow != nullptr || m_pCalendarFlyout != nullptr) {
            return;
        }
    }
    else if (msg.eventType == kEventSetFocus) {
        if (m_pDateWindow != nullptr || m_pCalendarFlyout != nullptr) {
            return;
        }
        if (m_bSuppressCalendarReopen) {
            //The calendar flyout just closed, so the SetFocus triggered by focus restoration must not reopen it
            //(several SetFocus messages may arrive in a row; the flag is cleared only on KillFocus/MouseButtonDown)
            return;
        }
        if (GetRect().IsZero() && (GetWindow() != nullptr)) {
            //Not shown yet: update the window once so the control position is determined before the edit window is shown
            GetWindow()->UpdateWindow();
        }
        if (IsFocused() && IsEnabled()) {
            if (GetEditFormat() == EditFormat::kDateCalendar) {
                //Use the new CalendarFlyout popup
                m_pCalendarFlyout = new CalendarFlyout(GetWindow());
                m_pCalendarFlyout->AttachDateSelected([this](WPARAM wParam, LPARAM lParam) {
                    if (wParam == 0) {
                        time_t t = (time_t)lParam;
                        struct tm date = Calendar::TimeTToDate(t);
                        SetDateTime(date);
                    }
                });
                m_pCalendarFlyout->AttachDateCleared([this]() {
                    ClearTime();
                });
                m_pCalendarFlyout->AttachClosed([this](ui::Flyout::CloseReason) {
                    m_pCalendarFlyout = nullptr;
                    //After the flyout closes, focus returning to this control triggers SetFocus, so reopening must be suppressed
                    m_bSuppressCalendarReopen = true;
                    return true;
                });
                struct tm initDate = IsValidDateTime() ? m_dateTime : Calendar::GetToday();
                if (!m_pCalendarFlyout->ShowAt(this, initDate, ui::Flyout::Placement::Bottom)) {
                    //When creation fails, ShowAt has already destroyed the object internally, so clear it to avoid a dangling pointer
                    m_pCalendarFlyout = nullptr;
                }
            }
            else {
                //Other formats use the unified DateTimeWnd implementation
                m_pDateWindow = new DateTimeWnd(this);
                if (m_pDateWindow->Init(this)) {
                    m_pDateWindow->ShowWindow();
                }
                else {
                    delete m_pDateWindow.get();
                    m_pDateWindow = nullptr;
                }
            }
        }
    }
    else if (msg.eventType == kEventKillFocus) {
        m_bSuppressCalendarReopen = false;
        Invalidate();
    }
    else if ((msg.eventType == kEventMouseButtonDown) ||
             (msg.eventType == kEventMouseDoubleClick) ||
             (msg.eventType == kEventMouseRButtonDown)) {
        if (GetWindow() != nullptr) {
            GetWindow()->ReleaseCapture();
        }
        //The user clicked explicitly, so the suppress flag is cleared
        m_bSuppressCalendarReopen = false;
        if (IsFocused() && IsEnabled() && (m_pDateWindow == nullptr) && (m_pCalendarFlyout == nullptr)) {
            if (GetEditFormat() == EditFormat::kDateCalendar) {
                //Use the new CalendarFlyout popup
                m_pCalendarFlyout = new CalendarFlyout(GetWindow());
                m_pCalendarFlyout->AttachDateSelected([this](WPARAM wParam, LPARAM lParam) {
                    if (wParam == 0) {
                        //Single selection mode
                        time_t t = (time_t)lParam;
                        struct tm date = Calendar::TimeTToDate(t);
                        SetDateTime(date);
                    }
                });
                m_pCalendarFlyout->AttachDateCleared([this]() {
                    ClearTime();
                });
                m_pCalendarFlyout->AttachClosed([this](ui::Flyout::CloseReason) {
                    m_pCalendarFlyout = nullptr;
                    //The SetFocus triggered by focus restoration after the flyout closes must not reopen it
                    m_bSuppressCalendarReopen = true;
                    return true;
                });
                struct tm initDate = IsValidDateTime() ? m_dateTime : Calendar::GetToday();
                if (!m_pCalendarFlyout->ShowAt(this, initDate, ui::Flyout::Placement::Bottom)) {
                    //When creation fails, ShowAt has already destroyed the object internally, so clear it to avoid a dangling pointer
                    m_pCalendarFlyout = nullptr;
                }
            }
            else {
                //Other formats use the unified DateTimeWnd implementation
                m_pDateWindow = new DateTimeWnd(this);
                if (m_pDateWindow->Init(this)) {
                    m_pDateWindow->ShowWindow();
                }
                else {
                    delete m_pDateWindow.get();
                    m_pDateWindow = nullptr;
                }
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
        //Convert the displayed text content into the date time format
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
            //Focus moved to a child control; no KillFocus event is sent
            return;
        }
    }
    BaseClass::SendEventMsg(msg);
}

void DateTime::EndEditDateTime()
{
    //When editing ends, the DateTimeWnd object must be reset and released:
    //DateTimeWnd is held only through a weak reference in m_pDateWindow; without resetting it, m_pDateWindow stays non-null and
    //which makes later clicks hit the "m_pDateWindow != nullptr, return directly" branch in HandleEvent,
    SendEvent(kEventKillFocus);
    if (m_pDateWindow != nullptr) {
        //and editing can never start again (after editing once, clicking can no longer edit the date).
        DateTimeWnd* pWnd = m_pDateWindow.get();
        m_pDateWindow = nullptr;
        delete pWnd;
    }
}

UiSize DateTime::EstimateText(UiSize szAvailable)
{
    //When no date is set (the display text is empty), estimate with a sample string as wide as the date format,
    //so that a width/height of auto is not estimated as 0 and the control does not collapse;
    //When a date is set, estimate with the real date text directly.
    std::string text = GetText();
    UiSize size;
    if (text.empty()) {
        std::string sample = GetSampleDateTimeString();
        if (!sample.empty()) {
            size = BaseClass::EstimateTextWith(sample, szAvailable);
        }
        else {
            size = BaseClass::EstimateText(szAvailable);
        }
    }
    else {
        size = BaseClass::EstimateText(szAvailable);
    }

    //When spin buttons are supported (spin_class is not empty), reserve the width of the spin-button container,
    //so a DateTime with width=auto also leaves room for the edit-mode spin buttons in the display state,
    //avoiding the spin buttons being squeezed or shown incompletely in edit mode.
    if (GetEditFormat() != EditFormat::kDateCalendar) {
        std::string spinClass = GetSpinClass();
        if (!spinClass.empty()) {
            int32_t nSpinWidth = GetSpinBoxWidth(spinClass);
            if (nSpinWidth > 0) {
                size.cx += nSpinWidth;
            }
        }
    }
    return size;
}

int32_t DateTime::GetSpinBoxWidth(const std::string& spinClass) const
{
    //spin_class format: "spin container class, up button class, down button class"; the width of the first segment (the container) is used
    std::list<std::string> classNames = StringUtil::Split(spinClass, ",");
    if (classNames.empty()) {
        return 0;
    }
    std::string spinBoxClass = classNames.front();
    StringUtil::Trim(spinBoxClass);
    if (spinBoxClass.empty()) {
        return 0;
    }

    //Parse the width attribute of the spin container class (same as Control::SetClass: look up the global class first, then the window class)
    std::string classAttributes = GlobalManager::Instance().GetClassAttributes(spinBoxClass);
    if (classAttributes.empty() && (GetWindow() != nullptr)) {
        classAttributes = GetWindow()->GetClassAttributes(spinBoxClass);
    }
    if (classAttributes.empty()) {
        return 0;
    }

    //dui's ParseAttributeList takes the quote character and returns void
    std::vector<std::pair<std::string, std::string>> attributeList;
    AttributeUtil::ParseAttributeList(classAttributes, '"', attributeList);
    if (attributeList.empty()) {
        AttributeUtil::ParseAttributeList(classAttributes, '\'', attributeList);
    }
    if (attributeList.empty()) {
        return 0;
    }

    int32_t nWidth = 0;
    for (const auto& attr : attributeList) {
        if (attr.first == "width") {
            std::string value = attr.second;
            StringUtil::Trim(value);
            //only a fixed pixel width is supported; auto / percentage / stretch cannot be determined statically while estimating, so they are ignored
            if (!value.empty() && (value != "auto") &&
                (value.find("%") == std::string::npos) &&
                (value.find("stretch") == std::string::npos)) {
                int32_t nRawWidth = StringUtil::StringToInt32(value);
                if (nRawWidth > 0) {
                    nWidth = Dpi().GetScaleInt(nRawWidth);
                }
            }
            break;  //Stop once the width attribute is found (whether or not it is usable)
        }
    }
    return nWidth;
}

std::string DateTime::GetSampleDateTimeString() const
{
    //use a fixed date (2000-01-01 00:00:00) to generate a sample string as wide as a real date,
    //It is used only for the size estimate and never becomes the actual display text.
    struct tm t = { 0, };
    t.tm_year = 100;   //Year 2000
    t.tm_mon  = 0;     //January
    t.tm_mday = 1;
    t.tm_hour = 0;
    t.tm_min  = 0;
    t.tm_sec  = 0;
    t.tm_wday = 6;     //2000-01-01 is a Saturday (used only by %a/%A/%w and similar formats)
    t.tm_yday = 0;
    t.tm_isdst = -1;   //Let mktime determine daylight saving time automatically
    std::string sFormat = GetStringFormat();
    if (sFormat.empty()) {
        return std::string();
    }
#ifdef DUI_UNICODE
    std::wstringstream ss;
#else
    std::stringstream ss;
#endif
    ss << std::put_time(&t, sFormat.c_str());
    return ss.str();
}

}//namespace ui
