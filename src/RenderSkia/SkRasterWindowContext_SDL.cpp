#include "dui/RenderSkia/SkRasterWindowContext_SDL.h"
#include "dui/Render/IRender.h"
#include "dui/Utils/PerformanceUtil.h"

#ifdef DUI_BUILD_FOR_SDL

#include <SDL3/SDL.h>

namespace ui {

SkRasterWindowContext_SDL::SkRasterWindowContext_SDL(SDL_Window* sdlWindow, std::unique_ptr<const skwindow::DisplayParams> params):
    skwindow::internal::RasterWindowContext(std::move(params)),
    m_sdlWindow(sdlWindow),
    m_sdlTextrue(nullptr)
{
    fWidth = 0;
    fHeight = 0;
}

SkRasterWindowContext_SDL::~SkRasterWindowContext_SDL()
{
    Clear();
}

void SkRasterWindowContext_SDL::Clear()
{
    if (m_sdlTextrue != nullptr) {
        SDL_DestroyTexture(m_sdlTextrue);
        m_sdlTextrue = nullptr;
    }
    m_fBackbufferSurface.reset();
    m_fSurfaceMemory.reset();
}

void SkRasterWindowContext_SDL::setDisplayParams(std::unique_ptr<const skwindow::DisplayParams> params)
{
    int32_t nWidth = width();
    int32_t nHeight = height();
    fDisplayParams = std::move(params);
    Clear();
    if ((nWidth > 0) && (nHeight > 0)) {
        this->resize(nWidth, nHeight);
    }   
}

void SkRasterWindowContext_SDL::resize(int nWidth, int nHeight)
{
    if (nWidth < 0) {
        nWidth = 0;
    }
    if (nHeight < 0) {
        nHeight = 0;
    }
    if ((fWidth == nWidth) && (fHeight == nHeight)) {
        if ((fWidth > 0) && (fHeight > 0)) {
            SkASSERT(m_fSurfaceMemory != nullptr);
            SkASSERT(m_fBackbufferSurface != nullptr);
        }
        return;
    }
    const skwindow::DisplayParams* pDisplayParams = getDisplayParams();
    SkASSERT(pDisplayParams != nullptr);
    if (pDisplayParams == nullptr) {
        return;
    }

    fWidth = nWidth;
    fHeight = nHeight;

    Clear();
    if ((nWidth == 0) || (nHeight == 0)) {
        return;
    }

    m_fSurfaceMemory.reset(nWidth * nHeight * sizeof(uint32_t));
    void* pixels = m_fSurfaceMemory.get();
    SkASSERT(pixels != nullptr);
    if (pixels == nullptr) {
        m_fSurfaceMemory.reset();
        fWidth = 0;
        fHeight = 0;
        return;
    }

    SkImageInfo info = SkImageInfo::Make(nWidth, nHeight, pDisplayParams->colorType(), SkAlphaType::kPremul_SkAlphaType, pDisplayParams->colorSpace());
    m_fBackbufferSurface = SkSurfaces::WrapPixels(info, pixels, sizeof(uint32_t) * nWidth);
    if (m_fBackbufferSurface == nullptr) {
        m_fSurfaceMemory.reset();
        fWidth = 0;
        fHeight = 0;
        return;
    }
}

sk_sp<SkSurface> SkRasterWindowContext_SDL::getBackbufferSurface()
{
    return m_fBackbufferSurface;
}

void SkRasterWindowContext_SDL::onSwapBuffers()
{
}

bool SkRasterWindowContext_SDL::PaintAndSwapBuffers(IRender* pRender, IRenderPaint* pRenderPaint)
{
    SkASSERT(m_sdlWindow != nullptr);
    if (m_sdlWindow == nullptr) {
        return false;
    }
    SkASSERT(m_fBackbufferSurface != nullptr);
    if (m_fBackbufferSurface == nullptr) {
        return false;
    }
    SkASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return false;
    }
    SkASSERT(pRenderPaint != nullptr);
    if (pRenderPaint == nullptr) {
        return false;
    }

