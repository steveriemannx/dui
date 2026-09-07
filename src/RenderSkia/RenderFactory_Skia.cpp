#include "dui/RenderSkia/RenderFactory_Skia.h"
#include "dui/RenderSkia/Font_Skia.h"
#include "dui/RenderSkia/FontMgr_Skia.h"
#include "dui/RenderSkia/Bitmap_Skia.h"
#include "dui/RenderSkia/Brush_Skia.h"
#include "dui/RenderSkia/Pen_Skia.h"
#include "dui/RenderSkia/Path_Skia.h"
#include "dui/RenderSkia/Matrix_Skia.h"
#if defined(DUI_BUILD_FOR_X11)
#include "dui/RenderSkia/Render_Skia_X11.h"
#include "dui/Core/MessageLoop_X11.h"
#include <X11/Xlib.h>
#endif

#if defined (DUI_BUILD_FOR_WAYLAND)
    #include "dui/RenderSkia/Render_Skia_Wayland.h"
    #include "dui/Core/MessageLoop_Wayland.h"
    #include <wayland-client.h>

#elif defined (DUI_BUILD_FOR_WIN)
    #include "dui/RenderSkia/Render_Skia_Windows.h"

#elif defined (DUI_BUILD_FOR_MACOS)
    #include "dui/RenderSkia/Render_Skia_MacOS.h"
#endif

namespace ui {

class RenderFactory_Skia::TImpl
{
public:
    /** Skia font manager
    */
    std::shared_ptr<IFontMgr> m_pFontMgr;
};

RenderFactory_Skia::RenderFactory_Skia()
{
    m_impl = new TImpl;

    //Create the Skia font manager object, unique within the process
    m_impl->m_pFontMgr = std::make_shared<FontMgr_Skia>();
    ASSERT(m_impl->m_pFontMgr != nullptr);
}

RenderFactory_Skia::~RenderFactory_Skia()
{
    if (m_impl != nullptr) {
        delete m_impl;
        m_impl = nullptr;
    }
}

IFont* RenderFactory_Skia::CreateIFont()
{
    return new Font_Skia(m_impl->m_pFontMgr);
}

IPen* RenderFactory_Skia::CreatePen(UiColor color, float fWidth)
{
    return new Pen_Skia(color, fWidth);
}

IBrush* RenderFactory_Skia::CreateBrush(UiColor color)
{
    return new Brush_Skia(color);
}

IPath* RenderFactory_Skia::CreatePath()
{
    return new Path_Skia();
}

IMatrix* RenderFactory_Skia::CreateMatrix()
{
    return new Matrix_Skia();
}

IBitmap* RenderFactory_Skia::CreateBitmap()
{
    return new Bitmap_Skia();
}

IRender* RenderFactory_Skia::CreateRender(const IRenderDpiPtr& spRenderDpi, void* platformData, RenderBackendType backendType)
{
#if defined (DUI_BUILD_FOR_WAYLAND)
    wl_surface* wlSurface = (wl_surface*)platformData;
    wl_shm* shm = MessageLoop_Wayland::GetShm();
    IRender* pRender = new Render_Skia_Wayland(wlSurface, shm, backendType);
#elif defined(DUI_BUILD_FOR_X11)
    Display* display = MessageLoop_X11::GetDisplay();
    Window xWindow = static_cast<Window>(reinterpret_cast<uintptr_t>(platformData));
    XWindowAttributes attrs{};
    if (display != nullptr && xWindow != 0) XGetWindowAttributes(display, xWindow, &attrs);
    if (xWindow == 0 && display != nullptr) {
        attrs.visual = DefaultVisual(display, DefaultScreen(display));
        attrs.depth = DefaultDepth(display, DefaultScreen(display));
        attrs.width = 1;
        attrs.height = 1;
    }
    IRender* pRender = new Render_Skia_X11(display, xWindow, attrs.visual, attrs.depth, attrs.width, attrs.height, backendType);
#elif defined(DUI_BUILD_FOR_WIN)
    HWND hWnd = (HWND)platformData;
    IRender* pRender = new Render_Skia_Windows(hWnd, backendType);
#elif defined(DUI_BUILD_FOR_MACOS)
    void* nsView = platformData;
    IRender* pRender = new Render_Skia_MacOS(nsView, backendType);
#else
    UNUSED_VARIABLE(platformData);
    UNUSED_VARIABLE(backendType);
    IRender* pRender = nullptr;
#endif
    ASSERT(pRender != nullptr);
    if (pRender != nullptr) {
        pRender->SetRenderDpi(spRenderDpi);
    }    
    return pRender;
}

IFontMgr* RenderFactory_Skia::GetFontMgr() const
{
    ASSERT(m_impl->m_pFontMgr != nullptr);
    return m_impl->m_pFontMgr.get();
}

} // namespace ui
