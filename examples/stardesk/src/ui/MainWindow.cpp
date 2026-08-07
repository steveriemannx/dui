#include "MainWindow.h"

#include "../app/AppConfig.h"
#include "../app/Language.h"
#include "../app/Theme.h"
#include "../net/Socket.h"
#include "../ui/AskWindow.h"

#include "dui/Utils/FilePathUtil.h"
#include "../ui/FileSendWindow.h"
#include "../ui/RemoteWindow.h"

#include <algorithm>
#include <cstdlib>

namespace sdk {

using ui::UiRect;
using ui::UiRectF;

// ---------------------------------------------------------------- ctor

MainWindow::MainWindow()
{
}

MainWindow::~MainWindow()
{
    // stop services first: their threads are joined here, so no callbacks can
    // fire after this point
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

DString MainWindow::GetSkinFolder()
{
    return _T("");
}

DString MainWindow::GetSkinFile()
{
    // Pure code mode: no layout XML
    return _T("");
}

void MainWindow::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 960;
    attrs.m_szInitSize.cy = 640;
    // system shadow: non-layered window + shadow attached, like the XML
    // configuration layered_window="false" shadow_attached="true" (two shadow
    // implementations exist: layered -> transparent self-drawn shadow,
    // normal -> shadow drawn on the window surface)
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = false;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcSizeBox = UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;

    // shadow_type="default" (system rounded shadow), same lookup as XML
    // shadow_type="system_round": the OS provides the shadow, so there is no
    // self-drawn shadow border (corner is 0 -> window size stays as declared)
    ui::Shadow::ShadowType nShadowType = ui::Shadow::ShadowType::kShadowSystemRound;
    ui::UiSize szBorderRound;
    ui::UiPadding rcShadowCorner;
    DString shadowImage;
    ui::Shadow::GetShadowParam(nShadowType, szBorderRound, rcShadowCorner, shadowImage);
    attrs.m_rcShadowCorner = rcShadowCorner;
    BaseClass::GetCreateWindowAttributes(attrs);
}

// ---------------------------------------------------------------- helpers

ui::Label* MainWindow::MakeLabel(const char* key, ThemeLabel::Role role)
{
    ThemeLabel* label = new ThemeLabel(this);
    label->SetRole(role);
    label->SetText(SDK_TR(key));
    return label;
}

ui::RichEdit* MainWindow::MakeEdit(const char* promptKey, bool password)
{
    ui::RichEdit* edit = new ui::RichEdit(this);
    edit->SetAttribute(_T("prompt_text"), SDK_TR(promptKey));
    edit->SetAttribute(_T("prompt_color"), Theme::Hex(Theme::Get().textSub));
    edit->SetAttribute(_T("single_line"), _T("true"));
    edit->SetAttribute(_T("font"), _T("system_12"));
    if (password) {
        edit->SetAttribute(_T("password"), _T("true"));
    }
    SetEditBk(edit);
    m_edits.push_back(edit);
    return edit;
}

void MainWindow::SetEditBk(ui::RichEdit* edit)
{
    const Palette& p = Theme::Get();
    edit->SetBkColor(p.inputBg);
    edit->SetAttribute(_T("border_size"), _T("1"));
    edit->SetAttribute(_T("border_color"), Theme::Hex(p.border));
    edit->SetAttribute(_T("border_round"), _T("4,4"));
    edit->SetAttribute(_T("textcolor"), Theme::Hex(p.textMain));
}

PillButton* MainWindow::MakePill(const char* key)
{
    PillButton* pill = new PillButton(this);
    pill->SetText(SDK_TR(key));
    return pill;
}

IconButton* MainWindow::MakeIconButton(VectorArt::Icon icon, const char* tooltipKey,
                                       const DString& name)
{
    IconButton* btn = new IconButton(this);
    btn->SetIcon(icon);
    btn->SetIconToolTip(SDK_TR(tooltipKey));
    btn->SetName(name);
    return btn;
}

// ---------------------------------------------------------------- window UI

void MainWindow::OnInitWindow()
{
    // pure-code windows must apply the caption/size-box rects themselves
    // (XML mode does this in WindowBuilder; attrs.m_rcCaption is not applied)
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    // system shadow (like shadow_type="system_round" in XML)
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemRound);

    const Palette& p = Theme::Get();

    // ---- root ----
    ui::VBox* root = new ui::VBox(this);
    root->SetBkColor(p.windowBg);
    root->SetAttribute(_T("width"), _T("stretch"));
    root->SetAttribute(_T("height"), _T("stretch"));

    // ---- caption bar ----
    ui::HBox* caption = new ui::HBox(this);
    caption->SetAttribute(_T("name"), _T("window_caption_bar"));
    caption->SetAttribute(_T("height"), _T("36"));
    caption->SetBkColor(p.captionBg);
    root->AddItem(caption);

    // logo (drawn control)
    class LogoControl : public ui::Control {
        typedef ui::Control BaseClass;
    public:
        explicit LogoControl(ui::Window* w) : BaseClass(w) { SetMouseEnabled(false); }
        virtual void Paint(ui::IRender* render, const UiRect& rc) override {
            BaseClass::Paint(render, rc);
            VectorArt::DrawLogo(render, GetRect(), Theme::IsDark());
        }
    };
    LogoControl* logo = new LogoControl(this);
    logo->SetAttribute(_T("width"), _T("28"));
    logo->SetAttribute(_T("height"), _T("28"));
    logo->SetAttribute(_T("margin"), _T("8,4,4,4"));
    caption->AddItem(logo);

    ThemeLabel* title = new ThemeLabel(this);
    title->SetRole(ThemeLabel::Role::Main);
    title->SetText(_T("StarDesk"));
    title->SetAttribute(_T("font"), _T("system_bold_14"));
    title->SetAttribute(_T("width"), _T("auto"));
    title->SetAttribute(_T("margin"), _T("2,0,0,0"));
    caption->AddItem(title);

