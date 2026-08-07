#ifndef STARDESK_NET_PROTOCOL_H_
#define STARDESK_NET_PROTOCOL_H_

#include "Socket.h"

#include <cstdint>
#include <string>
#include <vector>

namespace sdk {

/** StarDesk wire protocol.
 *
 *  Frame layout (little-endian):
 *    [magic u32 = 0x53444B31 ("SDK1")][type u8][payloadLen u32][payload]
 *
 *  Auth flow (control channel, TCP mainPort):
 *    client -> Hello{deviceName, mode, resCode, fps}
 *    host   -> Challenge{nonce[16]}          (after manual-accept dialog, if any)
 *    client -> Auth{sha256(nonce || password)[32]}
 *    host   -> AuthResult{ok, reason, filePort, token[32]}
 *    ... then streaming (ScreenInit/ScreenTile/CursorPos + InputEvent + Ping/Pong)
 *
 *  File channel (TCP filePort): first message FileAuth{token[32]}, then the
 *  File* messages below.
 */
enum class MsgType : uint8_t {
    Hello = 1,
    Challenge = 2,
    Auth = 3,
    AuthResult = 4,
    AcceptRequest = 5,   // host->client: manual-accept dialog is showing
    Ping = 6,
    Pong = 7,
    Disconnect = 8,

    // screen stream (host -> client)
    ScreenInit = 16,
    ScreenTile = 17,
    CursorPos = 18,
    ScreenEnd = 19,    // empty payload: end of the current tile batch - the
                       // client renders ONLY on this marker so a frame never
                       // mixes tiles from two frames (visible seams on fast
                       // scroll)
    // (ScreenTiles = 20 was a batched-tile message during development and
    // is intentionally not defined here - the per-tile ScreenTile message
    // with ScreenEnd framing is the shipping protocol)

    // input (client -> host)
    InputEvent = 20,

    // file channel
    FileAuth = 32,
    FileStart = 33,
    FileAccept = 34,
    FileChunk = 35,
    FileEnd = 36,
    FileAbort = 37,
    FileDone = 38,
};

struct Frame {
    MsgType type = MsgType::Hello;
    std::vector<uint8_t> payload;
};

// ---- framing ----
bool SendFrame(Socket::sock_t fd, MsgType type, const void* payload, size_t len);
bool SendFrame(Socket::sock_t fd, MsgType type, const std::vector<uint8_t>& payload);
/** Read one frame; returns false on error/close/timeout. */
bool RecvFrame(Socket::sock_t fd, Frame& frame, int timeoutMs);

// ---- payload building / parsing (little-endian) ----
class PayloadWriter {
public:
    void PutU8(uint8_t v);
    void PutU16(uint16_t v);
    void PutU32(uint32_t v);
    void PutU64(uint64_t v);
    void PutBytes(const void* data, size_t len);
    void PutString(const std::string& s); // u16 len + bytes
    const std::vector<uint8_t>& Result() const { return m_buf; }

private:
    std::vector<uint8_t> m_buf;
};

class PayloadReader {
public:
    PayloadReader(const uint8_t* data, size_t len) : m_p(data), m_n(len) {}

    bool Ok() const { return m_ok; }
    uint8_t GetU8();
    uint16_t GetU16();
    uint32_t GetU32();
    uint64_t GetU64();
    bool GetBytes(void* out, size_t len);
    bool GetString(std::string& out);

private:
    const uint8_t* m_p;
    size_t m_n;
    bool m_ok = true;
};

// ---- typed messages ----
enum class ConnMode : uint8_t { Control = 0, View = 1 };
enum class AuthResultCode : uint8_t { Ok = 0, WrongPassword = 1, Rejected = 2, Busy = 3 };

struct HelloMsg {
    std::string deviceName;
    ConnMode mode = ConnMode::Control;
    uint8_t resCode = 0; // 0=original 1=720p 2=1080p
    uint8_t fps = 30;    // 24/30/60
};

struct AuthResultMsg {
    AuthResultCode code = AuthResultCode::Ok;
    uint16_t filePort = 0;
    uint8_t token[32] = {0};
};

// input event payload (client -> host)
struct InputEventMsg {
    uint8_t kind = 0;      // 0=move 1=button 2=wheel 3=keyDown 4=keyUp
    uint16_t xNorm = 0;    // 0..65535 normalized x
    uint16_t yNorm = 0;    // 0..65535 normalized y
    uint8_t buttons = 0;   // bit0 left, bit1 right, bit2 middle, bit3 x1, bit4 x2
    int16_t wheelDelta = 0;
    uint16_t vkCode = 0;   // dui kVK_* (Windows VK codes)
    uint8_t modifiers = 0; // dui ModifierKey flags
    uint8_t flags = 0;     // bit0 = first press (kFirstPress)
};

// protocol constants
static const uint32_t kMagic = 0x53444B31; // "SDK1"
static const uint16_t kDiscoveryPort = 7456;
static const uint16_t kDefaultPort = 7456;

} // namespace sdk

#endif // STARDESK_NET_PROTOCOL_H_
