#ifndef UI_CONTROL_CALENDAR_H_
#define UI_CONTROL_CALENDAR_H_

#include "dui/Core/Control.h"
#include <ctime>

namespace ui
{

/** Calendar panel control (Calendar): a custom-painted month grid, supporting single and range selection
 *
 *  Features:
 *  1. Month view: a 6-row x 7-column date grid, automatically padded with the days of the previous/next month (shown in gray)
 *  2. Year view: 12 month cells (3 columns x 4 rows)
 *  3. Ten-year view: 12 year cells (3 columns x 4 rows)
 *  4. Single mode: click a date to select it, which fires kEventValueChanged
 *  5. Range mode: select the start and end dates by clicking/dragging, the days in between are highlighted
 *  6. Today highlight: the current date is marked with a theme-colored dot
 *  7. Hover effect: the date cell under the mouse shows a hover background
 *
 *  XML attributes:
 *    calendar_mode       single / range (default single)
 *    first_day_of_week   monday / sunday (default monday)
 *    show_today_button   true / false (default true, reserved)
 *    min_date / max_date limits of the selectable date range (format: yyyy-mm-dd)
 *
 *  Skin classes (global.xml):
 *    calendar              the overall container
 *    calendar_weekday      weekday header
 *    calendar_day          current month day (normal)
 *    calendar_day_muted    previous/next month day (gray)
 *    calendar_day_today    today (theme-colored border/dot)
 *    calendar_day_selected selected date (theme-colored background)
 *    calendar_day_range    day inside the range (light background)
 *    calendar_day_hovered  hovered date (hover background)
 */
class DUI_API Calendar : public Control
{
    typedef Control BaseClass;
public:
    explicit Calendar(Window* pWindow);
    Calendar(const Calendar& r) = delete;
    Calendar& operator=(const Calendar& r) = delete;
    virtual ~Calendar() override = default;

    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;

    virtual bool MouseMove(const EventArgs& msg) override;
    virtual bool ButtonDown(const EventArgs& msg) override;
    virtual bool ButtonUp(const EventArgs& msg) override;
    virtual bool MouseLeave(const EventArgs& msg) override;

public:
    /** Selection mode
    */
    enum class Mode : uint32_t
    {
        kSingle = 0,    // Single date selection
        kRange = 1      // Date range selection
    };

    /** Set the selection mode
    */
    void SetMode(Mode mode);
    Mode GetMode() const { return m_mode; }

    /** Set the year and month currently displayed (triggers a repaint)
    * @param [in] year The year (e.g. 2025)
    * @param [in] month The month (1-12)
    */
    void SetDisplayMonth(int32_t year, int32_t month);

    /** Get the currently displayed year
    */
    int32_t GetDisplayYear() const { return m_displayYear; }

    /** Get the currently displayed month (1-12)
    */
    int32_t GetDisplayMonth() const { return m_displayMonth; }

    /** Set the selected date (single mode)
    * @param [in] date The date; only the year, month and day need to be valid
    */
    void SetDate(const struct tm& date);

    /** Get the selected date (single mode)
    */
    const struct tm& GetDate() const { return m_selectedDate; }

    /** Set the date range (range mode)
    * @param [in] start The start date
    * @param [in] end The end date
    */
    void SetDateRange(const struct tm& start, const struct tm& end);

    /** Get the date range
    * @param [out] start The start date
    * @param [out] end The end date
    */
    void GetDateRange(struct tm& start, struct tm& end) const;

    /** Whether there is a selected date/range
    */
    bool HasSelection() const;

    /** Clear the selection
    */
    void ClearSelection();

    /** Set the first day of the week
    * @param [in] dayOfWeek 0 = Sunday, 1 = Monday (default 1)
    */
    void SetFirstDayOfWeek(int32_t dayOfWeek);
    int32_t GetFirstDayOfWeek() const { return m_firstDayOfWeek; }

    /** Set the limits of the selectable date range
    * @param [in] minDate The minimum date (empty means unlimited)
    * @param [in] maxDate The maximum date (empty means unlimited)
    */
    void SetDateLimit(const std::string& minDate, const std::string& maxDate);

    /** Jump to today
    */
    void GoToToday();

    /** Previous month/next month navigation
    */
    void NavigatePrev();
    void NavigateNext();

