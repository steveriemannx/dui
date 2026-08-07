#ifndef STARDESK_PLATFORM_INPUT_INJECT_H_
#define STARDESK_PLATFORM_INPUT_INJECT_H_

#include <string>

namespace sdk {

/** Remote input injection, platform implementation in
 *  src/platform/<os>/InputInject_*.{mm,cpp}:
 *    - macOS: CGEventPost (requires Accessibility permission)
 *    - Windows: SendInput
 *    - X11: XTest (XTestFakeMotionEvent/ButtonEvent/KeyEvent)
 */
class InputInjector {
public:
    /** Move the pointer to normalized coordinates (0..1) within the shared
     *  logical screen (screenW x screenH). */
    static bool MoveTo(double nx, double ny, int screenW, int screenH);
    /** Press/release a mouse button: 0=left, 1=right, 2=middle. */
    static bool Button(bool down, int button);
    /** Scroll wheel; positive deltaLines scrolls up. */
    static bool Wheel(int deltaLines);
    /** Key down/up by dui VirtualKeyCode (kVK_*). */
    static bool Key(uint16_t vkCode, bool down);

    /** Current pointer position in physical screen coordinates. */
    static bool GetCursorPos(int& x, int& y);

    /** True when injection is permitted (macOS Accessibility etc.). */
    static bool PermissionGranted();
    /** Human-readable permission hint for the UI. */
    static std::string PermissionHint();

    /** True when a StarDesk window (any instance) is under the physical
     *  point (x, y). The host skips injection there to break the
     *  same-machine feedback loop: a click aimed into the remote-view
     *  window would land on it and re-trigger an input event. Other
     *  platforms: false. */
    static bool IsOwnWindowAt(double x, double y);
    /** True when a StarDesk app is the focused/frontmost app (keys would
     *  echo back through the remote-view window). Other platforms: false. */
    static bool IsOwnAppFocused();
};

} // namespace sdk

#endif // STARDESK_PLATFORM_INPUT_INJECT_H_
