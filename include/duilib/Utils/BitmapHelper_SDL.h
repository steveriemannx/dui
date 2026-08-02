#ifndef UI_UTILS_BITMAP_HELPER_SDL_H_
#define UI_UTILS_BITMAP_HELPER_SDL_H_

#include "duilib/Render/IRender.h"

#if (defined(DUILIB_BUILD_FOR_SDL) || defined(DUILIB_BUILD_FOR_WAYLAND)) && !defined(DUILIB_BUILD_FOR_WIN)

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

#endif // UI_UTILS_BITMAP_HELPER_SDL_H_

