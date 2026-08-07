#include "RemoteWindowXml.h"

#include "FileSendWindowXml.h"

#include "examples/stardesk/src/app/Language.h"
#include "examples/stardesk/src/net/Protocol.h"

namespace sdk {

using ui::UiRect;

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
                p[0] = 0; p[1] = 0; p[2] = 0; p[3] = 255;
            }
            else {
                p[0] = 255; p[1] = 255; p[2] = 255; p[3] = 255;
            }
        }
    }
}

} // namespace

// ---------------------------------------------------------------- ctor

RemoteWindowXml::RemoteWindowXml(std::unique_ptr<ClientSession> session)
    : m_session(std::move(session))
{
}

RemoteWindowXml::~RemoteWindowXml()
{
    App::Instance().RemoveListener(this);
    if (m_session) {
        m_session->Stop();
    }
    if (m_onClosed) {
        m_onClosed();
    }
}

DString RemoteWindowXml::GetSkinFolder()
{
    return _T("stardesk_xml");
}

DString RemoteWindowXml::GetSkinFile()
{
    return _T("remote.xml");
}

void RemoteWindowXml::OnInitWindow()
{
    m_bitmap = dynamic_cast<ui::BitmapControl*>(FindControl(_T("remote_bitmap")));
    if (m_bitmap) {
        m_bitmap->SetAdaptiveDestRect(true);
        m_bitmap->SetMouseEnabled(!(m_session && m_session->IsViewMode()));
    }

    ui::Label* title = dynamic_cast<ui::Label*>(FindControl(_T("remote_title")));
    if (title && m_session) {
        title->SetText(ui::StringConvert::UTF8ToT(m_session->RemoteName()));
    }
    ui::Label* badge = dynamic_cast<ui::Label*>(FindControl(_T("view_badge")));
    if (badge && m_session && m_session->IsViewMode()) {
        badge->SetVisible(true);
    }
    m_statusLabel = dynamic_cast<ui::Label*>(FindControl(_T("status_label")));

    m_fitBtn = dynamic_cast<ui::Button*>(FindControl(_T("fit_btn")));
    if (m_fitBtn) {
        m_fitBtn->AttachClick([this](const ui::EventArgs&) {
            m_fitMode = !m_fitMode;
            if (m_bitmap) {
                m_bitmap->SetAdaptiveDestRect(m_fitMode);
            }
            if (m_fitBtn) {
                m_fitBtn->SetClass(m_fitMode ? _T("btn_global_blue_80x30")
                                             : _T("btn_global_color_white"));
            }
            return true;
        });
    }
    m_fullscreenBtn = dynamic_cast<ui::Button*>(FindControl(_T("fullscreen_btn")));
    if (m_fullscreenBtn) {
        m_fullscreenBtn->AttachClick([this](const ui::EventArgs&) {
            m_fullscreen = !m_fullscreen;
            if (m_fullscreen) {
                EnterFullscreen();
            }
            else {
                ExitFullscreen();
            }
            return true;
        });
    }
    ui::Button* sendBtn = dynamic_cast<ui::Button*>(FindControl(_T("sendfile_btn")));
    if (sendBtn) {
        sendBtn->AttachClick([this](const ui::EventArgs&) {
            if (m_session && m_session->IsConnected()) {
                FileSendWindowXml* w = new FileSendWindowXml(m_session->GetHost(),
                                                             m_session->FilePort(),
                                                             m_session->Token());
                w->CreateWnd(this, ui::WindowCreateParam(_T("StarDesk"), true));
                w->ShowWindow(ui::kSW_SHOW_NORMAL);
            }
            return true;
        });
    }
    ui::Button* disconnectBtn = dynamic_cast<ui::Button*>(FindControl(_T("disconnect_btn")));
    if (disconnectBtn) {
        disconnectBtn->AttachClick([this](const ui::EventArgs&) {
            if (m_disconnecting) {
                return true;
            }
            m_disconnecting = true;
            if (m_session) {
                m_session->Stop();
                m_session.reset();
            }
            CloseWnd();
            return true;
        });
    }

    App::Instance().AddListener(this);

    if (m_session) {
        ClientSession::Callbacks cb;
        cb.onFrame = [this](int w, int h, const std::vector<uint8_t>& rgba,
                            int cxn, int cyn) {
            PostToUI(ui::UiBind(this, [this, w, h, rgba, cxn, cyn]() {
                OnFrameReady(w, h, rgba, cxn, cyn);
            }));
        };
        cb.onStats = [this](int fps, int latencyMs) {
            PostToUI(ui::UiBind(this, [this, fps, latencyMs]() {
                OnStats(fps, latencyMs);
            }));
        };
        m_session->SetFrameCallbacks(std::move(cb));
    }

    UpdateStatus();
    BaseClass::OnInitWindow();
}

