#include "dui/dui.h"

// Windows, MacOS adn Linux support CEFControl
// FreeBSD does not support CEFControl
// DUI_CEF_COMPILED is set by src/CMakeLists.txt on the same condition that adds the
// CEF sources, and it has to be the switch here too: the CEF distribution is only
// downloaded when CEF is enabled, so including these headers in the default
// (-DDUI_ENABLE_CEF=OFF) build failed on a clean checkout, where
// third_party/libcef has never been fetched.
#if defined(DUI_CEF_COMPILED) && !defined(__FreeBSD__)
    #include "dui/dui_cef.h"
#endif
