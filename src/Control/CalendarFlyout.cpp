#include "dui/Control/CalendarFlyout.h"
#include "dui/Control/Calendar.h"
#include "dui/Control/Button.h"
#include "dui/Core/WindowBuilder.h"
#include "dui/Core/Keycode.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/Box.h"
#include "dui/Render/IRender.h"
#include "dui/Utils/StringUtil.h"
#include <array>

namespace ui
{

static const std::string kCalendarFlyoutXml = "<Window size=\"280,320\" caption=\"0,0,0,0\" use_system_caption=\"false\" shadow_type=\"default\" shadow_attached=\"true\" layered_window=\"true\" size_box=\"0,0,0,0\">\n"
                                           "  <VBox bkcolor=\"bg_window_card\" width=\"280\" height=\"320\" padding=\"8,8,8,8\">\n"
                                           "    <HBox height=\"32\" child_valign=\"center\">\n"
                                           "      <CalendarNavButton name=\"cal_prev\" direction=\"left\" width=\"28\" height=\"28\" normal_text_color=\"text_default\" hot_text_color=\"color_accent\" pushed_text_color=\"color_accent\" border_size=\"1\" border_round=\"4,4\" normal_border_color=\"border_control_normal\" hovered_border_color=\"border_btn_hovered\" hovered_color=\"bg_btn_hovered\" pressed_color=\"bg_btn_pressed\" tooltip_text_id=\"STRID_PUBLIC_CALENDAR_PREV_MONTH\"/>\n"
                                           "      <Control/>\n"
                                           "      <CalendarTitleButton name=\"cal_title\" text=\"2025-01\" width=\"auto\" height=\"28\" font=\"system_bold_14\" normal_text_color=\"text_default\" hot_text_color=\"color_accent\" pushed_text_color=\"color_accent\" tooltip_text_id=\"STRID_PUBLIC_CALENDAR_SWITCH_VIEW\" text_padding=\"8,0,22,0\" border_size=\"1\" border_round=\"4,4\" normal_border_color=\"border_control_normal\" hovered_border_color=\"border_btn_hovered\" hovered_color=\"bg_btn_hovered\" pressed_color=\"bg_btn_pressed\" disabled_text_color=\"text_muted\" disabled_border_color=\"border_control_disabled\"/>\n"
                                           "      <Control/>\n"
                                           "      <CalendarNavButton name=\"cal_next\" direction=\"right\" width=\"28\" height=\"28\" normal_text_color=\"text_default\" hot_text_color=\"color_accent\" pushed_text_color=\"color_accent\" border_size=\"1\" border_round=\"4,4\" normal_border_color=\"border_control_normal\" hovered_border_color=\"border_btn_hovered\" hovered_color=\"bg_btn_hovered\" pressed_color=\"bg_btn_pressed\" tooltip_text_id=\"STRID_PUBLIC_CALENDAR_NEXT_MONTH\"/>\n"
                                           "    </HBox>\n"
                                           "    <Calendar name=\"cal_grid\" height=\"240\" width=\"stretch\" calendar_mode=\"single\"/>\n"
                                           "    <HBox height=\"32\" child_valign=\"center\">\n"
                                           "      <Button name=\"cal_today\" text_id=\"STRID_PUBLIC_CALENDAR_TODAY\" width=\"60\" height=\"28\" font=\"system_regular_12\" text_color=\"color_accent\" border_size=\"1\" border_round=\"4,4\" normal_border_color=\"border_control_normal\" hovered_border_color=\"border_btn_hovered\" hovered_color=\"bg_btn_hovered\" pressed_color=\"bg_btn_pressed\"/>\n"
                                           "      <Control/>\n"
                                           "      <Button name=\"cal_clear\" text_id=\"STRID_PUBLIC_CALENDAR_CLEAR\" width=\"60\" height=\"28\" font=\"system_regular_12\" text_color=\"text_muted\" border_size=\"1\" border_round=\"4,4\" normal_border_color=\"border_control_normal\" hovered_border_color=\"border_btn_hovered\" hovered_color=\"bg_btn_hovered\" pressed_color=\"bg_btn_pressed\"/>\n"
                                           "    </HBox>\n"
                                           "  </VBox>\n"
                                           "</Window>";

namespace
{
/** Control factory callback: creates the calendar-specific custom controls by class name.
 *  Note: WindowBuilder calls pControl->SetWindow(pWindow) after GlobalManager::CreateControl returns,
 *        so constructing a Window with nullptr here is safe (the same approach as registered controls such as CefControl).
 */
Control* CreateCalendarCustomControl(const std::string& strControlName)
{
    if (strControlName == "CalendarTitleButton") {
        return new CalendarTitleButton(nullptr);
    }
    if (strControlName == "CalendarNavButton") {
        return new CalendarNavButton(nullptr);
    }
    return nullptr;
}

/** Register the custom control factory only once (in the global callback list, to avoid adding it repeatedly)
 */
void EnsureCalendarControlClassesRegistered()
{
    static bool s_registered = false;
    if (!s_registered) {
        s_registered = true;
        GlobalManager::Instance().AddCreateControlCallback(CreateCalendarCustomControl);
    }
}

} // namespace

void CalendarTitleButton::Paint(IRender* pRender, const UiRect& rcPaint)
{
    // Paint the button itself first (background, border, text) to keep the original interactive appearance
    BaseClass::Paint(pRender, rcPaint);

    // Draw the down arrow only while the button is enabled (month/year view: the title can drill up);
    // in the ten-year view the button is disabled and no arrow is drawn, which matches the meaning of "already the coarsest level, nothing left to drill up to".
    if (!IsEnabled()) {
        return;
    }

    // The arrow color matches the current state color of the button text (normal/hovered/pushed)
    UiColor arrowColor = GetArrowColor();
    if (arrowColor.IsEmpty()) {
        return;
    }

    const UiRect rc = GetRect();
    // The arrow is drawn inside the text padding area reserved on the right side of the button (see the right value of text_padding in the XML)
    const int32_t arrowSize = Dpi().GetScaleInt(7);  // The base width and height of the triangle (pixels), enlarged a little for cross-platform legibility
    const int32_t marginRight = Dpi().GetScaleInt(5); // Padding from the right border, shifted left a little to stay away from the edge
    const int32_t cx = rc.right - marginRight - arrowSize / 2;
    const int32_t cy = (rc.top + rc.bottom) / 2;
    std::array<UiPointF, 3> pts = {{
        UiPointF(static_cast<float>(cx - arrowSize / 2), static_cast<float>(cy - arrowSize / 2)),
        UiPointF(static_cast<float>(cx + arrowSize / 2), static_cast<float>(cy - arrowSize / 2)),
        UiPointF(static_cast<float>(cx),                       static_cast<float>(cy + arrowSize / 2)),
    }};

    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory == nullptr) {
        return;
    }
    std::unique_ptr<IPath> path(pRenderFactory->CreatePath());
    if (path == nullptr) {
        return;
    }
    path->AddPolygon(pts.data(), static_cast<int32_t>(pts.size()));
    path->Close();
    std::unique_ptr<IBrush> brush(pRenderFactory->CreateBrush(arrowColor));
    if (brush != nullptr) {
        pRender->FillPath(path.get(), brush.get());
    }
}

