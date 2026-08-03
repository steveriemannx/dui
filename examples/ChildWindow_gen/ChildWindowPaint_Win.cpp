#include "ChildWindowPaint.h"

#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <vector>

// Define the maximum number of cached bitmaps (adjustable as needed)
#define MAX_CACHE_BITMAP_COUNT 10

// Define the graphics type enumeration
typedef enum {
    SHAPE_RECTANGLE,
    SHAPE_CIRCLE,
    SHAPE_LINE,
    SHAPE_COUNT
} ShapeType;

// Cached bitmap structure
typedef struct {
    HBITMAP hBitmap;      // Cached bitmap handle
    HDC hMemDC;           // Memory DC handle
    HBITMAP hOldBitmap;   // The bitmap handle previously associated with the memory DC
    int width;            // Window width corresponding to the cache
    int height;           // Window height corresponding to the cache
} CacheBitmap;

// Random number initialization flag
static int g_randomInitialized = 0;

struct ChildWindowPaint::TImpl
{
public:
    TImpl();
    ~TImpl();

public:
    /** Draw the window content, normal window (for testing the child window drawing feature)
    */
    void PaintChildWindow(HWND hWnd, const ui::UiRect& rcPaint, bool bPaintFps);

    /** Draw the window content, layered window (for testing the child window drawing feature)
    */
    void PaintChildWindowLayered(HWND hWnd, const ui::UiRect& rcPaint, const ui::UiRect& rcChildWindow, bool bPaintFps);

private:
    /** Create a cached bitmap and draw random patterns
     * @param hdc Reference DC (used to create a compatible bitmap)
     * @param width Window width
     * @param height Window height
     * @param shapeCount Number of shapes
     * @return Returns true on success, false on failure
     */
    bool CreateCacheBitmap(HDC hdc, int width, int height, int shapeCount);

    /** Destroy all cached bitmaps and release GDI resources
    */
    void DestroyCacheBitmaps();

    /** Draw from the cache to the window DC
     * @param hdc Window DC
     * @param width Window width
     * @param height Window height
     * @return Returns true on success, false on failure
     */
    bool DrawFromCache(HDC hdc, int width, int height);

private:
    /** Create the drawing DC and bitmap, etc. (used only when drawing the layered window)
    */
    HDC GetPaintDC(HWND hWnd);

    /** Create a bitmap (used only when drawing the layered window)
    */
    HBITMAP CreateHBitmap(HWND hWnd, int32_t nWidth, int32_t nHeight, bool flipHeight) const;

private:
    // Bitmap cache data
    std::vector<CacheBitmap> m_cacheBitmaps;

private:
    // Drawing DC/bitmap, etc. (used only when drawing the layered window)
    HDC m_hPaintDC;     // Drawing DC
    HGDIOBJ m_hOldObj;  // The object previously associated with the DC
    HBITMAP m_hBitmap;  // Drawing bitmap
    int32_t m_nBitmapWidth;  // Bitmap width
    int32_t m_nBitmapHeight; // Bitmap height
};

ChildWindowPaint::TImpl::TImpl():
    m_hPaintDC(nullptr),
    m_hOldObj(nullptr),
    m_hBitmap(nullptr),
    m_nBitmapWidth(0),
    m_nBitmapHeight(0)
{
}

ChildWindowPaint::TImpl::~TImpl()
{
    DestroyCacheBitmaps();

    if (m_hPaintDC != nullptr) {
        ::SelectObject(m_hPaintDC, m_hOldObj);
        ::DeleteDC(m_hPaintDC);
    }
    if (m_hBitmap != nullptr) {
        ::DeleteObject(m_hBitmap);
        m_hBitmap = nullptr;
    }
}

HDC ChildWindowPaint::TImpl::GetPaintDC(HWND hWnd)
{
    if (!::IsWindow(hWnd)) {
        return nullptr;
    }
    RECT rcClient;
    ::GetClientRect(hWnd, &rcClient);
    int32_t nBitmapWidth = rcClient.right - rcClient.left;
    int32_t nBitmapHeight = rcClient.bottom - rcClient.top;
    if ((nBitmapWidth <= 0) || (nBitmapHeight <= 0)) {
        return nullptr;
    }
    if ((nBitmapWidth != m_nBitmapWidth) || (nBitmapHeight != m_nBitmapHeight)) {
        if (m_hBitmap != nullptr) {
            ::DeleteObject(m_hBitmap);
            m_hBitmap = nullptr;
        }
    }
    if (m_hBitmap == nullptr) {
        m_hBitmap = CreateHBitmap(hWnd, nBitmapWidth, nBitmapHeight, true);
        if (m_hBitmap == nullptr) {
            return nullptr;
        }
        m_nBitmapWidth = nBitmapWidth;
        m_nBitmapHeight = nBitmapHeight;
    }
    if (m_hPaintDC == nullptr) {
        HDC hWndDC = ::GetDC(hWnd);
        m_hPaintDC = ::CreateCompatibleDC(hWndDC);
        ::ReleaseDC(hWnd, hWndDC);
    }
    if (m_hPaintDC == nullptr) {
        return nullptr;
    }
    if (m_hOldObj == nullptr) {
        m_hOldObj = ::SelectObject(m_hPaintDC, m_hBitmap);
    }
    else {
        ::SelectObject(m_hPaintDC, m_hBitmap);
    }
    return m_hPaintDC;
}

