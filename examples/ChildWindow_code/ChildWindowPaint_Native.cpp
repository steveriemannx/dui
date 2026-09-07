#include "ChildWindowPaint.h"

#if !defined(DUI_BUILD_FOR_WIN) && !defined(DUI_BUILD_FOR_MACOS)

struct ChildWindowPaint::TImpl {};

ChildWindowPaint::ChildWindowPaint(ui::ChildWindow* pChildWindow):
    m_pChildWindow(pChildWindow),
    m_impl(nullptr)
{
}

ChildWindowPaint::~ChildWindowPaint()
{
    delete m_impl;
    m_impl = nullptr;
}

void ChildWindowPaint::PaintChildWindow(const ui::UiRect&, const ui::NativeMsg&, bool)
{
    if (m_pChildWindow == nullptr) {
        return;
    }
    ui::IRender* pRender = m_pChildWindow->GetChildWindowRender();
    if (pRender == nullptr) {
        return;
    }

    ui::UiRect childWndRect;
    m_pChildWindow->GetChildWindowRect(childWndRect);
    const ui::UiRectF rect(0.0f, 0.0f,
                           static_cast<float>(childWndRect.Width()),
                           static_cast<float>(childWndRect.Height()));
    // X11 child windows use their own raster surface. Paint an opaque base so
    // the child does not expose the parent through its transparent buffer.
    pRender->FillRect(rect, ui::UiColor(ui::UiColors::Blue));
    pRender->DrawRect(rect, ui::UiColor(ui::UiColors::White), 2.0f);
}

#endif
