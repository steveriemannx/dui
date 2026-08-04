#ifndef UI_RENDER_SKIA_GL_WINDOW_CONTEXT_WINDOWS_H_
#define UI_RENDER_SKIA_GL_WINDOW_CONTEXT_WINDOWS_H_

#include "dui/dui_config.h"

#ifdef DUI_BUILD_FOR_WIN
#include "dui/dui_config_windows.h"

#include "SkiaHeaderBegin.h"

#include "include/core/SkSurface.h"
#include "tools/window/GLWindowContext.h"

// The type of DisplayParams.fGrContextOptions is GrContextOptions:
// The struct size differs depending on whether the GR_TEST_UTILS macro is defined; if they are inconsistent, the program will crash. Check the consistency of this macro definition
#ifndef SK_GL
    #error 请检查宏定义：SK_GL与Skia库的编译选项保持一致
#endif

#include "SkiaHeaderEnd.h"

namespace ui 
{
class UiRect;
class IRender;
class IRenderPaint;

/** The origin of this class: skia\tools\window\win\GLWindowContext_win.cpp, with modifications
*   The function and variable naming follows Skia conventions, to make it easier to compare code changes when syncing code later.
*/
class SkGLWindowContext_Windows: public skwindow::internal::GLWindowContext
{
    typedef skwindow::internal::GLWindowContext BaseClass;
public:
    SkGLWindowContext_Windows(HWND hWnd, std::unique_ptr<const skwindow::DisplayParams> params);
    SkGLWindowContext_Windows(const SkGLWindowContext_Windows& r) = delete;
    SkGLWindowContext_Windows& operator = (const SkGLWindowContext_Windows& r) = delete;
    virtual ~SkGLWindowContext_Windows() override;

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

    /** After drawing, update the drawn data from the render engine to the window
    * @param [in] hPaintDC The DC currently being drawn to
    * @param [in] rcPaint The drawn area
    * @param [in] pRender The drawing engine interface, used to apply the drawing result to the window
    * @param [in] nWindowAlpha The window alpha
    * @return Returns true on success, false on failure
    */
    bool SwapPaintBuffers(HDC hPaintDC, const UiRect& rcPaint, IRender* pRender, uint8_t nWindowAlpha) const;

private:
    /** The window handle
    */
    HWND m_hWnd;

    /** The OpenGL render context
    */
    HGLRC m_fHGLRC;
};

} // namespace ui

#endif // UI_RENDER_SKIA_GL_WINDOW_CONTEXT_WINDOWS_H_

#endif //DUI_BUILD_FOR_WIN
