#ifndef EXAMPLES_APP_H_
#define EXAMPLES_APP_H_

// duilib
#include "duilib/duilib.h"

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

    /** Set the main window interface
    */
    void SetMainWindow(ui::Window* pWindow);

    /** Activate the main window
    */
    void ActiveMainWindow();

    /** Close the main window
    */
    void CloseMainWindow();

private:
    /** Initialize before running, called before entering the message loop
    */
    virtual void OnInit() override;

    /** Clean up on exit, called after exiting the message loop
    */
    virtual void OnCleanup() override;

    /** Enable the DPI awareness feature setting parameter
    */
    const ui::DpiInitParam& GetDpiInitParam() const;

private:
    /** Main window
    */
    ui::ControlPtrT<ui::Window> m_pMainWindow;

    /** Enable the DPI awareness feature setting parameter
    */
    ui::DpiInitParam m_dpiInitParam;
};

#endif // EXAMPLES_APP_H_
