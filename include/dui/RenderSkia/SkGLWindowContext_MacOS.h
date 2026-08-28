#ifndef UI_RENDER_SKIA_GL_WINDOW_CONTEXT_MACOS_H_
#define UI_RENDER_SKIA_GL_WINDOW_CONTEXT_MACOS_H_

#include "dui/dui_config.h"

#if defined(DUI_BUILD_FOR_MACOS)
#include "dui/dui_config_macos.h"

#include "SkiaHeaderBegin.h"

#include "include/core/SkSurface.h"
#include "tools/window/GLWindowContext.h"

#ifndef SK_GL
    #error 请检查宏定义：SK_GL与Skia库的编译选项保持一致
#endif

#include "SkiaHeaderEnd.h"

namespace ui 
{
class UiRect;
class IRender;
class IRenderPaint;

/** OpenGL window context for macOS native windows.
 *  Origin: skia\tools\window\mac\GaneshGLWindowContext_mac.mm + dui's
 *  SkGLWindowContext_Windows, with the same function/variable naming so
 *  upstream Skia syncs stay comparable. Uses NSOpenGL (deprecated but still
 *  supported on macOS); Metal would require rebuilding Skia with
 *  skia_use_metal=true.
 */
class SkGLWindowContext_MacOS: public skwindow::internal::GLWindowContext
{
    typedef skwindow::internal::GLWindowContext BaseClass;
public:
    SkGLWindowContext_MacOS(void* nsView, std::unique_ptr<const skwindow::DisplayParams> params);
    SkGLWindowContext_MacOS(const SkGLWindowContext_MacOS& r) = delete;
    SkGLWindowContext_MacOS& operator = (const SkGLWindowContext_MacOS& r) = delete;
    virtual ~SkGLWindowContext_MacOS() override;

public:
    /** Draw and swap to the screen (the Render implementation is already associated with the window); completes synchronously
    * @param [in] pRender The render engine interface
    * @param [in] pRenderPaint The callback interface needed for UI drawing
    */
    bool PaintAndSwapBuffers(IRender* pRender, IRenderPaint* pRenderPaint);

protected:
    virtual void resize(int w, int h) override;
    virtual void onSwapBuffers() override;
    virtual sk_sp<SkSurface> getBackbufferSurface() override;
    virtual sk_sp<const GrGLInterface> onInitializeContext() override;
    virtual void onDestroyContext() override;

private:
    void teardownContext();

    /** The NSView* hosting the GL drawable
    */
    void* m_nsView;

    /** The NSOpenGLContext* (retained)
    */
    void* m_glContext;

    /** The NSOpenGLPixelFormat* (retained)
    */
    void* m_pixelFormat;
};

} // namespace ui

#endif //DUI_BUILD_FOR_MACOS

#endif // UI_RENDER_SKIA_GL_WINDOW_CONTEXT_MACOS_H_
