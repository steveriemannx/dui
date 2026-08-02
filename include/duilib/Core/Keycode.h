#ifndef UI_CORE_KEYCODE_H_
#define UI_CORE_KEYCODE_H_

#include "duilib/duilib_defs.h"

namespace ui {

/*
* WM_KEYUP/DOWN/CHAR HIWORD(lParam) flags, this flag is set when the Win key is pressed
*/
#define kKF_EXTENDED    0x0100

/** Virtual key codes (consistent with the Windows system definitions, WinUser.h)
*/
enum VirtualKeyCode
{
    kVK_None        = 0x00, //No virtual key
    kVK_LBUTTON     = 0x01, //Left mouse button
    kVK_RBUTTON     = 0x02, //Right mouse button
    kVK_CANCEL      = 0x03, //Control-break processing
    kVK_MBUTTON     = 0x04, //Middle mouse button
    kVK_XBUTTON1    = 0x05, //X1 mouse button
    kVK_XBUTTON2    = 0x06, //X2 mouse button
    //- = 0x07, //0x07
    kVK_BACK        = 0x08, //BACKSPACE key
    kVK_TAB         = 0x09, //Tab key
    //- = 0x0A//-0B, //Reserved
    kVK_CLEAR       = 0x0C, //CLEAR key
    kVK_RETURN      = 0x0D, //Enter key
    //- = 0x0E//-0F, //Unassigned
    kVK_SHIFT       = 0x10, //SHIFT key
    kVK_CONTROL     = 0x11, //CTRL key
    kVK_MENU        = 0x12, //Alt key
    kVK_PAUSE       = 0x13, //PAUSE key
    kVK_CAPITAL     = 0x14, //CAPS LOCK key
    kVK_KANA        = 0x15, //IME Kana mode
    kVK_HANGUL      = 0x15, //IME Hanguel mode
    kVK_IME_ON      = 0x16, //IME on
    kVK_JUNJA       = 0x17, //IME Junja mode
    kVK_FINAL       = 0x18, //IME final mode
    kVK_HANJA       = 0x19, //IME Hanja mode
    kVK_KANJI       = 0x19, //IME Kanji mode
    kVK_IME_OFF     = 0x1A, //IME off
    kVK_ESCAPE      = 0x1B, //ESC key
    kVK_CONVERT     = 0x1C, //IME convert
    kVK_NONCONVERT  = 0x1D, //IME non-convert
    kVK_ACCEPT      = 0x1E, //IME accept
    kVK_MODECHANGE  = 0x1F, //IME mode change request
    kVK_SPACE       = 0x20, //Space key
    kVK_PRIOR       = 0x21, //PAGE UP key
    kVK_NEXT        = 0x22, //PAGE DOWN key
    kVK_END         = 0x23, //END key
    kVK_HOME        = 0x24, //HOME key
    kVK_LEFT        = 0x25, //LEFT ARROW key
    kVK_UP          = 0x26, //UP ARROW key
    kVK_RIGHT       = 0x27, //RIGHT ARROW key
    kVK_DOWN        = 0x28, //DOWN ARROW key
    kVK_SELECT      = 0x29, //SELECT key
    kVK_PRINT       = 0x2A, //PRINT key
    kVK_EXECUTE     = 0x2B, //EXECUTE key
    kVK_SNAPSHOT    = 0x2C, //PRINT SCREEN key
    kVK_INSERT      = 0x2D, //INS key
    kVK_DELETE      = 0x2E, //DEL key
    kVK_HELP        = 0x2F, //HELP key
    kVK_0           = 0x30, //0 key
    kVK_1           = 0x31, //1 key
    kVK_2           = 0x32, //2 key
    kVK_3           = 0x33, //3 key
    kVK_4           = 0x34, //4 key
    kVK_5           = 0x35, //5 key
    kVK_6           = 0x36, //6 key
    kVK_7           = 0x37, //7 key
    kVK_8           = 0x38, //8 key
    kVK_9           = 0x39, //9 key
    //- = 0x3A//-40, //Undefined
    kVK_A           = 0x41, //A key
    kVK_B           = 0x42, //B key
    kVK_C           = 0x43, //C key
    kVK_D           = 0x44, //D key
    kVK_E           = 0x45, //E key
    kVK_F           = 0x46, //F key
    kVK_G           = 0x47, //G key
    kVK_H           = 0x48, //H key
    kVK_I           = 0x49, //I key
    kVK_J           = 0x4A, //J key
    kVK_K           = 0x4B, //K key
    kVK_L           = 0x4C, //L key
    kVK_M           = 0x4D, //M key
    kVK_N           = 0x4E, //N key
    kVK_O           = 0x4F, //O key
    kVK_P           = 0x50, //P key
    kVK_Q           = 0x51, //Q key
    kVK_R           = 0x52, //R key
    kVK_S           = 0x53, //S key
    kVK_T           = 0x54, //T key
    kVK_U           = 0x55, //U key
    kVK_V           = 0x56, //V key
    kVK_W           = 0x57, //W key
    kVK_X           = 0x58, //X key
    kVK_Y           = 0x59, //Y key
    kVK_Z           = 0x5A, //Z key
    kVK_LWIN        = 0x5B, //Left Windows key
    kVK_RWIN        = 0x5C, //Right Windows key
    kVK_APPS        = 0x5D, //Application key
    //- = 0x5E, //Reserved
    kVK_SLEEP       = 0x5F, //Computer sleep key
    kVK_NUMPAD0     = 0x60, //Numeric keypad 0 key
    kVK_NUMPAD1     = 0x61, //Numeric keypad 1 key
    kVK_NUMPAD2     = 0x62, //Numeric keypad 2 key
    kVK_NUMPAD3     = 0x63, //Numeric keypad 3 key
    kVK_NUMPAD4     = 0x64, //Numeric keypad 4 key
    kVK_NUMPAD5     = 0x65, //Numeric keypad 5 key
    kVK_NUMPAD6     = 0x66, //Numeric keypad 6 key
    kVK_NUMPAD7     = 0x67, //Numeric keypad 7 key
    kVK_NUMPAD8     = 0x68, //Numeric keypad 8 key
    kVK_NUMPAD9     = 0x69, //Numeric keypad 9 key
    kVK_MULTIPLY    = 0x6A, //Multiply key '*'
    kVK_ADD         = 0x6B, //Add key '+'
    kVK_SEPARATOR   = 0x6C, //Separator key, keypad Enter
    kVK_SUBTRACT    = 0x6D, //Subtract key '-'
    kVK_DECIMAL     = 0x6E, //Period key '.'
    kVK_DIVIDE      = 0x6F, //Divide key '/'
    kVK_F1          = 0x70, //F1 key
    kVK_F2          = 0x71, //F2 key
    kVK_F3          = 0x72, //F3 key
    kVK_F4          = 0x73, //F4 key
    kVK_F5          = 0x74, //F5 key
    kVK_F6          = 0x75, //F6 key
    kVK_F7          = 0x76, //F7 key
    kVK_F8          = 0x77, //F8 key
    kVK_F9          = 0x78, //F9 key
    kVK_F10         = 0x79, //F10 key
    kVK_F11         = 0x7A, //F11 key
    kVK_F12         = 0x7B, //F12 key
    kVK_F13         = 0x7C, //F13 key
    kVK_F14         = 0x7D, //F14 key
    kVK_F15         = 0x7E, //F15 key
    kVK_F16         = 0x7F, //F16 key
    kVK_F17         = 0x80, //F17 key
    kVK_F18         = 0x81, //F18 key
    kVK_F19         = 0x82, //F19 key
    kVK_F20         = 0x83, //F20 key
    kVK_F21         = 0x84, //F21 key
    kVK_F22         = 0x85, //F22 key
    kVK_F23         = 0x86, //F23 key
    kVK_F24         = 0x87, //F24 key
    //- = 0x88//-8F, //Reserved
    kVK_NUMLOCK     = 0x90, //NUM LOCK key
    kVK_SCROLL      = 0x91, //SCROLL LOCK key
    //- = 0x92//-96, //OEM specific
    //- = 0x97//-9F, //Unassigned
    kVK_LSHIFT      = 0xA0, //Left SHIFT key
    kVK_RSHIFT      = 0xA1, //Right SHIFT key
    kVK_LCONTROL    = 0xA2, //Left Ctrl key
    kVK_RCONTROL    = 0xA3, //Right Ctrl key
    kVK_LMENU       = 0xA4, //Left ALT key
    kVK_RMENU       = 0xA5, //Right ALT key
    kVK_BROWSER_BACK        = 0xA6, //Browser back key
    kVK_BROWSER_FORWARD     = 0xA7, //Browser forward key
    kVK_BROWSER_REFRESH     = 0xA8, //Browser refresh key
    kVK_BROWSER_STOP        = 0xA9, //Browser stop key
    kVK_BROWSER_SEARCH      = 0xAA, //Browser search key
    kVK_BROWSER_FAVORITES   = 0xAB, //Browser favorites key
    kVK_BROWSER_HOME        = 0xAC, //Browser start and home key
    kVK_VOLUME_MUTE         = 0xAD, //Mute key
    kVK_VOLUME_DOWN         = 0xAE, //Volume down key
    kVK_VOLUME_UP           = 0xAF, //Volume up key
    kVK_MEDIA_NEXT_TRACK    = 0xB0, //Next track key
    kVK_MEDIA_PREV_TRACK    = 0xB1, //Previous track key
    kVK_MEDIA_STOP          = 0xB2, //Stop media key
    kVK_MEDIA_PLAY_PAUSE    = 0xB3, //Play/pause media key
    kVK_LAUNCH_MAIL         = 0xB4, //Launch mail key
    kVK_LAUNCH_MEDIA_SELECT = 0xB5, //Select media key
    kVK_LAUNCH_APP1         = 0xB6, //Launch application 1 key
    kVK_LAUNCH_APP2         = 0xB7, //Launch application 2 key
    //- = 0xB8//-B9, //Reserved
    kVK_OEM_1               = 0xBA, //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ;: key
    kVK_OEM_PLUS            = 0xBB, //For any country/region, the + key
    kVK_OEM_COMMA           = 0xBC, //For any country/region, the , key
    kVK_OEM_MINUS           = 0xBD, //For any country/region, the - key
    kVK_OEM_PERIOD          = 0xBE, //For any country/region, the . key
    kVK_OEM_2               = 0xBF, //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the /? key
    kVK_OEM_3               = 0xC0, //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the `~ key
    //- = 0xC1//-DA, //Reserved
    kVK_OEM_4               = 0xDB, //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the [{ key
    kVK_OEM_5               = 0xDC, //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \| key
    kVK_OEM_6               = 0xDD, //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ]} key
    kVK_OEM_7               = 0xDE, //Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '" key
    kVK_OEM_8               = 0xDF, //Used for miscellaneous characters; it can vary by keyboard.
    //- = 0xE0, //Reserved
    //- = 0xE1, //OEM specific
    kVK_OEM_102             = 0xE2, //The <> key on the US standard keyboard, or the \| key on the non-US 102-key keyboard
    //- = 0xE3//-E4, //OEM specific
    kVK_PROCESSKEY          = 0xE5, //IME PROCESS key
    //- = 0xE6, //OEM specific
    kVK_PACKET              = 0xE7, //Used to pass Unicode characters as keystrokes. The kVK_PACKET key is the low word of a 32-bit virtual key value used for non-keyboard input methods. For more information, see the comments in KEYBDINPUT, SendInput, WM_KEYDOWN and WM_KEYUP
    //- = 0xE8, //Unassigned
    //- = 0xE9//-F5, //OEM specific
    kVK_ATTN                = 0xF6, //Attn key
    kVK_CRSEL               = 0xF7, //CrSel key
    kVK_EXSEL               = 0xF8, //ExSel key
    kVK_EREOF               = 0xF9, //Erase EOF key
    kVK_PLAY                = 0xFA, //Play key
    kVK_ZOOM                = 0xFB, //Zoom key
    kVK_NONAME              = 0xFC, //Reserved
    kVK_PA1                 = 0xFD, //PA1 key
    kVK_OEM_CLEAR           = 0xFE, //Clear key
};

#ifdef DUILIB_BUILD_FOR_SDL

    //Conversion between VirtualKeyCode and SDL's SDL_Keycode
    typedef uint32_t SDL_Keycode;

    class DUILIB_API Keycode
    {
    public:
        /** Convert SDL_Keycode to ui::VirtualKeyCode (lossy, because SDL has more content than VirtualKeyCode)
        */
        static VirtualKeyCode GetVirtualKeyCode(SDL_Keycode sdlKeycode);

        /** Convert ui::VirtualKeyCode to SDL_Keycode
        */
        static SDL_Keycode GetSDLKeyCode(VirtualKeyCode vkCode);
    };

#endif

} // namespace ui

#endif // UI_CORE_KEYCODE_H_

