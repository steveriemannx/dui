#include "dui/Utils/BitmapHelper_SDL.h"
#include "dui/Core/GlobalManager.h"

#if (defined(DUI_BUILD_FOR_SDL) || defined(DUI_BUILD_FOR_WAYLAND)) && !defined(DUI_BUILD_FOR_WIN)

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

