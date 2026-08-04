#ifndef UI_CORE_SDL_LINUX_H_
#define UI_CORE_SDL_LINUX_H_

#include "dui/dui_defs.h"

#if defined (DUI_BUILD_FOR_LINUX) || defined (DUI_BUILD_FOR_FREEBSD)

namespace ui
{
/** Wrapper for the Linux SetFocus function, functionally similar to Windows' SetFocus(HWND)
*/
bool SetFocus_Linux(uint64_t x11WindowNumber);

}

#endif //DUI_BUILD_FOR_LINUX

#endif //UI_CORE_SDL_LINUX_H_
