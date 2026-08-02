#include "duilib/Core/Keyboard.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Core/MessageLoop_Wayland.h"

#ifdef DUILIB_BUILD_FOR_WAYLAND

#include <xkbcommon/xkbcommon.h>

namespace ui
{

// Access xkb state from Input_Wayland.cpp
extern struct xkb_state* GetWaylandXkbState();
extern struct xkb_keymap* GetWaylandXkbKeymap();

bool Keyboard::IsKeyDown(VirtualKeyCode nVirtKey)
{
    // Basic implementation - query xkb state for depressed keys
    (void)nVirtKey;
    return false;
}

bool Keyboard::IsNumLockOn()
{
    struct xkb_state* state = GetWaylandXkbState();
    struct xkb_keymap* keymap = GetWaylandXkbKeymap();
    if (!state || !keymap) return false;
    xkb_mod_index_t mod = xkb_keymap_mod_get_index(keymap, XKB_MOD_NAME_NUM);
    if (mod == XKB_MOD_INVALID) return false;
    return xkb_state_mod_index_is_active(state, mod, XKB_STATE_MODS_LOCKED) != 0;
}

bool Keyboard::IsScrollLockOn()
{
    struct xkb_state* state = GetWaylandXkbState();
    struct xkb_keymap* keymap = GetWaylandXkbKeymap();
    if (!state || !keymap) return false;
    xkb_mod_index_t mod = xkb_keymap_mod_get_index(keymap, "Scroll");
    if (mod == XKB_MOD_INVALID) return false;
    return xkb_state_mod_index_is_active(state, mod, XKB_STATE_MODS_LOCKED) != 0;
}

bool Keyboard::IsCapsLockOn()
{
    struct xkb_state* state = GetWaylandXkbState();
    struct xkb_keymap* keymap = GetWaylandXkbKeymap();
    if (!state || !keymap) return false;
    xkb_mod_index_t mod = xkb_keymap_mod_get_index(keymap, XKB_MOD_NAME_CAPS);
    if (mod == XKB_MOD_INVALID) return false;
    return xkb_state_mod_index_is_active(state, mod, XKB_STATE_MODS_LOCKED) != 0;
}

DString Keyboard::GetKeyName(VirtualKeyCode nVirtKey, bool /*fExtended*/)
{
    (void)nVirtKey;
    return _T("");
}

} // namespace ui

#endif // DUILIB_BUILD_FOR_WAYLAND