    /** Switch to the year view/ten-year view
    */
    void SetViewMode(int32_t viewMode); // 0 = month, 1 = year, 2 = ten-year
    int32_t GetViewMode() const { return m_viewMode; }

    /** Date range (used by the kEventValueChanged event, carrying the complete start and end dates without being limited by the width of time_t)
    */
    struct DateRange
    {
        time_t start = 0;   // Start date (time_t)
        time_t end = 0;     // End date (time_t)
    };

    /** Listen for the date change event
    * @param [in] callback The callback invoked when the date changes
    * @param [in] callbackID The ID of the callback
    * Parameter description:
    *   wParam: 0 = single mode, 1 = range mode
    *   lParam: in single mode, the time_t value of the selected date (0 on failure);
    *           meaningless in range mode (use pEventData to read the complete start and end values).
    *   pEventData: in range mode, points to DateRange (which holds the complete 64-bit start/end);
    *               it is valid only while this callback is dispatched, do not keep the pointer; it is nullptr in single mode.
    * Recommended usage (range mode):
    *   if (msg.pEventData != nullptr) {
    *       auto* pRange = static_cast<const Calendar::DateRange*>(msg.pEventData);
    *       // Read pRange->start / pRange->end
    *   }
    */
    void AttachDateChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventValueChanged, callback, callbackID); }

    /** Listen for the view mode change event (month/year/ten-year switch)
    * @param [in] callback The callback invoked when the view mode changes
    * @param [in] callbackID The ID of the callback
    * Parameter description:
    *   wParam: the new view mode (0 = month, 1 = year, 2 = ten-year)
    *   lParam: the old view mode (0 = month, 1 = year, 2 = ten-year)
    * Note: in calendar mode a view switch may come from the title button (drill up) or from a grid click (drill down,
    *       e.g. clicking a month in the year view enters the month view, and clicking a year in the ten-year view enters the year view). The flyout title bar must be
    *       refreshed in sync with this event, so that the title does not get out of step with the current view.
    */
    void AttachViewModeChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventViewModeChanged, callback, callbackID); }

    /** Subscribe to the "display period changed" event
     * Fired when the displayed period (the month in the month view, the year in the year/ten-year view) changes,
     * for example when paging with the keyboard (PgUp/PgDn), moving the focus across months/years with the arrow keys, or switching with the navigation buttons.
     * The flyout title bar must be refreshed accordingly (the title text = the currently displayed year/month/ten-year range).
     * Used together with AttachViewModeChanged: both a view mode change (month <-> year <-> ten-year) and a display period change refresh the title,
     * so both paths are covered and the title never gets out of step with the calendar content.
     */
    void AttachDisplayDateChanged(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventDisplayDateChanged, callback, callbackID); }

    /** Keyboard navigation: called by CalendarFlyout when a key is received (arrow keys move the focus, PgUp/PgDn page, Enter/Space select or drill down)
     * @param [in] msg The kEventKeyDown event (holds vkCode / modifierKey)
     * @return true means the key was handled (the event is not dispatched any further)
     * Note: closing the flyout with Esc is handled by the Flyout base class, this method does not handle Esc.
     */
    bool HandleKeyDown(const EventArgs& msg);

    /** Initialize the keyboard focus when opening: so that the focus ring appears as soon as the flyout is shown, without changing the displayed range
     * (prefer the selected date, otherwise today). Used only when the flyout is opened.
     */
    void SetInitialFocus();

public:
    /** Check whether two dates are the same day
    */
    static bool IsSameDay(const struct tm& a, const struct tm& b);

    /** Check whether a date is inside the range (including the boundaries)
    */
    static bool IsDateInRange(const struct tm& date, const struct tm& start, const struct tm& end);

    /** Date comparison: returns true when a < b
    */
    static bool IsDateLess(const struct tm& a, const struct tm& b);

    /** Convert a date to time_t (used for comparison and event parameters)
    */
    static time_t DateToTimeT(const struct tm& date);

    /** Convert time_t to a date
    */
    static struct tm TimeTToDate(time_t t);

    /** Parse a date string (yyyy-mm-dd)
    */
    static bool ParseDateString(const std::string& str, struct tm& date);

    /** Format a date string
    */
    static std::string FormatDateString(const struct tm& date);

    /** Get today's date
    */
    static struct tm GetToday();

