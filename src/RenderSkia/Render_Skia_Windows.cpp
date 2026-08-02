#include "duilib/RenderSkia/Render_Skia_Windows.h"

#ifdef DUILIB_BUILD_FOR_WIN

#include "duilib/RenderSkia/SkRasterWindowContext_Windows.h"
#include "duilib/RenderSkia/SkGLWindowContext_Windows.h"
#include "duilib/RenderSkia/WindowRgn_Windows.h"

#include "duilib/RenderSkia/SkiaHeaderBegin.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "include/core/SkRegion.h"

#include "duilib/RenderSkia/SkiaHeaderEnd.h"

namespace ui {

/** Create a WindowContext implemented with Raster
* @param [in] hWnd The associated window handle, can be nullptr
* @param [in] params Parameters related to display
*/
std::unique_ptr<skwindow::WindowContext> MakeRasterForWin(HWND hWnd, std::unique_ptr<const skwindow::DisplayParams> params)
{
    std::unique_ptr<skwindow::WindowContext> ctx(new SkRasterWindowContext_Windows(hWnd, std::move(params)));
    return ctx;
}

/** Create a WindowContext implemented with GPU
* @param [in] hWnd The associated window handle, can be nullptr
* @param [in] params Parameters related to display
*/
std::unique_ptr<skwindow::WindowContext> MakeGLForWin(HWND hWnd, std::unique_ptr<const skwindow::DisplayParams> params)
{
    std::unique_ptr<skwindow::WindowContext> ctx(new SkGLWindowContext_Windows(hWnd, std::move(params)));
    if (!ctx->isValid()) {
        return nullptr;
    }
    return ctx;
}

Render_Skia_Windows::Render_Skia_Windows(HWND hWnd, RenderBackendType backendType):
    m_hWnd(hWnd),
    m_backendType(backendType),
    m_hDC(nullptr),
    m_hOldObj(nullptr)
{
    if (backendType == RenderBackendType::kNativeGL_BackendType) {
        //GPU drawing must be bound to a window
        ASSERT(::IsWindow(hWnd));
        if (!::IsWindow(hWnd)) {
            backendType = RenderBackendType::kRaster_BackendType;
        }
    }
    //Create the WindowContext
    if (backendType == RenderBackendType::kNativeGL_BackendType) {
        //GPU drawing
        m_pWindowContext = MakeGLForWin(hWnd, std::make_unique<skwindow::DisplayParams>());
        ASSERT(m_pWindowContext != nullptr);
        if (m_pWindowContext != nullptr) {
            m_backendType = RenderBackendType::kNativeGL_BackendType;
        }        
    }
    //If GL fails, create a CPU drawing context
    if (m_pWindowContext == nullptr) {
        //CPU drawing
        m_pWindowContext = MakeRasterForWin(hWnd, std::make_unique <skwindow::DisplayParams>());
        ASSERT(m_pWindowContext != nullptr);
        if (m_pWindowContext != nullptr) {
            m_backendType = RenderBackendType::kRaster_BackendType;
        }        
    }
}

Render_Skia_Windows::~Render_Skia_Windows()
{
    DeleteDC();
}

RenderBackendType Render_Skia_Windows::GetRenderBackendType() const
{
    return m_backendType;
}

bool Render_Skia_Windows::Resize(int32_t width, int32_t height)
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

    DeleteDC();
    return true;
}

int32_t Render_Skia_Windows::GetWidth() const
{
    if (m_pWindowContext != nullptr) {
        return m_pWindowContext->width();
    }
    return 0;
}

int32_t Render_Skia_Windows::GetHeight() const
{
    if (m_pWindowContext != nullptr) {
        return m_pWindowContext->height();
    }
    return 0;
}

std::unique_ptr<ui::IRender> Render_Skia_Windows::Clone()
{
    std::unique_ptr<ui::IRender> pClone = std::make_unique<ui::Render_Skia_Windows>(m_hWnd, m_backendType);
    pClone->Resize(GetWidth(), GetHeight());
    pClone->SetRenderDpi(GetRenderDpi());
    pClone->BitBlt(0, 0, GetWidth(), GetHeight(), this, 0, 0, RopMode::kSrcCopy);
    return pClone;
}