    ThemeLabel* subtitle = new ThemeLabel(this);
    subtitle->SetRole(ThemeLabel::Role::Sub);
    subtitle->SetText(SDK_TR("app.subtitle"));
    subtitle->SetAttribute(_T("font"), _T("system_12"));
    subtitle->SetAttribute(_T("width"), _T("auto"));
    subtitle->SetAttribute(_T("margin"), _T("10,0,0,0"));
    caption->AddItem(subtitle);

    // spacer
    ui::Control* spacer = new ui::Control(this);
    spacer->SetMouseEnabled(false);
    caption->AddItem(spacer);

    // language cycle button (shows the current language name)
    m_langBtn = new PillButton(this);
    m_langBtn->SetText(Language::NativeName(Language::Get()));
    m_langBtn->SetAttribute(_T("width"), _T("80"));
    m_langBtn->SetAttribute(_T("height"), _T("26"));
    m_langBtn->SetAttribute(_T("margin"), _T("0,5,4,5"));
    m_langBtn->AttachClick([this](const ui::EventArgs&) {
        CycleLanguage();
        return true;
    });
    caption->AddItem(m_langBtn);

    // theme toggle
    m_themeBtn = MakeIconButton(Theme::IsDark() ? VectorArt::Icon::Moon : VectorArt::Icon::Sun,
                                "settings.theme", _T(""));
    m_themeBtn->SetAttribute(_T("width"), _T("30"));
    m_themeBtn->SetAttribute(_T("height"), _T("30"));
    m_themeBtn->SetAttribute(_T("margin"), _T("0,3,0,3"));
    m_themeBtn->AttachClick([this](const ui::EventArgs&) {
        App::Instance().SetTheme(!Theme::IsDark());
        return true;
    });
    caption->AddItem(m_themeBtn);

    // min / max / restore / close (names drive dui's built-in caption behavior)
    IconButton* minBtn = MakeIconButton(VectorArt::Icon::Min, "window.min", _T("minbtn"));
    minBtn->SetAttribute(_T("width"), _T("34"));
    minBtn->SetAttribute(_T("height"), _T("30"));
    minBtn->SetAttribute(_T("margin"), _T("0,3,0,3"));
    caption->AddItem(minBtn);

    IconButton* maxBtn = MakeIconButton(VectorArt::Icon::Max, "window.max", _T("maxbtn"));
    maxBtn->SetAttribute(_T("width"), _T("34"));
    maxBtn->SetAttribute(_T("height"), _T("30"));
    maxBtn->SetAttribute(_T("margin"), _T("0,3,0,3"));
    caption->AddItem(maxBtn);

    IconButton* restoreBtn = MakeIconButton(VectorArt::Icon::Restore, "window.restore", _T("restorebtn"));
    restoreBtn->SetAttribute(_T("width"), _T("34"));
    restoreBtn->SetAttribute(_T("height"), _T("30"));
    restoreBtn->SetAttribute(_T("margin"), _T("0,3,0,3"));
    restoreBtn->SetVisible(false);
    caption->AddItem(restoreBtn);

    IconButton* closeBtn = MakeIconButton(VectorArt::Icon::Close, "window.close", _T("closebtn"));
    closeBtn->SetAttribute(_T("width"), _T("34"));
    closeBtn->SetAttribute(_T("height"), _T("30"));
    closeBtn->SetAttribute(_T("margin"), _T("0,3,4,3"));
    caption->AddItem(closeBtn);

    // ---- body ----
    ui::HBox* body = new ui::HBox(this);
    root->AddItem(body);

    // ================= left sidebar =================
    m_sidebar = new ui::VBox(this);
    m_sidebar->SetAttribute(_T("width"), _T("288"));
    m_sidebar->SetAttribute(_T("padding"), _T("12,12,12,12"));
    m_sidebar->SetAttribute(_T("child_margin_y"), _T("10"));
    m_sidebar->SetBkColor(p.panelBg);
    body->AddItem(m_sidebar);

    // -- device card --
    CardBox* cardDevice = new CardBox(this);
    cardDevice->SetAttribute(_T("padding"), _T("12,10,12,12"));
    cardDevice->SetAttribute(_T("child_margin_y"), _T("6"));
    m_sidebar->AddItem(cardDevice);

    ThemeLabel* t1 = static_cast<ThemeLabel*>(MakeLabel("host.device", ThemeLabel::Role::Title));
    cardDevice->AddItem(t1);

    m_deviceNameLabel = static_cast<ThemeLabel*>(MakeLabel("", ThemeLabel::Role::Main));
    m_deviceNameLabel->SetText(App::Instance().Config().deviceName);
    m_deviceNameLabel->SetAttribute(_T("font"), _T("system_bold_12"));
    m_deviceNameLabel->SetAttribute(_T("width"), _T("stretch"));
    cardDevice->AddItem(m_deviceNameLabel);

    m_addrLabel = static_cast<ThemeLabel*>(MakeLabel("", ThemeLabel::Role::Sub));
    m_addrLabel->SetText(ui::StringUtil::Printf(_T("%d"), (int)App::Instance().Config().port));
    m_addrLabel->SetAttribute(_T("width"), _T("stretch"));
    m_addrLabel->SetAttribute(_T("text_align"), _T("left,top"));
    m_addrLabel->SetAttribute(_T("multi_line"), _T("true"));
    cardDevice->AddItem(m_addrLabel);

    // -- password card --
    CardBox* cardPassword = new CardBox(this);
    cardPassword->SetAttribute(_T("padding"), _T("12,10,12,12"));
    cardPassword->SetAttribute(_T("child_margin_y"), _T("6"));
    m_sidebar->AddItem(cardPassword);

    cardPassword->AddItem(MakeLabel("host.password", ThemeLabel::Role::Title));

    ui::HBox* pwdRow = new ui::HBox(this);
    pwdRow->SetAttribute(_T("child_margin_x"), _T("4"));
    cardPassword->AddItem(pwdRow);

