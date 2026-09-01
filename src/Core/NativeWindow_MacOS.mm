#include "dui/Core/NativeWindow_MacOS.h"
#include "dui/Core/MessageLoop_MacOS.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/WindowManager.h"
#include "dui/Core/WindowBase.h"
#include "dui/Core/DpiManager.h"
#include "dui/Core/WindowMessage.h"
#include "dui/Core/Control.h"
#include "dui/Control/Menu.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/PerformanceUtil.h"

#include <mutex>

#if defined(DUI_BUILD_FOR_MACOS)

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

/** Actively trigger the window's Hover message (internal user message)
*/
#define WM_USER_HOVER_MSG (kWM_USER + 4)

/** Deferred window-finalize message (posted from OnNativeWindowWillClose)
*/
#define WM_USER_FINALIZE_MSG (kWM_USER + 5)

namespace ui {
class NativeWindow_MacOS;

bool ModifyNsWindowShadowType(void* pNSWindow, NativeWindowShadowType nativeShadowType,
                              bool bPopupWindow = false, bool bUseSystemCaption = false);
void RestoreWindowShadowAfterFullscreen(void* pNSWindow, NativeWindowShadowType nativeShadowType);
} // namespace ui

// ---------------------------------------------------------------------------
// Internal helper: the NSView that hosts the dui rendering and forwards events
// (Objective-C classes must live at global scope, outside the C++ namespace)
// ---------------------------------------------------------------------------
@interface DuINativeView : NSView <NSTextInputClient, NSDraggingDestination>
{
@public
    ui::NativeWindow_MacOS* m_pNativeWindow; // non-retained, owned by the dui window
@private
    NSMutableString* m_markedText;           // IME composition text (may be nil)
}
@end

// Borderless windows (NSWindowStyleMaskBorderless, dui's default) cannot become
// key/main by default, which breaks keyboard focus and click delivery. This
// subclass opts back in, like any custom-chrome window. Child windows opt out
// again (like Windows WS_CHILD they never steal keyboard focus from the owner).
@interface DuIWindow : NSWindow
@property (nonatomic, assign) BOOL duiCannotBecomeKey;
@end

@implementation DuIWindow

- (instancetype)init
{
    self = [super init];
    if (self != nil) {
        _duiCannotBecomeKey = NO;
    }
    return self;
}

- (BOOL)canBecomeKeyWindow
{
    return !self.duiCannotBecomeKey;
}

- (BOOL)canBecomeMainWindow
{
    return !self.duiCannotBecomeKey;
}

//Titled windows have their frame constrained by AppKit (cannot cover the menu
//bar area, height gets clamped), which breaks dui's programmatic fullscreen
//(SetFrame over the whole screen) and its exact restoration afterwards. Return
//the requested rect unchanged - documented AppKit customization point.
- (NSRect)constrainFrameRect:(NSRect)frameRect toScreen:(NSScreen*)screen
{
    UNUSED_VARIABLE(screen);
    return frameRect;
}

@end

@implementation DuINativeView

- (instancetype)initWithFrame:(NSRect)frameRect
{
    self = [super initWithFrame:frameRect];
    if (self != nil) {
        m_markedText = [[NSMutableString alloc] init];
        [self registerForDraggedTypes:@[NSPasteboardTypeString, NSPasteboardTypeFileURL, NSFilenamesPboardType]];
    }
    return self;
}

- (BOOL)isFlipped
{
    //Top-left origin, matching dui's coordinate convention
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)acceptsFirstMouse:(NSEvent*)event
{
    //Deliver the first click even when the window is not active yet
    return YES;
}

- (BOOL)isOpaque
{
    // Popup windows use an opaque backing surface. Reporting the view as
    // transparent makes AppKit clear dirty regions before the GL frame arrives,
    // exposing a black intermediate frame.
    return (self.window != nil) ? self.window.opaque : NO;
}

- (void)drawRect:(NSRect)dirtyRect
{
    if (m_pNativeWindow != nullptr) {
        //Convert the AppKit dirty region (view points, flipped coords) to client
        //pixels and merge it into the dui update region, then repaint only that
        //region (dui supports partial paints, which is what keeps CPU rendering
        //fast - full-window repaints on every mouse move caused the lag).
        NSView* view = self;
        CGFloat scale = (view.window != nil) ? view.window.backingScaleFactor : 1.0;
        if (scale < 1.0) {
            scale = 1.0;
        }
        ui::UiRect rc;
        rc.left = (int32_t)(dirtyRect.origin.x * scale);
        rc.top = (int32_t)(dirtyRect.origin.y * scale);
        rc.right = (int32_t)((dirtyRect.origin.x + dirtyRect.size.width) * scale);
        rc.bottom = (int32_t)((dirtyRect.origin.y + dirtyRect.size.height) * scale);
        m_pNativeWindow->OnNativeMergeDirtyRect(rc);
        m_pNativeWindow->PaintWindow(false);
    }
    else {
        [[NSColor clearColor] set];
        NSRectFill(dirtyRect);
    }
}

- (void)mouseDown:(NSEvent*)event        { if (m_pNativeWindow) m_pNativeWindow->OnNativeEvent((void*)event); }
- (void)mouseUp:(NSEvent*)event          { if (m_pNativeWindow) m_pNativeWindow->OnNativeEvent((void*)event); }
- (void)mouseDragged:(NSEvent*)event     { if (m_pNativeWindow) m_pNativeWindow->OnNativeEvent((void*)event); }
- (void)rightMouseDown:(NSEvent*)event   { if (m_pNativeWindow) m_pNativeWindow->OnNativeEvent((void*)event); }
- (void)rightMouseUp:(NSEvent*)event     { if (m_pNativeWindow) m_pNativeWindow->OnNativeEvent((void*)event); }
- (void)rightMouseDragged:(NSEvent*)event{ if (m_pNativeWindow) m_pNativeWindow->OnNativeEvent((void*)event); }
- (void)otherMouseDown:(NSEvent*)event   { if (m_pNativeWindow) m_pNativeWindow->OnNativeEvent((void*)event); }
- (void)otherMouseUp:(NSEvent*)event     { if (m_pNativeWindow) m_pNativeWindow->OnNativeEvent((void*)event); }
- (void)otherMouseDragged:(NSEvent*)event{ if (m_pNativeWindow) m_pNativeWindow->OnNativeEvent((void*)event); }
- (void)scrollWheel:(NSEvent*)event      { if (m_pNativeWindow) m_pNativeWindow->OnNativeEvent((void*)event); }
- (void)mouseMoved:(NSEvent*)event       { if (m_pNativeWindow) m_pNativeWindow->OnNativeEvent((void*)event); }

- (void)keyDown:(NSEvent*)event
{
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnNativeEvent((void*)event);
    }
    //Route through the input system so plain characters reach insertText: and
    //command/keyboard-shortcut combinations go through doCommandBySelector:.
    [self interpretKeyEvents:@[event]];
}

- (void)keyUp:(NSEvent*)event
{
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnNativeEvent((void*)event);
    }
}

//--------------------------------------------------------------------------
// NSTextInputClient
//--------------------------------------------------------------------------
- (BOOL)hasMarkedText
{
    return (m_markedText != nil) && (m_markedText.length > 0);
}

- (NSRange)markedRange
{
    if ([self hasMarkedText]) {
        return NSMakeRange(0, m_markedText.length);
    }
    return NSMakeRange(NSNotFound, 0);
}

- (NSRange)selectedRange
{
    return NSMakeRange(0, 0);
}

- (void)setMarkedText:(id)string selectedRange:(NSRange)selectedRange replacementRange:(NSRange)replacementRange
{
    UNUSED_VARIABLE(selectedRange);
    UNUSED_VARIABLE(replacementRange);
    [m_markedText setString:@""];
    if ([string isKindOfClass:[NSAttributedString class]]) {
        [m_markedText setString:[(NSAttributedString*)string string]];
    }
    else if ([string isKindOfClass:[NSString class]]) {
        [m_markedText setString:(NSString*)string];
    }
    if (m_pNativeWindow != nullptr) {
        const std::wstring text = ui::StringConvert::UTF8ToWString(std::string(m_markedText.UTF8String));
        m_pNativeWindow->OnNativeMarkedText(text);
    }
}

- (void)unmarkText
{
    [m_markedText setString:@""];
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnNativeMarkedText(DStringW());
    }
}

- (NSArray<NSString*>*)validAttributesForMarkedText
{
    return @[];
}

- (NSAttributedString*)attributedSubstringForProposedRange:(NSRange)range actualRange:(NSRangePointer)actualRange
{
    UNUSED_VARIABLE(range);
    if (actualRange != NULL) {
        *actualRange = NSMakeRange(NSNotFound, 0);
    }
    return nil;
}

- (NSUInteger)characterIndexForPoint:(NSPoint)point
{
    UNUSED_VARIABLE(point);
    return 0;
}

- (NSRect)firstRectForCharacterRange:(NSRange)range actualRange:(NSRangePointer)actualRange
{
    if (actualRange != NULL) {
        *actualRange = range;
    }
    NSWindow* window = self.window;
    if (window == nil) {
        return self.bounds;
    }

    // Use the focused control's stored input rectangle so the IME candidate
    // window follows the caret instead of appearing at the window corner.
    if (m_pNativeWindow != nullptr) {
        ui::UiRect inputRect;
        if (m_pNativeWindow->GetTextInputArea(inputRect)) {
            const int32_t nCursor = m_pNativeWindow->GetTextInputCursorOffset();
            ui::UiPoint pt;
            pt.x = inputRect.left + nCursor;
            // firstRectForCharacterRange describes the insertion rectangle.
            // Use its bottom edge so the candidate window is placed below the
            // caret rather than at the top of the control.
            pt.y = inputRect.bottom;
            m_pNativeWindow->ClientToScreen(pt); // dui screen coords: top-left origin

            const NSRect screenFrame = [window screen].frame;
            const CGFloat width = 2.0;
            // Use a very small height: the IME candidate window is positioned
            // relative to this rect, so a large rect pushes it too far from the
            // text. A 1pt caret-like rect keeps it just below the input.
            const CGFloat height = 1.0;
            const CGFloat x = (CGFloat)pt.x;
            // Convert top-left screen coordinates to AppKit's bottom-left origin.
            const CGFloat y = (CGFloat)(screenFrame.origin.y + screenFrame.size.height - pt.y) - height;
            return NSMakeRect(x, y, width, height);
        }
    }

    // Fallback: report the window content rect so the IME still appears near
    // the window when no input rectangle has been set yet.
    return [window contentRectForFrameRect:window.frame];
}

- (NSAttributedString*)attributedString
{
    return [[NSAttributedString alloc] initWithString:m_markedText ? m_markedText : @""];
}

- (void)insertText:(id)string replacementRange:(NSRange)replacementRange
{
    UNUSED_VARIABLE(replacementRange);
    [m_markedText setString:@""];
    if (m_pNativeWindow == nullptr) {
        return;
    }
    if ([string isKindOfClass:[NSAttributedString class]]) {
        string = [(NSAttributedString*)string string];
    }
    if (![string isKindOfClass:[NSString class]]) {
        return;
    }
    const std::wstring text = ui::StringConvert::UTF8ToWString(std::string([(NSString*)string UTF8String]));
    m_pNativeWindow->OnNativeInsertText(text);
}

- (void)doCommandBySelector:(SEL)selector
{
    UNUSED_VARIABLE(selector);
    //No built-in editing commands; the dui layer handles shortcuts via keyDown
}

//--------------------------------------------------------------------------
// NSDraggingDestination
//--------------------------------------------------------------------------
- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender
{
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnDropBegin();
    }
    return NSDragOperationCopy;
}

- (NSDragOperation)draggingUpdated:(id<NSDraggingInfo>)sender
{
    if (m_pNativeWindow != nullptr) {
        const ui::UiPoint pt = [self duiPointFromDragInfo:sender];
        bool bHandled = false;
        m_pNativeWindow->OnDropPosition(pt, bHandled);
    }
    return NSDragOperationCopy;
}

- (void)draggingExited:(id<NSDraggingInfo>)sender
{
    UNUSED_VARIABLE(sender);
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnDropLeave();
    }
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender
{
    if (m_pNativeWindow == nullptr) {
        return NO;
    }
    const ui::UiPoint pt = [self duiPointFromDragInfo:sender];

    //Files first, then plain text
    NSArray<NSURL*>* fileURLs = [sender.draggingPasteboard readObjectsForClasses:@[[NSURL class]] options:nil];
    if (fileURLs.count > 0) {
        std::vector<DString> fileList;
        for (NSURL* url in fileURLs) {
            if (url.path != nil) {
                fileList.push_back(ui::StringConvert::UTF8ToT(std::string(url.path.UTF8String)));
            }
        }
        if (!fileList.empty()) {
            bool bHandled = false;
            m_pNativeWindow->OnDropFiles(DString(), fileList, pt, bHandled);
            return YES;
        }
    }

    NSString* text = [sender.draggingPasteboard stringForType:NSPasteboardTypeString];
    if (text != nil) {
        std::vector<DString> textList;
        //One element per line, matching the dui/SDL behavior
        NSArray<NSString*>* lines = [text componentsSeparatedByString:@"\n"];
        for (NSString* line in lines) {
            textList.push_back(ui::StringConvert::UTF8ToT(std::string(line.UTF8String)));
        }
        bool bHandled = false;
        m_pNativeWindow->OnDropTexts(textList, pt, bHandled);
        return YES;
    }
    return NO;
}

- (BOOL)wantsPeriodicDraggingUpdates
{
    return NO;
}

