#ifndef EXAMPLES_MAIN_TEST_APPLICATION_H_
#define EXAMPLES_MAIN_TEST_APPLICATION_H_

// duilib
#include "duilib/duilib.h"
#include <vector>

/** Main program entry
*/
class TestApplication
{
private:
    TestApplication();
    ~TestApplication();
    TestApplication(const TestApplication&) = delete;
    TestApplication& operator = (const TestApplication&) = delete;

public:
    /** Get the singleton object
    */
    static TestApplication& Instance();

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
    /** Main window list
    */
    std::vector<ui::ControlPtrT<ui::Window>> m_pMainWindows;

    /** Active window
    */
    ui::ControlPtrT<ui::Window> m_pActiveWindow;
};

#endif // EXAMPLES_MAIN_TEST_APPLICATION_H_
