#include "RemoteWindow.h"

#include "../app/Language.h"
#include "../app/Theme.h"
#include "../platform/InputInject.h"
#include "FileSendWindow.h"

namespace sdk {

using ui::UiRect;
using ui::UiRectF;

namespace {

// 16x16 arrow cursor (1 = black, 2 = white outline, 0 = transparent)
const uint8_t kCursorMask[16][16] = {
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,2,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,2,0,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,2,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,2,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,2,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,2,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,2,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,2,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,2,0,0,0,0,0},
    {1,1,1,2,1,1,1,1,1,1,1,2,0,0,0,0},
    {1,1,2,0,1,1,1,1,1,1,2,0,2,0,0,0},
    {1,2,0,0,1,1,1,1,1,2,0,0,2,2,0,0},
    {1,0,0,0,2,1,1,1,2,0,0,0,0,2,0,0},
    {0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0},
};

void DrawCursor(uint8_t* rgba, int w, int h, int cx, int cy)
{
    if (cx < 0 || cy < 0) {
        return;
    }
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            const int px = cx + x;
            const int py = cy + y;
            if (px < 0 || px >= w || py < 0 || py >= h) {
                continue;
            }
            const uint8_t m = kCursorMask[y][x];
            if (m == 0) {
                continue;
            }
            uint8_t* p = &rgba[(size_t)(py * w + px) * 4];
            if (m == 1) {
                p[0] = 0;
                p[1] = 0;
                p[2] = 0;
                p[3] = 255;
            }
            else {
                p[0] = 255;
                p[1] = 255;
                p[2] = 255;
                p[3] = 255;
            }
        }
    }
}

} // namespace

// ---------------------------------------------------------------- ctor

RemoteWindow::RemoteWindow(std::unique_ptr<ClientSession> session)
    : m_session(std::move(session))
{
}

RemoteWindow::~RemoteWindow()
{
    App::Instance().RemoveListener(this);
    if (m_session) {
        m_session->Stop();
    }
}

DString RemoteWindow::GetSkinFolder()
{
    return _T("");
}

DString RemoteWindow::GetSkinFile()
{
    return _T("");
}

void RemoteWindow::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
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

