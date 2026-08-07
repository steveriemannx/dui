#ifndef STARDESK_UI_REMOTE_WINDOW_H_
#define STARDESK_UI_REMOTE_WINDOW_H_

#include "dui/dui.h"

#include "../app/AppConfig.h"
#include "../session/ClientSession.h"
#include "VectorArt.h"

namespace sdk {

/** Remote-desktop view: decoded frames on a BitmapControl, a self-drawn
 *  toolbar (disconnect / send file / fit / 1:1 / fullscreen) and a status
 *  line (fps / latency / resolution / view-only badge). Fullscreen via
 *  WindowBase::EnterFullscreen, ESC exits (dui built-in).
 */
class RemoteWindow : public ui::WindowImplBase, public IAppListener
{
    typedef ui::WindowImplBase BaseClass;
public:
    /** Takes ownership of the (already connected) session. */
    RemoteWindow(std::unique_ptr<ClientSession> session);
    virtual ~RemoteWindow() override;

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs) override;
    virtual void OnInitWindow() override;

    virtual void OnThemeChanged() override { ApplyTheme(); }
    virtual void OnLanguageChanged() override { ApplyLanguage(); }

    void ApplyTheme();
    void ApplyLanguage();

    /** UI thread: render the latest frame into the BitmapControl. */
    void OnFrameReady(int w, int h, const std::vector<uint8_t>& rgba,
                      int cursorXN, int cursorYN);
    void OnStats(int fps, int latencyMs);

    /** Called when the window closes (session cleanup hook). */
    void SetClosedCallback(std::function<void()> cb) { m_onClosed = std::move(cb); }

private:
    void ToggleFullscreen();
    void Disconnect();
    void ToggleFit();
    void UpdateStatus();

    // ---- remote input forwarding (control mode only) ----
    void SendMouse(int kind, const ui::UiPoint& pt, uint8_t buttons);
    void SendButtonMsg(bool down, int button, const ui::UiPoint& pt);
    void SendKey(uint16_t vkCode, uint32_t modifierKey, bool down);
    bool MapToRemote(const ui::UiPoint& pt, double& nx, double& ny);

    virtual LRESULT OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey,
                                 const ui::NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnKeyUpMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey,
                               const ui::NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnMouseMoveMsg(const ui::UiPoint& pt, uint32_t modifierKey, bool bFromNC,
                                   const ui::NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnMouseWheelMsg(int32_t wheelDelta, const ui::UiPoint& pt, uint32_t modifierKey,
                                    const ui::NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnMouseLButtonDownMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                          const ui::NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnMouseLButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                        const ui::NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnMouseRButtonDownMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                          const ui::NativeMsg& nativeMsg, bool& bHandled) override;
    virtual LRESULT OnMouseRButtonUpMsg(const ui::UiPoint& pt, uint32_t modifierKey,
                                        const ui::NativeMsg& nativeMsg, bool& bHandled) override;

    std::unique_ptr<ClientSession> m_session;
    std::function<void()> m_onClosed;
    bool m_disconnecting = false;

    // UI controls
    ui::BitmapControl* m_bitmap = nullptr;
    ThemeLabel* m_titleLabel = nullptr;
    ThemeLabel* m_badgeLabel = nullptr;
    ThemeLabel* m_statusLabel = nullptr;
    PillButton* m_fitPill = nullptr;
    PillButton* m_fullscreenPill = nullptr;
    IconButton* m_sendFileBtn = nullptr;
    IconButton* m_disconnectBtn = nullptr;
    ui::VBox* m_toolbar = nullptr;

    bool m_fitMode = true;
    bool m_fullscreen = false;
    int m_lastFps = 0;
    int m_lastLatency = 0;
    bool m_hasStats = false;

    // frame data cached between OnFrameReady and paint
    std::mutex m_uiMutex;
    std::vector<uint8_t> m_frameCache;
    int m_cacheW = 0;
    int m_cacheH = 0;
    int m_cursorXN = -1;
    int m_cursorYN = -1;
    bool m_framePending = false;
};

} // namespace sdk

#endif // STARDESK_UI_REMOTE_WINDOW_H_
