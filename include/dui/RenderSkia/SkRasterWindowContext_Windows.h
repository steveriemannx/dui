#ifndef UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_WINDOWS_H_
#define UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_WINDOWS_H_

#include "dui/dui_config.h"

#ifdef DUI_BUILD_FOR_WIN
#include "dui/dui_config_windows.h"

#include "SkiaHeaderBegin.h"
#include "include/core/SkSurface.h"
#include "tools/window/RasterWindowContext.h"
#include "SkiaHeaderEnd.h"

// The type of DisplayParams.fGrContextOptions is GrContextOptions:
// The struct size differs depending on whether the GR_TEST_UTILS macro is defined; if they are inconsistent, the program will crash. Check the consistency of this macro definition
#ifndef SK_GL
    #error 请检查宏定义：SK_GL与Skia库的编译选项保持一致
#endif

namespace ui 
{
class UiRect;
class IRender;
class IRenderPaint;

/** The origin of this class: skia\tools\window\win\RasterWindowContext_win.cpp, with modifications
*   The function and variable naming follows Skia conventions, to make it easier to compare code changes when syncing code later.
*/
class SkRasterWindowContext_Windows: public skwindow::internal::RasterWindowContext
{
public:
    SkRasterWindowContext_Windows(HWND hWnd, std::unique_ptr<const skwindow::DisplayParams> params);
    SkRasterWindowContext_Windows(const SkRasterWindowContext_Windows& r) = delete;
    SkRasterWindowContext_Windows& operator = (const SkRasterWindowContext_Windows& r) = delete;
    virtual ~SkRasterWindowContext_Windows() override;

public:
    virtual sk_sp<SkSurface> getBackbufferSurface() override;
    virtual bool isValid() override { return SkToBool(m_hWnd); }
    virtual void resize(int w, int h) override;
    virtual void setDisplayParams(std::unique_ptr<const skwindow::DisplayParams> params) override;

public:
    /** Draw and swap to the screen (the Render implementation is already associated with the window); completes synchronously
    * @param [in] pRender The render engine interface
    * @param [in] pRenderPaint The callback interface needed for UI drawing
    */
    bool PaintAndSwapBuffers(IRender* pRender, IRenderPaint* pRenderPaint);

    /** Get the GDI handle of the bitmap
    */
    HBITMAP GetHBitmap() const;

protected:
    virtual void onSwapBuffers() override;

    /** After drawing, update the drawn data from the render engine to the window
    * @param [in] hPaintDC The DC currently being drawn to
    * @param [in] rcPaint The drawn area
    * @param [in] pRender The drawing engine interface, used to apply the drawing result to the window
    * @param [in] nLayeredWindowAlpha The window alpha, used as a parameter in the UpdateLayeredWindow function
    * @return Returns true on success, false on failure
    */
    bool SwapPaintBuffers(HDC hPaintDC, const UiRect& rcPaint, IRender* pRender, uint8_t nLayeredWindowAlpha) const;

    /** Get the client rectangle of the current window
    * @param [out] rcClient Returns the client area coordinates of the window
    */
    void GetClientRect(UiRect& rcClient) const;

    /** Get the window rectangle of the current window
    * @param [out] rcWindow Returns the screen coordinates of the top-left and bottom-right corners of the window
    */
    void GetWindowRect(UiRect& rcWindow) const;

    /** Create a device-independent bitmap
    *@return Returns the bitmap handle; the caller releases the bitmap resources
    */
    HBITMAP CreateHBitmap(int32_t nWidth, int32_t nHeight, bool flipHeight, LPVOID* pBits) const;

private:
    /** The Surface interface
    */
    sk_sp<SkSurface> m_fBackbufferSurface;

    /** The window handle
    */
    HWND m_hWnd;

    /** The GDI handle of the bitmap
    */
    HBITMAP m_hBitmap;
};

} // namespace ui

#endif // UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_WINDOWS_H_

#endif //DUI_BUILD_FOR_WIN
