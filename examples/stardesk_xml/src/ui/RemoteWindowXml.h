#ifndef STARDESK_XML_UI_REMOTE_WINDOW_H_
#define STARDESK_XML_UI_REMOTE_WINDOW_H_

#include "dui/dui.h"

#include "examples/stardesk/src/app/AppConfig.h"
#include "examples/stardesk/src/session/ClientSession.h"

namespace sdk {

/** Remote view window, XML mode: skin in stardesk_xml/remote.xml.
 *  Same display/input logic as the pure-code RemoteWindow.
 */
class RemoteWindowXml : public ui::WindowImplBase, public IAppListener
{
    typedef ui::WindowImplBase BaseClass;
public:
    RemoteWindowXml(std::unique_ptr<ClientSession> session);
    virtual ~RemoteWindowXml() override;

    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;
    virtual void OnInitWindow() override;

    virtual void OnThemeChanged() override { ApplyTheme(); }
    virtual void OnLanguageChanged() override { ApplyLanguage(); }

    void ApplyTheme();
    void ApplyLanguage();

    void SetClosedCallback(std::function<void()> cb) { m_onClosed = std::move(cb); }

private:
    void OnFrameReady(int w, int h, const std::vector<uint8_t>& rgba,
                      int cursorXN, int cursorYN);
    void OnStats(int fps, int latencyMs);
    void UpdateStatus();

    // input forwarding (control mode only)
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

    ui::BitmapControl* m_bitmap = nullptr;
    ui::Label* m_statusLabel = nullptr;
    ui::Button* m_fitBtn = nullptr;
    ui::Button* m_fullscreenBtn = nullptr;

    bool m_fitMode = true;
    bool m_fullscreen = false;
    int m_lastFps = 0;
    int m_lastLatency = 0;
    bool m_hasStats = false;
};

} // namespace sdk

#endif // STARDESK_XML_UI_REMOTE_WINDOW_H_
