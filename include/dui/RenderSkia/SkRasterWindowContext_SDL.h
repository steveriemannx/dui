#ifndef UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_SDL_H_
#define UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_SDL_H_

#include "dui/dui_config.h"

#ifdef DUI_BUILD_FOR_SDL

#include "SkiaHeaderBegin.h"

#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "src/base/SkAutoMalloc.h"
#include "tools/window/RasterWindowContext.h"

// The type of DisplayParams.fGrContextOptions is GrContextOptions:
// The struct size differs depending on whether the GR_TEST_UTILS macro is defined; if they are inconsistent, the program will crash. Check the consistency of this macro definition
#ifndef SK_GL
    #error 请检查宏定义：SK_GL与Skia库的编译选项保持一致
#endif

#include "SkiaHeaderEnd.h"

//Forward declarations of SDL types
struct SDL_Window;
struct SDL_Texture;

namespace ui 
{
class UiRect;
class IRender;
class IRenderPaint;

/** The origin of this class: skia\tools\window\win\RasterWindowContext_win.cpp, with modifications
*   The function and variable naming follows Skia conventions, to make it easier to compare code changes when syncing code later.
*/
class SkRasterWindowContext_SDL: public skwindow::internal::RasterWindowContext
{
public:
    SkRasterWindowContext_SDL(SDL_Window* sdlWindow, std::unique_ptr<const skwindow::DisplayParams> params);
    SkRasterWindowContext_SDL(const SkRasterWindowContext_SDL& r) = delete;
    SkRasterWindowContext_SDL& operator = (const SkRasterWindowContext_SDL& r) = delete;
    virtual ~SkRasterWindowContext_SDL() override;

public:
    virtual sk_sp<SkSurface> getBackbufferSurface() override;
    virtual bool isValid() override { return m_sdlWindow != nullptr; }
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

    /** After drawing, update the drawn data from the render engine to the window
    * @param [in] hPaintDC The DC currently being drawn to
    * @param [in] rcPaint The drawn area
    * @param [in] pRender The drawing engine interface, used to apply the drawing result to the window
    * @param [in] nLayeredWindowAlpha The window alpha, used as a parameter in the UpdateLayeredWindow function
    * @return Returns true on success, false on failure
    */
    bool SwapPaintBuffers(const UiRect& rcPaint, uint8_t nLayeredWindowAlpha);

    /** After drawing, update the drawn data from the render engine to the window (update the drawn data to the window device directly through the window's Surface)
    * @param [in] hPaintDC The DC currently being drawn to
    * @param [in] rcPaint The drawn area
    * @param [in] pRender The drawing engine interface, used to apply the drawing result to the window
    * @param [in] nLayeredWindowAlpha The window alpha, used as a parameter in the UpdateLayeredWindow function
    * @return Returns true on success, false on failure
    */
    bool SwapPaintBuffersFast(const UiRect& rcPaint, uint8_t nLayeredWindowAlpha);

    /** Get the client rectangle of the current window
    * @param [out] rcClient Returns the client area coordinates of the window
    */
    void GetClientRect(UiRect& rcClient) const;

    /** Mark the drawn area as valid
    */
    void ValidateRect(UiRect& rcPaint) const;

    /** Clean up resources
    */
    void Clear();

private:
    /** Get the color component order of Skia
    */
    bool GetSkiaColorByteOrder(SkColorType backSurfaceColorType, int32_t& backR, int32_t& backG, int32_t& backB, int32_t& backA) const;

    /** Get the color component order of SDL
    */
    bool GetSDLColorByteOrder(int32_t sdlFormat, int32_t& sdlR, int32_t& sdlG, int32_t& sdlB, int32_t& sdlA) const;

    /** Get the color component order from the mask
    */
    int32_t GetColorByteOrder(uint32_t mask) const;

    /** Update the color component order
    */
    void UpdateColorByteOrder(void* surfacePixels, int32_t nSurfaceWidth, const UiRect& rcPaint,
                              int32_t backR, int32_t backG, int32_t backB, int32_t backA,
                              int32_t sdlR, int32_t sdlG, int32_t sdlB, int32_t sdlA) const;

    /** Update the Alpha value in the color
    */
    void UpdateColorAlpha(void* surfacePixels, int32_t nSurfaceWidth, const UiRect& rcPaint, uint8_t nLayeredWindowAlpha,
                          int32_t sdlR, int32_t sdlG, int32_t sdlB, int32_t sdlA);

private:
    /** The Surface data
    */
    SkAutoMalloc m_fSurfaceMemory;

    /** The Surface interface
    */
    sk_sp<SkSurface> m_fBackbufferSurface;

    /** The associated window
    */
    SDL_Window* m_sdlWindow;

    /** The Texture drawn by SDL
    */
    SDL_Texture* m_sdlTextrue;
};

} // namespace ui

#endif //DUI_BUILD_FOR_SDL

#endif // UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_SDL_H_
