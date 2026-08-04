#ifndef UI_RENDER_SKIA_RENDER_WINDOWS_H_
#define UI_RENDER_SKIA_RENDER_WINDOWS_H_

#include "dui/RenderSkia/Render_Skia.h"

#ifdef DUI_BUILD_FOR_WIN
#include "dui/dui_config_windows.h"

namespace skwindow {
    class WindowContext;
}

namespace ui 
{
/** Windows implementation of the render engine interface
*/
class Render_Skia_Windows : public Render_Skia
{
public:
    /** Constructor
    * @param [in] hWnd The associated window handle, can be nullptr
    * @param [in] backendType The backend drawing type
    */
    Render_Skia_Windows(HWND hWnd, RenderBackendType backendType);
    Render_Skia_Windows(const Render_Skia_Windows& r) = delete;
    Render_Skia_Windows& operator = (const Render_Skia_Windows& r) = delete;
    virtual ~Render_Skia_Windows() override;

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

public:
    /** Get the DC handle; after use, call the ReleaseDC interface to release the resources
    */
    virtual HDC GetRenderDC(HWND hWnd) override;

    /** Release DC resources
    * @param [in] hdc The DC handle to release
    */
    virtual void ReleaseRenderDC(HDC hdc) override;
    
private:
    /** Delete the DC
    */
    void DeleteDC();

private:
    /** The WindowContext object
    */
    std::unique_ptr<skwindow::WindowContext> m_pWindowContext;

    /** The backend drawing type
    */
    RenderBackendType m_backendType;

    /** The associated window handle
    */
    HWND m_hWnd;

    /** The associated DC handle
    */
    HDC m_hDC;

    /** The original bitmap of the DC
    */
    HGDIOBJ m_hOldObj;
};

} // namespace ui

#endif // UI_RENDER_SKIA_RENDER_WINDOWS_H_

#endif //DUI_BUILD_FOR_WIN
