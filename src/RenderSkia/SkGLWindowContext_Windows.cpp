#include "duilib/RenderSkia/SkGLWindowContext_Windows.h"
#include "duilib/Render/IRender.h"

#ifdef DUILIB_BUILD_FOR_WIN

#include "duilib/RenderSkia/SkiaHeaderBegin.h"

#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "tools/ganesh/gl/win/SkWGL.h"
#include "tools/window/GLWindowContext.h"

#include <GL/gl.h>

#include "duilib/RenderSkia/SkiaHeaderEnd.h"

namespace ui {

SkGLWindowContext_Windows::SkGLWindowContext_Windows(HWND hWnd, std::unique_ptr<const skwindow::DisplayParams> params):
    skwindow::internal::GLWindowContext(std::move(params)),
    m_hWnd(hWnd),
    m_fHGLRC(nullptr)
{
    ASSERT(::IsWindow(hWnd));
    fWidth = 0;
    fHeight = 0;
    this->initializeContext();
}

SkGLWindowContext_Windows::~SkGLWindowContext_Windows()
{
    this->destroyContext();
}

void SkGLWindowContext_Windows::resize(int nWidth, int nHeight)
{
    if (nWidth < 0) {
        nWidth = 0;
    }
    if (nHeight < 0) {
        nHeight = 0;
    }
    if ((fWidth == nWidth) && (fHeight == nHeight)) {
        return;
    }
    BaseClass::resize(nWidth, nHeight);
    ASSERT(fWidth == nWidth);
    ASSERT(fHeight == nHeight);
}

void SkGLWindowContext_Windows::onSwapBuffers()
{
    HDC dc = ::GetDC(m_hWnd);
    ::SwapBuffers(dc);
    ::ReleaseDC(m_hWnd, dc);
}

sk_sp<SkSurface> SkGLWindowContext_Windows::getBackbufferSurface()
{
#ifdef _DEBUG
    ASSERT(m_fHGLRC != nullptr);
    //Only one current OpenGL Context is allowed per thread, so only one window per thread can draw with OpenGL; other threads must draw with the CPU
    HGLRC hHGLRC = wglGetCurrentContext();
    ASSERT(m_fHGLRC == hHGLRC);
#endif

    return BaseClass::getBackbufferSurface();
}

sk_sp<const GrGLInterface> SkGLWindowContext_Windows::onInitializeContext()
{
    ASSERT(::IsWindow(m_hWnd));
    if (!::IsWindow(m_hWnd)) {
        return nullptr;
    }
    const skwindow::DisplayParams* pDisplayParams = getDisplayParams();
    ASSERT(pDisplayParams != nullptr);
    if (pDisplayParams == nullptr) {
        return nullptr;
    }

    if (m_fHGLRC != nullptr) {
        destroyContext();
    }

    HDC dc = ::GetDC(m_hWnd);
    m_fHGLRC = SkCreateWGLContext(dc, pDisplayParams->msaaSampleCount(), false /* deepColor */,
                                  kGLPreferCompatibilityProfile_SkWGLContextRequest);
    if (nullptr == m_fHGLRC) {
        ::ReleaseDC(m_hWnd, dc);
        return nullptr;
    }

    SkWGLExtensions extensions;
    if (extensions.hasExtension(dc, "WGL_EXT_swap_control")) {
        extensions.swapInterval(pDisplayParams->disableVsync() ? 0 : 1);
    }

    // Look to see if RenderDoc is attached. If so, re-create the context with a core profile
    if (wglMakeCurrent(dc, m_fHGLRC)) {
        auto interface = GrGLMakeNativeInterface();
        bool renderDocAttached = interface->hasExtension("GL_EXT_debug_tool");
        interface.reset(nullptr);
        if (renderDocAttached) {
            wglDeleteContext(m_fHGLRC);
            m_fHGLRC = SkCreateWGLContext(dc, pDisplayParams->msaaSampleCount(), false /* deepColor */,
                                          kGLPreferCoreProfile_SkWGLContextRequest);
            if (nullptr == m_fHGLRC) {
                ::ReleaseDC(m_hWnd, dc);
                return nullptr;
            }
        }
    }

    if (wglMakeCurrent(dc, m_fHGLRC)) {
        glClearStencil(0);
        glClearColor(0, 0, 0, 0);
        glStencilMask(0xffffffff);
        glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // use DescribePixelFormat to get the stencil and color bit depth.
        int pixelFormat = ::GetPixelFormat(dc);
        PIXELFORMATDESCRIPTOR pfd = {0, };
        DescribePixelFormat(dc, pixelFormat, sizeof(pfd), &pfd);
        fStencilBits = pfd.cStencilBits;

        // Get sample count if the MSAA WGL extension is present
        if (extensions.hasExtension(dc, "WGL_ARB_multisample")) {
            static const int kSampleCountAttr = SK_WGL_SAMPLES;
            extensions.getPixelFormatAttribiv(dc,
                pixelFormat,
                0,
                1,
                &kSampleCountAttr,
                &fSampleCount);
            fSampleCount = std::max(fSampleCount, 1);
        }
        else {
            fSampleCount = 1;
        }

        RECT rect;
        ::GetClientRect(m_hWnd, &rect);
        fWidth = rect.right - rect.left;
        fHeight = rect.bottom - rect.top;
        glViewport(0, 0, fWidth, fHeight);
    }
    ::ReleaseDC(m_hWnd, dc);
    return GrGLMakeNativeInterface();
}

void SkGLWindowContext_Windows::onDestroyContext()
{
    if (m_fHGLRC != nullptr) {
        ASSERT(m_fHGLRC == wglGetCurrentContext());
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_fHGLRC);
        m_fHGLRC = nullptr;
    }
}

bool SkGLWindowContext_Windows::PaintAndSwapBuffers(IRender* /*pRender*/, IRenderPaint* pRenderPaint)
{
    HWND hWnd = m_hWnd;
    SkASSERT(::IsWindow(hWnd));
    if ((hWnd == nullptr) || !::IsWindow(hWnd)) {
        return false;
    }
    SkASSERT(pRenderPaint != nullptr);
    if (pRenderPaint == nullptr) {
        return false;
    }

    //Set the area to be drawn (the entire client area must be drawn; GL does not support partial drawing)
    RECT rectUpdate = { 0, };
    ::GetClientRect(m_hWnd, &rectUpdate);
    UiRect rcPaint;
    rcPaint.left = rectUpdate.left;
    rcPaint.top = rectUpdate.top;
    rcPaint.right = rectUpdate.right;
    rcPaint.bottom = rectUpdate.bottom;
    if (rcPaint.IsEmpty()) {
        //No drawing needed
        return false;
    }

    sk_sp<SkSurface> backbuffer = getBackbufferSurface();
    ASSERT(backbuffer != nullptr);
    if (backbuffer == nullptr) {
        return false;
    }

    //Start drawing
    PAINTSTRUCT ps = { 0, };
    ::BeginPaint(hWnd, &ps);
    ::EndPaint(hWnd, &ps);

    //Perform drawing
    pRenderPaint->DoPaint(rcPaint);

    //Mark the drawn area as valid
    ::ValidateRect(hWnd, nullptr);

    //Submit the drawing result and refresh it to the device DC
    if (auto dContext = this->directContext()) {
        dContext->flushAndSubmit(backbuffer.get(), GrSyncCpu::kYes);
    }

    //After drawing, update to the window
    swapBuffers();
    return true;
}

} // namespace ui

#endif //DUILIB_BUILD_FOR_WIN
