#include "dui/Utils/ScreenCapture.h"
#include "dui/dui_config.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/NativeWindow_MacOS.h"

#ifdef DUI_BUILD_FOR_MACOS
//MacOS
#import <Cocoa/Cocoa.h>

namespace ui
{

// Capture the screen image containing the given window (returns RGBA data with an alpha channel)
// Parameters:
//   targetWindow - NSWindow pointer of the target window (nullptr captures the current screen)
//   bitmap       - output RGBA image data (4 bytes per pixel: R, G, B, A)
//   width        - width of the output image
//   height       - height of the output image
// Return value:
//   true on success, false on failure
bool CaptureScreenBitmap_MacOS(void* targetWindow, std::vector<uint8_t>& bitmap, int32_t& width, int32_t& height) 
{
    return false;
    /** CGDisplayCreateImage is unavailable from macOS 15.0 on; the error asks for ScreenCaptureKit. #239
    @autoreleasepool {
        // 1. Check the screen-recording permission
        if (@available(macOS 10.15, *)) {
            if (!CGPreflightScreenCaptureAccess()) {
                NSLog(@"Please tick this application in System Settings > Security & Privacy > Screen Recording");
                return false;
            }
        }

        // 2. Determine the screen holding the target window
        NSScreen* targetScreen = nil;
        NSWindow* window = static_cast<NSWindow*>(targetWindow);
        if (window) {
            targetScreen = [window screen];
        }
        if (!targetScreen) {
            targetScreen = [NSScreen mainScreen];
        }

        // 3. Get the full screen area (frame instead of visibleFrame)
        NSRect screenRect = targetScreen.frame;  // Full screen area (including the menu bar)
        CGRect cgRect = NSRectToCGRect(screenRect);

        // 4. Get the display ID and capture the full screen
        CGDirectDisplayID displayID = [[targetScreen deviceDescription][@"NSScreenNumber"] unsignedIntValue];
        CGImageRef screenImage = CGDisplayCreateImage(displayID);  // Capture the whole screen
        if (!screenImage) {
            NSLog(@"Unable to capture the screen image; the permission may not have taken effect");
            return false;
        }

        // 5. Crop to the target area (when needed)
        CGImageRef croppedImage = CGImageCreateWithImageInRect(screenImage, cgRect);
        CGImageRelease(screenImage);
        if (!croppedImage) {
            return false;
        }

        // 6. Get the image dimensions
        width = static_cast<int32_t>(CGImageGetWidth(croppedImage));
        height = static_cast<int32_t>(CGImageGetHeight(croppedImage));
        if (width <= 0 || height <= 0) {
            CGImageRelease(croppedImage);
            return false;
        }

        // 7. Create an RGBA context and draw
        CGColorSpaceRef colorSpace = CGColorSpaceCreateWithName(kCGColorSpaceSRGB);
        if (!colorSpace) {
            CGImageRelease(croppedImage);
            return false;
        }

        size_t bytesPerRow = width * 4;
        std::vector<uint8_t> tempBuffer(width * height * 4);
        CGContextRef context = CGBitmapContextCreate(
            tempBuffer.data(), width, height, 8, bytesPerRow,
            colorSpace, kCGImageAlphaPremultipliedLast
        );
        CGColorSpaceRelease(colorSpace);

        if (!context) {
            CGImageRelease(croppedImage);
            return false;
        }

        CGContextDrawImage(context, CGRectMake(0, 0, width, height), croppedImage);
        CGContextRelease(context);
        CGImageRelease(croppedImage);

        // 8. Output the result
        bitmap.swap(tempBuffer);
        return true;
    }
    */
}

std::shared_ptr<IBitmap> ScreenCapture::CaptureBitmap(const Window* pWindow)
{
    if (pWindow == nullptr) {
        return nullptr;
    }
    NSWindow* pNSWindow = (NSWindow*)pWindow->NativeWnd()->GetNSWindow();
    if (pNSWindow == nullptr) {
        return nullptr;
    }
    
    std::vector<uint8_t> bitmap;
    int32_t width = 0;
    int32_t height = 0;
    if (!CaptureScreenBitmap_MacOS(pNSWindow, bitmap, width, height)) {
        return nullptr;
    }
    if ((width > 0) && (height > 0) && ((int32_t)bitmap.size() == (width * height * 4))) {
        std::shared_ptr<IBitmap> spBitmap;
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        ASSERT(pRenderFactory != nullptr);
        if (pRenderFactory != nullptr) {
            spBitmap.reset(pRenderFactory->CreateBitmap());
            if (spBitmap != nullptr) {
                if (!spBitmap->Init(width, height, bitmap.data())) {
                    spBitmap.reset();
                }
            }
        }
        return spBitmap;
    }
    return nullptr;
}

} // namespace ui

#endif //DUI_BUILD_FOR_MACOS
