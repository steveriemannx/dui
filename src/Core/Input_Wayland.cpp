#include "dui/Core/NativeWindow_Wayland.h"
#include "dui/Core/MessageLoop_Wayland.h"
#include "dui/Core/Keycode.h"
#include "dui/Core/Keyboard.h"
#include "dui/Utils/StringConvert.h"

#ifdef DUI_BUILD_FOR_WAYLAND

#include <wayland-client.h>
#include <wayland-cursor.h>
#include <xdg-shell-client-protocol.h>
#include <xkbcommon/xkbcommon.h>
#if defined(__linux__)
#include <linux/input-event-codes.h>
#elif defined(__FreeBSD__)
#include <dev/evdev/input-event-codes.h>
#endif
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <map>
#include <string>

namespace ui {

// Mouse button tracking for double-click detection
struct MouseButtonState {
    uint32_t lastClickTime = 0;
    UiPoint lastClickPos;
    int clickCount = 0;
    bool isDown = false;
};

// Global input state
static wl_pointer* s_pPointer = nullptr;
static wl_keyboard* s_pKeyboard = nullptr;
static struct xkb_context* s_xkbContext = nullptr;
static struct xkb_keymap* s_xkbKeymap = nullptr;
static struct xkb_state* s_xkbState = nullptr;

static NativeWindow_Wayland* s_pPointerFocusWindow = nullptr;
static NativeWindow_Wayland* s_pKeyboardFocusWindow = nullptr;
static uint32_t s_serial = 0;
static uint32_t s_pointerEnterSerial = 0;
static uint32_t s_modifiers = 0;

static wl_cursor_theme* s_cursorTheme = nullptr;
static wl_surface* s_cursorSurface = nullptr;
static CursorType s_currentWaylandCursor = CursorType::kCursorArrow;

static UiPoint s_pointerPos;
static MouseButtonState s_leftButton;
static MouseButtonState s_rightButton;
static MouseButtonState s_middleButton;

// Map wl_surface to NativeWindow_Wayland
static std::map<wl_surface*, NativeWindow_Wayland*> s_surfaceToWindow;

static bool InitWaylandCursor()
{
    if (s_cursorTheme != nullptr) {
        return true;
    }
    wl_shm* shm = MessageLoop_Wayland::GetShm();
    wl_compositor* compositor = MessageLoop_Wayland::GetCompositor();
    if (shm == nullptr || compositor == nullptr) {
        return false;
    }
    s_cursorTheme = wl_cursor_theme_load(nullptr, 24, shm);
    s_cursorSurface = wl_compositor_create_surface(compositor);
    if (s_cursorTheme == nullptr || s_cursorSurface == nullptr) {
        if (s_cursorSurface != nullptr) {
            wl_surface_destroy(s_cursorSurface);
            s_cursorSurface = nullptr;
        }
        if (s_cursorTheme != nullptr) {
            wl_cursor_theme_destroy(s_cursorTheme);
            s_cursorTheme = nullptr;
        }
        return false;
    }
    return true;
}

static const char* GetWaylandCursorName(CursorType cursorType)
{
    switch (cursorType) {
    case CursorType::kCursorSizeWE:   return "ew-resize";
    case CursorType::kCursorSizeNS:   return "ns-resize";
    case CursorType::kCursorSizeNWSE: return "nwse-resize";
    case CursorType::kCursorSizeNESW: return "nesw-resize";
    case CursorType::kCursorSizeAll:  return "all-scroll";
    case CursorType::kCursorHand:     return "pointer";
    case CursorType::kCursorIBeam:    return "text";
    case CursorType::kCursorWait:     return "wait";
    default:                          return "default";
    }
}

static void SetWaylandCursor(CursorType cursorType, uint32_t serial)
{
    if (s_pPointer == nullptr || serial == 0 || !InitWaylandCursor()) {
        return;
    }
    if (cursorType == s_currentWaylandCursor) {
        return;
    }

    wl_cursor* cursor = wl_cursor_theme_get_cursor(s_cursorTheme, GetWaylandCursorName(cursorType));
    if (cursor == nullptr) {
        cursor = wl_cursor_theme_get_cursor(s_cursorTheme, "default");
    }
    if (cursor == nullptr || cursor->image_count == 0) {
        return;
    }
    wl_cursor_image* image = cursor->images[0];
    wl_buffer* buffer = wl_cursor_image_get_buffer(image);
    if (buffer == nullptr) {
        return;
    }
    wl_surface_attach(s_cursorSurface, buffer, 0, 0);
    wl_surface_damage(s_cursorSurface, 0, 0, image->width, image->height);
    wl_surface_commit(s_cursorSurface);
    wl_pointer_set_cursor(s_pPointer, serial, s_cursorSurface,
                          image->hotspot_x, image->hotspot_y);
    s_currentWaylandCursor = cursorType;
}

static CursorType GetResizeCursor(NativeWindow_Wayland* window, const UiPoint& pt)
{
    if (window == nullptr || window->IsWindowMaximized() || window->IsWindowFullscreen()) {
        return CursorType::kCursorArrow;
    }
    INativeWindow* owner = window->GetOwner();
    if (owner == nullptr) {
        return CursorType::kCursorArrow;
    }

    UiRect client;
    window->GetClientRect(client);
    UiPadding shadow;
    owner->OnNativeGetShadowCorner(shadow);
    client.Deflate(shadow);
    UiRect sizeBox = owner->OnNativeGetSizeBox();

    const bool left = pt.x >= client.left && pt.x < client.left + sizeBox.left;
    const bool right = pt.x > client.right - sizeBox.right && pt.x <= client.right;
    const bool top = pt.y >= client.top && pt.y < client.top + sizeBox.top;
    const bool bottom = pt.y > client.bottom - sizeBox.bottom && pt.y <= client.bottom;

    if (left && top) return CursorType::kCursorSizeNWSE;
    if (right && top) return CursorType::kCursorSizeNESW;
    if (left && bottom) return CursorType::kCursorSizeNESW;
    if (right && bottom) return CursorType::kCursorSizeNWSE;
    if (left || right) return CursorType::kCursorSizeWE;
    if (top || bottom) return CursorType::kCursorSizeNS;
    return CursorType::kCursorArrow;
}

// Forward declare keyboard repeat state
static int s_keyRepeatRate = 25;
static int s_keyRepeatDelay = 400;
static int s_keyRepeatKey = 0;
static uint32_t s_keyRepeatTime = 0;

// Convert evdev scancode (from xkb) to VirtualKeyCode
static VirtualKeyCode EvdevToVirtualKey(xkb_keycode_t keycode)
{
    // xkb keycodes are evdev scancodes + 8
    // We map common keys directly
    xkb_keysym_t sym = xkb_state_key_get_one_sym(s_xkbState, keycode);
    if (sym == XKB_KEY_NoSymbol) return VirtualKeyCode::kVK_None;

    // Letters
    if (sym >= XKB_KEY_a && sym <= XKB_KEY_z) {
        return (VirtualKeyCode)((int)VirtualKeyCode::kVK_A + (sym - XKB_KEY_a));
    }
    if (sym >= XKB_KEY_A && sym <= XKB_KEY_Z) {
        return (VirtualKeyCode)((int)VirtualKeyCode::kVK_A + (sym - XKB_KEY_A));
    }
    // Numbers
    if (sym >= XKB_KEY_0 && sym <= XKB_KEY_9) {
        return (VirtualKeyCode)((int)VirtualKeyCode::kVK_0 + (sym - XKB_KEY_0));
    }
    // Function keys
    if (sym >= XKB_KEY_F1 && sym <= XKB_KEY_F24) {
        return (VirtualKeyCode)((int)VirtualKeyCode::kVK_F1 + (sym - XKB_KEY_F1));
    }

    switch (sym) {
    case XKB_KEY_Return:        return VirtualKeyCode::kVK_RETURN;
    case XKB_KEY_Escape:        return VirtualKeyCode::kVK_ESCAPE;
    case XKB_KEY_BackSpace:     return VirtualKeyCode::kVK_BACK;
    case XKB_KEY_Tab:           return VirtualKeyCode::kVK_TAB;
    case XKB_KEY_space:         return VirtualKeyCode::kVK_SPACE;
    case XKB_KEY_Delete:        return VirtualKeyCode::kVK_DELETE;
    case XKB_KEY_Insert:        return VirtualKeyCode::kVK_INSERT;
    case XKB_KEY_Home:          return VirtualKeyCode::kVK_HOME;
    case XKB_KEY_End:           return VirtualKeyCode::kVK_END;
    case XKB_KEY_Page_Up:       return VirtualKeyCode::kVK_PRIOR;
    case XKB_KEY_Page_Down:     return VirtualKeyCode::kVK_NEXT;
    case XKB_KEY_Left:          return VirtualKeyCode::kVK_LEFT;
    case XKB_KEY_Right:         return VirtualKeyCode::kVK_RIGHT;
    case XKB_KEY_Up:            return VirtualKeyCode::kVK_UP;
    case XKB_KEY_Down:          return VirtualKeyCode::kVK_DOWN;
    case XKB_KEY_Caps_Lock:     return VirtualKeyCode::kVK_CAPITAL;
    case XKB_KEY_Num_Lock:      return VirtualKeyCode::kVK_NUMLOCK;
    case XKB_KEY_Scroll_Lock:   return VirtualKeyCode::kVK_SCROLL;
    case XKB_KEY_Print:         return VirtualKeyCode::kVK_SNAPSHOT;
    case XKB_KEY_Pause:         return VirtualKeyCode::kVK_PAUSE;
    case XKB_KEY_Shift_L:       return VirtualKeyCode::kVK_SHIFT;
    case XKB_KEY_Shift_R:       return VirtualKeyCode::kVK_SHIFT;
    case XKB_KEY_Control_L:     return VirtualKeyCode::kVK_CONTROL;
    case XKB_KEY_Control_R:     return VirtualKeyCode::kVK_CONTROL;
    case XKB_KEY_Alt_L:         return VirtualKeyCode::kVK_MENU;
    case XKB_KEY_Alt_R:         return VirtualKeyCode::kVK_MENU;
    case XKB_KEY_Super_L:       return VirtualKeyCode::kVK_LWIN;
    case XKB_KEY_Super_R:       return VirtualKeyCode::kVK_RWIN;
    case XKB_KEY_Menu:          return VirtualKeyCode::kVK_MENU;
    case XKB_KEY_comma:         return VirtualKeyCode::kVK_OEM_COMMA;
    case XKB_KEY_period:        return VirtualKeyCode::kVK_OEM_PERIOD;
    case XKB_KEY_slash:         return VirtualKeyCode::kVK_OEM_2;
    case XKB_KEY_semicolon:     return VirtualKeyCode::kVK_OEM_1;
    case XKB_KEY_apostrophe:    return VirtualKeyCode::kVK_OEM_7;
    case XKB_KEY_bracketleft:   return VirtualKeyCode::kVK_OEM_4;
    case XKB_KEY_bracketright:  return VirtualKeyCode::kVK_OEM_6;
    case XKB_KEY_backslash:     return VirtualKeyCode::kVK_OEM_5;
    case XKB_KEY_minus:         return VirtualKeyCode::kVK_OEM_MINUS;
    case XKB_KEY_equal:         return VirtualKeyCode::kVK_OEM_PLUS;
    case XKB_KEY_grave:         return VirtualKeyCode::kVK_OEM_3;
    case XKB_KEY_KP_Enter:      return VirtualKeyCode::kVK_SEPARATOR;
    case XKB_KEY_KP_Divide:     return VirtualKeyCode::kVK_DIVIDE;
    case XKB_KEY_KP_Multiply:   return VirtualKeyCode::kVK_MULTIPLY;
    case XKB_KEY_KP_Subtract:   return VirtualKeyCode::kVK_SUBTRACT;
    case XKB_KEY_KP_Add:        return VirtualKeyCode::kVK_ADD;
    case XKB_KEY_KP_Decimal:    return VirtualKeyCode::kVK_DECIMAL;
    case XKB_KEY_KP_0: case XKB_KEY_KP_Insert: return VirtualKeyCode::kVK_NUMPAD0;
    case XKB_KEY_KP_1: case XKB_KEY_KP_End:    return VirtualKeyCode::kVK_NUMPAD1;
    case XKB_KEY_KP_2: case XKB_KEY_KP_Down:   return VirtualKeyCode::kVK_NUMPAD2;
    case XKB_KEY_KP_3: case XKB_KEY_KP_Next:   return VirtualKeyCode::kVK_NUMPAD3;
    case XKB_KEY_KP_4: case XKB_KEY_KP_Left:   return VirtualKeyCode::kVK_NUMPAD4;
    case XKB_KEY_KP_5: case XKB_KEY_KP_Begin:  return VirtualKeyCode::kVK_NUMPAD5;
    case XKB_KEY_KP_6: case XKB_KEY_KP_Right:  return VirtualKeyCode::kVK_NUMPAD6;
    case XKB_KEY_KP_7: case XKB_KEY_KP_Home:   return VirtualKeyCode::kVK_NUMPAD7;
    case XKB_KEY_KP_8: case XKB_KEY_KP_Up:     return VirtualKeyCode::kVK_NUMPAD8;
    case XKB_KEY_KP_9: case XKB_KEY_KP_Prior:  return VirtualKeyCode::kVK_NUMPAD9;
    case XKB_KEY_XF86AudioRaiseVolume: return VirtualKeyCode::kVK_VOLUME_UP;
    case XKB_KEY_XF86AudioLowerVolume: return VirtualKeyCode::kVK_VOLUME_DOWN;
    case XKB_KEY_XF86AudioMute:        return VirtualKeyCode::kVK_VOLUME_MUTE;
    default: break;
    }
    return VirtualKeyCode::kVK_None;
}

static uint32_t GetModifiersFromXkb()
{
    uint32_t mod = 0;
    if (!s_xkbState) return mod;
    xkb_state_component comp = (xkb_state_component)(XKB_STATE_MODS_DEPRESSED | XKB_STATE_MODS_LATCHED | XKB_STATE_MODS_LOCKED);
    if (xkb_state_mod_index_is_active(s_xkbState, xkb_keymap_mod_get_index(s_xkbKeymap, "Control"), comp))
        mod |= ModifierKey::kControl;
    if (xkb_state_mod_index_is_active(s_xkbState, xkb_keymap_mod_get_index(s_xkbKeymap, "Shift"), comp))
        mod |= ModifierKey::kShift;
    if (xkb_state_mod_index_is_active(s_xkbState, xkb_keymap_mod_get_index(s_xkbKeymap, "Mod1"), comp))
        mod |= ModifierKey::kAlt;
    if (xkb_state_mod_index_is_active(s_xkbState, xkb_keymap_mod_get_index(s_xkbKeymap, "Mod4"), comp))
        mod |= ModifierKey::kWin;
    return mod;
}

// Register a surface-to-window mapping
void RegisterWaylandSurface(wl_surface* surface, NativeWindow_Wayland* window)
{
    s_surfaceToWindow[surface] = window;
}

void UnregisterWaylandSurface(wl_surface* surface)
{
    if (s_pPointerFocusWindow && s_pPointerFocusWindow->m_pWaylandSurface == surface)
        s_pPointerFocusWindow = nullptr;
    if (s_pKeyboardFocusWindow && s_pKeyboardFocusWindow->m_pWaylandSurface == surface)
        s_pKeyboardFocusWindow = nullptr;
    s_surfaceToWindow.erase(surface);
}

static NativeWindow_Wayland* FindWindow(wl_surface* surface)
{
    auto it = s_surfaceToWindow.find(surface);
    if (it != s_surfaceToWindow.end()) return it->second;
    return nullptr;
}

// Check double-click
static bool IsDoubleClick(MouseButtonState& btn, const UiPoint& pt)
{
    uint32_t now = 0; // Simple - no timestamp available from Wayland
    int dx = std::abs(pt.x - btn.lastClickPos.x);
    int dy = std::abs(pt.y - btn.lastClickPos.y);
    bool isDouble = (dx <= 4 && dy <= 4);
    if (isDouble) {
        btn.clickCount++;
    } else {
        btn.clickCount = 1;
    }
    btn.lastClickPos = pt;
    return (btn.clickCount == 2);
}

// === Pointer Listeners ===
static void pointer_enter_handler(void* data, wl_pointer* pointer, uint32_t serial,
    wl_surface* surface, wl_fixed_t sx, wl_fixed_t sy)
{
    (void)data; (void)pointer; (void)serial;
    NativeWindow_Wayland* window = FindWindow(surface);
    s_pPointerFocusWindow = window;
    s_pointerEnterSerial = serial;
    s_pointerPos.x = wl_fixed_to_int(sx);
    s_pointerPos.y = wl_fixed_to_int(sy);
    SetWaylandCursor(CursorType::kCursorArrow, serial);
}

static void pointer_leave_handler(void* data, wl_pointer* pointer, uint32_t serial,
    wl_surface* surface)
{
    (void)data; (void)pointer; (void)serial; (void)surface;
    if (s_pPointerFocusWindow) {
        SetWaylandCursor(CursorType::kCursorArrow, s_pointerEnterSerial);
        INativeWindow* pOwner = s_pPointerFocusWindow->GetOwner();
        if (pOwner && !s_pPointerFocusWindow->IsClosingWnd()) {
            bool bHandled = false;
            pOwner->OnNativeMouseLeaveMsg(NativeMsg(0, 0, 0), bHandled);
        }
    }
    s_pPointerFocusWindow = nullptr;
}

static void pointer_motion_handler(void* data, wl_pointer* pointer, uint32_t time,
    wl_fixed_t sx, wl_fixed_t sy)
{
    (void)data; (void)pointer; (void)time;
    s_pointerPos.x = wl_fixed_to_int(sx);
    s_pointerPos.y = wl_fixed_to_int(sy);

    if (s_pPointerFocusWindow && !s_pPointerFocusWindow->IsClosingWnd()) {
        SetWaylandCursor(GetResizeCursor(s_pPointerFocusWindow, s_pointerPos),
                         s_pointerEnterSerial);
        INativeWindow* pOwner = s_pPointerFocusWindow->GetOwner();
        if (pOwner) {
            uint32_t mod = GetModifiersFromXkb();
            s_pPointerFocusWindow->SetLastMousePos(s_pointerPos);
            bool bHandled = false;
            pOwner->OnNativeMouseMoveMsg(s_pointerPos, mod, false, NativeMsg(0, 0, 0), bHandled);
        }
    }
}

static void pointer_button_handler(void* data, wl_pointer* pointer, uint32_t serial,
    uint32_t time, uint32_t button, uint32_t state)
{
    (void)data; (void)pointer; (void)serial; (void)time;
    if (!s_pPointerFocusWindow || s_pPointerFocusWindow->IsClosingWnd()) return;

    INativeWindow* pOwner = s_pPointerFocusWindow->GetOwner();
    if (!pOwner) return;

    uint32_t mod = GetModifiersFromXkb();
    bool bHandled = false;
    bool isDown = (state == WL_POINTER_BUTTON_STATE_PRESSED);

    if (button == BTN_LEFT) {
        if (isDown) {
            // Check for caption/resize BEFORE dui processing (serial is fresh)
            if (s_pPointerFocusWindow) {
                s_pPointerFocusWindow->ProcessWaylandMoveResize(s_pointerPos, serial);
            }
            // Then let dui handle the click normally
            if (IsDoubleClick(s_leftButton, s_pointerPos)) {
                pOwner->OnNativeMouseLButtonDbClickMsg(s_pointerPos, mod, NativeMsg(0, 0, 0), bHandled);
            } else {
                pOwner->OnNativeMouseLButtonDownMsg(s_pointerPos, mod, NativeMsg(0, 0, 0), bHandled);
            }
            s_leftButton.isDown = true;
        } else {
            pOwner->OnNativeMouseLButtonUpMsg(s_pointerPos, mod, NativeMsg(0, 0, 0), bHandled);
            s_leftButton.isDown = false;
        }
    } else if (button == BTN_RIGHT) {
        if (isDown) {
            if (IsDoubleClick(s_rightButton, s_pointerPos)) {
                pOwner->OnNativeMouseRButtonDbClickMsg(s_pointerPos, mod, NativeMsg(0, 0, 0), bHandled);
            } else {
                pOwner->OnNativeMouseRButtonDownMsg(s_pointerPos, mod, NativeMsg(0, 0, 0), bHandled);
            }
            s_rightButton.isDown = true;
        } else {
            pOwner->OnNativeMouseRButtonUpMsg(s_pointerPos, mod, NativeMsg(0, 0, 0), bHandled);
            s_rightButton.isDown = false;
        }
    } else if (button == BTN_MIDDLE) {
        if (isDown) {
            if (IsDoubleClick(s_middleButton, s_pointerPos)) {
                pOwner->OnNativeMouseMButtonDbClickMsg(s_pointerPos, mod, NativeMsg(0, 0, 0), bHandled);
            } else {
                pOwner->OnNativeMouseMButtonDownMsg(s_pointerPos, mod, NativeMsg(0, 0, 0), bHandled);
            }
            s_middleButton.isDown = true;
        } else {
            pOwner->OnNativeMouseMButtonUpMsg(s_pointerPos, mod, NativeMsg(0, 0, 0), bHandled);
            s_middleButton.isDown = false;
        }
    }
}

static void pointer_axis_handler(void* data, wl_pointer* pointer, uint32_t time,
    uint32_t axis, wl_fixed_t value)
{
    (void)data; (void)pointer; (void)time;
    if (!s_pPointerFocusWindow || s_pPointerFocusWindow->IsClosingWnd()) return;
    if (axis != WL_POINTER_AXIS_VERTICAL_SCROLL) return;

    INativeWindow* pOwner = s_pPointerFocusWindow->GetOwner();
    if (!pOwner) return;

    int32_t delta = wl_fixed_to_int(value);
    if (delta == 0) return;
    // Wayland axis values are typically in scroll units (e.g., 15 per notch)
    // Scale to match dui/native backend wheel delta
    delta = -delta * 8; // Negate and scale to match typical wheel delta

    uint32_t mod = GetModifiersFromXkb();
    bool bHandled = false;
    pOwner->OnNativeMouseWheelMsg(delta, s_pointerPos, mod, NativeMsg(0, 0, 0), bHandled);
}

static const wl_pointer_listener pointer_listener = {
    .enter = pointer_enter_handler,
    .leave = pointer_leave_handler,
    .motion = pointer_motion_handler,
    .button = pointer_button_handler,
    .axis = pointer_axis_handler,
    .frame = [](void*, wl_pointer*) {},
    .axis_source = [](void*, wl_pointer*, uint32_t) {},
    .axis_stop = [](void*, wl_pointer*, uint32_t, uint32_t) {},
    .axis_discrete = [](void*, wl_pointer*, uint32_t, int32_t) {},
    .axis_value120 = [](void*, wl_pointer*, uint32_t, int32_t) {},
    .axis_relative_direction = [](void*, wl_pointer*, uint32_t, uint32_t) {},
};

// === Keyboard Listeners ===
static void keyboard_keymap_handler(void* data, wl_keyboard* keyboard, uint32_t format,
    int32_t fd, uint32_t size)
{
    (void)data; (void)keyboard;
    if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
        close(fd);
        return;
    }

