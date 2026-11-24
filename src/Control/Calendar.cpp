#include "dui/Control/Calendar.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Window.h"
#include "dui/Core/Keycode.h"
#include "dui/Core/Keyboard.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/StringConvert.h"
#include <ctime>
#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace ui
{

// 7 days per row, 6 rows in total
static const int32_t kDaysPerWeek = 7;
static const int32_t kWeeksPerMonth = 6;
static const int32_t kDaysPerPage = kDaysPerWeek * kWeeksPerMonth; // 42

// Year view/ten-year view: 3 columns x 4 rows
static const int32_t kYearGridCols = 3;
static const int32_t kYearGridRows = 4;

Calendar::Calendar(Window* pWindow):
    Control(pWindow),
    m_mode(Mode::kSingle),
    m_displayYear(0),
    m_displayMonth(0),
    m_viewMode(0),
    m_firstDayOfWeek(1), // Monday by default
    m_bHasSelectedDate(false),
    m_bHasRangeStart(false),
    m_bHasRangeEnd(false),
    m_bRangeDragging(false),
    m_bRangeAwaitingEnd(false),
    m_bRangeKeyboardExtending(false),
    m_hoverYear(0),
    m_hoverMonth(0),
    m_hoverDay(0),
    m_bHasHover(false),
    m_bHasMinDate(false),
    m_bHasMaxDate(false),
    m_bCellsDirty(true),
    m_bTodayValid(false),
    m_focusDate{0},
    m_bHasFocusDate(false)
{
    // Initialize the displayed date to the current year and month
    m_today = GetToday();
    m_bTodayValid = true;
    m_displayYear = m_today.tm_year + 1900;
    m_displayMonth = m_today.tm_mon + 1;

    // Default size
    SetFixedWidth(UiFixedInt(280), false, true);
    SetFixedHeight(UiFixedInt(240), false, true);
}

std::string Calendar::GetType() const { return DUI_CTR_CALENDAR; }

void Calendar::SetAttribute(const std::string& strName, const std::string& strValue2)
{
    std::string strValue = strValue2;
    if (strName == "calendar_mode") {
        if (strValue == "range") {
            SetMode(Mode::kRange);
        }
        else {
            SetMode(Mode::kSingle);
        }
    }
    else if (strName == "first_day_of_week") {
        if (strValue == "sunday") {
            SetFirstDayOfWeek(0);
        }
        else {
            SetFirstDayOfWeek(1);
        }
    }
    else if (strName == "min_date") {
        struct tm date = {0, };
        if (ParseDateString(strValue, date)) {
            m_minDate = date;
            m_bHasMinDate = true;
            m_bCellsDirty = true;   // The selectable range changed, bDisabled must be recomputed
        }
    }
    else if (strName == "max_date") {
        struct tm date = {0, };
        if (ParseDateString(strValue, date)) {
            m_maxDate = date;
            m_bHasMaxDate = true;
            m_bCellsDirty = true;   // The selectable range changed, bDisabled must be recomputed
        }
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

void Calendar::SetMode(Mode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        ClearSelection();
        Invalidate();
    }
}

void Calendar::SetDisplayMonth(int32_t year, int32_t month)
{
    if (month < 1) {
        month = 1;
    }
    if (month > 12) {
        month = 12;
    }
    if (m_displayYear != year || m_displayMonth != month) {
        m_displayYear = year;
        m_displayMonth = month;
        m_bCellsDirty = true;
        Invalidate();
        // Notify the flyout to refresh the title: the display period (month/year/ten-year) has changed, so the title text must be updated.
        SendEvent(kEventDisplayDateChanged);
    }
}

void Calendar::SetDate(const struct tm& date)
{
    if (date.tm_year <= 0 || date.tm_mon < 0 || date.tm_mday <= 0) {
        return;
    }
    m_selectedDate = date;
    m_bHasSelectedDate = true;
    m_bHasRangeStart = false;
    m_bHasRangeEnd = false;
    m_bCellsDirty = true;   // The selected state changed, bSelected of the cells must be recomputed
    Invalidate();

    // Fire the event
    EmitDateChanged(false);
}

void Calendar::SetDateRange(const struct tm& start, const struct tm& end)
{
    if (start.tm_year <= 0 || start.tm_mon < 0 || start.tm_mday <= 0) {
        return;
    }
    if (end.tm_year <= 0 || end.tm_mon < 0 || end.tm_mday <= 0) {
        return;
    }
    if (IsDateLess(end, start)) {
        m_rangeStart = end;
        m_rangeEnd = start;
    }
    else {
        m_rangeStart = start;
        m_rangeEnd = end;
    }
    m_bHasRangeStart = true;
    m_bHasRangeEnd = true;
    m_bHasSelectedDate = false;
    m_bCellsDirty = true;   // The range changed, bSelected/bInRange of the cells must be recomputed
    Invalidate();

    // Fire the event
    EmitDateChanged(true);
}

void Calendar::GetDateRange(struct tm& start, struct tm& end) const
{
    start = m_rangeStart;
    end = m_rangeEnd;
}

bool Calendar::HasSelection() const
{
    return m_bHasSelectedDate || (m_bHasRangeStart && m_bHasRangeEnd);
}

void Calendar::ClearSelection()
{
    m_bHasSelectedDate = false;
    m_bHasRangeStart = false;
    m_bHasRangeEnd = false;
    m_bRangeDragging = false;
    m_bRangeAwaitingEnd = false;
    m_bRangeKeyboardExtending = false;
    m_bCellsDirty = true;   // Selection/range cleared, bSelected/bInRange of the cells must be recomputed
    Invalidate();
}

void Calendar::SetFirstDayOfWeek(int32_t dayOfWeek)
{
    if (dayOfWeek < 0 || dayOfWeek > 6) {
        return;
    }
    if (m_firstDayOfWeek != dayOfWeek) {
        m_firstDayOfWeek = dayOfWeek;
        m_bCellsDirty = true;
        Invalidate();
    }
}

void Calendar::SetDateLimit(const std::string& minDate, const std::string& maxDate)
{
    m_bHasMinDate = ParseDateString(minDate, m_minDate);
    m_bHasMaxDate = ParseDateString(maxDate, m_maxDate);
    m_bCellsDirty = true;   // The selectable range changed, bDisabled must be recomputed
    Invalidate();
}

void Calendar::RefreshToday()
{
    // The "today" cache must be refreshed when the system date changes, otherwise the highlight/GoToToday keeps the old date after midnight
    struct tm today = GetToday();
    if (m_bTodayValid && IsSameDay(today, m_today)) {
        return;
    }
    m_today = today;
    m_bTodayValid = true;
    m_bCellsDirty = true;   // bToday of the date cells must be recomputed
}

void Calendar::EmitDateChanged(bool bRange)
{
    if (!bRange) {
        // Single mode: lParam carries time_t directly (fully valid on 64-bit platforms)
        const time_t t = DateToTimeT(m_selectedDate);
        SendEvent(kEventValueChanged, (WPARAM)0, (LPARAM)t);
        return;
    }

    // Range mode: the start and end values are carried through pEventData (DateRange holds the complete 64-bit start/end), so lParam is meaningless.
    // Complete start and end values: a temporary object on the stack, valid only during this synchronous dispatch (the callback must not keep the pointer)
    DateRange range;
    range.start = DateToTimeT(m_rangeStart);
    range.end = DateToTimeT(m_rangeEnd);

    SendEvent(kEventValueChanged, (WPARAM)1, 0, static_cast<void*>(&range));
}

void Calendar::GoToToday()
{
    // Correct the cache before the user explicitly jumps to "today", so that it does not jump to the old date after midnight
    RefreshToday();
    if (m_bTodayValid) {
        SetDisplayMonth(m_today.tm_year + 1900, m_today.tm_mon + 1);
    }
}

void Calendar::NavigatePrev()
{
    if (m_viewMode == 0) {
        // Month view: previous month (clamped to the selectable range, it does not page to a month earlier than minDate)
        int32_t year = m_displayYear;
        int32_t month = m_displayMonth - 1;
        if (month < 1) {
            month = 12;
            --year;
        }
        if (m_bHasMinDate) {
            int32_t minYear = m_minDate.tm_year + 1900;
            int32_t minMonth = m_minDate.tm_mon + 1;
            if (year * 12 + month < minYear * 12 + minMonth) {
                return; // Already at the earliest selectable month, do not page back any further
            }
        }
        SetDisplayMonth(year, month);
    }
    else if (m_viewMode == 1) {
        // Year view: previous year (it does not page to a year earlier than minDate)
        if (m_bHasMinDate && (m_displayYear - 1) < (m_minDate.tm_year + 1900)) {
            return;
        }
        SetDisplayMonth(m_displayYear - 1, m_displayMonth);
    }
    else {
        // Ten-year view: previous ten-year span (it does not page to a span entirely earlier than minDate)
        if (m_bHasMinDate && (m_displayYear - 10) < (m_minDate.tm_year + 1900)) {
            return;
        }
        SetDisplayMonth(m_displayYear - 10, m_displayMonth);
    }
}

void Calendar::NavigateNext()
{
    if (m_viewMode == 0) {
        // Month view: next month (clamped to the selectable range, it does not page to a month later than maxDate)
        int32_t year = m_displayYear;
        int32_t month = m_displayMonth + 1;
        if (month > 12) {
            month = 1;
            ++year;
        }
        if (m_bHasMaxDate) {
            int32_t maxYear = m_maxDate.tm_year + 1900;
            int32_t maxMonth = m_maxDate.tm_mon + 1;
            if (year * 12 + month > maxYear * 12 + maxMonth) {
                return; // Already at the latest selectable month, do not page forward any further
            }
        }
        SetDisplayMonth(year, month);
    }
    else if (m_viewMode == 1) {
        // Year view: next year (it does not page to a year later than maxDate)
        if (m_bHasMaxDate && (m_displayYear + 1) > (m_maxDate.tm_year + 1900)) {
            return;
        }
        SetDisplayMonth(m_displayYear + 1, m_displayMonth);
    }
    else {
        // Ten-year view: next ten-year span (it does not page to a span entirely later than maxDate)
        if (m_bHasMaxDate && (m_displayYear + 10) > (m_maxDate.tm_year + 1900)) {
            return;
        }
        SetDisplayMonth(m_displayYear + 10, m_displayMonth);
    }
}

void Calendar::SetViewMode(int32_t viewMode)
{
    if (viewMode < 0 || viewMode > 2) {
        return;
    }
    if (m_viewMode != viewMode) {
        int32_t oldViewMode = m_viewMode;
        m_viewMode = viewMode;
        m_bCellsDirty = true;
        Invalidate();
        // Notify that the view mode changed (for example drilling down by clicking the grid: year -> month, ten-year -> year), so that the flyout can refresh its title bar.
        SendEvent(kEventViewModeChanged, (WPARAM)viewMode, (LPARAM)oldViewMode);
    }
}

namespace {
// Normalize tm: let mktime fix the out-of-range fields (e.g. February 31 -> March 3);
// hour is set to 12 to avoid daylight saving time boundary issues (consistent with GetMonthCells).
void NormalizeTm(struct tm& t)
{
    t.tm_hour = 12;
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_isdst = -1;
    time_t tt = std::mktime(&t);
    if (tt != (time_t)-1) {
        struct tm tmp = {0, };
#if defined (_WIN32) || defined (_WIN64)
        ::localtime_s(&tmp, &tt);
#else
        ::localtime_r(&tt, &tmp);
#endif
        t = tmp;
    }
}
}

void Calendar::EnsureFocusDate()
{
    if (m_bHasFocusDate) {
        return;
    }
    struct tm d = {0, };
    if (HasSelection()) {
        d = (m_mode == Mode::kSingle) ? m_selectedDate : m_rangeStart;
    }
    else {
        d = m_today;
    }
    m_focusDate = d;
    m_bHasFocusDate = true;
}

void Calendar::SetKeyboardFocusDate(const struct tm& date)
{
    m_focusDate = date;
    m_bHasFocusDate = true;
    Invalidate();
}

void Calendar::SyncFocusToDisplay()
{
    // After paging with PageUp/PageDown, align the keyboard focus with the new display range (keep the same day/month/year, only follow the display boundary)
    struct tm d = m_focusDate;
    d.tm_year = m_displayYear - 1900;
    if (m_viewMode == 0) {
        d.tm_mon = m_displayMonth - 1;
    }
    NormalizeTm(d);
    ClampFocusToLimits(d); // Clamp the focus back to the selectable range after paging
    SetKeyboardFocusDate(d);
}

void Calendar::MoveFocusDay(int32_t deltaDays)
{
    struct tm d = m_focusDate;
    d.tm_mday += deltaDays;
    NormalizeTm(d);
    ClampFocusToLimits(d); // Clamp to the selectable range, so that the focus ring does not land on a disabled day
    int32_t fy = d.tm_year + 1900;
    int32_t fm = d.tm_mon + 1;
    if (fy != m_displayYear || fm != m_displayMonth) {
        SetDisplayMonth(fy, fm); // Follow the display so that the focus is always visible
    }
    SetKeyboardFocusDate(d);
}

void Calendar::MoveFocusMonth(int32_t deltaMonths)
{
    struct tm d = m_focusDate;
    d.tm_mon += deltaMonths;
    d.tm_mday = 1; // Avoid the 31st being normalized by mktime into the month after next when crossing months (e.g. 1/31 + 1 month should be February, but keeping 31 would give March)
    NormalizeTm(d);
    ClampFocusToLimits(d);
    int32_t fy = d.tm_year + 1900;
    if (fy != m_displayYear) {
        SetDisplayMonth(fy, d.tm_mon + 1); // Follow the year in the year view
    }
    SetKeyboardFocusDate(d);
}

void Calendar::MoveFocusYear(int32_t deltaYears)
{
    struct tm d = m_focusDate;
    d.tm_year += deltaYears;
    // Note: tm_mday is not reset; keeping the day number makes Ctrl+arrow jump to "the same day of the previous/next year" instead of the 1st in the month view
    NormalizeTm(d);
    ClampFocusToLimits(d);
    int32_t fy = d.tm_year + 1900;
    if (m_viewMode == 0) {
        // Month view: the grid shows a single month, so the display must follow the focus when it crosses a year,
        // otherwise the focus ring lands on an invisible month and the UI looks as if it does not refresh (it jumps only on the next ordinary move).
        SetDisplayMonth(fy, d.tm_mon + 1);
    }
    else if (m_viewMode == 1) {
        // Year view: the grid shows a single year, so the display follows the focus only when it crosses a year
        if (fy != m_displayYear) {
            SetDisplayMonth(fy, m_displayMonth);
        }
    }
    else {
        // Ten-year view: the display follows the focus only when it crosses a ten-year boundary (moving between years inside the ten-year grid keeps them visible)
        int32_t decade = (fy / 10) * 10;
        int32_t curDecade = (m_displayYear / 10) * 10;
        if (decade != curDecade) {
            SetDisplayMonth(fy, m_displayMonth);
        }
    }
    SetKeyboardFocusDate(d);
}

void Calendar::SelectFocusedDay(const EventArgs& msg)
{
    if (m_viewMode != 0) {
        return;
    }
    m_bRangeKeyboardExtending = false; // Switch to two-step clicking/single mode, ending the keyboard extension state
    // Out-of-range/disabled checks
    if (m_bHasMinDate && IsDateLess(m_focusDate, m_minDate)) {
        return;
    }
    if (m_bHasMaxDate && IsDateLess(m_maxDate, m_focusDate)) {
        return;
    }
    if (m_mode == Mode::kSingle) {
        SetDate(m_focusDate);
        // SetDate fires kEventValueChanged, which may close the flyout synchronously and destroy this control, so check for expiry before accessing this
        if (msg.IsSenderExpired()) {
            return;
        }
    }
    else {
        // Range mode: two-step selection (the same semantics as mouse interaction)
        if (!m_bRangeAwaitingEnd) {
            m_rangeDragStart = m_focusDate;
            m_rangeStart = m_focusDate;
            m_rangeEnd = m_focusDate;
            m_bHasRangeStart = true;
            m_bHasRangeEnd = false;
            m_bRangeAwaitingEnd = true;
            m_bCellsDirty = true;   // The range start day changed, bSelected must be recomputed
            Invalidate();
        }
        else {
            struct tm d = m_focusDate;
            if (IsDateLess(d, m_rangeDragStart)) {
                m_rangeEnd = m_rangeDragStart;
                m_rangeStart = d;
            }
            else {
                m_rangeStart = m_rangeDragStart;
                m_rangeEnd = d;
            }
            m_bHasRangeStart = true;
            m_bHasRangeEnd = true;
            m_bRangeAwaitingEnd = false;
            m_bCellsDirty = true;   // The range is confirmed, bSelected/bInRange must be recomputed
            Invalidate();
            EmitDateChanged(true);
            if (msg.IsSenderExpired()) {
                return;
            }
        }
    }
}

void Calendar::ClampFocusToLimits(struct tm& date)
{
    // Clamp the focus date to the selectable range [m_minDate, m_maxDate], so that the focus ring does not land on a disabled day
    if (m_bHasMinDate && IsDateLess(date, m_minDate)) {
        date = m_minDate;
    }
    if (m_bHasMaxDate && IsDateLess(m_maxDate, date)) {
        date = m_maxDate;
    }
}

void Calendar::ExtendRangeSelection(int32_t deltaDays)
{
    if (m_viewMode != 0 || m_mode != Mode::kRange) {
        return;
    }
    if (!m_bRangeKeyboardExtending) {
        // Start the keyboard range extension anchored at the current focus; also cancel the "waiting for the end day" state of the two-step click
        m_rangeDragStart = m_focusDate;
        m_bRangeKeyboardExtending = true;
        m_bRangeAwaitingEnd = false;
    }
    MoveFocusDay(deltaDays); // Move the focus by the step first (it is already clamped to the selectable range internally)
    if (IsDateLess(m_focusDate, m_rangeDragStart)) {
        m_rangeStart = m_focusDate;
        m_rangeEnd = m_rangeDragStart;
    }
    else {
        m_rangeStart = m_rangeDragStart;
        m_rangeEnd = m_focusDate;
    }
    m_bHasRangeStart = true;
    m_bHasRangeEnd = true;
    ClampFocusToLimits(m_rangeStart);
    ClampFocusToLimits(m_rangeEnd);
    m_bCellsDirty = true;   // The keyboard-extended range changed, bSelected/bInRange must be recomputed
    Invalidate();
}

void Calendar::CommitRangeSelection(const EventArgs& /*msg*/)
{
    // Finalize the range produced by the keyboard extension and fire the changed event (the caller checks whether the control was destroyed after SendEvent)
    m_bRangeKeyboardExtending = false;
    m_bRangeAwaitingEnd = false;
    m_bHasRangeStart = true;
    m_bHasRangeEnd = true;
    m_bCellsDirty = true;   // The range is finalized, bSelected/bInRange must be recomputed
    Invalidate();
    EmitDateChanged(true);
}

void Calendar::MoveFocusToMonthStart()
{
    struct tm d = {0, };
    d.tm_year = m_displayYear - 1900;
    d.tm_mon = m_displayMonth - 1;
    d.tm_mday = 1;
    NormalizeTm(d);
    ClampFocusToLimits(d);
    SetKeyboardFocusDate(d);
}

void Calendar::MoveFocusToMonthEnd()
{
    struct tm d = {0, };
    d.tm_year = m_displayYear - 1900;
    d.tm_mon = m_displayMonth; // 0-based next month
    d.tm_mday = 0;            // day 0 = the last day of the current month
    NormalizeTm(d);
    ClampFocusToLimits(d);
    SetKeyboardFocusDate(d);
}

void Calendar::MoveFocusToYearBoundary(bool bToStart)
{
    struct tm d = m_focusDate;
    d.tm_year = m_displayYear - 1900;
    d.tm_mon = bToStart ? 0 : 11; // January / December
    d.tm_mday = 1;
    NormalizeTm(d);
    ClampFocusToLimits(d);
    SetKeyboardFocusDate(d);
}

void Calendar::MoveFocusToDecadeBoundary(bool bToStart)
{
    int32_t startYear = (m_displayYear / 10) * 10;
    int32_t year = bToStart ? startYear : (startYear + 11);
    struct tm d = m_focusDate;
    d.tm_year = year - 1900;
    d.tm_mday = 1;
    NormalizeTm(d);
    ClampFocusToLimits(d);
    SetKeyboardFocusDate(d);
}

bool Calendar::HandleKeyDown(const EventArgs& msg)
{
    EnsureFocusDate();
    const uint32_t vk = msg.vkCode;
    const bool bCtrl = (msg.modifierKey & kControl) != 0;
    const bool bShift = (msg.modifierKey & kShift) != 0;

    // Range mode with a keyboard extension in progress: the arrow keys keep extending the interval towards the focus (the same origin as starting an extension with Shift+arrow)
    if (m_viewMode == 0 && m_mode == Mode::kRange && m_bRangeKeyboardExtending && !bCtrl) {
        switch (vk) {
        case kVK_LEFT:  ExtendRangeSelection(-1); return true;
        case kVK_RIGHT: ExtendRangeSelection(1); return true;
        case kVK_UP:    ExtendRangeSelection(-kDaysPerWeek); return true;
        case kVK_DOWN:  ExtendRangeSelection(kDaysPerWeek); return true;
        default: break;
        }
    }

    switch (vk) {
    case kVK_LEFT:
        if (m_viewMode == 0) {
            if (m_mode == Mode::kRange && bShift) { ExtendRangeSelection(-1); }
            else if (bCtrl) { MoveFocusYear(-1); }        // Ctrl+Left: jump to the previous year
            else       { MoveFocusDay(-1); }
        }
        else if (m_viewMode == 1) {
            if (bCtrl) { MoveFocusYear(-10); }       // Ctrl+Left: jump to the previous ten-year span
            else       { MoveFocusMonth(-1); }
        }
        else {
            if (bCtrl) { MoveFocusYear(-100); }      // Ctrl+Left: jump to the previous century
            else       { MoveFocusYear(-1); }
        }
        return true;
    case kVK_RIGHT:
        if (m_viewMode == 0) {
            if (m_mode == Mode::kRange && bShift) { ExtendRangeSelection(1); }
            else if (bCtrl) { MoveFocusYear(1); }
            else       { MoveFocusDay(1); }
        }
        else if (m_viewMode == 1) {
            if (bCtrl) { MoveFocusYear(10); }
            else       { MoveFocusMonth(1); }
        }
        else {
            if (bCtrl) { MoveFocusYear(100); }
            else       { MoveFocusYear(1); }
        }
        return true;
    case kVK_UP:
        if (m_viewMode == 0) {
            if (m_mode == Mode::kRange && bShift) { ExtendRangeSelection(-kDaysPerWeek); }
            else if (bCtrl) { MoveFocusYear(-1); }
            else       { MoveFocusDay(-kDaysPerWeek); }
        }
        else if (m_viewMode == 1) {
            if (bCtrl) { MoveFocusYear(-10); }
            else       { MoveFocusMonth(-kYearGridCols); }
        }
        else {
            if (bCtrl) { MoveFocusYear(-100); }
            else       { MoveFocusYear(-kYearGridCols); }
        }
        return true;
    case kVK_DOWN:
        if (m_viewMode == 0) {
            if (m_mode == Mode::kRange && bShift) { ExtendRangeSelection(kDaysPerWeek); }
            else if (bCtrl) { MoveFocusYear(1); }
            else       { MoveFocusDay(kDaysPerWeek); }
        }
        else if (m_viewMode == 1) {
            if (bCtrl) { MoveFocusYear(10); }
            else       { MoveFocusMonth(kYearGridCols); }
        }
        else {
            if (bCtrl) { MoveFocusYear(100); }
            else       { MoveFocusYear(kYearGridCols); }
        }
        return true;
    case kVK_HOME: // Home: jump to the start of the period (first day of the month / first month of the year / first year of the ten-year span)
        if (m_viewMode == 0)      { MoveFocusToMonthStart(); }
        else if (m_viewMode == 1) { MoveFocusToYearBoundary(true); }
        else                      { MoveFocusToDecadeBoundary(true); }
        return true;
    case kVK_END:  // End: jump to the end of the period (last day of the month / last month of the year / last year of the ten-year span)
        if (m_viewMode == 0)      { MoveFocusToMonthEnd(); }
        else if (m_viewMode == 1) { MoveFocusToYearBoundary(false); }
        else                      { MoveFocusToDecadeBoundary(false); }
        return true;
    case kVK_PRIOR: // PageUp: previous month / year / ten-year span (already clamped to the selectable range boundary)
        NavigatePrev();
        SyncFocusToDisplay();
        return true;
    case kVK_NEXT: // PageDown: next month / year / ten-year span (already clamped to the selectable range boundary)
        NavigateNext();
        SyncFocusToDisplay();
        return true;
    case kVK_RETURN:
    case kVK_SPACE:
        if (m_viewMode == 0) {
            if (m_mode == Mode::kRange && m_bRangeKeyboardExtending) {
                CommitRangeSelection(msg); // Confirm the range produced by the keyboard extension
            }
            else {
                SelectFocusedDay(msg);
            }
        }
        else if (m_viewMode == 1) {
            // Drill down from the year view to the month view
            SetDisplayMonth(m_focusDate.tm_year + 1900, m_focusDate.tm_mon + 1);
            SetViewMode(0);
        }
        else {
            // Drill down from the ten-year view to the year view
            SetDisplayMonth(m_focusDate.tm_year + 1900, m_displayMonth);
            SetViewMode(1);
        }
        return true;
    default:
        return false;
    }
}

void Calendar::SetInitialFocus()
{
    EnsureFocusDate();
    Invalidate();
}

void Calendar::DrawFocusRing(IRender* pRender, const UiRect& rect, float cornerRadius)
{
    UiColor focusColor = GetUiColor("border_focus_ring");
    if (focusColor.IsEmpty()) {
        return;
    }
    UiRectF focusRectF;
    focusRectF.left = (float)(rect.left + 1);
    focusRectF.top = (float)(rect.top + 1);
    focusRectF.right = (float)(rect.right - 1);
    focusRectF.bottom = (float)(rect.bottom - 1);
    float radius = Dpi().GetScaleFloat(cornerRadius);
    float width = Dpi().GetScaleFloat(1.5f);
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory != nullptr) {
        std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(focusColor, width));
        pRender->DrawRoundRect(focusRectF, radius, radius, pen.get());
    }
}

