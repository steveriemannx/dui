#include "duilib/duilib.h"
#include "CodeForm.h"
#include "ThemeInit.h"
#include "duilib/Utils/AppEntry.h"

/** App: FrameworkThread subclass that serves as the DUILIB_APP_ENTRY target.
 *  RunMessageLoop() calls OnInit() -> message loop -> OnCleanup().
 */
class App : public ui::FrameworkThread
{
public:
    App() : FrameworkThread(_T("App"), ui::kThreadUI) {}

    void Run() { RunMessageLoop(); }

private:
    virtual void OnInit() override
    {
        // Initialize framework using the binary's directory as resource root
        // (resources dir may not exist for standalone distribution - that's OK,
        //  all theme data is registered via InitThemeCompileTime below)
        ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += _T("resources\\");
        ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

        // Compile-time theme: colors, fonts, classes (no XML or files needed)
        InitThemeCompileTime();

        // Create standalone window - all UI defined in C++, no external files
        CodeForm* pCodeForm = new CodeForm();
        pCodeForm->CreateWnd(nullptr, ui::WindowCreateParam(_T("Code UI"), true));
        pCodeForm->PostQuitMsgWhenClosed(true);
        pCodeForm->ShowWindow(ui::kSW_SHOW_NORMAL);
    }

    virtual void OnCleanup() override
    {
        ui::GlobalManager::Instance().Shutdown();
    }
};

DUILIB_APP_ENTRY(App)