    m_hostPwdEdit = MakeEdit("host.password", true);
    m_hostPwdEdit->SetText(App::Instance().Config().password);
    m_hostPwdEdit->SetAttribute(_T("height"), _T("28"));
    pwdRow->AddItem(m_hostPwdEdit);

    IconButton* genBtn = MakeIconButton(VectorArt::Icon::Random, "host.genPassword", _T(""));
    genBtn->SetAttribute(_T("width"), _T("28"));
    genBtn->SetAttribute(_T("height"), _T("28"));
    genBtn->AttachClick([this](const ui::EventArgs&) {
        const DString pwd = AppConfig::GeneratePassword();
        m_hostPwdEdit->SetText(pwd);
        App::Instance().Config().password = pwd;
        App::Instance().Config().Save();
        return true;
    });
    pwdRow->AddItem(genBtn);

    m_pwdShowBtn = MakeIconButton(VectorArt::Icon::Eye, "host.showPassword", _T(""));
    m_pwdShowBtn->SetAttribute(_T("width"), _T("28"));
    m_pwdShowBtn->SetAttribute(_T("height"), _T("28"));
    m_pwdShowBtn->AttachClick([this](const ui::EventArgs&) {
        m_pwdVisible = !m_pwdVisible;
        m_hostPwdEdit->SetAttribute(_T("password"), m_pwdVisible ? _T("false") : _T("true"));
        m_pwdShowBtn->SetIconToolTip(SDK_TR(m_pwdVisible ? "host.hidePassword" : "host.showPassword"));
        return true;
    });
    pwdRow->AddItem(m_pwdShowBtn);

    // -- accept mode card --
    CardBox* cardAccept = new CardBox(this);
    cardAccept->SetAttribute(_T("padding"), _T("12,10,12,12"));
    cardAccept->SetAttribute(_T("child_margin_y"), _T("6"));
    m_sidebar->AddItem(cardAccept);

    cardAccept->AddItem(MakeLabel("host.acceptMode", ThemeLabel::Role::Title));

    ui::HBox* acceptRow = new ui::HBox(this);
    acceptRow->SetAttribute(_T("child_margin_x"), _T("6"));
    cardAccept->AddItem(acceptRow);

    m_autoAcceptPill = MakePill("host.autoAccept");
    m_autoAcceptPill->SetAttribute(_T("height"), _T("26"));
    acceptRow->AddItem(m_autoAcceptPill);

    m_manualAcceptPill = MakePill("host.manualAccept");
    m_manualAcceptPill->SetAttribute(_T("height"), _T("26"));
    acceptRow->AddItem(m_manualAcceptPill);

    const bool manual = App::Instance().Config().manualAccept;
    m_autoAcceptPill->SetSelected(!manual);
    m_manualAcceptPill->SetSelected(manual);

    auto selectAccept = [this](bool manual) {
        App::Instance().Config().manualAccept = manual;
        App::Instance().Config().Save();
        m_autoAcceptPill->SetSelected(!manual);
        m_manualAcceptPill->SetSelected(manual);
    };
    m_autoAcceptPill->AttachClick([selectAccept](const ui::EventArgs&) {
        selectAccept(false);
        return true;
    });
    m_manualAcceptPill->AttachClick([selectAccept](const ui::EventArgs&) {
        selectAccept(true);
        return true;
    });

    // -- share mode card --
    CardBox* cardShare = new CardBox(this);
    cardShare->SetAttribute(_T("padding"), _T("12,10,12,12"));
    cardShare->SetAttribute(_T("child_margin_y"), _T("6"));
    m_sidebar->AddItem(cardShare);

    cardShare->AddItem(MakeLabel("host.shareMode", ThemeLabel::Role::Title));

    ui::HBox* shareRow = new ui::HBox(this);
    shareRow->SetAttribute(_T("child_margin_x"), _T("6"));
    cardShare->AddItem(shareRow);

    m_mirrorPill = MakePill("host.shareMirror");
    m_mirrorPill->SetAttribute(_T("height"), _T("26"));
    shareRow->AddItem(m_mirrorPill);

    m_extendPill = MakePill("host.shareExtend");
    m_extendPill->SetAttribute(_T("height"), _T("26"));
    shareRow->AddItem(m_extendPill);

    const bool extend = App::Instance().Config().extendScreen;
    m_mirrorPill->SetSelected(!extend);
    m_extendPill->SetSelected(extend);

    auto selectShare = [this](bool extend) {
        App::Instance().Config().extendScreen = extend;
        App::Instance().Config().Save();
        m_mirrorPill->SetSelected(!extend);
        m_extendPill->SetSelected(extend);
    };
    m_mirrorPill->AttachClick([selectShare](const ui::EventArgs&) {
        selectShare(false);
        return true;
    });
    m_extendPill->AttachClick([selectShare](const ui::EventArgs&) {
        selectShare(true);
        return true;
    });

    // -- host status card --
    CardBox* cardStatus = new CardBox(this);
    cardStatus->SetAttribute(_T("padding"), _T("12,10,12,12"));
    cardStatus->SetAttribute(_T("child_margin_y"), _T("6"));
    m_sidebar->AddItem(cardStatus);

    cardStatus->AddItem(MakeLabel("host.status", ThemeLabel::Role::Title));

    m_hostStatusLabel = static_cast<ThemeLabel*>(MakeLabel("", ThemeLabel::Role::Success));
    m_hostStatusLabel->SetText(SDK_TR("host.ready"));
    m_hostStatusLabel->SetAttribute(_T("width"), _T("stretch"));
    m_hostStatusLabel->SetAttribute(_T("text_align"), _T("left,top"));
    m_hostStatusLabel->SetAttribute(_T("multi_line"), _T("true"));
    cardStatus->AddItem(m_hostStatusLabel);

