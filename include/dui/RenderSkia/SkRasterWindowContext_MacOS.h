#ifndef UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_MACOS_H_
#define UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_MACOS_H_

#include "dui/dui_config.h"

#if defined(DUI_BUILD_FOR_MACOS)

#include "SkiaHeaderBegin.h"

#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "src/base/SkAutoMalloc.h"
#include "tools/window/RasterWindowContext.h"

#ifndef SK_GL
    #error 请检查宏定义：SK_GL与Skia库的编译选项保持一致
#endif

#include "SkiaHeaderEnd.h"

namespace ui 
{
class UiRect;
class IRender;
class IRenderPaint;

/** CPU-raster window context for macOS native windows.
 *  The SkSurface wraps our own raster memory (like the SDL version); the
 *  pixels are blitted into the NSView's current CGContext on swap.
 */
class SkRasterWindowContext_MacOS: public skwindow::internal::RasterWindowContext
{
public:
    SkRasterWindowContext_MacOS(void* nsView, std::unique_ptr<const skwindow::DisplayParams> params);
    SkRasterWindowContext_MacOS(const SkRasterWindowContext_MacOS& r) = delete;
    SkRasterWindowContext_MacOS& operator = (const SkRasterWindowContext_MacOS& r) = delete;
    virtual ~SkRasterWindowContext_MacOS() override;

public:
    virtual sk_sp<SkSurface> getBackbufferSurface() override;
    virtual bool isValid() override { return m_nsView != nullptr; }
    virtual void resize(int w, int h) override;
    virtual void setDisplayParams(std::unique_ptr<const skwindow::DisplayParams> params) override;

public:
    /** Draw and swap to the screen (the Render implementation is already associated with the window); completes synchronously
    * @param [in] pRender The render engine interface
    * @param [in] pRenderPaint The callback interface needed for UI drawing
    */
    bool PaintAndSwapBuffers(IRender* pRender, IRenderPaint* pRenderPaint);

protected:
    virtual void onSwapBuffers() override;

    /** Present the raster pixels into the view's current CGContext
    * @param [in] rcPaint The drawn area
    * @param [in] nLayeredWindowAlpha The window alpha [0,255]
    * @return Returns true on success, false on failure
    */
    bool SwapPaintBuffers(const UiRect& rcPaint, uint8_t nLayeredWindowAlpha);

private:
    /** The Surface data
    */
    SkAutoMalloc m_fSurfaceMemory;

    /** The Surface interface
    */
    sk_sp<SkSurface> m_fBackbufferSurface;

    /** The NSView* hosting the presentation
    */
    void* m_nsView;
};

} // namespace ui

#endif //DUI_BUILD_FOR_MACOS

#endif // UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_MACOS_H_
