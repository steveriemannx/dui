#ifndef UI_CORE_KEYBOARD_H_
#define UI_CORE_KEYBOARD_H_

#include "duilib/duilib_defs.h"
#include "duilib/Core/Keycode.h"

namespace ui {

/** Keyboard key modifier flags
*/
enum ModifierKey {
    kNone       = 0,
    kShift      = 1 << 0,   //The Shift key is held down
    kControl    = 1 << 1,   //The Control key is held down
    kAlt        = 1 << 2,   //The Alt key is held down
    kWin        = 1 << 3,   //The Win key is held down
    kFirstPress = 1 << 4,   //First press flag (i.e., if the key was not pressed before the message was sent)
    kIsSystemKey= 1 << 5    //The message is one of the following: WM_SYSCHAR/WM_SYSKEYDOWN/WM_SYSKEYUP (Windows systems only)
};

/** Keyboard operations
*/
class DUILIB_API Keyboard
{
public:
    /** Determine whether a virtual key is pressed (suitable for calling while processing window messages)
    */
    static bool IsKeyDown(VirtualKeyCode nVirtKey);

    /** Whether the NUM LOCK key is on (suitable for calling while processing window messages)
    * @return Returns true if it is on, otherwise off
    */
    static bool IsNumLockOn();

    /** Whether the SCROLL LOCK key is on (suitable for calling while processing window messages)
    */
    static bool IsScrollLockOn();

    /** Whether the CAPS LOCK key is on (suitable for calling while processing window messages)
    */
    static bool IsCapsLockOn();

    /** Get the display name of the key
    * @param [in] nVirtKey The virtual key code
    * @param [in] fExtended Whether it has the extended flag
    */
    static DString GetKeyName(VirtualKeyCode nVirtKey, bool fExtended);
};

} // namespace ui

#endif // UI_CORE_KEYBOARD_H_

