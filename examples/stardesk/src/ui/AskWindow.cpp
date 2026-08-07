#include "AskWindow.h"

#include "../app/Language.h"
#include "../app/Theme.h"

namespace sdk {

using ui::UiRect;
using ui::UiRectF;

AskWindow::AskWindow(const AskInfo& info, ResultCb cb)
    : m_info(info), m_cb(std::move(cb))
{
}

AskWindow::~AskWindow()
{
    App::Instance().RemoveListener(this);
    if (!m_done && m_cb) {
        // window closed without a decision = reject
        m_cb(false);
    }
}

DString AskWindow::GetSkinFolder()
{
    return _T("");
}

DString AskWindow::GetSkinFile()
{
    return _T("");
}

void AskWindow::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 380;
    attrs.m_szInitSize.cy = 190;
    // system shadow: non-layered window + shadow attached, like the XML
    // configuration layered_window="false" shadow_attached="true" (two shadow
    // implementations exist: layered -> transparent self-drawn shadow,
    // normal -> shadow drawn on the window surface)
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = false;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcCaption = UiRect(0, 0, 0, 32);
    attrs.m_bCaptionDefined = true;
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

void AskWindow::OnInitWindow()
{
    // pure-code windows must apply the caption/size-box rects themselves
    // (XML mode does this in WindowBuilder; attrs.m_rcCaption is not applied)
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 32), true);
    // system shadow (like shadow_type="system_round" in XML)
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemRound);

    const Palette& p = Theme::Get();

    ui::VBox* root = new ui::VBox(this);
    root->SetBkColor(p.windowBg);
    root->SetAttribute(_T("width"), _T("stretch"));
    root->SetAttribute(_T("height"), _T("stretch"));

    // caption
    ui::HBox* caption = new ui::HBox(this);
    caption->SetAttribute(_T("height"), _T("32"));
    caption->SetBkColor(p.captionBg);
    root->AddItem(caption);

    ThemeLabel* title = new ThemeLabel(this);
    title->SetRole(ThemeLabel::Role::Main);
    title->SetText(SDK_TR("ask.title"));
    title->SetAttribute(_T("font"), _T("system_bold_12"));
    title->SetAttribute(_T("width"), _T("auto"));
    title->SetAttribute(_T("margin"), _T("12,0,0,0"));
    caption->AddItem(title);

    ui::Control* spacer = new ui::Control(this);
    spacer->SetMouseEnabled(false);
    caption->AddItem(spacer);

    IconButton* closeBtn = new IconButton(this);
    closeBtn->SetIcon(VectorArt::Icon::Close);
    closeBtn->SetIconToolTip(SDK_TR("window.close"));
    closeBtn->SetName(_T("closebtn"));
    closeBtn->SetAttribute(_T("width"), _T("28"));
    closeBtn->SetAttribute(_T("height"), _T("26"));
    closeBtn->SetAttribute(_T("margin"), _T("0,3,4,3"));
    caption->AddItem(closeBtn);

    // body
    ui::VBox* body = new ui::VBox(this);
    body->SetAttribute(_T("padding"), _T("16,14,16,14"));
    body->SetAttribute(_T("child_margin_y"), _T("12"));
    root->AddItem(body);

    m_textLabel = new ThemeLabel(this);
    m_textLabel->SetRole(ThemeLabel::Role::Main);
    m_textLabel->SetAttribute(_T("text_align"), _T("left,top"));
    m_textLabel->SetAttribute(_T("multi_line"), _T("true"));
    m_textLabel->SetAttribute(_T("width"), _T("stretch"));
    m_textLabel->SetAttribute(_T("height"), _T("auto"));
    body->AddItem(m_textLabel);

    ui::HBox* btnRow = new ui::HBox(this);
    btnRow->SetAttribute(_T("child_halign"), _T("right"));
    btnRow->SetAttribute(_T("child_margin_x"), _T("8"));
    body->AddItem(btnRow);

    PillButton* rejectBtn = new PillButton(this);
    rejectBtn->SetText(SDK_TR("ask.reject"));
    rejectBtn->SetAttribute(_T("width"), _T("80"));
    rejectBtn->SetAttribute(_T("height"), _T("30"));
    rejectBtn->AttachClick([this](const ui::EventArgs&) {
        if (!m_done && m_cb) m_cb(false);
        m_done = true;
        CloseWnd();
        return true;
    });
    btnRow->AddItem(rejectBtn);

    AccentButton* acceptBtn = new AccentButton(this);
    acceptBtn->SetText(SDK_TR("ask.accept"));
    acceptBtn->SetAttribute(_T("width"), _T("80"));
    acceptBtn->SetAttribute(_T("height"), _T("30"));
    acceptBtn->AttachClick([this](const ui::EventArgs&) {
        if (!m_done && m_cb) m_cb(true);
        m_done = true;
        CloseWnd();
        return true;
    });
    btnRow->AddItem(acceptBtn);

    AttachBox(root);
    App::Instance().AddListener(this);

    ApplyLanguage();

    BaseClass::OnInitWindow();
}

void AskWindow::ApplyTheme()
{
    const Palette& p = Theme::Get();
    ui::VBox* root = dynamic_cast<ui::VBox*>(GetRoot());
    if (root != nullptr) {
        root->SetBkColor(p.windowBg);
    }
    InvalidateAll();
}

void AskWindow::ApplyLanguage()
{
    if (m_textLabel == nullptr) {
        return;
    }
    // "设备名(ip) 请求控制/观看 本机"
    const DString mode = m_info.view ? SDK_TR("client.modeView") : SDK_TR("client.modeControl");
    const std::string fmt = ui::StringConvert::TToUTF8(SDK_TR("ask.text"));
    const std::string name = ui::StringConvert::TToUTF8(m_info.deviceName);
    const std::string ip = ui::StringConvert::TToUTF8(m_info.ip);
    const std::string mode8 = ui::StringConvert::TToUTF8(mode);
    const std::string text = ui::StringUtil::Printf(fmt.c_str(), name.c_str(), ip.c_str(), mode8.c_str());
    m_textLabel->SetText(ui::StringConvert::UTF8ToT(text));
    InvalidateAll();
}

} // namespace sdk
