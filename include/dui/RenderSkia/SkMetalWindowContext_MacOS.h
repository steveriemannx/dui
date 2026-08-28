#ifndef UI_RENDER_SKIA_METAL_WINDOW_CONTEXT_MACOS_H_
#define UI_RENDER_SKIA_METAL_WINDOW_CONTEXT_MACOS_H_

#include "dui/dui_config.h"

#if defined(DUI_BUILD_FOR_MACOS)
#include "dui/dui_config_macos.h"

#include "SkiaHeaderBegin.h"

#include "include/core/SkSurface.h"
#include "tools/window/MetalWindowContext.h"

#include "SkiaHeaderEnd.h"

namespace ui
{
class UiRect;
class IRender;
class IRenderPaint;

/** Metal window context for macOS native windows.
 *  Origin: skia\tools\window\mac\GaneshMetalWindowContext_mac.mm + the
 *  skwindow::internal::MetalWindowContext base, adapted to the dui paint
 *  model (PaintAndSwapBuffers + full-window GPU paint like the GL backend).
 *  Requires libskia.a built with skia_use_metal=true (see dui_deps.cmake).
 */
class SkMetalWindowContext_MacOS: public skwindow::internal::MetalWindowContext
{
    typedef skwindow::internal::MetalWindowContext BaseClass;
public:
    SkMetalWindowContext_MacOS(void* nsView, std::unique_ptr<const skwindow::DisplayParams> params);
    SkMetalWindowContext_MacOS(const SkMetalWindowContext_MacOS& r) = delete;
    SkMetalWindowContext_MacOS& operator = (const SkMetalWindowContext_MacOS& r) = delete;
    virtual ~SkMetalWindowContext_MacOS() override;

public:
    /** Draw and swap to the screen (the Render implementation is already associated with the window); completes synchronously
    * @param [in] pRender The render engine interface
    * @param [in] pRenderPaint The callback interface needed for UI drawing
    */
    bool PaintAndSwapBuffers(IRender* pRender, IRenderPaint* pRenderPaint);

protected:
    virtual sk_sp<SkSurface> getBackbufferSurface() override;
    virtual void resize(int w, int h) override;
    virtual void onSwapBuffers() override;
    virtual bool onInitializeContext() override;
    virtual void onDestroyContext() override;

private:
    /** The NSView* hosting the Metal layer
    */
    void* m_nsView;

    /** The drawable-backed surface for the current paint (the drawable is
    * acquired once per paint and released when the frame is presented)
    */
    sk_sp<SkSurface> m_currentSurface;
};

} // namespace ui

#endif //DUI_BUILD_FOR_MACOS

#endif // UI_RENDER_SKIA_METAL_WINDOW_CONTEXT_MACOS_H_
