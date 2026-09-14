#include "dui/Utils/TrayIcon.h"
#ifdef DUI_BUILD_FOR_X11
namespace ui {
class X11TrayIcon final : public TrayIcon {
public:
 bool SetIcon(const Window*, const std::string&) override { return false; }
 bool SetTooltip(const std::string&) override { return true; }
 bool ShowBalloon(const std::string&, const std::string&, uint32_t) override { return false; }
 bool Hide() override { return true; }
 bool Show() override { return true; }
 bool IsTrayVisible() const override { return false; }
 bool Remove() override { return true; }
 void* GetTrayHandle() const override { return nullptr; }
};
std::unique_ptr<TrayIcon> TrayIcon::Create(const Window*, const std::string&, const std::string&) { return std::make_unique<X11TrayIcon>(); }
}
#endif