void Calendar::GetMonthCells(int32_t year, int32_t month, std::vector<DayCell>& cells) const
{
    cells.clear();
    cells.reserve(kDaysPerPage);

    // Get the weekday of the first day of the month (0 = Sunday)
    struct tm firstDay = {0, };
    firstDay.tm_year = year - 1900;
    firstDay.tm_mon = month - 1;
    firstDay.tm_mday = 1;
    firstDay.tm_hour = 12; // Avoid daylight saving time issues
    time_t t = std::mktime(&firstDay);
    if (t == (time_t)-1) {
        return;
    }
    struct tm firstDayOfWeek = {0, };
#if defined (_WIN32) || defined (_WIN64)
    ::localtime_s(&firstDayOfWeek, &t);
#else
    ::localtime_r(&t, &firstDayOfWeek);
#endif
    int32_t wday = firstDayOfWeek.tm_wday; // 0 = Sunday

    // Compute the first day of the grid (it may belong to the previous month)
    // firstDayOfWeek: 0 = Sunday, 1 = Monday, ...
    // m_firstDayOfWeek: 0 = Sunday, 1 = Monday, ...
    int32_t offset = (wday - m_firstDayOfWeek + kDaysPerWeek) % kDaysPerWeek;
    if (offset < 0) {
        offset = 0;
    }

    // First day of the grid
    struct tm gridStart = firstDay;
    gridStart.tm_mday = 1 - offset;
    t = std::mktime(&gridStart);
    if (t == (time_t)-1) {
        return;
    }
#if defined (_WIN32) || defined (_WIN64)
    ::localtime_s(&gridStart, &t);
#else
    ::localtime_r(&t, &gridStart);
#endif

    // Fill 42 days
    for (int32_t i = 0; i < kDaysPerPage; ++i) {
        struct tm day = gridStart;
        day.tm_mday = gridStart.tm_mday + i;
        t = std::mktime(&day);
        if (t == (time_t)-1) {
            continue;
        }
#if defined (_WIN32) || defined (_WIN64)
        ::localtime_s(&day, &t);
#else
        ::localtime_r(&t, &day);
#endif

        DayCell cell;
        cell.year = day.tm_year + 1900;
        cell.month = day.tm_mon + 1;
        cell.day = day.tm_mday;
        cell.bCurrentMonth = (day.tm_mon == month - 1);
        cell.bToday = (m_bTodayValid && IsSameDay(day, m_today));
        cell.bSelected = false;
        cell.bInRange = false;
        cell.bHovered = false;
        cell.bDisabled = false;

        // Check whether it is selected
        if (m_bHasSelectedDate && IsSameDay(day, m_selectedDate)) {
            cell.bSelected = true;
        }

        // Check whether it is inside the range
        if (m_bHasRangeStart && m_bHasRangeEnd) {
            if (IsSameDay(day, m_rangeStart) || IsSameDay(day, m_rangeEnd)) {
                cell.bSelected = true;
            }
            else if (IsDateInRange(day, m_rangeStart, m_rangeEnd)) {
                cell.bInRange = true;
            }
        }
        else if (m_bHasRangeStart && IsSameDay(day, m_rangeStart)) {
            // Range selection: while only the start day is selected and the end day is still pending, highlight the start day with the selected style
            cell.bSelected = true;
        }

        // Check whether it is hovered
        if (m_bHasHover && cell.year == m_hoverYear && cell.month == m_hoverMonth && cell.day == m_hoverDay) {
            cell.bHovered = true;
        }

        // Check whether it exceeds the range limits
        if (m_bHasMinDate && IsDateLess(day, m_minDate)) {
            cell.bDisabled = true;
        }
        if (m_bHasMaxDate && IsDateLess(m_maxDate, day)) {
            cell.bDisabled = true;
        }

        cells.push_back(cell);
    }
}