UiColor CalendarTitleButton::GetArrowColor() const
{
    ControlStateType state = GetState();
    std::string colorStr;
    if (state == kControlStatePushed) {
        colorStr = GetStateTextColor(kControlStatePushed);
        if (colorStr.empty()) {
            state = kControlStateHot;
        }
    }
    if (state == kControlStateHot) {
        std::string hovered = GetStateTextColor(kControlStateHot);
        if (!hovered.empty()) {
            colorStr = hovered;
        }
        else {
            state = kControlStateNormal;
        }
    }
    if (colorStr.empty()) {
        colorStr = GetStateTextColor(kControlStateNormal);
    }
    if (colorStr.empty()) {
        return UiColor();
    }
    return GetUiColor(colorStr);
}

void CalendarNavButton::SetAttribute(const std::string& strName, const std::string& strValue)
{
    if (strName == "direction") {
        // left = previous month (<), right = next month (>); a missing value is treated as left, which tolerates an omitted configuration
        m_direction = (strValue == "right") ? Direction::kRight : Direction::kLeft;
        return;
    }
    BaseClass::SetAttribute(strName, strValue);
}

void CalendarNavButton::Paint(IRender* pRender, const UiRect& rcPaint)
{
    // Paint the button itself first (background, border) to keep the original interactive appearance; this button has no text and acts as an icon button only.
    BaseClass::Paint(pRender, rcPaint);

    // The icon color matches the current state color of the button text (normal/hovered/pushed)
    UiColor arrowColor = GetArrowColor();
    if (arrowColor.IsEmpty()) {
        return;
    }

    const UiRect rc = GetRect();
    // Draw a centered vector triangle (chevron): the same origin and style as the down arrow of the title;
    // the navigation buttons are always enabled during the lifetime of the flyout, so they are always drawn (no disabled check).
    const int32_t arrowSize = Dpi().GetScaleInt(8); // Edge length of the triangle (pixels), DPI aware
    const int32_t cx = (rc.left + rc.right) / 2;
    const int32_t cy = (rc.top + rc.bottom) / 2;
    const float half = static_cast<float>(arrowSize) / 2.0f;

    std::array<UiPointF, 3> pts;
    if (m_direction == Direction::kRight) {
        // Right-pointing triangle: the apex is on the right and the base on the left (next month)
        pts = std::array<UiPointF, 3>{{
            UiPointF(static_cast<float>(cx) + half, static_cast<float>(cy)),
            UiPointF(static_cast<float>(cx) - half, static_cast<float>(cy) - half),
            UiPointF(static_cast<float>(cx) - half, static_cast<float>(cy) + half),
        }};
    }
    else {
        // Left-pointing triangle: the apex is on the left and the base on the right (previous month)
        pts = std::array<UiPointF, 3>{{
            UiPointF(static_cast<float>(cx) - half, static_cast<float>(cy)),
            UiPointF(static_cast<float>(cx) + half, static_cast<float>(cy) - half),
            UiPointF(static_cast<float>(cx) + half, static_cast<float>(cy) + half),
        }};
    }

    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory == nullptr) {
        return;
    }
    std::unique_ptr<IPath> path(pRenderFactory->CreatePath());
    if (path == nullptr) {
        return;
    }
    path->AddPolygon(pts.data(), static_cast<int32_t>(pts.size()));
    path->Close();
    std::unique_ptr<IBrush> brush(pRenderFactory->CreateBrush(arrowColor));
    if (brush != nullptr) {
        pRender->FillPath(path.get(), brush.get());
    }
}

