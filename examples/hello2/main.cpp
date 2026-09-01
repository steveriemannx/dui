#include "dui/dui.h"
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

int main()
{
    class Hello2App : public ui::FrameworkThread
    {
    public:
        Hello2App() : FrameworkThread(DUI_T("Hello2"), ui::kThreadUI) {}
        void Run() { RunMessageLoop(); }
    protected:
        void OnInit() override
        {
            // Self-contained resources: load from the flat private resources
            // folder next to the executable instead of the global resources/.
            ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
            resourcePath += DUI_T("hello2_resources/");

            ui::LocalFilesResParam resParam(resourcePath);
            resParam.themePath = ui::FilePath();
            if (!ui::GlobalManager::Instance().Startup(resParam)) {
                ui::SystemUtil::ShowMessageBox(nullptr, DUI_T("Failed to load hello2 resources."), DUI_T("hello2"));
                return;
            }

            MainForm* window = new MainForm();
            if (!window->CreateWnd(nullptr, ui::WindowCreateParam(DUI_T("Hello2 (self-contained)"), true))) {
                ui::SystemUtil::ShowMessageBox(nullptr, DUI_T("Failed to create the window."), DUI_T("hello2"));
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

    Hello2App app;
    app.Run();
    return 0;
}
