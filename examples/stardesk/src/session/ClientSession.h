#ifndef STARDESK_SESSION_CLIENT_SESSION_H_
#define STARDESK_SESSION_CLIENT_SESSION_H_

#include "../net/Protocol.h"
#include "../net/Socket.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace sdk {

/** Client-side connection: connect + auth handshake, then the receive loop
 *  (screen stream in step 4, input sending in step 5). Owns its socket.
 */
class ClientSession {
public:
    enum class State {
        Idle,
        Connecting,     // TCP connect + Hello
        WaitingAccept,  // host is showing the manual-accept dialog
        Authenticating, // waiting for the challenge/auth result
        Connected,
        Failed,         // see LastError()
        Closed,
    };

    struct Options {
        std::string host;
        uint16_t port = kDefaultPort;
        DString password;
        ConnMode mode = ConnMode::Control;
        int resCode = 0; // 0=original 1=720p 2=1080p
        int fps = 30;    // 24/30/60
    };

    struct Callbacks {
        /** UI thread (posted): state transition, including failures.
         *  text is the human-readable state/error in the current language. */
        std::function<void(State state, const DString& text)> onState;
        /** Session thread: a new decoded frame is available (snapshot copy).
         *  cursorXN/YN are the remote cursor position normalized (0..65535),
         *  or -1 when no cursor info arrived yet. */
        std::function<void(int w, int h, const std::vector<uint8_t>& rgba,
                           int cursorXN, int cursorYN)> onFrame;
        /** Session thread: ~1/s stream statistics. */
        std::function<void(int fps, int latencyMs)> onStats;
    };

    explicit ClientSession(const Options& opt, Callbacks cb);
    ~ClientSession();

    void Start();
    void Stop();

    State GetState() const { return m_state; }
    const std::string& LastError() const { return m_lastError; }
    bool IsConnected() const { return m_state == State::Connected; }

    const std::string& GetHost() const { return m_opt.host; }
    const std::string& RemoteName() const { return m_remoteName; }
    uint16_t FilePort() const { return m_filePort; }
    const uint8_t* Token() const { return m_token; }
    bool IsViewMode() const { return m_mode == ConnMode::View; }
    int PeerFps() const { return m_peerFps; }
    int PeerRes() const { return m_peerRes; }

    /** Send one input event (control mode only; no-op in view mode). */
    void SendInput(const InputEventMsg& ev);

    /** Replace the onFrame/onStats callbacks at runtime (thread-safe; the
     *  remote window registers them after the connection is established). */
    void SetFrameCallbacks(Callbacks cb);

private:
    void Run();
    void RunImpl();
    void Cleanup();
    void SetState(State st, const DString& text);
    void FailNet(const DString& text);

    Options m_opt;
    Callbacks m_cb;
    mutable std::mutex m_cbMutex;

    std::thread m_thread;
    std::atomic<bool> m_running{false};
    std::atomic<State> m_state{State::Idle};

    Socket::sock_t m_fd = Socket::kInvalid;
    std::string m_lastError;
    std::string m_remoteName;
    uint16_t m_filePort = 0;
    uint8_t m_token[32] = {0};
    ConnMode m_mode = ConnMode::Control;
    int m_peerFps = 0;
    int m_peerRes = 0;

    std::mutex m_sendMutex;

    // ---- decoded frame state (receive thread) ----
    std::mutex m_frameMutex;
    int m_frameW = 0;
    int m_frameH = 0;
    std::vector<uint8_t> m_frame;   // RGBA
    bool m_frameDirty = false;      // new tiles since the last onFrame
    int m_cursorXN = -1;
    int m_cursorYN = -1;

    // ---- stats (receive thread) ----
    std::chrono::steady_clock::time_point m_statsAt;
    int m_framesThisSecond = 0;
    int m_latencyMs = 0;
    std::chrono::steady_clock::time_point m_lastPingAt;
    std::chrono::steady_clock::time_point m_lastNotifyAt;

    void HandleScreenInit(const Frame& frame);
    void HandleScreenTile(const Frame& frame);
    void HandleCursorPos(const Frame& frame);
    void NotifyFrame();
    void UpdateStats();
};

} // namespace sdk

#endif // STARDESK_SESSION_CLIENT_SESSION_H_