    // ================= right side =================
    ui::VBox* mainArea = new ui::VBox(this);
    mainArea->SetAttribute(_T("padding"), _T("12,8,12,12"));
    mainArea->SetAttribute(_T("child_margin_y"), _T("8"));
    body->AddItem(mainArea);

    // -- tabs --
    ui::HBox* tabRow = new ui::HBox(this);
    tabRow->SetAttribute(_T("child_margin_x"), _T("6"));
    mainArea->AddItem(tabRow);

    m_tabRemote = MakePill("client.title");
    m_tabRemote->SetAttribute(_T("width"), _T("96"));
    m_tabRemote->SetAttribute(_T("height"), _T("28"));
    tabRow->AddItem(m_tabRemote);

    m_tabFile = MakePill("file.title");
    m_tabFile->SetAttribute(_T("width"), _T("96"));
    m_tabFile->SetAttribute(_T("height"), _T("28"));
    tabRow->AddItem(m_tabFile);

    m_tabRemote->AttachClick([this](const ui::EventArgs&) {
        SelectTab(true);
        return true;
    });
    m_tabFile->AttachClick([this](const ui::EventArgs&) {
        SelectTab(false);
        return true;
    });

    // ---- remote tab ----
    m_panelRemote = new ui::VBox(this);
    m_panelRemote->SetAttribute(_T("child_margin_y"), _T("10"));
    mainArea->AddItem(m_panelRemote);

    // LAN devices card
    CardBox* cardDevices = new CardBox(this);
    cardDevices->SetAttribute(_T("padding"), _T("12,10,12,12"));
    cardDevices->SetAttribute(_T("child_margin_y"), _T("4"));
    m_panelRemote->AddItem(cardDevices);

    cardDevices->AddItem(MakeLabel("client.devices", ThemeLabel::Role::Title));

    m_deviceList = new ui::VBox(this);
    m_deviceList->SetAttribute(_T("child_margin_y"), _T("4"));
    cardDevices->AddItem(m_deviceList);

    m_deviceEmptyLabel = static_cast<ThemeLabel*>(MakeLabel("file.empty", ThemeLabel::Role::Sub));
    m_deviceEmptyLabel->SetText(SDK_TR("file.empty"));
    m_deviceEmptyLabel->SetAttribute(_T("height"), _T("24"));
    m_deviceList->AddItem(m_deviceEmptyLabel);

    // connect settings card
    CardBox* cardConn = new CardBox(this);
    cardConn->SetAttribute(_T("padding"), _T("12,10,12,12"));
    cardConn->SetAttribute(_T("child_margin_y"), _T("8"));
    m_panelRemote->AddItem(cardConn);

    cardConn->AddItem(MakeLabel("client.title", ThemeLabel::Role::Title));

    auto makeRow = [this](CardBox* parent) {
        ui::HBox* row = new ui::HBox(this);
        row->SetAttribute(_T("child_margin_x"), _T("6"));
        parent->AddItem(row);
        return row;
    };
    auto makeFieldLabel = [this](const char* key) {
        ThemeLabel* l = static_cast<ThemeLabel*>(MakeLabel(key, ThemeLabel::Role::Sub));
        l->SetAttribute(_T("width"), _T("auto"));
        l->SetAttribute(_T("margin"), _T("0,0,2,0"));
        return l;
    };

    // IP + port row
    ui::HBox* row1 = makeRow(cardConn);
    row1->AddItem(makeFieldLabel("client.ip"));
    m_remoteIpEdit = MakeEdit("client.ip", false);
    m_remoteIpEdit->SetText(_T("127.0.0.1"));
    m_remoteIpEdit->SetAttribute(_T("height"), _T("28"));
    row1->AddItem(m_remoteIpEdit);

    row1->AddItem(makeFieldLabel("client.port"));
    m_remotePortEdit = MakeEdit("client.port", false);
    m_remotePortEdit->SetText(ui::StringUtil::Printf(_T("%d"), (int)App::Instance().Config().port));
    m_remotePortEdit->SetAttribute(_T("width"), _T("70"));
    m_remotePortEdit->SetAttribute(_T("height"), _T("28"));
    row1->AddItem(m_remotePortEdit);

    // password row
    ui::HBox* row2 = makeRow(cardConn);
    row2->AddItem(makeFieldLabel("client.password"));
    m_remotePwdEdit = MakeEdit("client.password", true);
    m_remotePwdEdit->SetAttribute(_T("height"), _T("28"));
    row2->AddItem(m_remotePwdEdit);

    // mode row
    ui::HBox* row3 = makeRow(cardConn);
    row3->AddItem(makeFieldLabel("client.mode"));
    m_controlPill = MakePill("client.modeControl");
    m_controlPill->SetAttribute(_T("width"), _T("64"));
    m_controlPill->SetAttribute(_T("height"), _T("26"));
    row3->AddItem(m_controlPill);
    m_viewPill = MakePill("client.modeView");
    m_viewPill->SetAttribute(_T("width"), _T("64"));
    m_viewPill->SetAttribute(_T("height"), _T("26"));
    row3->AddItem(m_viewPill);
    m_controlPill->SetSelected(true);

    auto selectMode = [this](bool view) {
        m_controlPill->SetSelected(!view);
        m_viewPill->SetSelected(view);
    };
    m_controlPill->AttachClick([selectMode](const ui::EventArgs&) {
        selectMode(false);
        return true;
    });
    m_viewPill->AttachClick([selectMode](const ui::EventArgs&) {
        selectMode(true);
        return true;
    });

    // resolution row
    ui::HBox* row4 = makeRow(cardConn);
    row4->AddItem(makeFieldLabel("client.resolution"));
    auto makeResPill = [this, &row4](const char* key, int idx) {
        PillButton* pill = MakePill(key);
        pill->SetAttribute(_T("width"), _T("60"));
        pill->SetAttribute(_T("height"), _T("26"));
        row4->AddItem(pill);
        return pill;
    };
    m_resOriginalPill = makeResPill("client.resOriginal", 0);
    m_res720Pill = makeResPill("client.res720p", 1);
    m_res1080Pill = makeResPill("client.res1080p", 2);