// ---------------------------------------------------------------- frame display

void RemoteWindowXml::OnFrameReady(int w, int h, const std::vector<uint8_t>& rgba,
                                   int cursorXN, int cursorYN)
{
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
    UpdateStatus();
}

void RemoteWindowXml::OnStats(int fps, int latencyMs)
{
    m_lastFps = fps;
    m_lastLatency = latencyMs;
    m_hasStats = true;
    UpdateStatus();
}

void RemoteWindowXml::UpdateStatus()
{
    if (m_statusLabel == nullptr) {
        return;
    }
    if (m_hasStats) {
        m_statusLabel->SetText(ui::StringUtil::Printf(
            _T("%s %d  |  %s %d ms"),
            SDK_TR("remote.fpsLabel").c_str(), m_lastFps,
            SDK_TR("remote.latencyLabel").c_str(), m_lastLatency));
    }
    else {
        m_statusLabel->SetText(SDK_TR("remote.waitingStream"));
    }
}

// ---------------------------------------------------------------- remote input

bool RemoteWindowXml::MapToRemote(const ui::UiPoint& pt, double& nx, double& ny)
{
    if (m_bitmap == nullptr || m_session == nullptr || !m_session->IsConnected()) {
        return false;
    }
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

void RemoteWindowXml::SendMouse(int kind, const ui::UiPoint& pt, uint8_t buttons)
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

void RemoteWindowXml::SendButtonMsg(bool down, int button, const ui::UiPoint& pt)
{
    if (m_session == nullptr || m_session->IsViewMode()) {
        return;
    }
    double nx = 0.0, ny = 0.0;
    if (!MapToRemote(pt, nx, ny)) {
        return;
    }
    InputEventMsg ev;
    ev.kind = 1;
    ev.xNorm = (uint16_t)(nx * 65535.0);
    ev.yNorm = (uint16_t)(ny * 65535.0);
    ev.buttons = (uint8_t)((1u << button) | (down ? 0x10u : 0u));
    m_session->SendInput(ev);
}

void RemoteWindowXml::SendKey(uint16_t vkCode, uint32_t modifierKey, bool down)
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

LRESULT RemoteWindowXml::OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey,
                                      const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    if (vkCode == ui::kVK_ESCAPE && m_fullscreen) {
        bHandled = false;
        return BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
    }
    SendKey((uint16_t)vkCode, modifierKey, true);
    bHandled = true;
    return 0;
}

LRESULT RemoteWindowXml::OnKeyUpMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey,
                                    const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    SendKey((uint16_t)vkCode, modifierKey, false);
    bHandled = true;
    return 0;
}

LRESULT RemoteWindowXml::OnMouseMoveMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                        bool bFromNC, const ui::NativeMsg& nativeMsg,
                                        bool& bHandled)
{
    SendMouse(0, pt, 0);
    return BaseClass::OnMouseMoveMsg(pt, modifierKey, bFromNC, nativeMsg, bHandled);
}

LRESULT RemoteWindowXml::OnMouseWheelMsg(int32_t wheelDelta, const ui::UiPoint& pt,
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

LRESULT RemoteWindowXml::OnMouseLButtonDownMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                               const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    SendButtonMsg(true, 0, pt);
    return BaseClass::OnMouseLButtonDownMsg(pt, modifierKey, nativeMsg, bHandled);
}

LRESULT RemoteWindowXml::OnMouseLButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                             const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    SendButtonMsg(false, 0, pt);
    return BaseClass::OnMouseLButtonUpMsg(pt, modifierKey, nativeMsg, bHandled);
}

LRESULT RemoteWindowXml::OnMouseRButtonDownMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                               const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    SendButtonMsg(true, 1, pt);
    return BaseClass::OnMouseRButtonDownMsg(pt, modifierKey, nativeMsg, bHandled);
}

LRESULT RemoteWindowXml::OnMouseRButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                             const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    SendButtonMsg(false, 1, pt);
    return BaseClass::OnMouseRButtonUpMsg(pt, modifierKey, nativeMsg, bHandled);
}

// ---------------------------------------------------------------- theme / language

void RemoteWindowXml::ApplyTheme()
{
    InvalidateAll();
}

void RemoteWindowXml::ApplyLanguage()
{
    if (m_fitBtn) {
        m_fitBtn->SetText(SDK_TR("remote.fit"));
    }
    if (m_fullscreenBtn) {
        m_fullscreenBtn->SetText(m_fullscreen ? SDK_TR("remote.exitFullscreen")
                                              : SDK_TR("remote.fullscreen"));
    }
    UpdateStatus();
    InvalidateAll();
}

} // namespace sdk