    //Get the area that needs to be drawn
    UiRect rcPaint;
    bool bUpdateRect = pRenderPaint->GetUpdateRect(rcPaint); //Returns true if partial drawing is supported; only the updated area is drawn to improve efficiency
    if (bUpdateRect && !rcPaint.IsEmpty()) {
        //Ensure the validity of the area
        UiRect rcClient;
        GetClientRect(rcClient);
        rcPaint.Intersect(rcClient);
    }
    if (rcPaint.IsEmpty()) {
        //Partial drawing is not supported; the entire client area of the window must be redrawn every time
        GetClientRect(rcPaint);
    }
    if (rcPaint.IsEmpty()) {
        //No drawing needed
        return false;
    }

    //The window alpha
    uint8_t nLayeredWindowAlpha = pRenderPaint->GetLayeredWindowAlpha();

    //Whether it is a full draw
    const bool bFullPaint = (rcPaint.Width() == width()) && (rcPaint.Height() == height());
    SkCanvas* skCanvas = nullptr;
    if (!bFullPaint) {
        //Use the clip region to avoid drawing data in other unrelated areas
        skCanvas = m_fBackbufferSurface->getCanvas();
        if (skCanvas != nullptr) {
            skCanvas->save();
            skCanvas->clipIRect(SkIRect::MakeLTRB(rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom));
        }
    }

    //Perform drawing
    bool bRet = pRenderPaint->DoPaint(rcPaint);
    if (bRet) {
        //After drawing, update to the window
        SwapPaintBuffers(rcPaint, nLayeredWindowAlpha);
    }

    //After drawing, mark the drawn area as valid
    if (bUpdateRect) {
        ValidateRect(rcPaint);
    }

    if (skCanvas != nullptr) {
        skCanvas->restore();
    }
    return bRet;
}

bool SkRasterWindowContext_SDL::SwapPaintBuffers(const UiRect& rcPaint, uint8_t nLayeredWindowAlpha)
{
    PerformanceStat statPerformance(_T("PaintWindow, SkRasterWindowContext_SDL::SwapPaintBuffers"));
    ASSERT(!rcPaint.IsEmpty());
    if (rcPaint.IsEmpty()) {
        return false;
    }
    ASSERT(m_sdlWindow != nullptr);
    if (m_sdlWindow == nullptr) {
        return false;
    }
    ASSERT(m_fSurfaceMemory.get() != nullptr);
    if (m_fSurfaceMemory.get() == nullptr) {
        return false;
    }
    ASSERT(m_fBackbufferSurface.get() != nullptr);
    if (m_fBackbufferSurface.get() == nullptr) {
        return false;
    }

    if (SwapPaintBuffersFast(rcPaint, nLayeredWindowAlpha)) {
        //Update the drawn data to the window device directly through the window's Surface (no GPU, faster)
        return true;
    }

    SDL_Renderer* sdlRenderer = SDL_GetRenderer(m_sdlWindow);
    ASSERT(sdlRenderer != nullptr);
    if (sdlRenderer == nullptr) {
        return false;
    }

    if (m_sdlTextrue == nullptr) {
        // Render to the window (IRender -> draw to the SDL Render -> update to the SDL window)
#ifdef DUI_BUILD_FOR_WIN
        SDL_PixelFormat format = SDL_PIXELFORMAT_BGRA32;
#else
        SDL_PixelFormat format = SDL_PIXELFORMAT_RGBA32;
#endif
        m_sdlTextrue = SDL_CreateTexture(sdlRenderer, format, SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING, width(), height());
        ASSERT(m_sdlTextrue != nullptr);
    }
    
    if (m_sdlTextrue == nullptr) {
        return false;
    }

    //Copy the UI data to the texture
    bool bDrawOk = false;
    if ((rcPaint.Width() != width()) || (rcPaint.Height() != height())) {
        //Partial drawing: draw only the updated part
        SDL_Rect rect;
        rect.x = rcPaint.left;
        rect.y = rcPaint.top;
        rect.w = rcPaint.Width();
        rect.h = rcPaint.Height();        
        SkIRect bounds = SkIRect::MakeLTRB(rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom);
        sk_sp<SkImage> snapshotImage = m_fBackbufferSurface->makeImageSnapshot(bounds);
        if (snapshotImage != nullptr) {
            SkPixmap pixmap;
            if (snapshotImage->peekPixels(&pixmap) && (pixmap.addr() != nullptr) && (pixmap.width() == rcPaint.Width()) && (pixmap.height() == rcPaint.Height())) {
                SDL_UpdateTexture(m_sdlTextrue, &rect, pixmap.addr(), (int)pixmap.rowBytes());
                bDrawOk = true;
            }
        }
        ASSERT(bDrawOk);
    }
    if (!bDrawOk) {
        //Full drawing
        SDL_UpdateTexture(m_sdlTextrue, nullptr, m_fSurfaceMemory.get(), m_fBackbufferSurface->width() * sizeof(uint32_t));
    }

    //Set the alpha of the texture
    if (nLayeredWindowAlpha != 255) {
        SDL_SetTextureAlphaMod(m_sdlTextrue, nLayeredWindowAlpha);
    }
    //Clear the source SDL window to avoid drawing onto residual images with a transparent window, which would make the window shadow increasingly dark
    SDL_RenderClear(sdlRenderer);

    //Draw the texture
    SDL_RenderTexture(sdlRenderer, m_sdlTextrue, nullptr, nullptr);

    //Submit the drawn data (this step is the slowest: fast on Windows where the time is negligible; but on Linux with X11 (in a VM), each call takes about 9 ms)
    SDL_RenderPresent(sdlRenderer);
    return true;
}

