#include "ClientSession.h"

#include "../app/AppConfig.h"
#include "../app/Language.h"
#include "../codec/FrameCodec.h"
#include "../net/Sha256.h"
#include "../net/TcpClient.h"

#include <chrono>

namespace sdk {

// ---------------------------------------------------------------- ClientSession

ClientSession::ClientSession(const Options& opt, Callbacks cb)
    : m_opt(opt), m_cb(std::move(cb)), m_mode(opt.mode)
{
}

ClientSession::~ClientSession()
{
    Stop();
}

void ClientSession::Start()
{
    if (m_running) {
        return;
    }
    m_running = true;
    m_thread = std::thread(&ClientSession::Run, this);
}

void ClientSession::Stop()
{
    // Always join the session thread (it may still be finishing even when
    // m_running is already false); std::thread destructor aborts otherwise.
    const bool wasRunning = m_running.exchange(false);
    if (wasRunning) {
        // wake the receive loop without releasing the fd number; the session
        // thread owns the fd and closes it itself
        Socket::Shutdown(m_fd);
    }
    if (m_thread.joinable()) {
        m_thread.join();
    }
    if (m_state != State::Failed && m_state != State::Closed) {
        SetState(State::Closed, _T(""));
    }
}

void ClientSession::SetState(State st, const DString& text)
{
    m_state = st;
    if (m_cb.onState) {
        m_cb.onState(st, text);
    }
}

void ClientSession::SendInput(const InputEventMsg& ev)
{
    if (!IsConnected() || IsViewMode()) {
        return; // server-side gate too
    }
    PayloadWriter w;
    w.PutU8(ev.kind);
    w.PutU16(ev.xNorm);
    w.PutU16(ev.yNorm);
    w.PutU8(ev.buttons);
    w.PutU16((uint16_t)(int16_t)ev.wheelDelta);
    w.PutU16(ev.vkCode);
    w.PutU8(ev.modifiers);
    w.PutU8(ev.flags);
    std::lock_guard<std::mutex> lock(m_sendMutex);
    SendFrame(m_fd, MsgType::InputEvent, w.Result());
}

void ClientSession::SetFrameCallbacks(Callbacks cb)
{
    std::lock_guard<std::mutex> lock(m_cbMutex);
    m_cb.onFrame = std::move(cb.onFrame);
    m_cb.onStats = std::move(cb.onStats);
}

void ClientSession::Run()
{
    RunImpl();
    Socket::Close(m_fd);
    m_fd = Socket::kInvalid;
    m_running = false;
}

void ClientSession::RunImpl()
{
    SetState(State::Connecting, SDK_TR("client.connecting"));

    // ---- connect ----
    m_fd = TcpClient::ConnectTo(m_opt.host, m_opt.port, 5000);
    if (m_fd == Socket::kInvalid) {
        m_lastError = "connect failed";
        if (m_running) SetState(State::Failed, SDK_TR("client.timeout"));
        m_running = false;
        return;
    }

    // ---- Hello ----
    {
        PayloadWriter w;
        w.PutString(ui::StringConvert::TToUTF8(App::Instance().Config().deviceName));
        w.PutU8((uint8_t)m_opt.mode);
        w.PutU8((uint8_t)m_opt.resCode);
        w.PutU8((uint8_t)m_opt.fps);
        if (!SendFrame(m_fd, MsgType::Hello, w.Result())) {
            FailNet(SDK_TR("client.networkError"));
            return;
        }
    }

    // ---- wait for challenge (host may show a manual-accept dialog first) ----
    Frame frame;
    while (m_running) {
        if (!RecvFrame(m_fd, frame, 15000)) {
            FailNet(SDK_TR("client.networkError"));
            return;
        }
        if (frame.type == MsgType::AcceptRequest) {
            SetState(State::WaitingAccept, SDK_TR("client.waitingAccept"));
            continue;
        }
        if (frame.type == MsgType::Challenge && frame.payload.size() >= 16) {
            break; // challenge received
        }
        if (frame.type == MsgType::AuthResult) {
            PayloadReader r(frame.payload.data(), frame.payload.size());
            const AuthResultCode code = (AuthResultCode)r.GetU8();
            m_lastError = "auth rejected";
            if (m_running) SetState(State::Failed, SDK_TR("client.rejected"));
            m_running = false;
            Socket::Close(m_fd);
            m_fd = Socket::kInvalid;
            return;
        }
    }
    if (!m_running) {
        return;
    }

    // ---- Auth (the challenge frame from the loop above is still in `frame`) ----
    {
        PayloadReader r(frame.payload.data(), frame.payload.size());
        uint8_t nonce[16] = {0};
        r.GetBytes(nonce, 16);
        if (!r.Ok()) {
            FailNet(SDK_TR("client.networkError"));
            return;
        }
        std::string challenge;
        challenge.assign((const char*)nonce, 16);
        challenge += ui::StringConvert::TToUTF8(m_opt.password);
        uint8_t digest[Sha256::kDigestSize];
        Sha256::Hash(challenge, digest);
        if (!SendFrame(m_fd, MsgType::Auth, digest, Sha256::kDigestSize)) {
            FailNet(SDK_TR("client.networkError"));
            return;
        }
    }

    // ---- AuthResult ----
    {
        Frame result;
        if (!RecvFrame(m_fd, result, 10000) || result.type != MsgType::AuthResult) {
            FailNet(SDK_TR("client.networkError"));
            return;
        }
        PayloadReader r(result.payload.data(), result.payload.size());
        const AuthResultCode code = (AuthResultCode)r.GetU8();
        m_filePort = r.GetU16();
        r.GetBytes(m_token, 32);
        if (code == AuthResultCode::Ok) {
            App::Instance().FileTx().SetSessionToken(m_token);
        }
        if (code != AuthResultCode::Ok) {
            m_lastError = "auth rejected";
            if (m_running) SetState(State::Failed, SDK_TR("client.authFailed"));
            m_running = false;
            Socket::Close(m_fd);
            m_fd = Socket::kInvalid;
            return;
        }
    }

    SetState(State::Connected, SDK_TR("client.connected_"));

    // ---- receive loop ----
    m_statsAt = std::chrono::steady_clock::now();
    m_lastPingAt = m_statsAt;
    m_lastNotifyAt = m_statsAt;
    while (m_running) {
        if (!Socket::WaitReadable(m_fd, 200)) {
            // periodic latency ping
            const auto now = std::chrono::steady_clock::now();
            if (now - m_lastPingAt > std::chrono::seconds(2)) {
                m_lastPingAt = now;
                PayloadWriter w;
                w.PutU64((uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(
                             now.time_since_epoch()).count());
                std::lock_guard<std::mutex> lock(m_sendMutex);
                SendFrame(m_fd, MsgType::Ping, w.Result());
            }
            continue;
        }
        Frame frame;
        if (!RecvFrame(m_fd, frame, 5000)) {
            break;
        }
        switch (frame.type) {
        case MsgType::Ping: {
            std::lock_guard<std::mutex> lock(m_sendMutex);
            SendFrame(m_fd, MsgType::Pong, frame.payload);
            break;
        }
        case MsgType::Pong: {
            PayloadReader r(frame.payload.data(), frame.payload.size());
            const uint64_t t0 = r.GetU64();
            const auto now = std::chrono::steady_clock::now();
            if (t0 > 0) {
                m_latencyMs = (int)std::chrono::duration_cast<std::chrono::milliseconds>(
                                  now - std::chrono::steady_clock::time_point(
                                            std::chrono::milliseconds(t0)))
                                  .count();
            }
            break;
        }
        case MsgType::Disconnect: {
            m_running = false;
            break;
        }
        case MsgType::ScreenInit: {
            HandleScreenInit(frame);
            break;
        }
        case MsgType::ScreenTile: {
            HandleScreenTile(frame);
            break;
        }
        case MsgType::CursorPos: {
            HandleCursorPos(frame);
            break;
        }
        default:
            break;
        }
    }

    Socket::Close(m_fd);
    m_fd = Socket::kInvalid;
    m_running = false;
    SetState(State::Closed, SDK_TR("client.remoteDisconnected"));
    return;

}

void ClientSession::FailNet(const DString& text)
{
    m_lastError = Socket::LastError();
    Socket::Close(m_fd);
    m_fd = Socket::kInvalid;
    m_running = false;
    if (m_state != State::Connected) {
        SetState(State::Failed, text);
    }
    else {
        SetState(State::Closed, SDK_TR("client.remoteDisconnected"));
    }
}

void ClientSession::Cleanup()
{
    Socket::Close(m_fd);
    m_fd = Socket::kInvalid;
    m_running = false;
}

// ---------------------------------------------------------------- frame stream

void ClientSession::HandleScreenInit(const Frame& frame)
{
    PayloadReader r(frame.payload.data(), frame.payload.size());
    const uint32_t w = r.GetU32();
    const uint32_t h = r.GetU32();
    if (!r.Ok() || w == 0 || h == 0 || w > 16384 || h > 16384) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(m_frameMutex);
        m_frameW = (int)w;
        m_frameH = (int)h;
        m_frame.assign((size_t)w * h * 4, 0xFF);
        m_frameDirty = true;
    }
    NotifyFrame();
}

void ClientSession::HandleScreenTile(const Frame& frame)
{
    PayloadReader r(frame.payload.data(), frame.payload.size());
    const int x = r.GetU16();
    const int y = r.GetU16();
    const int tw = r.GetU16();
    const int th = r.GetU16();
    std::vector<uint8_t> webp;
    webp.assign(frame.payload.begin() + 8, frame.payload.end());
    if (!r.Ok() || webp.empty()) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(m_frameMutex);
        if (m_frameW <= 0 || m_frameH <= 0) {
            return;
        }
        FrameDecoder::DecodeTileInto(webp.data(), webp.size(),
                                     m_frame.data(), m_frameW, m_frameH, x, y);
        m_frameDirty = true;
    }
    NotifyFrame();
}

