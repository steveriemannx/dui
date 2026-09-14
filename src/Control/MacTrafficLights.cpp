#include "dui/Control/MacTrafficLights.h"

#if defined(DUI_BUILD_FOR_MACOS)

#include "dui/Core/DpiManager.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Render/IRender.h"
#include <memory>
#include <vector>

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

// Glyphs drawn inside the circles while the pointer is on the traffic lights
// (macOS reveals all three at once, painted in a darker tone of the circle color).
// Sizes match the buttons drawn by macOS itself: the close "x" is 7pt across and
// the minimize "-" 8pt long, both with a 2pt stroke and rounded ends.
const float kGlyphHalfSize = 3.5f;   // half extent of the close "x"
const float kMinusHalfSize = 4.0f;   // half length of the minimize "-"
const float kGlyphPenWidth = 2.0f;   // stroke width of the "x" and "-"
// The zoom glyph is a pair of triangles laid out on the 45 degree diagonal that
// runs from the top-left to the bottom-right, with the apexes pointing away from
// the center ("expand").
const float kTriangleApex = 4.4f;    // distance from the circle center to the apex
const float kTriangleBase = 1.0f;    // distance from the circle center to the base
const float kTriangleHalf = 3.4f;    // half length of the base

// macOS light-appearance traffic-light colors (ARGB).
const UiColor kCloseColor = UiColor(0xFFFF5F57);
const UiColor kClosePressed = UiColor(0xFFD93B33);
const UiColor kMinimizeColor = UiColor(0xFFFEBC2E);
const UiColor kMinimizePressed = UiColor(0xFFCF9A16);
const UiColor kZoomColor = UiColor(0xFF28C840);
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

UiColor MacTrafficLights::GetZoneColor(ClickZone zone, bool bPressed) const
{
    if (!m_bWindowActive) {
        return kInactiveColor;
    }
    // Hovering does not shade the circle: macOS reveals the glyphs instead
    // (only the pressed state darkens the color).
    switch (zone) {
    case ClickZone::kClose:
        return bPressed ? kClosePressed : kCloseColor;
    case ClickZone::kMinimize:
        return bPressed ? kMinimizePressed : kMinimizeColor;
    case ClickZone::kZoom:
        return bPressed ? kZoomPressed : kZoomColor;
    default:
        return kInactiveColor;
    }
}

void MacTrafficLights::PaintGlyphs(IRender* pRender, float fScale, const float (&cx)[3], float cy) const
{
    // macOS shows the glyphs of all three buttons as soon as the pointer is
    // anywhere on the traffic lights, not only on the button under the pointer,
    // and paints them in a darker tone of the circle color. Resolving that color
    // up front keeps the round end caps below from double-compositing (a
    // translucent stroke and its cap overlap).
    const auto glyphColor = [](const UiColor& circleColor) {
        return UiColor(255, (uint8_t)(circleColor.GetR() * 3 / 5),
                            (uint8_t)(circleColor.GetG() * 3 / 5),
                            (uint8_t)(circleColor.GetB() * 3 / 5));
    };
    const auto zoneColor = [&](ClickZone zone) {
        return glyphColor(GetZoneColor(zone, m_pressedZone == zone));
    };

    const float fStroke = kGlyphPenWidth * fScale;
    const float fCap = fStroke / 2.0f;
    // The render backend ignores the pen caps, so a disc is drawn at both ends of
    // every stroke; each stroke is shortened by the cap radius, so the total
    // extent still matches the sizes macOS uses.
    const float fHalf = (kGlyphHalfSize - kGlyphPenWidth * 0.5f) * fScale;
    const float fMinusHalf = (kMinusHalfSize - kGlyphPenWidth * 0.5f) * fScale;
    const auto stroke = [&](const UiColor& color, const UiPointF& pt1, const UiPointF& pt2) {
        pRender->DrawLine(pt1, pt2, color, fStroke);
        pRender->FillCircle(pt1, fCap, color);
        pRender->FillCircle(pt2, fCap, color);
    };

    // Close: "x"
    const UiColor closeColor = zoneColor(ClickZone::kClose);
    stroke(closeColor, UiPointF(cx[0] - fHalf, cy - fHalf), UiPointF(cx[0] + fHalf, cy + fHalf));
    stroke(closeColor, UiPointF(cx[0] + fHalf, cy - fHalf), UiPointF(cx[0] - fHalf, cy + fHalf));

    // Minimize: "-"
    const UiColor minimizeColor = zoneColor(ClickZone::kMinimize);
    stroke(minimizeColor, UiPointF(cx[1] - fMinusHalf, cy), UiPointF(cx[1] + fMinusHalf, cy));

    // Zoom (enter fullscreen): two triangles on the top-left <-> bottom-right
    // diagonal, apexes pointing away from the center.
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory == nullptr) {
        return;
    }
    const float fDiagonal = 0.7071068f; // 1 / sqrt(2)
    const float fApex = kTriangleApex * fScale * fDiagonal;
    const float fBase = kTriangleBase * fScale * fDiagonal;
    const float fBaseHalf = kTriangleHalf * fScale * fDiagonal;
    const UiColor zoomColor = zoneColor(ClickZone::kZoom);
    for (int i = 0; i < 2; ++i) {
        // Towards the top-left for the first triangle, towards the bottom-right for
        // the second one; the perpendicular of that diagonal spreads the base.
        const float fSign = (i == 0) ? 1.0f : -1.0f;
        std::vector<UiPointF> points;
        points.reserve(3);
        points.emplace_back(cx[2] + fSign * fApex, cy + fSign * fApex);
        points.emplace_back(cx[2] + fSign * fBase + fBaseHalf, cy + fSign * fBase - fBaseHalf);
        points.emplace_back(cx[2] + fSign * fBase - fBaseHalf, cy + fSign * fBase + fBaseHalf);
        std::unique_ptr<IPath> path(pRenderFactory->CreatePath());
        std::unique_ptr<IBrush> brush(pRenderFactory->CreateBrush(zoomColor));
        if ((path == nullptr) || (brush == nullptr)) {
            continue;
        }
        path->AddPolygon(points.data(), (int32_t)points.size());
        path->Close();
        pRender->FillPath(path.get(), brush.get());
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
    const float fCenters[3] = { GetRect().left + kCloseX * fScale,
                                GetRect().left + kMinimizeX * fScale,
                                GetRect().left + kZoomX * fScale };
    const float cy = GetRect().top + GetRect().Height() / 2.0f;
    const float radius = kRadius * fScale;
    for (int i = 0; i < 3; ++i) {
        const ClickZone zone = static_cast<ClickZone>(i);
        const bool bPressed = (m_pressedZone == zone);
        pRender->FillCircle(UiPointF(fCenters[i], cy), radius, GetZoneColor(zone, bPressed));
    }

    // The glyphs appear while the pointer is on the traffic lights (or a button is
    // held down); macOS uses the same "reveal" for the whole group.
    if ((m_hoverZone != ClickZone::kNone) || (m_pressedZone != ClickZone::kNone)) {
        PaintGlyphs(pRender, fScale, fCenters, cy);
    }
}

} // namespace ui

#endif // DUI_BUILD_FOR_MACOS