void RemoteWindow::OnInitWindow()
{
    // pure-code windows must apply the caption/size-box rects themselves
    // (XML mode does this in WindowBuilder; attrs.m_rcCaption is not applied)
    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
    // system shadow (like shadow_type="system_round" in XML)
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemRound);

    const Palette& p = Theme::Get();
    const bool viewOnly = m_session && m_session->IsViewMode();

    ui::VBox* root = new ui::VBox(this);
    root->SetBkColor(p.windowBg);
    root->SetAttribute(_T("width"), _T("stretch"));
    root->SetAttribute(_T("height"), _T("stretch"));

    // ---- caption ----
    ui::HBox* caption = new ui::HBox(this);
    caption->SetAttribute(_T("height"), _T("36"));
    caption->SetBkColor(p.captionBg);
    root->AddItem(caption);

    m_titleLabel = new ThemeLabel(this);
    m_titleLabel->SetRole(ThemeLabel::Role::Main);
    m_titleLabel->SetText(m_session ? ui::StringConvert::UTF8ToT(m_session->RemoteName()) : _T(""));
    m_titleLabel->SetAttribute(_T("font"), _T("system_bold_12"));
    m_titleLabel->SetAttribute(_T("width"), _T("auto"));
    m_titleLabel->SetAttribute(_T("margin"), _T("12,0,0,0"));
    caption->AddItem(m_titleLabel);

    if (viewOnly) {
        m_badgeLabel = new ThemeLabel(this);
        m_badgeLabel->SetRole(ThemeLabel::Role::Accent);
        m_badgeLabel->SetText(SDK_TR("remote.viewOnly"));
        m_badgeLabel->SetAttribute(_T("width"), _T("auto"));
        m_badgeLabel->SetAttribute(_T("margin"), _T("10,0,0,0"));
        caption->AddItem(m_badgeLabel);
    }

    ui::Control* spacer = new ui::Control(this);
    spacer->SetMouseEnabled(false);
    caption->AddItem(spacer);

    IconButton* closeBtn = new IconButton(this);
    closeBtn->SetIcon(VectorArt::Icon::Close);
    closeBtn->SetIconToolTip(SDK_TR("window.close"));
    closeBtn->SetName(_T("closebtn"));
    closeBtn->SetAttribute(_T("width"), _T("34"));
    closeBtn->SetAttribute(_T("height"), _T("30"));
    closeBtn->SetAttribute(_T("margin"), _T("0,3,4,3"));
    caption->AddItem(closeBtn);

    // ---- screen ----
    m_bitmap = new ui::BitmapControl(this);
    m_bitmap->SetAttribute(_T("name"), _T("remote_bitmap"));
    m_bitmap->SetAttribute(_T("width"), _T("stretch"));
    m_bitmap->SetAttribute(_T("height"), _T("stretch"));
    m_bitmap->SetBkColor(p.windowBg);
    m_bitmap->SetAdaptiveDestRect(true); // fit, aspect preserved
    m_bitmap->SetSupportMultiThread(false);
    m_bitmap->SetMouseEnabled(!viewOnly);
    root->AddItem(m_bitmap);

    // ---- toolbar ----
    m_toolbar = new ui::VBox(this);
    m_toolbar->SetAttribute(_T("height"), _T("40"));
    m_toolbar->SetBkColor(p.panelBg);
    root->AddItem(m_toolbar);

    ui::HBox* toolRow = new ui::HBox(this);
    toolRow->SetAttribute(_T("child_margin_x"), _T("8"));
    toolRow->SetAttribute(_T("padding"), _T("10,5,10,5"));
    m_toolbar->AddItem(toolRow);

    m_fitPill = new PillButton(this);
    m_fitPill->SetText(SDK_TR("remote.fit"));
    m_fitPill->SetAttribute(_T("width"), _T("86"));
    m_fitPill->SetAttribute(_T("height"), _T("28"));
    m_fitPill->SetSelected(true);
    m_fitPill->AttachClick([this](const ui::EventArgs&) {
        ToggleFit();
        return true;
    });
    toolRow->AddItem(m_fitPill);

    m_fullscreenPill = new PillButton(this);
    m_fullscreenPill->SetText(SDK_TR("remote.fullscreen"));
    m_fullscreenPill->SetAttribute(_T("width"), _T("86"));
    m_fullscreenPill->SetAttribute(_T("height"), _T("28"));
    m_fullscreenPill->AttachClick([this](const ui::EventArgs&) {
        ToggleFullscreen();
        return true;
    });
    toolRow->AddItem(m_fullscreenPill);

    m_sendFileBtn = new IconButton(this);
    m_sendFileBtn->SetIcon(VectorArt::Icon::Folder);
    m_sendFileBtn->SetIconToolTip(SDK_TR("remote.sendFile"));
    m_sendFileBtn->SetAttribute(_T("width"), _T("28"));
    m_sendFileBtn->SetAttribute(_T("height"), _T("28"));
    m_sendFileBtn->AttachClick([this](const ui::EventArgs&) {
        if (m_session && m_session->IsConnected()) {
            FileSendWindow* w = new FileSendWindow(m_session->GetHost(),
                                                   m_session->FilePort(),
                                                   m_session->Token());
            w->CreateWnd(this, ui::WindowCreateParam(_T("StarDesk"), true));
            w->ShowWindow(ui::kSW_SHOW_NORMAL);
        }
        return true;
    });
    toolRow->AddItem(m_sendFileBtn);

    ui::Control* spacer2 = new ui::Control(this);
    spacer2->SetMouseEnabled(false);
    toolRow->AddItem(spacer2);

    m_statusLabel = new ThemeLabel(this);
    m_statusLabel->SetRole(ThemeLabel::Role::Sub);
    m_statusLabel->SetText(SDK_TR("remote.waitingStream"));
    m_statusLabel->SetAttribute(_T("width"), _T("auto"));
    m_statusLabel->SetAttribute(_T("margin"), _T("0,0,4,0"));
    toolRow->AddItem(m_statusLabel);

    m_disconnectBtn = new IconButton(this);
    m_disconnectBtn->SetIcon(VectorArt::Icon::Power);
    m_disconnectBtn->SetIconToolTip(SDK_TR("remote.disconnect"));
    m_disconnectBtn->SetAttribute(_T("width"), _T("28"));
    m_disconnectBtn->SetAttribute(_T("height"), _T("28"));
    m_disconnectBtn->AttachClick([this](const ui::EventArgs&) {
        Disconnect();
        return true;
    });
    toolRow->AddItem(m_disconnectBtn);

    AttachBox(root);
    App::Instance().AddListener(this);

    // ---- wire the session ----
    if (m_session) {
        ClientSession::Callbacks cb;
        cb.onFrame = [this]() {
            // latest-wins: fetch the newest complete frame when the UI runs
            PostToUI(ui::UiBind(this, [this]() {
                int w = 0, h = 0, cxn = -1, cyn = -1;
                std::vector<uint8_t> rgba;
                if (m_session && m_session->TakeLatestFrame(w, h, rgba, cxn, cyn)) {
                    OnFrameReady(w, h, rgba, cxn, cyn);
                }
            }));
        };
        cb.onStats = [this](int fps, int latencyMs) {
            PostToUI(ui::UiBind(this, [this, fps, latencyMs]() {
                OnStats(fps, latencyMs);
            }));
        };
        m_session->SetFrameCallbacks(std::move(cb));
    }

    BaseClass::OnInitWindow();
}