    void* map = mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        close(fd);
        return;
    }

    if (!s_xkbContext) {
        s_xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    }

    if (s_xkbKeymap) xkb_keymap_unref(s_xkbKeymap);
    if (s_xkbState) xkb_state_unref(s_xkbState);

    s_xkbKeymap = xkb_keymap_new_from_string(s_xkbContext, (const char*)map,
        XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);

    if (s_xkbKeymap) {
        s_xkbState = xkb_state_new(s_xkbKeymap);
    } else {
        s_xkbState = nullptr;
    }

    munmap(map, size);
    close(fd);
}

static void keyboard_enter_handler(void* data, wl_keyboard* keyboard, uint32_t serial,
    wl_surface* surface, wl_array* keys)
{
    (void)data; (void)keyboard; (void)serial; (void)keys;
    NativeWindow_Wayland* window = FindWindow(surface);
    s_pKeyboardFocusWindow = window;
}

static void keyboard_leave_handler(void* data, wl_keyboard* keyboard, uint32_t serial,
    wl_surface* surface)
{
    (void)data; (void)keyboard; (void)serial; (void)surface;
    s_pKeyboardFocusWindow = nullptr;
    s_keyRepeatKey = 0;
}

static void keyboard_key_handler(void* data, wl_keyboard* keyboard, uint32_t serial,
    uint32_t time, uint32_t key, uint32_t state)
{
    (void)data; (void)keyboard; (void)serial; (void)time;
    if (!s_pKeyboardFocusWindow || s_pKeyboardFocusWindow->IsClosingWnd()) return;
    if (!s_xkbState) return;

    INativeWindow* pOwner = s_pKeyboardFocusWindow->GetOwner();
    if (!pOwner) return;

    xkb_keycode_t keycode = key + 8;
    uint32_t mod = GetModifiersFromXkb();
    VirtualKeyCode vkCode = EvdevToVirtualKey(keycode);

    if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
        // Update xkb state
        xkb_state_update_key(s_xkbState, keycode, XKB_KEY_DOWN);
        mod = GetModifiersFromXkb();

        bool bHandled = false;
        pOwner->OnNativeKeyDownMsg(vkCode, mod, NativeMsg(0, 0, 0), bHandled);

        // Handle text input for printable keys
        xkb_keysym_t sym = xkb_state_key_get_one_sym(s_xkbState, keycode);
        if (sym != XKB_KEY_NoSymbol) {
            char buf[8] = {};
            int len = xkb_keysym_to_utf8(sym, buf, sizeof(buf));
            // Control keys have UTF-8 representations too -- BackSpace is
            // 0x08, Return 0x0D, Escape 0x1B -- and sending them as text puts
            // them *into* the field: the caret moves along and a glyph the font
            // does not have is drawn in the space.  They are key events, not
            // characters; RichEdit handles them from the key handler that has
            // already run above.  Only printable characters are sent as text.
            const unsigned char firstByte = len > 0 ? (unsigned char)buf[0] : 0;
            if (len > 0 && len < 8 && firstByte >= 0x20 && firstByte != 0x7F) {
                // dui's RichEdit reads the character from msg.wParam (a
                // wide-string pointer) and msg.lParam (its length), NOT from
                // vkCode -- see RichEdit::OnInputChar.  Sending the character
                // as vkCode, which is what this used to do, left the edit
                // control with an empty string to insert: characters arrived
                // and nothing appeared in the field.  The keysym's UTF-8 is
                // converted and passed the way the Windows backend passes it,
                // with vkCode = kVK_None, so multi-byte input (Chinese) works
                // as well as ASCII.
                std::wstring textW = StringConvert::UTF8ToWString(std::string(buf, len));
                if (!textW.empty()) {
                    bHandled = false;
                    NativeMsg charMsg(0, (WPARAM)textW.c_str(), (LPARAM)textW.size());
                    pOwner->OnNativeCharMsg(VirtualKeyCode::kVK_None, mod, charMsg, bHandled);
                }
            }
        }
    } else {
        xkb_state_update_key(s_xkbState, keycode, XKB_KEY_UP);
        mod = GetModifiersFromXkb();
        bool bHandled = false;
        pOwner->OnNativeKeyUpMsg(vkCode, mod, NativeMsg(0, 0, 0), bHandled);
    }
}