UiColor CalendarNavButton::GetArrowColor() const
{
    ControlStateType state = GetState();
    std::string colorStr;
    if (state == kControlStatePushed) {
        colorStr = GetStateTextColor(kControlStatePushed);
        if (colorStr.empty()) {
            state = kControlStateHot;
        }
    }
    if (state == kControlStateHot) {
        std::string hovered = GetStateTextColor(kControlStateHot);
        if (!hovered.empty()) {
            colorStr = hovered;
        }
        else {
            state = kControlStateNormal;
        }
    }
    if (colorStr.empty()) {
        colorStr = GetStateTextColor(kControlStateNormal);
    }
    if (colorStr.empty()) {
        return UiColor();
    }
    return GetUiColor(colorStr);
}

CalendarFlyout::CalendarFlyout(Window* pParentWindow):
    Flyout(pParentWindow),
    m_mode(0),
    m_bHasInitDate(false),
    m_bHasInitRange(false),
    m_firstDayOfWeek(1),
    m_pTitleBtn(nullptr),
    m_pCalendar(nullptr)
{
    m_initDate = Calendar::GetToday();
    m_initRangeStart = m_initDate;
    m_initRangeEnd = m_initDate;
}

bool CalendarFlyout::ShowAt(Control* pAnchor, const struct tm& initDate, Placement placement)
{
    m_initDate = initDate;
    m_bHasInitDate = true;
    // Register the calendar-specific custom control (the title button, which draws its own down arrow) only once.
    EnsureCalendarControlClassesRegistered();
    // The calendar needs to receive keyboard events (arrow-key/Enter navigation), so the flyout must take the focus.
    // The DateTime side already has the m_bSuppressCalendarReopen flag, so the focus falling back after closing does not cause a reopen loop.
    SetNoFocus(false);
    return Flyout::ShowAt(pAnchor, kCalendarFlyoutXml, placement);
}

void CalendarFlyout::SetMode(int32_t mode)
{
    m_mode = mode;
    Calendar* pCalendar = GetCalendar();
    if (pCalendar != nullptr) {
        pCalendar->SetMode(mode == 0 ? Calendar::Mode::kSingle : Calendar::Mode::kRange);
    }
}

