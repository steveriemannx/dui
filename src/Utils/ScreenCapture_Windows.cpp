#include "dui/Utils/ScreenCapture.h"

#if defined (DUI_BUILD_FOR_WIN)

#include "dui/Core/GlobalManager.h"
#include "dui/Core/Control.h"

namespace ui
{
/** Create a bitmap
*/
static HBITMAP CreateBitmap(const Window* pWindow, int32_t nWidth, int32_t nHeight, bool flipHeight, LPVOID* pBits)
{
    ASSERT((nWidth > 0) && (nHeight > 0));
    if (nWidth == 0 || nHeight == 0) {
        return nullptr;
    }

    BITMAPINFO bmi;
    ::ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = nWidth;
    if (flipHeight) {
        bmi.bmiHeader.biHeight = -nHeight;//Negative value means the bitmap direction is top-to-bottom with the top-left corner as the origin
    }
    else {
        bmi.bmiHeader.biHeight = nHeight; //Positive value means the bitmap direction is bottom-to-top with the bottom-left corner as the origin
    }
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = nWidth * nHeight * sizeof(DWORD);

    HWND hWnd = (pWindow != nullptr) ? pWindow->NativeWnd()->GetHWND() : nullptr;
    HBITMAP hBitmap = nullptr;
    HDC hdc = ::GetDC(hWnd);
    ASSERT(hdc != nullptr);
    if (hdc != nullptr) {
        hBitmap = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, pBits, nullptr, 0);
    }
    ::ReleaseDC(hWnd, hdc);
    return hBitmap;
}

std::shared_ptr<IBitmap> ScreenCapture::CaptureBitmap(const Window* pWindow)
{
    if (pWindow == nullptr) {
        return nullptr;
    }
    std::shared_ptr<IBitmap> spBitmap;
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory != nullptr) {
        spBitmap.reset(pRenderFactory->CreateBitmap());
    }
    if (spBitmap == nullptr) {
        return nullptr;
    }

    // Capture the screen bitmap
    UiRect rcMonitor;
    pWindow->GetMonitorRect(rcMonitor);
    int32_t xScreen = rcMonitor.left;
    int32_t yScreen = rcMonitor.top;
    int32_t cxScreen = rcMonitor.Width();
    int32_t cyScreen = rcMonitor.Height();
    if ((cxScreen <= 0) || (cyScreen <= 0)) {
        return nullptr;
    }
    HWND hWnd = nullptr; // Get the desktop
    HDC hdcSrc = ::GetDC(hWnd); // Get the screen handle
    if (hdcSrc == nullptr) {
        return nullptr;
    }
    HDC hdcDst = ::CreateCompatibleDC(hdcSrc); // Create a DC compatible with the screen
    if (hdcDst == nullptr) {
        ::ReleaseDC(hWnd, hdcSrc);
        return nullptr;
    }

    LPVOID pBits = nullptr;
    HBITMAP hBitmap = CreateBitmap(pWindow, cxScreen, cyScreen, true, &pBits);
    if (hBitmap == nullptr) {
        ::ReleaseDC(hWnd, hdcSrc); // Release the handle
        ::DeleteDC(hdcDst);
        return nullptr;
    }
    ::SelectObject(hdcDst, hBitmap);
    ::BitBlt(hdcDst, 0, 0, cxScreen, cyScreen, hdcSrc, xScreen, yScreen, SRCCOPY); // Copy the screen content to the bitmap
    ::ReleaseDC(hWnd, hdcSrc); // Release the handle
    ::DeleteDC(hdcDst);

    if (!spBitmap->Init(cxScreen, cyScreen, pBits)) {
        spBitmap.reset();
    }
    ::DeleteObject(hBitmap);
    return spBitmap;
}

} // namespace ui

#endif //DUI_BUILD_FOR_WIN
