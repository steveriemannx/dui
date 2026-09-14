#ifndef UI_CONTROL_RICHEDIT_H_
#define UI_CONTROL_RICHEDIT_H_

// This header picks a platform implementation from the DUI_BUILD_FOR_* macros, so it
// has to see them itself. Without this, including "dui/Control/RichEdit.h" first in a
// translation unit (that is, before any other dui header has pulled in dui_config.h)
// silently selected none of the three and left ui::RichEdit undeclared. Latent while
// every in-repo includer happened to include something else first.
#include "dui/dui_config.h"

#if defined (DUI_BUILD_FOR_WIN)
    #include "dui/Control/RichEdit_Windows.h"
#elif defined (DUI_BUILD_FOR_WAYLAND) || defined (DUI_BUILD_FOR_X11)
    #include "dui/Control/RichEdit_Native.h"
#elif defined (DUI_BUILD_FOR_MACOS)
    #include "dui/Control/RichEdit_MacOS.h"
#endif

#endif // UI_CONTROL_RICHEDIT_H_
