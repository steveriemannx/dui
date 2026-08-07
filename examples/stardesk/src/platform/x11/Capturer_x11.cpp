// X11 screen capture via XGetImage from the root window (X11 session only;
// Wayland is not supported - see README).
// Mirror mode: the primary screen area; extend mode: the root window, which
// under Xinerama already spans all monitors.

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "../Capturer.h"

namespace sdk {

struct Capturer::Impl {
    Display* display = nullptr;
    Window root = 0;
    bool available = false;
    bool started = false;
    std::string error;
    int width = 0;
    int height = 0;
    int originX = 0;
    int originY = 0;
    bool extend = false;
};

Capturer::Capturer() : m_impl(new Impl()) {}

Capturer::~Capturer()
{
    Stop();
}

bool Capturer::PlatformSupported()
{
    Display* d = XOpenDisplay(nullptr);
    if (d == nullptr) {
        return false;
    }
    XCloseDisplay(d);
    return true;
}

bool Capturer::Start(bool extend)
{
    m_impl->error.clear();
    m_impl->display = XOpenDisplay(nullptr);
    if (m_impl->display == nullptr) {
        m_impl->error = "cannot open X display (Wayland sessions are not supported)";
        return false;
    }
    m_impl->extend = extend;
    const int screen = DefaultScreen(m_impl->display);
    m_impl->root = RootWindow(m_impl->display, screen);
    // extend: root spans all monitors (Xinerama); mirror: primary screen size
    m_impl->width = DisplayWidth(m_impl->display, screen);
    m_impl->height = DisplayHeight(m_impl->display, screen);
    m_impl->available = true;
    m_impl->started = true;
    return true;
}

void Capturer::Stop()
{
    m_impl->started = false;
    if (m_impl->display != nullptr) {
        XCloseDisplay(m_impl->display);
        m_impl->display = nullptr;
    }
}

bool Capturer::Capture(CaptureFrame& out)
{
    if (!m_impl->available || !m_impl->started || m_impl->display == nullptr) {
        return false;
    }
    const int w = m_impl->width;
    const int h = m_impl->height;

    XImage* img = XGetImage(m_impl->display, m_impl->root, 0, 0, w, h,
                            AllPlanes, ZPixmap);
    if (img == nullptr) {
        return false;
    }

    out.width = w;
    out.height = h;
    out.rgba.resize((size_t)w * h * 4);
    const unsigned long rMask = img->red_mask;
    const unsigned long gMask = img->green_mask;
    const unsigned long bMask = img->blue_mask;
    const int rShift = __builtin_ctzl(rMask);
    const int gShift = __builtin_ctzl(gMask);
    const int bShift = __builtin_ctzl(bMask);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const unsigned long px = XGetPixel(img, x, y);
            uint8_t* p = &out.rgba[(size_t)(y * w + x) * 4];
            p[0] = (uint8_t)((px & rMask) >> rShift);
            p[1] = (uint8_t)((px & gMask) >> gShift);
            p[2] = (uint8_t)((px & bMask) >> bShift);
            p[3] = 255;
        }
    }
    XDestroyImage(img);
    return true;
}

int Capturer::GetWidth() const { return m_impl->width; }
int Capturer::GetHeight() const { return m_impl->height; }
int Capturer::GetOriginX() const { return m_impl->originX; }
int Capturer::GetOriginY() const { return m_impl->originY; }
bool Capturer::IsAvailable() const { return m_impl->available; }
std::string Capturer::GetError() const { return m_impl->error; }

} // namespace sdk