//Converts the drop location (window coordinates, bottom-left) to dui client coords (top-left)
- (ui::UiPoint)duiPointFromDragInfo:(id<NSDraggingInfo>)sender
{
    const NSPoint p = [self convertPoint:[sender draggingLocation] fromView:nil];
    ui::UiPoint pt;
    pt.x = (int32_t)p.x;
    pt.y = (int32_t)p.y;
    return pt;
}

@end

// ---------------------------------------------------------------------------
// Internal helper: NSWindow delegate forwarding lifecycle events to the C++ side
// ---------------------------------------------------------------------------
@interface DuIWindowDelegate : NSObject <NSWindowDelegate>
{
@public
    ui::NativeWindow_MacOS* m_pNativeWindow; // non-retained
}
@end

@implementation DuIWindowDelegate

- (void)windowWillMove:(NSNotification*)notification
{
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnNativeWindowWillMove();
    }
}

- (void)windowDidResize:(NSNotification*)notification
{
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnNativeWindowDidResize();
    }
}

- (void)windowDidMove:(NSNotification*)notification
{
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnNativeWindowDidMove();
    }
}

- (void)windowDidBecomeKey:(NSNotification*)notification
{
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnNativeWindowBecomeKey();
    }
}

- (void)windowDidResignKey:(NSNotification*)notification
{
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnNativeWindowResignKey();
    }
}

- (void)windowWillClose:(NSNotification*)notification
{
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnNativeWindowWillClose();
    }
}

- (void)windowDidEnterFullScreen:(NSNotification*)notification
{
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnNativeWindowDidEnterFullscreen();
    }
}

- (void)windowDidExitFullScreen:(NSNotification*)notification
{
    if (m_pNativeWindow != nullptr) {
        m_pNativeWindow->OnNativeWindowDidExitFullscreen();
    }
}

- (BOOL)windowShouldClose:(id)sender
{
    //Let the dui layer decide (CloseWnd sets m_bCloseing before [window close])
    return YES;
}

@end

// ---------------------------------------------------------------------------
// Window hit-testing (mirrors the SDL hit-test in NativeWindow_SDL.cpp):
// returns where a client point falls - resize borders, draggable caption, or
// the normal client area. Used to drive title-bar dragging and edge resizing
// on borderless dui windows.
// ---------------------------------------------------------------------------
namespace ui {
namespace {

enum MacHitTestResult
{
    kMacHitTestNormal = 0,
    kMacHitTestDraggable,          // In the title bar (not over a control)
    kMacHitTestResizeTopLeft,
    kMacHitTestResizeTop,
    kMacHitTestResizeTopRight,
    kMacHitTestResizeLeft,
    kMacHitTestResizeRight,
    kMacHitTestResizeBottomLeft,
    kMacHitTestResizeBottom,
    kMacHitTestResizeBottomRight,
};

MacHitTestResult MacWindowHitTest(NativeWindow_MacOS* pNativeWindow, const UiPoint& ptClient)
{
    INativeWindow* pOwner = pNativeWindow->GetOwner();
    if ((pOwner == nullptr) || pNativeWindow->IsUseSystemCaption() || pNativeWindow->IsWindowFullscreen()) {
        return kMacHitTestNormal;
    }

    UiRect rcClient;
    pNativeWindow->GetClientRect(rcClient);

    //Client area, excluding the shadow area
    UiPadding rcCorner;
    pOwner->OnNativeGetShadowCorner(rcCorner);
    rcClient.Deflate(rcCorner);

    if (!pNativeWindow->IsWindowMaximized()) {
        //Not maximized: check the resize borders
        UiRect rcSizeBox = pOwner->OnNativeGetSizeBox();
        if (ptClient.y < rcClient.top + rcSizeBox.top) {
            if (ptClient.y >= rcClient.top) {
                if (ptClient.x < (rcClient.left + rcSizeBox.left) && ptClient.x >= rcClient.left) {
                    return kMacHitTestResizeTopLeft;
                }
                else if (ptClient.x > (rcClient.right - rcSizeBox.right) && ptClient.x <= rcClient.right) {
                    return kMacHitTestResizeTopRight;
                }
                return kMacHitTestResizeTop;
            }
            return kMacHitTestNormal;
        }
        else if (ptClient.y > rcClient.bottom - rcSizeBox.bottom) {
            if (ptClient.y <= rcClient.bottom) {
                if (ptClient.x < (rcClient.left + rcSizeBox.left) && ptClient.x >= rcClient.left) {
                    return kMacHitTestResizeBottomLeft;
                }
                else if (ptClient.x > (rcClient.right - rcSizeBox.right) && ptClient.x <= rcClient.right) {
                    return kMacHitTestResizeBottomRight;
                }
                return kMacHitTestResizeBottom;
            }
            return kMacHitTestNormal;
        }
        if (ptClient.x < rcClient.left + rcSizeBox.left) {
            if (ptClient.x >= rcClient.left) {
                return kMacHitTestResizeLeft;
            }
            return kMacHitTestNormal;
        }
        if (ptClient.x > rcClient.right - rcSizeBox.right) {
            if (ptClient.x <= rcClient.right) {
                return kMacHitTestResizeRight;
            }
            return kMacHitTestNormal;
        }
    }

    //The title bar area (caption rect is relative to the client rect)
    UiRect rcCaption;
    pOwner->OnNativeGetCaptionRect(rcCaption);
    UiRect rcCaptionRect;
    rcCaptionRect.left = rcClient.left + rcCaption.left;
    rcCaptionRect.right = rcClient.right - rcCaption.right;
    rcCaptionRect.top = rcClient.top + rcCaption.top;
    rcCaptionRect.bottom = rcClient.top + rcCaption.bottom;
    if (rcCaptionRect.ContainsPt(ptClient)) {
        if (pOwner->OnNativeIsPtInCaptionBarControl(ptClient)) {
            return kMacHitTestNormal;    //Controls on the title bar are the client area
        }
        return kMacHitTestDraggable;
    }
    return kMacHitTestNormal;
}

/** Edge-resize drag: track the mouse until the button is released, applying the
 *  new frame as the pointer moves. macOS has no built-in borderless resize.
 */
void MacBeginResizeDrag(NativeWindow_MacOS* pNativeWindow, MacHitTestResult hitTest, NSEvent* /*beginEvent*/)
{
    NSWindow* window = (__bridge NSWindow*)pNativeWindow->GetNSWindow();
    if (window == nil) {
        return;
    }
    NSRect startFrame = window.frame;
    const NSPoint startMouse = [NSEvent mouseLocation]; //screen coords, bottom-left origin

    const bool bLeft = (hitTest == kMacHitTestResizeLeft) || (hitTest == kMacHitTestResizeTopLeft) || (hitTest == kMacHitTestResizeBottomLeft);
    const bool bRight = (hitTest == kMacHitTestResizeRight) || (hitTest == kMacHitTestResizeTopRight) || (hitTest == kMacHitTestResizeBottomRight);
    const bool bTop = (hitTest == kMacHitTestResizeTop) || (hitTest == kMacHitTestResizeTopLeft) || (hitTest == kMacHitTestResizeTopRight);
    const bool bBottom = (hitTest == kMacHitTestResizeBottom) || (hitTest == kMacHitTestResizeBottomLeft) || (hitTest == kMacHitTestResizeBottomRight);

    while (true) {
        NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskLeftMouseUp | NSEventMaskLeftMouseDragged
                                            untilDate:[NSDate distantFuture]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event == nil) {
            break;
        }
        if (event.type == NSEventTypeLeftMouseUp) {
            break;
        }
        //The window may be destroyed while the drag is running (e.g. the app is
        //closing); bail out before touching the C++ object.
        if ((pNativeWindow == nullptr) || !pNativeWindow->IsWindow() ||
            (window != (__bridge NSWindow*)pNativeWindow->GetNSWindow())) {
            break;
        }
        const NSPoint currentMouse = [NSEvent mouseLocation];
        const CGFloat dx = currentMouse.x - startMouse.x;
        const CGFloat dy = currentMouse.y - startMouse.y;

        NSRect newFrame = startFrame;
        if (bLeft) {
            newFrame.origin.x = startFrame.origin.x + dx;
            newFrame.size.width = startFrame.size.width - dx;
        }
        if (bRight) {
            newFrame.size.width = startFrame.size.width + dx;
        }
        if (bBottom) {
            newFrame.origin.y = startFrame.origin.y + dy;
            newFrame.size.height = startFrame.size.height - dy;
        }
        if (bTop) {
            newFrame.size.height = startFrame.size.height + dy;
        }

        //Enforce the minimum size
        const UiSize& szMin = pNativeWindow->GetWindowMinimumSize();
        if ((szMin.cx > 0) && (newFrame.size.width < szMin.cx)) {
            newFrame.size.width = szMin.cx;
        }
        if ((szMin.cy > 0) && (newFrame.size.height < szMin.cy)) {
            newFrame.size.height = szMin.cy;
        }
        [window setFrame:newFrame display:YES];
    }
}

} // namespace (anonymous)

} // namespace ui

