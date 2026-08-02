#include "duilib/Utils/BitmapHelper_SDL.h"
#include "duilib/Core/GlobalManager.h"

#if (defined(DUILIB_BUILD_FOR_SDL) || defined(DUILIB_BUILD_FOR_WAYLAND)) && !defined(DUILIB_BUILD_FOR_WIN)

namespace ui {
namespace BitmapHelper {

IRender* CreateRenderObject(IBitmap* pBitmap)
{
    (void)pBitmap;
    return nullptr;
}

} // namespace BitmapHelper
} // namespace ui

#endif