    auto selectRes = [this](int idx) {
        m_resOriginalPill->SetSelected(idx == 0);
        m_res720Pill->SetSelected(idx == 1);
        m_res1080Pill->SetSelected(idx == 2);
    };
    const int defRes = App::Instance().Config().wantRes;
    selectRes(defRes);
    m_resOriginalPill->AttachClick([selectRes](const ui::EventArgs&) { selectRes(0); return true; });
    m_res720Pill->AttachClick([selectRes](const ui::EventArgs&) { selectRes(1); return true; });
    m_res1080Pill->AttachClick([selectRes](const ui::EventArgs&) { selectRes(2); return true; });

    // fps row
    ui::HBox* row5 = makeRow(cardConn);
    row5->AddItem(makeFieldLabel("client.fps"));
    auto makeFpsPill = [this, &row5](const char* key) {
        PillButton* pill = MakePill(key);
        pill->SetAttribute(_T("width"), _T("60"));
        pill->SetAttribute(_T("height"), _T("26"));
        row5->AddItem(pill);
        return pill;
    };
    m_fps24Pill = makeFpsPill("client.fps24");
    m_fps30Pill = makeFpsPill("client.fps30");
    m_fps60Pill = makeFpsPill("client.fps60");

    auto selectFps = [this](int idx) {
        m_fps24Pill->SetSelected(idx == 0);
        m_fps30Pill->SetSelected(idx == 1);
        m_fps60Pill->SetSelected(idx == 2);
    };
    const int defFps = App::Instance().Config().wantFps;
    selectFps(defFps == 24 ? 0 : (defFps == 60 ? 2 : 1));
    m_fps24Pill->AttachClick([selectFps](const ui::EventArgs&) { selectFps(0); return true; });
    m_fps30Pill->AttachClick([selectFps](const ui::EventArgs&) { selectFps(1); return true; });
    m_fps60Pill->AttachClick([selectFps](const ui::EventArgs&) { selectFps(2); return true; });

    // connect row
    ui::HBox* row6 = makeRow(cardConn);
    m_connectBtn = new AccentButton(this);
    m_connectBtn->SetText(SDK_TR("client.connect"));
    m_connectBtn->SetAttribute(_T("width"), _T("110"));
    m_connectBtn->SetAttribute(_T("height"), _T("32"));
    m_connectBtn->AttachClick([this](const ui::EventArgs&) {
        OnConnectClicked();
        return true;
    });
    row6->AddItem(m_connectBtn);

    m_connStatusLabel = static_cast<ThemeLabel*>(MakeLabel("", ThemeLabel::Role::Sub));
    m_connStatusLabel->SetText(_T(""));
    m_connStatusLabel->SetAttribute(_T("margin"), _T("6,0,0,0"));
    m_connStatusLabel->SetAttribute(_T("text_align"), _T("left,top"));
    m_connStatusLabel->SetAttribute(_T("multi_line"), _T("true"));
    row6->AddItem(m_connStatusLabel);

    // ---- file tab ----
    m_panelFile = new ui::VBox(this);
    m_panelFile->SetAttribute(_T("child_margin_y"), _T("10"));
    m_panelFile->SetVisible(false);
    mainArea->AddItem(m_panelFile);

    // send card
    CardBox* cardSend = new CardBox(this);
    cardSend->SetAttribute(_T("padding"), _T("12,14,12,14"));
    cardSend->SetAttribute(_T("child_margin_y"), _T("8"));
    m_panelFile->AddItem(cardSend);

    m_sendFileBtn = new AccentButton(this);
    m_sendFileBtn->SetText(SDK_TR("file.title"));
    m_sendFileBtn->SetAttribute(_T("width"), _T("140"));
    m_sendFileBtn->SetAttribute(_T("height"), _T("34"));
    m_sendFileBtn->SetAttribute(_T("halign"), _T("left"));
    m_sendFileBtn->AttachClick([this](const ui::EventArgs&) {
        OnSendFileClicked();
        return true;
    });
    cardSend->AddItem(m_sendFileBtn);

    ThemeLabel* hint = static_cast<ThemeLabel*>(MakeLabel("file.dragHint", ThemeLabel::Role::Sub));
    hint->SetAttribute(_T("height"), _T("20"));
    cardSend->AddItem(hint);

    // receive card
    CardBox* cardReceive = new CardBox(this);
    cardReceive->SetAttribute(_T("padding"), _T("12,10,12,12"));
    cardReceive->SetAttribute(_T("child_margin_y"), _T("6"));
    m_panelFile->AddItem(cardReceive);

    ui::HBox* dirRow = new ui::HBox(this);
    dirRow->SetAttribute(_T("child_margin_x"), _T("6"));
    cardReceive->AddItem(dirRow);

    ThemeLabel* dirTitle = static_cast<ThemeLabel*>(MakeLabel("host.fileSaveDir", ThemeLabel::Role::Title));
    dirTitle->SetAttribute(_T("width"), _T("auto"));
    dirRow->AddItem(dirTitle);

    ui::Control* dirSpacer = new ui::Control(this);
    dirSpacer->SetMouseEnabled(false);
    dirRow->AddItem(dirSpacer);

    PillButton* openDirBtn = new PillButton(this);
    openDirBtn->SetText(SDK_TR("file.openDir"));
    openDirBtn->SetAttribute(_T("width"), _T("80"));
    openDirBtn->SetAttribute(_T("height"), _T("24"));
    openDirBtn->AttachClick([this](const ui::EventArgs&) {
        OnOpenReceiveDir();
        return true;
    });
    dirRow->AddItem(openDirBtn);