// ---------------------------------------------------------------------------
// Drawing helper class (same role as NativeWindowRenderPaint in the SDL version)
// ---------------------------------------------------------------------------
namespace ui {

class NativeWindowRenderPaint :
    public IRenderPaint
{
public:
    NativeWindow_MacOS* m_pNativeWindow = nullptr;
    INativeWindow* m_pOwner = nullptr;
    NativeMsg m_nativeMsg;
    bool m_bHandled = false;

public:
    virtual bool DoPaint(const UiRect& rcPaint) override
    {
        if (m_pOwner != nullptr) {
            m_pOwner->OnNativePaintMsg(rcPaint, m_nativeMsg, m_bHandled);
            return true;
        }
        return false;
    }

    virtual uint8_t GetLayeredWindowAlpha() override
    {
        return m_pNativeWindow->GetLayeredWindowAlpha();
    }

    virtual bool GetUpdateRect(UiRect& rcUpdate) const override
    {
        rcUpdate = m_pNativeWindow->GetUpdateRect();
        return !rcUpdate.IsEmpty();
    }
};

// ---------------------------------------------------------------------------
// NativeWindow_MacOS
// ---------------------------------------------------------------------------
NativeWindow_MacOS::NativeWindow_MacOS(INativeWindow* pOwner):
    m_pOwner(pOwner),
    m_nsWindow(nullptr),
    m_nsView(nullptr),
    m_displayTimer(nullptr),
    m_pRender(nullptr),
    m_bChildWindow(false),
    m_pParentNativeWindow(nullptr),
    m_bCloseing(false),
    m_closeParam(kWindowCloseNormal),
    m_bFullscreen(false),
    m_bUseSystemCaption(false),
    m_bMouseCapture(false),
    m_nLayeredWindowAlpha(255),
    m_nLayeredWindowOpacity(255),
    m_bEnableDragDrop(true),
    m_bFakeModal(false),
    m_bDoModal(false),
    m_bIsLayeredWindow(false),
    m_bWindowShown(false),
    m_ptLastMousePos(-1, -1)
{
    ASSERT(m_pOwner != nullptr);
}

NativeWindow_MacOS::~NativeWindow_MacOS()
{
    ClearNativeWindow();
}

void* NativeWindow_MacOS::GetWindowHandle() const
{
    //The render factory receives the NSView* as platform data
    return m_nsView;
}

void* NativeWindow_MacOS::GetNSView() const
{
    return m_nsView;
}

void* NativeWindow_MacOS::GetNSWindow() const
{
    return m_nsWindow;
}

bool NativeWindow_MacOS::IsWindow() const
{
    return m_nsWindow != nullptr;
}

bool NativeWindow_MacOS::IsChildWindow() const
{
    return m_bChildWindow;
}

bool NativeWindow_MacOS::IsPopupWindow() const
{
    return (m_createParam.m_dwStyle & kWS_POPUP) != 0;
}

bool NativeWindow_MacOS::SetParentWindow(NativeWindow_MacOS* pParentWindow)
{
    NSWindow* window = (m_nsWindow != nullptr) ? (__bridge NSWindow*)m_nsWindow : nil;
    if (window == nil) {
        return false;
    }
    if (pParentWindow == nullptr) {
        //Detach from the current parent (keeps the window, top-level)
        NSWindow* oldParent = [window parentWindow];
        if (oldParent != nil) {
            [oldParent removeChildWindow:window];
        }
        m_pParentNativeWindow = nullptr;
        m_bChildWindow = false;
        return true;
    }
    NSWindow* parentNSWindow = (__bridge NSWindow*)pParentWindow->m_nsWindow;
    if ((parentNSWindow == nil) || (parentNSWindow == window)) {
        return false;
    }
    [parentNSWindow addChildWindow:window ordered:NSWindowAbove];
    m_pParentNativeWindow = pParentWindow;
    m_bChildWindow = true;
    return true;
}

DString NativeWindow_MacOS::GetVideoDriverName() const
{
    return DUI_T("cocoa");
}

DString NativeWindow_MacOS::GetWindowRenderName() const
{
    return DUI_T("skia");
}

// ---------------------------------------------------------------------------
// Creation
// ---------------------------------------------------------------------------
bool NativeWindow_MacOS::CreateWnd(NativeWindow_MacOS* pParentWindow,
                                   const WindowCreateParam& createParam,
                                   const WindowCreateAttributes& createAttributes)
{
    UNUSED_VARIABLE(pParentWindow);
    ASSERT(m_nsWindow == nullptr);
    if (m_nsWindow != nullptr) {
        return false;
    }

    //Save the parameters
    m_createParam = createParam;
    m_bUseSystemCaption = createAttributes.m_bUseSystemCaptionDefined &&
                          createAttributes.m_bUseSystemCaption;
    if (m_createParam.m_dwStyle == 0) {
        m_createParam.m_dwStyle = kWS_OVERLAPPEDWINDOW;
    }

    // Honor the initial size from WindowCreateAttributes (set by pure-code and
    // generated-code forms). Without this, macOS would fall back to 800x600
    // instead of the XML-matched size.
    if (createAttributes.m_bInitSizeDefined) {
        if (createAttributes.m_szInitSize.cx > 0) {
            m_createParam.m_nWidth = createAttributes.m_szInitSize.cx;
        }
        if (createAttributes.m_szInitSize.cy > 0) {
            m_createParam.m_nHeight = createAttributes.m_szInitSize.cy;
        }
    }

    if (!CreateWindowAndRender(nullptr, createAttributes)) {
        return false;
    }

    if (m_pOwner != nullptr) {
        bool bHandled = false;
        m_pOwner->OnNativeCreateWndMsg(false, NativeMsg(0, 0, 0), bHandled);
    }
    return true;
}

bool NativeWindow_MacOS::CreateWindowAndRender(NativeWindow_MacOS* pParentWindow, const WindowCreateAttributes& createAttributes)
{
    //The window content size (logical points), dui top-left convention
    int32_t nWidth = m_createParam.m_nWidth;
    int32_t nHeight = m_createParam.m_nHeight;
    if (nWidth <= 0) {
        nWidth = 800;
    }
    if (nHeight <= 0) {
        nHeight = 600;
    }

    NSRect contentRect = NSMakeRect(0, 0, nWidth, nHeight);

    //Child windows are positioned relative to the parent window's client area,
    //not on the screen; they also never take keyboard focus.
    const bool bIsChildWindow = m_bChildWindow && (pParentWindow != nullptr);
    NSView* parentView = nullptr;
    NSWindow* parentNSWindow = nullptr;
    if (bIsChildWindow) {
        parentView = (__bridge NSView*)pParentWindow->m_nsView;
        parentNSWindow = (__bridge NSWindow*)pParentWindow->m_nsWindow;
        if ((parentView == nil) || (parentNSWindow == nil)) {
            return false;
        }
    }

    //Use the final AppKit style from the start. Previously normal dui windows
    //were created borderless and converted to a titled window later when the
    //system shadow was initialized. AppKit recalculates the frame and shadow
    //during that conversion, which looks like a startup zoom.
    const bool bPopupWindow = (m_createParam.m_dwStyle & kWS_POPUP) != 0;
    const bool bUseSystemShadowChrome = !bIsChildWindow &&
                                        !bPopupWindow &&
                                        createAttributes.m_bShadowAttached;
    NSWindowStyleMask styleMask = NSWindowStyleMaskBorderless;
    if (bUseSystemShadowChrome) {
        styleMask = NSWindowStyleMaskTitled |
                    NSWindowStyleMaskFullSizeContentView |
                    NSWindowStyleMaskClosable |
                    NSWindowStyleMaskMiniaturizable |
                    NSWindowStyleMaskResizable;
    }

    DuIWindow* window = [[DuIWindow alloc] initWithContentRect:contentRect
                                                    styleMask:styleMask
                                                      backing:NSBackingStoreBuffered
                                                        defer:NO];
    if (window == nil) {
        return false;
    }
    const bool bIsMenu = dynamic_cast<ui::Menu*>(m_pOwner) != nullptr;
    const bool bIsNoActivate = (m_createParam.m_dwExStyle & kWS_EX_NOACTIVATE) != 0;
    window.duiCannotBecomeKey = (bIsChildWindow || bIsMenu || bIsNoActivate) ? YES : NO;
    [window setOpaque:NO];
    [window setBackgroundColor:[NSColor clearColor]];
    // Keep the window out of the compositor while the framework builds the
    // control tree and changes the native shadow/style. Some AppKit versions
    // briefly composite a newly-created borderless window during that work.
    // Menus and other popup windows are shown directly by SetWindowPos with
    // kSWP_SHOWWINDOW, without a subsequent ShowWindow call to restore alpha.
    // Keep them visible from the start; only normal windows need the hidden
    // pre-render state below.
    [window setAlphaValue:bPopupWindow ? 1.0 : 0.0];
    if (bUseSystemShadowChrome) {
        window.titlebarAppearsTransparent = YES;
        window.titleVisibility = NSWindowTitleHidden;
    }
    // Disable AppKit's default window animation for all dui windows; the
    // borderless->titled shadow conversion plus a deferred first paint would
    // otherwise expose a one-frame transparent/white intermediate.
    window.animationBehavior = NSWindowAnimationBehaviorNone;
    [window setAcceptsMouseMovedEvents:YES];
    [window setIgnoresMouseEvents:NO];
    [window setReleasedWhenClosed:NO];
    if (bIsChildWindow) {
        //Child windows have no chrome of their own: no shadow, no rounded corners
        //(the parent window provides those)
        [window setHasShadow:NO];
    }
    else if (m_createParam.m_dwStyle == kWS_POPUP) {
        // Combo/menu popups must receive their native shadow before the window
        // enters the dui initialization callbacks. Applying it later makes
        // AppKit rebuild the popup surface and causes an intermittent flicker.
        [window setOpaque:YES];
        [window setBackgroundColor:[NSColor whiteColor]];
        [window setHasShadow:YES];
        m_systemShadowType = NativeWindowShadowType::kShadowSystemDefault;
    }

    //Top-left positioning convention (macOS screen origin is bottom-left).
    //The coordinates are only applied when explicitly set (kCW_USEDEFAULT means "use the default position").
    if (bIsChildWindow) {
        m_pParentNativeWindow = pParentWindow;
        //(m_createParam.m_nX/nY) are in the parent client area (top-left origin,
        //points). Convert to screen coordinates for the initial frame placement.
        NSRect baseRect = [parentView convertRect:NSMakeRect((CGFloat)m_createParam.m_nX,
                                                             (CGFloat)m_createParam.m_nY,
                                                             (CGFloat)nWidth,
                                                             (CGFloat)nHeight)
                                           toView:nil];
        NSRect screenRect = [parentNSWindow convertRectToScreen:baseRect];
        [window setFrameOrigin:screenRect.origin];

        //Attach as an ordered child of the owner so the OS keeps it glued above
        //its parent and follows it on moves without extra bookkeeping here.
        [parentNSWindow addChildWindow:window ordered:NSWindowAbove];
    }
    else if (m_createParam.m_bCenterWindow) {
        [window center];
    }
    else if ((m_createParam.m_nX != kCW_USEDEFAULT) && (m_createParam.m_nY != kCW_USEDEFAULT)) {
        [window setFrameTopLeftPoint:NSMakePoint(m_createParam.m_nX, m_createParam.m_nY)];
    }

    //The content view hosting the render
    DuINativeView* view = [[DuINativeView alloc] initWithFrame:contentRect];
    view->m_pNativeWindow = this;
    [window setContentView:view];

    DuIWindowDelegate* delegate = [[DuIWindowDelegate alloc] init];
    delegate->m_pNativeWindow = this;
    window.delegate = delegate;
    //NSWindow.delegate is a weak property; keep a strong reference on the C++ side
    //so windowWillClose/windowDidResize/... keep firing for the whole window lifetime.
    m_delegate = (__bridge_retained void*)delegate;

    //The dui object keeps raw (non-owning) pointers; the ObjC objects are owned
    //by the NSWindow/view hierarchy and released when the window is destroyed.
    m_nsWindow = (__bridge void*)window;
    m_nsView = (__bridge void*)view;

    if (bIsMenu) {
        // Menus use AppKit's shadow, but their borderless content view must be
        // clipped to a native rounded shape from first paint.
        [window setOpaque:NO];
        [window setBackgroundColor:[NSColor clearColor]];
        view.wantsLayer = YES;
        CALayer* layer = view.layer;
        if (layer != nil) {
            layer.cornerRadius = 10.0;
            layer.masksToBounds = YES;
        }
    }

    m_bIsLayeredWindow = createAttributes.m_bIsLayeredWindow;
    if (createAttributes.m_bIsLayeredWindow) {
        [window setOpaque:NO];
        [window setBackgroundColor:[NSColor clearColor]];
    }

    //Coalesce repaints to ~60 Hz. Invalidate() only marks the view dirty; this
    //timer drives the actual display pass, so a burst of invalidates (mouse
    //tracking) results in at most one paint per frame instead of one per event.
    //The timer runs in the common modes so it also fires during tracking loops.
    //__unsafe_unretained is safe here: the timer is always invalidated in
    //ClearNativeWindow BEFORE the view/window are released.
    __unsafe_unretained NSView* weakView = view;
    NSTimer* displayTimer = [NSTimer timerWithTimeInterval:1.0 / 60.0
                                                   repeats:YES
                                                     block:^(NSTimer* /*timer*/) {
        NSView* v = weakView;
        if (v != nil) {
            [v displayIfNeeded];
        }
    }];
    [[NSRunLoop mainRunLoop] addTimer:displayTimer forMode:NSRunLoopCommonModes];
    m_displayTimer = (__bridge_retained void*)displayTimer;

    InitNativeWindow();
    return true;
}

void NativeWindow_MacOS::InitNativeWindow()
{
    if (m_pOwner == nullptr) {
        return;
    }
    const DpiManager& dpi = m_pOwner->OnNativeGetDpi();

    //Synchronize the display scale into the DpiManager
    const float fDisplayScale = GetWindowDisplayScale();
    if (fDisplayScale > 0.0f) {
        uint32_t nScaleFactor = (uint32_t)(fDisplayScale * 100.0f + 0.5f);
        if (nScaleFactor != dpi.GetDisplayScaleFactor()) {
            m_pOwner->OnNativeProcessDisplayScaleChangedMsg(fDisplayScale, fDisplayScale);
        }
    }

    //Apply the system shadow for borderless windows (enables macOS native
    //shadow + rounded corners; the self-drawn traffic lights handle the rest).
    //Child windows never get chrome of their own: no shadow conversion either.
    if (!m_bChildWindow && (m_createParam.m_dwStyle != kWS_POPUP)) {
        SetSystemShadowType(NativeWindowShadowType::kShadowSystemDefault);
    }
}

// ---------------------------------------------------------------------------
// Show / hide / state
// ---------------------------------------------------------------------------
bool NativeWindow_MacOS::ShowWindow(ShowWindowCommands nCmdShow)
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    NSWindow* window = (__bridge NSWindow*)m_nsWindow;
    const bool bWasVisible = [window isVisible];

    switch (nCmdShow) {
    case kSW_HIDE:
        [window orderOut:nil];
        break;
    case kSW_SHOW_MINIMIZED:
        ui::Menu::CloseAllMenus();
        [window miniaturize:nil];
        break;
    case kSW_SHOW_MAXIMIZED:
        [window zoom:nil];
        break;
    case kSW_SHOW_NOACTIVATE:
    case kSW_SHOW_NA:
        //Show without activating (equivalent to Windows SW_SHOWNA): the window
        //is ordered in front but must NOT become key/main. makeKeyAndOrderFront
        //here would steal key focus from the owner, which is fatal for
        //drag-out (the ghost DragWindow would resign the main window's key
        //status -> kill focus -> ClearDragStatus aborts the drag instantly)
        // Pre-render first frame before the window is composited. Borderless
        // windows use clearColor for rounded corners; showing an empty backing
        // store exposes the desktop for one frame.
        if (!bWasVisible && !m_bChildWindow && m_createParam.m_dwStyle != kWS_POPUP) {
            PaintWindow(true);
        }
        [window setAlphaValue:(m_nLayeredWindowAlpha / 255.0f)];
        if (!bWasVisible && !m_bChildWindow && m_createParam.m_dwStyle != kWS_POPUP) {
            //Commit the rendered backing store while the window is still out
            //of the window server's visible ordering.
            [window displayIfNeeded];
        }
        //Non-activating popups must still be above their active host window.
        //orderFront: can leave them behind the host in AppKit's window level
        //ordering, making the popup visible but not clickable.
        if ((m_createParam.m_dwStyle & kWS_POPUP) != 0) {
            [window orderFrontRegardless];
        }
        else {
            [window orderFront:nil];
        }
        m_bWindowShown = true;
        if (!bWasVisible && !m_bChildWindow && m_createParam.m_dwStyle != kWS_POPUP) {
            [window displayIfNeeded];
        }
        break;
    case kSW_MINIMIZE:
        ui::Menu::CloseAllMenus();
        [window miniaturize:nil];
        break;
    case kSW_RESTORE:
        [window deminiaturize:nil];
        if ([window isZoomed]) {
            [window zoom:nil];
        }
        break;
    case kSW_SHOW_NORMAL:
    case kSW_SHOW:
    default:
        // Pre-render first frame before the window is composited.
        if (!bWasVisible && !m_bChildWindow && m_createParam.m_dwStyle != kWS_POPUP) {
            PaintWindow(true);
        }
        [window setAlphaValue:(m_nLayeredWindowAlpha / 255.0f)];
        if (!bWasVisible && !m_bChildWindow && m_createParam.m_dwStyle != kWS_POPUP) {
            //Commit the rendered backing store before making the window visible.
            [window displayIfNeeded];
        }
        if (m_bChildWindow) {
            //Child windows never become the key window (same as Windows WS_CHILD).
            //orderOut detaches AppKit child windows from their parent, so
            //re-attach before showing again (fullscreen hides the whole root).
            if ([window parentWindow] == nil && m_pParentNativeWindow != nullptr) {
                NSWindow* parentNSWindow = (__bridge NSWindow*)m_pParentNativeWindow->m_nsWindow;
                if (parentNSWindow != nil) {
                    [parentNSWindow addChildWindow:window ordered:NSWindowAbove];
                }
            }
            [window orderFront:nil];
        }
        else {
            //Activate the app before ordering the window to the front.
            //SimpleApp shows the window in FrameworkThread::OnInit, which runs
            //before CheckInitMacOS() (called when the message loop starts)
            //activates the application; without this, a window shown while
            //another app is frontmost stays behind it.  Skip popups (menus,
            //combo dropdowns), which must never steal the app's key status.
            if (m_createParam.m_dwStyle != kWS_POPUP) {
                [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
            }
            //makeKeyAndOrderFront (with animationBehavior None above) is the
            //one-step ordering that comes out ABOVE the previous frontmost
            //app's windows (e.g. the Finder window when opening via Finder).
            //orderFrontRegardless alone leaves the window sandwiched below it
            //because the app-activation raise never picks it up.
            [window makeKeyAndOrderFront:nil];
            //Ensure the dui view is the first responder so keyDown/text input reaches it
            if (m_nsView != nullptr) {
                [window makeFirstResponder:(__bridge NSView*)m_nsView];
            }
        }
        m_bWindowShown = true;
        if (!bWasVisible && !m_bChildWindow && m_createParam.m_dwStyle != kWS_POPUP) {
            [window displayIfNeeded];
        }
        //Mirror WM_WINDOWPOSCHANGED: lets listeners (e.g. ui::ChildWindow)
        //sync native child-window positions with the laid-out control rects.
        //Without this a freshly shown borderless window leaves OS child windows
        //stuck at their creation frame until the user moves/resizes the window.
        if (m_pOwner != nullptr) {
            bool bPosHandled = false;
            m_pOwner->OnNativeWindowPosChangedMsg(NativeMsg(0, 0, 0), bPosHandled);
        }
        break;
    }
    return true;
}

bool NativeWindow_MacOS::IsWindowVisible() const
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    return [(__bridge NSWindow*)m_nsWindow isVisible];
}

