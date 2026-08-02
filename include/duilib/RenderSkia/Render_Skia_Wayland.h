#ifndef UI_RENDER_SKIA_RENDER_WAYLAND_H_
#define UI_RENDER_SKIA_RENDER_WAYLAND_H_

#include "duilib/RenderSkia/Render_Skia.h"

#ifdef DUILIB_BUILD_FOR_WAYLAND

namespace skwindow {
    class WindowContext;
}

// Wayland types, forward declarations
struct wl_surface;
struct wl_shm;

namespace ui {

/** Render engine implementation for Wayland
*/
class Render_Skia_Wayland: public Render_Skia
{
public:
    Render_Skia_Wayland(wl_surface* wlSurface, wl_shm* shm, RenderBackendType backendType);
    Render_Skia_Wayland(const Render_Skia_Wayland& r) = delete;
    Render_Skia_Wayland& operator = (const Render_Skia_Wayland& r) = delete;
    virtual ~Render_Skia_Wayland() override;

public:
    virtual RenderBackendType GetRenderBackendType() const override;
    virtual bool Resize(int32_t width, int32_t height) override;
    virtual int32_t GetWidth() const override;
    virtual int32_t GetHeight() const override;
    virtual std::unique_ptr<IRender> Clone() override;
    virtual bool PaintAndSwapBuffers(IRenderPaint* pRenderPaint) override;
    virtual bool SetWindowRoundRectRgn(const UiRect& rcWnd, float rx, float ry, bool bRedraw) override;
    virtual bool SetWindowRectRgn(const UiRect& rcWnd, bool bRedraw) override;
    virtual void ClearWindowRgn(bool bRedraw) override;
    virtual SkSurface* GetSkSurface() const override;
    virtual SkCanvas* GetSkCanvas() const override;

private:
    std::unique_ptr<skwindow::WindowContext> m_pWindowContext;
    RenderBackendType m_backendType;
    wl_surface* m_wlSurface;
};

} // namespace ui

#endif // DUILIB_BUILD_FOR_WAYLAND

#endif // UI_RENDER_SKIA_RENDER_WAYLAND_H_
