#ifndef UI_CONTROL_RICHEDIT_H_
#define UI_CONTROL_RICHEDIT_H_

#if defined (DUI_BUILD_FOR_WIN)
    #include "dui/Control/RichEdit_Windows.h"
#elif defined (DUI_BUILD_FOR_WAYLAND) || defined (DUI_BUILD_FOR_X11)
    #include "dui/Control/RichEdit_Native.h"
#elif defined (DUI_BUILD_FOR_MACOS)
    #include "dui/Control/RichEdit_MacOS.h"
#endif

#endif // UI_CONTROL_RICHEDIT_H_