static void keyboard_modifiers_handler(void* data, wl_keyboard* keyboard, uint32_t serial,
    uint32_t mods_depressed, uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    (void)data; (void)keyboard; (void)serial;
    if (s_xkbState) {
        xkb_state_update_mask(s_xkbState, mods_depressed, mods_latched, mods_locked, 0, 0, group);
    }
}

static void keyboard_repeat_info_handler(void* data, wl_keyboard* keyboard,
    int32_t rate, int32_t delay)
{
    (void)data; (void)keyboard;
    if (rate > 0) s_keyRepeatRate = 1000 / rate; // Convert chars/sec to ms interval
    if (delay > 0) s_keyRepeatDelay = delay;
}

static const wl_keyboard_listener keyboard_listener = {
    .keymap = keyboard_keymap_handler,
    .enter = keyboard_enter_handler,
    .leave = keyboard_leave_handler,
    .key = keyboard_key_handler,
    .modifiers = keyboard_modifiers_handler,
    .repeat_info = keyboard_repeat_info_handler,
};

// === Seat Listener ===
static void seat_capabilities_handler(void* data, wl_seat* seat, uint32_t caps)
{
    (void)data;
    wl_seat* pSeat = MessageLoop_Wayland::GetSeat();
    if (!pSeat) return;

    if (caps & WL_SEAT_CAPABILITY_POINTER) {
        if (!s_pPointer) {
            s_pPointer = wl_seat_get_pointer(pSeat);
            wl_pointer_add_listener(s_pPointer, &pointer_listener, nullptr);
        }
    } else {
        if (s_pPointer) {
            wl_pointer_release(s_pPointer);
            s_pPointer = nullptr;
        }
    }

    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        if (!s_pKeyboard) {
            s_pKeyboard = wl_seat_get_keyboard(pSeat);
            wl_keyboard_add_listener(s_pKeyboard, &keyboard_listener, nullptr);
        }
    } else {
        if (s_pKeyboard) {
            wl_keyboard_release(s_pKeyboard);
            s_pKeyboard = nullptr;
        }
    }
}

