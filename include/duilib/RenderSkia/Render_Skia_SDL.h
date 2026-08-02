#ifndef UI_RENDER_SKIA_RENDER_SDL_H_
#define UI_RENDER_SKIA_RENDER_SDL_H_

#include "duilib/RenderSkia/Render_Skia.h"

#ifdef DUILIB_BUILD_FOR_SDL

namespace skwindow {
    class WindowContext;
}

//Forward declarations of SDL types
struct SDL_Window;

namespace ui 
{
/** SDL implementation of the render engine interface
*/
class Render_Skia_SDL: public Render_Skia
{
public:
    /** Constructor
    * @param [in] sdlWindow The associated window, can be nullptr
    * @param [in] backendType The backend drawing type
    */
    Render_Skia_SDL(SDL_Window* sdlWindow, RenderBackendType backendType);
    Render_Skia_SDL(const Render_Skia_SDL& r) = delete;
    Render_Skia_SDL& operator = (const Render_Skia_SDL& r) = delete;
    virtual ~Render_Skia_SDL() override;

public:
    /** Get the backend render type
    */
    virtual RenderBackendType GetRenderBackendType() const override;

    /** The size has changed
    */
    virtual bool Resize(int32_t width, int32_t height) override;

    /** Get the width
    */
    virtual int32_t GetWidth() const override;

    /** Get the height
    */
    virtual int32_t GetHeight() const override;

    /** Clone a new render object
    */
    virtual std::unique_ptr<IRender> Clone() override;

    /** Draw and swap to the screen (the Render implementation is already associated with the window); completes synchronously
    * @param [in] pRenderPaint The callback interface needed for UI drawing
    */
    virtual bool PaintAndSwapBuffers(IRenderPaint* pRenderPaint) override;

    /** Set the window shape to a rounded rectangle
    * @param [in] rcWnd The area for which to set the RGN; coordinates are screen coordinates
    * @param [in] rx The corner radius width; must not be 0
    * @param [in] ry The corner radius height; must not be 0
    * @param [in] bRedraw Whether to redraw
    */
    virtual bool SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw) override;

    /** Set the window shape to a rectangle
    * @param [in] rcWnd The area for which to set the RGN; coordinates are screen coordinates
    * @param [in] bRedraw Whether to redraw
    */
    virtual bool SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw) override;

    /** Clear the window shape setting and restore the system default shape
    * @param [in] bRedraw Whether to redraw
    */
    virtual void ClearWindowRgn(bool bRedraw) override;

    /** Get the SkSurface interface
    */
    virtual SkSurface* GetSkSurface() const override;

    /** Get the SkCanvas interface
    */
    virtual SkCanvas* GetSkCanvas() const override;

private:
#ifdef DUILIB_BUILD_FOR_WIN
    /** Get the DC handle; after use, call the ReleaseDC interface to release the resources
    */
    virtual HDC GetRenderDC(HWND hWnd) override;

    /** Release DC resources
    * @param [in] hdc The DC handle to release
    */
    virtual void ReleaseRenderDC(HDC hdc) override;
#endif
   
private:
    /** The WindowContext object
    */
    std::unique_ptr<skwindow::WindowContext> m_pWindowContext;

    /** The backend drawing type
    */
    RenderBackendType m_backendType;

    /** The associated window
    */
    SDL_Window* m_sdlWindow;
};

} // namespace ui

#endif //DUILIB_BUILD_FOR_SDL

#endif // UI_RENDER_SKIA_RENDER_SDL_H_