bool SkRasterWindowContext_SDL::SwapPaintBuffersFast(const UiRect& rcPaint, uint8_t nLayeredWindowAlpha)
{
    ASSERT(!rcPaint.IsEmpty());
    if (rcPaint.IsEmpty()) {
        return false;
    }
    ASSERT(m_sdlWindow != nullptr);
    if (m_sdlWindow == nullptr) {
        return false;
    }
    ASSERT(m_fSurfaceMemory.get() != nullptr);
    if (m_fSurfaceMemory.get() == nullptr) {
        return false;
    }
    ASSERT(m_fBackbufferSurface.get() != nullptr);
    if (m_fBackbufferSurface.get() == nullptr) {
        return false;
    }

    SDL_Surface* sdlSurface = SDL_GetWindowSurface(m_sdlWindow);
    if (sdlSurface == nullptr) {
        return false;
    }

    if ((sdlSurface->pixels == nullptr) || (sdlSurface->w != width()) || (sdlSurface->h != height()) || (sdlSurface->pitch != width() * (int32_t)sizeof(uint32_t))) {
        //Size mismatch
        return false;
    }
    
    SkColorType backSurfaceColorType = m_fBackbufferSurface->imageInfo().colorInfo().colorType();
    int32_t backR = -1;
    int32_t backG = -1;
    int32_t backB = -1;
    int32_t backA = -1;
    if (!GetSkiaColorByteOrder(backSurfaceColorType, backR, backG, backB, backA)) {
        return false;
    }

    SDL_PixelFormat sdlFormat = sdlSurface->format;
    int32_t sdlR = -1;
    int32_t sdlG = -1;
    int32_t sdlB = -1;
    int32_t sdlA = -1;
    if (!GetSDLColorByteOrder(sdlFormat, sdlR, sdlG, sdlB, sdlA)) {
        return false;
    }

    //Measure performance
    PerformanceStat statPerformance(_T("PaintWindow, SkRasterWindowContext_SDL::SwapPaintBuffersFast"));

    bool bDrawOk = false;
    if ((rcPaint.Width() != width()) || (rcPaint.Height() != height())) {
        //Partial drawing: draw only the updated part
        SDL_Rect rect;
        rect.x = rcPaint.left;
        rect.y = rcPaint.top;
        rect.w = rcPaint.Width();
        rect.h = rcPaint.Height();
        //Copy data row by row (one row per copy)
        const int32_t nMaxRow = rcPaint.top + rcPaint.Height();
        const int32_t nWidth = rcPaint.Width();
        for (int32_t nRow = rcPaint.top; nRow < nMaxRow; ++nRow) {
            ::memcpy((uint32_t*)sdlSurface->pixels + nRow * sdlSurface->w + rcPaint.left,
                     (uint32_t*)m_fSurfaceMemory.get() + nRow * sdlSurface->w + rcPaint.left,
                     nWidth * sizeof(uint32_t));
        }

        //Handle the color order
        UpdateColorByteOrder(sdlSurface->pixels, sdlSurface->w, rcPaint, backR, backG, backB, backA, sdlR, sdlG, sdlB, sdlA);
        UpdateColorAlpha(sdlSurface->pixels, sdlSurface->w, rcPaint, nLayeredWindowAlpha, sdlR, sdlG, sdlB, sdlA);
        SDL_UpdateWindowSurfaceRects(m_sdlWindow, &rect, 1);
        bDrawOk = true;
        ASSERT(bDrawOk);
    }
    if (!bDrawOk) {
        //Full drawing
        ::memcpy(sdlSurface->pixels, m_fSurfaceMemory.get(), sdlSurface->h * sdlSurface->pitch);
        UpdateColorByteOrder(sdlSurface->pixels, sdlSurface->w, rcPaint, backR, backG, backB, backA, sdlR, sdlG, sdlB, sdlA);
        UpdateColorAlpha(sdlSurface->pixels, sdlSurface->w, rcPaint, nLayeredWindowAlpha, sdlR, sdlG, sdlB, sdlA);
        SDL_UpdateWindowSurface(m_sdlWindow);
    }
    return true;
}

