#include "duilib/duilib.h"
#include "MainForm.h"
#include "duilib/Utils/AppEntry.h"

/** App: FrameworkThread subclass that serves as the DUILIB_APP_ENTRY target.
 *  RunMessageLoop() calls OnInit() -> message loop -> OnCleanup().
 */
class App : public ui::FrameworkThread
{
public:
    App() : FrameworkThread(_T("App"), ui::kThreadUI) {}

    void Run() { RunMessageLoop(true); } // bSupportIdle: paint the child window when the message queue is idle

private:
    virtual void OnInit() override
    {
        //Initialize global resources, use the local folder as the resource
        ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += _T("resources\\");
        ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

        // Create a default centered window with a shadow
        MainForm* window = new MainForm();
        m_pMainForm = window;
        window->CreateWnd(nullptr, ui::WindowCreateParam(_T("ChildWindow"), true));
        window->PostQuitMsgWhenClosed(true);
        window->ShowWindow(ui::kSW_SHOW_NORMAL);
    }

    virtual void OnCleanup() override
    {
        ui::GlobalManager::Instance().Shutdown();
    }

    /** Main thread: the message loop is in the Idle state
    */
    virtual void OnMessageLoopIdle() override
    {
        if (m_pMainForm != nullptr) {
            m_pMainForm->PaintNextChildWindow();
        }
    }

private:
    /** Main window
    */
    ui::ControlPtrT<MainForm> m_pMainForm;
};

DUILIB_APP_ENTRY(App)
