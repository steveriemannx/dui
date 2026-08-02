#include "duilib/RenderSkia/WindowRgn_Windows.h"

#ifdef DUILIB_BUILD_FOR_WIN

#include "duilib/RenderSkia/SkiaHeaderBegin.h"

#include "include/core/SkPath.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkPathMeasure.h"

#include "duilib/RenderSkia/SkiaHeaderEnd.h"

namespace ui
{
// Generate an HRGN from Skia's rounded-rectangle path, ensuring it matches drawRoundRect exactly
static HRGN CreateHRGNFromSkiaRoundRect(const SkRect& skRect, SkScalar rx, SkScalar ry)
{
    // Create a Skia rounded-rectangle path (ensuring the path is closed)
    SkPathBuilder pathBuilder;
    pathBuilder.addRRect(SkRRect::MakeRectXY(skRect, rx, ry));
    pathBuilder.close(); // Explicitly close the path to avoid missing endpoints

    // Use SkPathMeasure to discretize the path into a dense set of points
    SkPathMeasure pathMeasure(pathBuilder.snapshot(), true); // true means a closed path
    std::vector<POINT> points;

    // The total length of the path
    SkScalar pathLength = pathMeasure.getLength();
    if (pathLength <= 0) {
        return nullptr;
    }

    // The discretization step (smaller is finer; 1.0f is recommended, balancing precision and performance)
    const SkScalar step = 1.0f;
    SkScalar distance = 0.0f;

    // Iterate over the path and extract points by step
    while (distance <= pathLength) {
        SkPoint pos;
        SkVector tan;
        // Get the point coordinates and tangent at the specified distance
        if (pathMeasure.getPosTan(distance, &pos, &tan)) {
            // Convert to a Windows POINT (float to int; rounding is more accurate)
            POINT pt = {
                static_cast<LONG>(std::round(pos.x())),
                static_cast<LONG>(std::round(pos.y()))
            };
            // Avoid adding duplicate points
            if (points.empty() || !(points.back().x == pt.x && points.back().y == pt.y)) {
                points.push_back(pt);
            }
        }
        distance += step;
    }

    // Remove the last duplicate start point (duplicated when the path is closed)
    if ((points.size() >= 2) &&
        (points.front().x == points.back().x) &&
        (points.front().y == points.back().y)) {
        points.pop_back();
    }

    // Validate the point set (at least 3 points are needed to form a polygon)
    if (points.size() < 3) {
        return nullptr;
    }

    // Create a polygon HRGN (ALTERNATE fill mode suits rounded corners)
    HRGN hRgn = ::CreatePolygonRgn(points.data(), static_cast<int>(points.size()), ALTERNATE);
    return hRgn;
}

bool WindowRgn::SetWindowRoundRectRgn(HWND hWnd, const UiRect& rcWnd, float rx, float ry, bool bRedraw)
{
    ASSERT(::IsWindow(hWnd));
    if (!::IsWindow(hWnd)) {
        return false;
    }
    SkRect skRect;
    skRect.fLeft = 0;
    skRect.fTop = 0;
    skRect.fRight = skRect.fLeft + rcWnd.Width();
    skRect.fBottom = skRect.fTop + rcWnd.Height();

    HRGN hRgn = CreateHRGNFromSkiaRoundRect(skRect, rx, ry);
    int nRet = ::SetWindowRgn(hWnd, hRgn, bRedraw ? TRUE : FALSE);
    return nRet != 0;
}

bool WindowRgn::SetWindowRectRgn(HWND hWnd, const UiRect& rcWnd, bool bRedraw)
{
    ASSERT(::IsWindow(hWnd));
    if (!::IsWindow(hWnd)) {
        return false;
    }
    HRGN hRgn = ::CreateRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom);
    int nRet = ::SetWindowRgn(hWnd, hRgn, bRedraw ? TRUE : FALSE);
    return nRet != 0;
}

void WindowRgn::ClearWindowRgn(HWND hWnd, bool bRedraw)
{
    ASSERT(::IsWindow(hWnd));
    if (!::IsWindow(hWnd)) {
        return;
    }
    ::SetWindowRgn(hWnd, nullptr, bRedraw ? TRUE : FALSE);
}

} //namespace ui

#endif //DUILIB_BUILD_FOR_WIN