HBITMAP ChildWindowPaint::TImpl::CreateHBitmap(HWND hWnd, int32_t nWidth, int32_t nHeight, bool flipHeight) const
{
    ASSERT((nWidth > 0) && (nHeight > 0));
    if (nWidth <= 0 || nHeight <= 0) {
        return nullptr;
    }

    BITMAPINFO bmi;
    ::ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = nWidth;
    if (flipHeight) {
        bmi.bmiHeader.biHeight = -nHeight;// Negative means the bitmap direction is from top to bottom, with the top-left corner as the origin
    }
    else {
        bmi.bmiHeader.biHeight = nHeight; // Positive means the bitmap direction is from bottom to top, with the bottom-left corner as the origin
    }
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = nWidth * nHeight * sizeof(DWORD);

    HBITMAP hBitmap = nullptr;
    HDC hdc = ::GetDC(hWnd);
    ASSERT(hdc != nullptr);
    if (hdc != nullptr) {
        void* pBits = nullptr;
        hBitmap = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    }
    ::ReleaseDC(hWnd, hdc);
    return hBitmap;
}

void ChildWindowPaint::TImpl::DestroyCacheBitmaps()
{
    for (size_t i = 0; i < m_cacheBitmaps.size(); i++) {
        if (m_cacheBitmaps[i].hMemDC) {
            SelectObject(m_cacheBitmaps[i].hMemDC, m_cacheBitmaps[i].hOldBitmap);// Restore the previously associated bitmap
            DeleteDC(m_cacheBitmaps[i].hMemDC);
            m_cacheBitmaps[i].hMemDC = nullptr;
        }
        if (m_cacheBitmaps[i].hBitmap) {
            DeleteObject(m_cacheBitmaps[i].hBitmap);
            m_cacheBitmaps[i].hBitmap = nullptr;
        }
    }
    m_cacheBitmaps.clear();
}