protected:
    /** Date cell information (used for hit testing and painting)
    */
    struct DayCell
    {
        int32_t year;       // Actual year
        int32_t month;      // Actual month (1-12)
        int32_t day;        // Actual day
        UiRect rect;        // Drawing area (DIP)
        bool bCurrentMonth; // Whether it belongs to the current month
        bool bToday;        // Whether it is today
        bool bSelected;     // Whether it is selected
        bool bInRange;      // Whether it is inside the range (range mode)
        bool bHovered;      // Whether it is hovered
        bool bDisabled;     // Whether it is outside the selectable range
    };

    /** Get the date cell layout of a given year and month
    * @param [in] year The year
    * @param [in] month The month (1-12)
    * @param [out] cells Outputs 42 cells (6 rows x 7 columns)
    */
    void GetMonthCells(int32_t year, int32_t month, std::vector<DayCell>& cells) const;

    /** Hit test a date cell by coordinates
    * @param [in] pt The mouse position (client coordinates of the control)
    * @param [out] cell The information of the hit cell
    * @return Whether a cell was hit
    */
    bool HitTestDay(const UiPoint& pt, DayCell& cell) const;

    /** Paint the month view
    */
    void DrawMonthView(IRender* pRender, const UiRect& rect);

    /** Paint the year view (12 months)
    */
    void DrawYearView(IRender* pRender, const UiRect& rect);

    /** Paint the ten-year view (12 years)
    */
    void DrawDecadeView(IRender* pRender, const UiRect& rect);

    /** Paint a single date cell
    */
    void DrawDayCell(IRender* pRender, const DayCell& cell, IFont* pFont);

    /** Paint the weekday header row
    */
    void DrawWeekdayHeader(IRender* pRender, const UiRect& rect, IFont* pFont);

    /** Get the weekday header text (from the multi-language resources STRID_PUBLIC_CALENDAR_WEEKDAY_*)
    * @param [in] index 0 = first day (after the offset by firstDayOfWeek)
    */
    std::string GetWeekdayText(int32_t index) const;

    /** Get the month display text (from the multi-language resources STRID_PUBLIC_CALENDAR_MONTH_*)
    * @param [in] month 1~12
    */
    std::string GetMonthText(int32_t month) const;

