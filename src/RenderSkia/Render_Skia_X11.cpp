#include "dui/RenderSkia/Render_Skia_X11.h"
#include "tools/window/WindowContext.h"
#include "tools/window/DisplayParams.h"

#ifdef DUI_BUILD_FOR_X11

// Skia's unix headers, inside the guard: they include <X11/X.h>, and this file is
// compiled on Windows and macOS too, where its whole body is switched off.
#include "tools/window/unix/RasterWindowContext_unix.h"
#include "tools/window/unix/XlibWindowInfo.h"

#include "dui/RenderSkia/SkiaHeaderBegin.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "include/core/SkRegion.h"

#include "dui/RenderSkia/SkiaHeaderEnd.h"

namespace ui {

static std::unique_ptr<skwindow::WindowContext> MakeRasterForX11(Display* display, Window window, Visual* visual, int depth, int width, int height, std::unique_ptr<const skwindow::DisplayParams> params)
{
    XVisualInfo visualInfo{}; visualInfo.visual = visual; visualInfo.depth = depth;
    skwindow::XlibWindowInfo info{display, window, nullptr, &visualInfo, width, height};
    return skwindow::MakeRasterForXlib(info, std::move(params));
}

Render_Skia_X11::Render_Skia_X11(Display* display, Window window, Visual* visual, int depth, int width, int height, RenderBackendType backendType):
    m_display(display), m_window(window), m_visual(visual), m_depth(depth), m_width(width), m_height(height),
    m_backendType(backendType)
{
    if (backendType == RenderBackendType::kNativeGL_BackendType) {
        if (m_display == nullptr) {
            m_backendType = RenderBackendType::kRaster_BackendType;
        }
    }

    m_pWindowContext = MakeRasterForX11(m_display, m_window, m_visual, m_depth, m_width, m_height,
        std::make_unique<skwindow::DisplayParams>());
    if (m_pWindowContext != nullptr) {
        m_backendType = RenderBackendType::kRaster_BackendType;
    }
}

Render_Skia_X11::~Render_Skia_X11()
{
}

RenderBackendType Render_Skia_X11::GetRenderBackendType() const
{
    return m_backendType;
}

bool Render_Skia_X11::Resize(int32_t width, int32_t height)
{
    if ((width <= 0) || (height <= 0)) return false;
    if ((GetWidth() == width) && (GetHeight() == height)) return true;

    if (m_pWindowContext == nullptr) return false;
    m_pWindowContext->resize(width, height);
    return true;
}

int32_t Render_Skia_X11::GetWidth() const
{
    if (m_pWindowContext != nullptr) return m_pWindowContext->width();
    return 0;
}

int32_t Render_Skia_X11::GetHeight() const
{
    if (m_pWindowContext != nullptr) return m_pWindowContext->height();
    return 0;
}

std::unique_ptr<ui::IRender> Render_Skia_X11::Clone()
{
    std::unique_ptr<ui::IRender> pClone = std::make_unique<ui::Render_Skia_X11>(
        m_display, m_window, m_visual, m_depth, m_width, m_height, m_backendType);
    pClone->Resize(GetWidth(), GetHeight());
    pClone->SetRenderDpi(GetRenderDpi());
    pClone->BitBlt(0, 0, GetWidth(), GetHeight(), this, 0, 0, RopMode::kSrcCopy);
    return pClone;
}

bool Render_Skia_X11::PaintAndSwapBuffers(IRenderPaint* pRenderPaint)
{
    ASSERT(pRenderPaint != nullptr);
    if (m_pWindowContext == nullptr || pRenderPaint == nullptr) return false;

    UiRect rcPaint;
    if (!pRenderPaint->GetUpdateRect(rcPaint)) {
        rcPaint = UiRect(0, 0, GetWidth(), GetHeight());
    }
    pRenderPaint->DoPaint(rcPaint);
    m_pWindowContext->swapBuffers();
    return true;
}

SkSurface* Render_Skia_X11::GetSkSurface() const
{
    if (m_pWindowContext == nullptr) return nullptr;
    sk_sp<SkSurface> backbuffer = m_pWindowContext->getBackbufferSurface();
    return backbuffer.get();
}

SkCanvas* Render_Skia_X11::GetSkCanvas() const
{
    if (m_pWindowContext == nullptr) return nullptr;
    sk_sp<SkSurface> backbuffer = m_pWindowContext->getBackbufferSurface();
    if (backbuffer == nullptr) return nullptr;
    return backbuffer->getCanvas();
}

bool Render_Skia_X11::SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw)
{
    (void)rcWnd; (void)rx; (void)ry; (void)bRedraw;
    return false;
}

bool Render_Skia_X11::SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw)
{
    (void)rcWnd; (void)bRedraw;
    return false;
}

void Render_Skia_X11::ClearWindowRgn(bool bRedraw)
{
    (void)bRedraw;
}

} // namespace ui

#endif // DUI_BUILD_FOR_X11