bool ChildWindowPaint::TImpl::CreateCacheBitmap(HDC hdc, int width, int height, int shapeCount)
{
    // Destroy the old cache first
    DestroyCacheBitmaps();

    if (width <= 0 || height <= 0 || shapeCount <= 0) {
        return false;
    }

    // Initialize the random number generator (only once)
    if (!g_randomInitialized) {
        srand((unsigned int)time(NULL));
        g_randomInitialized = 1;
    }

    for (int32_t nIndex = 0; nIndex < MAX_CACHE_BITMAP_COUNT; ++nIndex) {

        // Create a memory DC and a compatible bitmap
        CacheBitmap cache = { 0 };
        cache.hMemDC = CreateCompatibleDC(hdc);
        if (!cache.hMemDC) {
            return false;
        }

        cache.hBitmap = CreateCompatibleBitmap(hdc, width, height);
        if (!cache.hBitmap) {
            DeleteDC(cache.hMemDC);
            return false;
        }

        // Select the bitmap into the memory DC
        cache.hOldBitmap = (HBITMAP)SelectObject(cache.hMemDC, cache.hBitmap);
        cache.width = width;
        cache.height = height;

        // Clear the screen (black background)
        RECT rectClient = { 0, 0, width, height };
        HBRUSH hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(cache.hMemDC, &rectClient, hBlackBrush);
        DeleteObject(hBlackBrush);

        // Draw random shapes to the memory bitmap in a loop
        for (int i = 0; i < shapeCount; i++) {
            // Generate a random color (RGB)
            BYTE r = (BYTE)(rand() % 256);
            BYTE g = (BYTE)(rand() % 256);
            BYTE b = (BYTE)(rand() % 256);
            COLORREF color = RGB(r, g, b);

            // Random line width (1-10 pixels)
            int lineWidth = 1 + rand() % 10;

            // Create a pen (for outlines/lines)
            HPEN hPen = CreatePen(PS_SOLID, lineWidth, color);
            HPEN hOldPen = (HPEN)SelectObject(cache.hMemDC, hPen);

            // Create a brush (for filling)
            HBRUSH hBrush = CreateSolidBrush(color);
            HBRUSH hOldBrush = (HBRUSH)SelectObject(cache.hMemDC, hBrush);

            // Randomly select the shape type
            ShapeType shape = (ShapeType)(rand() % SHAPE_COUNT);

            switch (shape) {
            case SHAPE_RECTANGLE: {
                // Random rectangle position and size
                int x = rand() % width;
                int y = rand() % height;
                int w = 10 + rand() % 100;  // Width 10-109
                int h = 10 + rand() % 100;  // Height 10-109

                // Ensure the shape is within the window
                if (x + w > width) w = width - x;
                if (y + h > height) h = height - y;

                RECT rect = { x, y, x + w, y + h };

                // Randomly choose fill or outline
                if (rand() % 2 == 0) {
                    FillRect(cache.hMemDC, &rect, hBrush);  // Fill the rectangle
                }
                else {
                    Rectangle(cache.hMemDC, rect.left, rect.top, rect.right, rect.bottom); // Outline the rectangle
                }
                break;
            }

            case SHAPE_CIRCLE: {
                // Random circle position and radius (Windows GDI uses an ellipse to simulate a circle)
                int centerX = rand() % width;
                int centerY = rand() % height;
                int radius = 5 + rand() % 50;  // Radius 5-54

                // Ensure the circle is within the window
                if (centerX - radius < 0) centerX = radius;
                if (centerX + radius > width) centerX = width - radius;
                if (centerY - radius < 0) centerY = radius;
                if (centerY + radius > height) centerY = height - radius;

                int left = centerX - radius;
                int top = centerY - radius;
                int right = centerX + radius;
                int bottom = centerY + radius;

                // Randomly choose fill or outline
                if (rand() % 2 == 0) {
                    Ellipse(cache.hMemDC, left, top, right, bottom); // Fill the circle (ellipse)
                }
                else {
                    // Outline the circle: first draw a filled circle, then draw an inner circle with the background color
                    Ellipse(cache.hMemDC, left, top, right, bottom);
                    // Create a background color pen
                    HPEN hBgPen = CreatePen(PS_SOLID, lineWidth, RGB(0, 0, 0));
                    SelectObject(cache.hMemDC, hBgPen);
                    // Draw the inner circle to achieve the outline effect
                    int innerRadius = radius - lineWidth;
                    if (innerRadius > 0) {
                        Ellipse(cache.hMemDC, centerX - innerRadius, centerY - innerRadius,
                            centerX + innerRadius, centerY + innerRadius);
                    }
                    DeleteObject(hBgPen);
                    // Restore the original pen
                    SelectObject(cache.hMemDC, hPen);
                }
                break;
            }

            case SHAPE_LINE: {
                // Random line start and end points
                int x1 = rand() % width;
                int y1 = rand() % height;
                int x2 = rand() % width;
                int y2 = rand() % height;

                MoveToEx(cache.hMemDC, x1, y1, NULL);
                LineTo(cache.hMemDC, x2, y2);  // Draw the line
                break;
            }

            default:
                break;
            }

            // Restore and release GDI objects (to avoid resource leaks)
            SelectObject(cache.hMemDC, hOldPen);
            DeleteObject(hPen);
            SelectObject(cache.hMemDC, hOldBrush);
            DeleteObject(hBrush);
        }

        // Add to the cache list
        m_cacheBitmaps.push_back(cache);
    }
    return true;
}

bool ChildWindowPaint::TImpl::DrawFromCache(HDC hdc, int width, int height)
{
    if (m_cacheBitmaps.empty() || 
        m_cacheBitmaps[0].width != width ||
        m_cacheBitmaps[0].height != height) {
        return false;
    }

    // Copy the cached bitmap directly to the window DC (BitBlt is the fastest drawing method in GDI)
    int32_t nIndex = rand() % (int32_t)m_cacheBitmaps.size();
    BitBlt(hdc, 
           0, 0, width, height, 
           m_cacheBitmaps[nIndex].hMemDC,
           0, 0, 
           SRCCOPY);

    return true;
}

void ChildWindowPaint::TImpl::PaintChildWindow(HWND hWnd, const ui::UiRect& /*rcPaint*/, bool bPaintFps)
{
    // Get the window client area size
    RECT rectClient;
    ::GetClientRect(hWnd, &rectClient);
    int width = rectClient.right - rectClient.left;
    int height = rectClient.bottom - rectClient.top;

    // Start drawing
    PAINTSTRUCT ps = { 0, };
    HDC hPaintDC = ::BeginPaint(hWnd, &ps);
    if (bPaintFps) {// Dynamic drawing
        // Prefer drawing from the cache; rebuild it when invalidated
        if (!DrawFromCache(hPaintDC, width, height)) {
            // Cache does not exist / size mismatch; create a new cache and draw
            CreateCacheBitmap(hPaintDC, width, height, 60);
            // Try drawing from the cache again
            DrawFromCache(hPaintDC, width, height);
        }
    }
    else if (hPaintDC != nullptr) {
        // Static drawing: draw a blue background color
        HBRUSH hBlueBrush = ::CreateSolidBrush(RGB(0, 0, 255));
        if (hBlueBrush != nullptr) {
            ::FillRect(hPaintDC, &rectClient, hBlueBrush);
            ::DeleteObject(hBlueBrush);
        }
    }
    // Normal window
    ::EndPaint(hWnd, &ps);
}

