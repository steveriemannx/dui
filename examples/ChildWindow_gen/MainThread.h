#ifndef EXAMPLES_MAIN_THREAD_H_
#define EXAMPLES_MAIN_THREAD_H_

// duilib
#include "duilib/duilib.h"

class MainForm;
/** Main thread
*/
class MainThread : public ui::FrameworkThread
{
public:
    MainThread();
    virtual ~MainThread() override;

private:
    /** Initialize before running; called before entering the message loop
    */
    virtual void OnInit() override;

    /** Clean up on exit; called after leaving the message loop
    */
    virtual void OnCleanup() override;

    /** Main thread: the message loop is in the Idle state
    */
    virtual void OnMessageLoopIdle()  override;

private:
    /** Main window
    */
    ui::ControlPtrT<MainForm> m_pMainForm;
};

#endif // EXAMPLES_MAIN_THREAD_H_
