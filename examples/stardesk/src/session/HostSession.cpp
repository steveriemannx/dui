#include "HostSession.h"

#include "../app/AppConfig.h"
#include "../app/Language.h"
#include "../codec/FrameCodec.h"
#include "../net/Sha256.h"
#include "../net/TcpServer.h"
#include "../platform/InputInject.h"

#include <algorithm>
#include <chrono>
#include <thread>

namespace sdk {

namespace {

uint64_t g_seed = (uint64_t)std::chrono::steady_clock::now().time_since_epoch().count();

uint64_t NextRandom()
{
    // xorshift64 (thread-safe enough for nonces/tokens)
    uint64_t x = g_seed;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    g_seed = x;
    return x;
}

void FillRandom(uint8_t* out, size_t len)
{
    for (size_t i = 0; i < len; i += 8) {
        const uint64_t v = NextRandom();
        for (size_t j = 0; j < 8 && i + j < len; ++j) {
            out[i + j] = (uint8_t)(v >> (j * 8));
        }
    }
}

} // namespace

// ---------------------------------------------------------------- HostSession

HostSession::HostSession(Socket::sock_t fd, std::string peerIp, uint16_t filePort,
                         const Callbacks& cb)
    : m_fd(fd), m_peerIp(std::move(peerIp)), m_filePort(filePort), m_cb(cb)
{
}

HostSession::~HostSession()
{
    Stop();
}

void HostSession::Start()
{
    if (m_running) {
        return;
    }
    m_running = true;
    m_thread = std::thread(&HostSession::Run, this);
}

void HostSession::TakeFd()
{
    // transfer the socket to thread-local ownership: from now on only this
    // thread may Close() it; Stop() only Shutdown()s to wake blocked recv
    m_threadFd = m_fd;
    m_fd = Socket::kInvalid;
}

void HostSession::Stop()
{
    // Always join the session thread (it may still be finishing even when
    // m_running is already false); std::thread destructor aborts otherwise.
    const bool wasRunning = m_running.exchange(false);
    if (wasRunning) {
        // wake the blocked recv WITHOUT releasing the fd number (Close() from
        // another thread can race with fd reuse and kill a newer connection);
        // the session thread owns the fd and closes it itself
        Socket::Shutdown(m_fd);
        Socket::Shutdown(m_threadFd);
        m_askCv.notify_all();
    }
    if (m_thread.joinable()) {
        m_thread.join();
    }
    // stop the capture thread too (it may be blocked in Capture(); the
    // SCScreenshotManager call has its own 3s timeout)
    m_captureRunning = false;
    if (m_captureThread.joinable()) {
        m_captureThread.join();
    }
}

void HostSession::RespondAsk(bool accept)
{
    {
        std::lock_guard<std::mutex> lock(m_askMutex);
        m_askDecided = true;
        m_askAccept = accept;
    }
    m_askCv.notify_all();
}

void HostSession::Run()
{
    TakeFd();
    RunImpl();
    CloseThreadFd(); // thread owns the fd; no-op if RunImpl already closed it
    m_finished = true;
}

void HostSession::CloseThreadFd()
{
    Socket::Close(m_threadFd);
    m_threadFd = Socket::kInvalid;
}

void HostSession::RunImpl()
{
    const auto t0 = std::chrono::steady_clock::now();
    bool ok = false;

    // ---- 1. Hello ----
    Frame frame;
    if (!RecvFrame(m_threadFd, frame, 10000) || frame.type != MsgType::Hello) {
        CloseThreadFd();
        m_running = false;
        if (m_cb.onClosed) m_cb.onClosed();
        return;
    }
    PayloadReader r(frame.payload.data(), frame.payload.size());
    std::string deviceName;
    if (!r.GetString(deviceName) || !r.Ok()) {
        CloseThreadFd();
        m_running = false;
        if (m_cb.onClosed) m_cb.onClosed();
        return;
    }
    m_view = (ConnMode)r.GetU8() == ConnMode::View;
    m_peerRes = r.GetU8();
    m_peerFps = r.GetU8();

    const AppConfig& cfg = App::Instance().Config();
    const std::string password = ui::StringConvert::TToUTF8(cfg.password);

    // tell the client a manual-accept dialog is showing
    if (cfg.manualAccept) {
        PayloadWriter w;
        w.PutString(ui::StringConvert::TToUTF8(App::GetHostName()));
        SendFrame(m_threadFd, MsgType::AcceptRequest, w.Result());

        AskInfo info;
        info.deviceName = ui::StringConvert::UTF8ToT(deviceName);
        info.ip = ui::StringConvert::UTF8ToT(m_peerIp);
        info.view = m_view;
        if (m_cb.onAsk) {
            // posted by the caller side (MainWindow wraps with PostTask)
            m_cb.onAsk(this, info);
        }
        {
            std::unique_lock<std::mutex> lock(m_askMutex);
            if (!m_askDecided) {
                m_askCv.wait_for(lock, std::chrono::seconds(60));
            }
        }
        if (!m_askDecided || !m_askAccept) {
            // rejected or timed out
            PayloadWriter w;
            w.PutU8((uint8_t)AuthResultCode::Rejected);
            w.PutU16(0);
            uint8_t zero[32] = {0};
            w.PutBytes(zero, 32);
            SendFrame(m_threadFd, MsgType::AuthResult, w.Result());
            CloseThreadFd();
            m_running = false;
            if (m_cb.onClosed) m_cb.onClosed();
            return;
        }
    }

    // ---- 2. challenge ----
    uint8_t nonce[16];
    FillRandom(nonce, sizeof(nonce));
    PayloadWriter w1;
    w1.PutBytes(nonce, sizeof(nonce));
    if (!SendFrame(m_threadFd, MsgType::Challenge, w1.Result())) {
        CloseThreadFd();
        m_running = false;
        if (m_cb.onClosed) m_cb.onClosed();
        return;
    }

    // ---- 3. auth ----
    if (!RecvFrame(m_threadFd, frame, 10000) || frame.type != MsgType::Auth ||
        frame.payload.size() != Sha256::kDigestSize) {
        CloseThreadFd();
        m_running = false;
        if (m_cb.onClosed) m_cb.onClosed();
        return;
    }

    uint8_t expected[Sha256::kDigestSize];
    {
        std::string challenge;
        challenge.assign((const char*)nonce, sizeof(nonce));
        challenge += password;
        Sha256::Hash(challenge, expected);
    }
    ok = std::memcmp(expected, frame.payload.data(), Sha256::kDigestSize) == 0;

    FillRandom(m_token, sizeof(m_token));
    if (ok) {
        // register the session token so the host can send files back
        App::Instance().FileTx().SetSessionToken(m_token);
    }

    PayloadWriter w2;
    w2.PutU8((uint8_t)(ok ? AuthResultCode::Ok : AuthResultCode::WrongPassword));
    w2.PutU16(m_filePort);
    w2.PutBytes(m_token, sizeof(m_token));
    SendFrame(m_threadFd, MsgType::AuthResult, w2.Result());

    if (!ok) {
        CloseThreadFd();
        m_running = false;
        if (m_cb.onClosed) m_cb.onClosed();
        return;
    }

    m_connected = true;
    if (m_cb.onStatus) {
        // "xxx 正在连接本机" (format via UTF-8 to work on narrow + wide builds)
        const std::string fmt = ui::StringConvert::TToUTF8(SDK_TR("host.sessionOf"));
        const std::string text = ui::StringUtil::Printf(fmt.c_str(), deviceName.c_str());
        m_cb.onStatus(ui::StringConvert::UTF8ToT(text), true);
    }

    // ---- screen capture (may fall back to the test pattern) ----
    m_capturer = std::make_unique<Capturer>();
    m_captureRunning = true;
    if (m_capturer->Start(cfg.extendScreen)) {
        m_usingTestPattern = false;
        m_logicalW = m_capturer->GetWidth();
        m_logicalH = m_capturer->GetHeight();
    }
    else {
        m_usingTestPattern = true;
        m_testPattern = std::make_unique<TestPatternSource>();
        // logical size will be set by the first encoded frame
        // surface the failure on the host status bar (permission hint when
        // the cause is TCC, otherwise the capturer error verbatim)
        if (m_cb.onStatus) {
            const std::string why = m_capturer->GetError();
            if (why.find("permission") != std::string::npos) {
                m_cb.onStatus(SDK_TR("remote.permissionHint"), false);
            }
            else {
                const std::string fmt = ui::StringConvert::TToUTF8(SDK_TR("host.captureError"));
                const std::string text = ui::StringUtil::Printf(fmt.c_str(), why.c_str());
                m_cb.onStatus(ui::StringConvert::UTF8ToT(text), false);
            }
        }
    }
    m_captureThread = std::thread(&HostSession::CaptureLoop, this);

    // ---- 4. streaming loop (heartbeat for now; screen/input in later steps) ----
    const auto lastRecv = std::make_shared<std::chrono::steady_clock::time_point>(t0);
    const auto lastPing = std::make_shared<std::chrono::steady_clock::time_point>(t0);
    while (m_running) {
        // heartbeat: ping every 3s, drop after 15s of silence
        const auto now = std::chrono::steady_clock::now();
        if (now - *lastPing > std::chrono::seconds(3)) {
            PayloadWriter w;
            w.PutU64((uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
            SendFrameLocked(MsgType::Ping, w.Result());
            *lastPing = now;
        }
        if (now - *lastRecv > std::chrono::seconds(15)) {
            break; // dead peer
        }

        if (!Socket::WaitReadable(m_threadFd, 100)) {
            continue;
        }
        if (!RecvFrame(m_threadFd, frame, 5000)) {
            break;
        }
        switch (frame.type) {
        case MsgType::Ping: {
            SendFrameLocked(MsgType::Pong, frame.payload);
            break;
        }
        case MsgType::Pong: {
            *lastRecv = std::chrono::steady_clock::now();
            break;
        }
        case MsgType::InputEvent: {
            HandleInputEvent(frame.payload); // view-only sessions gate inside
            break;
        }
        case MsgType::Disconnect: {
            m_running = false;
            break;
        }
        default:
            break;
        }
        *lastRecv = std::chrono::steady_clock::now();
    }

    // stop the capture thread (it may be blocked in Capture(); the
    // SCScreenshotManager call has its own 3s timeout)
    m_captureRunning = false;
    if (m_captureThread.joinable()) {
        m_captureThread.join();
    }
    m_capturer.reset();
    m_testPattern.reset();

    Cleanup();
    if (m_cb.onStatus) {
        m_cb.onStatus(SDK_TR("host.ready"), true);
    }
    if (m_cb.onClosed) {
        m_cb.onClosed();
    }
}

void HostSession::Cleanup()
{
    m_connected = false;
    m_running = false;
}

void HostSession::HandleInputEvent(const std::vector<uint8_t>& payload)
{
    // view-only sessions never inject (server-side enforcement)
    if (m_view) {
        return;
    }
    PayloadReader r(payload.data(), payload.size());
    if (!r.Ok()) {
        return;
    }
    const uint8_t kind = r.GetU8();
    const uint16_t xNorm = r.GetU16();
    const uint16_t yNorm = r.GetU16();
    const uint8_t buttons = r.GetU8();
    const int16_t wheelDelta = (int16_t)r.GetU16();
    const uint16_t vkCode = r.GetU16();
    const uint8_t modifiers = r.GetU8();
    (void)buttons;

    const int lw = m_logicalW.load();
    const int lh = m_logicalH.load();
    if (lw <= 0 || lh <= 0) {
        // screen size not known yet (capture still starting): a normalized
        // position cannot be mapped - dropping avoids moving the cursor to
        // the screen origin
        return;
    }
    const double nx = (double)xNorm / 65535.0;
    const double ny = (double)yNorm / 65535.0;

    // same-machine feedback guard: never inject into a StarDesk window
    // (any instance). A click/key aimed into the remote-view window would
    // land on it and re-trigger an input event (infinite loop); the windows
    // are excluded from the capture anyway, so they are invisible in the
    // stream and there is nothing meaningful to operate there.
    if (m_capturer) {
        const int pw = m_capturer->GetWidth();   // physical points
        const int ph = m_capturer->GetHeight();
        const int ox = m_capturer->GetOriginX();
        const int oy = m_capturer->GetOriginY();
        if (kind <= 2) { // mouse move / button / wheel carry a position
            if (InputInjector::IsOwnWindowAt(ox + nx * pw, oy + ny * ph)) {
                return;
            }
        }
        else if (InputInjector::IsOwnAppFocused()) {
            return;
        }
    }

    switch (kind) {
    case 0: // mouse move
        InputInjector::MoveTo(nx, ny, lw, lh);
        break;
    case 1: { // mouse button: bit0..2 in `buttons`
        if ((buttons & 0x01) && (buttons & 0x10)) InputInjector::Button(true, 0);
        if ((buttons & 0x02) && (buttons & 0x10)) InputInjector::Button(true, 1);
        if ((buttons & 0x04) && (buttons & 0x10)) InputInjector::Button(true, 2);
        if ((buttons & 0x01) && !(buttons & 0x10)) InputInjector::Button(false, 0);
        if ((buttons & 0x02) && !(buttons & 0x10)) InputInjector::Button(false, 1);
        if ((buttons & 0x04) && !(buttons & 0x10)) InputInjector::Button(false, 2);
        break;
    }
    case 2: // wheel
        InputInjector::Wheel(wheelDelta);
        break;
    case 3: // key down
        InputInjector::Key(vkCode, true);
        break;
    case 4: // key up
        InputInjector::Key(vkCode, false);
        break;
    default:
        break;
    }
}

bool HostSession::SendFrameLocked(MsgType type, const std::vector<uint8_t>& payload)
{
    std::lock_guard<std::mutex> lock(m_sendMutex);
    if (!m_connected) {
        return false;
    }
    return SendFrame(m_threadFd, type, payload);
}

// backpressure: pause briefly when the socket is congested (peer not reading),
// so the capture thread cannot monopolize the send lock forever
static bool SendFrameLockedPaced(bool ok)
{
    if (!ok) {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
    return ok;
}

void HostSession::CaptureLoop()
{
    FrameEncoder encoder(75);
    const int fps = std::max(1, std::min(60, m_peerFps.load()));
    const int periodMs = std::max(10, 1000 / fps);
    const int resTargetW = m_peerRes.load() == 1 ? 1280 : (m_peerRes.load() == 2 ? 1920 : 0);
    const int resTargetH = m_peerRes.load() == 1 ? 720 : (m_peerRes.load() == 2 ? 1080 : 0);
    int lastTargetW = -1, lastTargetH = -1;

    auto nextTick = std::chrono::steady_clock::now();
    int cursorTick = 0;
    while (m_captureRunning) {
        nextTick += std::chrono::milliseconds(periodMs);

        // switch the SCStream to the peer's requested resolution once
        // (aspect-preserving; the downscale below then becomes a no-op)
        if (resTargetW != lastTargetW || resTargetH != lastTargetH) {
            lastTargetW = resTargetW;
            lastTargetH = resTargetH;
            if (m_capturer) {
                m_capturer->SetTargetResolution(resTargetW, resTargetH);
            }
        }

        CaptureFrame frame;
        bool got = m_capturer && m_capturer->Capture(frame);
        if (!got && m_usingTestPattern && m_testPattern) {
            got = m_testPattern->Capture(frame, m_logicalW.load() > 0 ? m_logicalW.load() : 1280,
                                         m_logicalH.load() > 0 ? m_logicalH.load() : 720);
        }
        if (got) {
            const uint8_t* rgba = frame.rgba.data();
            int w = frame.width;
            int h = frame.height;
            std::vector<uint8_t> down;
            int dw = 0, dh = 0;
            if (resTargetW > 0 &&
                FrameEncoder::Downscale(rgba, w, h, resTargetW, resTargetH, down, dw, dh)) {
                rgba = down.data();
                w = dw;
                h = dh;
            }
            std::vector<EncodedTile> tiles;
            const bool sizeChanged = encoder.EncodeFrame(rgba, w, h, tiles);
            if (sizeChanged) {
                m_logicalW = w;
                m_logicalH = h;
                PayloadWriter wInit;
                wInit.PutU32((uint32_t)w);
                wInit.PutU32((uint32_t)h);
                if (!SendFrameLocked(MsgType::ScreenInit, wInit.Result())) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }
            }
            for (const EncodedTile& t : tiles) {
                PayloadWriter wt;
                wt.PutU16((uint16_t)t.x);
                wt.PutU16((uint16_t)t.y);
                wt.PutU16((uint16_t)t.w);
                wt.PutU16((uint16_t)t.h);
                wt.PutBytes(t.webp.data(), t.webp.size());
                if (!SendFrameLocked(MsgType::ScreenTile, wt.Result())) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                    break; // socket is congested; drop the rest of this batch
                }
            }
            // end-of-frame marker: the client renders only on this, so a
            // frame never mixes tiles from two batches (seams on fast scroll)
            SendFrameLocked(MsgType::ScreenEnd, {});
        }

        // cursor position at ~20fps (normalized to the logical screen)
        if (++cursorTick >= std::max(1, periodMs / 50)) {
            cursorTick = 0;
            int cx = 0, cy = 0;
            if (m_capturer && InputInjector::GetCursorPos(cx, cy)) {
                const int ox = m_capturer->GetOriginX();
                const int oy = m_capturer->GetOriginY();
                // GetCursorPos returns PHYSICAL point coordinates; the
                // normalized position must use the physical screen size too
                // (the logical frame size is the captured pixel size, e.g.
                // 2x Retina - dividing by it would put the cursor at half
                // the true distance)
                const int pw = m_capturer->GetWidth();
                const int ph = m_capturer->GetHeight();
                if (pw > 0 && ph > 0) {
                    PayloadWriter wc;
                    wc.PutU16((uint16_t)std::max(0, std::min(65535,
                        (int)((double)(cx - ox) / pw * 65535.0))));
                    wc.PutU16((uint16_t)std::max(0, std::min(65535,
                        (int)((double)(cy - oy) / ph * 65535.0))));
                    wc.PutU8(1); // visible
                    SendFrameLocked(MsgType::CursorPos, wc.Result());
                }
            }
        }

        const auto now = std::chrono::steady_clock::now();
        if (now < nextTick) {
            std::this_thread::sleep_for(nextTick - now);
        }
        else {
            nextTick = now; // capture is slower than the requested fps: don't catch up
        }
    }
}

// ---------------------------------------------------------------- HostService

HostService::HostService()
{
}

HostService::~HostService()
{
    Stop();
}

bool HostService::Start(uint16_t port, const HostSession::Callbacks& cb)
{
    if (m_running) {
        return true;
    }
    m_cb = cb;
    // bind the host port; when it is taken (e.g. a second StarDesk instance
    // on this machine) fall back to the next free port in a small range so
    // both instances can host at once. Port() reports what was bound.
    m_port = 0;
    m_server = std::make_unique<TcpServer>();
    for (int i = 0; i < 10 && m_port == 0; ++i) {
        const uint32_t p = (uint32_t)port + (uint32_t)i;
        if (p > 65535) {
            break;
        }
        if (m_server->Start((uint16_t)p, [this](Socket::sock_t fd, const std::string& ip) {
                OnAccept(fd, ip);
            })) {
            m_port = (uint16_t)p;
        }
    }
    m_running = m_port != 0;
    return m_running;
}

void HostService::Stop()
{
    if (!m_running) {
        return;
    }
    m_running = false;
    if (m_server) {
        m_server->Stop();
        m_server.reset();
    }
    std::vector<HostSessionPtr> sessions;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        sessions.swap(m_sessions);
    }
    for (auto& s : sessions) {
        s->Stop();
    }
}

int HostService::SessionCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return (int)m_sessions.size();
}

HostSessionPtr HostService::FirstConnectedSession() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const HostSessionPtr& s : m_sessions) {
        if (s->IsConnected()) {
            return s;
        }
    }
    return nullptr;
}

bool HostService::IsRunning() const
{
    return m_running;
}

void HostService::OnAccept(Socket::sock_t fd, const std::string& peerIp)
{
    // the advertised file port is the app's actual bound file port
    // (HostService may have fallen back to a free port on this machine)
    HostSessionPtr session =
        std::make_shared<HostSession>(fd, peerIp, App::Instance().FileTx().BoundPort(), m_cb);
    session->Start();
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // prune finished sessions (their threads have exited; safe to drop)
        m_sessions.erase(std::remove_if(m_sessions.begin(), m_sessions.end(),
                                        [](const HostSessionPtr& s) { return s->IsFinished(); }),
                         m_sessions.end());
        m_sessions.push_back(session);
    }
}

} // namespace sdk
