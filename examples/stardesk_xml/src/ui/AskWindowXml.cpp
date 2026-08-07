#include "AskWindowXml.h"

#include "examples/stardesk/src/app/Language.h"

namespace sdk {

AskWindowXml::AskWindowXml(const AskInfo& info, ResultCb cb)
    : m_info(info), m_cb(std::move(cb))
{
}

AskWindowXml::~AskWindowXml()
{
    if (!m_done && m_cb) {
        m_cb(false); // closed without a decision = reject
    }
}

DString AskWindowXml::GetSkinFolder()
{
    return _T("stardesk_xml");
}

DString AskWindowXml::GetSkinFile()
{
    return _T("ask.xml");
}

void AskWindowXml::OnInitWindow()
{
    ui::Label* text = dynamic_cast<ui::Label*>(FindControl(_T("ask_text")));
    if (text) {
        const DString mode = m_info.view ? SDK_TR("client.modeView") : SDK_TR("client.modeControl");
        const std::string fmt = ui::StringConvert::TToUTF8(SDK_TR("ask.text"));
        const std::string name = ui::StringConvert::TToUTF8(m_info.deviceName);
        const std::string ip = ui::StringConvert::TToUTF8(m_info.ip);
        const std::string mode8 = ui::StringConvert::TToUTF8(mode);
        const std::string result = ui::StringUtil::Printf(fmt.c_str(), name.c_str(),
                                                          ip.c_str(), mode8.c_str());
        text->SetText(ui::StringConvert::UTF8ToT(result));
    }
    ui::Button* reject = dynamic_cast<ui::Button*>(FindControl(_T("reject_btn")));
    if (reject) {
        reject->AttachClick([this](const ui::EventArgs&) {
            if (!m_done && m_cb) {
                m_cb(false);
            }
            m_done = true;
            CloseWnd();
            return true;
        });
    }
    ui::Button* accept = dynamic_cast<ui::Button*>(FindControl(_T("accept_btn")));
    if (accept) {
        accept->AttachClick([this](const ui::EventArgs&) {
            if (!m_done && m_cb) {
                m_cb(true);
            }
            m_done = true;
            CloseWnd();
            return true;
        });
    }
    BaseClass::OnInitWindow();
}

} // namespace sdk