// ---------------------------------------------------------------- frame display

void RemoteWindow::OnFrameReady(int w, int h, const std::vector<uint8_t>& rgba,
                                int cursorXN, int cursorYN)
{
    // compose the cursor into the frame
    std::vector<uint8_t> frame = rgba;
    if (cursorXN >= 0 && cursorYN >= 0 && w > 0 && h > 0) {
        const int cx = (int)((int64_t)cursorXN * w / 65535);
        const int cy = (int)((int64_t)cursorYN * h / 65535);
        DrawCursor(frame.data(), w, h, cx, cy);
    }
    if (m_bitmap != nullptr && !frame.empty()) {
        m_bitmap->SetAdaptiveDestRect(m_fitMode);
        m_bitmap->SetBitmapData(w, h, frame.data(), (int)frame.size());
    }
    if (m_statusLabel != nullptr) {
        UpdateStatus();
    }
}

void RemoteWindow::OnStats(int fps, int latencyMs)
{
    m_lastFps = fps;
    m_lastLatency = latencyMs;
    m_hasStats = true;
    UpdateStatus();
}

void RemoteWindow::UpdateStatus()
{
    if (m_statusLabel == nullptr) {
        return;
    }
    DString text;
    if (m_hasStats) {
        text = ui::StringUtil::Printf(_T("%s %d  |  %s %d ms"),
                                      SDK_TR("remote.fpsLabel").c_str(), m_lastFps,
                                      SDK_TR("remote.latencyLabel").c_str(), m_lastLatency);
    }
    else {
        text = SDK_TR("remote.waitingStream");
    }
    m_statusLabel->SetText(text);
}

// ---------------------------------------------------------------- remote input

bool RemoteWindow::MapToRemote(const ui::UiPoint& pt, double& nx, double& ny)
{
    if (m_bitmap == nullptr || m_session == nullptr || !m_session->IsConnected()) {
        return false;
    }
    // bitmap control rect (window coords) + bitmap dest rect (control coords)
    const UiRect ctrl = m_bitmap->GetRect();
    const UiRect dest = m_bitmap->GetBitmapDest();
    const int x0 = ctrl.left + dest.left;
    const int y0 = ctrl.top + dest.top;
    if (dest.Width() <= 0 || dest.Height() <= 0) {
        return false;
    }
    nx = (double)(pt.x - x0) / dest.Width();
    ny = (double)(pt.y - y0) / dest.Height();
    return nx >= 0.0 && nx <= 1.0 && ny >= 0.0 && ny <= 1.0;
}

void RemoteWindow::SendMouse(int kind, const ui::UiPoint& pt, uint8_t buttons)
{
    if (m_session == nullptr || m_session->IsViewMode()) {
        return;
    }
    double nx = 0.0, ny = 0.0;
    if (!MapToRemote(pt, nx, ny)) {
        return;
    }
    InputEventMsg ev;
    ev.kind = (uint8_t)kind;
    ev.xNorm = (uint16_t)(nx * 65535.0);
    ev.yNorm = (uint16_t)(ny * 65535.0);
    ev.buttons = buttons;
    m_session->SendInput(ev);
}

void RemoteWindow::SendButtonMsg(bool down, int button, const ui::UiPoint& pt)
{
    if (m_session == nullptr || m_session->IsViewMode()) {
        return;
    }
    double nx = 0.0, ny = 0.0;
    if (!MapToRemote(pt, nx, ny)) {
        return;
    }
    InputEventMsg ev;
    ev.kind = 1; // button
    ev.xNorm = (uint16_t)(nx * 65535.0);
    ev.yNorm = (uint16_t)(ny * 65535.0);
    ev.buttons = (uint8_t)((1u << button) | (down ? 0x10u : 0u));
    m_session->SendInput(ev);
}

void RemoteWindow::SendKey(uint16_t vkCode, uint32_t modifierKey, bool down)
{
    if (m_session == nullptr || m_session->IsViewMode()) {
        return;
    }
    InputEventMsg ev;
    ev.kind = down ? 3 : 4;
    ev.vkCode = vkCode;
    ev.modifiers = (uint8_t)(modifierKey & 0x0F);
    m_session->SendInput(ev);
}