bool Calendar::HitTestDay(const UiPoint& pt, DayCell& cell) const
{
    if (m_viewMode != 0) {
        return false;
    }
    // Lazy recomputation: both m_cells and m_bCellsDirty are mutable, so the cache can be refreshed inside a const method;
    // it no longer depends on the "Paint first, then hit test" call order (a change of construction order cannot break clicking)
    if (m_bCellsDirty) {
        GetMonthCells(m_displayYear, m_displayMonth, m_cells);
        m_bCellsDirty = false;
    }
    for (const auto& c : m_cells) {
        if (c.rect.ContainsPt(pt)) {
            cell = c;
            return true;
        }
    }
    return false;
}

void Calendar::Paint(IRender* pRender, const UiRect& rcPaint)
{
    BaseClass::Paint(pRender, rcPaint);
    if (pRender == nullptr) {
        return;
    }

    // Correct the "today" cache before every paint: the highlight/GoToToday must follow the system date after midnight
    RefreshToday();

    UiRect rect = GetRect();
    UiPadding rcPadding = GetControlPadding();
    rect.Deflate(rcPadding);

    // Get the font
    IFont* pFont = GetIFontById("system_regular_14");

    if (m_viewMode == 0) {
        // Month view: paint the weekday header first, then the date grid
        UiRect headerRect = rect;
        headerRect.bottom = headerRect.top + Dpi().GetScaleInt(24);
        DrawWeekdayHeader(pRender, headerRect, pFont);

        UiRect gridRect = rect;
        gridRect.top = headerRect.bottom;
        DrawMonthView(pRender, gridRect);
    }
    else if (m_viewMode == 1) {
        DrawYearView(pRender, rect);
    }
    else {
        DrawDecadeView(pRender, rect);
    }
}

