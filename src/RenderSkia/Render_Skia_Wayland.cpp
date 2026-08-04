#include "dui/RenderSkia/Render_Skia_Wayland.h"
#include "dui/RenderSkia/SkRasterWindowContext_Wayland.h"

#ifdef DUI_BUILD_FOR_WAYLAND

#include "dui/RenderSkia/SkiaHeaderBegin.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "include/core/SkRegion.h"

#include "dui/RenderSkia/SkiaHeaderEnd.h"

namespace ui {

static std::unique_ptr<skwindow::WindowContext> MakeRasterForWayland(wl_surface* wlSurface, wl_shm* shm, std::unique_ptr<const skwindow::DisplayParams> params)
{
    return std::make_unique<SkRasterWindowContext_Wayland>(wlSurface, shm, std::move(params));
}

Render_Skia_Wayland::Render_Skia_Wayland(wl_surface* wlSurface, wl_shm* shm, RenderBackendType backendType):
    m_wlSurface(wlSurface),
    m_backendType(backendType)
{
    if (backendType == RenderBackendType::kNativeGL_BackendType) {
        ASSERT(m_wlSurface != nullptr);
        if (m_wlSurface == nullptr) {
            m_backendType = RenderBackendType::kRaster_BackendType;
        }
    }

    m_pWindowContext = MakeRasterForWayland(m_wlSurface, shm,
        std::make_unique<skwindow::DisplayParams>());
    if (m_pWindowContext != nullptr) {
        m_backendType = RenderBackendType::kRaster_BackendType;
    }
}

Render_Skia_Wayland::~Render_Skia_Wayland()
{
}

RenderBackendType Render_Skia_Wayland::GetRenderBackendType() const
{
    return m_backendType;
}

bool Render_Skia_Wayland::Resize(int32_t width, int32_t height)
{
    if ((width <= 0) || (height <= 0)) return false;
    if ((GetWidth() == width) && (GetHeight() == height)) return true;

    if (m_pWindowContext == nullptr) return false;
    m_pWindowContext->resize(width, height);
    return true;
}

int32_t Render_Skia_Wayland::GetWidth() const
{
    if (m_pWindowContext != nullptr) return m_pWindowContext->width();
    return 0;
}

int32_t Render_Skia_Wayland::GetHeight() const
{
    if (m_pWindowContext != nullptr) return m_pWindowContext->height();
    return 0;
}

std::unique_ptr<ui::IRender> Render_Skia_Wayland::Clone()
{
    std::unique_ptr<ui::IRender> pClone = std::make_unique<ui::Render_Skia_Wayland>(
        m_wlSurface, nullptr, m_backendType);
    pClone->Resize(GetWidth(), GetHeight());
    pClone->SetRenderDpi(GetRenderDpi());
    pClone->BitBlt(0, 0, GetWidth(), GetHeight(), this, 0, 0, RopMode::kSrcCopy);
    return pClone;
}

bool Render_Skia_Wayland::PaintAndSwapBuffers(IRenderPaint* pRenderPaint)
{
    ASSERT(pRenderPaint != nullptr);
    if (m_pWindowContext == nullptr || pRenderPaint == nullptr) return false;

    SkRasterWindowContext_Wayland* pWindowContext =
        dynamic_cast<SkRasterWindowContext_Wayland*>(m_pWindowContext.get());
    if (pWindowContext != nullptr) {
        return pWindowContext->PaintAndSwapBuffers(this, pRenderPaint);
    }
    return false;
}

SkSurface* Render_Skia_Wayland::GetSkSurface() const
{
    if (m_pWindowContext == nullptr) return nullptr;
    sk_sp<SkSurface> backbuffer = m_pWindowContext->getBackbufferSurface();
    return backbuffer.get();
}

SkCanvas* Render_Skia_Wayland::GetSkCanvas() const
{
    if (m_pWindowContext == nullptr) return nullptr;
    sk_sp<SkSurface> backbuffer = m_pWindowContext->getBackbufferSurface();
    if (backbuffer == nullptr) return nullptr;
    return backbuffer->getCanvas();
}

bool Render_Skia_Wayland::SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw)
{
    (void)rcWnd; (void)rx; (void)ry; (void)bRedraw;
    return false;
}

bool Render_Skia_Wayland::SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw)
{
    (void)rcWnd; (void)bRedraw;
    return false;
}

void Render_Skia_Wayland::ClearWindowRgn(bool bRedraw)
{
    (void)bRedraw;
}

} // namespace ui

#endif // DUI_BUILD_FOR_WAYLAND
