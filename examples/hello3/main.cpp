#include "dui/dui.h"
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

int main()
{
    class Hello3App : public ui::FrameworkThread
    {
    public:
        Hello3App() : FrameworkThread(_T("Hello3"), ui::kThreadUI) {}
        void Run() { RunMessageLoop(); }
    protected:
        void OnInit() override
        {
            // Resources are kept flat next to the executable.
            ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();

            ui::LocalFilesResParam resParam(resourcePath);
            resParam.themePath = ui::FilePath();
            if (!ui::GlobalManager::Instance().Startup(resParam)) {
                ui::SystemUtil::ShowMessageBox(nullptr, _T("Failed to load hello3 resources."), _T("hello3"));
                return;
            }

            MainForm* window = new MainForm();
            if (!window->CreateWnd(nullptr, ui::WindowCreateParam(_T("Hello3 (flat resources)"), true))) {
                ui::SystemUtil::ShowMessageBox(nullptr, _T("Failed to create the window."), _T("hello3"));
                delete window;
                return;
            }
            window->PostQuitMsgWhenClosed(true);
            window->ShowWindow(ui::kSW_SHOW_NORMAL);
        }
        void OnCleanup() override
        {
            ui::GlobalManager::Instance().Shutdown();
        }
    };

    Hello3App app;
    app.Run();
    return 0;
}
