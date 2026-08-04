#ifndef UI_CONTROL_RICHEDIT_H_
#define UI_CONTROL_RICHEDIT_H_

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
    #include "dui/Control/RichEdit_Windows.h"
#elif defined (DUI_BUILD_FOR_SDL) || defined (DUI_BUILD_FOR_WAYLAND)
    #include "dui/Control/RichEdit_SDL.h"
#endif

#endif // UI_CONTROL_RICHEDIT_H_
