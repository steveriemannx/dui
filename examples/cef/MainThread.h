#ifndef EXAMPLES_MAIN_THREAD_H_
#define EXAMPLES_MAIN_THREAD_H_

// duilib
#include "duilib/duilib.h"

/** Main thread
*/
class MainThread : public ui::FrameworkThread
{
public:
    MainThread();
    virtual ~MainThread() override;

    //Enable the DPI awareness feature setting parameter
    const ui::DpiInitParam& GetDpiInitParam() const;

private:
    /** Initialize before running, called before entering the message loop
    */
    virtual void OnInit() override;

    /** Clean up on exit, called after exiting the message loop
    */
    virtual void OnCleanup() override;

private:
    //Enable the DPI awareness feature setting parameter
    ui::DpiInitParam m_dpiInitParam;
};

#endif // EXAMPLES_MAIN_THREAD_H_
