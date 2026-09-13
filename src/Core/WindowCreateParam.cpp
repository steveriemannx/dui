#include "dui/Core/WindowCreateParam.h"

namespace ui {

WindowCreateParam::WindowCreateParam():
    m_className("dui_window"),
    m_dwClassStyle(kCS_VREDRAW | kCS_HREDRAW | kCS_DBLCLKS),
    m_dwStyle(0),
    m_dwExStyle(0),
    m_nX(kCW_USEDEFAULT),
    m_nY(kCW_USEDEFAULT),
    m_nWidth(kCW_USEDEFAULT),
    m_nHeight(kCW_USEDEFAULT),
    m_bCenterWindow(false)
{
}

WindowCreateParam::WindowCreateParam(const std::string& windowTitle, const std::string& windowId):
    WindowCreateParam()
{
    m_windowTitle = windowTitle;
    m_windowId = windowId;
}

WindowCreateParam::WindowCreateParam(const std::string& windowTitle, bool bCenterWindow, const std::string& windowId) :
    WindowCreateParam()
{
    m_windowTitle = windowTitle;
    m_bCenterWindow = bCenterWindow;
    m_windowId = windowId;
}

} // namespace ui
