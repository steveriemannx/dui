#ifndef UI_UTILS_TRAY_ICON_H_
#define UI_UTILS_TRAY_ICON_H_

#include "duilib/duilib_defs.h"
#include "duilib/Core/UiTypes.h"
#include <functional>
#include <memory>

namespace ui
{

class Window;

/** Tray icon message types
*/
enum class TrayIconMessageType
{
    kLeftClick,        // Left button click
    kLeftDoubleClick,  // Left button double-click      (not available on non-Windows platforms using SDL)
    kRightClick,       // Right button click
    kRightDoubleClick, // Right button double-click      (not available on non-Windows platforms using SDL)
    kMiddleClick,      // Middle button click
    kMouseMove,        // Mouse move      (not available on non-Windows platforms using SDL)
    kShowBalloon,      // Balloon notification clicked (not available on non-Windows platforms using SDL)
};

/** Tray icon message callback function
 * @param [in] msgType The message type
 * @param [in] x Mouse X coordinate (screen coordinates)
 * @param [in] y Mouse Y coordinate (screen coordinates)
 */
typedef std::function<void(TrayIconMessageType msgType, int32_t x, int32_t y)> TrayIconMessageCallback;

/** Tray icon functionality
*/
class DUILIB_API TrayIcon
{
protected:
    TrayIcon();
    TrayIcon(const TrayIcon&) = delete;
    TrayIcon& operator=(const TrayIcon&) = delete;

public:
    virtual ~TrayIcon();

    /** Create a tray icon instance
    * @param [in] pWindow Pointer to the associated window
    * @param [in] iconFilePath Icon file path (*.ico format supported; other formats are also supported, but ICO is recommended)
    * @param [in] tooltip Tray tooltip text
    * @return Pointer to the tray icon instance, or nullptr if creation fails
    */
    static std::unique_ptr<TrayIcon> Create(const Window* pWindow, const DString& iconFilePath, const DString& tooltip = _T(""));

public:
    /** Set the tray icon
    * @param [in] pWindow Pointer to the associated window (used to look up image resources)
    * @param [in] iconFilePath Icon file path (*.ico format supported; other formats are also supported, but ICO is recommended)
    * @return Returns true on success, false on failure
    */
    virtual bool SetIcon(const Window* pWindow, const DString& iconFilePath) = 0;

    /** Set the tray tooltip text
    * @param [in] tooltip The tooltip text
    * @return Returns true on success, false on failure
    */
    virtual bool SetTooltip(const DString& tooltip) = 0;

    /** Show a balloon notification
    * @param [in] title The balloon title
    * @param [in] content The balloon content
    * @param [in] timeoutMs Display timeout in milliseconds, 3000ms by default
    * @return Returns true on success, false on failure
    */
    virtual bool ShowBalloon(const DString& title, const DString& content, uint32_t timeoutMs = 3000) = 0;

    /** Hide the tray icon
    * @return Returns true if hidden successfully, false on failure
    */
    virtual bool Hide() = 0;

    /** Show the tray icon (if previously hidden)
    * @return Returns true if shown successfully, false on failure
    */
    virtual bool Show() = 0;

    /** Whether the tray icon is currently visible
    * @return Returns true if visible, false otherwise
    */
    virtual bool IsTrayVisible() const = 0;

    /** Remove the tray icon
    * @return Returns true if removed successfully, false on failure
    */
    virtual bool Remove() = 0;

    /** Get the tray handle of the internal implementation
    *@return Returns an SDL_Tray* in the SDL implementation, and the tray window handle HWND in the Windows SDK implementation
    */
    virtual void* GetTrayHandle() const = 0;

    /** Set the message callback function
    * @param [in] callback The callback function
    */
    void SetMessageCallback(TrayIconMessageCallback callback);

protected:
    /** Trigger the message callback
    */
    void NotifyMessage(TrayIconMessageType msgType, int32_t x, int32_t y);

private:
    /** Message callback function
    */
    TrayIconMessageCallback m_messageCallback;
};

} //namespace ui

#endif // UI_UTILS_TRAY_ICON_H_
