#include "duilib/Core/CursorManager.h"

#ifdef DUILIB_BUILD_FOR_WAYLAND

#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/Control.h"
#include "duilib/Utils/FilePathUtil.h"
#include "duilib/Utils/FileUtil.h"
#include <map>

namespace ui
{
class CursorManager::TImpl
{
public:
    TImpl();
    ~TImpl();

    std::map<CursorType, CursorID> m_cursorIdMap;
    CursorID m_currentCursorId;
    bool m_bCursorVisible;
};

CursorManager::TImpl::TImpl():
    m_currentCursorId(0),
    m_bCursorVisible(true)
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
    (void)cursorType;
    // Wayland cursor management will be handled through wl_pointer
    return true;
}

bool CursorManager::SetImageCursor(const Window* pWindow, const FilePath& curImagePath)
{
    (void)pWindow;
    (void)curImagePath;
    return false;
}

bool CursorManager::ShowCursor(bool bShow)
{
    m_impl->m_bCursorVisible = bShow;
    return true;
}

CursorID CursorManager::GetCursorID() const
{
    return m_impl->m_currentCursorId;
}

bool CursorManager::SetCursorByID(CursorID cursorId)
{
    (void)cursorId;
    return true;
}

} // namespace ui

#endif // DUILIB_BUILD_FOR_WAYLAND