    m_receiveDirLabel = static_cast<ThemeLabel*>(MakeLabel("", ThemeLabel::Role::Sub));
    DString receiveDir = App::Instance().Config().receiveDir;
    if (receiveDir.empty()) {
        receiveDir = AppConfig::GetDefaultReceiveDir().ToString();
    }
    m_receiveDirLabel->SetText(receiveDir);
    m_receiveDirLabel->SetAttribute(_T("width"), _T("stretch"));
    m_receiveDirLabel->SetAttribute(_T("text_align"), _T("left,top"));
    m_receiveDirLabel->SetAttribute(_T("multi_line"), _T("true"));
    cardReceive->AddItem(m_receiveDirLabel);

    ThemeLabel* transferTitle = static_cast<ThemeLabel*>(MakeLabel("file.status", ThemeLabel::Role::Title));
    cardReceive->AddItem(transferTitle);

    m_transferList = new ui::VBox(this);
    m_transferList->SetAttribute(_T("child_margin_y"), _T("4"));
    cardReceive->AddItem(m_transferList);

    m_transferEmptyLabel = static_cast<ThemeLabel*>(MakeLabel("file.empty", ThemeLabel::Role::Sub));
    m_transferEmptyLabel->SetText(SDK_TR("file.empty"));
    m_transferEmptyLabel->SetAttribute(_T("height"), _T("22"));
    m_transferList->AddItem(m_transferEmptyLabel);

    AttachBox(root);

    App::Instance().AddListener(this);
    SelectTab(true);

    // ---- host service (always listening) ----
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
            AskWindow* dlg = new AskWindow(info, [session](bool accept) {
                session->RespondAsk(accept);
            });
            dlg->CreateWnd(this, ui::WindowCreateParam(_T("StarDesk"), true));
            dlg->ShowWindow(ui::kSW_SHOW_NORMAL);
        }));
    };
    hostCb.onClosed = []() {
        // host status is restored by onStatus in HostSession::Run
    };
    if (!m_host->Start(App::Instance().Config().port, hostCb)) {
        SetHostStatus(SDK_TR("client.networkError"), false);
    }
    // global file-transfer progress -> transfer list
    App::Instance().FileTx().SetItemCallback([this](const FileTransfer::Item& item) {
        PostToUI(ui::UiBind(this, [this, item]() { OnItemUpdated(item); }));
    });

    // LAN discovery: beacon + device list
    const std::string deviceName =
        ui::StringConvert::TToUTF8(App::Instance().Config().deviceName);
    App::Instance().Disc().Start(deviceName, App::Instance().Config().port,
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

    // local IPs
    std::vector<DString> ips;
    for (const std::string& ip : Socket::GetLocalIPv4s()) {
        ips.push_back(ui::StringConvert::UTF8ToT(ip));
    }
    SetLocalAddresses(ips);

    BaseClass::OnInitWindow();
}

// ---------------------------------------------------------------- logic

void MainWindow::SelectTab(bool remote)
{
    m_tabRemote->SetSelected(remote);
    m_tabFile->SetSelected(!remote);
    m_panelRemote->SetVisible(remote);
    m_panelFile->SetVisible(!remote);
}

void MainWindow::CycleLanguage()
{
    const int cur = (int)Language::Get();
    const int next = (cur + 1) % Language::Count();
    App::Instance().SetLanguage((Lang)next);
}

void MainWindow::OnConnectClicked()
{
    // stop any previous session (connected or in progress); the remote window
    // (added in the screen step) provides the dedicated disconnect button
    if (m_client) {
        m_client->Stop();
        m_client.reset();
        m_connStatusLabel->SetText(_T(""));
        m_connectBtn->SetText(SDK_TR("client.connect"));
    }

    ClientSession::Options opt;
    opt.host = ui::StringConvert::TToUTF8(m_remoteIpEdit->GetText());
    opt.port = (uint16_t)std::max(1, std::min(65535,
                std::atoi(ui::StringConvert::TToUTF8(m_remotePortEdit->GetText()).c_str())));
    opt.password = m_remotePwdEdit->GetText();
    opt.mode = m_viewPill->IsSelected() ? ConnMode::View : ConnMode::Control;
    opt.resCode = m_res1080Pill->IsSelected() ? 2 : (m_res720Pill->IsSelected() ? 1 : 0);
    opt.fps = m_fps60Pill->IsSelected() ? 60 : (m_fps24Pill->IsSelected() ? 24 : 30);

    if (opt.host.empty() || opt.port == 0) {
        m_connStatusLabel->SetText(SDK_TR("client.networkError"));
        return;
    }

    ClientSession::Callbacks cb;
    cb.onState = [this](ClientSession::State st, const DString& text) {
        PostToUI(ui::UiBind(this, [this, st, text]() { OnClientState(st, text); }));
    };

    m_client = std::make_unique<ClientSession>(opt, std::move(cb));
    m_client->Start();
}

void MainWindow::OnClientState(ClientSession::State state, const DString& text)
{
    if (state == ClientSession::State::Connected) {
        m_connectBtn->SetText(SDK_TR("client.disconnect"));
        m_connStatusLabel->SetRole(ThemeLabel::Role::Success);
        m_connStatusLabel->SetText(text);

        // hand the session to a remote window; re-create the client session
        // state so the main window can connect again later
        if (m_client) {
            RemoteWindow* rw = new RemoteWindow(std::move(m_client));
            rw->SetClosedCallback([this]() {
                PostToUI(ui::UiBind(this, [this]() {
                    m_connectBtn->SetText(SDK_TR("client.connect"));
                    m_connStatusLabel->SetRole(ThemeLabel::Role::Sub);
                    m_connStatusLabel->SetText(SDK_TR("client.remoteDisconnected"));
                }));
            });
            rw->CreateWnd(this, ui::WindowCreateParam(_T("StarDesk"), true));
            rw->ShowWindow(ui::kSW_SHOW_NORMAL);
        }
    }
    else if (state == ClientSession::State::Failed) {
        m_connStatusLabel->SetRole(ThemeLabel::Role::Danger);
        m_connStatusLabel->SetText(text);
        if (m_client) {
            m_client->Stop();
            m_client.reset();
        }
    }
    else if (state == ClientSession::State::Closed) {
        m_connStatusLabel->SetRole(ThemeLabel::Role::Sub);
        m_connStatusLabel->SetText(text);
        m_connectBtn->SetText(SDK_TR("client.connect"));
    }
    else {
        m_connStatusLabel->SetRole(ThemeLabel::Role::Sub);
        m_connStatusLabel->SetText(text);
    }
}

