#ifndef UI_CONTROL_CALENDAR_FLYOUT_H_
#define UI_CONTROL_CALENDAR_FLYOUT_H_

#include "dui/Control/Flyout.h"
#include "dui/Control/Button.h"
#include "dui/Control/Calendar.h"
#include <ctime>
#include <functional>
#include <vector>

namespace ui
{

class Calendar;

/** Calendar title button: draws a "down arrow" (a vector triangle) on top of Button, replacing the font-dependent downward triangle character.
 *
 *  Background: the original implementation appended the "down small triangle" character (U+25BE) to the title text to indicate "can drill up". That geometric-shape character is missing
 *        from the default fonts on some platforms (Linux/macOS/FreeBSD, etc.), so it was rendered as a tofu box or blank, which looked like a display error.
 *        Drawing it as a vector (IRender + IPath) makes rendering consistent on all platforms and independent of the font coverage.
 *
 *  Behaviour: the arrow is drawn only while the button is enabled (month/year view: the title can drill up); in the ten-year view the button is disabled and no arrow is drawn,
 *        which matches the meaning of "already the coarsest level, nothing left to drill up to". The arrow color follows the current state color of the button text (normal/hovered/pushed).
 */
class DUI_API CalendarTitleButton : public Button
{
    typedef Button BaseClass;
public:
    explicit CalendarTitleButton(Window* pWindow) : Button(pWindow) {}
    CalendarTitleButton(const CalendarTitleButton&) = delete;
    CalendarTitleButton& operator=(const CalendarTitleButton&) = delete;
    virtual ~CalendarTitleButton() override = default;

    /** Custom painting: paint the button itself first (background/border/text), then draw the down arrow in the area reserved on its right
     */
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;

private:
    /** Resolve the arrow color: it matches the current state color of the button text (falling back normal -> hovered -> pushed)
     */
    UiColor GetArrowColor() const;
};

/** Calendar previous/next month navigation button: draws a vector chevron (left/right) itself, replacing the font-dependent single guillemet characters.
 *
 *  Background: the original implementation used text="&#x2039;" / "&#x203A;" (single left/right guillemets) as the navigation arrows. These two characters belong to the "General Punctuation" block;
 *        although their coverage is better than that of the fixed geometric-shape character (U+25BE), they still depend on the runtime font providing the corresponding glyph, and in environments
 *        where the font is missing or the fallback chain does not reach them they can also turn into tofu boxes. Drawing them as a vector (IRender + IPath) gives the same origin as the
 *        down arrow of the title, consistent rendering on all platforms, independence from the font coverage, and a uniform visual style for the three indicators (left/down/right).
 *
 *  Behaviour: the icon color follows the current state color of the button text (normal/hovered/pushed); the direction is specified by the direction attribute of the XML
 *        (left = previous month, right = next month), and a missing value is treated as left. The navigation buttons are always enabled during the lifetime of the flyout, so they are always drawn.
 */
class DUI_API CalendarNavButton : public Button
{
    typedef Button BaseClass;
public:
    enum class Direction
    {
        kLeft,  // Previous month
        kRight, // Next month
    };

    explicit CalendarNavButton(Window* pWindow) : Button(pWindow), m_direction(Direction::kLeft) {}
    CalendarNavButton(const CalendarNavButton&) = delete;
    CalendarNavButton& operator=(const CalendarNavButton&) = delete;
    virtual ~CalendarNavButton() override = default;

    /** Custom painting: paint the button itself first (background/border), then draw the vector chevron (left/right) in the center
     */
    virtual void Paint(IRender* pRender, const UiRect& rcPaint) override;

protected:
    /** Parse the direction attribute from the XML (left/right); the other attributes are handled by the base class
     */
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;

private:
    /** Resolve the icon color: it matches the current state color of the button text (falling back normal -> hovered -> pushed)
     */
    UiColor GetArrowColor() const;

    Direction m_direction; // Direction of the arrow
};

/** Calendar flyout window (CalendarFlyout): hosts the Calendar control on top of Flyout
 *
 *  Features:
 *  1. Creates the Calendar control, the header navigation (previous month/title/next month) and the bottom buttons (today/clear) internally
 *  2. Three-level view navigation (month/year/ten-year): click the title to switch
 *  3. Single mode: closes automatically after a date is selected and fires the callback
 *  4. Range mode: closes automatically after a range is selected and fires the callback
 *
 *  Usage:
 *  @code
 *      ui::CalendarFlyout* pFlyout = new ui::CalendarFlyout(this);
 *      pFlyout->ShowAt(pAnchorButton, initialDate, ui::Flyout::Placement::Bottom);
 *      pFlyout->AttachDateSelectedEx([](WPARAM wParam, LPARAM lParam, const ui::Calendar::DateRange* pRange) {
 *          // wParam: 0 = single mode, 1 = range mode
 *          // lParam: in single mode, the time_t value of the selected date; meaningless in range mode
 *          // pRange: non-null only in range mode, holds the complete 64-bit start/end
 *          return true;
 *      });
 *  @endcode
 */
class DUI_API CalendarFlyout : public Flyout
{
    typedef Flyout BaseClass;
public:
    /** Constructor
    * @param [in] pParentWindow The parent window (the anchor control must belong to this window)
    */
    explicit CalendarFlyout(Window* pParentWindow);
    CalendarFlyout(const CalendarFlyout& r) = delete;
    CalendarFlyout& operator=(const CalendarFlyout& r) = delete;
    virtual ~CalendarFlyout() override = default;

