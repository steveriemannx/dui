#include "duilib/RenderSkia/SkRasterWindowContext_Wayland.h"
#include "duilib/Render/IRender.h"
#include "duilib/Core/MessageLoop_Wayland.h"
#include "duilib/Utils/PerformanceUtil.h"

#ifdef DUILIB_BUILD_FOR_WAYLAND

#include <sys/mman.h>
#include <unistd.h>
#include <cstring>

namespace ui {

// Buffer release listener
static void wl_buffer_release_handler(void* data, struct wl_buffer* /*buffer*/) {
    auto* wlBuf = static_cast<WaylandBuffer*>(data);
    if (wlBuf) wlBuf->released = true;
}
static const struct wl_buffer_listener wl_buffer_release_listener = {
    .release = wl_buffer_release_handler,
};

SkRasterWindowContext_Wayland::SkRasterWindowContext_Wayland(wl_surface* wlSurface, wl_shm* shm, std::unique_ptr<const skwindow::DisplayParams> params):
    skwindow::internal::RasterWindowContext(std::move(params)),
    m_wlSurface(wlSurface),
    m_shm(shm),
    m_display(MessageLoop_Wayland::GetDisplay()),
    m_frameReceived(true)
{
    fWidth = 0;
    fHeight = 0;
}

SkRasterWindowContext_Wayland::~SkRasterWindowContext_Wayland()
{
    Clear();
}

void SkRasterWindowContext_Wayland::Clear()
{
    for (auto* wlBuf : m_buffers) {
        DestroyWaylandBuffer(wlBuf);
    }
    m_buffers.clear();
    m_fBackbufferSurface.reset();
    m_fSurfaceMemory.reset();
}

void SkRasterWindowContext_Wayland::setDisplayParams(std::unique_ptr<const skwindow::DisplayParams> params)
{
    int32_t nWidth = width();
    int32_t nHeight = height();
    fDisplayParams = std::move(params);
    Clear();
    if ((nWidth > 0) && (nHeight > 0)) {
        this->resize(nWidth, nHeight);
    }
}

void SkRasterWindowContext_Wayland::resize(int nWidth, int nHeight)
{
    if (nWidth < 0) nWidth = 0;
    if (nHeight < 0) nHeight = 0;

    if ((fWidth == nWidth) && (fHeight == nHeight)) {
        if ((fWidth > 0) && (fHeight > 0)) {
            SkASSERT(m_fSurfaceMemory.get() != nullptr);
            SkASSERT(m_fBackbufferSurface != nullptr);
        }
        return;
    }

    const skwindow::DisplayParams* pDisplayParams = getDisplayParams();
    SkASSERT(pDisplayParams != nullptr);
    if (pDisplayParams == nullptr) return;

    fWidth = nWidth;
    fHeight = nHeight;

    m_fBackbufferSurface.reset();
    m_fSurfaceMemory.reset();

    if ((nWidth == 0) || (nHeight == 0)) return;

    m_fSurfaceMemory.reset(nWidth * nHeight * sizeof(uint32_t));
    void* pixels = m_fSurfaceMemory.get();
    SkASSERT(pixels != nullptr);
    if (pixels == nullptr) {
        m_fSurfaceMemory.reset();
        fWidth = 0;
        fHeight = 0;
        return;
    }

    SkImageInfo info = SkImageInfo::Make(nWidth, nHeight, pDisplayParams->colorType(),
                                          SkAlphaType::kPremul_SkAlphaType, pDisplayParams->colorSpace());
    m_fBackbufferSurface = SkSurfaces::WrapPixels(info, pixels, sizeof(uint32_t) * nWidth);
    if (m_fBackbufferSurface == nullptr) {
        m_fSurfaceMemory.reset();
        fWidth = 0;
        fHeight = 0;
        return;
    }
}

sk_sp<SkSurface> SkRasterWindowContext_Wayland::getBackbufferSurface()
{
    return m_fBackbufferSurface;
}

void SkRasterWindowContext_Wayland::onSwapBuffers()
{
}

void SkRasterWindowContext_Wayland::OnBufferRelease(wl_buffer* buffer)
{
    for (auto* wlBuf : m_buffers) {
        if (wlBuf->buffer == buffer) {
            wlBuf->released = true;
            return;
        }
    }
}

void SkRasterWindowContext_Wayland::OnFrameCallback()
{
    m_frameReceived = true;
}

void SkRasterWindowContext_Wayland::CommitAndFlush()
{
    if (m_wlSurface && m_display) {
        wl_surface_commit(m_wlSurface);
        wl_display_flush(m_display);
    }
}

WaylandBuffer* SkRasterWindowContext_Wayland::CreateWaylandBuffer(int32_t width, int32_t height)
{
    if (!m_shm || width <= 0 || height <= 0) return nullptr;

    int32_t stride = width * 4;
    int32_t size = stride * height;

    int fd = memfd_create("duilib-wl-shm", MFD_CLOEXEC | MFD_ALLOW_SEALING);
    if (fd < 0) return nullptr;
    if (ftruncate(fd, size) < 0) {
        close(fd);
        return nullptr;
    }

    void* data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        return nullptr;
    }