void MainWindow::OnHostStatus(const DString& text, bool ok)
{
    SetHostStatus(text, ok);
}

// ---------------------------------------------------------------- file tab

void MainWindow::OnSendFileClicked()
{
    // client side: session to the peer; host side: first connected session
    if (m_client && m_client->IsConnected()) {
        FileSendWindow* w = new FileSendWindow(m_client->GetHost(), m_client->FilePort(),
                                               m_client->Token());
        w->CreateWnd(this, ui::WindowCreateParam(_T("StarDesk"), true));
        w->ShowWindow(ui::kSW_SHOW_NORMAL);
        return;
    }
    if (m_host) {
        HostSessionPtr s = m_host->FirstConnectedSession();
        if (s) {
            FileSendWindow* w = new FileSendWindow(s->PeerIp(),
                                                   (uint16_t)(App::Instance().Config().port + 1),
                                                   s->Token());
            w->CreateWnd(this, ui::WindowCreateParam(_T("StarDesk"), true));
            w->ShowWindow(ui::kSW_SHOW_NORMAL);
            return;
        }
    }
    m_connStatusLabel->SetRole(ThemeLabel::Role::Sub);
    m_connStatusLabel->SetText(SDK_TR("client.remoteDisconnected"));
}

void MainWindow::OnItemUpdated(const FileTransfer::Item& item)
{
    m_transfers[item.id] = item;
    // rebuild the list lazily (few transfers; simple and correct)
    if (m_transferList == nullptr) {
        return;
    }
    ClearTransferRows();
    size_t count = 0;
    for (const auto& kv : m_transfers) {
        if (count >= 8) {
            break;
        }
        const FileTransfer::Item& it = kv.second;
        CardBox* row = new CardBox(this);
        row->SetAttribute(_T("height"), _T("34"));
        row->SetAttribute(_T("padding"), _T("10,4,10,4"));
        m_transferList->AddItem(row);

        ui::VBox* info = new ui::VBox(this);
        info->SetAttribute(_T("child_margin_y"), _T("1"));
        row->AddItem(info);

        ThemeLabel* name = new ThemeLabel(this);
        name->SetRole(ThemeLabel::Role::Main);
        name->SetText(ui::StringConvert::UTF8ToT(it.name));
        name->SetAttribute(_T("width"), _T("stretch"));
        name->SetAttribute(_T("font"), _T("system_12"));
        info->AddItem(name);

        ThemeLabel* status = new ThemeLabel(this);
        ThemeLabel::Role role = ThemeLabel::Role::Sub;
        DString text;
        switch (it.state) {
        case FileTransfer::ItemState::Transferring:
            text = ui::StringUtil::Printf(_T("%llu / %llu KB"),
                                          (unsigned long long)(it.transferred / 1024),
                                          (unsigned long long)(it.size / 1024));
            break;
        case FileTransfer::ItemState::Done:
            role = ThemeLabel::Role::Success;
            text = SDK_TR("file.done");
            break;
        case FileTransfer::ItemState::Failed:
            role = ThemeLabel::Role::Danger;
            text = SDK_TR("file.failed");
            break;
        case FileTransfer::ItemState::Rejected:
            role = ThemeLabel::Role::Danger;
            text = SDK_TR("file.rejected");
            break;
        case FileTransfer::ItemState::Cancelled:
            text = SDK_TR("file.cancelled");
            break;
        default:
            text = SDK_TR("file.waiting");
            break;
        }
        status->SetRole(role);
        status->SetText(text);
        status->SetAttribute(_T("width"), _T("stretch"));
        info->AddItem(status);

        row->AddItem(info);
        ++count;
    }
    if (count == 0) {
        m_transferEmptyLabel = static_cast<ThemeLabel*>(MakeLabel("file.empty", ThemeLabel::Role::Sub));
        m_transferEmptyLabel->SetText(SDK_TR("file.empty"));
        m_transferEmptyLabel->SetAttribute(_T("height"), _T("22"));
        m_transferList->AddItem(m_transferEmptyLabel);
    }
    InvalidateAll();
}

void MainWindow::ClearTransferRows()
{
    if (m_transferList == nullptr) {
        return;
    }
    m_transferList->RemoveAllItems();
    m_transferEmptyLabel = nullptr;
}

void MainWindow::RebuildTransferList()
{
    if (m_transferList == nullptr) {
        return;
    }
    ClearTransferRows();
    // re-render from the stored map (language switch etc.)
    size_t count = 0;
    for (const auto& kv : m_transfers) {
        if (count >= 8) {
            break;
        }
        CardBox* row = new CardBox(this);
        row->SetAttribute(_T("height"), _T("34"));
        row->SetAttribute(_T("padding"), _T("10,4,10,4"));
        m_transferList->AddItem(row);
        ThemeLabel* name = new ThemeLabel(this);
        name->SetRole(ThemeLabel::Role::Main);
        name->SetText(ui::StringConvert::UTF8ToT(kv.second.name));
        name->SetAttribute(_T("width"), _T("stretch"));
        name->SetAttribute(_T("font"), _T("system_12"));
        row->AddItem(name);
        ++count;
    }
    if (count == 0) {
        m_transferEmptyLabel = static_cast<ThemeLabel*>(MakeLabel("file.empty", ThemeLabel::Role::Sub));
        m_transferEmptyLabel->SetText(SDK_TR("file.empty"));
        m_transferEmptyLabel->SetAttribute(_T("height"), _T("22"));
        m_transferList->AddItem(m_transferEmptyLabel);
    }
}

