#ifndef STARDESK_XML_UI_ASK_WINDOW_H_
#define STARDESK_XML_UI_ASK_WINDOW_H_

#include "dui/dui.h"

#include "examples/stardesk/src/app/AppConfig.h"
#include "examples/stardesk/src/session/HostSession.h"

namespace sdk {

/** Manual-accept dialog, XML mode: skin in stardesk_xml/ask.xml. */
class AskWindowXml : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    typedef std::function<void(bool accept)> ResultCb;

    AskWindowXml(const AskInfo& info, ResultCb cb);
    virtual ~AskWindowXml() override;

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void OnInitWindow() override;

private:
    AskInfo m_info;
    ResultCb m_cb;
    bool m_done = false;
};

} // namespace sdk

#endif // STARDESK_XML_UI_ASK_WINDOW_H_