bool Render_Skia_Windows::PaintAndSwapBuffers(IRenderPaint* pRenderPaint)
{
    ASSERT(pRenderPaint != nullptr);
    ASSERT(m_pWindowContext != nullptr);
    if ((m_pWindowContext != nullptr) && (pRenderPaint != nullptr)) {
        if (m_backendType == RenderBackendType::kNativeGL_BackendType) {
            SkGLWindowContext_Windows* pWindowContext = dynamic_cast<SkGLWindowContext_Windows*>(m_pWindowContext.get());
            ASSERT(pWindowContext != nullptr);
            if (pWindowContext != nullptr) {
                return pWindowContext->PaintAndSwapBuffers(this, pRenderPaint);
            }
        }
        else if (m_backendType == RenderBackendType::kRaster_BackendType) {
            SkRasterWindowContext_Windows* pWindowContext = dynamic_cast<SkRasterWindowContext_Windows*>(m_pWindowContext.get());
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

SkSurface* Render_Skia_Windows::GetSkSurface() const
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

SkCanvas* Render_Skia_Windows::GetSkCanvas() const
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

HDC Render_Skia_Windows::GetRenderDC(HWND hWnd)
{
    if (m_hDC != nullptr) {
        return m_hDC;
    }
    SkCanvas* skCanvas = GetSkCanvas();
    ASSERT(skCanvas != nullptr);
    if (skCanvas == nullptr) {
        return nullptr;
    }
    ASSERT(m_pWindowContext != nullptr);
    if (m_pWindowContext == nullptr) {
        return nullptr;
    }
    SkRasterWindowContext_Windows* pWindowContext = dynamic_cast<SkRasterWindowContext_Windows*>(m_pWindowContext.get());
    if (pWindowContext == nullptr) {
        //If it is not CPU rendering, no DC is provided. The GetRenderDC function is currently only used by RichEdit; nullptr is returned here so RichEdit uses another drawing method
        ASSERT(GetRenderBackendType() != RenderBackendType::kRaster_BackendType);
        return nullptr;
    }

    HBITMAP hBitmap = pWindowContext->GetHBitmap();
    ASSERT(hBitmap != nullptr);
    if (hBitmap == nullptr) {
        return nullptr;
    }
    HDC hDeskDC = ::GetDC(hWnd);
    HDC hGetDC = ::CreateCompatibleDC(hDeskDC);
    ::ReleaseDC(hWnd, hDeskDC);
    hDeskDC = nullptr;

    m_hOldObj = ::SelectObject(hGetDC, hBitmap);

    if (skCanvas->isClipEmpty()) {
        ::IntersectClipRect(hGetDC, 0, 0, 0, 0);
    }
    else if (skCanvas->isClipRect()) {
        SkRect rcClip;
        if (skCanvas->getLocalClipBounds(&rcClip)) {
            RECT rc = { (int)rcClip.left(),(int)rcClip.top(),(int)rcClip.right(),(int)rcClip.bottom() };
            ::InflateRect(&rc, -1, -1); //Note: it needs to be shrunk inward by one pixel
            ::IntersectClipRect(hGetDC, rc.left, rc.top, rc.right, rc.bottom);
        }
    }
    else
    {
        SkRegion rgn;
        skCanvas->temporary_internal_getRgnClip(&rgn);
        SkRegion::Iterator it(rgn);
        int nCount = 0;
        for (; !it.done(); it.next()) {
            ++nCount;
        }
        it.rewind();

        int nSize = sizeof(RGNDATAHEADER) + nCount * sizeof(RECT);
        RGNDATA* rgnData = (RGNDATA*)::malloc(nSize);
        ASSERT(rgnData != nullptr);
        if (rgnData != nullptr) {
            memset(rgnData, 0, nSize);
            rgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
            rgnData->rdh.iType = RDH_RECTANGLES;
            rgnData->rdh.nCount = nCount;
            rgnData->rdh.rcBound.right = GetWidth();
            rgnData->rdh.rcBound.bottom = GetHeight();

            nCount = 0;
            LPRECT pRc = (LPRECT)rgnData->Buffer;
            for (; !it.done(); it.next()) {
                SkIRect skrc = it.rect();
                RECT rc = { skrc.fLeft,skrc.fTop,skrc.fRight,skrc.fBottom };
                pRc[nCount++] = rc;
            }

            HRGN hRgn = ::ExtCreateRegion(nullptr, nSize, rgnData);
            ::free(rgnData);
            ::SelectClipRgn(hGetDC, hRgn);
            ::DeleteObject(hRgn);
        }
    }

    ::SetGraphicsMode(hGetDC, GM_ADVANCED);
    const SkPoint& ptOrg = GetPointOrg();
    ::SetViewportOrgEx(hGetDC, (int)ptOrg.x(), (int)ptOrg.y(), nullptr);

    struct IxForm
    {
        enum Index {
            kMScaleX = 0,
            kMSkewX,
            kMTransX,
            kMSkewY,
            kMScaleY,
            kMTransY,
            kMPersp0,
            kMPersp1,
            kMPersp2
        };
    };
    SkMatrix mtx = skCanvas->getTotalMatrix();
    XFORM xForm = { mtx.get(IxForm::kMScaleX),mtx.get(IxForm::kMSkewY),
                    mtx.get(IxForm::kMSkewX),mtx.get(IxForm::kMScaleY),
                    mtx.get(IxForm::kMTransX),mtx.get(IxForm::kMTransY) };
    ::SetWorldTransform(hGetDC, &xForm);
    m_hDC = hGetDC;
    return hGetDC;
}

void Render_Skia_Windows::ReleaseRenderDC(HDC hdc)
{
    if (hdc == m_hDC) {
        DeleteDC();
    }
}

void Render_Skia_Windows::DeleteDC()
{
    if (m_hDC != nullptr) {
        if (m_hOldObj != nullptr) {
            ::SelectObject(m_hDC, m_hOldObj);
            m_hOldObj = nullptr;
        }
        ::DeleteDC(m_hDC);
        m_hDC = nullptr;
    }
}

bool Render_Skia_Windows::SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw)
{
    return WindowRgn::SetWindowRoundRectRgn(m_hWnd, rcWnd, rx, ry, bRedraw);
}

bool Render_Skia_Windows::SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw)
{
    return WindowRgn::SetWindowRectRgn(m_hWnd, rcWnd, bRedraw);
}

void Render_Skia_Windows::ClearWindowRgn(bool bRedraw)
{
    return WindowRgn::ClearWindowRgn(m_hWnd, bRedraw);
}

} // namespace ui

#endif //DUILIB_BUILD_FOR_WIN
