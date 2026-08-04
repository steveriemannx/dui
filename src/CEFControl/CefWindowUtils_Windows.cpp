#include "dui/CEFControl/CefWindowUtils.h"

#if defined (DUI_BUILD_FOR_WIN) && defined (DUI_BUILD_FOR_CEF)

#include "dui/Core/Window.h"

namespace ui
{
void SetCefWindowPos(CefWindowHandle cefWindow, CefControl* pCefControl)
{
    if ((cefWindow == 0) || (pCefControl == nullptr)) {
        return;
    }
    Window* pWindow = pCefControl->GetWindow();
    if (pWindow == nullptr) {
        return;
    }
    HWND hwnd = (HWND)cefWindow;
    if (::IsWindow(hwnd)) {
        UiRect rc = pCefControl->GetPos();
        pCefControl->Dpi().ClientSizeToWindowSize(rc);
        ::SetWindowPos(hwnd, HWND_TOP, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOZORDER);
    }
}

void SetCefWindowVisible(CefWindowHandle cefWindow, CefControl* pCefControl)
{
    if ((cefWindow == 0) || (pCefControl == nullptr)) {
        return;
    }
    Window* pWindow = pCefControl->GetWindow();
    if (pWindow == nullptr) {
        return;
    }
    HWND hwnd = (HWND)cefWindow;
    if (hwnd) {
        if (pCefControl->IsVisible()) {
            ShowWindow(hwnd, SW_SHOW);
        }
        else {
            ::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
        }
    }
}

void SetCefWindowParent(CefWindowHandle cefWindow, CefControl* pCefControl)
{
    if ((cefWindow == 0) || (pCefControl == nullptr)) {
        return;
    }
    if (!::IsWindow((HWND)cefWindow)) {
        return;
    }
    Window* pWindow = pCefControl->GetWindow();
    if (pWindow == nullptr) {
        return;
    }
    HWND hParent = pWindow->NativeWnd()->GetHWND();
    if (!::IsWindow(hParent)) {
        return;
    }
    ::SetParent((HWND)cefWindow, hParent);

    // Reset the WS_CLIPSIBLINGS and WS_CLIPCHILDREN styles for the new parent window; otherwise the CEF window refresh will have problems
    LONG style = ::GetWindowLong(hParent, GWL_STYLE);
    ::SetWindowLong(hParent, GWL_STYLE, style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
}

bool CaptureCefWindowBitmap(CefWindowHandle cefWindow, std::vector<uint8_t>& bitmap, int32_t& width, int32_t& height)
{
    HWND hwnd = cefWindow;
    if (!::IsWindow(hwnd)) {
        return false;
    }
    // Get the window size
    RECT rect = { 0, 0, 0, 0 };
    if (!GetClientRect(hwnd, &rect)) {
        return false;
    }

    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    if (width <= 0 || height <= 0) {
        return false;
    }

    // Create a device context
    HDC hdcScreen = ::GetDC(nullptr);
    if (hdcScreen == nullptr) {
        return false;
    }
    HDC hdcWindow = ::GetDC(hwnd);
    if (hdcWindow == nullptr) {
        ::ReleaseDC(nullptr, hdcScreen);
        return false;
    }

    HDC hdcMemDC = ::CreateCompatibleDC(hdcWindow);
    if (hdcMemDC == nullptr) {
        ::ReleaseDC(nullptr, hdcScreen);
        ::ReleaseDC(hwnd, hdcWindow);
        return false;
    }

    // Create the bitmap
    HBITMAP hBitmap = ::CreateCompatibleBitmap(hdcWindow, width, height);
    if (hBitmap == nullptr) {
        ::DeleteDC(hdcMemDC);
        ::ReleaseDC(nullptr, hdcScreen);
        ::ReleaseDC(hwnd, hdcWindow);
        return false;
    }

    HGDIOBJ hOldObj = ::SelectObject(hdcMemDC, hBitmap);

    // Copy the screen contents to the bitmap
    ::BitBlt(hdcMemDC, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);

    // Get the bitmap information
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = width;
    bi.biHeight = -height;  // Positive means bottom-to-top, negative means top-to-bottom
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // Allocate memory and get the bitmap data
    bitmap.resize(width * height * 4);
    ::GetDIBits(hdcMemDC, hBitmap, 0, height, bitmap.data(), (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Clean up resources
    ::SelectObject(hdcMemDC, hOldObj);
    ::DeleteObject(hBitmap);
    ::DeleteDC(hdcMemDC);
    ::ReleaseDC(nullptr, hdcScreen);
    ::ReleaseDC(hwnd, hdcWindow);

    return true;
}

void SetCefWindowCursor(CefWindowHandle cefWindow, CefCursorHandle cursor)
{
    if ((cefWindow == nullptr) || (cursor == nullptr)) {
        return;
    }
    ::SetClassLongPtr((HWND)cefWindow, GCLP_HCURSOR, reinterpret_cast<LONG_PTR>(cursor));
    ::SetCursor(cursor);
}

void RemoveCefWindowFromParent(CefWindowHandle cefWindow)
{
    HWND hWnd = (HWND)cefWindow;
    if (::IsWindow(hWnd)) {
        ::SetParent(hWnd, nullptr);
    }
}

} //namespace ui

#endif //DUI_BUILD_FOR_WIN/DUI_BUILD_FOR_CEF
