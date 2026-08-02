#include "duilib/Utils/ScreenCapture_X11.h"
#include "duilib/Core/GlobalManager.h"

#if defined (DUILIB_BUILD_FOR_LINUX) || defined (DUILIB_BUILD_FOR_FREEBSD)
//Linux/FreeBSD OS

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <cassert>
#include <stdexcept>
#include <cstdint>
#include <cstdlib>

namespace ui
{
// Thread-local variable: stores the X11 error state (to avoid multi-thread conflicts)
static thread_local bool s_x11ErrorOccurred = false;

// Custom X11 error handler
static int X11ErrorHandler(Display* display, XErrorEvent* event)
{
    (void)display; // Unused parameter; avoid a compile warning
    (void)event;   // Unused parameter; avoid a compile warning
    
    // Mark that an X11 error occurred
    s_x11ErrorOccurred = true;
    return 0; // Returning 0 means the error was handled, avoiding a program crash
}

// Helper function: count the number of valid bits in the mask (used to determine whether an Alpha channel exists)
static int MaskBitCount(uint32_t mask)
{
    if (mask == 0) {
        return 0;
    }
    return 32 - __builtin_clz(mask); // Position of the highest set bit in the mask (works with GCC/Clang)
}

// Helper function: extract RGBA components from an X11 pixel (compatible with old Xlib without alpha_mask)
static void ExtractRGBA(XImage* ximage, uint32_t pixel, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
{
    r = g = b = 0xFF;
    a = 0xFF; // Default: fully opaque

    if (ximage->depth >= 24) {
        // Extract RGB components dynamically (based on the display's color masks)
        r = (pixel & ximage->red_mask) >> ((ximage->red_mask != 0) ? __builtin_ctz(ximage->red_mask) : 0);
        g = (pixel & ximage->green_mask) >> ((ximage->green_mask != 0) ? __builtin_ctz(ximage->green_mask) : 0);
        b = (pixel & ximage->blue_mask) >> ((ximage->blue_mask != 0) ? __builtin_ctz(ximage->blue_mask) : 0);

        // Handle the Alpha channel (without relying on alpha_mask; inferred from the depth and the total bit count of the RGB masks)
        if (ximage->depth == 32) {
            // Count the total bits of the three RGB masks
            int rgbTotalBits = MaskBitCount(ximage->red_mask) + 
                               MaskBitCount(ximage->green_mask) + 
                               MaskBitCount(ximage->blue_mask);
            
            // If the total RGB bit count < 32, an Alpha channel exists (the remaining bits are Alpha)
            if (rgbTotalBits < 32) {
                // Find the position of the Alpha channel (high bits not occupied by RGB)
                uint32_t alphaMask = ~(ximage->red_mask | ximage->green_mask | ximage->blue_mask) & 0xFFFFFFFF;
                if (alphaMask != 0) {
                    a = (pixel & alphaMask) >> __builtin_ctz(alphaMask);
                }
            }
            // Otherwise (RGB occupies all 32 bits), there is no Alpha channel; keep a=0xFF
        }
        // 24-bit depth: no Alpha channel; keep a=0xFF
    }
}

// Capture the image of the screen where the specified window is located
// Parameters:
//   display      - Connection to the X server
//   targetWindow - X11 window ID of the target window
//   bitmap       - Output RGB image data
//   width        - Width of the output image
//   height       - Height of the output image
// Return value:
//   Returns true on success, false on failure
static bool CaptureScreenBitmap(::Display* display, ::Window targetWindow, std::vector<uint8_t>& bitmap, int32_t& width, int32_t& height)
{
    bitmap.clear();
    width = 0;
    height = 0;

    if (display == nullptr) {
        return false;
    }

    // Save the original error handler and install the custom one
    XErrorHandler originalErrorHandler = XSetErrorHandler(X11ErrorHandler);
    if (originalErrorHandler == nullptr) {
        return false;
    }

    // Reset the error state
    s_x11ErrorOccurred = false;

    bool result = false;
    try {
        // Get the target window attributes
        ::XWindowAttributes attr;
        if (s_x11ErrorOccurred || !::XGetWindowAttributes(display, targetWindow, &attr)) {
            throw std::runtime_error("Failed to get window attributes");
        }

        // Get the screen where the window is located
        ::Screen* screen = attr.screen;
        if (s_x11ErrorOccurred || !screen) {
            throw std::runtime_error("Invalid screen");
        }

        // Get the root window of the screen (the whole screen)
        ::Window rootWindow = RootWindowOfScreen(screen);
        width = static_cast<int32_t>(screen->width);    // Type conversion is safe
        height = static_cast<int32_t>(screen->height);  // Type conversion is safe

        if (width < 1 || height < 1) {
            throw std::runtime_error("Invalid screen size");
        }

        // Capture the screen content (use AllPlanes to ensure all planes are fetched)
        ::XImage* ximage = ::XGetImage(
            display,
            rootWindow,
            0, 0,               // Capture the entire screen
            width, height,      // Size of the capture area
            AllPlanes,          // Capture all color planes
            ZPixmap             // Pixel format (32-bit aligned)
        );

        if (s_x11ErrorOccurred || !ximage) {
            throw std::runtime_error("Failed to get XImage");
        }

        // Manage the XImage resource with RAII (for exception safety)
        struct ImageDestroyer {
            XImage* img;
            ~ImageDestroyer() { 
                if (img) XDestroyImage(img);
            }
        } imgDestroyer{ ximage };

        // Validate the XImage format (must be 24-bit or 32-bit)
        if (ximage->depth != 24 && ximage->depth != 32) {
            throw std::runtime_error("Unsupported image depth");
        }

        // Allocate an RGBA buffer (4 bytes per pixel)
        const size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
        bitmap.resize(pixelCount * 4);

        // Convert XImage data to RGBA format
        for (int32_t y = 0; y < height && !s_x11ErrorOccurred; ++y) {
            for (int32_t x = 0; x < width && !s_x11ErrorOccurred; ++x) {
                // Get the pixel value (XGetPixel has the best compatibility; large images could be optimized by accessing the data buffer directly)
                const uint32_t pixel = XGetPixel(ximage, x, y);
                const size_t index = (static_cast<size_t>(y) * static_cast<size_t>(width) + static_cast<size_t>(x)) * 4;

                uint8_t r, g, b, a;
                ExtractRGBA(ximage, pixel, r, g, b, a);

                bitmap[index] = r;
                bitmap[index + 1] = g;
                bitmap[index + 2] = b;
                bitmap[index + 3] = a;
            }
        }

        // Check whether an error occurred during conversion
        if (s_x11ErrorOccurred) {
            throw std::runtime_error("Error during pixel conversion");
        }

        result = true;
    }
    catch (...) {
        // Catch all exceptions to ensure resources are released properly
        bitmap.clear();
        width = 0;
        height = 0;
        result = false;
    }

    // Restore the original error handler
    XSetErrorHandler(originalErrorHandler);
    // Reset the error state
    s_x11ErrorOccurred = false;

    return result;
}

std::shared_ptr<IBitmap> ScreenCapture_X11::CaptureBitmap(const ui::Window* pWindow)
{
    if (pWindow == nullptr) {
        return nullptr;
    }

    // Get the native window pointer
    const NativeWindow* pNativeWnd = pWindow->NativeWnd();
    if (pNativeWnd == nullptr) {
        return nullptr;
    }

    // Open a connection to the X server (using the DISPLAY environment variable)
    ::Display* display = ::XOpenDisplay(nullptr);
    if (!display) {
        return nullptr;
    }

    // RAII: close the X connection automatically
    struct DisplayCloser {
        Display* d;
        ~DisplayCloser() { 
            if (d) ::XCloseDisplay(d);
        }
    } displayCloser{ display };

    // Get the X11 window ID of the target window (check whether it is an invalid window)
    const ::Window targetWindow = pNativeWnd->GetX11WindowNumber();
    if (targetWindow == BadWindow) {
        return nullptr;
    }

    std::vector<uint8_t> bitmapData;
    int32_t width = 0;
    int32_t height = 0;

    // Capture the screen image (X11 errors are handled internally)
    if (!CaptureScreenBitmap(display, targetWindow, bitmapData, width, height)) {
        return nullptr;
    }

    // Validate the captured data
    const size_t expectedSize = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
    if (width <= 0 || height <= 0 || bitmapData.size() != expectedSize) {
        return nullptr;
    }

    // Create an IBitmap object
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory == nullptr) {
        return nullptr;
    }

    std::shared_ptr<IBitmap> spBitmap(pRenderFactory->CreateBitmap());
    if (spBitmap == nullptr) {
        return nullptr;
    }

    // Initialize the bitmap
    if (!spBitmap->Init(width, height, bitmapData.data())) {
        return nullptr;
    }

    return spBitmap;
}

} // namespace ui

#endif //defined (DUILIB_BUILD_FOR_LINUX) || defined (DUILIB_BUILD_FOR_FREEBSD)