void MainWindow::OnOpenReceiveDir()
{
    DString dir = App::Instance().Config().receiveDir;
    if (dir.empty()) {
        dir = AppConfig::GetDefaultReceiveDir().ToString();
    }
    ui::FilePathUtil::CreateDirectories(dir);
#ifdef DUI_BUILD_FOR_WIN
    const std::string cmd = "explorer "" + ui::StringConvert::TToUTF8(dir) + """;
#else
    const std::string cmd = "open "" + ui::StringConvert::TToUTF8(dir) + """;
#endif
    std::system(cmd.c_str());
}

void MainWindow::SetHostStatus(const DString& text, bool ok)
{
    m_hostStatusLabel->SetRole(ok ? ThemeLabel::Role::Success : ThemeLabel::Role::Danger);
    m_hostStatusLabel->SetText(text);
}

void MainWindow::SetLocalAddresses(const std::vector<DString>& ips)
{
    DString text;
    for (size_t i = 0; i < ips.size(); ++i) {
        if (i > 0) {
            text += _T("\n");
        }
        text += ips[i];
        text += ui::StringUtil::Printf(_T(" : %d"), (int)App::Instance().Config().port);
    }
    if (text.empty()) {
        text = ui::StringUtil::Printf(_T("127.0.0.1 : %d"), (int)App::Instance().Config().port);
    }
    m_addrLabel->SetText(text);
}

void MainWindow::SetLanDevices(const std::vector<std::pair<DString, DString>>& nameIpPorts)
{
    // rebuild the device row list
    m_deviceList->RemoveAllItems();
    m_deviceEmptyLabel = nullptr;

    for (const auto& nameIp : nameIpPorts) {
        CardBox* row = new CardBox(this);
        row->SetAttribute(_T("height"), _T("30"));
        row->SetAttribute(_T("padding"), _T("10,0,10,0"));

        ThemeLabel* l = new ThemeLabel(this);
        l->SetRole(ThemeLabel::Role::Main);
        l->SetText(nameIp.first);
        l->SetAttribute(_T("width"), _T("stretch"));
        row->AddItem(l);

        ThemeLabel* r = new ThemeLabel(this);
        r->SetRole(ThemeLabel::Role::Sub);
        r->SetText(nameIp.second);
        r->SetAttribute(_T("width"), _T("auto"));
        row->AddItem(r);

        row->AttachClick([this, nameIp](const ui::EventArgs&) {
            // "ip:port" -> fill both fields
            const size_t colon = nameIp.second.find(_T(':'));
            m_remoteIpEdit->SetText(nameIp.second.substr(0, colon));
            if (colon != DString::npos) {
                m_remotePortEdit->SetText(nameIp.second.substr(colon + 1));
            }
            return true;
        });
        m_deviceList->AddItem(row);
    }

    if (nameIpPorts.empty()) {
        m_deviceEmptyLabel = static_cast<ThemeLabel*>(MakeLabel("client.noDevices", ThemeLabel::Role::Sub));
        m_deviceEmptyLabel->SetText(SDK_TR("client.noDevices"));
        m_deviceEmptyLabel->SetAttribute(_T("height"), _T("24"));
        m_deviceList->AddItem(m_deviceEmptyLabel);
    }
}

// ---------------------------------------------------------------- theme / language

void MainWindow::ApplyThemeEdits()
{
    for (ui::RichEdit* edit : m_edits) {
        SetEditBk(edit);
    }
}

void MainWindow::ApplyTheme()
{
    const Palette& p = Theme::Get();
    ui::VBox* root = dynamic_cast<ui::VBox*>(GetRoot());
    if (root != nullptr) {
        root->SetBkColor(p.windowBg);
    }
    if (m_sidebar != nullptr) {
        m_sidebar->SetBkColor(p.panelBg);
    }
    ApplyThemeEdits();
    m_themeBtn->SetIcon(Theme::IsDark() ? VectorArt::Icon::Moon : VectorArt::Icon::Sun);
    InvalidateAll();
}

void MainWindow::ApplyLanguage()
{
    m_langBtn->SetText(Language::NativeName(Language::Get()));

    m_tabRemote->SetText(SDK_TR("client.title"));
    m_tabFile->SetText(SDK_TR("file.title"));
    m_autoAcceptPill->SetText(SDK_TR("host.autoAccept"));
    m_manualAcceptPill->SetText(SDK_TR("host.manualAccept"));
    m_mirrorPill->SetText(SDK_TR("host.shareMirror"));
    m_extendPill->SetText(SDK_TR("host.shareExtend"));
    m_controlPill->SetText(SDK_TR("client.modeControl"));
    m_viewPill->SetText(SDK_TR("client.modeView"));
    m_resOriginalPill->SetText(SDK_TR("client.resOriginal"));
    m_res720Pill->SetText(SDK_TR("client.res720p"));
    m_res1080Pill->SetText(SDK_TR("client.res1080p"));
    m_fps24Pill->SetText(SDK_TR("client.fps24"));
    m_fps30Pill->SetText(SDK_TR("client.fps30"));
    m_fps60Pill->SetText(SDK_TR("client.fps60"));
    m_connectBtn->SetText(SDK_TR("client.connect"));

    // status lines (re-render with the current language)
    SetHostStatus(SDK_TR("host.ready"), true);

    // prompt texts per field
    m_remoteIpEdit->SetAttribute(_T("prompt_text"), SDK_TR("client.ip"));
    m_remotePortEdit->SetAttribute(_T("prompt_text"), SDK_TR("client.port"));
    m_remotePwdEdit->SetAttribute(_T("prompt_text"), SDK_TR("client.password"));
    m_hostPwdEdit->SetAttribute(_T("prompt_text"), SDK_TR("host.password"));

    InvalidateAll();
}

void MainWindow::OnThemeChanged()
{
    ApplyTheme();
}

void MainWindow::OnLanguageChanged()
{
    ApplyLanguage();
}

} // namespace sdk
