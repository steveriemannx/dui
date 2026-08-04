#ifndef EXAMPLES_APP_H_
#define EXAMPLES_APP_H_

// dui
#include "dui/dui.h"
#include <vector>

/** Application entry point (defined in main.cpp).
 *  Declared in a header so that the macOS entry point (main_macos.mm,
 *  Objective-C++ for CEF) can access the singleton as well.
 */
class App : public ui::FrameworkThread
{
private:
    App();
    ~App();
    App(const App&) = delete;
    App& operator = (const App&) = delete;

public:
    /** Get the singleton object
    */
    static App& Instance();

    /** Run the program functions
    */
    int Run(int argc, char** argv);

    /** Add a window interface
    */
    void AddMainWindow(ui::Window* pWindow);

    /** Remove a window interface
    */
    void RemoveMainWindow(ui::Window* pWindow);

    /** Set the active window
    */
    void SetActiveMainWindow(ui::Window* pWindow);

    /** Activate the window
    */
    void ActiveMainWindow();

    /** Close all windows
    */
    void CloseMainWindow();

private:
    /** Initialize before running, called before entering the message loop
    */
    virtual void OnInit() override;

    /** Clean up on exit, called after exiting the message loop
    */
    virtual void OnCleanup() override;

    /** Callback function for the main process singleton
    */
    void OnAlreadyRunningAppRelaunch(const std::vector<DString>& argumentList);

    /** Enable the DPI awareness feature setting parameter
    */
    const ui::DpiInitParam& GetDpiInitParam() const;

private:
    /** Main window list
    */
    std::vector<ui::ControlPtrT<ui::Window>> m_pMainWindows;

    /** Active window
    */
    ui::ControlPtrT<ui::Window> m_pActiveWindow;

    /** Enable the DPI awareness feature setting parameter
    */
    ui::DpiInitParam m_dpiInitParam;
};

#endif // EXAMPLES_APP_H_