bool NativeWindow_MacOS::EnableWindow(bool bEnable)
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    NSWindow* window = (__bridge NSWindow*)m_nsWindow;
    if (bEnable) {
        [window setIgnoresMouseEvents:NO];
        [window setAlphaValue:1.0];
    }
    else {
        [window setIgnoresMouseEvents:YES];
        [window setAlphaValue:0.5];
    }
    return true;
}

bool NativeWindow_MacOS::IsWindowEnabled() const
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    return ![(__bridge NSWindow*)m_nsWindow ignoresMouseEvents];
}

bool NativeWindow_MacOS::IsWindowMaximized() const
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    return [(__bridge NSWindow*)m_nsWindow isZoomed];
}

bool NativeWindow_MacOS::IsWindowMinimized() const
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    return [(__bridge NSWindow*)m_nsWindow isMiniaturized];
}

bool NativeWindow_MacOS::IsWindowFullscreen() const
{
    return m_bFullscreen;
}

bool NativeWindow_MacOS::EnterFullscreen()
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    if (m_bChildWindow || [(__bridge NSWindow*)m_nsWindow isMiniaturized]) {
        //Minimized or child windows are not allowed to activate fullscreen
        return false;
    }
    if (m_bFullscreen) {
        return true;
    }
    NSWindow* window = (__bridge NSWindow*)m_nsWindow;

    //Borderless windows cannot reliably use -toggleFullScreen (it requires
    //NSWindowStyleMaskFullScreen, and the Spaces transition does not track the
    //attached child windows dui uses). Cover the screen with this same window
    //instead (SDL parity); the frame change produces a resize notification,
    //which triggers a complete re-layout of the controls.
    GetWindowRect(m_rcBeforeFullscreen);
    m_levelBeforeFullscreen = (int32_t)[window level];
    m_preFullscreenShadowType = m_systemShadowType;
    if (m_preFullscreenShadowType != NativeWindowShadowType::kShadowSystemDisabled) {
        //Hide the rounded corners/hairline while the window covers the screen
        SetSystemShadowType(NativeWindowShadowType::kShadowSystemDisabled);
    }
    if (!IsWindowAlwaysOnTop()) {
        //Above the menu bar so the fullscreen window covers the whole screen
        [window setLevel:(NSMainMenuWindowLevel + 1)];
    }

    NSScreen* screen = [window screen] != nil ? [window screen] : [NSScreen mainScreen];
    [window setFrame:[screen frame] display:YES animate:NO];
    m_bFullscreen = true;

    //WM_WINDOWPOSCHANGED parity (the Windows backend forces one after changing
    //the geometry): lets listeners (e.g. ui::ChildWindow) re-sync immediately
    if (m_pOwner != nullptr) {
        bool bPosHandled = false;
        m_pOwner->OnNativeWindowPosChangedMsg(NativeMsg(0, 0, 0), bPosHandled);
    }

    if (m_pOwner != nullptr) {
        //Synchronous notification (like the SDL implementation): sends
        //kWindowEnterFullscreenMsg and applies the fullscreen margins
        m_pOwner->OnNativeWindowEnterFullscreen();
    }
    return true;
}

bool NativeWindow_MacOS::ExitFullscreen()
{
    if ((m_nsWindow == nullptr) || !m_bFullscreen) {
        return false;
    }
    NSWindow* window = (__bridge NSWindow*)m_nsWindow;
    m_bFullscreen = false;

    NSScreen* screen = [window screen] != nil ? [window screen] : [NSScreen mainScreen];
    const NSRect screenFrame = [screen frame];
    const NSRect frame = NSMakeRect((CGFloat)m_rcBeforeFullscreen.left,
                                    screenFrame.origin.y + screenFrame.size.height -
                                        (CGFloat)m_rcBeforeFullscreen.bottom,
                                    (CGFloat)m_rcBeforeFullscreen.Width(),
                                    (CGFloat)m_rcBeforeFullscreen.Height());
    [window setFrame:frame display:YES animate:NO];

    [window setLevel:(NSInteger)m_levelBeforeFullscreen];
    if ((m_preFullscreenShadowType != NativeWindowShadowType::kShadowSystemDisabled) &&
        (m_systemShadowType == NativeWindowShadowType::kShadowSystemDisabled)) {
        //Restore the rounded corners / OS shadow
        SetSystemShadowType(m_preFullscreenShadowType);
    }

    if (m_pOwner != nullptr) {
        //Synchronous notification (like the SDL implementation): sends
        //kWindowExitFullscreenMsg so listeners refresh their state
        m_pOwner->OnNativeWindowExitFullscreen();
    }

    //Restore child-window positions after Window has restored its old root.
    //Doing this before the exit notification would use the temporary
    //FullscreenBox layout and leave native child windows in the wrong cells.
    if (m_pOwner != nullptr) {
        bool bPosHandled = false;
        m_pOwner->OnNativeWindowPosChangedMsg(NativeMsg(0, 0, 0), bPosHandled);
    }
    return true;
}

// ---------------------------------------------------------------------------
// Focus / foreground
// ---------------------------------------------------------------------------
bool NativeWindow_MacOS::SetWindowForeground()
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    [(__bridge NSWindow*)m_nsWindow makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
    return true;
}

bool NativeWindow_MacOS::IsWindowForeground() const
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    return [(__bridge NSWindow*)m_nsWindow isKeyWindow];
}

bool NativeWindow_MacOS::SetWindowFocus()
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    [(__bridge NSWindow*)m_nsWindow makeKeyWindow];
    return true;
}

bool NativeWindow_MacOS::KillWindowFocus()
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    [NSApp preventWindowOrdering];
    return true;
}

bool NativeWindow_MacOS::IsWindowFocused() const
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    return [(__bridge NSWindow*)m_nsWindow isKeyWindow];
}

void NativeWindow_MacOS::CheckSetWindowFocus()
{
    SetWindowFocus();
}

void NativeWindow_MacOS::SetWindowAlwaysOnTop(bool bOnTop)
{
    if (m_nsWindow == nullptr) {
        return;
    }
    NSWindow* window = (__bridge NSWindow*)m_nsWindow;
    if (bOnTop) {
        [window setLevel:NSFloatingWindowLevel];
    }
    else {
        [window setLevel:NSNormalWindowLevel];
    }
}

bool NativeWindow_MacOS::IsWindowAlwaysOnTop() const
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    return [(__bridge NSWindow*)m_nsWindow level] == NSFloatingWindowLevel;
}

void NativeWindow_MacOS::CenterWindow()
{
    if (m_nsWindow != nullptr) {
        [(__bridge NSWindow*)m_nsWindow center];
    }
}

// ---------------------------------------------------------------------------
// Close / destroy
// ---------------------------------------------------------------------------
void NativeWindow_MacOS::CloseWnd(int32_t nRet)
{
    m_closeParam = nRet;
    m_bCloseing = true;
    if (m_nsWindow != nullptr) {
        NSWindow* window = (__bridge NSWindow*)m_nsWindow;
        // Hide popup windows before entering AppKit's close/deferred-release
        // sequence. Otherwise the native shadow can remain composited for one
        // frame after the popup content has been detached.
        if (m_createParam.m_dwStyle == kWS_POPUP) {
            [window orderOut:nil];
            [window setHasShadow:NO];
        }
        [window close];
    }
}

void NativeWindow_MacOS::Close()
{
    //Close synchronously
    m_bCloseing = true;
    if (m_pOwner == nullptr) {
        return;
    }
    bool bHandled = false;
    m_pOwner->OnNativeWindowCloseMsg(0, NativeMsg(0, 0, 0), bHandled);
    if (m_nsWindow != nullptr) {
        [(__bridge NSWindow*)m_nsWindow close];
    }
}

bool NativeWindow_MacOS::IsClosingWnd() const
{
    return m_bCloseing;
}

int32_t NativeWindow_MacOS::GetCloseParam() const
{
    return m_closeParam;
}

void NativeWindow_MacOS::OnNativeWindowWillClose()
{
    if (m_pOwner == nullptr) {
        return;
    }
    bool bHandled = false;
    m_pOwner->OnNativeWindowCloseMsg(m_closeParam, NativeMsg(0, 0, 0), bHandled);

    //Run the same pre-close cleanup as the Windows/SDL backends before the
    //post-close sequence. This lets applications detach child-window event
    //handlers and close native child windows before the owning control tree is
    //destroyed (important when the window is closed from fullscreen).
    m_pOwner->OnNativePreCloseWindow();

    //The window has been closed: run the framework close sequence
    //(PostCloseWindow -> PostQuitMsg so "quit when last window closes" works,
    //matching the SDL/Windows backends).
    m_pOwner->OnNativePostCloseWindow();

    //Defer the destruction out of the [NSWindow close] call stack: the owner's
    //OnNativeFinalMessage deletes this object, and destroying the NSWindow while
    //-[NSWindow close] is still running would be re-entrant. A user message
    //processed by the main loop finalizes it instead.
    static std::once_flag s_finalizeHandlerFlag;
    std::call_once(s_finalizeHandlerFlag, []() {
        MessageLoop_MacOS::AddUserMessageCallback(WM_USER_FINALIZE_MSG, [](uint32_t /*msgId*/, WPARAM wParam, LPARAM /*lParam*/) {
            NativeWindow_MacOS* pWindow = reinterpret_cast<NativeWindow_MacOS*>(wParam);
            if (pWindow != nullptr) {
                pWindow->OnNativeFinalizeFromMessage();
            }
        });
    });
    MessageLoop_MacOS::PostUserEvent(WM_USER_FINALIZE_MSG, (WPARAM)this, 0);
}

void NativeWindow_MacOS::OnNativeFinalizeFromMessage()
{
    if (m_bFinalized) {
        return;
    }
    m_bFinalized = true;
    OnFinalMessage();
}

void NativeWindow_MacOS::OnNativeWindowDidEnterFullscreen()
{
    m_bFullscreen = true;
}

void NativeWindow_MacOS::OnNativeWindowDidExitFullscreen()
{
    m_bFullscreen = false;
    //After exiting fullscreen the WindowServer re-composites the window; toggle
    //the style mask to force the rounded corners/shadow back (macOS quirk).
    if (m_systemShadowType != NativeWindowShadowType::kShadowSystemDisabled) {
        RestoreWindowShadowAfterFullscreen(m_nsWindow, m_systemShadowType);
    }
}

void NativeWindow_MacOS::OnFinalMessage()
{
    if (m_pOwner != nullptr) {
        m_pOwner->OnNativeFinalMessage();
    }
}

void NativeWindow_MacOS::ClearNativeWindow()
{
    //Stop the repaint timer first so no display pass can race the teardown.
    if (m_displayTimer != nullptr) {
        NSTimer* timer = (__bridge_transfer NSTimer*)m_displayTimer;
        [timer invalidate];
        m_displayTimer = nullptr;
    }

    //Sever the view/delegate back-pointers BEFORE releasing the ObjC objects:
    //leftover AppKit events (e.g. a mouse-move in flight during close) would
    //otherwise call into the freed C++ object -> EXC_BAD_ACCESS.
    if (m_nsView != nullptr) {
        DuINativeView* view = (__bridge DuINativeView*)m_nsView;
        view->m_pNativeWindow = nullptr;
    }
    if (m_delegate != nullptr) {
        DuIWindowDelegate* delegate = (__bridge DuIWindowDelegate*)m_delegate;
        delegate->m_pNativeWindow = nullptr;
    }
    if (m_nsWindow != nullptr) {
        NSWindow* window = (__bridge_transfer NSWindow*)m_nsWindow;
        window.delegate = nil;
        [window orderOut:nil];
        m_nsWindow = nullptr;
    }
    //Release the retained delegate (NSWindow.delegate is weak, so it must be held here)
    if (m_delegate != nullptr) {
        DuIWindowDelegate* delegate = (__bridge_transfer DuIWindowDelegate*)m_delegate;
        delegate = nil;
        m_delegate = nullptr;
    }
    m_nsView = nullptr;
    m_pRender = nullptr;
}