static const wl_seat_listener seat_listener = {
    .capabilities = seat_capabilities_handler,
    .name = [](void*, wl_seat*, const char*) {},
};

void MessageLoop_Wayland::InitWaylandInput()
{
    wl_seat* seat = MessageLoop_Wayland::GetSeat();
    if (seat) {
        wl_seat_add_listener(seat, &seat_listener, nullptr);
    }
}

void MessageLoop_Wayland::ShutdownWaylandInput()
{
    if (s_cursorSurface) {
        wl_surface_destroy(s_cursorSurface);
        s_cursorSurface = nullptr;
    }
    if (s_cursorTheme) {
        wl_cursor_theme_destroy(s_cursorTheme);
        s_cursorTheme = nullptr;
    }
    if (s_pPointer) {
        wl_pointer_release(s_pPointer);
        s_pPointer = nullptr;
    }
    if (s_pKeyboard) {
        wl_keyboard_release(s_pKeyboard);
        s_pKeyboard = nullptr;
    }
    if (s_xkbState) {
        xkb_state_unref(s_xkbState);
        s_xkbState = nullptr;
    }
    if (s_xkbKeymap) {
        xkb_keymap_unref(s_xkbKeymap);
        s_xkbKeymap = nullptr;
    }
    if (s_xkbContext) {
        xkb_context_unref(s_xkbContext);
        s_xkbContext = nullptr;
    }
    s_pPointerFocusWindow = nullptr;
    s_pKeyboardFocusWindow = nullptr;
    s_pointerEnterSerial = 0;
    s_currentWaylandCursor = CursorType::kCursorArrow;
    s_surfaceToWindow.clear();
}

// Exports for Keyboard_Wayland.cpp
struct xkb_state* GetWaylandXkbState() { return s_xkbState; }
struct xkb_keymap* GetWaylandXkbKeymap() { return s_xkbKeymap; }

} // namespace ui

#endif // DUI_BUILD_FOR_WAYLAND
