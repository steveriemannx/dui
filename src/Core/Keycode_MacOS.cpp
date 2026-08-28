#include "dui/Core/Keycode.h"

#if defined(DUI_BUILD_FOR_MACOS)

#include <cstdint>

namespace ui
{

/** Maps the AppKit (Carbon) virtual keycode (NSEvent.keyCode) to the
 *  dui VirtualKeyCode (Windows VK layout). Unmapped keys return kVK_None.
 */
VirtualKeyCode GetVirtualKeyCodeFromNativeKeyCode(uint16_t nativeKeyCode)
{
    switch (nativeKeyCode) {
    //--- Letters (US QWERTY layout) ---
    case 0x00: return kVK_A;
    case 0x01: return kVK_S;
    case 0x02: return kVK_D;
    case 0x03: return kVK_F;
    case 0x04: return kVK_H;
    case 0x05: return kVK_G;
    case 0x06: return kVK_Z;
    case 0x07: return kVK_X;
    case 0x08: return kVK_C;
    case 0x09: return kVK_V;
    case 0x0B: return kVK_B;
    case 0x0C: return kVK_Q;
    case 0x0D: return kVK_W;
    case 0x0E: return kVK_E;
    case 0x0F: return kVK_R;
    case 0x10: return kVK_Y;
    case 0x11: return kVK_T;
    case 0x1F: return kVK_O;
    case 0x20: return kVK_U;
    case 0x22: return kVK_I;
    case 0x23: return kVK_P;
    case 0x25: return kVK_L;
    case 0x26: return kVK_J;
    case 0x28: return kVK_K;
    case 0x2D: return kVK_N;
    case 0x2E: return kVK_M;

    //--- Digits ---
    case 0x12: return kVK_1;
    case 0x13: return kVK_2;
    case 0x14: return kVK_3;
    case 0x15: return kVK_4;
    case 0x16: return kVK_6;
    case 0x17: return kVK_5;
    case 0x19: return kVK_9;
    case 0x1A: return kVK_7;
    case 0x1C: return kVK_8;
    case 0x1D: return kVK_0;

    //--- Punctuation (US layout) ---
    case 0x18: return kVK_OEM_PLUS;       // =
    case 0x1B: return kVK_OEM_MINUS;      // -
    case 0x1E: return kVK_OEM_6;          // ]
    case 0x21: return kVK_OEM_4;          // [
    case 0x27: return kVK_OEM_7;          // '
    case 0x29: return kVK_OEM_1;          // ;
    case 0x2A: return kVK_OEM_5;          // \
    case 0x2B: return kVK_OEM_COMMA;      // ,
    case 0x2C: return kVK_OEM_2;          // /
    case 0x2F: return kVK_OEM_PERIOD;     // .
    case 0x32: return kVK_OEM_3;          // `

    //--- Editing / control ---
    case 0x24: return kVK_RETURN;
    case 0x30: return kVK_TAB;
    case 0x31: return kVK_SPACE;
    case 0x33: return kVK_BACK;           // Backspace (Delete)
    case 0x35: return kVK_ESCAPE;
    case 0x75: return kVK_DELETE;         // Forward Delete

    //--- Modifiers ---
    case 0x37: return kVK_LWIN;           // Command acts as the dui Win key
    case 0x38: return kVK_SHIFT;
    case 0x3C: return kVK_RSHIFT;
    case 0x39: return kVK_CAPITAL;        // Caps Lock
    case 0x3A: return kVK_MENU;           // Option
    case 0x3D: return kVK_RMENU;
    case 0x3B: return kVK_CONTROL;
    case 0x3E: return kVK_RCONTROL;

    //--- Navigation ---
    case 0x73: return kVK_HOME;
    case 0x74: return kVK_PRIOR;          // Page Up
    case 0x77: return kVK_END;
    case 0x79: return kVK_NEXT;           // Page Down
    case 0x7B: return kVK_LEFT;
    case 0x7C: return kVK_RIGHT;
    case 0x7D: return kVK_DOWN;
    case 0x7E: return kVK_UP;

    //--- Function keys ---
    case 0x7A: return kVK_F1;
    case 0x78: return kVK_F2;
    case 0x63: return kVK_F3;
    case 0x76: return kVK_F4;
    case 0x60: return kVK_F5;
    case 0x61: return kVK_F6;
    case 0x62: return kVK_F7;
    case 0x64: return kVK_F8;
    case 0x65: return kVK_F9;
    case 0x6D: return kVK_F10;
    case 0x67: return kVK_F11;
    case 0x6F: return kVK_F12;
    case 0x69: return kVK_F13;
    case 0x6B: return kVK_F14;
    case 0x71: return kVK_F15;
    case 0x6A: return kVK_F16;

    //--- Keypad ---
    case 0x52: return kVK_NUMPAD0;
    case 0x53: return kVK_NUMPAD1;
    case 0x54: return kVK_NUMPAD2;
    case 0x55: return kVK_NUMPAD3;
    case 0x56: return kVK_NUMPAD4;
    case 0x57: return kVK_NUMPAD5;
    case 0x58: return kVK_NUMPAD6;
    case 0x59: return kVK_NUMPAD7;
    case 0x5B: return kVK_NUMPAD8;
    case 0x5C: return kVK_NUMPAD9;
    case 0x41: return kVK_DECIMAL;        // Keypad .
    case 0x43: return kVK_MULTIPLY;       // Keypad *
    case 0x45: return kVK_ADD;            // Keypad +
    case 0x4B: return kVK_DIVIDE;         // Keypad /
    case 0x4E: return kVK_SUBTRACT;       // Keypad -
    case 0x4C: return kVK_SEPARATOR;      // Keypad Enter

    default:
        return kVK_None;
    }
}

} // namespace ui

#endif // defined(DUI_BUILD_FOR_MACOS)