// ---------------------------------------------------------------------------
// Messages
// ---------------------------------------------------------------------------
LRESULT NativeWindow_MacOS::PostMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_USER_HOVER_MSG) {
        //Dispatch the tooltip hover event synchronously with the last known
        //mouse position (macOS has no native WM_MOUSEHOVER equivalent).
        bool bHandled = false;
        m_pOwner->OnNativeMouseHoverMsg(m_ptLastMousePos, 0, NativeMsg(uMsg, wParam, lParam), bHandled);
        return 0;
    }
    //User messages go through the message loop
    if (MessageLoop_MacOS::PostUserEvent(uMsg, wParam, lParam)) {
        return 0;
    }
    return -1;
}

void NativeWindow_MacOS::PostQuitMsg(int32_t nExitCode)
{
    MessageLoop_MacOS::PostQuitMsg(nExitCode);
}

LRESULT NativeWindow_MacOS::CallDefaultWindowProc(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    //No WndProc on macOS
    return 0;
}

// ---------------------------------------------------------------------------
// Painting
// ---------------------------------------------------------------------------
void NativeWindow_MacOS::PaintWindow(bool bPaintAll)
{
    //Re-entrancy guard: dui's paint/layout path calls Invalidate() which may
    //trigger a nested displayIfNeeded; bail out and repaint once more after.
    if (m_bPainting) {
        m_bPendingPaint = true;
        return;
    }
    m_bPainting = true;
    m_bPendingPaint = false;

    PerformanceStat statPerformance(DUI_T("PaintWindow, NativeWindow_MacOS::PaintWindow(Total)"));
    if (bPaintAll) {
        m_rcUpdateRect.Clear();
    }
    INativeWindow* pOwner = m_pOwner;
    ASSERT(pOwner != nullptr);
    if (pOwner == nullptr) {
        return;
    }
    std::weak_ptr<WeakFlag> ownerFlag = pOwner->GetWeakFlag();
    bool bPaint = pOwner->OnNativePreparePaint();
    if (bPaint && !ownerFlag.expired()) {
        IRender* pRender = pOwner->OnNativeGetRender();
        ASSERT(pRender != nullptr);
        if ((pRender != nullptr) && !ownerFlag.expired()) {
            NativeWindowRenderPaint renderPaint;
            renderPaint.m_pNativeWindow = this;
            renderPaint.m_pOwner = pOwner;
            renderPaint.m_nativeMsg = NativeMsg(0, 0, 0);
            renderPaint.m_bHandled = false;
            bPaint = pRender->PaintAndSwapBuffers(&renderPaint);
        }
    }
    m_rcUpdateRect.Clear();
    m_bPainting = false;
    //Something invalidated during the paint (nested Invalidate only marked the
    //view dirty); do one more display pass so the pending region is drawn.
    if (m_bPendingPaint && (m_nsView != nullptr)) {
        [(__bridge NSView*)m_nsView displayIfNeeded];
    }
}

const UiRect& NativeWindow_MacOS::GetUpdateRect() const
{
    return m_rcUpdateRect;
}

void NativeWindow_MacOS::Invalidate(const UiRect& rcItem)
{
    if (m_nsView == nullptr) {
        return;
    }
    NSView* view = (__bridge NSView*)m_nsView;
    //Accumulate the region that needs repainting (dui's partial-paint support).
    //An empty rect means "the whole window" and resets the update region.
    if (rcItem.IsEmpty()) {
        m_rcUpdateRect.Clear();
        [view setNeedsDisplay:YES];
    }
    else {
        if (m_rcUpdateRect.IsEmpty()) {
            m_rcUpdateRect = rcItem;
        }
        else {
            m_rcUpdateRect.Union(rcItem);
        }
        //rcItem is in client PIXELS; setNeedsDisplayInRect: expects view POINTS
        //(a 2x mismatch here marks the wrong region, and AppKit clears it to
        //transparent before drawRect - on this non-opaque window the stale
        //cleared area shows the windows behind as flickering squares).
        CGFloat scale = (view.window != nil) ? view.window.backingScaleFactor : 1.0;
        if (scale < 1.0) {
            scale = 1.0;
        }
        NSRect rc = NSMakeRect(rcItem.left / scale, rcItem.top / scale,
                               rcItem.Width() / scale, rcItem.Height() / scale);
        [view setNeedsDisplayInRect:rc];
    }
    //The actual repaint is driven by the ~60 Hz display timer (see
    //CreateWindowAndRender), so bursts of invalidates coalesce into at most one
    //paint per frame - the synchronous displayIfNeeded used here before
    //saturated the CPU on every mouse move with CPU rendering.
}

void NativeWindow_MacOS::OnNativeMergeDirtyRect(const UiRect& rcDirty)
{
    if (m_rcUpdateRect.IsEmpty()) {
        m_rcUpdateRect = rcDirty;
    }
    else {
        m_rcUpdateRect.Union(rcDirty);
    }
}

bool NativeWindow_MacOS::UpdateWindow() const
{
    if (m_nsView == nullptr) {
        return false;
    }
    [(__bridge NSView*)m_nsView displayIfNeeded];
    return true;
}

// ---------------------------------------------------------------------------
// Rect / position helpers
// ---------------------------------------------------------------------------
void NativeWindow_MacOS::GetClientRect(UiRect& rcClient) const
{
    if (m_nsView == nullptr) {
        rcClient.Clear();
        return;
    }
    NSView* view = (__bridge NSView*)m_nsView;
    NSRect rc = view.bounds;
    //Mirror the SDL implementation: when the dui pixel density is enabled the
    //client rect is reported in pixels (points x backing scale), which is the
    //space dui lays the UI out in (mouse coordinates are converted the same
    //way via DpiManager::WindowSizeToClientSize).
    if (GlobalManager::Instance().Dpi().IsPixelDensityEnabled()) {
        rc = [view convertRectToBacking:rc];
    }
    rcClient.left = 0;
    rcClient.top = 0;
    rcClient.right = (int32_t)rc.size.width;
    rcClient.bottom = (int32_t)rc.size.height;
}

void NativeWindow_MacOS::GetWindowRect(UiRect& rcWindow) const
{
    if (m_nsWindow == nullptr) {
        rcWindow.Clear();
        return;
    }
    NSWindow* window = (__bridge NSWindow*)m_nsWindow;
    NSRect frame = window.frame;
    const NSRect screenFrame = [window screen].frame;
    rcWindow.left = (int32_t)frame.origin.x;
    rcWindow.top = (int32_t)(screenFrame.origin.y + screenFrame.size.height -
                             frame.origin.y - frame.size.height);
    rcWindow.right = rcWindow.left + (int32_t)frame.size.width;
    rcWindow.bottom = rcWindow.top + (int32_t)frame.size.height;
}

void NativeWindow_MacOS::ScreenToClient(UiPoint& pt) const
{
    if (m_nsWindow == nullptr) {
        return;
    }
    NSWindow* window = (__bridge NSWindow*)m_nsWindow;
    NSPoint p = NSMakePoint(pt.x, pt.y);
    //Convert from screen (top-left convention) to window points
    const NSRect screenFrame = [window screen].frame;
    p.y = screenFrame.size.height - p.y;
    NSPoint local = [window convertPointFromScreen:p];
    const NSRect contentRect = [window contentRectForFrameRect:window.frame];
    local.y = contentRect.size.height - local.y;
    pt.x = (int32_t)local.x;
    pt.y = (int32_t)local.y;
}

void NativeWindow_MacOS::ClientToScreen(UiPoint& pt) const
{
    if (m_nsWindow == nullptr) {
        return;
    }
    //dui control geometry is in client pixels; AppKit windows use points.
    if (m_pOwner != nullptr) {
        m_pOwner->OnNativeGetDpi().ClientSizeToWindowSize(pt);
    }
    NSWindow* window = (__bridge NSWindow*)m_nsWindow;
    NSRect contentRect = [window contentRectForFrameRect:window.frame];
    NSPoint local = NSMakePoint(pt.x, contentRect.size.height - pt.y);
    NSPoint screen = [window convertPointToScreen:local];
    const NSRect screenFrame = [window screen].frame;
    pt.x = (int32_t)screen.x;
    pt.y = (int32_t)(screenFrame.origin.y + screenFrame.size.height - screen.y);
}

void NativeWindow_MacOS::GetCursorPos(UiPoint& pt) const
{
    NSPoint p = [NSEvent mouseLocation];
    const NSRect screenFrame = [NSScreen mainScreen].frame;
    pt.x = (int32_t)p.x;
    pt.y = (int32_t)(screenFrame.size.height - p.y);
}

const UiPoint& NativeWindow_MacOS::GetLastMousePos() const
{
    return m_ptLastMousePos;
}

void NativeWindow_MacOS::SetLastMousePos(const UiPoint& pt)
{
    m_ptLastMousePos = pt;
}

bool NativeWindow_MacOS::MoveWindow(int32_t X, int32_t Y, int32_t nWidth, int32_t nHeight, bool /*bRepaint*/)
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    NSWindow* window = (__bridge NSWindow*)m_nsWindow;
    if (m_bChildWindow) {
        //X/Y are in the PARENT's client area (top-left origin, points)
        NSWindow* parentWnd = [window parentWindow];
        if (parentWnd == nil && m_pParentNativeWindow != nullptr) {
            //orderOut can detach macOS child windows from their owner; re-attach
            //before computing screen coordinates / moving.
            NSWindow* parentNSWindow = (__bridge NSWindow*)m_pParentNativeWindow->m_nsWindow;
            if (parentNSWindow != nil) {
                [parentNSWindow addChildWindow:window ordered:NSWindowAbove];
                parentWnd = parentNSWindow;
            }
        }
        if ((parentWnd == nil) || (parentWnd.contentView == nil)) {
            return false;
        }
        NSRect baseRect = [parentWnd.contentView convertRect:NSMakeRect((CGFloat)X, (CGFloat)Y,
                                                                        (CGFloat)nWidth, (CGFloat)nHeight)
                                                      toView:nil];
        NSRect screenRect = [parentWnd convertRectToScreen:baseRect];
        NSRect frame = NSMakeRect(screenRect.origin.x, screenRect.origin.y, (CGFloat)nWidth, (CGFloat)nHeight);
        [window setFrame:frame display:YES];
        return true;
    }
    const NSRect screenFrame = [window screen].frame;
    NSRect frame = NSMakeRect(X,
                              screenFrame.origin.y + screenFrame.size.height - Y - nHeight,
                              nWidth, nHeight);
    [window setFrame:frame display:YES];
    return true;
}

bool NativeWindow_MacOS::SetWindowPos(const NativeWindow_MacOS* /*pInsertAfterWindow*/,
                                      InsertAfterFlag insertAfterFlag,
                                      int32_t X, int32_t Y, int32_t cx, int32_t cy, uint32_t uFlags)
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    NSWindow* window = (__bridge NSWindow*)m_nsWindow;
    if (insertAfterFlag == InsertAfterFlag::kHWND_TOPMOST) {
        [window setLevel:NSFloatingWindowLevel];
    }
    else if (insertAfterFlag == InsertAfterFlag::kHWND_NOTOPMOST) {
        [window setLevel:NSNormalWindowLevel];
    }

    //Respect kSWP_NOMOVE / kSWP_NOSIZE like the SDL backend does. Callers such as
    //WindowBase::Resize pass placeholder 0,0 with kSWP_NOMOVE; moving the window
    //there would yank it to the top-left corner.
    if ((uFlags & kSWP_NOMOVE) && (uFlags & kSWP_NOSIZE)) {
        return true;
    }
    NSRect frame = [window frame];
    if (!(uFlags & kSWP_NOSIZE)) {
        frame.size.width = cx;
        frame.size.height = cy;
    }
    if (!(uFlags & kSWP_NOMOVE)) {
        NSScreen* screen = [window screen];
        if (screen == nil) {
            //The window may not be attached to a screen yet (e.g. a popup menu
            //before it is shown); fall back to the main screen for positioning.
            screen = [NSScreen mainScreen];
        }
        const NSRect screenFrame = screen.frame;
        //dui top-left convention: Y is measured from the screen top.
        frame.origin.x = X;
        frame.origin.y = screenFrame.size.height - Y - frame.size.height;
    }
    [window setFrame:frame display:YES];

    if (uFlags & kSWP_HIDEWINDOW) {
        [window orderOut:nil];
    }
    if (uFlags & kSWP_SHOWWINDOW) {
        if (uFlags & kSWP_NOACTIVATE) {
            //Non-activating popups (menus, combos) must not steal key focus.
            [window orderFront:nil];
        }
        else {
            [window makeKeyAndOrderFront:nil];
        }
    }
    return true;
}

void NativeWindow_MacOS::SetWindowMaximumSize(const UiSize& szMaxWindow)
{
    m_szMaxWindow = szMaxWindow;
    if (m_nsWindow != nullptr) {
        NSWindow* window = (__bridge NSWindow*)m_nsWindow;
        if ((szMaxWindow.cx > 0) && (szMaxWindow.cy > 0)) {
            window.contentMaxSize = NSMakeSize(szMaxWindow.cx, szMaxWindow.cy);
        }
    }
}

