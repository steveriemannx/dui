#include "TestApplication.h"
#include "duilib/Utils/AppEntry.h"

// On macOS the entry point is provided by main_macos.mm (Objective-C++ for CEF)
#if !defined(__APPLE__)
DUILIB_APP_ENTRY_ARGS(TestApplication)
#endif