void CalendarFlyout::SetInitRange(const struct tm& start, const struct tm& end)
{
    m_initRangeStart = start;
    m_initRangeEnd = end;
    m_bHasInitRange = true;
    Calendar* pCalendar = GetCalendar();
    if (pCalendar != nullptr) {
        pCalendar->SetDateRange(start, end);
    }
}

void CalendarFlyout::SetFirstDayOfWeek(int32_t dayOfWeek)
{
    m_firstDayOfWeek = dayOfWeek;
    Calendar* pCalendar = GetCalendar();
    if (pCalendar != nullptr) {
        pCalendar->SetFirstDayOfWeek(dayOfWeek);
    }
}

void CalendarFlyout::SetDateLimit(const std::string& minDate, const std::string& maxDate)
{
    m_minDate = minDate;
    m_maxDate = maxDate;
    Calendar* pCalendar = GetCalendar();
    if (pCalendar != nullptr) {
        pCalendar->SetDateLimit(minDate, maxDate);
    }
}

void CalendarFlyout::AttachDateSelected(const DateSelectedEvent& callback)
{
    if (callback) {
        m_dateSelectedCallbacks.push_back(callback);
    }
}

void CalendarFlyout::AttachDateSelectedEx(const DateSelectedExEvent& callback)
{
    if (callback) {
        m_dateSelectedExCallbacks.push_back(callback);
    }
}

void CalendarFlyout::AttachDateCleared(const DateClearedEvent& callback)
{
    if (callback) {
        m_dateClearedCallbacks.push_back(callback);
    }
}

void CalendarFlyout::OnInitWindow()
{
    BaseClass::OnInitWindow();
    InitControls();
}