void Calendar::DrawWeekdayHeader(IRender* pRender, const UiRect& rect, IFont* pFont)
{
    if (pFont == nullptr) {
        return;
    }
    int32_t cellWidth = rect.Width() / kDaysPerWeek;
    UiColor textColor = GetUiColor("text_muted");
    if (textColor.IsEmpty()) {
        textColor = UiColor(0xFF888888);
    }

    for (int32_t i = 0; i < kDaysPerWeek; ++i) {
        UiRect cellRect;
        cellRect.left = rect.left + i * cellWidth;
        cellRect.right = cellRect.left + cellWidth;
        cellRect.top = rect.top;
        cellRect.bottom = rect.bottom;

        DrawStringParam param;
        param.textRect = cellRect;
        param.dwTextColor = textColor;
        param.pFont = pFont;
        param.uFormat = TEXT_HCENTER | TEXT_VCENTER;
        pRender->DrawString(GetWeekdayText(i), param);
    }
}

std::string Calendar::GetWeekdayText(int32_t index) const
{
    // The weekday header uses the multi-language resources (STRID_PUBLIC_CALENDAR_WEEKDAY_*), which supports localization;
    // index: 0 = first day (after the offset by firstDayOfWeek)
    static const std::string kWeekdayIds[] = {
        "STRID_PUBLIC_CALENDAR_WEEKDAY_SUNDAY",
        "STRID_PUBLIC_CALENDAR_WEEKDAY_MONDAY",
        "STRID_PUBLIC_CALENDAR_WEEKDAY_TUESDAY",
        "STRID_PUBLIC_CALENDAR_WEEKDAY_WEDNESDAY",
        "STRID_PUBLIC_CALENDAR_WEEKDAY_THURSDAY",
        "STRID_PUBLIC_CALENDAR_WEEKDAY_FRIDAY",
        "STRID_PUBLIC_CALENDAR_WEEKDAY_SATURDAY",
    };
    int32_t dayIndex = (m_firstDayOfWeek + index) % kDaysPerWeek;
    std::string strText = GlobalManager::Instance().Lang().GetStringViaID(kWeekdayIds[dayIndex]);
    if (strText.empty()) {
        // Fallback when the language resource is missing (to avoid blank text): short English names
        static const std::string kFallback[] = {
            "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
        };
        strText = kFallback[dayIndex];
    }
    return strText;
}