void ChildWindowPaint::TImpl::PaintChildWindowLayered(HWND hWnd, const ui::UiRect& /*rcPaint*/,
                                                      const ui::UiRect& rcChildWindow, bool bPaintFps)
{
    // Get the window client area size
    RECT rcClient;
    ::GetClientRect(hWnd, &rcClient);
    int width = rcClient.right - rcClient.left;
    int height = rcClient.bottom - rcClient.top;

    RECT rectUpdate = { 0, };
    ::GetUpdateRect(hWnd, &rectUpdate, FALSE);

    // Start drawing
    HDC hPaintDC = GetPaintDC(hWnd);
    if (hPaintDC == nullptr) {
        return;
    }

    if (bPaintFps) {// Dynamic drawing
        // Prefer drawing from the cache; rebuild it when invalidated
        if (!DrawFromCache(hPaintDC, width, height)) {
            // Cache does not exist / size mismatch; create a new cache and draw
            CreateCacheBitmap(hPaintDC, width, height, 60);
            // Try drawing from the cache again
            DrawFromCache(hPaintDC, width, height);
        }
    }
    else {
        // Static drawing: draw a blue background color
        HBRUSH hBlueBrush = ::CreateSolidBrush(RGB(0, 0, 255));
        if (hBlueBrush != nullptr) {
            ::FillRect(hPaintDC, &rcClient, hBlueBrush);
            ::DeleteObject(hBlueBrush);
        }
    }

    // Finish drawing
    BYTE nLayeredWindowAlpha = 255;
    POINT pt = { rcChildWindow.left, rcChildWindow.top };
    SIZE szWindow = { rcClient.right - rcClient.left, rcClient.bottom - rcClient.top };
    POINT ptSrc = { 0, 0 };
    //BLENDFUNCTION bf = { AC_SRC_OVER, 0, nLayeredWindowAlpha, AC_SRC_ALPHA };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, nLayeredWindowAlpha, AC_SRC_OVER };
    bool bRet = ::UpdateLayeredWindow(hWnd, nullptr, &pt, &szWindow, hPaintDC, &ptSrc, 0, &bf, ULW_ALPHA) != FALSE;

    // Mark the drawing area as valid
    ::ValidateRect(hWnd, &rectUpdate);
}

ChildWindowPaint::ChildWindowPaint(ui::ChildWindow* pChildWindow) :
    m_pChildWindow(pChildWindow),
    m_impl(nullptr)
{
}

ChildWindowPaint::~ChildWindowPaint()
{
    if (m_impl != nullptr) {
        delete m_impl;
        m_impl = nullptr;
    }
}

void ChildWindowPaint::PaintChildWindow(const ui::UiRect& rcPaint, const ui::NativeMsg& nativeMsg, bool bPaintFps)
{
    ASSERT(nativeMsg.uMsg == WM_PAINT);
    if (nativeMsg.uMsg != WM_PAINT) {
        return;
    }

    HWND hWnd = (HWND)nativeMsg.wParam;
    ASSERT((hWnd != nullptr) && ::IsWindow(hWnd));
    if ((hWnd == nullptr) || !::IsWindow(hWnd)) {
        return;
    }

    RECT rectUpdate = { 0, };
    if (!::GetUpdateRect(hWnd, &rectUpdate, FALSE)) {
        // No drawing needed
        return;
    }

    bool bLayeredWindow = false;
    if (::GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED) {
        // Layered window
        COLORREF crKey = 0;
        BYTE bAlpha = 0;
        DWORD dwFlags = LWA_ALPHA | LWA_COLORKEY;
        // When true is returned, the window must not be drawn as a layered window; it must be drawn in normal window mode
        bool bAttributes = ::GetLayeredWindowAttributes(hWnd, &crKey, &bAlpha, &dwFlags) != FALSE;
        if (!bAttributes) {
            bLayeredWindow = true;
        }
    }
    if (m_impl == nullptr) {
        m_impl = new TImpl;
    }
    if (bLayeredWindow) {
        // Layered window
        ui::UiRect rcChildWindow;
        if (m_pChildWindow != nullptr) {
            m_pChildWindow->GetChildWindowRect(rcChildWindow);
        }        
        m_impl->PaintChildWindowLayered(hWnd, rcPaint, rcChildWindow, bPaintFps);
    }
    else {
        // Normal window
        m_impl->PaintChildWindow(hWnd, rcPaint, bPaintFps);
    }
}

#endif
