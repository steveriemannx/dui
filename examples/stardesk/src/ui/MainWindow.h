#ifndef STARDESK_UI_MAIN_WINDOW_H_
#define STARDESK_UI_MAIN_WINDOW_H_

#include "dui/dui.h"

#include "../app/AppConfig.h"
#include "../session/ClientSession.h"
#include "../session/HostSession.h"
#include "VectorArt.h"

namespace sdk {

/** StarDesk main window: host panel (left) + remote-control / file tabs (right).
 *  Pure code UI; every control is built in OnInitWindow().
 */
class MainWindow : public ui::WindowImplBase, public IAppListener
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainWindow();
    virtual ~MainWindow() override;

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs) override;
    virtual void OnInitWindow() override;

    // IAppListener (UI thread)
    virtual void OnThemeChanged() override;
    virtual void OnLanguageChanged() override;

    /** Re-apply theme colors to the few controls that cache them. */
    void ApplyTheme();
    /** Re-apply all UI strings in the current language. */
    void ApplyLanguage();

    /** Set the host status line (e.g. "ready", "device X connected"). */
    void SetHostStatus(const DString& text, bool ok);

    /** Local IPs + port shown in the device card (filled by the network layer). */
    void SetLocalAddresses(const std::vector<DString>& ips);

    /** Refresh the LAN device discovery list. */
    void SetLanDevices(const std::vector<std::pair<DString, DString>>& nameIpPorts);

private:
    // ---- construction helpers ----
    ui::Label* MakeLabel(const char* key, ThemeLabel::Role role);
    ui::RichEdit* MakeEdit(const char* promptKey, bool password);
    PillButton* MakePill(const char* key);
    IconButton* MakeIconButton(VectorArt::Icon icon, const char* tooltipKey,
                               const DString& name);

    // ---- UI logic ----
    void SelectTab(bool remote);
    void OnConnectClicked();
    void OnClientState(ClientSession::State state, const DString& text);
    void OnHostStatus(const DString& text, bool ok);

    // ---- file tab ----
    void OnSendFileClicked();
    void OnItemUpdated(const FileTransfer::Item& item);
    void OnOpenReceiveDir();
    void RebuildTransferList();
    void ClearTransferRows();

    void ApplyThemeEdits();
    void SetEditBk(ui::RichEdit* edit);

    // caption
    IconButton* m_themeBtn = nullptr;
    PillButton* m_langBtn = nullptr;

    // sidebar
    ui::VBox* m_sidebar = nullptr;
    ThemeLabel* m_deviceNameLabel = nullptr;
    ThemeLabel* m_addrLabel = nullptr;
    ui::RichEdit* m_hostPwdEdit = nullptr;
    IconButton* m_pwdShowBtn = nullptr;
    bool m_pwdVisible = false;
    PillButton* m_autoAcceptPill = nullptr;
    PillButton* m_manualAcceptPill = nullptr;
    PillButton* m_mirrorPill = nullptr;
    PillButton* m_extendPill = nullptr;
    ThemeLabel* m_hostStatusLabel = nullptr;

    // right side
    PillButton* m_tabRemote = nullptr;
    PillButton* m_tabFile = nullptr;
    ui::VBox* m_panelRemote = nullptr;
    ui::VBox* m_panelFile = nullptr;

    // connect panel
    ui::VBox* m_deviceList = nullptr;
    ThemeLabel* m_deviceEmptyLabel = nullptr;
    ui::RichEdit* m_remoteIpEdit = nullptr;
    ui::RichEdit* m_remotePortEdit = nullptr;
    ui::RichEdit* m_remotePwdEdit = nullptr;
    PillButton* m_controlPill = nullptr;
    PillButton* m_viewPill = nullptr;
    PillButton* m_resOriginalPill = nullptr;
    PillButton* m_res720Pill = nullptr;
    PillButton* m_res1080Pill = nullptr;
    PillButton* m_fps24Pill = nullptr;
    PillButton* m_fps30Pill = nullptr;
    PillButton* m_fps60Pill = nullptr;
    AccentButton* m_connectBtn = nullptr;
    ThemeLabel* m_connStatusLabel = nullptr;

    // file tab
    ThemeLabel* m_receiveDirLabel = nullptr;
    AccentButton* m_sendFileBtn = nullptr;
    ui::VBox* m_transferList = nullptr;
    ThemeLabel* m_transferEmptyLabel = nullptr;
    std::map<uint32_t, FileTransfer::Item> m_transfers;

    // language cycling support
    void CycleLanguage();

    // services
    std::unique_ptr<HostService> m_host;
    std::unique_ptr<ClientSession> m_client;

    // list of edits needing theme background refresh
    std::vector<ui::RichEdit*> m_edits;
};

} // namespace sdk

#endif // STARDESK_UI_MAIN_WINDOW_H_
