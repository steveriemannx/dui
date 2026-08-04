//WorkerThread.h
#ifndef EXAMPLES_WORKDER_THREAD_H_
#define EXAMPLES_WORKDER_THREAD_H_

// dui
#include "dui/dui.h"

class MainForm;

/** Worker thread
*/
class WorkerThread : public ui::FrameworkThread
{
public:
    /** Thread constructor
    @param [in] nThreadIdentifier Thread identifier; the thread identifier is needed for inter-thread communication, so it must be unique
    */
    explicit WorkerThread(int32_t nThreadIdentifier);
    virtual ~WorkerThread() override;

public:
    /** Set the interface to the main form, used to interact with the main form
    */
    void SetMainForm(MainForm* pMainForm);

private:
    /** Initialize before running; called before entering the message loop
    */
    virtual void OnInit() override;

    /** Clean up on exit; called after leaving the message loop
    */
    virtual void OnCleanup() override;

    /** Output a log
    */
    void PrintLog(const DString& log);

private:
    /** Interface to the main form
    */
    ui::ControlPtrT<MainForm> m_pMainForm;

};

#endif // EXAMPLES_WORKDER_THREAD_H_