std::string Calendar::GetMonthText(int32_t month) const
{
    // The month text uses the multi-language resources (STRID_PUBLIC_CALENDAR_MONTH_*), which supports localization
    static const std::string kMonthIds[] = {
        "STRID_PUBLIC_CALENDAR_MONTH_JANUARY",
        "STRID_PUBLIC_CALENDAR_MONTH_FEBRUARY",
        "STRID_PUBLIC_CALENDAR_MONTH_MARCH",
        "STRID_PUBLIC_CALENDAR_MONTH_APRIL",
        "STRID_PUBLIC_CALENDAR_MONTH_MAY",
        "STRID_PUBLIC_CALENDAR_MONTH_JUNE",
        "STRID_PUBLIC_CALENDAR_MONTH_JULY",
        "STRID_PUBLIC_CALENDAR_MONTH_AUGUST",
        "STRID_PUBLIC_CALENDAR_MONTH_SEPTEMBER",
        "STRID_PUBLIC_CALENDAR_MONTH_OCTOBER",
        "STRID_PUBLIC_CALENDAR_MONTH_NOVEMBER",
        "STRID_PUBLIC_CALENDAR_MONTH_DECEMBER",
    };
    if ((month < 1) || (month > 12)) {
        return std::string();
    }
    std::string strText = GlobalManager::Instance().Lang().GetStringViaID(kMonthIds[month - 1]);
    if (strText.empty()) {
        // Fallback when the language resource is missing: use the "Month %d" numeric format
        strText = StringUtil::Printf("Month %d", month);
    }
    return strText;
}