    wl_shm_pool* pool = wl_shm_create_pool(m_shm, fd, size);
    if (!pool) {
        munmap(data, size);
        close(fd);
        return nullptr;
    }

    wl_buffer* buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    if (!buffer) {
        munmap(data, size);
        return nullptr;
    }

    auto* wlBuf = new WaylandBuffer();
    wlBuf->buffer = buffer;
    wlBuf->data = data;
    wlBuf->width = width;
    wlBuf->height = height;
    wlBuf->stride = stride;
    wlBuf->size = size;
    wlBuf->released = true;

    // Set up buffer release listener
    wl_buffer_add_listener(buffer, &wl_buffer_release_listener, wlBuf);

    return wlBuf;
}

void SkRasterWindowContext_Wayland::DestroyWaylandBuffer(WaylandBuffer* wlBuf)
{
    if (!wlBuf) return;
    if (wlBuf->buffer) {
        wl_buffer_destroy(wlBuf->buffer);
    }
    if (wlBuf->data) {
        munmap(wlBuf->data, wlBuf->size);
    }
    delete wlBuf;
}

WaylandBuffer* SkRasterWindowContext_Wayland::GetFreeBuffer(int32_t width, int32_t height)
{
    // Find a released buffer with matching size
    for (auto* wlBuf : m_buffers) {
        if (wlBuf->released && wlBuf->width == width && wlBuf->height == height) {
            wlBuf->released = false;
            return wlBuf;
        }
    }

    // Wait for a buffer to be released if we have too many
    if (m_buffers.size() >= 3 && m_display) {
        int maxRetries = 10;
        while (maxRetries-- > 0) {
            wl_display_dispatch_pending(m_display);
            for (auto* wlBuf : m_buffers) {
                if (wlBuf->released && wlBuf->width == width && wlBuf->height == height) {
                    wlBuf->released = false;
                    return wlBuf;
                }
            }
        }
    }

    // Create a new buffer
    WaylandBuffer* wlBuf = CreateWaylandBuffer(width, height);
    if (wlBuf) {
        wlBuf->released = false;
        m_buffers.push_back(wlBuf);
    }
    return wlBuf;
}

