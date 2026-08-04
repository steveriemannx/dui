#include "BrowserForm_Windows.h"

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
#include "Windows/BrowserBox_Windows.h"
#include "browser/BrowserManager.h"

namespace
{
    // Register this message; receiving it means the taskbar button corresponding to the window has been created by the system, and at this time the ITaskbarList4 interface is initialized
    UINT WM_TASKBARBUTTONCREATED = ::RegisterWindowMessage(TEXT("TaskbarButtonCreated"));
}

BrowserForm_Windows::BrowserForm_Windows()
{
}

BrowserForm_Windows::~BrowserForm_Windows()
{
}

BrowserBox* BrowserForm_Windows::CreateBrowserBox(ui::Window* pWindow, std::string id)
{
    return new BrowserBox_Windows(pWindow, id);
}

LRESULT BrowserForm_Windows::OnWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if (uMsg == WM_TASKBARBUTTONCREATED) {
        bHandled = true;
        m_taskbarManager.Init(this);

        // Because when the window was just created, the browser box had already been created but the WM_TASKBARBUTTONCREATED message had not been received yet, so the RegisterTab function was not called; therefore, after receiving the message, iterate through the tabs that have not been registered again
        for (size_t i = 0; i < m_pBorwserBoxTab->GetItemCount(); ++i) {
            ui::Control *pBoxItem = m_pBorwserBoxTab->GetItemAt(i);
            ASSERT(pBoxItem != nullptr);
            if (pBoxItem == nullptr) {
                continue;
            }

            BrowserBox_Windows* pBrowserBox = dynamic_cast<BrowserBox_Windows*>(pBoxItem);
            if (pBrowserBox == nullptr) {
                continue;
            }

            TaskbarTabItem* pTaskbarItem = pBrowserBox->GetTaskbarItem();
            if (pTaskbarItem != nullptr) {
                m_taskbarManager.RegisterTab(*pTaskbarItem);
            }
        }

        return TRUE;
    }
    return BaseClass::OnWindowMessage(uMsg, wParam, lParam, bHandled);
}

void BrowserForm_Windows::OnCreateNewTabPage(ui::TabCtrlItem* pTabItem, BrowserBox* pBrowserBox)
{
    BaseClass::OnCreateNewTabPage(pTabItem, pBrowserBox);

    BrowserBox_Windows* pBrowserBoxWindows = dynamic_cast<BrowserBox_Windows*>(pBrowserBox);
    if (pBrowserBoxWindows != nullptr) {
        auto pTaskbarItem = pBrowserBoxWindows->GetTaskbarItem();
        if (pTaskbarItem) {
            m_taskbarManager.RegisterTab(*pTaskbarItem);
        }
    }
}

void BrowserForm_Windows::OnCloseTabPage(BrowserBox* pBrowserBox)
{
    BaseClass::OnCloseTabPage(pBrowserBox);

    BrowserBox_Windows* pBrowserBoxWindows = dynamic_cast<BrowserBox_Windows*>(pBrowserBox);
    if (pBrowserBoxWindows != nullptr) {
        auto pTaskbarItem = pBrowserBoxWindows->GetTaskbarItem();
        if (pTaskbarItem) {
            m_taskbarManager.UnregisterTab(*pTaskbarItem);
        }
    }
}

#endif //defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