void ClientSession::HandleCursorPos(const Frame& frame)
{
    PayloadReader r(frame.payload.data(), frame.payload.size());
    const int xn = r.GetU16();
    const int yn = r.GetU16();
    (void)r.GetU8(); // visible
    if (!r.Ok()) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(m_frameMutex);
        m_cursorXN = xn;
        m_cursorYN = yn;
    }
}

void ClientSession::NotifyFrame()
{
    // coalesce + throttle: tiles arrive per-batch, the UI refreshes at ~30fps
    const auto now = std::chrono::steady_clock::now();
    if (now - m_lastNotifyAt < std::chrono::milliseconds(33)) {
        return;
    }
    m_lastNotifyAt = now;
    std::vector<uint8_t> snapshot;
    int w = 0, h = 0, cxn = -1, cyn = -1;
    {
        std::lock_guard<std::mutex> lock(m_frameMutex);
        if (!m_frameDirty) {
            return;
        }
        m_frameDirty = false;
        w = m_frameW;
        h = m_frameH;
        snapshot = m_frame;
        cxn = m_cursorXN;
        cyn = m_cursorYN;
    }
    Callbacks cb;
    {
        std::lock_guard<std::mutex> lock(m_cbMutex);
        cb = m_cb;
    }
    if (cb.onFrame && w > 0 && h > 0) {
        cb.onFrame(w, h, snapshot, cxn, cyn);
    }
    UpdateStats();
}

void ClientSession::UpdateStats()
{
    const auto now = std::chrono::steady_clock::now();
    ++m_framesThisSecond;
    if (now - m_statsAt >= std::chrono::seconds(1)) {
        const double dt = std::chrono::duration<double>(now - m_statsAt).count();
        const int fps = (int)(m_framesThisSecond / dt);
        m_framesThisSecond = 0;
        m_statsAt = now;
        Callbacks cb;
        {
            std::lock_guard<std::mutex> lock(m_cbMutex);
            cb = m_cb;
        }
        if (cb.onStats) {
            cb.onStats(fps, m_latencyMs);
        }
    }
}

} // namespace sdk
