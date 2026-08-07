#ifndef STARDESK_XML_UI_MAIN_WINDOW_H_
#define STARDESK_XML_UI_MAIN_WINDOW_H_

#include "dui/dui.h"

#include "examples/stardesk/src/app/AppConfig.h"
#include "examples/stardesk/src/session/ClientSession.h"
#include "examples/stardesk/src/session/HostSession.h"

namespace sdk {

/** StarDesk main window, XML mode: skin in stardesk_xml/main.xml.
 *  Same feature code as the pure-code version; controls come from the XML.
 */
class MainWindowXml : public ui::WindowImplBase, public IAppListener
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainWindowXml();
    virtual ~MainWindowXml() override;

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void OnInitWindow() override;

    virtual void OnThemeChanged() override { ApplyTheme(); }
    virtual void OnLanguageChanged() override { ApplyLanguage(); }

    void ApplyTheme();
    void ApplyLanguage();
    void SetHostStatus(const DString& text, bool ok);
    void SetLocalAddresses(const std::vector<DString>& ips);
    void SetLanDevices(const std::vector<std::pair<DString, DString>>& nameIpPorts);

private:
    // ---- logic (mirrors the pure-code main window) ----
    void SelectTab(bool remote);
    void OnConnectClicked();
    void OnClientState(ClientSession::State state, const DString& text);
    void OnHostStatus(const DString& text, bool ok);
    void OnSendFileClicked();
    void OnItemUpdated(const FileTransfer::Item& item);
    void OnOpenReceiveDir();
    void RebuildTransferList();
    void CycleLanguage();
    void ToggleTheme();

    // helpers
    ui::RichEdit* GetEdit(const char* name);
    ui::Button* GetButton(const char* name);
    ui::Label* GetLabel(const char* name);
    ui::Combo* GetCombo(const char* name);
    ui::Option* GetOption(const char* name);

    std::unique_ptr<HostService> m_host;
    std::unique_ptr<ClientSession> m_client;
    std::map<uint32_t, FileTransfer::Item> m_transfers;
    bool m_pwdVisible = false;
};

} // namespace sdk

#endif // STARDESK_XML_UI_MAIN_WINDOW_H_
