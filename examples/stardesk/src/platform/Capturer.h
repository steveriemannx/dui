#ifndef STARDESK_PLATFORM_CAPTURER_H_
#define STARDESK_PLATFORM_CAPTURER_H_

#include "dui/dui.h"

#include <memory>
#include <string>
#include <vector>

namespace sdk {

/** One captured frame: RGBA (top-left origin, 4 bytes/pixel). */
struct CaptureFrame {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> rgba;
};

/** Primary-display screen capture, platform implementation in
 *  src/platform/<os>/Capturer_*.{mm,cpp}:
 *    - macOS: ScreenCaptureKit (SCScreenshotManager, macOS 14+; requires
 *      Screen Recording permission)
 *    - Windows: GDI BitBlt from the desktop DC
 *    - X11: XGetImage from the root window
 */
class Capturer {
public:
    Capturer();
    ~Capturer();

    /** Open the capture source. mode: false = mirror primary display,
     *  true = span all displays into one canvas. Returns false when the
     *  source is unavailable (e.g. missing permission); GetError() explains.
     */
    bool Start(bool extend);
    void Stop();

    /** Grab one frame; returns false on transient failure (caller retries).
     *  On success `out` holds RGBA of GetWidth()xGetHeight(). */
    bool Capture(CaptureFrame& out);

    int GetWidth() const;
    int GetHeight() const;
    /** Origin of the captured area in physical screen coordinates
     *  (0,0 for the primary display; negative on multi-display layouts). */
    int GetOriginX() const;
    int GetOriginY() const;

    bool IsAvailable() const;
    /** Human-readable failure reason (permission hint etc.). */
    std::string GetError() const;

    /** True when the platform supports capturing the screen at all. */
    static bool PlatformSupported();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

/** Synthetic moving pattern used when the real screen cannot be captured
 *  (missing permission / unsupported platform), so the stream pipeline can
 *  still be exercised end to end.
 */
class TestPatternSource {
public:
    TestPatternSource();
    /** Returns a frame of the requested size; animates on every call. */
    bool Capture(CaptureFrame& out, int width, int height);

private:
    uint64_t m_tick = 0;
};

} // namespace sdk

#endif // STARDESK_PLATFORM_CAPTURER_H_