bool SkRasterWindowContext_SDL::GetSkiaColorByteOrder(SkColorType backSurfaceColorType, int32_t& backR, int32_t& backG, int32_t& backB, int32_t& backA) const
{
    if (backSurfaceColorType == kBGRA_8888_SkColorType) {
        backB = 0;
        backG = 1;
        backR = 2;
        backA = 3;
        return true;
    }
    else if (backSurfaceColorType == kRGBA_8888_SkColorType) {
        backR = 0;
        backG = 1;
        backB = 2;
        backA = 3;
        return true;
    }
    return false;
}

bool SkRasterWindowContext_SDL::GetSDLColorByteOrder(int32_t sdlFormat, int32_t& sdlR, int32_t& sdlG, int32_t& sdlB, int32_t& sdlA) const
{
    int bpp = 0;
    Uint32 sdlRmask = 0;
    Uint32 sdlGmask = 0;
    Uint32 sdlBmask = 0;
    Uint32 sdlAmask = 0;
    SDL_GetMasksForPixelFormat((SDL_PixelFormat)sdlFormat, &bpp, &sdlRmask, &sdlGmask, &sdlBmask, &sdlAmask);
    if (bpp != 32) {
        return false;
    }
    sdlR = GetColorByteOrder(sdlRmask);
    sdlG = GetColorByteOrder(sdlGmask);
    sdlB = GetColorByteOrder(sdlBmask);
    if (sdlAmask != 0) {
        sdlA = GetColorByteOrder(sdlAmask);
    }
    else {
        sdlA = 3;
        if ((sdlR == 3) || (sdlG == 3) || (sdlB == 3)) {
            return false;
        }
    }
    return (sdlR >= 0) && (sdlG >= 0) && (sdlB >= 0) && (sdlA >= 0);
}

int32_t SkRasterWindowContext_SDL::GetColorByteOrder(uint32_t mask) const
{
    //Endian Order: LE
    int32_t colorOrder = -1;
    if (mask == 0x000000FF) {
        colorOrder = 0;
    }
    else if (mask == 0x0000FF00) {
        colorOrder = 1;
    }
    else if (mask == 0x00FF0000) {
        colorOrder = 2;
    }
    else if (mask == 0xFF000000) {
        colorOrder = 3;
    }
    return colorOrder;
}

