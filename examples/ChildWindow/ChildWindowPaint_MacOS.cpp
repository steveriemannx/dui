#include "ChildWindowPaint.h"

#if defined (DUI_BUILD_FOR_MACOS)

#include <stdlib.h>
#include <time.h>
#include <math.h>

// Define the graphics type enumeration
typedef enum {
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_LINE,
    SHAPE_COUNT
} ShapeType;

struct ChildWindowPaint::TImpl
{
    // Random number generator state
    unsigned int nSeed = 0;
};

static ui::UiColor GetRandomColor()
{
    return ui::UiColor((uint8_t)(rand() % 256),
                   (uint8_t)(rand() % 256),
                   (uint8_t)(rand() % 256));
}

static void DrawRandomPatterns(ui::IRender* pRender, int32_t windowWidth, int32_t windowHeight, int count)
{
    if (pRender == nullptr) {
        return;
    }
    for (int i = 0; i < count; ++i) {
        switch (rand() % SHAPE_COUNT) {
        case SHAPE_RECTANGLE: {
            // Random rectangle position and size
            int32_t left = rand() % (windowWidth > 0 ? windowWidth : 1);
            int32_t top = rand() % (windowHeight > 0 ? windowHeight : 1);
            int32_t width = 10 + rand() % 80;
            int32_t height = 10 + rand() % 80;
            ui::UiRectF rect((float)left, (float)top, (float)(left + width), (float)(top + height));
            if (rand() % 2 == 0) {
                pRender->FillRect(rect, GetRandomColor());
            }
            else {
                pRender->DrawRect(rect, (ui::UiColor)GetRandomColor(), 1.0f);
            }
            break;
        }

        case SHAPE_CIRCLE: {
            // Random circle position and radius
            float centerX = (float)(rand() % (windowWidth > 0 ? windowWidth : 1));
            float centerY = (float)(rand() % (windowHeight > 0 ? windowHeight : 1));
            float radius = (float)(5 + rand() % 50);
            ui::UiPointF center(centerX, centerY);
            if (rand() % 2 == 0) {
                pRender->FillCircle(center, radius, GetRandomColor());
            }
            else {
                pRender->DrawCircle(center, radius, (ui::UiColor)GetRandomColor(), 1.0f);
            }
            break;
        }

        case SHAPE_LINE: {
            // Random line start and end points
            ui::UiPointF start((float)(rand() % (windowWidth > 0 ? windowWidth : 1)),
                               (float)(rand() % (windowHeight > 0 ? windowHeight : 1)));
            ui::UiPointF end((float)(rand() % (windowWidth > 0 ? windowWidth : 1)),
                             (float)(rand() % (windowHeight > 0 ? windowHeight : 1)));
            pRender->DrawLine(start, end, (ui::UiColor)GetRandomColor(), 1.0f);
            break;
        }

        default:
            break;
        }
    }
}

ChildWindowPaint::ChildWindowPaint(ui::ChildWindow* pChildWindow) :
    m_pChildWindow(pChildWindow),
    m_impl(nullptr)
{
    ::srand((unsigned int)::time(nullptr));
    if (m_impl == nullptr) {
        m_impl = new TImpl;
    }
}

ChildWindowPaint::~ChildWindowPaint()
{
    if (m_impl != nullptr) {
        delete m_impl;
        m_impl = nullptr;
    }
}

void ChildWindowPaint::PaintChildWindow(const ui::UiRect& rcPaint, const ui::NativeMsg& nativeMsg, bool bPaintFps)
{
    (void)nativeMsg;
    (void)rcPaint;
    if (m_pChildWindow == nullptr) {
        return;
    }
    ui::IRender* pRender = m_pChildWindow->GetChildWindowRender();
    if (pRender == nullptr) {
        return;
    }

    if (bPaintFps) {
        ui::UiRect childWndRect;
        m_pChildWindow->GetChildWindowRect(childWndRect);
        DrawRandomPatterns(pRender, childWndRect.Width(), childWndRect.Height(), 40);
    }
    else {
        // Show a solid color
        ui::UiRect childWndRect;
        m_pChildWindow->GetChildWindowRect(childWndRect);
        ui::UiRectF rect(0.0f, 0.0f, (float)childWndRect.Width(), (float)childWndRect.Height());
        pRender->FillRect(rect, ui::UiColor(ui::UiColors::Blue));
    }
}

#endif // DUI_BUILD_FOR_MACOS
