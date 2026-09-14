#include "dui/Core/ThreadMessage.h"
#include "dui/Core/Keyboard.h"
#include "dui/Core/CursorManager.h"
#include "dui/Core/DpiAwareness.h"
#include "dui/Core/ToolTip.h"
#include "dui/Utils/MonitorUtil.h"
#include "dui/Utils/Clipboard.h"
#include "dui/Utils/FileDialog.h"
#include "dui/Utils/SystemUtil.h"
#include "dui/Utils/StringConvert.h"

// The X11 headers belong inside the guard. This file is compiled on every non-Wayland
// platform -- its name does not match the `_X11.` pattern the backend filter uses -- so
// outside the guard a Windows or macOS build has to have X11 installed to compile a
// translation unit whose entire body is switched off. That is exactly what happened on
// Windows: "cannot open include file: 'X11/Xlib.h'".
#ifdef DUI_BUILD_FOR_X11

#include "dui/Core/MessageLoop_X11.h"
#include "dui/Core/NativeWindow_X11.h"
#include <X11/keysym.h>
#include <X11/Xlib.h>

namespace ui {
class ThreadMessage::TImpl { public: uint32_t id = 0; bool stopped = false; };
ThreadMessage::ThreadMessage() : m_impl(new TImpl) {}
ThreadMessage::~ThreadMessage() { Clear(); delete m_impl; m_impl = nullptr; }
void ThreadMessage::Initialize(void*) {}
bool ThreadMessage::PostMsg(uint32_t id, WPARAM w, LPARAM l, uint32_t* error) { if (error) *error = 0; return !m_impl->stopped && id == m_impl->id && MessageLoop_X11::PostUserEvent(id, w, l); }
void ThreadMessage::RemoveDuplicateMsg(uint32_t id) { if (id == m_impl->id) MessageLoop_X11::RemoveDuplicateMsg(id); }
void ThreadMessage::SetMessageCallback(uint32_t id, const ThreadMessageCallback& cb) { if (m_impl->id) MessageLoop_X11::RemoveUserMessageCallback(m_impl->id); m_impl->id = id; if (id) MessageLoop_X11::AddUserMessageCallback(id, cb); }
void ThreadMessage::Clear() { if (!m_impl || m_impl->stopped) return; if (m_impl->id) MessageLoop_X11::RemoveUserMessageCallback(m_impl->id); m_impl->id = 0; m_impl->stopped = true; }
void ThreadMessage::OnUserMessage(uint32_t, WPARAM, LPARAM) {}
class CursorManager::TImpl { public: CursorID id = 1; bool visible = true; };
CursorManager::CursorManager() : m_impl(new TImpl) {}
CursorManager::~CursorManager() { delete m_impl; }
bool CursorManager::SetCursor(CursorType) { return true; }
bool CursorManager::SetImageCursor(const Window*, const FilePath&) { return false; }
bool CursorManager::ShowCursor(bool show) { m_impl->visible = show; return true; }
CursorID CursorManager::GetCursorID() const { return m_impl->id; }
bool CursorManager::SetCursorByID(CursorID id) { m_impl->id = id; return true; }
bool Keyboard::IsKeyDown(VirtualKeyCode key) { Display* d = MessageLoop_X11::GetDisplay(); if (!d) return false; char keys[32] = {}; XQueryKeymap(d, keys); KeyCode code = XKeysymToKeycode(d, static_cast<KeySym>(key)); return code && (keys[code >> 3] & (1 << (code & 7))); }
bool Keyboard::IsNumLockOn() { return false; }
bool Keyboard::IsScrollLockOn() { return false; }
bool Keyboard::IsCapsLockOn() { return false; }
std::string Keyboard::GetKeyName(VirtualKeyCode key, bool) { const char* name = XKeysymToString(static_cast<KeySym>(key)); return name ? StringConvert::UTF8ToT(name) : ""; }
DpiAwareness::DpiAwareness() : m_dpiAwarenessMode(DpiAwarenessMode::kPerMonitorDpiAware_V2) {}
DpiAwareness::~DpiAwareness() = default;
bool DpiAwareness::InitDpiAwareness(DpiAwarenessMode mode) { m_dpiAwarenessMode = mode == DpiAwarenessMode::kDpiUnaware ? mode : DpiAwarenessMode::kPerMonitorDpiAware_V2; return true; }
DpiAwarenessMode DpiAwareness::SetDpiAwareness(DpiAwarenessMode mode) { InitDpiAwareness(mode); return m_dpiAwarenessMode; }
DpiAwarenessMode DpiAwareness::GetDpiAwareness() const { return m_dpiAwarenessMode; }
float MonitorUtil::GetPrimaryMonitorDisplayScale() { return 1.0f; }
float MonitorUtil::GetWindowDisplayScale(const WindowBase*, float& density) { density = 1.0f; return 1.0f; }
class ToolTip::TImpl {};
ToolTip::ToolTip() : m_impl(new TImpl) {}
ToolTip::~ToolTip() { delete m_impl; }
void ToolTip::SetMouseTracking(WindowBase*, bool) {}
void ToolTip::ShowToolTip(WindowBase*, const UiRect&, uint32_t, const UiPoint&, const std::string&) {}
void ToolTip::HideToolTip() {}
void ToolTip::ClearMouseTracking() {}
void ToolTip::DestroyToolTip() {}
bool Clipboard::GetClipboardText(std::wstring& text) { text.clear(); return false; }
bool Clipboard::GetClipboardText(std::string& text) { text.clear(); return false; }
bool Clipboard::SetClipboardText(const std::wstring&) { return false; }
bool Clipboard::SetClipboardText(const std::string&) { return false; }
bool FileDialog::BrowseForFolder(Window*, FilePath&, const FilePath&) { return false; }
bool FileDialog::BrowseForFolders(Window*, std::vector<FilePath>&, const FilePath&) { return false; }
bool FileDialog::BrowseForFile(Window*, FilePath&, bool, const std::vector<FileType>&, int32_t, const std::string&, const std::string&, const FilePath&) { return false; }
bool FileDialog::BrowseForFiles(Window*, std::vector<FilePath>&, const std::vector<FileType>&, int32_t, const std::string&, const FilePath&) { return false; }
bool SystemUtil::OpenUrl(const std::string&) { return false; }
bool SystemUtil::ShowMessageBox(const Window*, const std::string&, const std::string&) { return false; }
bool NativeWindow_X11::IsWindowForeground() const { return IsWindowFocused(); }
void NativeWindow_X11::KeepParentActive() {}
} // namespace ui
#endif
