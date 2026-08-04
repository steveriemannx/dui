#include "dui/Utils/TrayIcon.h"
#include "dui/Core/Window.h"

#if defined(DUI_BUILD_FOR_SDL)
#include <SDL3/SDL.h>
#endif

namespace ui
{

#if defined(DUI_BUILD_FOR_SDL) || defined(DUI_BUILD_FOR_WAYLAND)

class TrayIconImpl : public TrayIcon
{
public:
    explicit TrayIconImpl(const Window* pWindow) : m_pWindow(pWindow) {}
    virtual ~TrayIconImpl() override {}

    virtual bool SetIcon(const Window* pWindow, const DString& iconFilePath) override { (void)pWindow; (void)iconFilePath; return false; }
    virtual bool SetTooltip(const DString& tooltip) override { (void)tooltip; return false; }
    virtual bool ShowBalloon(const DString& title, const DString& content, uint32_t timeoutMs) override { (void)title; (void)content; (void)timeoutMs; return false; }
    virtual bool Hide() override { return false; }
    virtual bool Show() override { return false; }
    virtual bool IsTrayVisible() const override { return false; }
    virtual bool Remove() override { return false; }
    virtual void* GetTrayHandle() const override { return nullptr; }

private:
    const Window* m_pWindow;
};

std::unique_ptr<TrayIcon> TrayIcon::Create(const Window* pWindow, const DString& iconFilePath, const DString& tooltip)
{
    (void)iconFilePath; (void)tooltip;
    return std::make_unique<TrayIconImpl>(pWindow);
}

#endif // DUI_BUILD_FOR_SDL || DUI_BUILD_FOR_WAYLAND

} // namespace ui