    /** Show the calendar flyout around the anchor control
    * @param [in] pAnchor The anchor control
    * @param [in] initDate The initially selected date (single mode) or the range start date (range mode)
    * @param [in] placement The desired popup placement
    * @return Returns true on success
    */
    bool ShowAt(Control* pAnchor, const struct tm& initDate, Placement placement = Placement::Bottom);

    /** Set the selection mode
    * @param [in] mode 0 = single mode, 1 = range mode
    */
    void SetMode(int32_t mode);

    /** Get the selection mode
    */
    int32_t GetMode() const { return m_mode; }

    /** Set the initial range used by range selection
    * @param [in] start The start date
    * @param [in] end The end date
    */
    void SetInitRange(const struct tm& start, const struct tm& end);

    /** Set the first day of the week
    * @param [in] dayOfWeek 0 = Sunday, 1 = Monday (default 1)
    */
    void SetFirstDayOfWeek(int32_t dayOfWeek);

    /** Set the limits of the selectable date range
    * @param [in] minDate The minimum date (yyyy-mm-dd, empty means unlimited)
    * @param [in] maxDate The maximum date (yyyy-mm-dd, empty means unlimited)
    */
    void SetDateLimit(const std::string& minDate, const std::string& maxDate);

    /** Callback type of the date selection event (fired before closing automatically after a date/range is selected)
    * @param [in] wParam 0 = single mode, 1 = range mode
    * @param [in] lParam In single mode, the time_t value of the selected date; meaningless in range mode.
    * @note This callback only applies to single mode; use DateSelectedExEvent for range mode (it can read the complete 64-bit start and end values).
    */
    typedef std::function<void(WPARAM wParam, LPARAM lParam)> DateSelectedEvent;

    /** Listen for the date selection event (two-parameter signature, single mode only)
    * @param [in] callback The callback function
    */
    void AttachDateSelected(const DateSelectedEvent& callback);

    /** Callback type of the date selection event (recommended, carries the complete 64-bit start and end values)
    * @param [in] wParam 0 = single mode, 1 = range mode
    * @param [in] lParam In single mode, the time_t value of the selected date; meaningless in range mode
    * @param [in] pRange Non-null only in range mode (wParam == 1), points to Calendar::DateRange (which holds the complete 64-bit
    *            start/end); it is nullptr in single mode. The pointer is valid only during this callback and must not be kept.
    */
    typedef std::function<void(WPARAM wParam, LPARAM lParam, const Calendar::DateRange* pRange)> DateSelectedExEvent;

    /** Listen for the date selection event (three-parameter signature, which can obtain the complete 64-bit range values)
    * @param [in] callback The callback function
    */
    void AttachDateSelectedEx(const DateSelectedExEvent& callback);

    /** Listen for the clear-date event (fired when the "Clear" button is clicked)
    */
    typedef std::function<void()> DateClearedEvent;
    void AttachDateCleared(const DateClearedEvent& callback);

public:
    // WindowImplBase interface
    // Note: do not override GetSkinFolder/GetSkinFile - the Flyout base class implementation returns m_skinFolder (the resource path of the parent window)
    // and m_xml (the inline XML passed to ShowAt); overriding them with an empty value makes loading the window XML fail and the window be destroyed right after creation
    virtual void OnInitWindow() override;

    /** Keyboard navigation: intercept the arrow keys/PgUp/PgDn/Enter/Space and forward them to the inner Calendar;
     *  Esc is still handled by the Flyout base class (it closes the flyout).
     */
    virtual LRESULT OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

private:
    /** Initialize the inner controls
    */
    void InitControls();

    /** Update the title text (according to the current view mode)
    */
    void UpdateTitle();

    /** Click handler for the previous month/previous year
    */
    void OnPrevClicked();

    /** Click handler for the next month/next year
    */
    void OnNextClicked();

    /** Title click (switches the view mode)
    */
    void OnTitleClicked();

    /** Today button click
    */
    void OnTodayClicked();

    /** Clear button click
    */
    void OnClearClicked();

    /** Date changed event of the calendar control
    */
    void OnCalendarDateChanged(const EventArgs& msg);

    /** Build a kEventKeyDown event and forward it to Calendar::HandleKeyDown (for OnKeyDownMsg to call)
     */
    bool HandleCalendarKey(Calendar* pCalendar, VirtualKeyCode vkCode, uint32_t modifierKey);

    /** Get the inner Calendar control
    */
    Calendar* GetCalendar() const;

private:
    /** Selection mode: 0 = single mode, 1 = range mode
    */
    int32_t m_mode;

    /** Initial date (single mode)
    */
    struct tm m_initDate;
    bool m_bHasInitDate;

    /** Initial range (range mode)
    */
    struct tm m_initRangeStart;
    struct tm m_initRangeEnd;
    bool m_bHasInitRange;

    /** First day of the week
    */
    int32_t m_firstDayOfWeek;

    /** Limits of the selectable date range
    */
    std::string m_minDate;
    std::string m_maxDate;

    /** Inner control pointers
    */
    Button* m_pTitleBtn;
    Control* m_pCalendar;

    /** Date selection callback (old two-parameter version)
    */
    std::vector<DateSelectedEvent> m_dateSelectedCallbacks;

    /** Date selection callback (extended three-parameter version, carries the complete 64-bit range)
    */
    std::vector<DateSelectedExEvent> m_dateSelectedExCallbacks;

    /** Clear-date callback
    */
    std::vector<DateClearedEvent> m_dateClearedCallbacks;
};

} // namespace ui

#endif // UI_CONTROL_CALENDAR_FLYOUT_H_