LRESULT RemoteWindow::OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey,
                                   const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    if (vkCode == ui::kVK_ESCAPE && m_fullscreen) {
        // let dui handle ESC to exit fullscreen
        bHandled = false;
        return BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
    }
    SendKey((uint16_t)vkCode, modifierKey, true);
    bHandled = true;
    return 0;
}

LRESULT RemoteWindow::OnKeyUpMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey,
                                 const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    SendKey((uint16_t)vkCode, modifierKey, false);
    bHandled = true;
    return 0;
}

LRESULT RemoteWindow::OnMouseMoveMsg(const ui::UiPoint& pt, uint32_t modifierKey, bool bFromNC,
                                     const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    SendMouse(0, pt, 0);
    return BaseClass::OnMouseMoveMsg(pt, modifierKey, bFromNC, nativeMsg, bHandled);
}

LRESULT RemoteWindow::OnMouseWheelMsg(int32_t wheelDelta, const ui::UiPoint& pt,
                                      uint32_t modifierKey,
                                      const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    if (m_session && !m_session->IsViewMode()) {
        double nx = 0.0, ny = 0.0;
        if (MapToRemote(pt, nx, ny)) {
            InputEventMsg ev;
            ev.kind = 2;
            ev.xNorm = (uint16_t)(nx * 65535.0);
            ev.yNorm = (uint16_t)(ny * 65535.0);
            ev.wheelDelta = (int16_t)(wheelDelta / 120);
            m_session->SendInput(ev);
        }
    }
    return BaseClass::OnMouseWheelMsg(wheelDelta, pt, modifierKey, nativeMsg, bHandled);
}

LRESULT RemoteWindow::OnMouseLButtonDownMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                            const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    SendButtonMsg(true, 0, pt);
    return BaseClass::OnMouseLButtonDownMsg(pt, modifierKey, nativeMsg, bHandled);
}

LRESULT RemoteWindow::OnMouseLButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                          const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    SendButtonMsg(false, 0, pt);
    return BaseClass::OnMouseLButtonUpMsg(pt, modifierKey, nativeMsg, bHandled);
}

LRESULT RemoteWindow::OnMouseRButtonDownMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                            const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    SendButtonMsg(true, 1, pt);
    return BaseClass::OnMouseRButtonDownMsg(pt, modifierKey, nativeMsg, bHandled);
}

LRESULT RemoteWindow::OnMouseRButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                          const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    SendButtonMsg(false, 1, pt);
    return BaseClass::OnMouseRButtonUpMsg(pt, modifierKey, nativeMsg, bHandled);
}

// ---------------------------------------------------------------- actions

void RemoteWindow::ToggleFit()
{
    m_fitMode = !m_fitMode;
    m_fitPill->SetSelected(m_fitMode);
    if (m_bitmap != nullptr) {
        m_bitmap->SetAdaptiveDestRect(m_fitMode);
        if (!m_fitMode) {
            // 1:1: center the bitmap at its native size
            m_bitmap->SetAdaptiveDestRect(false);
        }
    }
    InvalidateAll();
}

void RemoteWindow::ToggleFullscreen()
{
    m_fullscreen = !m_fullscreen;
    if (m_fullscreen) {
        EnterFullscreen();
    }
    else {
        ExitFullscreen();
    }
}

void RemoteWindow::Disconnect()
{
    if (m_disconnecting) {
        return;
    }
    m_disconnecting = true;
    if (m_session) {
        m_session->Stop();
        m_session.reset();
    }
    CloseWnd();
}

// ---------------------------------------------------------------- theme / language

void RemoteWindow::ApplyTheme()
{
    const Palette& p = Theme::Get();
    ui::VBox* root = dynamic_cast<ui::VBox*>(GetRoot());
    if (root != nullptr) {
        root->SetBkColor(p.windowBg);
    }
    if (m_bitmap != nullptr) {
        m_bitmap->SetBkColor(p.windowBg);
    }
    if (m_toolbar != nullptr) {
        m_toolbar->SetBkColor(p.panelBg);
    }
    InvalidateAll();
}

void RemoteWindow::ApplyLanguage()
{
    m_fullscreenPill->SetText(SDK_TR("remote.fullscreen"));
    m_fitPill->SetText(SDK_TR("remote.fit"));
    m_sendFileBtn->SetIconToolTip(SDK_TR("remote.sendFile"));
    m_disconnectBtn->SetIconToolTip(SDK_TR("remote.disconnect"));
    if (m_badgeLabel != nullptr) {
        m_badgeLabel->SetText(SDK_TR("remote.viewOnly"));
    }
    UpdateStatus();
    InvalidateAll();
}

} // namespace sdk