void CalendarFlyout::InitControls()
{
    Window* pThisWindow = this;
    if (pThisWindow == nullptr) {
        return;
    }

    // Find the inner controls
    m_pTitleBtn = dynamic_cast<Button*>(pThisWindow->FindControl("cal_title"));
    m_pCalendar = pThisWindow->FindControl("cal_grid");
    Calendar* pCalendar = GetCalendar();

    if (pCalendar != nullptr) {
        // Apply the initial settings
        if (m_mode == 1) {
            pCalendar->SetMode(Calendar::Mode::kRange);
        }
        pCalendar->SetFirstDayOfWeek(m_firstDayOfWeek);
        if (!m_minDate.empty() || !m_maxDate.empty()) {
            pCalendar->SetDateLimit(m_minDate, m_maxDate);
        }
        if (m_bHasInitRange && (m_mode == 1)) {
            // Range mode: use the initial range
            pCalendar->SetDateRange(m_initRangeStart, m_initRangeEnd);
            pCalendar->SetDisplayMonth(m_initRangeStart.tm_year + 1900, m_initRangeStart.tm_mon + 1);
        }
        else if (m_bHasInitDate && (m_mode == 0)) {
            // Single mode: use the initial date
            pCalendar->SetDate(m_initDate);
            pCalendar->SetDisplayMonth(m_initDate.tm_year + 1900, m_initDate.tm_mon + 1);
        }
        else {
            pCalendar->GoToToday();
        }

        // Bind the date-changed event
        pCalendar->AttachDateChanged([this](const EventArgs& msg) {
            // In range mode pEventData points to a Calendar::DateRange (the complete 64-bit start and end values), in single mode it is nullptr.
            // The pointer is valid only while this event is dispatched synchronously, so it must be consumed within this callback when it is passed to an external callback.
            const Calendar::DateRange* pRange = nullptr;
            if ((msg.wParam == 1) && (msg.pEventData != nullptr)) {
                pRange = static_cast<const Calendar::DateRange*>(msg.pEventData);
            }

            // Fire the date-selected callback (two parameters, single mode only)
            std::vector<DateSelectedEvent> callbacks = m_dateSelectedCallbacks;
            for (const DateSelectedEvent& callback : callbacks) {
                callback(msg.wParam, msg.lParam);
            }

            // Fire the date-selected callback (three parameters, carrying the complete 64-bit range)
            std::vector<DateSelectedExEvent> exCallbacks = m_dateSelectedExCallbacks;
            for (const DateSelectedExEvent& callback : exCallbacks) {
                callback(msg.wParam, msg.lParam, pRange);
            }
            Dismiss();
            return true;
        });

        // Bind the view-mode-changed event: when drilling down by clicking the grid in the year/ten-year view (year -> month, ten-year -> year),
        // the title bar must be refreshed as well, otherwise the title keeps the text of the previous view level.
        pCalendar->AttachViewModeChanged([this](const EventArgs&) {
            UpdateTitle();
            return true;
        });

        // Bind the display-period-changed event: paging with the keyboard (PgUp/PgDn), moving the focus across months/years with the arrow keys, and switching with the navigation buttons
        // all change the display period (month/year/ten-year), so the title bar (which shows the current year/month/ten-year range) must be refreshed as well.
        // Handling it through a single event covers every path (including keyboard navigation) and keeps the title in sync with the calendar content.
        pCalendar->AttachDisplayDateChanged([this](const EventArgs&) {
            UpdateTitle();
            return true;
        });

        // Keyboard navigation: initialize the keyboard focus when opening, so that the focus ring appears immediately (prefer the selected date, otherwise today)
        pCalendar->SetInitialFocus();

        // Give the keyboard focus to the Calendar control: defer it until the window has been shown (SetWindowForeground),
        // to avoid the focus being reset, so that the arrow-key/Enter navigation works.
        GlobalManager::Instance().Thread().PostDelayedTask(
            kThreadUI,
            UiBind(this, [this]() {
                Calendar* pCal = GetCalendar();
                if (pCal != nullptr && IsWindow() && (pCal->GetWindow() != nullptr)) {
                    pCal->SetFocus();
                }
            }),
            0);
    }

    // Previous month/next month buttons
    Control* pPrevBtn = pThisWindow->FindControl("cal_prev");
    if (pPrevBtn != nullptr) {
        pPrevBtn->AttachClick([this](const EventArgs&) {
            OnPrevClicked();
            return true;
        });
    }
    Control* pNextBtn = pThisWindow->FindControl("cal_next");
    if (pNextBtn != nullptr) {
        pNextBtn->AttachClick([this](const EventArgs&) {
            OnNextClicked();
            return true;
        });
    }

    // Title button click (switches the view)
    if (m_pTitleBtn != nullptr) {
        m_pTitleBtn->AttachClick([this](const EventArgs&) {
            OnTitleClicked();
            return true;
        });
    }

    // Today button
    Control* pTodayBtn = pThisWindow->FindControl("cal_today");
    if (pTodayBtn != nullptr) {
        pTodayBtn->AttachClick([this](const EventArgs&) {
            OnTodayClicked();
            return true;
        });
    }

    // Clear button
    Control* pClearBtn = pThisWindow->FindControl("cal_clear");
    if (pClearBtn != nullptr) {
        pClearBtn->AttachClick([this](const EventArgs&) {
            OnClearClicked();
            return true;
        });
    }

    UpdateTitle();
}

void CalendarFlyout::UpdateTitle()
{
    Calendar* pCalendar = GetCalendar();
    if (pCalendar == nullptr || m_pTitleBtn == nullptr) {
        return;
    }

    int32_t viewMode = pCalendar->GetViewMode();
    int32_t year = pCalendar->GetDisplayYear();
    int32_t month = pCalendar->GetDisplayMonth();

    std::string text;
    if (viewMode == 0) {
        // Month view: yyyy-MM (language-neutral numeric format)
        text = StringUtil::Printf("%04d-%02d", year, month);
    }
    else if (viewMode == 1) {
        // Year view: yyyy
        text = StringUtil::Printf("%04d", year);
    }
    else {
        // Ten-year view: yyyy - yyyy
        int32_t startYear = (year / 10) * 10;
        text = StringUtil::Printf("%04d - %04d", startYear, startYear + 9);
    }

    // The "interactive/disabled" state of the title button must match the current view level:
    // - Month view / year view: the title can be clicked to drill up (month -> year -> ten-year), and CalendarTitleButton draws the down arrow itself to indicate "expandable".
    // - Ten-year view (viewMode == 2): this is already the coarsest level and there is no further view to drill up to,
    //   so the button is set to the disabled state; CalendarTitleButton does not draw the arrow when it is disabled, which makes "clicking has no effect" visually consistent
    //   and prevents the user from assuming that drilling up is still possible (together with disabled_text_color / disabled_border_color for the disabled look).
    if (viewMode == 2) {
        m_pTitleBtn->SetEnabled(false);
    }
    else {
        m_pTitleBtn->SetEnabled(true);
    }

    m_pTitleBtn->SetText(text);
}