void Calendar::DrawMonthView(IRender* pRender, const UiRect& rect)
{
    // Recompute the cells unconditionally: bSelected/bHovered/bInRange/bDisabled and the other states change at any time with SetDate/hovering,
    // so if this were changed to "recompute only when m_bCellsDirty is set", every setter that changes these states would have to remember to mark it dirty,
    // which is very easy to miss (this once made the hover/selection highlight disappear in single mode). Painting the month view is not a high-frequency per-frame path, so the cost of 42 mktime calls is negligible.
    GetMonthCells(m_displayYear, m_displayMonth, m_cells);
    m_bCellsDirty = false;

    int32_t cellWidth = rect.Width() / kDaysPerWeek;
    int32_t cellHeight = rect.Height() / kWeeksPerMonth;

    IFont* pFont = GetIFontById("system_regular_14");

    for (size_t i = 0; i < m_cells.size(); ++i) {
        DayCell& cell = m_cells[i];
        int32_t row = (int32_t)(i / kDaysPerWeek);
        int32_t col = (int32_t)(i % kDaysPerWeek);

        cell.rect.left = rect.left + col * cellWidth;
        cell.rect.right = cell.rect.left + cellWidth;
        cell.rect.top = rect.top + row * cellHeight;
        cell.rect.bottom = cell.rect.top + cellHeight;

        DrawDayCell(pRender, cell, pFont);
    }
}

void Calendar::DrawDayCell(IRender* pRender, const DayCell& cell, IFont* pFont)
{
    if (pFont == nullptr) {
        return;
    }

    // Padding
    UiRect innerRect = cell.rect;
    innerRect.Deflate(2, 2);

    UiColor textColor;
    UiColor bgColor;
    UiColor borderColor;
    float borderWidth = 0.0f;
    float cornerRadius = 4.0f;

    // Determine the colors
    if (cell.bDisabled) {
        textColor = GetUiColor("text_disabled");
    }
    else if (cell.bSelected) {
        textColor = GetUiColor("text_primary_btn_normal");
        bgColor = GetUiColor("color_accent");
    }
    else if (cell.bInRange) {
        textColor = GetUiColor("text_default");
        bgColor = GetUiColor("color_accent");
        bgColor = UiColor(bgColor.GetA(), bgColor.GetR(), bgColor.GetG(), bgColor.GetB()); // Semi-transparent effect
        // Actually use a lighter background color
        bgColor = GetUiColor("bg_btn_hovered");
    }
    else if (cell.bHovered) {
        textColor = GetUiColor("text_default");
        bgColor = GetUiColor("bg_btn_hovered");
    }
    else if (cell.bToday) {
        textColor = GetUiColor("color_accent");
        borderColor = GetUiColor("color_accent");
        borderWidth = 1.0f;
    }
    else if (!cell.bCurrentMonth) {
        textColor = GetUiColor("text_muted");
    }
    else {
        textColor = GetUiColor("text_default");
    }

    // Default color fallback
    if (textColor.IsEmpty()) {
        textColor = UiColor(0xFF000000);
    }

    // Paint the background
    if (!bgColor.IsEmpty()) {
        UiRectF bgRectF;
        bgRectF.left = (float)innerRect.left;
        bgRectF.top = (float)innerRect.top;
        bgRectF.right = (float)innerRect.right;
        bgRectF.bottom = (float)innerRect.bottom;
        float radius = Dpi().GetScaleFloat(cornerRadius);
        pRender->FillRoundRect(bgRectF, radius, radius, bgColor);
    }

    // Paint the border (today)
    if (!borderColor.IsEmpty() && borderWidth > 0.1f) {
        UiRectF borderRectF;
        borderRectF.left = (float)innerRect.left;
        borderRectF.top = (float)innerRect.top;
        borderRectF.right = (float)innerRect.right;
        borderRectF.bottom = (float)innerRect.bottom;
        float radius = Dpi().GetScaleFloat(cornerRadius);
        float width = Dpi().GetScaleFloat(borderWidth);
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        if (pRenderFactory != nullptr) {
            std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(borderColor, width));
            pRender->DrawRoundRect(borderRectF, radius, radius, pen.get());
        }
    }

    // Paint the text
    std::string text = StringUtil::Printf("%d", cell.day);
    DrawStringParam param;
    param.textRect = cell.rect;
    param.dwTextColor = textColor;
    param.pFont = pFont;
    param.uFormat = TEXT_HCENTER | TEXT_VCENTER;
    pRender->DrawString(text, param);

    // Keyboard focus ring (different from mouse hovering: drawn only when a focus date exists through keyboard navigation)
    if (m_bHasFocusDate && cell.year == (m_focusDate.tm_year + 1900) &&
        cell.month == (m_focusDate.tm_mon + 1) && cell.day == m_focusDate.tm_mday) {
        DrawFocusRing(pRender, innerRect, cornerRadius);
    }
}

