#ifndef EXAMPLES_MAIN_TEST_APPLICATION_H_
#define EXAMPLES_MAIN_TEST_APPLICATION_H_

// duilib
#include "duilib/duilib.h"

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
    /** Main window
    */
    ui::ControlPtrT<ui::Window> m_pMainWindow;
};

#endif // EXAMPLES_MAIN_TEST_APPLICATION_H_
