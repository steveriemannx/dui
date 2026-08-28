#include "dui/Utils/BitmapHelper_SDL.h"
#include "dui/Core/GlobalManager.h"

#if defined(DUI_BUILD_FOR_MACOS)

namespace ui {
namespace BitmapHelper {

IRender* CreateRenderObject(IBitmap* pBitmap)
{
    if (pBitmap == nullptr) {
        return nullptr;
    }
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory == nullptr) {
        return nullptr;
    }
    IRender* pRender = pRenderFactory->CreateRender(nullptr);
    if (pRender == nullptr) {
        return nullptr;
    }
    const int32_t nWidth = (int32_t)pBitmap->GetWidth();
    const int32_t nHeight = (int32_t)pBitmap->GetHeight();
    if (!pRender->Resize(nWidth, nHeight)) {
        delete pRender;
        return nullptr;
    }
    UiRect rect(0, 0, nWidth, nHeight);
    pRender->DrawImage(rect, pBitmap, rect, UiRect(), rect, UiRect());
    return pRender;
}

} // namespace BitmapHelper
} // namespace ui

#endif //DUI_BUILD_FOR_MACOS
