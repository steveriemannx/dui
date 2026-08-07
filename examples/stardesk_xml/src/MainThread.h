#ifndef STARDESK_XML_MAIN_THREAD_H_
#define STARDESK_XML_MAIN_THREAD_H_

#include "dui/dui.h"

namespace sdk {

/** Application thread (XML mode): boots the framework, starts the shared
 *  StarDesk services and opens the XML main window. */
class MainThread : public ui::FrameworkThread
{
    typedef ui::FrameworkThread BaseClass;
public:
    MainThread();
    virtual ~MainThread() override;

    void Run() { RunMessageLoop(); }

private:
    virtual void OnInit() override;
    virtual void OnCleanup() override;
};

} // namespace sdk

#endif // STARDESK_XML_MAIN_THREAD_H_
