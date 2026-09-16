#include "dui/RenderSkia/Render_Skia_SDL.h"
#include "dui/RenderSkia/SkRasterWindowContext_SDL.h"

#ifdef DUI_BUILD_FOR_SDL

#include "dui/RenderSkia/SkiaHeaderBegin.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "include/core/SkRegion.h"

#include "dui/RenderSkia/SkiaHeaderEnd.h"

#ifdef DUI_BUILD_FOR_WIN
    #include "dui/RenderSkia/WindowRgn_Windows.h"
    #include "SDL3/SDL.h"
#endif

namespace ui {

/** Create a WindowContext implemented with Raster
* @param [in] hWnd The associated window handle, can be nullptr
* @param [in] params Parameters related to display
*/
std::unique_ptr<skwindow::WindowContext> MakeRasterForSDL(SDL_Window* sdlWindow, std::unique_ptr<const skwindow::DisplayParams> params)
{
    std::unique_ptr<skwindow::WindowContext> ctx(new SkRasterWindowContext_SDL(sdlWindow, std::move(params)));
    return ctx;
}

/** Create a WindowContext implemented with GPU
* @param [in] hWnd The associated window handle, can be nullptr
* @param [in] params Parameters related to display
*/
//std::unique_ptr<skwindow::WindowContext> MakeGLForWin(HWND hWnd, const skwindow::DisplayParams& params)
//{
//    std::unique_ptr<skwindow::WindowContext> ctx(new SkGLWindowContext_Windows(hWnd, params));
//    if (!ctx->isValid()) {
//        return nullptr;
//    }
//    return ctx;
//}

Render_Skia_SDL::Render_Skia_SDL(SDL_Window* sdlWindow, RenderBackendType backendType):
    m_sdlWindow(sdlWindow),
    m_backendType(backendType)
{
    if (backendType == RenderBackendType::kNativeGL_BackendType) {
        //GPU drawing must be bound to a window
        ASSERT(m_sdlWindow != nullptr);
        if (m_sdlWindow == nullptr) {
            backendType = RenderBackendType::kRaster_BackendType;
        }
    }
    //Create the WindowContext
    //if (backendType == RenderBackendType::kNativeGL_BackendType) {
    //    //GPU drawing
    //    m_pWindowContext = MakeGLForWin(hWnd, skwindow::DisplayParams());
    //    ASSERT(m_pWindowContext != nullptr);
    //    if (m_pWindowContext != nullptr) {
    //        m_backendType = RenderBackendType::kNativeGL_BackendType;
    //    }        
    //}
    //If GL fails, create a CPU drawing context
    if (m_pWindowContext == nullptr) {
        //CPU drawing
        m_pWindowContext = MakeRasterForSDL(m_sdlWindow, std::make_unique<skwindow::DisplayParams>());
        ASSERT(m_pWindowContext != nullptr);
        if (m_pWindowContext != nullptr) {
            m_backendType = RenderBackendType::kRaster_BackendType;
        }        
    }
}

Render_Skia_SDL::~Render_Skia_SDL()
{
}

RenderBackendType Render_Skia_SDL::GetRenderBackendType() const
{
    return m_backendType;
}

bool Render_Skia_SDL::Resize(int32_t width, int32_t height)
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

int32_t Render_Skia_SDL::GetWidth() const
{
    if (m_pWindowContext != nullptr) {
        return m_pWindowContext->width();
    }
    return 0;
}

int32_t Render_Skia_SDL::GetHeight() const
{
    if (m_pWindowContext != nullptr) {
        return m_pWindowContext->height();
    }
    return 0;
}

std::unique_ptr<ui::IRender> Render_Skia_SDL::Clone()
{
    std::unique_ptr<ui::IRender> pClone = std::make_unique<ui::Render_Skia_SDL>(m_sdlWindow, m_backendType);
    pClone->Resize(GetWidth(), GetHeight());
    pClone->SetRenderDpi(GetRenderDpi());
    pClone->BitBlt(0, 0, GetWidth(), GetHeight(), this, 0, 0, RopMode::kSrcCopy);
    return pClone;
}

bool Render_Skia_SDL::PaintAndSwapBuffers(IRenderPaint* pRenderPaint)
{
    ASSERT(pRenderPaint != nullptr);
    ASSERT(m_pWindowContext != nullptr);
    if ((m_pWindowContext != nullptr) && (pRenderPaint != nullptr)) {
        if (m_backendType == RenderBackendType::kNativeGL_BackendType) {
            /*SkGLWindowContext_Windows* pWindowContext = dynamic_cast<SkGLWindowContext_Windows*>(m_pWindowContext.get());
            ASSERT(pWindowContext != nullptr);
            if (pWindowContext != nullptr) {
                return pWindowContext->PaintAndSwapBuffers(this, pRenderPaint);
            }*/
        }
        else if (m_backendType == RenderBackendType::kRaster_BackendType) {
            SkRasterWindowContext_SDL* pWindowContext = dynamic_cast<SkRasterWindowContext_SDL*>(m_pWindowContext.get());
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

SkSurface* Render_Skia_SDL::GetSkSurface() const
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

SkCanvas* Render_Skia_SDL::GetSkCanvas() const
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

bool Render_Skia_SDL::SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw)
{
#ifdef DUI_BUILD_FOR_WIN
    if (m_sdlWindow != nullptr) {
        SDL_PropertiesID propID = SDL_GetWindowProperties(m_sdlWindow);
        HWND hWnd = (HWND)SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        if (::IsWindow(hWnd)) {
            return WindowRgn::SetWindowRoundRectRgn(hWnd, rcWnd, rx, ry, bRedraw);
        }
    }
#else
    //Not needed; with SDL, the window can be set to support transparency, avoiding the need for RGN settings
    (void)rcWnd;
    (void)rx;
    (void)ry;
    (void)bRedraw;
#endif
    return false;
}

bool Render_Skia_SDL::SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw)
{
#ifdef DUI_BUILD_FOR_WIN
    if (m_sdlWindow != nullptr) {
        SDL_PropertiesID propID = SDL_GetWindowProperties(m_sdlWindow);
        HWND hWnd = (HWND)SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        if (::IsWindow(hWnd)) {
            return WindowRgn::SetWindowRectRgn(hWnd, rcWnd, bRedraw);
        }
    }
#else
    //Not needed; with SDL, the window can be set to support transparency, avoiding the need for RGN settings
    (void)rcWnd;
    (void)bRedraw;
#endif
    return false;
}

void Render_Skia_SDL::ClearWindowRgn(bool bRedraw)
{
#ifdef DUI_BUILD_FOR_WIN
    if (m_sdlWindow != nullptr) {
        SDL_PropertiesID propID = SDL_GetWindowProperties(m_sdlWindow);
        HWND hWnd = (HWND)SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
        if (::IsWindow(hWnd)) {
            return WindowRgn::ClearWindowRgn(hWnd, bRedraw);
        }
    }
#else
    //Not needed; with SDL, the window can be set to support transparency, avoiding the need for RGN settings
    (void)bRedraw;
#endif
}

#ifdef DUI_BUILD_FOR_WIN

HDC Render_Skia_SDL::GetRenderDC(HWND /*hWnd*/)
{
    ASSERT(0);
    return nullptr;
}

void Render_Skia_SDL::ReleaseRenderDC(HDC /*hdc*/)
{
    ASSERT(0);
}

#endif

} // namespace ui

#endif //DUI_BUILD_FOR_SDL
