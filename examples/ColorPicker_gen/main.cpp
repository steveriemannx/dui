#include "dui/dui.h"
#include "dui/Utils/AppEntry.h"

/** App: FrameworkThread subclass that serves as the DUI_APP_ENTRY target.
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
        // Initialize global resources, using the local folder as the resource
        ui::FilePath resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += _T("resources\\");
        ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

        // Create the color picker control (its built-in skin is provided by the library and read from disk; this example has no layout of its own)
        ui::ColorPicker* pColorPicker = new ui::ColorPicker;
        pColorPicker->CreateWnd(nullptr, ui::WindowCreateParam(_T("ColorPicker (Generated Code)"), true));
        pColorPicker->ShowWindow(ui::kSW_SHOW_NORMAL);

        // Set the color before selection
        pColorPicker->SetSelectedColor(ui::UiColor(ui::UiColors::White));

        // After closing the window, exit the main thread
        pColorPicker->PostQuitMsgWhenClosed(true);
    }

    virtual void OnCleanup() override
    {
        ui::GlobalManager::Instance().Shutdown();
    }
};

DUI_APP_ENTRY(App)
