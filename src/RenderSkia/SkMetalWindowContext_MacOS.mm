#include "dui/RenderSkia/SkMetalWindowContext_MacOS.h"
#include "dui/Render/IRender.h"

#if defined(DUI_BUILD_FOR_MACOS)

#include "dui/RenderSkia/SkiaHeaderBegin.h"

#include "include/core/SkCanvas.h"
#include "include/gpu/ganesh/GrDirectContext.h"

#include <Cocoa/Cocoa.h>
#include <QuartzCore/CAMetalLayer.h>

#include "dui/RenderSkia/SkiaHeaderEnd.h"

namespace ui {

SkMetalWindowContext_MacOS::SkMetalWindowContext_MacOS(void* nsView, std::unique_ptr<const skwindow::DisplayParams> params):
    BaseClass(std::move(params)),
    m_nsView(nsView)
{
    fWidth = 0;
    fHeight = 0;
    this->initializeContext();
}

SkMetalWindowContext_MacOS::~SkMetalWindowContext_MacOS()
{
    this->destroyContext();
}

bool SkMetalWindowContext_MacOS::onInitializeContext()
{
    if (m_nsView == nullptr) {
        return false;
    }
    NSView* view = (__bridge NSView*)m_nsView;

    CAMetalLayer* metalLayer = [CAMetalLayer layer];
    metalLayer.device = (__bridge id<MTLDevice>)fDevice.get();
    metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    metalLayer.framebufferOnly = NO;
    metalLayer.contentsGravity = kCAGravityTopLeft;
    BOOL useVsync = fDisplayParams->disableVsync() ? NO : YES;
    metalLayer.displaySyncEnabled = useVsync;
    NSColorSpace* cs = view.window.colorSpace;
    if (cs != nil) {
        metalLayer.colorspace = cs.CGColorSpace;
    }
    CGFloat scale = (view.window != nil) ? view.window.backingScaleFactor : 1.0;
    if (scale < 1.0) {
        scale = 1.0;
    }
    metalLayer.contentsScale = scale;

    //Make the view layer-backed and host the Metal layer as a SUBLAYER. We do
    //NOT use view.layer = metalLayer (layer-hosting): that stops drawRect,
    //which is what drives dui's paint cycle (the ~60 Hz display timer calls
    //displayIfNeeded -> drawRect -> PaintWindow -> this context's paint).
    view.wantsLayer = YES;
    metalLayer.frame = view.bounds;
    metalLayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
    [view.layer addSublayer:metalLayer];

    //The base class (getBackbufferSurface -> [fMetalLayer nextDrawable]) uses
    //fMetalLayer - the skia mac template assigns it the same way.
    fMetalLayer = metalLayer;
    return true;
}

void SkMetalWindowContext_MacOS::onDestroyContext()
{
    m_currentSurface.reset();
    if (fMetalLayer != nil) {
        [fMetalLayer removeFromSuperlayer];
        fMetalLayer = nil;
    }
}

void SkMetalWindowContext_MacOS::resize(int w, int h)
{
    if (w < 0) {
        w = 0;
    }
    if (h < 0) {
        h = 0;
    }
    fWidth = w;
    fHeight = h;
    if ((fMetalLayer == nil) || (m_nsView == nullptr)) {
        return;
    }
    CAMetalLayer* metalLayer = fMetalLayer;
    NSView* view = (__bridge NSView*)m_nsView;
    //dui passes the CLIENT PIXEL size; the layer's drawableSize is in pixels
    //and its contentsScale maps the layer's point space to device pixels.
    metalLayer.drawableSize = CGSizeMake((CGFloat)w, (CGFloat)h);
    CGFloat scale = (view.window != nil) ? view.window.backingScaleFactor : 1.0;
    if (scale < 1.0) {
        scale = 1.0;
    }
    metalLayer.contentsScale = scale;
    metalLayer.frame = view.bounds;
}

sk_sp<SkSurface> SkMetalWindowContext_MacOS::getBackbufferSurface()
{
    //Acquire the drawable once per paint and return the SAME surface for the
    //whole paint (dui calls GetSkCanvas() many times while painting); the
    //cache is dropped in onSwapBuffers so the next frame gets a fresh drawable.
    if (m_currentSurface == nullptr) {
        m_currentSurface = BaseClass::getBackbufferSurface();
    }
    return m_currentSurface;
}

void SkMetalWindowContext_MacOS::onSwapBuffers()
{
    BaseClass::onSwapBuffers();
    m_currentSurface.reset();
}

bool SkMetalWindowContext_MacOS::PaintAndSwapBuffers(IRender* pRender, IRenderPaint* pRenderPaint)
{
    SkASSERT(pRender != nullptr);
    SkASSERT(pRenderPaint != nullptr);
    if ((pRender == nullptr) || (pRenderPaint == nullptr)) {
        return false;
    }
    sk_sp<SkSurface> backbuffer = getBackbufferSurface();
    if (backbuffer == nullptr) {
        return false;
    }
    SkCanvas* pCanvas = backbuffer->getCanvas();
    SkASSERT(pCanvas != nullptr);
    if (pCanvas == nullptr) {
        return false;
    }

    //Metal (like GL) redraws the whole window; GPU-fast, no partial paint.
    const UiRect rcPaint(0, 0, fWidth, fHeight);
    const bool bRet = pRenderPaint->DoPaint(rcPaint);
    if (bRet) {
        GrDirectContext* pContext = directContext();
        if (pContext != nullptr) {
            //Skia defers GPU work; submit the frame before presenting, exactly
            //like the GL backend (without this the back buffer stays empty).
            pContext->flushAndSubmit(backbuffer.get(), GrSyncCpu::kNo);
        }
    }
    //Always present: consumes (and releases) the acquired drawable.
    onSwapBuffers();
    return bRet;
}

} // namespace ui

#endif //DUI_BUILD_FOR_MACOS