void Calendar::DrawYearView(IRender* pRender, const UiRect& rect)
{
    IFont* pFont = GetIFontById("system_regular_14");
    if (pFont == nullptr) {
        return;
    }

    int32_t cellWidth = rect.Width() / kYearGridCols;
    int32_t cellHeight = rect.Height() / kYearGridRows;

    UiColor textColor = GetUiColor("text_default");
    UiColor selectedColor = GetUiColor("color_accent");
    UiColor selectedTextColor = GetUiColor("text_primary_btn_normal");

    int32_t currentYear = m_displayYear;
    for (int32_t i = 0; i < 12; ++i) {
        int32_t row = i / kYearGridCols;
        int32_t col = i % kYearGridCols;
        int32_t month = i + 1;

        UiRect cellRect;
        cellRect.left = rect.left + col * cellWidth;
        cellRect.right = cellRect.left + cellWidth;
        cellRect.top = rect.top + row * cellHeight;
        cellRect.bottom = cellRect.top + cellHeight;

        UiRect innerRect = cellRect;
        innerRect.Deflate(4, 4);

        bool bSelected = (m_bHasSelectedDate && m_selectedDate.tm_year + 1900 == currentYear && m_selectedDate.tm_mon + 1 == month);
        bool bCurrent = (m_bTodayValid && m_today.tm_year + 1900 == currentYear && m_today.tm_mon + 1 == month);

        // Background
        if (bSelected) {
            UiRectF bgRectF;
            bgRectF.left = (float)innerRect.left;
            bgRectF.top = (float)innerRect.top;
            bgRectF.right = (float)innerRect.right;
            bgRectF.bottom = (float)innerRect.bottom;
            float radius = Dpi().GetScaleFloat(4.0f);
            pRender->FillRoundRect(bgRectF, radius, radius, selectedColor);
        }

        // Text
        std::string text = GetMonthText(month);
        DrawStringParam param;
        param.textRect = cellRect;
        param.dwTextColor = bSelected ? selectedTextColor : (bCurrent ? selectedColor : textColor);
        param.pFont = pFont;
        param.uFormat = TEXT_HCENTER | TEXT_VCENTER;
        pRender->DrawString(text, param);

        // Keyboard focus ring (year view: the focus stays on one month)
        if (m_bHasFocusDate && (m_focusDate.tm_year + 1900) == currentYear &&
            (m_focusDate.tm_mon + 1) == month) {
            DrawFocusRing(pRender, innerRect, 4.0f);
        }
    }
}

void Calendar::DrawDecadeView(IRender* pRender, const UiRect& rect)
{
    IFont* pFont = GetIFontById("system_regular_14");
    if (pFont == nullptr) {
        return;
    }

    int32_t cellWidth = rect.Width() / kYearGridCols;
    int32_t cellHeight = rect.Height() / kYearGridRows;

    UiColor textColor = GetUiColor("text_default");
    UiColor selectedColor = GetUiColor("color_accent");
    UiColor selectedTextColor = GetUiColor("text_primary_btn_normal");

    int32_t startYear = (m_displayYear / 10) * 10;
    for (int32_t i = 0; i < 12; ++i) {
        int32_t row = i / kYearGridCols;
        int32_t col = i % kYearGridCols;
        int32_t year = startYear + i;

        UiRect cellRect;
        cellRect.left = rect.left + col * cellWidth;
        cellRect.right = cellRect.left + cellWidth;
        cellRect.top = rect.top + row * cellHeight;
        cellRect.bottom = cellRect.top + cellHeight;

        UiRect innerRect = cellRect;
        innerRect.Deflate(4, 4);

        bool bSelected = (m_bHasSelectedDate && m_selectedDate.tm_year + 1900 == year);
        bool bCurrent = (m_bTodayValid && m_today.tm_year + 1900 == year);

        // Background
        if (bSelected) {
            UiRectF bgRectF;
            bgRectF.left = (float)innerRect.left;
            bgRectF.top = (float)innerRect.top;
            bgRectF.right = (float)innerRect.right;
            bgRectF.bottom = (float)innerRect.bottom;
            float radius = Dpi().GetScaleFloat(4.0f);
            pRender->FillRoundRect(bgRectF, radius, radius, selectedColor);
        }

        // Text
        std::string text = StringUtil::Printf("%d", year);
        DrawStringParam param;
        param.textRect = cellRect;
        param.dwTextColor = bSelected ? selectedTextColor : (bCurrent ? selectedColor : textColor);
        param.pFont = pFont;
        param.uFormat = TEXT_HCENTER | TEXT_VCENTER;
        pRender->DrawString(text, param);

        // Keyboard focus ring (ten-year view: the focus stays on one year)
        if (m_bHasFocusDate && (m_focusDate.tm_year + 1900) == year) {
            DrawFocusRing(pRender, innerRect, 4.0f);
        }
    }
}

bool Calendar::MouseMove(const EventArgs& msg)
{
    if (m_viewMode == 0) {
        // Dragging in range mode: update the range end in real time to provide visual feedback
        if (m_mode == Mode::kRange && m_bRangeDragging) {
            DayCell cell;
            if (HitTestDay(msg.ptMouse, cell)) {
                struct tm date = {0, };
                date.tm_year = cell.year - 1900;
                date.tm_mon = cell.month - 1;
                date.tm_mday = cell.day;
                // Determine start/end from the mouse position, using m_rangeDragStart as the anchor
                if (IsDateLess(date, m_rangeDragStart)) {
                    m_rangeStart = date;
                    m_rangeEnd = m_rangeDragStart;
                }
                else {
                    m_rangeStart = m_rangeDragStart;
                    m_rangeEnd = date;
                }
                m_bHasRangeStart = true;
                m_bHasRangeEnd = true;
                m_bCellsDirty = true;   // The drag updated the range, bSelected/bInRange must be recomputed
                Invalidate();
            }
        }
        DayCell cell;
        if (HitTestDay(msg.ptMouse, cell)) {
            if (!m_bHasHover || m_hoverYear != cell.year || m_hoverMonth != cell.month || m_hoverDay != cell.day) {
                m_hoverYear = cell.year;
                m_hoverMonth = cell.month;
                m_hoverDay = cell.day;
                m_bHasHover = true;
                m_bCellsDirty = true;   // The hovered cell changed, bHovered must be recomputed
                Invalidate();
            }
        }
        else {
            if (m_bHasHover) {
                m_bHasHover = false;
                m_bCellsDirty = true;   // The mouse left the control, bHovered must be recomputed
                Invalidate();
            }
        }
    }
    return BaseClass::MouseMove(msg);
}