private:
    /** Selection mode
    */
    Mode m_mode;

    /** Currently displayed year and month
    */
    int32_t m_displayYear;
    int32_t m_displayMonth;

    /** View mode: 0 = month, 1 = year, 2 = ten-year
    */
    int32_t m_viewMode;

    /** First day of the week (0 = Sunday, 1 = Monday)
    */
    int32_t m_firstDayOfWeek;

    /** Selected date (single mode)
    */
    struct tm m_selectedDate;
    bool m_bHasSelectedDate;

    /** Start and end dates of the range selection
    */
    struct tm m_rangeStart;
    struct tm m_rangeEnd;
    bool m_bHasRangeStart;
    bool m_bHasRangeEnd;

    /** Range selection in progress: the mouse is pressed but not released (dragging)
    */
    bool m_bRangeDragging;
    struct tm m_rangeDragStart;

    /** Range selection in progress: the start day has been chosen and the end day is awaited
    *   Two-step click mode: the first click sets the start day (the flyout is not closed), the second click sets the end day (fires the event and closes)
    */
    bool m_bRangeAwaitingEnd;

    /** Range selection in progress: the keyboard is extending the selection with Shift+arrow keys (kept separate from the mouse dragging flag m_bRangeDragging, so that mouse movement cannot take over by accident)
    */
    bool m_bRangeKeyboardExtending;

    /** Hovered date cell
    */
    int32_t m_hoverYear;
    int32_t m_hoverMonth;
    int32_t m_hoverDay;
    bool m_bHasHover;

    /** Limits of the selectable date range
    */
    struct tm m_minDate;
    struct tm m_maxDate;
    bool m_bHasMinDate;
    bool m_bHasMaxDate;

    /** Cache of the cells of the current month (used for hit testing)
    */
    mutable std::vector<DayCell> m_cells;
    mutable bool m_bCellsDirty;

    /** Cache of today's date
    */
    struct tm m_today;
    bool m_bTodayValid;

    /** Refresh the "today" cache (recomputed when the day changes, and makes the month/today highlight repaint)
    */
    void RefreshToday();

    /** Fire the kEventValueChanged event (uniform encoding: single mode wParam=0/lParam=time_t/pEventData=nullptr;
    *   range mode wParam=1/lParam=0/pEventData=a temporary object pointing to DateRange)
    * @param [in] bRange true = range mode, false = single mode
    * Note: the event is dispatched synchronously, and a callback that closes the flyout may destroy this control, so the caller must check
    *       msg.IsSenderExpired() afterwards before accessing this again.
    */
    void EmitDateChanged(bool bRange);

    /** "Focus date" used by keyboard navigation (kept separate from the mouse hover m_hover)
     *  - Month view: the year/month/day are all valid
     *  - Year view: the year/month are valid (the day is fixed to 1)
     *  - Ten-year view: only the year is valid (the month is fixed to m_displayMonth)
     */
    struct tm m_focusDate;
    bool m_bHasFocusDate;

    /** Make sure the keyboard focus date is initialized (on the first key press or when opening): prefer the selected date, otherwise today
     */
    void EnsureFocusDate();

    /** Set the keyboard focus date and make it visible (switching the displayed month/year/ten-year span if needed)
     */
    void SetKeyboardFocusDate(const struct tm& date);

    /** Align the keyboard focus with the current display range (called after paging with PageUp/PageDown, so that the focus stays inside the visible area)
     */
    void SyncFocusToDisplay();

    /** Month view: move the focus by days (deltaDays may be positive or negative, e.g. +-1 / +-7)
     */
    void MoveFocusDay(int32_t deltaDays);

    /** Year view: move the focus by months (deltaMonths may be positive or negative, e.g. +-1 / +-3)
     */
    void MoveFocusMonth(int32_t deltaMonths);

    /** Ten-year view: move the focus by years (deltaYears may be positive or negative, e.g. +-1 / +-3)
     */
    void MoveFocusYear(int32_t deltaYears);

    /** In the month view, select the date the keyboard focus is currently on
     *  - Single mode: SetDate (fires kEventValueChanged, then the flyout closes)
     *  - Range mode: two-step selection (the first step sets the start day, the second the end day)
     * @param [in] msg The keyboard event that triggered this operation (used to detect whether the control has been destroyed after SetDate)
     */
    void SelectFocusedDay(const EventArgs& msg);

    /** Clamp the focus date into the selectable range [m_minDate, m_maxDate] (out-of-range values are clamped to the boundary), so that the focus ring does not land on a disabled day
     */
    void ClampFocusToLimits(struct tm& date);

    /** In the month view, extend the range selection with Shift+arrow keys, anchored at the current focus (live preview that reuses the drawing logic of mouse dragging)
     * @param [in] deltaDays The number of days the focus moves (corresponding to the arrow keys, e.g. +-1 / +-7), which is also the extension step of the range
     */
    void ExtendRangeSelection(int32_t deltaDays);

    /** Confirm the current keyboard range selection (triggered by Enter/Space while m_bRangeKeyboardExtending is true): finalize the interval and fire kEventValueChanged
     * @param [in] msg The keyboard event that triggered this operation (used to detect whether the control has been destroyed)
     */
    void CommitRangeSelection(const EventArgs& msg);

    /** Month view: jump to the first day of the currently displayed month (Home)
     */
    void MoveFocusToMonthStart();

    /** Month view: jump to the last day of the currently displayed month (End)
     */
    void MoveFocusToMonthEnd();

    /** Year view: jump to the first month (Home) / the last month (End) of the currently displayed year
     */
    void MoveFocusToYearBoundary(bool bToStart);

    /** Ten-year view: jump to the first year (Home) / the last year (End) of the current ten-year span
     */
    void MoveFocusToDecadeBoundary(bool bToStart);

    /** Paint the keyboard focus ring (a 1px rounded border inside the given rectangle, using the theme color border_focus_ring)
     */
    void DrawFocusRing(IRender* pRender, const UiRect& rect, float cornerRadius);
};

} // namespace ui

#endif // UI_CONTROL_CALENDAR_H_
