#include "MainThread.h"
#include "MainForm.h"

MainThread::MainThread() :
    FrameworkThread(_T("MainThread"), ui::kThreadUI)
{
}

MainThread::~MainThread()
{
}

void MainThread::OnInit()
{
    // Initialize global resources
    constexpr ui::ResourceType resType = ui::ResourceType::kLocalFiles;
    if (resType == ui::ResourceType::kLocalFiles) {
        // Use a local folder as the resource
        ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += _T("resources\\");
        ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));
    }
    else if (resType == ui::ResourceType::kZipFile) {
        // Use a local zip archive as the resource (the archive is in the same directory as the exe)    
        ui::ZipFileResParam resParam;
        resParam.resourcePath = _T("resources\\");
        resParam.zipFilePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resParam.zipFilePath += _T("resources.zip");
        resParam.zipPassword = _T("");
        ui::GlobalManager::Instance().Startup(resParam);
    }
    else {
        return;
    }

    // Create a default centered window with a shadow
    MainForm* window = new MainForm();
    m_pMainForm = window;
    window->CreateWnd(nullptr, ui::WindowCreateParam(_T("ChildWindow"), true));
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NORMAL);
}

void MainThread::OnCleanup()
{
    ui::GlobalManager::Instance().Shutdown();
}

void MainThread::OnMessageLoopIdle()
{
    if (m_pMainForm != nullptr) {
        m_pMainForm->PaintNextChildWindow();
    }
}