bool SkRasterWindowContext_Wayland::PaintAndSwapBuffers(IRender* pRender, IRenderPaint* pRenderPaint)
{
    if (!m_wlSurface || !m_fBackbufferSurface || !pRender || !pRenderPaint) return false;

    UiRect rcPaint;
    bool bUpdateRect = pRenderPaint->GetUpdateRect(rcPaint);
    if (bUpdateRect && !rcPaint.IsEmpty()) {
        UiRect rcClient;
        GetClientRect(rcClient);
        rcPaint.Intersect(rcClient);
    }
    if (rcPaint.IsEmpty()) {
        GetClientRect(rcPaint);
    }
    if (rcPaint.IsEmpty()) return false;

    uint8_t nLayeredWindowAlpha = pRenderPaint->GetLayeredWindowAlpha();
    const bool bFullPaint = (rcPaint.Width() == width()) && (rcPaint.Height() == height());

    SkCanvas* skCanvas = nullptr;
    if (!bFullPaint) {
        skCanvas = m_fBackbufferSurface->getCanvas();
        if (skCanvas != nullptr) {
            skCanvas->save();
            skCanvas->clipIRect(SkIRect::MakeLTRB(rcPaint.left, rcPaint.top, rcPaint.right, rcPaint.bottom));
        }
    }

    bool bRet = pRenderPaint->DoPaint(rcPaint);
    if (bRet) {
        SwapPaintBuffers(rcPaint, nLayeredWindowAlpha);
    }

    if (bUpdateRect) {
        ValidateRect(rcPaint);
    }

    if (skCanvas != nullptr) {
        skCanvas->restore();
    }
    return bRet;
}

bool SkRasterWindowContext_Wayland::SwapPaintBuffers(const UiRect& rcPaint, uint8_t nLayeredWindowAlpha)
{
    PerformanceStat statPerformance(_T("PaintWindow, SkRasterWindowContext_Wayland::SwapPaintBuffers"));
    (void)nLayeredWindowAlpha;

    if (rcPaint.IsEmpty() || !m_wlSurface) return false;

    void* srcPixels = m_fSurfaceMemory.get();
    if (!srcPixels) return false;

    int32_t w = width();
    int32_t h = height();
    if (w <= 0 || h <= 0) return false;

    WaylandBuffer* wlBuf = GetFreeBuffer(w, h);
    if (!wlBuf) return false;

    // Copy rendered pixels, converting premultiplied alpha to straight alpha
    // Skia uses premultiplied alpha (R*A, G*A, B*A, A)
    // Wayland ARGB8888 expects straight alpha (R, G, B, A)
    auto premulToStraight = [](uint32_t* dst, const uint32_t* src, int count) {
        for (int i = 0; i < count; i++) {
            uint32_t p = src[i];
            uint8_t a = (p >> 24) & 0xFF;
            if (a == 0) { dst[i] = 0; continue; }
            if (a == 255) { dst[i] = p; continue; }
            uint8_t r = ((p >> 16) & 0xFF) * 255 / a;
            uint8_t g = ((p >> 8) & 0xFF) * 255 / a;
            uint8_t b = (p & 0xFF) * 255 / a;
            dst[i] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    };

    if ((rcPaint.Width() == w) && (rcPaint.Height() == h)) {
        premulToStraight((uint32_t*)wlBuf->data, (uint32_t*)srcPixels, h * w);
    } else {
        uint32_t* dst = (uint32_t*)wlBuf->data;
        uint32_t* src = (uint32_t*)srcPixels;
        int32_t copyWidth = rcPaint.Width();
        for (int32_t row = rcPaint.top; row < rcPaint.bottom; row++) {
            int off = row * w + rcPaint.left;
            premulToStraight(dst + off, src + off, copyWidth);
        }
    }

    wl_surface_attach(m_wlSurface, wlBuf->buffer, 0, 0);
    wl_surface_damage_buffer(m_wlSurface, rcPaint.left, rcPaint.top, rcPaint.Width(), rcPaint.Height());

    wl_surface_commit(m_wlSurface);
    if (m_display) {
        wl_display_flush(m_display);
        // Dispatch pending events to get buffer release callbacks
        wl_display_dispatch_pending(m_display);
    }

    return true;
}

void SkRasterWindowContext_Wayland::GetClientRect(UiRect& rcClient) const
{
    rcClient.left = 0;
    rcClient.top = 0;
    rcClient.right = rcClient.left + width();
    rcClient.bottom = rcClient.top + height();
}

void SkRasterWindowContext_Wayland::ValidateRect(UiRect& rcPaint) const
{
    (void)rcPaint;
}

} // namespace ui

#endif // DUILIB_BUILD_FOR_WAYLAND