const UiSize& NativeWindow_MacOS::GetWindowMaximumSize() const
{
    return m_szMaxWindow;
}

void NativeWindow_MacOS::SetWindowMinimumSize(const UiSize& szMinWindow)
{
    m_szMinWindow = szMinWindow;
    if (m_nsWindow != nullptr) {
        NSWindow* window = (__bridge NSWindow*)m_nsWindow;
        if ((szMinWindow.cx > 0) && (szMinWindow.cy > 0)) {
            window.contentMinSize = NSMakeSize(szMinWindow.cx, szMinWindow.cy);
        }
    }
}

const UiSize& NativeWindow_MacOS::GetWindowMinimumSize() const
{
    return m_szMinWindow;
}

// ---------------------------------------------------------------------------
// Capture
// ---------------------------------------------------------------------------
void NativeWindow_MacOS::SetCapture()
{
    m_bMouseCapture = true;
}

void NativeWindow_MacOS::ReleaseCapture()
{
    m_bMouseCapture = false;
}

bool NativeWindow_MacOS::IsCaptured() const
{
    return m_bMouseCapture;
}

// ---------------------------------------------------------------------------
// Window region (no-op: macOS windows are naturally shaped)
// ---------------------------------------------------------------------------
bool NativeWindow_MacOS::SetWindowRoundRectRgn(const UiRect& /*rcWnd*/, float /*rx*/, float /*ry*/, bool /*bRedraw*/)
{
    return true;
}

bool NativeWindow_MacOS::SetWindowRectRgn(const UiRect& /*rcWnd*/, bool /*bRedraw*/)
{
    return true;
}

void NativeWindow_MacOS::ClearWindowRgn(bool /*bRedraw*/)
{
}

// ---------------------------------------------------------------------------
// Layered window / alpha
// ---------------------------------------------------------------------------
bool NativeWindow_MacOS::SetLayeredWindow(bool bIsLayeredWindow, bool /*bRedraw*/)
{
    m_bIsLayeredWindow = bIsLayeredWindow;
    if (m_nsWindow != nullptr) {
        NSWindow* window = (__bridge NSWindow*)m_nsWindow;
        if (bIsLayeredWindow) {
            [window setOpaque:NO];
            [window setBackgroundColor:[NSColor clearColor]];
        }
        else {
            [window setOpaque:YES];
            [window setBackgroundColor:[NSColor windowBackgroundColor]];
        }
    }
    return true;
}

bool NativeWindow_MacOS::IsLayeredWindow() const
{
    return m_bIsLayeredWindow;
}

void NativeWindow_MacOS::SetLayeredWindowAlpha(int32_t nAlpha)
{
    m_nLayeredWindowAlpha = (uint8_t)std::clamp(nAlpha, 0, 255);
    if (m_nsWindow != nullptr && m_bWindowShown) {
        [(__bridge NSWindow*)m_nsWindow setAlphaValue:(m_nLayeredWindowAlpha / 255.0f)];
    }
}

uint8_t NativeWindow_MacOS::GetLayeredWindowAlpha() const
{
    return m_nLayeredWindowAlpha;
}

void NativeWindow_MacOS::SetLayeredWindowOpacity(int32_t nAlpha)
{
    m_nLayeredWindowOpacity = (uint8_t)std::clamp(nAlpha, 0, 255);
}

uint8_t NativeWindow_MacOS::GetLayeredWindowOpacity() const
{
    return m_nLayeredWindowOpacity;
}

// ---------------------------------------------------------------------------
// System shadow
// ---------------------------------------------------------------------------
bool NativeWindow_MacOS::IsSystemShadowSupported() const
{
    //macOS provides system shadows (see INativeWindow.h)
    return true;
}

bool NativeWindow_MacOS::SetSystemShadowType(NativeWindowShadowType nativeShadowType)
{
    if (m_nsWindow == nullptr) {
        return false;
    }
    if (m_bChildWindow && (nativeShadowType != NativeWindowShadowType::kShadowSystemDisabled)) {
        //Child windows have no chrome of their own; converting their style mask
        //(titled + hidden title bar, rounded corners, shadow) would corrupt them
        return false;
    }
    //Reuse the pure-AppKit window-chrome helper below (converts a
    //borderless window into a titled document window with the title bar hidden,
    //giving the OS rounded corners and a deep shadow).
    const bool bPopupWindow = m_createParam.m_dwStyle == kWS_POPUP;
    if (ModifyNsWindowShadowType(m_nsWindow, nativeShadowType, bPopupWindow, m_bUseSystemCaption)) {
        //The style-mask conversion must not detach our view from the window;
        //re-assert it as the content view defensively.
        if (m_nsView != nullptr) {
            NSWindow* window = (__bridge NSWindow*)m_nsWindow;
            if (window.contentView != (__bridge NSView*)m_nsView) {
                window.contentView = (__bridge NSView*)m_nsView;
            }
        }
        m_systemShadowType = nativeShadowType;
        return true;
    }
    return false;
}

NativeWindowShadowType NativeWindow_MacOS::GetSystemShadowType() const
{
    return m_systemShadowType;
}

void NativeWindow_MacOS::RefreshSystemShadow()
{
    if (m_nsWindow != nullptr) {
        [(__bridge NSWindow*)m_nsWindow invalidateShadow];
    }
}

void NativeWindow_MacOS::ClearWindowRgnForSystemShadow()
{
}

// ---------------------------------------------------------------------------
// Title / icon
// ---------------------------------------------------------------------------
void NativeWindow_MacOS::SetText(const DString& strText)
{
    if (m_nsWindow == nullptr) {
        return;
    }
    const std::string text = StringConvert::TToUTF8(strText);
    NSString* nsTitle = [NSString stringWithUTF8String:text.c_str()];
    [(__bridge NSWindow*)m_nsWindow setTitle:nsTitle];
}

DString NativeWindow_MacOS::GetText() const
{
    if (m_nsWindow == nullptr) {
        return DString();
    }
    NSString* nsTitle = [(__bridge NSWindow*)m_nsWindow title];
    if (nsTitle == nil) {
        return DString();
    }
    return StringConvert::UTF8ToT(std::string([nsTitle UTF8String]));
}

bool NativeWindow_MacOS::SetWindowIcon(const FilePath& /*iconFilePath*/)
{
    return false;
}

bool NativeWindow_MacOS::SetWindowIcon(const std::vector<uint8_t>& /*iconFileData*/, const DString& /*iconFileName*/)
{
    return false;
}

void NativeWindow_MacOS::SetUseSystemCaption(bool bUseSystemCaption)
{
    m_bUseSystemCaption = bUseSystemCaption;
}

bool NativeWindow_MacOS::IsUseSystemCaption() const
{
    return m_bUseSystemCaption;
}

void NativeWindow_MacOS::KeepParentActive()
{
}

// ---------------------------------------------------------------------------
// Modal
// ---------------------------------------------------------------------------
int32_t NativeWindow_MacOS::DoModal(NativeWindow_MacOS* pParentWindow,
                                    const WindowCreateParam& createParam,
                                    const WindowCreateAttributes& createAttributes,
                                    bool bCloseByEsc,
                                    bool bCloseByEnter)
{
    ASSERT(m_nsWindow == nullptr);
    if (m_nsWindow != nullptr) {
        return -1;
    }

    //Save the parameters and use the same window-creation path as CreateWnd.
    m_createParam = createParam;
    m_bUseSystemCaption = createAttributes.m_bUseSystemCaptionDefined &&
                          createAttributes.m_bUseSystemCaption;
    if (m_createParam.m_dwStyle == 0) {
        m_createParam.m_dwStyle = kWS_OVERLAPPEDWINDOW;
    }

    if (!CreateWindowAndRender(pParentWindow, createAttributes)) {
        return -1;
    }

    if (m_pOwner != nullptr) {
        bool bHandled = false;
        m_pOwner->OnNativeCreateWndMsg(true, NativeMsg(0, 0, 0), bHandled);
    }

    m_bDoModal = true;
    ShowWindow(ShowWindowCommands::kSW_SHOW_NORMAL);
    MessageLoop_MacOS msgLoop;
    msgLoop.RunDoModal(*this, bCloseByEsc, bCloseByEnter);
    m_bDoModal = false;

    //Return focus to the owner after the modal dialog closes.
    if (pParentWindow != nullptr && pParentWindow->IsWindow()) {
        pParentWindow->SetWindowFocus();
    }
    return m_closeParam;
}

void NativeWindow_MacOS::ShowModalFake(NativeWindow_MacOS* /*pParentWindow*/)
{
    //Show the fake-modal window. Do NOT disable the owner: on macOS
    //EnableWindow(false) also makes the owner semi-transparent, which was not
    //the intended look for this example.
    ShowWindow(ShowWindowCommands::kSW_SHOW_NORMAL);
    m_bFakeModal = true;
}

void NativeWindow_MacOS::OnCloseModalFake(NativeWindow_MacOS* pParentWindow)
{
    if (m_bFakeModal) {
        m_bFakeModal = false;
        if (pParentWindow != nullptr && pParentWindow->IsWindow()) {
            pParentWindow->SetWindowFocus();
        }
    }
}

bool NativeWindow_MacOS::IsFakeModal() const
{
    return m_bFakeModal;
}

bool NativeWindow_MacOS::IsDoModal() const
{
    return m_bDoModal;
}

// ---------------------------------------------------------------------------
// Hotkeys (Phase 3)
// ---------------------------------------------------------------------------
int32_t NativeWindow_MacOS::SetWindowHotKey(uint8_t /*wVirtualKeyCode*/, uint8_t /*wModifiers*/)
{
    return -1;
}

bool NativeWindow_MacOS::GetWindowHotKey(uint8_t& /*wVirtualKeyCode*/, uint8_t& /*wModifiers*/) const
{
    return false;
}

bool NativeWindow_MacOS::RegisterHotKey(uint8_t /*wVirtualKeyCode*/, uint8_t /*wModifiers*/, int32_t /*id*/)
{
    return false;
}

bool NativeWindow_MacOS::UnregisterHotKey(int32_t /*id*/)
{
    return false;
}

// ---------------------------------------------------------------------------
// Misc stubs (later phases)
// ---------------------------------------------------------------------------
bool NativeWindow_MacOS::CreateChildWnd(NativeWindow_MacOS* pParentWindow, int32_t nX, int32_t nY, int32_t nWidth, int32_t nHeight)
{
    ASSERT(m_nsWindow == nullptr);
    if (m_nsWindow != nullptr) {
        return false;
    }
    ASSERT(pParentWindow != nullptr);
    if (pParentWindow == nullptr) {
        return false;
    }

    //The child window coordinates are in the parent client area (top-left
    //origin, points); the window stays hidden until ShowWindow is called
    //(avoids flicker when creating the window, same as top-level creation).
    m_bChildWindow = true;
    m_createParam = WindowCreateParam();
    m_createParam.m_nX = nX;
    m_createParam.m_nY = nY;
    m_createParam.m_nWidth = nWidth;
    m_createParam.m_nHeight = nHeight;

    WindowCreateAttributes createAttributes;
    if (!CreateWindowAndRender(pParentWindow, createAttributes)) {
        m_bChildWindow = false;
        return false;
    }

    if (m_pOwner != nullptr) {
        bool bHandled = false;
        m_pOwner->OnNativeCreateWndMsg(false, NativeMsg(0, 0, 0), bHandled);
    }
    return true;
}

void NativeWindow_MacOS::SetImeOpenStatus(bool /*bOpen*/)
{
}

void NativeWindow_MacOS::SetTextInputArea(const UiRect* rect, int32_t nCursor)
{
    if (rect == nullptr) {
        m_bHasTextInputArea = false;
        m_textInputArea.Clear();
        m_nTextInputCursor = 0;
        return;
    }

    m_bHasTextInputArea = true;
    m_textInputArea = *rect;
    m_nTextInputCursor = nCursor;
}

bool NativeWindow_MacOS::GetTextInputArea(UiRect& rect) const
{
    if (!m_bHasTextInputArea) {
        return false;
    }
    rect = m_textInputArea;
    return true;
}

int32_t NativeWindow_MacOS::GetTextInputCursorOffset() const
{
    return m_nTextInputCursor;
}

void NativeWindow_MacOS::SetEnableDragDrop(bool bEnable)
{
    m_bEnableDragDrop = bEnable;
}

bool NativeWindow_MacOS::IsEnableDragDrop() const
{
    return m_bEnableDragDrop;
}

Control* NativeWindow_MacOS::FindControl(const UiPoint& pt) const
{
    if (m_pOwner != nullptr) {
        return m_pOwner->OnNativeFindControl(pt);
    }
    return nullptr;
}

bool NativeWindow_MacOS::NeedCenterWindowAfterCreated() const
{
    return m_createParam.m_bCenterWindow;
}

bool NativeWindow_MacOS::GetWindowSize(int32_t* w, int32_t* h) const
{
    if (m_nsView == nullptr) {
        return false;
    }
    NSRect rc = [(__bridge NSView*)m_nsView bounds];
    if (w != nullptr) {
        *w = (int32_t)rc.size.width;
    }
    if (h != nullptr) {
        *h = (int32_t)rc.size.height;
    }
    return true;
}

