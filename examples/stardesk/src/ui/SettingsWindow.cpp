#include "SettingsWindow.h"

#include "../app/Language.h"
#include "../app/Theme.h"

#include <functional>
#include <vector>

namespace sdk {

using ui::UiRect;

SettingsWindow::SettingsWindow()
{
}

SettingsWindow::~SettingsWindow()
{
    App::Instance().RemoveListener(this);
}

DString SettingsWindow::GetSkinFolder()
{
    return _T("");
}

DString SettingsWindow::GetSkinFile()
{
    return _T("");
}

void SettingsWindow::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 420;
    attrs.m_szInitSize.cy = 300;
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

// ---------------------------------------------------------------- helpers

PillButton* SettingsWindow::MakePill(const char* key)
{
    PillButton* pill = new PillButton(this);
    pill->SetText(SDK_TR(key));
    return pill;
}

// ---------------------------------------------------------------- UI

void SettingsWindow::OnInitWindow()
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
    title->SetText(SDK_TR("settings.title"));
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

    // body: one option row per client setting
    ui::VBox* body = new ui::VBox(this);
    body->SetAttribute(_T("padding"), _T("16,14,16,14"));
    body->SetAttribute(_T("child_margin_y"), _T("10"));
    root->AddItem(body);

    const AppConfig& cfg = App::Instance().Config();

    // helper: one row of [label + pills], returns the created pills in order
    auto addPillRow = [this, body](ThemeLabel*& labelRef, const char* labelKey,
                                   const std::vector<const char*>& keys,
                                   int selectedIdx,
                                   const std::function<void(int)>& apply) {
        ui::HBox* row = new ui::HBox(this);
        row->SetAttribute(_T("height"), _T("30"));
        row->SetAttribute(_T("child_margin_x"), _T("6"));
        body->AddItem(row);

        ThemeLabel* label = new ThemeLabel(this);
        label->SetRole(ThemeLabel::Role::Sub);
        label->SetText(SDK_TR(labelKey));
        label->SetAttribute(_T("width"), _T("auto"));
        label->SetAttribute(_T("margin"), _T("0,0,8,0"));
        row->AddItem(label);
        labelRef = label;

        std::vector<PillButton*> pills;
        for (const char* key : keys) {
            PillButton* pill = MakePill(key);
            pill->SetAttribute(_T("width"), _T("64"));
            pill->SetAttribute(_T("height"), _T("28"));
            const int idx = (int)pills.size();
            pill->SetSelected(idx == selectedIdx);
            pill->AttachClick([apply, idx](const ui::EventArgs&) {
                apply(idx);
                return true;
            });
            row->AddItem(pill);
            pills.push_back(pill);
        }
        return pills;
    };

    auto applyMode = [this](int idx) {
        App::Instance().Config().viewOnly = (idx == 1);
        App::Instance().Config().Save();
        m_controlPill->SetSelected(idx == 0);
        m_viewOnlyPill->SetSelected(idx == 1);
    };
    auto applyRes = [this](int idx) {
        App::Instance().Config().wantRes = idx;
        App::Instance().Config().Save();
        m_resOriginalPill->SetSelected(idx == 0);
        m_res720Pill->SetSelected(idx == 1);
        m_res1080Pill->SetSelected(idx == 2);
    };
    auto applyFps = [this](int idx) {
        App::Instance().Config().wantFps = (idx == 0 ? 24 : (idx == 2 ? 60 : 30));
        App::Instance().Config().Save();
        m_fps24Pill->SetSelected(idx == 0);
        m_fps30Pill->SetSelected(idx == 1);
        m_fps60Pill->SetSelected(idx == 2);
    };

    // mode (control / view only)
    const std::vector<PillButton*> modePills =
        addPillRow(m_modeLabel, "client.mode",
                   { "client.modeControl", "client.modeView" },
                   cfg.viewOnly ? 1 : 0, applyMode);
    m_controlPill = modePills[0];
    m_viewOnlyPill = modePills[1];

    // resolution (original / 720p / 1080p)
    const std::vector<PillButton*> resPills =
        addPillRow(m_resLabel, "client.resolution",
                   { "client.resOriginal", "client.res720p", "client.res1080p" },
                   cfg.wantRes, applyRes);
    m_resOriginalPill = resPills[0];
    m_res720Pill = resPills[1];
    m_res1080Pill = resPills[2];

    // frame rate (24 / 30 / 60)
    const int fpsIdx = (cfg.wantFps == 24 ? 0 : (cfg.wantFps == 60 ? 2 : 1));
    const std::vector<PillButton*> fpsPills =
        addPillRow(m_fpsLabel, "client.fps",
                   { "client.fps24", "client.fps30", "client.fps60" },
                   fpsIdx, applyFps);
    m_fps24Pill = fpsPills[0];
    m_fps30Pill = fpsPills[1];
    m_fps60Pill = fpsPills[2];

    // done row
    ui::HBox* doneRow = new ui::HBox(this);
    doneRow->SetAttribute(_T("height"), _T("34"));
    doneRow->SetAttribute(_T("child_halign"), _T("right"));
    doneRow->SetAttribute(_T("child_margin_x"), _T("6"));
    body->AddItem(doneRow);

    AccentButton* doneBtn = new AccentButton(this);
    doneBtn->SetText(SDK_TR("common.ok"));
    doneBtn->SetAttribute(_T("width"), _T("80"));
    doneBtn->SetAttribute(_T("height"), _T("30"));
    doneBtn->AttachClick([this](const ui::EventArgs&) {
        CloseWnd();
        return true;
    });
    doneRow->AddItem(doneBtn);

    AttachBox(root);
    App::Instance().AddListener(this);

    ApplyLanguage();

    BaseClass::OnInitWindow();
}

void SettingsWindow::ApplyTheme()
{
    const Palette& p = Theme::Get();
    ui::VBox* root = dynamic_cast<ui::VBox*>(GetRoot());
    if (root != nullptr) {
        root->SetBkColor(p.windowBg);
    }
    InvalidateAll();
}

void SettingsWindow::ApplyLanguage()
{
    if (m_modeLabel == nullptr) {
        return;
    }
    m_modeLabel->SetText(SDK_TR("client.mode"));
    m_resLabel->SetText(SDK_TR("client.resolution"));
    m_fpsLabel->SetText(SDK_TR("client.fps"));
    m_controlPill->SetText(SDK_TR("client.modeControl"));
    m_viewOnlyPill->SetText(SDK_TR("client.modeView"));
    m_resOriginalPill->SetText(SDK_TR("client.resOriginal"));
    m_res720Pill->SetText(SDK_TR("client.res720p"));
    m_res1080Pill->SetText(SDK_TR("client.res1080p"));
    m_fps24Pill->SetText(SDK_TR("client.fps24"));
    m_fps30Pill->SetText(SDK_TR("client.fps30"));
    m_fps60Pill->SetText(SDK_TR("client.fps60"));
    InvalidateAll();
}

} // namespace sdk
