// macOS screen capture via ScreenCaptureKit.
//
// CGDisplayCreateImage is unavailable on macOS 15+ (dui issue #239), so this
// uses SCScreenshotManager (macOS 14+). Requires Screen Recording permission;
// when missing, Start() reports an error and the host falls back to the
// test-pattern source.
//
// Multi-display ("extend") mode captures all displays in one image; mirror
// mode captures the main display only.

#import <CoreGraphics/CoreGraphics.h>
#import <Cocoa/Cocoa.h>
#import <ScreenCaptureKit/ScreenCaptureKit.h>
#import <dispatch/dispatch.h>

#include "../Capturer.h"

namespace sdk {

struct Capturer::Impl {
    bool available = false;
    bool started = false;
    std::string error;
    int width = 0;
    int height = 0;
    int originX = 0;
    int originY = 0;
    bool extend = false;
};

Capturer::Capturer() : m_impl(new Impl()) {}

Capturer::~Capturer()
{
    Stop();
}

bool Capturer::PlatformSupported()
{
    // SCScreenshotManager: macOS 14.0+
    if (@available(macOS 14.0, *)) {
        return true;
    }
    return false;
}

bool Capturer::Start(bool extend)
{
    m_impl->extend = extend;
    m_impl->error.clear();

    // preflight the screen-recording permission WITHOUT triggering the system
    // dialog (which would block); unpermitted -> caller falls back to the
    // test-pattern source and the UI shows a hint
    if (@available(macOS 10.15, *)) {
        if (!CGPreflightScreenCaptureAccess()) {
            m_impl->available = false;
            m_impl->error = "screen recording permission not granted";
            return false;
        }
    }

    if (@available(macOS 14.0, *)) {
        // fetch the shareable content. NOTE: when the screen-recording
        // permission is missing, this THROWS an NSException instead of
        // returning an error (macOS 14+ behavior) - catch it so the host can
        // fall back to the test pattern instead of crashing.
        @try {
        NSError* err = nil;
        SCShareableContent* content = [SCShareableContent currentShareableContentWithError:&err];
        if (content == nil || content.displays.count == 0) {
            m_impl->available = false;
            if (err != nil && err.code == 4608 /* SCErrorStreamUnavailable */) {
                m_impl->error = "screen recording permission not granted";
            }
            else {
                m_impl->error = "no capture source";
            }
            return false;
        }

        NSMutableArray<SCDisplay*>* displays = [NSMutableArray array];
        if (extend) {
            for (SCDisplay* d in content.displays) {
                [displays addObject:d];
            }
        }
        else {
            // main display: match NSScreen.mainScreen deviceDescription
            CGDirectDisplayID mainID = (CGDirectDisplayID)[[[NSScreen mainScreen]
                deviceDescription][@"NSScreenNumber"] unsignedIntValue];
            for (SCDisplay* d in content.displays) {
                if (d.displayID == mainID) {
                    [displays addObject:d];
                    break;
                }
            }
            if (displays.count == 0 && content.displays.count > 0) {
                [displays addObject:content.displays[0]];
            }
        }
        if (displays.count == 0) {
            m_impl->available = false;
            m_impl->error = "no capture source";
            return false;
        }

        // union of the selected displays (side-by-side layouts included)
        double minX = 0, minY = 0, maxX = 0, maxY = 0;
        bool first = true;
        for (SCDisplay* d in displays) {
            CGRect f = d.frame;
            if (first) {
                minX = f.origin.x; minY = f.origin.y;
                maxX = f.origin.x + f.size.width;
                maxY = f.origin.y + f.size.height;
                first = false;
            }
            else {
                minX = MIN(minX, f.origin.x);
                minY = MIN(minY, f.origin.y);
                maxX = MAX(maxX, f.origin.x + f.size.width);
                maxY = MAX(maxY, f.origin.y + f.size.height);
            }
        }
        m_impl->width = (int)(maxX - minX);
        m_impl->height = (int)(maxY - minY);
        m_impl->originX = (int)minX;
        m_impl->originY = (int)minY;
        m_impl->available = true;
        m_impl->started = true;
        return true;
        } @catch (NSException* e) {
            m_impl->available = false;
            m_impl->error = "screen recording permission not granted";
            return false;
        }
    }
    m_impl->error = "requires macOS 14+";
    return false;
}

void Capturer::Stop()
{
    m_impl->started = false;
}

bool Capturer::Capture(CaptureFrame& out)
{
    if (!m_impl->available || !m_impl->started) {
        return false;
    }
    __block bool ok = false;
    if (@available(macOS 14.0, *)) {
        @try {
        NSError* err = nil;
        SCShareableContent* content = [SCShareableContent currentShareableContentWithError:&err];
        if (content == nil || content.displays.count == 0) {
            return false;
        }
        NSMutableArray<SCDisplay*>* displays = [NSMutableArray array];
        if (m_impl->extend) {
            for (SCDisplay* d in content.displays) {
                [displays addObject:d];
            }
        }
        else {
            CGDirectDisplayID mainID = (CGDirectDisplayID)[[[NSScreen mainScreen]
                deviceDescription][@"NSScreenNumber"] unsignedIntValue];
            for (SCDisplay* d in content.displays) {
                if (d.displayID == mainID) {
                    [displays addObject:d];
                    break;
                }
            }
            if (displays.count == 0 && content.displays.count > 0) {
                [displays addObject:content.displays[0]];
            }
        }
        if (displays.count == 0) {
            return false;
        }
        SCContentFilter* filter = [[SCContentFilter alloc] initWithDisplays:displays
                                                            excludingWindows:@[]];
        SCStreamConfiguration* conf = [[SCStreamConfiguration alloc] init];
        conf.width = m_impl->width;
        conf.height = m_impl->height;
        conf.showsCursor = NO;
        conf.minimumFrameInterval = CMTimeMake(1, 30);

        dispatch_semaphore_t sem = dispatch_semaphore_create(0);
        __block CGImageRef image = NULL;
        [SCScreenshotManager captureImageWithContentFilter:filter
                                            configuration:conf
                                        completionHandler:^(CGImageRef _Nullable img, NSError* _Nullable err) {
            if (img != NULL) {
                image = CGImageRetain(img);
            }
            dispatch_semaphore_signal(sem);
        }];
        dispatch_semaphore_wait(sem, dispatch_time(DISPATCH_TIME_NOW, 3 * NSEC_PER_SEC));

        if (image == NULL) {
            return false;
        }

        const size_t w = CGImageGetWidth(image);
        const size_t h = CGImageGetHeight(image);
        out.width = (int)w;
        out.height = (int)h;
        out.rgba.resize(w * h * 4);

        CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();
        CGContextRef ctx = CGBitmapContextCreate(out.rgba.data(), w, h, 8, w * 4, cs,
                                                 kCGImageAlphaPremultipliedLast);
        CGColorSpaceRelease(cs);
        if (ctx == NULL) {
            CGImageRelease(image);
            return false;
        }
        // flip: CGImage is bottom-up, we want top-down RGBA
        CGContextTranslateCTM(ctx, 0, (CGFloat)h);
        CGContextScaleCTM(ctx, 1.0, -1.0);
        CGContextDrawImage(ctx, CGRectMake(0, 0, (CGFloat)w, (CGFloat)h), image);
        CGContextRelease(ctx);
        CGImageRelease(image);
        ok = true;
        } @catch (NSException* e) {
            return false;
        }
    }
    return ok;
}

int Capturer::GetWidth() const { return m_impl->width; }
int Capturer::GetHeight() const { return m_impl->height; }
int Capturer::GetOriginX() const { return m_impl->originX; }
int Capturer::GetOriginY() const { return m_impl->originY; }
bool Capturer::IsAvailable() const { return m_impl->available; }
std::string Capturer::GetError() const { return m_impl->error; }

} // namespace sdk