void SkRasterWindowContext_SDL::UpdateColorByteOrder(void* surfacePixels, int32_t nSurfaceWidth, const UiRect& rcPaint,
                                                     int32_t backR, int32_t backG, int32_t backB, int32_t backA,
                                                     int32_t sdlR, int32_t sdlG, int32_t sdlB, int32_t sdlA) const
{
    if ((surfacePixels == nullptr) || (nSurfaceWidth < 1) || rcPaint.IsEmpty()) {
        return;
    }
    bool bDiffR = sdlR != backR;
    bool bDiffG = sdlG != backG;
    bool bDiffB = sdlB != backB;
    bool bDiffA = sdlA != backA;
    if (!bDiffR && !bDiffG && !bDiffB && !bDiffA) {
        //The color format is the same; no need to update the color data
        return;
    }
    uint32_t colorValue = 0;
    const int32_t nMaxRow = rcPaint.top + rcPaint.Height();
    const int32_t nWidth = rcPaint.Width();
    for (int32_t nRow = rcPaint.top; nRow < nMaxRow; ++nRow) {
        for (int32_t nCol = 0; nCol < nWidth; ++nCol) {
            uint32_t* pColorValue = (uint32_t*)surfacePixels + nRow * nSurfaceWidth + rcPaint.left + nCol;
            colorValue = *pColorValue;
            if (bDiffR) {
                ((uint8_t*)pColorValue)[sdlR] = ((uint8_t*)&colorValue)[backR];
            }
            if (bDiffG) {
                ((uint8_t*)pColorValue)[sdlG] = ((uint8_t*)&colorValue)[backG];
            }
            if (bDiffB) {
                ((uint8_t*)pColorValue)[sdlB] = ((uint8_t*)&colorValue)[backB];
            }
            if (bDiffA) {
                ((uint8_t*)pColorValue)[sdlA] = ((uint8_t*)&colorValue)[backA];
            }
        }
    }
}

void SkRasterWindowContext_SDL::UpdateColorAlpha(void* surfacePixels, int32_t nSurfaceWidth, const UiRect& rcPaint, uint8_t nLayeredWindowAlpha,
                                                 int32_t sdlR, int32_t sdlG, int32_t sdlB, int32_t sdlA)
{
    if ((surfacePixels == nullptr) || (nSurfaceWidth < 1) || rcPaint.IsEmpty() || (nLayeredWindowAlpha == 255)) {
        return;
    }
    const int32_t nMaxRow = rcPaint.top + rcPaint.Height();
    const int32_t nWidth = rcPaint.Width();
    for (int32_t nRow = rcPaint.top; nRow < nMaxRow; ++nRow) {
        for (int32_t nCol = 0; nCol < nWidth; ++nCol) {
            uint32_t* pColorValue = (uint32_t*)surfacePixels + nRow * nSurfaceWidth + rcPaint.left + nCol;
            uint8_t& r = ((uint8_t*)pColorValue)[sdlR];
            uint8_t& g = ((uint8_t*)pColorValue)[sdlG];
            uint8_t& b = ((uint8_t*)pColorValue)[sdlB];
            uint8_t& a = ((uint8_t*)pColorValue)[sdlA];
            r = r * nLayeredWindowAlpha / 255;
            g = g * nLayeredWindowAlpha / 255;
            b = b * nLayeredWindowAlpha / 255;
            a = a * nLayeredWindowAlpha / 255;
        }
    }
}

void SkRasterWindowContext_SDL::GetClientRect(UiRect& rcClient) const
{
    ASSERT(m_sdlWindow != nullptr);
    ASSERT((width() > 0) && (height() > 0));
    rcClient.left = 0;
    rcClient.top = 0;
    rcClient.right = rcClient.left + width();
    rcClient.bottom = rcClient.top + height();
}

void SkRasterWindowContext_SDL::ValidateRect(UiRect& rcPaint) const
{
    if ((m_sdlWindow == nullptr) || rcPaint.IsEmpty()) {
        return;
    }
#ifdef DUI_BUILD_FOR_WIN
    SDL_PropertiesID propID = SDL_GetWindowProperties(m_sdlWindow);
    HWND hWnd = (HWND)SDL_GetPointerProperty(propID, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    if ((hWnd != nullptr) && ::IsWindow(hWnd)) {
        RECT rectPaint = { 0, };
        rectPaint.left = rcPaint.left;
        rectPaint.top = rcPaint.top;
        rectPaint.right = rcPaint.right;
        rectPaint.bottom = rcPaint.bottom;
        ::ValidateRect(hWnd, &rectPaint);
    }
#endif
}

} // namespace ui

#endif //DUI_BUILD_FOR_SDL
