#include "MainThread.h"
#include "browser/BrowserManager.h"
#include "browser/BrowserForm.h"
#include <chrono>

MainThread::MainThread() :
    FrameworkThread(_T("MainThread"), ui::kThreadUI)
{
}

MainThread::~MainThread()
{
}

const ui::DpiInitParam& MainThread::GetDpiInitParam() const
{
    return m_dpiInitParam;
}

void MainThread::OnInit()
{   
    //Set the callback function that controls the main process singleton
    ui::CefManager::GetInstance()->SetAlreadyRunningAppRelaunch(UiBind(&MainThread::OnAlreadyRunningAppRelaunch, this, std::placeholders::_1));

    //Create the first window
    std::string id = BrowserManager::GetInstance()->CreateBrowserID();
    BrowserManager::GetInstance()->CreateBorwserBox(nullptr, id, _T(""));
}

void MainThread::OnCleanup()
{
    ui::GlobalManager::Instance().Shutdown();
}

void MainThread::OnAlreadyRunningAppRelaunch(const std::vector<DString>& argumentList)
{
    if (ui::GlobalManager::Instance().IsInUIThread()) {
        //CEF 133 calls this interface
        BrowserForm* pBrowserForm = BrowserManager::GetInstance()->GetLastActiveBrowserForm();
        if (pBrowserForm != nullptr) {
            pBrowserForm->SetWindowForeground();
            if (!argumentList.empty()) {
                //Only process the first argument
                DString url = argumentList[0];
                pBrowserForm->OpenLinkUrl(url, false);
            }
        }
    }
    else {
        //Forward to the UI thread for processing
        ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, UiBind(&MainThread::OnAlreadyRunningAppRelaunch, this, argumentList));
    }
}