bool NativeWindow_MacOS::GetWindowSizeInPixels(int32_t* w, int32_t* h) const
{
    if (m_nsView == nullptr) {
        return false;
    }
    NSRect rc = [(__bridge NSView*)m_nsView convertRectToBacking:[(__bridge NSView*)m_nsView bounds]];
    if (w != nullptr) {
        *w = (int32_t)rc.size.width;
    }
    if (h != nullptr) {
        *h = (int32_t)rc.size.height;
    }
    return true;
}

float NativeWindow_MacOS::GetDisplayContentScale() const
{
    if (m_nsWindow == nullptr) {
        return 1.0f;
    }
    return (float)[(__bridge NSWindow*)m_nsWindow backingScaleFactor];
}

float NativeWindow_MacOS::GetWindowDisplayScale() const
{
    if (m_nsWindow == nullptr) {
        return 1.0f;
    }
    return (float)[(__bridge NSWindow*)m_nsWindow backingScaleFactor];
}

float NativeWindow_MacOS::GetWindowPixelDensity() const
{
    return GetWindowDisplayScale();
}

bool NativeWindow_MacOS::GetMonitorRect(UiRect& rcMonitor) const
{
    UiRect rcWork;
    return GetMonitorRect(rcMonitor, rcWork);
}

bool NativeWindow_MacOS::GetMonitorRect(UiRect& rcMonitor, UiRect& rcWork) const
{
    NSScreen* screen = nil;
    if (m_nsWindow != nullptr) {
        screen = [(__bridge NSWindow*)m_nsWindow screen];
    }
    if (screen == nil) {
        screen = [NSScreen mainScreen];
    }
    if (screen == nil) {
        return false;
    }
    const NSRect screenFrame = screen.frame;

    NSRect rc = screen.frame;
    rcMonitor.left = (int32_t)rc.origin.x;
    rcMonitor.top = (int32_t)(screenFrame.size.height - rc.origin.y - rc.size.height);
    rcMonitor.right = rcMonitor.left + (int32_t)rc.size.width;
    rcMonitor.bottom = rcMonitor.top + (int32_t)rc.size.height;

    rc = [screen visibleFrame];
    rcWork.left = (int32_t)rc.origin.x;
    rcWork.top = (int32_t)(screenFrame.size.height - rc.origin.y - rc.size.height);
    rcWork.right = rcWork.left + (int32_t)rc.size.width;
    rcWork.bottom = rcWork.top + (int32_t)rc.size.height;
    return true;
}

bool NativeWindow_MacOS::GetMonitorWorkRect(UiRect& rcWork) const
{
    NSScreen* screen = nil;
    if (m_nsWindow != nullptr) {
        screen = [(__bridge NSWindow*)m_nsWindow screen];
    }
    if (screen == nil) {
        screen = [NSScreen mainScreen];
    }
    if (screen == nil) {
        return false;
    }
    NSRect rc = [screen visibleFrame];
    const NSRect screenFrame = screen.frame;
    rcWork.left = (int32_t)rc.origin.x;
    rcWork.top = (int32_t)(screenFrame.size.height - rc.origin.y - rc.size.height);
    rcWork.right = rcWork.left + (int32_t)rc.size.width;
    rcWork.bottom = rcWork.top + (int32_t)rc.size.height;
    return true;
}

bool NativeWindow_MacOS::GetMonitorWorkRect(const UiPoint& /*pt*/, UiRect& rcWork) const
{
    return GetMonitorWorkRect(rcWork);
}

bool NativeWindow_MacOS::GetPrimaryMonitorWorkRect(UiRect& rcWork)
{
    NSScreen* screen = [NSScreen mainScreen];
    if (screen == nullptr) {
        return false;
    }
    NSRect rc = [screen visibleFrame];
    const NSRect screenFrame = [NSScreen mainScreen].frame;
    rcWork.left = (int32_t)rc.origin.x;
    rcWork.top = (int32_t)(screenFrame.size.height - rc.origin.y - rc.size.height);
    rcWork.right = rcWork.left + (int32_t)rc.size.width;
    rcWork.bottom = rcWork.top + (int32_t)rc.size.height;
    return true;
}

INativeWindow* NativeWindow_MacOS::WindowBaseFromPoint(const UiPoint& /*pt*/, bool /*bIgnoreChildWindow*/)
{
    return m_pOwner;
}

void NativeWindow_MacOS::SetEnableSnapLayoutMenu(bool /*bEnable*/)
{
}

bool NativeWindow_MacOS::IsEnableSnapLayoutMenu() const
{
    return false;
}

void NativeWindow_MacOS::SetEnableSysMenu(bool /*bEnable*/)
{
}

bool NativeWindow_MacOS::IsEnableSysMenu() const
{
    return false;
}

// ---------------------------------------------------------------------------
// Text input (NSTextInputClient bridge)
// ---------------------------------------------------------------------------
void NativeWindow_MacOS::OnNativeInsertText(const DStringW& text)
{
    INativeWindow* pOwner = m_pOwner;
    if ((pOwner == nullptr) || text.empty()) {
        return;
    }
    //Equivalent to the SDL SDL_EVENT_TEXT_INPUT / Windows WM_CHAR message:
    //wParam = the address of the whole string, lParam = the character count
    NativeMsg nativeMsg(kWM_USER, (WPARAM)text.c_str(), (LPARAM)text.size());
    bool bHandled = false;
    pOwner->OnNativeCharMsg(VirtualKeyCode::kVK_None, 0, nativeMsg, bHandled);
}

void NativeWindow_MacOS::OnNativeMarkedText(const DStringW& text)
{
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        return;
    }
    bool bHandled = false;
    if (text.empty()) {
        //Composition finished
        pOwner->OnNativeImeEndCompositionMsg(NativeMsg(0, 0, 0), bHandled);
    }
    else {
        //Composition (re)started or updated
        pOwner->OnNativeImeStartCompositionMsg(NativeMsg(0, 0, 0), bHandled);
        NativeMsg nativeMsg(kWM_USER, (WPARAM)text.c_str(), (LPARAM)text.size());
        bHandled = false;
        pOwner->OnNativeImeCompositionMsg(nativeMsg, bHandled);
    }
}

// ---------------------------------------------------------------------------
// Drag & drop (NSDraggingDestination bridge, reusing the shared ControlDropData_SDL)
// ---------------------------------------------------------------------------
void NativeWindow_MacOS::OnDropBegin()
{
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        return;
    }
    ControlDropData_SDL data;
    data.m_bHandled = false;
    data.m_ptClientX = 0;
    data.m_ptClientY = 0;
    data.m_bTextData = false;
    pOwner->OnNativeDropEnterMsg(kControlDropTypeSDL, &data);
}

void NativeWindow_MacOS::OnDropPosition(const UiPoint& pt, bool& bHandled)
{
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        bHandled = false;
        return;
    }
    UiPoint clientPt = pt;
    pOwner->OnNativeGetDpi().WindowSizeToClientSize(clientPt);
    ControlDropData_SDL data;
    data.m_bHandled = false;
    data.m_ptClientX = clientPt.x;
    data.m_ptClientY = clientPt.y;
    data.m_bTextData = false;
    pOwner->OnNativeDropOverMsg(kControlDropTypeSDL, &data);
    bHandled = data.m_bHandled;
}

void NativeWindow_MacOS::OnDropTexts(const std::vector<DString>& textList, const UiPoint& pt, bool& bHandled)
{
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        bHandled = false;
        return;
    }
    UiPoint clientPt = pt;
    pOwner->OnNativeGetDpi().WindowSizeToClientSize(clientPt);
    ControlDropData_SDL data;
    data.m_bHandled = false;
    data.m_ptClientX = clientPt.x;
    data.m_ptClientY = clientPt.y;
    data.m_bTextData = true;
    data.m_textList = textList;
    pOwner->OnNativeDropMsg(kControlDropTypeSDL, &data);
    bHandled = data.m_bHandled;
}

void NativeWindow_MacOS::OnDropFiles(const DString& source, const std::vector<DString>& fileList, const UiPoint& pt, bool& bHandled)
{
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        bHandled = false;
        return;
    }
    UiPoint clientPt = pt;
    pOwner->OnNativeGetDpi().WindowSizeToClientSize(clientPt);
    ControlDropData_SDL data;
    data.m_bHandled = false;
    data.m_ptClientX = clientPt.x;
    data.m_ptClientY = clientPt.y;
    data.m_bTextData = false;
    data.m_source = source;
    data.m_fileList = fileList;
    pOwner->OnNativeDropMsg(kControlDropTypeSDL, &data);
    bHandled = data.m_bHandled;
}

void NativeWindow_MacOS::OnDropLeave()
{
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        return;
    }
    pOwner->OnNativeDropLeaveMsg();
}

// ---------------------------------------------------------------------------
// AppKit event bridge
// ---------------------------------------------------------------------------
bool NativeWindow_MacOS::OnNativeEvent(void* pEvent)
{
    //No event delivery after the close started (leftover events during the
    //deferred teardown would touch a half-destroyed window).
    if (m_bCloseing || m_bFinalized) {
        return false;
    }
    NSEvent* event = (NSEvent*)pEvent;
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        return false;
    }

    const NSEventType type = event.type;
    NSPoint locationInWindow = event.locationInWindow;

    //Convert to dui client coordinates (top-left origin). Mirror the SDL
    //implementation: the event arrives in window (point) coordinates and is
    //converted to client (pixel) coordinates via the DpiManager - that is the
    //space dui lays the UI out in and dispatches control hit-testing with.
    UiPoint pt;
    if (m_nsView != nullptr) {
        NSView* view = (__bridge NSView*)m_nsView;
        NSPoint p = [view convertPoint:locationInWindow fromView:nil];
        pt.x = (int32_t)p.x;
        pt.y = (int32_t)p.y;
    }
    pOwner->OnNativeGetDpi().WindowSizeToClientSize(pt);

    uint32_t modifierKey = 0;
    NSEventModifierFlags flags = event.modifierFlags;
    if (flags & NSEventModifierFlagShift) {
        modifierKey |= ModifierKey::kShift;
    }
    if (flags & NSEventModifierFlagControl) {
        modifierKey |= ModifierKey::kControl;
    }
    if (flags & NSEventModifierFlagOption) {
        modifierKey |= ModifierKey::kAlt;
    }
    if (flags & NSEventModifierFlagCommand) {
        modifierKey |= ModifierKey::kWin;
    }

    bool bHandled = false;
    LRESULT lResult = 0;

    //Clicking outside an open menu should dismiss it. Menu windows themselves
    //are excluded: their own click handling activates/closes the menu.
    if ((type == NSEventTypeLeftMouseDown) ||
        (type == NSEventTypeRightMouseDown) ||
        (type == NSEventTypeOtherMouseDown)) {
        WindowBase* pWindowBase = dynamic_cast<WindowBase*>(pOwner);
        if ((pWindowBase != nullptr) && (dynamic_cast<ui::Menu*>(pWindowBase) == nullptr)) {
            ui::Menu::CloseAllMenus();
        }
    }

    switch (type) {
    case NSEventTypeLeftMouseDown:
        //Borderless windows: title-bar dragging and edge resizing are handled
        //here (no WndProc on macOS); the UI still receives the click otherwise.
        if (!IsUseSystemCaption() && !IsWindowFullscreen()) {
            MacHitTestResult hitTest = MacWindowHitTest(this, pt);
            if (hitTest == kMacHitTestDraggable) {
                [(__bridge NSWindow*)m_nsWindow performWindowDragWithEvent:event];
                return true;
            }
            if (hitTest != kMacHitTestNormal) {
                MacBeginResizeDrag(this, hitTest, event);
                return true;
            }
        }
        //fall through: normal left-button-down message
    case NSEventTypeRightMouseDown:
    case NSEventTypeOtherMouseDown:
        m_ptLastMousePos = pt;
        if (type == NSEventTypeLeftMouseDown) {
            lResult = pOwner->OnNativeMouseLButtonDownMsg(pt, modifierKey, NativeMsg(0, 0, 0), bHandled);
        }
        else if (type == NSEventTypeRightMouseDown) {
            lResult = pOwner->OnNativeMouseRButtonDownMsg(pt, modifierKey, NativeMsg(0, 0, 0), bHandled);
        }
        else {
            lResult = pOwner->OnNativeMouseMButtonDownMsg(pt, modifierKey, NativeMsg(0, 0, 0), bHandled);
        }
        break;
    case NSEventTypeLeftMouseUp:
    case NSEventTypeRightMouseUp:
    case NSEventTypeOtherMouseUp:
        m_ptLastMousePos = pt;
        if (type == NSEventTypeLeftMouseUp) {
            lResult = pOwner->OnNativeMouseLButtonUpMsg(pt, modifierKey, NativeMsg(0, 0, 0), bHandled);
            //Simulate Windows: double-click arrives after the up message
            if (!bHandled && (event.clickCount >= 2)) {
                bool bDbClickHandled = false;
                pOwner->OnNativeMouseLButtonDbClickMsg(pt, modifierKey, NativeMsg(0, 0, 0), bDbClickHandled);
            }
        }
        else if (type == NSEventTypeRightMouseUp) {
            lResult = pOwner->OnNativeMouseRButtonUpMsg(pt, modifierKey, NativeMsg(0, 0, 0), bHandled);
            //Simulate Windows: trigger a Context Menu event on right-button up
            if (!bHandled) {
                bool bMenuHandled = false;
                pOwner->OnNativeContextMenuMsg(pt, NativeMsg(0, 0, 0), bMenuHandled);
            }
        }
        else {
            lResult = pOwner->OnNativeMouseMButtonUpMsg(pt, modifierKey, NativeMsg(0, 0, 0), bHandled);
        }
        break;
    case NSEventTypeLeftMouseDragged:
    case NSEventTypeRightMouseDragged:
    case NSEventTypeOtherMouseDragged:
    case NSEventTypeMouseMoved:
        m_ptLastMousePos = pt;
        lResult = pOwner->OnNativeMouseMoveMsg(pt, modifierKey, false, NativeMsg(0, 0, 0), bHandled);
        break;
    case NSEventTypeScrollWheel:
        {
            //Prefer the dominant axis. Horizontal trackpad swipes are encoded
            //with wParam=1 so ScrollBox can route them to the horizontal bar
            //even when a vertical scrollbar is present.
            const double dx = event.scrollingDeltaX;
            const double dy = event.scrollingDeltaY;
            if (fabs(dx) > fabs(dy)) {
                int32_t wheelDelta = (int32_t)(dx * 120.0);
                lResult = pOwner->OnNativeMouseWheelMsg(wheelDelta, pt, modifierKey, NativeMsg(0, 1, 0), bHandled);
            }
            else {
                int32_t wheelDelta = (int32_t)(dy * 120.0);
                lResult = pOwner->OnNativeMouseWheelMsg(wheelDelta, pt, modifierKey, NativeMsg(0, 0, 0), bHandled);
            }
        }
        break;
    case NSEventTypeKeyDown:
        {
            VirtualKeyCode vkCode = GetVirtualKeyCodeFromNativeKeyCode((uint16_t)event.keyCode);
            lResult = pOwner->OnNativeKeyDownMsg(vkCode, modifierKey, NativeMsg(0, 0, 0), bHandled);
        }
        break;
    case NSEventTypeKeyUp:
        {
            VirtualKeyCode vkCode = GetVirtualKeyCodeFromNativeKeyCode((uint16_t)event.keyCode);
            lResult = pOwner->OnNativeKeyUpMsg(vkCode, modifierKey, NativeMsg(0, 0, 0), bHandled);
        }
        break;
    default:
        break;
    }
    return bHandled;
}

