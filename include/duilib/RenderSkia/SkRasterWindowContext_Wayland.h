#ifndef UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_WAYLAND_H_
#define UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_WAYLAND_H_

#include "duilib/duilib_config.h"

#ifdef DUILIB_BUILD_FOR_WAYLAND

#include "SkiaHeaderBegin.h"

#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "src/base/SkAutoMalloc.h"
#include "tools/window/RasterWindowContext.h"

#ifndef SK_GL
    #error Please check macro: SK_GL should match Skia library build options
#endif

#include "SkiaHeaderEnd.h"

#include <wayland-client.h>
#include <vector>

namespace ui {

class UiRect;
class IRender;
class IRenderPaint;

struct WaylandBuffer {
    wl_buffer* buffer;
    void* data;
    int32_t width;
    int32_t height;
    int32_t stride;
    size_t size;
    bool released;
};

/** RasterWindowContext for Wayland - manages SHM buffers and Skia surfaces
*/
class SkRasterWindowContext_Wayland: public skwindow::internal::RasterWindowContext
{
public:
    SkRasterWindowContext_Wayland(wl_surface* wlSurface, wl_shm* shm, std::unique_ptr<const skwindow::DisplayParams> params);
    SkRasterWindowContext_Wayland(const SkRasterWindowContext_Wayland& r) = delete;
    SkRasterWindowContext_Wayland& operator = (const SkRasterWindowContext_Wayland& r) = delete;
    virtual ~SkRasterWindowContext_Wayland() override;

public:
    virtual sk_sp<SkSurface> getBackbufferSurface() override;
    virtual bool isValid() override { return m_wlSurface != nullptr && m_shm != nullptr; }
    virtual void resize(int w, int h) override;
    virtual void setDisplayParams(std::unique_ptr<const skwindow::DisplayParams> params) override;

public:
    /** Paint and swap buffers to the Wayland surface
    */
    bool PaintAndSwapBuffers(IRender* pRender, IRenderPaint* pRenderPaint);

    /** Handle buffer release event (called from Wayland listener)
    */
    void OnBufferRelease(wl_buffer* buffer);

    /** Set frame callback received
    */
    void OnFrameCallback();

    /** Commit and flush the surface
    */
    void CommitAndFlush();

protected:
    virtual void onSwapBuffers() override;

    /** Swap paint buffers - copy rendered content to Wayland buffer and commit
    */
    bool SwapPaintBuffers(const UiRect& rcPaint, uint8_t nLayeredWindowAlpha);

    /** Get client rect
    */
    void GetClientRect(UiRect& rcClient) const;

    /** Mark painted area as valid
    */
    void ValidateRect(UiRect& rcPaint) const;

    /** Create a Wayland buffer from SHM pool
    */
    WaylandBuffer* CreateWaylandBuffer(int32_t width, int32_t height);

    /** Destroy a Wayland buffer
    */
    void DestroyWaylandBuffer(WaylandBuffer* wlBuf);

    /** Get a free buffer (or create one), optionally blocking until one is available
    */
    WaylandBuffer* GetFreeBuffer(int32_t width, int32_t height);

    /** Cleanup resources
    */
    void Clear();

private:
    /** Surface memory for Skia backbuffer
    */
    SkAutoMalloc m_fSurfaceMemory;

    /** Skia backbuffer surface
    */
    sk_sp<SkSurface> m_fBackbufferSurface;

    /** Wayland surface
    */
    wl_surface* m_wlSurface;

    /** Wayland SHM interface
    */
    wl_shm* m_shm;

    /** Wayland display (for roundtrips)
    */
    wl_display* m_display;

    /** Buffer pool
    */
    std::vector<WaylandBuffer*> m_buffers;

    /** Frame callback received flag
    */
    bool m_frameReceived;
};

} // namespace ui

#endif // DUILIB_BUILD_FOR_WAYLAND

#endif // UI_RENDER_SKIA_RASTER_WINDOW_CONTEXT_WAYLAND_H_
