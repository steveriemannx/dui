#include "dui/Control/MacTrafficLights.h"

#if defined(DUI_BUILD_FOR_MACOS)

#include "dui/Core/DpiManager.h"
#include "dui/Render/IRender.h"

namespace ui
{

namespace
{
// macOS traffic-light geometry, relative to the control (values in points,
// scaled by the DPI factor at draw time). Visual circles are 14pt in diameter
// ("14px"), with a slightly larger 16pt hit area; the centers keep a ~23pt
// spacing so the circles do not overlap and sit at the natural macOS positions.
const float kCloseX = 15.75f;
const float kMinimizeX = 38.5f;
const float kZoomX = 61.5f;
const float kRadius = 7.0f;    // visual radius: 14pt diameter = 28px @2x
const float kHitRadius = 8.0f; // hit radius: 16pt diameter = 32px @2x

// macOS light-appearance traffic-light colors (ARGB).
const UiColor kCloseColor = UiColor(0xFFFF5F57);
const UiColor kCloseHover = UiColor(0xFFE0443E);
const UiColor kClosePressed = UiColor(0xFFD93B33);
const UiColor kMinimizeColor = UiColor(0xFFFEBC2E);
const UiColor kMinimizeHover = UiColor(0xFFD89E1C);
const UiColor kMinimizePressed = UiColor(0xFFCF9A16);
const UiColor kZoomColor = UiColor(0xFF28C840);
const UiColor kZoomHover = UiColor(0xFF1FA736);
const UiColor kZoomPressed = UiColor(0xFF18A02C);
const UiColor kInactiveColor = UiColor(0xFFC9C9C9);
} // namespace

MacTrafficLights::MacTrafficLights(Window* pWindow)
    : Control(pWindow)
{
    // Receive mouse events so the hover/pressed states and clicks work.
    SetMouseEnabled(true);
}

void MacTrafficLights::SetWindowActive(bool bActive)
{
    if (m_bWindowActive != bActive) {
        m_bWindowActive = bActive;
        Invalidate();
    }
}

MacTrafficLights::ClickZone MacTrafficLights::HitTestZone(const UiPoint& pt) const
{
    // pt is in control-local coordinates, while the geometry constants are in
    // points; scale them by the DPI factor so the hit zones match the circles
    // painted by Paint().
    const float fScale = Dpi().GetDisplayScale();
    const float cx[3] = { kCloseX * fScale, kMinimizeX * fScale, kZoomX * fScale };
    const float cy = GetRect().Height() / 2.0f;
    const float radius = kHitRadius * fScale;
    for (int i = 0; i < 3; ++i) {
        const float dx = pt.x - cx[i];
        const float dy = pt.y - cy;
        if ((dx * dx + dy * dy) <= (radius * radius)) {
            return static_cast<ClickZone>(i);
        }
    }
    return ClickZone::kNone;
}

void MacTrafficLights::HandleEvent(const EventArgs& msg)
{
    if (msg.eventType == kEventMouseMove) {
        UiPoint pt(msg.ptMouse);
        pt.Offset(-GetRect().left, -GetRect().top);
        const ClickZone zone = HitTestZone(pt);
        if (zone != m_hoverZone) {
            m_hoverZone = zone;
            Invalidate();
        }
    }
    else if (msg.eventType == kEventMouseLeave) {
        if (m_hoverZone != ClickZone::kNone) {
            m_hoverZone = ClickZone::kNone;
            Invalidate();
        }
    }
    else if (msg.eventType == kEventMouseButtonDown) {
        UiPoint pt(msg.ptMouse);
        pt.Offset(-GetRect().left, -GetRect().top);
        m_pressedZone = HitTestZone(pt);
        SetMouseFocused(true);
        Invalidate();
    }
    else if (msg.eventType == kEventMouseButtonUp) {
        if (m_pressedZone != ClickZone::kNone) {
            UiPoint pt(msg.ptMouse);
            pt.Offset(-GetRect().left, -GetRect().top);
            if (HitTestZone(pt) == m_pressedZone) {
                m_clickedZone = m_pressedZone;
                SendEvent(kEventClick);
            }
        }
        m_pressedZone = ClickZone::kNone;
        SetMouseFocused(false);
        Invalidate();
    }
    Control::HandleEvent(msg);
}

UiColor MacTrafficLights::GetZoneColor(ClickZone zone, bool bHover, bool bPressed) const
{
    if (!m_bWindowActive) {
        return kInactiveColor;
    }
    switch (zone) {
    case ClickZone::kClose:
        return bPressed ? kClosePressed : (bHover ? kCloseHover : kCloseColor);
    case ClickZone::kMinimize:
        return bPressed ? kMinimizePressed : (bHover ? kMinimizeHover : kMinimizeColor);
    case ClickZone::kZoom:
        return bPressed ? kZoomPressed : (bHover ? kZoomHover : kZoomColor);
    default:
        return kInactiveColor;
    }
}

void MacTrafficLights::Paint(IRender* pRender, const UiRect& rcPaint)
{
    if (GetRect().IsEmpty()) {
        return;
    }
    // Paint coordinates are window-absolute (the render origin is not translated
    // per control), while the geometry constants are in points. Scale them by the
    // DPI factor and offset by the control rect, or the circles would land
    // outside the control and be clipped away by its rect clip.
    const float fScale = Dpi().GetDisplayScale();
    const float cx[3] = { kCloseX * fScale, kMinimizeX * fScale, kZoomX * fScale };
    const float cy = GetRect().top + GetRect().Height() / 2.0f;
    const float radius = kRadius * fScale;
    for (int i = 0; i < 3; ++i) {
        const ClickZone zone = static_cast<ClickZone>(i);
        const bool bHover = (m_hoverZone == zone);
        const bool bPressed = (m_pressedZone == zone);
        pRender->FillCircle(UiPointF(GetRect().left + cx[i], cy), radius, GetZoneColor(zone, bHover, bPressed));
    }
}

} // namespace ui

#endif // DUI_BUILD_FOR_MACOS
