#include "FileTransfer.h"

#include "../app/AppConfig.h"
#include "../net/Protocol.h"
#include "../net/TcpClient.h"
#include "../net/TcpServer.h"

#include "dui/Utils/FilePathUtil.h"

#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

namespace sdk {

namespace {

const uint64_t kChunkSize = 64 * 1024;

bool ReadChunk(FILE* f, uint64_t offset, uint8_t* buf, size_t len)
{
#ifdef _WIN32
    if (_fseeki64(f, (__int64)offset, SEEK_SET) != 0) {
#else
    if (fseeko(f, (off_t)offset, SEEK_SET) != 0) {
#endif
        return false;
    }
    return fread(buf, 1, len, f) == len;
}

} // namespace

// ---------------------------------------------------------------- lifecycle

FileTransfer::FileTransfer()
{
}

FileTransfer::~FileTransfer()
{
    Stop();
}

bool FileTransfer::GetToken(uint8_t out[32]) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::memcpy(out, m_token, 32);
    return true;
}

void FileTransfer::Start(uint16_t port, const uint8_t token[32], ItemCb cb)
{
    if (m_running) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::memcpy(m_token, token, 32);
        m_cb = std::move(cb);
    }
    m_server = std::make_unique<TcpServer>();
    const bool started = m_server->Start(port, [this](Socket::sock_t fd, const std::string& ip) {
        OnAccept(fd, ip);
    });
    m_running = started;
}

void FileTransfer::SetSessionToken(const uint8_t token[32])
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::memcpy(m_token, token, 32);
}

void FileTransfer::SetItemCallback(ItemCb cb)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_cb = std::move(cb);
}

void FileTransfer::Stop()
{
    if (!m_running && !m_sending) {
        return;
    }
    m_running = false;
    if (m_server) {
        m_server->Stop();
        m_server.reset();
    }
    // wake the blocked recv loops WITHOUT releasing fd numbers (fd-reuse race);
    // the worker threads close their own fds
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (Socket::sock_t fd : m_connFds) {
            Socket::Shutdown(fd);
        }
        Socket::Shutdown(m_sendFd);
    }
    if (m_sending) {
        m_sending = false;
        if (m_sendThread.joinable()) {
            m_sendThread.join();
        }
    }
    std::vector<std::thread> conns;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        conns.swap(m_connThreads);
        m_connFds.clear();
        m_sendFd = Socket::kInvalid;
    }
    for (auto& t : conns) {
        if (t.joinable()) {
            t.join();
        }
    }
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_items.clear();
    }
}

void FileTransfer::OnAccept(Socket::sock_t fd, const std::string& /*peerIp*/)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connFds.push_back(fd);
        m_connThreads.emplace_back(&FileTransfer::RecvLoop, this, fd);
    }
}

void FileTransfer::Notify(uint32_t id)
{
    ItemCb cb;
    Item item;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_items.find(id);
        if (it == m_items.end()) {
            return;
        }
        item = it->second;
        cb = m_cb;
    }
    if (cb) {
        cb(item);
    }
}

// ---------------------------------------------------------------- receive side

