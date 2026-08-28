#include "dui/RenderSkia/SkRasterWindowContext_MacOS.h"
#include "dui/Render/IRender.h"

#if defined(DUI_BUILD_FOR_MACOS)

#include "dui/RenderSkia/SkiaHeaderBegin.h"

#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPixmap.h"
#include "tools/window/RasterWindowContext.h"

#include <Cocoa/Cocoa.h>

#include "dui/RenderSkia/SkiaHeaderEnd.h"

namespace ui {

SkRasterWindowContext_MacOS::SkRasterWindowContext_MacOS(void* nsView, std::unique_ptr<const skwindow::DisplayParams> params):
    skwindow::internal::RasterWindowContext(std::move(params)),
    m_nsView(nsView)
{
    fWidth = 0;
    fHeight = 0;
}

SkRasterWindowContext_MacOS::~SkRasterWindowContext_MacOS()
{
    m_fBackbufferSurface.reset();
    m_fSurfaceMemory.reset();
}

void SkRasterWindowContext_MacOS::setDisplayParams(std::unique_ptr<const skwindow::DisplayParams> params)
{
    int32_t nWidth = width();
    int32_t nHeight = height();
    fDisplayParams = std::move(params);
    m_fBackbufferSurface.reset();
    m_fSurfaceMemory.reset();
    if ((nWidth > 0) && (nHeight > 0)) {
        this->resize(nWidth, nHeight);
    }
}

void SkRasterWindowContext_MacOS::resize(int nWidth, int nHeight)
{
    if (nWidth < 0) {
        nWidth = 0;
    }
    if (nHeight < 0) {
        nHeight = 0;
    }
    if ((fWidth == nWidth) && (fHeight == nHeight)) {
        if ((fWidth > 0) && (fHeight > 0)) {
            SkASSERT(m_fSurfaceMemory.get() != nullptr);
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

    m_fBackbufferSurface.reset();
    m_fSurfaceMemory.reset();
    if ((nWidth == 0) || (nHeight == 0)) {
        return;
    }

    m_fSurfaceMemory.reset((size_t)nWidth * nHeight * sizeof(uint32_t));
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

sk_sp<SkSurface> SkRasterWindowContext_MacOS::getBackbufferSurface()
{
    return m_fBackbufferSurface;
}

void SkRasterWindowContext_MacOS::onSwapBuffers()
{
    //The actual presentation happens in SwapPaintBuffers (needs the current CGContext)
}

bool SkRasterWindowContext_MacOS::SwapPaintBuffers(const UiRect& rcPaint, uint8_t nLayeredWindowAlpha)
{
    if (m_fBackbufferSurface == nullptr) {
        return false;
    }
    if (rcPaint.IsEmpty()) {
        return false;
    }

    SkPixmap pixmap;
    if (!m_fBackbufferSurface->peekPixels(&pixmap)) {
        return false;
    }

    CGContextRef cgContext = (CGContextRef)[[NSGraphicsContext currentContext] CGContext];
    if (cgContext == nullptr) {
        return false;
    }

    const int32_t width = pixmap.width();
    const int32_t height = pixmap.height();
    if ((width <= 0) || (height <= 0)) {
        return false;
    }

    //Build a CGImage from the N32 premul pixels. NOTE: this Skia fork's kN32
    //surface is RGBA in memory (verified: red = ff 00 00 ff), so the bitmap
    //info must be declared alpha-last + big-endian (RGBA) - declaring BGRA
    //here swapped the red/blue channels (and shifted yellow/green).
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGDataProviderRef provider = CGDataProviderCreateWithData(
        nullptr,
        pixmap.addr(),
        (size_t)pixmap.rowBytes() * height,
        nullptr);
    CGImageRef cgImage = CGImageCreate(
        width, height, 8, 32, pixmap.rowBytes(), colorSpace,
        kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big,
        provider, nullptr, false, kCGRenderingIntentDefault);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    if (cgImage == nullptr) {
        return false;
    }

    const bool bLayered = (nLayeredWindowAlpha < 255);
    if (bLayered) {
        CGContextSaveGState(cgContext);
        CGContextSetAlpha(cgContext, nLayeredWindowAlpha / 255.0f);
    }

    //The CGContext is in point coordinates; draw the pixel-sized image at the
    //point size (backing scale) so 1 image pixel = 1 device pixel on Retina.
    CGFloat scale = 1.0;
    NSView* view = (__bridge NSView*)m_nsView;
    if (view != nil) {
        scale = view.window.backingScaleFactor;
        if (scale < 1.0) {
            scale = 1.0;
        }
    }
    const CGFloat drawHeight = height / scale;

    //Only the dirty region needs to be presented. Crop the image (bounds-only,
    //no pixel copy) and draw the crop at the matching position.
    CGRect rcCrop = CGRectMake(rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height());
    CGImageRef dirtyImage = CGImageCreateWithImageInRect(cgImage, rcCrop);
    CGImageRelease(cgImage);
    if (dirtyImage == nullptr) {
        return false;
    }

    //The NSView CGContext is bottom-up; Skia's origin is top-left, so flip.
    //After the flip the origin sits at the view's bottom-left with y up; the
    //dirty region (top-left-origin client pixels) maps to
    //x = left/scale, y = drawHeight - bottom/scale.
    CGContextSaveGState(cgContext);
    CGContextTranslateCTM(cgContext, 0, drawHeight);
    CGContextScaleCTM(cgContext, 1.0, -1.0);
    CGRect rcDst = CGRectMake(rcPaint.left / scale,
                              drawHeight - rcPaint.bottom / scale,
                              rcPaint.Width() / scale,
                              rcPaint.Height() / scale);
    CGContextDrawImage(cgContext, rcDst, dirtyImage);
    CGContextRestoreGState(cgContext);

    if (bLayered) {
        CGContextRestoreGState(cgContext);
    }

    CGImageRelease(dirtyImage);
    return true;
}

bool SkRasterWindowContext_MacOS::PaintAndSwapBuffers(IRender* pRender, IRenderPaint* pRenderPaint)
{
    SkASSERT(pRender != nullptr);
    SkASSERT(pRenderPaint != nullptr);
    if ((pRender == nullptr) || (pRenderPaint == nullptr)) {
        return false;
    }
    if (m_fBackbufferSurface == nullptr) {
        return false;
    }

    //Get the area that needs to be drawn (partial paint support: only the dirty
    //region is redrawn, which keeps CPU rendering responsive on mouse tracking).
    UiRect rcPaint;
    bool bUpdateRect = pRenderPaint->GetUpdateRect(rcPaint); //true when partial painting is supported
    if (bUpdateRect && !rcPaint.IsEmpty()) {
        //Ensure the area is within the client area
        UiRect rcClient(0, 0, fWidth, fHeight);
        rcPaint.Intersect(rcClient);
    }
    if (rcPaint.IsEmpty()) {
        //Partial drawing is not supported; the entire client area must be redrawn
        rcPaint = UiRect(0, 0, fWidth, fHeight);
    }
    if (rcPaint.IsEmpty()) {
        //No drawing needed
        return false;
    }

    const bool bFullPaint = (rcPaint.Width() >= fWidth) && (rcPaint.Height() >= fHeight);
    SkCanvas* pCanvas = m_fBackbufferSurface->getCanvas();
    SkASSERT(pCanvas != nullptr);
    if (pCanvas == nullptr) {
        return false;
    }

    if (!bFullPaint) {
        //Clip the canvas so drawing cannot leak into unrelated areas; the rest
        //of the surface keeps the previous frame's content.
        pCanvas->save();
        pCanvas->clipIRect(SkIRect::MakeLTRB(rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom));
    }

    const bool bRet = pRenderPaint->DoPaint(rcPaint);
    if (bRet) {
        SwapPaintBuffers(rcPaint, pRenderPaint->GetLayeredWindowAlpha());
    }

    if (!bFullPaint) {
        pCanvas->restore();
    }
    return bRet;
}

} // namespace ui

#endif //DUI_BUILD_FOR_MACOS
