#include "dui/Core/CursorManager.h"

#if defined(DUI_BUILD_FOR_MACOS)

#import <Cocoa/Cocoa.h>

namespace ui
{
class CursorManager::TImpl
{
public:
    TImpl();
    ~TImpl();
};

CursorManager::TImpl::TImpl()
{
}

CursorManager::TImpl::~TImpl()
{
}

CursorManager::CursorManager()
{
    m_impl = new TImpl;
}

CursorManager::~CursorManager()
{
    if (m_impl != nullptr) {
        delete m_impl;
        m_impl = nullptr;
    }
}

bool CursorManager::SetCursor(CursorType cursorType)
{
    // Map the dui cursor types to the closest AppKit system cursors.
    NSCursor* cursor = nil;
    switch (cursorType) {
    case CursorType::kCursorArrow:
        cursor = [NSCursor arrowCursor];
        break;
    case CursorType::kCursorIBeam:
        cursor = [NSCursor IBeamCursor];
        break;
    case CursorType::kCursorHand:
        cursor = [NSCursor pointingHandCursor];
        break;
    case CursorType::kCursorWait:
    case CursorType::kCursorProgress:
        // AppKit has no spinner/busy cursor; fall back to the arrow.
        cursor = [NSCursor arrowCursor];
        break;
    case CursorType::kCursorCross:
        cursor = [NSCursor crosshairCursor];
        break;
    case CursorType::kCursorSizeWE:
        cursor = [NSCursor resizeLeftRightCursor];
        break;
    case CursorType::kCursorSizeNS:
        cursor = [NSCursor resizeUpDownCursor];
        break;
    case CursorType::kCursorSizeNWSE:
    case CursorType::kCursorSizeNESW:
        // AppKit has no diagonal resize cursors; use the closest resize cursor.
        cursor = [NSCursor resizeLeftRightCursor];
        break;
    case CursorType::kCursorSizeAll:
        cursor = [NSCursor arrowCursor];
        break;
    case CursorType::kCursorNo:
        cursor = [NSCursor operationNotAllowedCursor];
        break;
    default:
        cursor = [NSCursor arrowCursor];
        break;
    }

    if (cursor != nil) {
        [cursor set];
        return true;
    }
    return false;
}

bool CursorManager::SetImageCursor(const Window* pWindow, const FilePath& curImagePath)
{
    // Image cursors (NSCursor + custom NSImage) are not implemented yet.
    (void)pWindow;
    (void)curImagePath;
    return false;
}

bool CursorManager::ShowCursor(bool bShow)
{
    // Hide the cursor until the mouse moves again when hiding is requested;
    // when showing, re-display the cursor immediately.
    [NSCursor setHiddenUntilMouseMoves:!bShow];
    return true;
}

CursorID CursorManager::GetCursorID() const
{
    return 0;
}

bool CursorManager::SetCursorByID(CursorID cursorId)
{
    (void)cursorId;
    return false;
}

} // namespace ui

#endif // defined(DUI_BUILD_FOR_MACOS)