void FileTransfer::RecvLoop(Socket::sock_t fd)
{
    // ---- auth ----
    Frame frame;
    if (!RecvFrame(fd, frame, 10000) || frame.type != MsgType::FileAuth ||
        frame.payload.size() != 32) {
        Socket::Close(fd);
        return;
    }
    uint8_t expected[32];
    GetToken(expected);
    if (std::memcmp(expected, frame.payload.data(), 32) != 0) {
        Socket::Close(fd);
        return; // bad token
    }

    uint32_t currentId = 0;
    FILE* outFile = nullptr;
    ui::FilePath outPath;
    uint64_t expectedSize = 0;

    auto failCurrent = [&]() {
        if (outFile != nullptr) {
            std::fclose(outFile);
            outFile = nullptr;
        }
        if (currentId != 0) {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                auto it = m_items.find(currentId);
                if (it != m_items.end()) {
                    it->second.state = ItemState::Failed;
                }
            }
            Notify(currentId);
            currentId = 0;
        }
    };

    while (m_running) {
        if (!RecvFrame(fd, frame, 30000)) {
            failCurrent();
            break;
        }
        PayloadReader r(frame.payload.data(), frame.payload.size());
        switch (frame.type) {
        case MsgType::FileStart: {
            if (currentId != 0) {
                failCurrent(); // no parallel files on one connection
            }
            const uint32_t id = r.GetU32();
            std::string name;
            r.GetString(name);
            const uint64_t size = r.GetU64();
            if (!r.Ok() || name.empty() || name.size() > 512) {
                continue;
            }
            // reject path separators in the name (path traversal guard)
            for (char c : name) {
                if (c == '/' || c == '\\') {
                    name = "unnamed";
                    break;
                }
            }
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                Item item;
                item.id = id;
                item.name = name;
                item.size = size;
                item.incoming = true;
                m_items[id] = item;
            }
            currentId = id;
            expectedSize = size;

            // always accept in phase 1; save into the receive directory
            const AppConfig& cfg = App::Instance().Config();
            ui::FilePath dir(cfg.receiveDir.empty()
                                 ? AppConfig::GetDefaultReceiveDir().ToString()
                                 : cfg.receiveDir);
            if (!dir.IsExistsDirectory()) {
                ui::FilePathUtil::CreateDirectories(dir.ToString());
            }
            outPath = dir;
            outPath.JoinFilePath(ui::FilePath(ui::StringConvert::UTF8ToT(name)));
            outFile = fopen(outPath.NativePathA().c_str(), "wb");
            if (outFile == nullptr) {
                PayloadWriter w;
                w.PutU32(id);
                w.PutU8(0);
                SendFrame(fd, MsgType::FileAccept, w.Result());
                failCurrent();
                continue;
            }
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                auto it = m_items.find(id);
                if (it != m_items.end()) {
                    it->second.state = ItemState::Transferring;
                }
            }
            Notify(id);
            PayloadWriter w;
            w.PutU32(id);
            w.PutU8(1);
            SendFrame(fd, MsgType::FileAccept, w.Result());
            break;
        }
        case MsgType::FileChunk: {
            if (currentId == 0 || outFile == nullptr) {
                continue;
            }
            const uint32_t id = r.GetU32();
            const uint64_t offset = r.GetU64();
            const size_t dataLen = frame.payload.size() - 12;
            if (!r.Ok() || id != currentId) {
                continue;
            }
            if (dataLen > 0) {
#ifdef _WIN32
                const bool seekOk = _fseeki64(outFile, (__int64)offset, SEEK_SET) == 0;
#else
                const bool seekOk = fseeko(outFile, (off_t)offset, SEEK_SET) == 0;
#endif
                if (!seekOk || fwrite(frame.payload.data() + 12, 1, dataLen, outFile) != dataLen) {
                    failCurrent();
                    break;
                }
            }
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                auto it = m_items.find(id);
                if (it != m_items.end()) {
                    it->second.transferred = offset + dataLen;
                }
            }
            Notify(id);
            break;
        }
        case MsgType::FileEnd: {
            if (currentId == 0) {
                continue;
            }
            const uint32_t id = r.GetU32();
            if (id != currentId || outFile == nullptr) {
                continue;
            }
            std::fclose(outFile);
            outFile = nullptr;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                auto it = m_items.find(id);
                if (it != m_items.end()) {
                    it->second.state = ItemState::Done;
                    it->second.transferred = it->second.size;
                }
            }
            Notify(id);
            PayloadWriter w;
            w.PutU32(id);
            SendFrame(fd, MsgType::FileDone, w.Result());
            currentId = 0;
            break;
        }
        case MsgType::FileAbort: {
            const uint32_t id = r.GetU32();
            if (id == currentId) {
                if (outFile != nullptr) {
                    std::fclose(outFile);
                    outFile = nullptr;
                }
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    auto it = m_items.find(id);
                    if (it != m_items.end()) {
                        it->second.state = (it->second.transferred == 0)
                                               ? ItemState::Rejected
                                               : ItemState::Cancelled;
                    }
                }
                Notify(id);
                currentId = 0;
            }
            break;
        }
        default:
            break;
        }
    }

    if (outFile != nullptr) {
        std::fclose(outFile);
    }
    Socket::Close(fd);
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_connFds.erase(std::remove(m_connFds.begin(), m_connFds.end(), fd),
                        m_connFds.end());
    }
}

// ---------------------------------------------------------------- send side

bool FileTransfer::SendFiles(const std::string& host, uint16_t port,
                             const uint8_t token[32],
                             const std::vector<ui::FilePath>& files,
                             ItemCb cb, std::string& err)
{
    if (m_sending) {
        err = "busy";
        return false;
    }
    if (files.empty()) {
        err = "empty";
        return false;
    }
    m_sending = true;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cb = std::move(cb);
        std::memcpy(m_token, token, 32);
        for (const ui::FilePath& f : files) {
            Item item;
            item.id = m_nextId++;
            item.name = ui::StringConvert::TToUTF8(f.GetFileName());
            item.size = f.IsExistsFile() ? (uint64_t)f.GetFileSize() : 0;
            item.state = ItemState::Waiting;
            m_items[item.id] = item;
        }
    }
    m_sendThread = std::thread([this, host, port, files]() {
        const Socket::sock_t fd = TcpClient::ConnectTo(host, port, 5000);
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_sendFd = fd;
        }
        if (fd == Socket::kInvalid) {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (auto& kv : m_items) {
                if (!kv.second.incoming && kv.second.state == ItemState::Waiting) {
                    kv.second.state = ItemState::Failed;
                }
            }
            for (auto& kv : m_items) {
                if (!kv.second.incoming && kv.second.state == ItemState::Failed) {
                    Notify(kv.first);
                }
            }
            m_sending = false;
            return;
        }
        PayloadWriter w;
        w.PutBytes(m_token, 32);
        SendFrame(fd, MsgType::FileAuth, w.Result());
        SendLoop(fd, files);
        Socket::Close(fd);
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_sendFd = Socket::kInvalid;
        }
        m_sending = false;
    });
    return true;
}

