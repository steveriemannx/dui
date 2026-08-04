#include "dui/dui.h"

// Windows, MacOS adn Linux support CEFControl
// FreeBSD does not support CEFControl
#if !defined(__FreeBSD__)
    #include "dui/dui_cef.h"
#endif