bool Calendar::ButtonDown(const EventArgs& msg)
{
    bool bRet = BaseClass::ButtonDown(msg);
    if (msg.IsSenderExpired()) {
        return false;
    }

    if (m_viewMode == 0) {
        // Month view: select a date
        DayCell cell;
        if (HitTestDay(msg.ptMouse, cell) && !cell.bDisabled) {
            if (m_mode == Mode::kSingle) {
                struct tm date = {0, };
                date.tm_year = cell.year - 1900;
                date.tm_mon = cell.month - 1;
                date.tm_mday = cell.day;
                SetDate(date);
                // The event callback may close the flyout synchronously and destroy this object, so check for expiry before continuing
                if (msg.IsSenderExpired()) {
                    return false;
                }
            }
            else {
                // Range mode: two-step clicking is supported (the first click sets the start day without closing, the second click sets the end day)
                struct tm date = {0, };
                date.tm_year = cell.year - 1900;
                date.tm_mon = cell.month - 1;
                date.tm_mday = cell.day;
                m_bRangeDragging = true;
                m_bRangeKeyboardExtending = false; // The mouse takes over, ending the keyboard extension state
                if (m_bRangeAwaitingEnd) {
                    // Second click: keep m_rangeDragStart (it points at the start day of the first click)
                    // The actual end day is determined in ButtonUp
                }
                else {
                    // First click or restart: set the start day
                    m_rangeDragStart = date;
                    m_rangeStart = date;
                    m_rangeEnd = date;
                    m_bHasRangeStart = true;
                    m_bHasRangeEnd = false;
                    m_bHasSelectedDate = false;
                    m_bCellsDirty = true;   // The range start day changed, bSelected must be recomputed
                    Invalidate();
                }
            }
        }
    }
    else if (m_viewMode == 1) {
        // Year view: clicking a month enters the month view
        UiRect rect = GetRect();
        int32_t cellWidth = rect.Width() / kYearGridCols;
        int32_t cellHeight = rect.Height() / kYearGridRows;
        int32_t col = (msg.ptMouse.x - rect.left) / cellWidth;
        int32_t row = (msg.ptMouse.y - rect.top) / cellHeight;
        if (col >= 0 && col < kYearGridCols && row >= 0 && row < kYearGridRows) {
            int32_t month = row * kYearGridCols + col + 1;
            if (month >= 1 && month <= 12) {
                SetDisplayMonth(m_displayYear, month);
                SetViewMode(0);
            }
        }
    }
    else {
        // Ten-year view: clicking a year enters the year view
        UiRect rect = GetRect();
        int32_t cellWidth = rect.Width() / kYearGridCols;
        int32_t cellHeight = rect.Height() / kYearGridRows;
        int32_t col = (msg.ptMouse.x - rect.left) / cellWidth;
        int32_t row = (msg.ptMouse.y - rect.top) / cellHeight;
        if (col >= 0 && col < kYearGridCols && row >= 0 && row < kYearGridRows) {
            int32_t year = (m_displayYear / 10) * 10 + row * kYearGridCols + col;
            SetDisplayMonth(year, m_displayMonth);
            SetViewMode(1);
        }
    }
    return bRet;
}

bool Calendar::ButtonUp(const EventArgs& msg)
{
    if (m_viewMode == 0 && m_mode == Mode::kRange && m_bRangeDragging) {
        m_bRangeDragging = false;
        DayCell cell;
        if (HitTestDay(msg.ptMouse, cell) && !cell.bDisabled) {
            struct tm date = {0, };
            date.tm_year = cell.year - 1900;
            date.tm_mon = cell.month - 1;
            date.tm_mday = cell.day;

            if (IsSameDay(date, m_rangeDragStart) && !m_bRangeAwaitingEnd) {
                // First click on the same day: the start day has already been selected and highlighted in ButtonDown,
                // so set the "waiting for the end day" state here without firing an event and without closing the flyout
                m_rangeStart = date;
                m_rangeEnd = date;
                m_bHasRangeStart = true;
                m_bHasRangeEnd = false;
                m_bRangeAwaitingEnd = true;
                m_bCellsDirty = true;   // The range start day changed, bSelected must be recomputed
                Invalidate();
                return BaseClass::ButtonUp(msg);
            }

            // Complete the range selection (second click, or dragging to a different day)
            if (IsDateLess(date, m_rangeDragStart)) {
                m_rangeEnd = m_rangeDragStart;
                m_rangeStart = date;
            }
            else {
                m_rangeStart = m_rangeDragStart;
                m_rangeEnd = date;
            }
            m_bHasRangeStart = true;
            m_bHasRangeEnd = true;
            m_bRangeAwaitingEnd = false;
            m_bCellsDirty = true;   // The range is confirmed, bSelected/bInRange must be recomputed
            Invalidate();

            // Fire the event
            EmitDateChanged(true);
            // The event callback may close the flyout synchronously and destroy this object, so check for expiry before continuing
            if (msg.IsSenderExpired()) {
                return false;
            }
        }
    }
    return BaseClass::ButtonUp(msg);
}

bool Calendar::MouseLeave(const EventArgs& msg)
{
    if (m_bHasHover) {
        m_bHasHover = false;
        m_bCellsDirty = true;   // The mouse left the control, bHovered must be recomputed
        Invalidate();
    }
    return BaseClass::MouseLeave(msg);
}

bool Calendar::IsSameDay(const struct tm& a, const struct tm& b)
{
    return a.tm_year == b.tm_year && a.tm_mon == b.tm_mon && a.tm_mday == b.tm_mday;
}

bool Calendar::IsDateInRange(const struct tm& date, const struct tm& start, const struct tm& end)
{
    if (IsDateLess(date, start)) {
        return false;
    }
    if (IsDateLess(end, date)) {
        return false;
    }
    return true;
}

bool Calendar::IsDateLess(const struct tm& a, const struct tm& b)
{
    if (a.tm_year != b.tm_year) {
        return a.tm_year < b.tm_year;
    }
    if (a.tm_mon != b.tm_mon) {
        return a.tm_mon < b.tm_mon;
    }
    return a.tm_mday < b.tm_mday;
}

time_t Calendar::DateToTimeT(const struct tm& date)
{
    struct tm t = date;
    t.tm_hour = 12;
    t.tm_min = 0;
    t.tm_sec = 0;
    t.tm_isdst = -1;
    return std::mktime(&t);
}

struct tm Calendar::TimeTToDate(time_t t)
{
    struct tm date = {0, };
#if defined (_WIN32) || defined (_WIN64)
    ::localtime_s(&date, &t);
#else
    ::localtime_r(&t, &date);
#endif
    return date;
}

bool Calendar::ParseDateString(const std::string& str, struct tm& date)
{
    if (str.empty()) {
        return false;
    }
    // Supports yyyy-mm-dd or yyyy/mm/dd
    std::string s = str;
    StringUtil::ReplaceAll("/", "-", s);
    int32_t year = 0, month = 0, day = 0;
#ifdef DUI_UNICODE
#  if defined (_WIN32) || defined (_WIN64)
    if (::swscanf_s(s.c_str(), "%d-%d-%d", &year, &month, &day) == 3) {
#  else
    if (::swscanf(s.c_str(), "%d-%d-%d", &year, &month, &day) == 3) {
#  endif
#else
#  if defined (_WIN32) || defined (_WIN64)
    if (::sscanf_s(s.c_str(), "%d-%d-%d", &year, &month, &day) == 3) {
#  else
    if (::sscanf(s.c_str(), "%d-%d-%d", &year, &month, &day) == 3) {
#  endif
#endif
        if (year >= 1900 && year <= 9999 && month >= 1 && month <= 12 && day >= 1 && day <= 31) {
            date.tm_year = year - 1900;
            date.tm_mon = month - 1;
            date.tm_mday = day;
            return true;
        }
    }
    return false;
}

std::string Calendar::FormatDateString(const struct tm& date)
{
    return StringUtil::Printf("%04d-%02d-%02d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
}

struct tm Calendar::GetToday()
{
    time_t t = std::time(nullptr);
    struct tm today = {0, };
#if defined (_WIN32) || defined (_WIN64)
    ::localtime_s(&today, &t);
#else
    ::localtime_r(&t, &today);
#endif
    return today;
}

} // namespace ui
