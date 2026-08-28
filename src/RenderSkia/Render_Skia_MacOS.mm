#include "dui/RenderSkia/Render_Skia_MacOS.h"
#include "dui/RenderSkia/SkGLWindowContext_MacOS.h"
#include "dui/RenderSkia/SkMetalWindowContext_MacOS.h"
#include "dui/RenderSkia/SkRasterWindowContext_MacOS.h"

#if defined(DUI_BUILD_FOR_MACOS)

#include "dui/RenderSkia/SkiaHeaderBegin.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"

#include "dui/RenderSkia/SkiaHeaderEnd.h"

namespace ui {

/** Create a WindowContext implemented with Raster
* @param [in] nsView The associated NSView*, can be nullptr
* @param [in] params Parameters related to display
*/
static std::unique_ptr<skwindow::WindowContext> MakeRasterForMac(void* nsView, std::unique_ptr<const skwindow::DisplayParams> params)
{
    std::unique_ptr<skwindow::WindowContext> ctx(new SkRasterWindowContext_MacOS(nsView, std::move(params)));
    return ctx;
}

/** Create a WindowContext implemented with GPU (Metal)
* @param [in] nsView The associated NSView*, can be nullptr
* @param [in] params Parameters related to display
* Requires libskia.a built with skia_use_metal=true.
*/
static std::unique_ptr<skwindow::WindowContext> MakeMetalForMac(void* nsView, std::unique_ptr<const skwindow::DisplayParams> params)
{
    std::unique_ptr<skwindow::WindowContext> ctx(new SkMetalWindowContext_MacOS(nsView, std::move(params)));
    if (!ctx->isValid()) {
        return nullptr;
    }
    return ctx;
}

/** Create a WindowContext implemented with GPU (OpenGL)
* @param [in] nsView The associated NSView*, can be nullptr
* @param [in] params Parameters related to display
*/
static std::unique_ptr<skwindow::WindowContext> MakeGLForMac(void* nsView, std::unique_ptr<const skwindow::DisplayParams> params)
{
    std::unique_ptr<skwindow::WindowContext> ctx(new SkGLWindowContext_MacOS(nsView, std::move(params)));
    if (!ctx->isValid()) {
        return nullptr;
    }
    return ctx;
}

Render_Skia_MacOS::Render_Skia_MacOS(void* nsView, RenderBackendType backendType):
    m_nsView(nsView),
    m_backendType(backendType)
{
    //GPU drawing must be bound to a view
    if ((backendType == RenderBackendType::kNativeGL_BackendType) ||
        (backendType == RenderBackendType::kMetal_BackendType)) {
        ASSERT(m_nsView != nullptr);
        if (m_nsView == nullptr) {
            backendType = RenderBackendType::kRaster_BackendType;
        }
    }
    //Create the WindowContext
    if (backendType == RenderBackendType::kMetal_BackendType) {
        //GPU drawing (Metal)
        m_pWindowContext = MakeMetalForMac(m_nsView, std::make_unique<skwindow::DisplayParams>());
        ASSERT(m_pWindowContext != nullptr);
        if (m_pWindowContext != nullptr) {
            m_backendType = RenderBackendType::kMetal_BackendType;
        }
    }
    else if (backendType == RenderBackendType::kNativeGL_BackendType) {
        //GPU drawing (OpenGL)
        m_pWindowContext = MakeGLForMac(m_nsView, std::make_unique<skwindow::DisplayParams>());
        ASSERT(m_pWindowContext != nullptr);
        if (m_pWindowContext != nullptr) {
            m_backendType = RenderBackendType::kNativeGL_BackendType;
        }
    }
    //If GL fails, create a CPU drawing context
    if (m_pWindowContext == nullptr) {
        //CPU drawing
        m_pWindowContext = MakeRasterForMac(m_nsView, std::make_unique<skwindow::DisplayParams>());
        ASSERT(m_pWindowContext != nullptr);
        if (m_pWindowContext != nullptr) {
            m_backendType = RenderBackendType::kRaster_BackendType;
        }
    }
}

Render_Skia_MacOS::~Render_Skia_MacOS()
{
}

RenderBackendType Render_Skia_MacOS::GetRenderBackendType() const
{
    return m_backendType;
}

bool Render_Skia_MacOS::Resize(int32_t width, int32_t height)
{
    ASSERT((width > 0) && (height > 0));
    if ((width <= 0) || (height <= 0)) {
        return false;
    }
    if ((GetWidth() == width) && (GetHeight() == height)) {
        return true;
    }

    ASSERT(m_pWindowContext != nullptr);
    if (m_pWindowContext == nullptr) {
        return false;
    }
    m_pWindowContext->resize(width, height);
    ASSERT(GetSkCanvas() != nullptr);
    return true;
}

int32_t Render_Skia_MacOS::GetWidth() const
{
    if (m_pWindowContext != nullptr) {
        return m_pWindowContext->width();
    }
    return 0;
}

int32_t Render_Skia_MacOS::GetHeight() const
{
    if (m_pWindowContext != nullptr) {
        return m_pWindowContext->height();
    }
    return 0;
}

std::unique_ptr<ui::IRender> Render_Skia_MacOS::Clone()
{
    std::unique_ptr<ui::IRender> pClone = std::make_unique<ui::Render_Skia_MacOS>(m_nsView, m_backendType);
    pClone->Resize(GetWidth(), GetHeight());
    pClone->SetRenderDpi(GetRenderDpi());
    pClone->BitBlt(0, 0, GetWidth(), GetHeight(), this, 0, 0, RopMode::kSrcCopy);
    return pClone;
}

bool Render_Skia_MacOS::PaintAndSwapBuffers(IRenderPaint* pRenderPaint)
{
    ASSERT(pRenderPaint != nullptr);
    ASSERT(m_pWindowContext != nullptr);
    if ((m_pWindowContext != nullptr) && (pRenderPaint != nullptr)) {
        if (m_backendType == RenderBackendType::kMetal_BackendType) {
            SkMetalWindowContext_MacOS* pWindowContext = dynamic_cast<SkMetalWindowContext_MacOS*>(m_pWindowContext.get());
            ASSERT(pWindowContext != nullptr);
            if (pWindowContext != nullptr) {
                return pWindowContext->PaintAndSwapBuffers(this, pRenderPaint);
            }
        }
        else if (m_backendType == RenderBackendType::kNativeGL_BackendType) {
            SkGLWindowContext_MacOS* pWindowContext = dynamic_cast<SkGLWindowContext_MacOS*>(m_pWindowContext.get());
            ASSERT(pWindowContext != nullptr);
            if (pWindowContext != nullptr) {
                return pWindowContext->PaintAndSwapBuffers(this, pRenderPaint);
            }
        }
        else if (m_backendType == RenderBackendType::kRaster_BackendType) {
            SkRasterWindowContext_MacOS* pWindowContext = dynamic_cast<SkRasterWindowContext_MacOS*>(m_pWindowContext.get());
            ASSERT(pWindowContext != nullptr);
            if (pWindowContext != nullptr) {
                return pWindowContext->PaintAndSwapBuffers(this, pRenderPaint);
            }
        }
        else {
            ASSERT(false);
        }
    }
    return false;
}

SkSurface* Render_Skia_MacOS::GetSkSurface() const
{
    ASSERT(m_pWindowContext != nullptr);
    if (m_pWindowContext == nullptr) {
        return nullptr;
    }
    //Since m_pWindowContext internally holds a member variable, returning the raw SkSurface pointer is safe
    sk_sp<SkSurface> backbuffer = m_pWindowContext->getBackbufferSurface();
    ASSERT(backbuffer != nullptr);
    return backbuffer.get();
}

SkCanvas* Render_Skia_MacOS::GetSkCanvas() const
{
    ASSERT(m_pWindowContext != nullptr);
    if (m_pWindowContext == nullptr) {
        return nullptr;
    }
    sk_sp<SkSurface> backbuffer = m_pWindowContext->getBackbufferSurface();
    ASSERT(backbuffer != nullptr);
    if (backbuffer == nullptr) {
        return nullptr;
    }
    ASSERT(backbuffer->getCanvas() != nullptr);
    return backbuffer->getCanvas();
}

bool Render_Skia_MacOS::SetWindowRoundRectRgn(const UiRect& /*rcWnd*/, float /*rx*/, float /*ry*/, bool /*bRedraw*/)
{
    //macOS windows are naturally shaped; no window region support needed
    return true;
}

bool Render_Skia_MacOS::SetWindowRectRgn(const UiRect& /*rcWnd*/, bool /*bRedraw*/)
{
    return true;
}

void Render_Skia_MacOS::ClearWindowRgn(bool /*bRedraw*/)
{
}

} // namespace ui

#endif //DUI_BUILD_FOR_MACOS