// ---------------------------------------------------------------------------
// NSWindow delegate callbacks
// ---------------------------------------------------------------------------
void NativeWindow_MacOS::OnNativeWindowDidResize()
{
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        return;
    }
    UiSize newWindowSize;
    GetWindowSize(&newWindowSize.cx, &newWindowSize.cy);
    WindowSizeType sizeType = WindowSizeType::kSIZE_RESTORED;
    if (IsWindowMaximized()) {
        sizeType = WindowSizeType::kSIZE_MAXIMIZED;
    }
    else if (IsWindowMinimized()) {
        sizeType = WindowSizeType::kSIZE_MINIMIZED;
    }
    bool bHandled = false;
    pOwner->OnNativeSizeMsg(sizeType, newWindowSize, NativeMsg(0, 0, 0), bHandled);

    //A resize also moves/resizes the window frame; deliver the pos-changed
    //message (WM_WINDOWPOSCHANGED parity) so native child windows re-sync.
    bool bPosHandled = false;
    pOwner->OnNativeWindowPosChangedMsg(NativeMsg(0, 0, 0), bPosHandled);

    //After a resize the whole content must be redrawn (the dui size handler
    //resized the render surface); request a full repaint.
    Invalidate(UiRect());
}

void NativeWindow_MacOS::OnNativeWindowDidMove()
{
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        return;
    }
    //Popups are not attached to the owner; dismiss open menus only when the
    //window that moved is the host window of one of them (moving the owner
    //drops the popup, Windows parity). Transient windows moving - tooltips,
    //the menu windows themselves, other popups - must not dismiss menus.
    if (WindowBase* pMovedWindow = dynamic_cast<WindowBase*>(pOwner)) {
        ui::Menu::CloseMenusIfHostWindowMoved(pMovedWindow);
    }
    UiPoint ptTopLeft;
    GetWindowRect(m_rcWindowRect);
    ptTopLeft.x = m_rcWindowRect.left;
    ptTopLeft.y = m_rcWindowRect.top;
    bool bHandled = false;
    pOwner->OnNativeMoveMsg(ptTopLeft, NativeMsg(0, 0, 0), bHandled);
}

void NativeWindow_MacOS::OnNativeWindowWillMove()
{
    if (m_pOwner == nullptr) {
        return;
    }
    UiRect rcWindow;
    GetWindowRect(rcWindow);
    bool bHandled = false;
    //Notify the host before AppKit moves the window so transient controls can
    //close before the compositor presents a frame at the old position.
    m_pOwner->OnNativeMoveMsg({rcWindow.left, rcWindow.top}, NativeMsg(0, 0, 0), bHandled);
}

void NativeWindow_MacOS::OnNativeWindowBecomeKey()
{
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        return;
    }
    //Keep the dui view as the first responder so keyboard/text input keeps flowing
    if (m_nsWindow != nullptr) {
        NSWindow* window = (__bridge NSWindow*)m_nsWindow;
        if ((m_nsView != nullptr) && (window.firstResponder != (__bridge NSView*)m_nsView)) {
            [window makeFirstResponder:(__bridge NSView*)m_nsView];
        }
        //Refresh the shadow so it matches the active-window appearance
        [window invalidateShadow];
    }
    bool bHandled = false;
    pOwner->OnNativeSetFocusMsg(nullptr, NativeMsg(0, 0, 0), bHandled);
}

void NativeWindow_MacOS::OnNativeWindowResignKey()
{
    INativeWindow* pOwner = m_pOwner;
    if (pOwner == nullptr) {
        return;
    }
    bool bHandled = false;
    pOwner->OnNativeKillFocusMsg(nullptr, NativeMsg(0, 0, 0), bHandled);
}

/** Enable/disable the OS-provided window shadow (and matching rounded corners)
 *  by converting the borderless window into a titled document window with
 *  the title bar hidden (FullSizeContentView + titlebarAppearsTransparent).
 *  Returns true on success.
 */
bool ModifyNsWindowShadowType(void* pNSWindow, NativeWindowShadowType nativeShadowType,
                              bool bPopupWindow, bool bUseSystemCaption)
{
    if (pNSWindow == nullptr) {
        return false;
    }
    // Must run on the main thread (AppKit)
    if (![NSThread isMainThread]) {
        __block bool result = false;
        dispatch_sync(dispatch_get_main_queue(), ^{
            result = ModifyNsWindowShadowType(pNSWindow, nativeShadowType, bPopupWindow, bUseSystemCaption);
        });
        return result;
    }

    NSWindow* window = (__bridge NSWindow*)pNSWindow;
    if (window == nullptr || ![window isKindOfClass:[NSWindow class]]) {
        return false;
    }

    NSView* contentView = window.contentView;
    if (contentView == nil) {
        if (nativeShadowType == NativeWindowShadowType::kShadowSystemDisabled) {
            [window setHasShadow:NO];
        }
        else {
            [window setHasShadow:YES];
        }
        [window invalidateShadow];
        return YES;
    }

    // Keep popup windows borderless. Converting a popup to a titled/resizable
    // window just to obtain a system shadow makes AppKit rebuild its surface,
    // which appears as a dark shadow flicker when a Combo is shown or closed.
    if (bPopupWindow) {
        if (nativeShadowType == NativeWindowShadowType::kShadowSystemRound ||
            nativeShadowType == NativeWindowShadowType::kShadowSystemSmallRound) {
            // MenuBar pure-code menus need the same rounded AppKit shadow as
            // XML-backed menus. A titled mask with a hidden title bar gives
            // AppKit a rounded shadow instead of a rectangular popup shadow.
            window.titlebarAppearsTransparent = YES;
            window.titleVisibility = NSWindowTitleHidden;
            NSWindowStyleMask requiredMask = NSWindowStyleMaskTitled |
                                             NSWindowStyleMaskFullSizeContentView |
                                             NSWindowStyleMaskClosable;
            [window setStyleMask:requiredMask];
            [[window standardWindowButton:NSWindowCloseButton] setHidden:YES];
            [[window standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
            [[window standardWindowButton:NSWindowZoomButton] setHidden:YES];
            window.backgroundColor = [NSColor clearColor];
            [window setHasShadow:YES];
            [window invalidateShadow];
        }
        [window setHasShadow:(nativeShadowType != NativeWindowShadowType::kShadowSystemDisabled)];
        if (nativeShadowType == NativeWindowShadowType::kShadowSystemRound ||
            nativeShadowType == NativeWindowShadowType::kShadowSystemSmallRound) {
            // Keep the shadow native to AppKit while clipping the borderless
            // popup content to the matching rounded shape.
            [window setOpaque:NO];
            window.backgroundColor = [NSColor clearColor];
            contentView.wantsLayer = YES;
            CALayer* layer = contentView.layer;
            if (layer != nil) {
                layer.cornerRadius = (nativeShadowType == NativeWindowShadowType::kShadowSystemSmallRound) ? 5.0 : 10.0;
                layer.masksToBounds = YES;
            }
            window.backgroundColor = [NSColor clearColor];
        }
        return YES;
    }

    // Convert the borderless window into a titled document window with the
    // title bar hidden: gives the OS rounded corners, a deep shadow, and
    // hides the native traffic-light buttons (dui draws its own).
    if (nativeShadowType != NativeWindowShadowType::kShadowSystemDisabled) {
        // Force the light appearance so the traffic lights match the dui light theme
        window.appearance = [NSAppearance appearanceNamed:NSAppearanceNameAqua];

        window.titlebarAppearsTransparent = YES;
        window.titleVisibility = NSWindowTitleHidden;
        NSWindowStyleMask requiredMask = NSWindowStyleMaskTitled |
                                         NSWindowStyleMaskFullSizeContentView |
                                         NSWindowStyleMaskClosable |
                                         NSWindowStyleMaskMiniaturizable |
                                         NSWindowStyleMaskResizable;
        if ((window.styleMask & requiredMask) != requiredMask) {
            [window setStyleMask:requiredMask];
        }
        if (!bUseSystemCaption) {
            // Normal dui windows draw their own caption controls.
            [[window standardWindowButton:NSWindowCloseButton] setHidden:YES];
            [[window standardWindowButton:NSWindowMiniaturizeButton] setHidden:YES];
            [[window standardWindowButton:NSWindowZoomButton] setHidden:YES];
        }

        // macOS-style rounded corners + hairline border on the content view
        window.backgroundColor = [NSColor clearColor];
        contentView.wantsLayer = YES;
        CALayer* layer = contentView.layer;
        if (layer != nil) {
            const CGFloat scale = (window.backingScaleFactor > 0.0f) ? window.backingScaleFactor : 1.0f;
            layer.cornerRadius = 10.0;
            layer.masksToBounds = YES;
            layer.borderWidth = 1.0f / scale;   // 1 physical pixel hairline
            layer.borderColor = [NSColor colorWithWhite:0.0f alpha:0.12f].CGColor;
        }

        [window setHasShadow:YES];
        [window invalidateShadow];
        return YES;
    }

    switch (nativeShadowType) {
    case NativeWindowShadowType::kShadowSystemDisabled:
        [window setHasShadow:NO];
        if (contentView.wantsLayer) {
            CALayer* layer = contentView.layer;
            if (layer != nil) {
                layer.cornerRadius = 0;
                layer.masksToBounds = NO;
            }
        }
        break;
    case NativeWindowShadowType::kShadowSystemDefault:
    case NativeWindowShadowType::kShadowSystemRound:
        [window setHasShadow:YES];
        window.backgroundColor = [NSColor clearColor];
        break;
    case NativeWindowShadowType::kShadowSystemSmallRound: {
        [window setHasShadow:YES];
        contentView.wantsLayer = YES;
        CALayer* layer = contentView.layer;
        if (layer != nil) {
            layer.cornerRadius = 5.0;
            layer.masksToBounds = YES;
        }
        window.backgroundColor = [NSColor clearColor];
        break;
    }
    case NativeWindowShadowType::kShadowSystemDoNotRound: {
        [window setHasShadow:YES];
        if (contentView.wantsLayer) {
            CALayer* l2 = contentView.layer;
            if (l2 != nil) {
                l2.cornerRadius = 0;
                l2.masksToBounds = NO;
            }
        }
        window.backgroundColor = [NSColor clearColor];
        break;
    }
    default:
        break;
    }

    [window invalidateShadow];
    return YES;
}

/** After exiting fullscreen the WindowServer re-composites the window; toggle
 *  the style mask to force the rounded corners back (macOS quirk).
 */
void RestoreWindowShadowAfterFullscreen(void* pNSWindow, NativeWindowShadowType nativeShadowType)
{
    if (pNSWindow == nullptr) {
        return;
    }
    if (![NSThread isMainThread]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            RestoreWindowShadowAfterFullscreen(pNSWindow, nativeShadowType);
        });
        return;
    }
    if (nativeShadowType == NativeWindowShadowType::kShadowSystemDisabled) {
        return;
    }
    NSWindow* window = (__bridge NSWindow*)pNSWindow;
    if (window == nullptr) {
        return;
    }
    // Toggle the title mask to force WindowServer to re-composite the window
    // so the rounded corners come back after fullscreen. Then force a full
    // shadow regeneration (hasShadow NO->YES) and re-apply the chrome, or the
    // recomposited window keeps a faint shadow.
    NSWindowStyleMask mask = window.styleMask;
    window.styleMask = mask & ~NSWindowStyleMaskTitled;
    window.styleMask = mask;

    ModifyNsWindowShadowType(pNSWindow, nativeShadowType);

    [window setHasShadow:NO];
    [window setHasShadow:YES];
    [window invalidateShadow];
}

} // namespace ui

#endif //DUI_BUILD_FOR_MACOS