void CalendarFlyout::OnPrevClicked()
{
    Calendar* pCalendar = GetCalendar();
    if (pCalendar != nullptr) {
        pCalendar->NavigatePrev();
        UpdateTitle();
    }
}

void CalendarFlyout::OnNextClicked()
{
    Calendar* pCalendar = GetCalendar();
    if (pCalendar != nullptr) {
        pCalendar->NavigateNext();
        UpdateTitle();
    }
}

void CalendarFlyout::OnTitleClicked()
{
    Calendar* pCalendar = GetCalendar();
    if (pCalendar != nullptr) {
        int32_t mode = pCalendar->GetViewMode();
        if (mode == 0) {
            pCalendar->SetViewMode(1); // month -> year (drill up one level, coarser granularity)
        }
        else if (mode == 1) {
            pCalendar->SetViewMode(2); // year -> ten-year (drill up one level, coarser granularity)
        }
        else {
            // The ten-year view is already the coarsest level and the title has nothing left to drill up to, so it stays unchanged.
            // (Unlike the old implementation it no longer jumps back to the month view, which avoided the direction confusion of skipping the year view.)
        }
        // The title refresh is handled uniformly by the AttachViewModeChanged callback (triggered by SetViewMode), so there is no need to call it again here.
    }
}

void CalendarFlyout::OnTodayClicked()
{
    Calendar* pCalendar = GetCalendar();
    if (pCalendar != nullptr) {
        // Only navigate to the current month; do not commit a date and do not close the flyout.
        // Committing a date is still done by clicking the grid (single mode) or by the "OK" button, which avoids committing "today" and closing on a single misclick.
        pCalendar->GoToToday();
        UpdateTitle();
    }
}

void CalendarFlyout::OnClearClicked()
{
    Calendar* pCalendar = GetCalendar();
    if (pCalendar != nullptr) {
        pCalendar->ClearSelection();
    }
    // Fire the clear callback
    std::vector<DateClearedEvent> callbacks = m_dateClearedCallbacks;
    for (const DateClearedEvent& callback : callbacks) {
        callback();
    }
    Dismiss();
}

Calendar* CalendarFlyout::GetCalendar() const
{
    if (m_pCalendar == nullptr) {
        return nullptr;
    }
    return dynamic_cast<Calendar*>(m_pCalendar);
}

LRESULT CalendarFlyout::OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    Calendar* pCalendar = GetCalendar();
    if (pCalendar != nullptr) {
        bool bHandledLocal = false;
        switch (vkCode) {
        case kVK_LEFT:
        case kVK_RIGHT:
        case kVK_UP:
        case kVK_DOWN:
        case kVK_PRIOR: //PageUp
        case kVK_NEXT:  //PageDown
        case kVK_HOME:  // Home: jump to the start of the period
        case kVK_END:   // End: jump to the end of the period
            // Arrow keys / paging / Home / End: always navigate the calendar (the buttons do not use these keys, so there is no conflict)
            bHandledLocal = HandleCalendarKey(pCalendar, vkCode, modifierKey);
            break;
        case kVK_RETURN:
        case kVK_SPACE:
            // Enter / Space: used to select / drill down only when the calendar itself has the focus;
            // if the focus is on a button, let the base class activate the button, to avoid a double response.
            if (GetFocusControl() == pCalendar) {
                bHandledLocal = HandleCalendarKey(pCalendar, vkCode, modifierKey);
            }
            break;
        default:
            break;
        }
        if (bHandledLocal) {
            bHandled = true;
            return 0;
        }
    }
    // All other keys (including Esc, which closes the flyout) are handled by the base class
    return BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
}

bool CalendarFlyout::HandleCalendarKey(Calendar* pCalendar, VirtualKeyCode vkCode, uint32_t modifierKey)
{
    if (pCalendar == nullptr) {
        return false;
    }
    EventArgs keyMsg;
    keyMsg.eventType = kEventKeyDown;
    keyMsg.vkCode = vkCode;
    keyMsg.modifierKey = modifierKey;
    keyMsg.SetSender(pCalendar); // The flyout may be closed and the Calendar destroyed after SetDate, so IsSenderExpired relies on this
    return pCalendar->HandleKeyDown(keyMsg);
}

} // namespace ui
