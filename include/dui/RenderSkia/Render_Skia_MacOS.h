#ifndef UI_RENDER_SKIA_RENDER_MACOS_H_
#define UI_RENDER_SKIA_RENDER_MACOS_H_

#include "dui/RenderSkia/Render_Skia.h"

#if defined(DUI_BUILD_FOR_MACOS)
#include "dui/dui_config_macos.h"

namespace skwindow {
    class WindowContext;
}

namespace ui 
{
/** macOS native implementation of the render engine interface.
 *  Metal / GL first (SkMetalWindowContext_MacOS / SkGLWindowContext_MacOS),
 *  CPU raster fallback (SkRasterWindowContext_MacOS) - same structure as
 *  Render_Skia_Windows.
 */
class Render_Skia_MacOS : public Render_Skia
{
public:
    /** Constructor
    * @param [in] nsView The associated NSView*, can be nullptr
    * @param [in] backendType The backend drawing type
    */
    Render_Skia_MacOS(void* nsView, RenderBackendType backendType);
    Render_Skia_MacOS(const Render_Skia_MacOS& r) = delete;
    Render_Skia_MacOS& operator = (const Render_Skia_MacOS& r) = delete;
    virtual ~Render_Skia_MacOS() override;

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

public:
    /** Get the SkSurface interface
    */
    virtual SkSurface* GetSkSurface() const override;

    /** Get the SkCanvas interface
    */
    virtual SkCanvas* GetSkCanvas() const override;

    /** Set the window shape to a rounded rectangle (no-op on macOS: windows are naturally shaped)
    */
    virtual bool SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw) override;

    /** Set the window shape to a rectangle (no-op on macOS)
    */
    virtual bool SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw) override;

    /** Clear the window shape setting (no-op on macOS)
    */
    virtual void ClearWindowRgn(bool bRedraw) override;

private:
    /** The NSView* hosting the render
    */
    void* m_nsView;

    /** The drawing backend type
    */
    RenderBackendType m_backendType;

    /** The window context
    */
    std::unique_ptr<skwindow::WindowContext> m_pWindowContext;
};

} // namespace ui

#endif //DUI_BUILD_FOR_MACOS

#endif // UI_RENDER_SKIA_RENDER_MACOS_H_
