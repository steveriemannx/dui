#include "dui/RenderSkia/SkGLWindowContext_MacOS.h"
#include "dui/Render/IRender.h"

#if defined(DUI_BUILD_FOR_MACOS)

#include "dui/RenderSkia/SkiaHeaderBegin.h"

#include "include/core/SkCanvas.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/gl/mac/GrGLMakeMacInterface.h"
#include "tools/window/GLWindowContext.h"

#include <Cocoa/Cocoa.h>
#include <OpenGL/gl.h>

#include "dui/RenderSkia/SkiaHeaderEnd.h"

// NSOpenGL is deprecated on macOS; the whole file is one deprecation scope.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

namespace ui {

SkGLWindowContext_MacOS::SkGLWindowContext_MacOS(void* nsView, std::unique_ptr<const skwindow::DisplayParams> params):
    skwindow::internal::GLWindowContext(std::move(params)),
    m_nsView(nsView),
    m_glContext(nullptr),
    m_pixelFormat(nullptr)
{
    fWidth = 0;
    fHeight = 0;
    this->initializeContext();
}

SkGLWindowContext_MacOS::~SkGLWindowContext_MacOS()
{
    this->teardownContext();
    this->destroyContext();
}

void SkGLWindowContext_MacOS::teardownContext()
{
    NSOpenGLContext* glContext = (__bridge NSOpenGLContext*)m_glContext;
    if (glContext != nil) {
        [glContext clearDrawable];
    }
}

sk_sp<const GrGLInterface> SkGLWindowContext_MacOS::onInitializeContext()
{
    NSView* view = (__bridge NSView*)m_nsView;
    ASSERT(view != nil);
    if (view == nil) {
        return nullptr;
    }

    //Best-resolution backing: the GL drawable is pixel-sized (points x backing
    //scale), matching dui's pixel-density client rect on Retina displays.
    [view setWantsBestResolutionOpenGLSurface:YES];

    // Build the pixel format (window backing scale aware; MSAA handled by DisplayParams).
    NSOpenGLPixelFormatAttribute attribs[] = {
        NSOpenGLPFAClosestPolicy,
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        0
    };
    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
    if (pixelFormat == nil) {
        // Fall back to the default pixel format when the accelerated one fails.
        pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:nil];
    }
    if (pixelFormat == nil) {
        return nullptr;
    }

    NSOpenGLContext* glContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    if (glContext == nil) {
        return nullptr;
    }
    [glContext setView:view];
    [glContext makeCurrentContext];

    GLint swapInterval = 0;
    [glContext setValues:&swapInterval forParameter:NSOpenGLContextParameterSwapInterval];

    m_pixelFormat = (__bridge_retained void*)pixelFormat;
    m_glContext = (__bridge_retained void*)glContext;

    // Query the backing scale so fWidth/fHeight track the pixel size.
    CGFloat scale = [view convertRectToBacking:view.bounds].size.width / MAX(view.bounds.size.width, 1.0);
    fWidth = (int)(view.bounds.size.width * scale);
    fHeight = (int)(view.bounds.size.height * scale);

    return GrGLInterfaces::MakeMac();
}

void SkGLWindowContext_MacOS::onDestroyContext()
{
    this->teardownContext();

    if (m_glContext != nullptr) {
        NSOpenGLContext* glContext = (__bridge_transfer NSOpenGLContext*)m_glContext;
        glContext = nil;
        m_glContext = nullptr;
    }
    if (m_pixelFormat != nullptr) {
        NSOpenGLPixelFormat* pixelFormat = (__bridge_transfer NSOpenGLPixelFormat*)m_pixelFormat;
        pixelFormat = nil;
        m_pixelFormat = nullptr;
    }
}

void SkGLWindowContext_MacOS::resize(int w, int h)
{
    if (w < 0) {
        w = 0;
    }
    if (h < 0) {
        h = 0;
    }
    if ((fWidth == w) && (fHeight == h)) {
        return;
    }
    BaseClass::resize(w, h);

    NSView* view = (__bridge NSView*)m_nsView;
    NSOpenGLContext* glContext = (__bridge NSOpenGLContext*)m_glContext;
    if ((view != nil) && (glContext != nil)) {
        [glContext update];
    }
}

void SkGLWindowContext_MacOS::onSwapBuffers()
{
    NSOpenGLContext* glContext = (__bridge NSOpenGLContext*)m_glContext;
    if (glContext != nil) {
        [glContext flushBuffer];
    }
}

sk_sp<SkSurface> SkGLWindowContext_MacOS::getBackbufferSurface()
{
    if (fWidth == 0 || fHeight == 0) {
        return nullptr;
    }
    return BaseClass::getBackbufferSurface();
}

bool SkGLWindowContext_MacOS::PaintAndSwapBuffers(IRender* pRender, IRenderPaint* pRenderPaint)
{
    ASSERT(pRender != nullptr);
    ASSERT(pRenderPaint != nullptr);
    if ((pRender == nullptr) || (pRenderPaint == nullptr)) {
        return false;
    }

    //Skia's GL surface wraps the CURRENT context's default FBO (glGetIntegerv
    //GL_FRAMEBUFFER_BINDING); without a current context the GL calls no-op and
    //the window renders black. Make the context current for the whole paint.
    NSOpenGLContext* glContext = (__bridge NSOpenGLContext*)m_glContext;
    if (glContext == nil) {
        return false;
    }
    [glContext makeCurrentContext];

    SkSurface* pBackSurface = this->getBackbufferSurface().get();
    ASSERT(pBackSurface != nullptr);
    if (pBackSurface == nullptr) {
        return false;
    }
    SkCanvas* pCanvas = pBackSurface->getCanvas();
    ASSERT(pCanvas != nullptr);
    if (pCanvas == nullptr) {
        return false;
    }
    pCanvas->clear(SK_ColorTRANSPARENT);

    const UiRect rcPaint(0, 0, fWidth, fHeight);
    if (!pRenderPaint->DoPaint(rcPaint)) {
        return false;
    }

    //Submit the drawing result to the GPU. Skia defers rendering; without this
    //the back buffer never receives the drawn content and the window is black.
    if (auto dContext = this->directContext()) {
        dContext->flushAndSubmit(pBackSurface, GrSyncCpu::kYes);
    }

    this->onSwapBuffers();
    return true;
}

} // namespace ui

#pragma clang diagnostic pop

#endif //DUI_BUILD_FOR_MACOS
