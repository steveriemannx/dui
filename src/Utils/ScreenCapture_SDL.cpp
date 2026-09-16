#include "dui/Utils/ScreenCapture.h"

namespace ui
{

std::shared_ptr<IBitmap> ScreenCapture::CaptureBitmap(const Window* /*pWindow*/)
{
    // SDL-only: screen capture not implemented in generic SDL backend
    return nullptr;
}

} // namespace ui