void FileTransfer::SendLoop(Socket::sock_t fd, const std::vector<ui::FilePath>& files)
{
    for (const ui::FilePath& path : files) {
        if (!m_running && !m_sending) {
            break;
        }
        FILE* f = fopen(path.NativePathA().c_str(), "rb");
        if (f == nullptr) {
            // mark failed
            std::lock_guard<std::mutex> lock(m_mutex);
            for (auto& kv : m_items) {
                if (!kv.second.incoming && kv.second.state == ItemState::Waiting &&
                    kv.second.name == ui::StringConvert::TToUTF8(path.GetFileName())) {
                    kv.second.state = ItemState::Failed;
                    Notify(kv.first);
                }
            }
            continue;
        }
        // file size
#ifdef _WIN32
        _fseeki64(f, 0, SEEK_END);
        const uint64_t size = (uint64_t)_ftelli64(f);
#else
        fseeko(f, 0, SEEK_END);
        const uint64_t size = (uint64_t)ftello(f);
#endif

        uint32_t id = 0;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            for (auto& kv : m_items) {
                if (!kv.second.incoming && kv.second.state == ItemState::Waiting &&
                    kv.second.name == ui::StringConvert::TToUTF8(path.GetFileName())) {
                    id = kv.first;
                    kv.second.size = size;
                    kv.second.state = ItemState::Transferring;
                    break;
                }
            }
        }
        if (id == 0) {
            std::fclose(f);
            continue;
        }
        Notify(id);

        // FileStart
        {
            PayloadWriter w;
            w.PutU32(id);
            w.PutString(ui::StringConvert::TToUTF8(path.GetFileName()));
            w.PutU64(size);
            if (!SendFrame(fd, MsgType::FileStart, w.Result())) {
                std::fclose(f);
                break;
            }
        }
        // FileAccept (we always accept in phase 1, but wait for the peer)
        Frame frame;
        if (!RecvFrame(fd, frame, 15000) || frame.type != MsgType::FileAccept ||
            frame.payload.size() < 5) {
            std::fclose(f);
            std::lock_guard<std::mutex> lock(m_mutex);
            m_items[id].state = ItemState::Rejected;
            Notify(id);
            break;
        }
        PayloadReader r(frame.payload.data(), frame.payload.size());
        const bool accept = r.GetU8() != 0;
        if (!accept) {
            std::fclose(f);
            std::lock_guard<std::mutex> lock(m_mutex);
            m_items[id].state = ItemState::Rejected;
            Notify(id);
            continue;
        }

        // chunks
        uint64_t offset = 0;
        std::vector<uint8_t> buf(kChunkSize);
        bool aborted = false;
        while (offset < size) {
            const size_t len = (size_t)std::min<uint64_t>(kChunkSize, size - offset);
            if (!ReadChunk(f, offset, buf.data(), len)) {
                aborted = true;
                break;
            }
            PayloadWriter w;
            w.PutU32(id);
            w.PutU64(offset);
            w.PutBytes(buf.data(), len);
            if (!SendFrame(fd, MsgType::FileChunk, w.Result())) {
                aborted = true;
                break;
            }
            offset += len;
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_items[id].transferred = offset;
            }
            Notify(id);

            // cancellation check (peer may abort while we send)
            if (m_running && Socket::WaitReadable(fd, 0)) {
                Frame f2;
                if (RecvFrame(fd, f2, 100) && f2.type == MsgType::FileAbort) {
                    aborted = true;
                    break;
                }
            }
        }
        std::fclose(f);

        if (aborted) {
            PayloadWriter w;
            w.PutU32(id);
            SendFrame(fd, MsgType::FileAbort, w.Result());
            std::lock_guard<std::mutex> lock(m_mutex);
            m_items[id].state = ItemState::Cancelled;
            Notify(id);
            break;
        }

        // FileEnd + FileDone
        {
            PayloadWriter w;
            w.PutU32(id);
            if (!SendFrame(fd, MsgType::FileEnd, w.Result())) {
                break;
            }
        }
        if (!RecvFrame(fd, frame, 15000) || frame.type != MsgType::FileDone) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_items[id].state = ItemState::Failed;
            Notify(id);
            break;
        }
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_items[id].state = ItemState::Done;
        }
        Notify(id);
    }
}

void FileTransfer::Cancel(uint32_t id)
{
    // local side: mark cancelled; the peer learns via socket close or abort
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_items.find(id);
    if (it != m_items.end() && (it->second.state == ItemState::Waiting ||
                                it->second.state == ItemState::Transferring)) {
        it->second.state = ItemState::Cancelled;
        Notify(id);
    }
}

} // namespace sdk
