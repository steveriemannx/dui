#ifndef UI_CONTROL_MACTRAFFICLIGHTS_H_
#define UI_CONTROL_MACTRAFFICLIGHTS_H_

#include "dui/dui_defs.h"

#if defined(DUI_BUILD_FOR_MACOS)

#include "dui/Core/Control.h"

namespace ui
{
/** Self-drawn macOS traffic-light buttons (close/minimize/fullscreen) painted
 *  by dui itself. Only used on macOS (instantiated by WindowImplBase for the
 *  caption bar); not compiled on other platforms. Draws the three circles with
 *  the macOS colors, tracks the hover/pressed state and reports a click
 *  through the standard kEventClick — the window reads GetClickedZone() to
 *  dispatch it (green = toggle fullscreen, like the native macOS behavior).
 */
class DUI_API MacTrafficLights : public Control
{
public:
    enum class ClickZone
    {
        kClose = 0,
        kMinimize = 1,
        kZoom = 2,
        kNone = 3,
    };

    MacTrafficLights(Window* pWindow);

    /** Set the window activation state: true keeps the buttons colored,
     *  false switches them to the macOS inactive gray.
     */
    void SetWindowActive(bool bActive);
    bool IsWindowActive() const { return m_bWindowActive; }

    /** The button zone of the most recent click (read inside the click handler). */
    ClickZone GetClickedZone() const { return m_clickedZone; }

protected:
    void Paint(IRender* pRender, const UiRect& rcPaint) override;
    void HandleEvent(const EventArgs& msg) override;
    bool CanPlaceCaptionBar() const override { return true; }

private:
    ClickZone HitTestZone(const UiPoint& pt) const;
    UiColor GetZoneColor(ClickZone zone, bool bHover, bool bPressed) const;

    bool m_bWindowActive = true;
    ClickZone m_hoverZone = ClickZone::kNone;
    ClickZone m_pressedZone = ClickZone::kNone;
    ClickZone m_clickedZone = ClickZone::kNone;
};

} // namespace ui

#endif // DUI_BUILD_FOR_MACOS

#endif // UI_CONTROL_MACTRAFFICLIGHTS_H_
