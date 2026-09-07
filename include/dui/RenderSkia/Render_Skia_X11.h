#ifndef UI_RENDER_SKIA_RENDER_X11_H_
#define UI_RENDER_SKIA_RENDER_X11_H_

#include "dui/RenderSkia/Render_Skia.h"

#ifdef DUI_BUILD_FOR_X11

namespace skwindow {
    class WindowContext;
}

// X11 types, forward declarations
#include <X11/Xlib.h>

namespace ui {

/** Render engine implementation for X11
*/
class Render_Skia_X11: public Render_Skia
{
public:
    Render_Skia_X11(Display* display, Window window, Visual* visual, int depth, int width, int height, RenderBackendType backendType);
    Render_Skia_X11(const Render_Skia_X11& r) = delete;
    Render_Skia_X11& operator = (const Render_Skia_X11& r) = delete;
    virtual ~Render_Skia_X11() override;

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
    Display* m_display;
    Window m_window;
    Visual* m_visual;
    int m_depth;
    int m_width;
    int m_height;
};

} // namespace ui

#endif // DUI_BUILD_FOR_X11

#endif // UI_RENDER_SKIA_RENDER_X11_H_
