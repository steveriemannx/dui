#ifndef STARDESK_SESSION_HOST_SESSION_H_
#define STARDESK_SESSION_HOST_SESSION_H_

#include "../net/Protocol.h"
#include "../net/Socket.h"
#include "../platform/Capturer.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace sdk {

/** Info shown in the manual-accept dialog (UI thread). */
struct AskInfo {
    DString deviceName;
    DString ip;
    bool view = false; // true = view-only request
};

/** One accepted host connection: runs the auth handshake on its own thread,
 *  then the heartbeat/screen/input loop. The screen stream (step 4) and input
 *  injection (step 5) hook into Run().
 */
class HostSession;
typedef std::shared_ptr<HostSession> HostSessionPtr;

class HostSession {
public:
    struct Callbacks {
        /** Session thread: host-side status line. */
        std::function<void(const DString& text, bool ok)> onStatus;
        /** UI thread (posted): show the manual-accept dialog. The session is
         *  passed so the dialog can report the decision back. */
        std::function<void(HostSession* session, const AskInfo& info)> onAsk;
        /** Session thread: connection closed / never established. */
        std::function<void()> onClosed;
    };

    HostSession(Socket::sock_t fd, std::string peerIp, uint16_t filePort,
                const Callbacks& cb);
    ~HostSession();

    void Start();
    void Stop();

    /** UI thread: user's decision for the pending manual-accept dialog. */
    void RespondAsk(bool accept);

    bool IsViewMode() const { return m_view; }
    const std::string& PeerIp() const { return m_peerIp; }
    bool IsConnected() const { return m_connected; }
    /** Session token (file channel); also registers it on the global
     *  FileTransfer service so the host can send files back. */
    const uint8_t* Token() const { return m_token; }

private:
    void Run();
    void RunImpl();
    void TakeFd();
    void CloseThreadFd();
    void Cleanup();

    // ---- screen stream ----
    void CaptureLoop();
    void HandleInputEvent(const std::vector<uint8_t>& payload);
    bool SendFrameLocked(MsgType type, const std::vector<uint8_t>& payload);

    Socket::sock_t m_fd = Socket::kInvalid;   // owned by the accept thread until TakeFd()
    Socket::sock_t m_threadFd = Socket::kInvalid; // owned by the session thread
    std::string m_peerIp;
    uint16_t m_filePort = 0; // this app's file-channel port (advertised in AuthResult)
    Callbacks m_cb;

    std::thread m_thread;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_connected{false};
    std::atomic<bool> m_view{false};
    std::atomic<bool> m_finished{false};

    // capture/encode state (owned by the capture thread once started)
    std::unique_ptr<Capturer> m_capturer;
    std::unique_ptr<TestPatternSource> m_testPattern;
    bool m_usingTestPattern = false;
    std::thread m_captureThread;
    std::atomic<bool> m_captureRunning{false};
    std::mutex m_sendMutex; // serializes sends from session + capture threads
    std::atomic<int> m_logicalW{0};
    std::atomic<int> m_logicalH{0};

    // input state (session thread)
    uint8_t m_lastModifiers = 0;
    uint8_t m_token[32] = {0};

public:
    /** True once the session thread has fully exited (safe to prune). */
    bool IsFinished() const { return m_finished; }

private:

    // manual-accept handshake
    std::mutex m_askMutex;
    std::condition_variable m_askCv;
    bool m_askDecided = false;
    bool m_askAccept = false;

    // stream state (steps 4/5)
    std::atomic<int> m_peerFps{30};
    std::atomic<int> m_peerRes{0};
};

/** Owns the host listener: one TcpServer + one HostSession per connection. */
class HostService {
public:
    HostService();
    ~HostService();

    /** Bind + start the listener; false when the port is unavailable. */
    bool Start(uint16_t port, const HostSession::Callbacks& cb);
    void Stop();

    bool IsRunning() const;
    uint16_t Port() const { return m_port; }
    int SessionCount() const;
    /** First connected session (for host-side file sends); null when idle. */
    HostSessionPtr FirstConnectedSession() const;

private:
    void OnAccept(Socket::sock_t fd, const std::string& peerIp);

    std::unique_ptr<class TcpServer> m_server;
    mutable std::mutex m_mutex;
    std::vector<HostSessionPtr> m_sessions;
    HostSession::Callbacks m_cb;
    uint16_t m_port = 0;
    std::atomic<bool> m_running{false};
};

} // namespace sdk

#endif // STARDESK_SESSION_HOST_SESSION_H_
