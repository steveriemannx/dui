#include "MainWindowXml.h"

#include "AskWindowXml.h"
#include "FileSendWindowXml.h"
#include "RemoteWindowXml.h"

#include "examples/stardesk/src/app/Theme.h"

#include "examples/stardesk/src/app/Language.h"
#include "examples/stardesk/src/net/Socket.h"

#include "dui/Utils/FilePathUtil.h"

#include <algorithm>
#include <cstdlib>

namespace sdk {

using ui::UiRect;

// ---------------------------------------------------------------- ctor

MainWindowXml::MainWindowXml()
{
}

MainWindowXml::~MainWindowXml()
{
    if (m_host) {
        m_host->Stop();
        m_host.reset();
    }
    if (m_client) {
        m_client->Stop();
        m_client.reset();
    }
    App::Instance().RemoveListener(this);
}

DString MainWindowXml::GetSkinFolder()
{
    return _T("stardesk_xml");
}

DString MainWindowXml::GetSkinFile()
{
    return _T("main.xml");
}

// ---------------------------------------------------------------- helpers

ui::RichEdit* MainWindowXml::GetEdit(const char* name)
{
    return dynamic_cast<ui::RichEdit*>(FindControl(ui::StringConvert::UTF8ToT(name)));
}

ui::Button* MainWindowXml::GetButton(const char* name)
{
    return dynamic_cast<ui::Button*>(FindControl(ui::StringConvert::UTF8ToT(name)));
}

ui::Label* MainWindowXml::GetLabel(const char* name)
{
    return dynamic_cast<ui::Label*>(FindControl(ui::StringConvert::UTF8ToT(name)));
}

ui::Combo* MainWindowXml::GetCombo(const char* name)
{
    return dynamic_cast<ui::Combo*>(FindControl(ui::StringConvert::UTF8ToT(name)));
}

ui::Option* MainWindowXml::GetOption(const char* name)
{
    return dynamic_cast<ui::Option*>(FindControl(ui::StringConvert::UTF8ToT(name)));
}

// ---------------------------------------------------------------- init

void MainWindowXml::OnInitWindow()
{
    // runtime style classes used by the XML (not in global.xml)
    AddClass(_T("card"),
             _T("bkcolor='#FFFFFFFF' border_size='1' border_color='#FFD9DEE6' border_round='6,6'"));
    AddClass(_T("edit"),
             _T("bkcolor='#FFFFFFFF' border_size='1' border_color='#FFD9DEE6' border_round='4,4' textcolor='#FF1F2329'"));

    const AppConfig& cfg = App::Instance().Config();

    // ---- combos ----
    auto fillCombo = [this](const char* name, const std::vector<DString>& items, int sel) {
        ui::Combo* combo = GetCombo(name);
        if (combo == nullptr) {
            return;
        }
        ui::TreeView* tree = combo->GetTreeView();
        if (tree == nullptr) {
            return;
        }
        ui::TreeNode* root = tree->GetRootNode();
        for (const DString& item : items) {
            ui::TreeNode* node = new ui::TreeNode(this);
            node->SetClass(_T("tree_node"));
            node->SetText(item);
            root->AddChildNode(node);
        }
        combo->SetCurSel(sel);
    };
    fillCombo("mode_combo", { SDK_TR("client.modeControl"), SDK_TR("client.modeView") }, 0);
    fillCombo("res_combo",
              { SDK_TR("client.resOriginal"), SDK_TR("client.res720p"), SDK_TR("client.res1080p") },
              cfg.wantRes);
    fillCombo("fps_combo", { _T("24"), _T("30"), _T("60") },
              cfg.wantFps == 24 ? 0 : (cfg.wantFps == 60 ? 2 : 1));

    // ---- fields ----
    ui::RichEdit* hostPwd = GetEdit("host_pwd");
    if (hostPwd) {
        hostPwd->SetText(cfg.password);
    }
    ui::RichEdit* portEdit = GetEdit("remote_port");
    if (portEdit) {
        portEdit->SetText(ui::StringUtil::Printf(_T("%d"), (int)cfg.port));
    }
    ui::Label* nameLabel = GetLabel("device_name");
    if (nameLabel) {
        nameLabel->SetText(cfg.deviceName);
    }

    ui::Option* optAuto = GetOption("opt_auto");
    ui::Option* optManual = GetOption("opt_manual");
    if (optAuto && optManual) {
        optAuto->Selected(!cfg.manualAccept);
        optManual->Selected(cfg.manualAccept);
        optAuto->AttachSelect([this](const ui::EventArgs&) {
            App::Instance().Config().manualAccept = false;
            App::Instance().Config().Save();
            return true;
        });
        optManual->AttachSelect([this](const ui::EventArgs&) {
            App::Instance().Config().manualAccept = true;
            App::Instance().Config().Save();
            return true;
        });
    }
    ui::Option* optMirror = GetOption("opt_mirror");
    ui::Option* optExtend = GetOption("opt_extend");
    if (optMirror && optExtend) {
        optMirror->Selected(!cfg.extendScreen);
        optExtend->Selected(cfg.extendScreen);
        optMirror->AttachSelect([this](const ui::EventArgs&) {
            App::Instance().Config().extendScreen = false;
            App::Instance().Config().Save();
            return true;
        });
        optExtend->AttachSelect([this](const ui::EventArgs&) {
            App::Instance().Config().extendScreen = true;
            App::Instance().Config().Save();
            return true;
        });
    }

    // ---- caption buttons ----
    ui::Button* langBtn = GetButton("langbtn");
    if (langBtn) {
        langBtn->SetText(Language::NativeName(Language::Get()));
        langBtn->AttachClick([this](const ui::EventArgs&) {
            CycleLanguage();
            return true;
        });
    }
    ui::Button* themeBtn = GetButton("themebtn");
    if (themeBtn) {
        themeBtn->SetText(Theme::IsDark() ? _T("☀️") : _T("🌙"));
        themeBtn->AttachClick([this](const ui::EventArgs&) {
            ToggleTheme();
            return true;
        });
    }

    // ---- password buttons ----
    ui::Button* genBtn = GetButton("genpwd_btn");
    if (genBtn) {
        genBtn->AttachClick([this](const ui::EventArgs&) {
            const DString pwd = AppConfig::GeneratePassword();
            ui::RichEdit* edit = GetEdit("host_pwd");
            if (edit) {
                edit->SetText(pwd);
            }
            App::Instance().Config().password = pwd;
            App::Instance().Config().Save();
            return true;
        });
    }
    ui::Button* showBtn = GetButton("showpwd_btn");
    if (showBtn) {
        showBtn->AttachClick([this](const ui::EventArgs&) {
            ui::RichEdit* edit = GetEdit("host_pwd");
            if (edit == nullptr) {
                return true;
            }
            m_pwdVisible = !m_pwdVisible;
            edit->SetAttribute(_T("password"), m_pwdVisible ? _T("false") : _T("true"));
            return true;
        });
    }

    // ---- tabs ----
    ui::Button* tabRemote = GetButton("tab_remote");
    ui::Button* tabFile = GetButton("tab_file");
    if (tabRemote) {
        tabRemote->AttachClick([this](const ui::EventArgs&) {
            SelectTab(true);
            return true;
        });
    }
    if (tabFile) {
        tabFile->AttachClick([this](const ui::EventArgs&) {
            SelectTab(false);
            return true;
        });
    }

    // ---- connect ----
    ui::Button* connectBtn = GetButton("connect_btn");
    if (connectBtn) {
        connectBtn->AttachClick([this](const ui::EventArgs&) {
            OnConnectClicked();
            return true;
        });
    }

    // ---- file tab ----
    ui::Button* sendBtn = GetButton("send_file_btn");
    if (sendBtn) {
        sendBtn->AttachClick([this](const ui::EventArgs&) {
            OnSendFileClicked();
            return true;
        });
    }
    ui::Button* openDirBtn = GetButton("open_dir_btn");
    if (openDirBtn) {
        openDirBtn->AttachClick([this](const ui::EventArgs&) {
            OnOpenReceiveDir();
            return true;
        });
    }
    ui::Label* receiveDir = GetLabel("receive_dir");
    if (receiveDir) {
        DString dir = cfg.receiveDir;
        if (dir.empty()) {
            dir = AppConfig::GetDefaultReceiveDir().ToString();
        }
        receiveDir->SetText(dir);
    }

    // ---- services ----
    App::Instance().AddListener(this);

    m_host = std::make_unique<HostService>();
    HostSession::Callbacks hostCb;
    hostCb.onStatus = [this](const DString& text, bool ok) {
        PostToUI(ui::UiBind(this, [this, text, ok]() { OnHostStatus(text, ok); }));
    };
    hostCb.onAsk = [this](HostSession* session, const AskInfo& info) {
        PostToUI(ui::UiBind(this, [this, session, info]() {
            if (session == nullptr) {
                return;
            }
            AskWindowXml* dlg = new AskWindowXml(info, [session](bool accept) {
                session->RespondAsk(accept);
            });
            dlg->CreateWnd(this, ui::WindowCreateParam(_T("StarDesk"), true));
            dlg->ShowWindow(ui::kSW_SHOW_NORMAL);
        }));
    };
    hostCb.onClosed = []() {};
    if (!m_host->Start(cfg.port, hostCb)) {
        SetHostStatus(SDK_TR("client.networkError"), false);
    }

    App::Instance().FileTx().SetItemCallback([this](const FileTransfer::Item& item) {
        PostToUI(ui::UiBind(this, [this, item]() { OnItemUpdated(item); }));
    });

    const std::string deviceName = ui::StringConvert::TToUTF8(cfg.deviceName);
    App::Instance().Disc().Start(deviceName, cfg.port,
        [this](const std::vector<Discovery::Device>& devices) {
            PostToUI(ui::UiBind(this, [this, devices]() {
                std::vector<std::pair<DString, DString>> rows;
                for (const Discovery::Device& d : devices) {
                    const DString ip = ui::StringConvert::UTF8ToT(d.ip);
                    const DString ipPort = ui::StringUtil::Printf(
                        _T("%s:%d"), ip.c_str(), (int)d.port);
                    rows.emplace_back(ui::StringConvert::UTF8ToT(d.name), ipPort);
                }
                SetLanDevices(rows);
            }));
        });

    std::vector<DString> ips;
    for (const std::string& ip : Socket::GetLocalIPv4s()) {
        ips.push_back(ui::StringConvert::UTF8ToT(ip));
    }
    SetLocalAddresses(ips);

    SetHostStatus(SDK_TR("host.ready"), true);
    SelectTab(true);

    BaseClass::OnInitWindow();
}

// ---------------------------------------------------------------- logic

void MainWindowXml::SelectTab(bool remote)
{
    ui::VBox* panelRemote = dynamic_cast<ui::VBox*>(FindControl(_T("panel_remote")));
    ui::VBox* panelFile = dynamic_cast<ui::VBox*>(FindControl(_T("panel_file")));
    ui::Button* tabRemote = GetButton("tab_remote");
    ui::Button* tabFile = GetButton("tab_file");
    if (panelRemote) {
        panelRemote->SetVisible(remote);
    }
    if (panelFile) {
        panelFile->SetVisible(!remote);
    }
    if (tabRemote) {
        tabRemote->SetClass(remote ? _T("btn_global_blue_80x30") : _T("btn_global_color_white"));
    }
    if (tabFile) {
        tabFile->SetClass(remote ? _T("btn_global_color_white") : _T("btn_global_blue_80x30"));
    }
}

void MainWindowXml::CycleLanguage()
{
    const int cur = (int)Language::Get();
    const int next = (cur + 1) % Language::Count();
    App::Instance().SetLanguage((Lang)next);
}

void MainWindowXml::ToggleTheme()
{
    App::Instance().SetTheme(!Theme::IsDark());
}

void MainWindowXml::OnConnectClicked()
{
    if (m_client) {
        m_client->Stop();
        m_client.reset();
        ui::Label* status = GetLabel("conn_status");
        if (status) {
            status->SetText(_T(""));
        }
        ui::Button* btn = GetButton("connect_btn");
        if (btn) {
            btn->SetText(SDK_TR("client.connect"));
        }
    }

    ui::RichEdit* ipEdit = GetEdit("remote_ip");
    ui::RichEdit* portEdit = GetEdit("remote_port");
    ui::RichEdit* pwdEdit = GetEdit("remote_pwd");
    ui::Combo* modeCombo = GetCombo("mode_combo");
    ui::Combo* resCombo = GetCombo("res_combo");
    ui::Combo* fpsCombo = GetCombo("fps_combo");
    if (ipEdit == nullptr || portEdit == nullptr || pwdEdit == nullptr) {
        return;
    }

    ClientSession::Options opt;
    opt.host = ui::StringConvert::TToUTF8(ipEdit->GetText());
    opt.port = (uint16_t)std::max(1, std::min(65535,
                std::atoi(ui::StringConvert::TToUTF8(portEdit->GetText()).c_str())));
    opt.password = pwdEdit->GetText();
    opt.mode = (modeCombo && modeCombo->GetCurSel() == 1) ? ConnMode::View : ConnMode::Control;
    opt.resCode = resCombo ? resCombo->GetCurSel() : 0;
    const int fpsSel = fpsCombo ? fpsCombo->GetCurSel() : 1;
    opt.fps = fpsSel == 0 ? 24 : (fpsSel == 2 ? 60 : 30);

    if (opt.host.empty()) {
        return;
    }

    ClientSession::Callbacks cb;
    cb.onState = [this](ClientSession::State st, const DString& text) {
        PostToUI(ui::UiBind(this, [this, st, text]() { OnClientState(st, text); }));
    };
    m_client = std::make_unique<ClientSession>(opt, std::move(cb));
    m_client->Start();
}

void MainWindowXml::OnClientState(ClientSession::State state, const DString& text)
{
    ui::Label* status = GetLabel("conn_status");
    ui::Button* btn = GetButton("connect_btn");
    if (state == ClientSession::State::Connected) {
        if (btn) {
            btn->SetText(SDK_TR("client.disconnect"));
        }
        if (status) {
            status->SetAttribute(_T("textcolor"), _T("green"));
            status->SetText(text);
        }
        if (m_client) {
            RemoteWindowXml* rw = new RemoteWindowXml(std::move(m_client));
            rw->SetClosedCallback([this]() {
                PostToUI(ui::UiBind(this, [this]() {
                    ui::Button* b = GetButton("connect_btn");
                    if (b) {
                        b->SetText(SDK_TR("client.connect"));
                    }
                    ui::Label* s = GetLabel("conn_status");
                    if (s) {
                        s->SetAttribute(_T("textcolor"), _T("gray"));
                        s->SetText(SDK_TR("client.remoteDisconnected"));
                    }
                }));
            });
            rw->CreateWnd(this, ui::WindowCreateParam(_T("StarDesk"), true));
            rw->ShowWindow(ui::kSW_SHOW_NORMAL);
        }
    }
    else if (state == ClientSession::State::Failed) {
        if (status) {
            status->SetAttribute(_T("textcolor"), _T("red"));
            status->SetText(text);
        }
        if (m_client) {
            m_client->Stop();
            m_client.reset();
        }
    }
    else if (state == ClientSession::State::Closed) {
        if (btn) {
            btn->SetText(SDK_TR("client.connect"));
        }
        if (status) {
            status->SetAttribute(_T("textcolor"), _T("gray"));
            status->SetText(text);
        }
    }
    else {
        if (status) {
            status->SetAttribute(_T("textcolor"), _T("gray"));
            status->SetText(text);
        }
    }
}

void MainWindowXml::OnHostStatus(const DString& text, bool ok)
{
    SetHostStatus(text, ok);
}

void MainWindowXml::SetHostStatus(const DString& text, bool ok)
{
    ui::Label* status = GetLabel("host_status");
    if (status) {
        status->SetAttribute(_T("textcolor"), ok ? _T("green") : _T("red"));
        status->SetText(text);
    }
}

void MainWindowXml::SetLocalAddresses(const std::vector<DString>& ips)
{
    ui::Label* addr = GetLabel("device_addr");
    if (addr == nullptr) {
        return;
    }
    DString text;
    for (size_t i = 0; i < ips.size(); ++i) {
        if (i > 0) {
            text += _T("\n");
        }
        text += ips[i];
        text += ui::StringUtil::Printf(_T(" : %d"), (int)App::Instance().Config().port);
    }
    if (text.empty()) {
        text = ui::StringUtil::Printf(_T("127.0.0.1 : %d"),
                                      (int)App::Instance().Config().port);
    }
    addr->SetText(text);
}

void MainWindowXml::SetLanDevices(const std::vector<std::pair<DString, DString>>& nameIpPorts)
{
    ui::VBox* list = dynamic_cast<ui::VBox*>(FindControl(_T("device_list")));
    if (list == nullptr) {
        return;
    }
    list->RemoveAllItems();
    if (nameIpPorts.empty()) {
        ui::Label* empty = new ui::Label(this);
        empty->SetClass(_T("listitem"));
        empty->SetText(SDK_TR("client.noDevices"));
        empty->SetFontId(_T("system_12"));
        empty->SetAttribute(_T("textcolor"), _T("gray"));
        empty->SetFixedHeight(ui::UiFixedInt(24), true, true);
        list->AddItem(empty);
        return;
    }
    for (const auto& nameIp : nameIpPorts) {
        ui::Label* item = new ui::Label(this);
        item->SetClass(_T("listitem"));
        item->SetText(nameIp.first + _T("   ") + nameIp.second);
        item->SetFontId(_T("system_12"));
        item->SetFixedHeight(ui::UiFixedInt(26), true, true);
        item->AttachClick([this, nameIp](const ui::EventArgs&) {
            const size_t colon = nameIp.second.find(_T(':'));
            ui::RichEdit* ip = GetEdit("remote_ip");
            ui::RichEdit* port = GetEdit("remote_port");
            if (ip && colon != DString::npos) {
                ip->SetText(nameIp.second.substr(0, colon));
            }
            if (port && colon != DString::npos) {
                port->SetText(nameIp.second.substr(colon + 1));
            }
            return true;
        });
        list->AddItem(item);
    }
}

// ---------------------------------------------------------------- file tab

void MainWindowXml::OnSendFileClicked()
{
    ui::Label* status = GetLabel("conn_status");
    if (m_client && m_client->IsConnected()) {
        FileSendWindowXml* w = new FileSendWindowXml(m_client->GetHost(),
                                                     m_client->FilePort(),
                                                     m_client->Token());
        w->CreateWnd(this, ui::WindowCreateParam(_T("StarDesk"), true));
        w->ShowWindow(ui::kSW_SHOW_NORMAL);
        return;
    }
    if (m_host) {
        HostSessionPtr s = m_host->FirstConnectedSession();
        if (s) {
            FileSendWindowXml* w = new FileSendWindowXml(
                s->PeerIp(), (uint16_t)(App::Instance().Config().port + 1), s->Token());
            w->CreateWnd(this, ui::WindowCreateParam(_T("StarDesk"), true));
            w->ShowWindow(ui::kSW_SHOW_NORMAL);
            return;
        }
    }
    if (status) {
        status->SetAttribute(_T("textcolor"), _T("gray"));
        status->SetText(SDK_TR("client.remoteDisconnected"));
    }
}

void MainWindowXml::OnItemUpdated(const FileTransfer::Item& item)
{
    m_transfers[item.id] = item;
    RebuildTransferList();
}

void MainWindowXml::RebuildTransferList()
{
    ui::VBox* list = dynamic_cast<ui::VBox*>(FindControl(_T("transfer_list")));
    if (list == nullptr) {
        return;
    }
    list->RemoveAllItems();
    size_t count = 0;
    for (const auto& kv : m_transfers) {
        if (count >= 8) {
            break;
        }
        const FileTransfer::Item& it = kv.second;
        DString text = ui::StringConvert::UTF8ToT(it.name);
        DString state;
        switch (it.state) {
        case FileTransfer::ItemState::Transferring:
            state = ui::StringUtil::Printf(_T("  %llu / %llu KB"),
                                           (unsigned long long)(it.transferred / 1024),
                                           (unsigned long long)(it.size / 1024));
            break;
        case FileTransfer::ItemState::Done:
            state = _T("  ") + SDK_TR("file.done");
            break;
        case FileTransfer::ItemState::Failed:
            state = _T("  ") + SDK_TR("file.failed");
            break;
        case FileTransfer::ItemState::Rejected:
            state = _T("  ") + SDK_TR("file.rejected");
            break;
        case FileTransfer::ItemState::Cancelled:
            state = _T("  ") + SDK_TR("file.cancelled");
            break;
        default:
            state = _T("  ") + SDK_TR("file.waiting");
            break;
        }
        ui::Label* row = new ui::Label(this);
        row->SetClass(_T("listitem"));
        row->SetText(text + state);
        row->SetFontId(_T("system_12"));
        row->SetFixedHeight(ui::UiFixedInt(26), true, true);
        list->AddItem(row);
        ++count;
    }
    if (count == 0) {
        ui::Label* empty = new ui::Label(this);
        empty->SetClass(_T("listitem"));
        empty->SetText(SDK_TR("file.empty"));
        empty->SetFontId(_T("system_12"));
        empty->SetAttribute(_T("textcolor"), _T("gray"));
        empty->SetFixedHeight(ui::UiFixedInt(22), true, true);
        list->AddItem(empty);
    }
}

void MainWindowXml::OnOpenReceiveDir()
{
    DString dir = App::Instance().Config().receiveDir;
    if (dir.empty()) {
        dir = AppConfig::GetDefaultReceiveDir().ToString();
    }
    ui::FilePathUtil::CreateDirectories(dir);
#ifdef DUI_BUILD_FOR_WIN
    const std::string cmd = "explorer \"" + ui::StringConvert::TToUTF8(dir) + "\"";
#else
    const std::string cmd = "open \"" + ui::StringConvert::TToUTF8(dir) + "\"";
#endif
    std::system(cmd.c_str());
}

// ---------------------------------------------------------------- theme / language

void MainWindowXml::ApplyTheme()
{
    // The XML skin uses global.xml colors; only the theme button icon flips.
    ui::Button* themeBtn = GetButton("themebtn");
    if (themeBtn) {
        themeBtn->SetText(Theme::IsDark() ? _T("☀️") : _T("🌙"));
    }
    InvalidateAll();
}

void MainWindowXml::ApplyLanguage()
{
    ui::Button* langBtn = GetButton("langbtn");
    if (langBtn) {
        langBtn->SetText(Language::NativeName(Language::Get()));
    }
    ui::Label* subtitle = GetLabel("caption_subtitle");
    if (subtitle) {
        subtitle->SetText(SDK_TR("app.subtitle"));
    }
    SetHostStatus(SDK_TR("host.ready"), true);
    ui::Button* sendBtn = GetButton("send_file_btn");
    if (sendBtn) {
        sendBtn->SetText(SDK_TR("file.title"));
    }
    ui::Button* tabRemote = GetButton("tab_remote");
    if (tabRemote) {
        tabRemote->SetText(SDK_TR("client.title"));
    }
    ui::Button* tabFile = GetButton("tab_file");
    if (tabFile) {
        tabFile->SetText(SDK_TR("file.title"));
    }
    ui::Button* connectBtn = GetButton("connect_btn");
    if (connectBtn && !(m_client && m_client->IsConnected())) {
        connectBtn->SetText(SDK_TR("client.connect"));
    }
    RebuildTransferList();
    InvalidateAll();
}

} // namespace sdk
