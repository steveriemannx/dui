#ifndef UI_UTILS_BITMAP_HELPER_Native_H_
#define UI_UTILS_BITMAP_HELPER_Native_H_

#include "dui/Render/IRender.h"

#if (defined(DUI_BUILD_FOR_WAYLAND) || defined(DUI_BUILD_FOR_X11) || defined(DUI_BUILD_FOR_MACOS)) && !defined(DUI_BUILD_FOR_WIN)

namespace ui 
{
    namespace BitmapHelper 
    {
        /** Create a render object from bitmap data
        */
        IRender* CreateRenderObject(IBitmap* pBitmap);
    }
}

#endif

#endif // UI_UTILS_BITMAP_HELPER_Native_H_
