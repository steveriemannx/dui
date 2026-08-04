#ifndef UI_CORE_WINDOW_MANAGER_H_
#define UI_CORE_WINDOW_MANAGER_H_

#include "dui/Core/UiTypes.h"
#include "dui/Core/ControlPtrT.h"

namespace ui 
{
class Window;
class WindowBase;

/** Used to manage the lifecycle of all windows
 */
class DUI_API WindowManager
{
public:
    WindowManager();
    ~WindowManager();
    WindowManager(const WindowManager&) = delete;
    WindowManager& operator = (const WindowManager&) = delete;

public:
    /** Add a window interface (mainly used for operations such as redrawing after skin/language switching, resource synchronization, etc.)
    * @param [in] pWindow The window interface
    */
    void AddWindow(Window* pWindow);

    /** Remove a window
    * @param [in] pWindow The window interface
    */
    void RemoveWindow(Window* pWindow);

    /** Determine whether the window is currently contained
    * @param [in] pWindow The window interface
    */
    bool HasWindow(Window* pWindow) const;
    bool HasWindowBase(WindowBase* pWindowBase) const;

    /** Get the list of all windows
    */
    std::vector<WindowPtr> GetAllWindowList() const;

    /** Get all windows under the specified window class
    * @param [in] windowClassName The window class name passed in when creating the window
    */
    std::vector<WindowPtr> GetAllWindowList(const DString& windowClassName) const;

    /** Get the window corresponding to the specified window ID
    * @param [in] windowId Window ID; theoretically this ID is unique
    * @return Returns the window corresponding to this window ID; if there are multiple, the first matching window is returned
    */
    WindowPtr GetWindowById(const DString& windowId) const;

    /** Clear
    */
    void Clear();

private:
    /** Window list
    */
    std::vector<WindowPtr> m_windowList;
};

} //namespace ui 

#endif //UI_CORE_WINDOW_MANAGER_H_
