#ifndef _UI_CONTROL_DATETIME_WND_WINDOWS_H_
#define _UI_CONTROL_DATETIME_WND_WINDOWS_H_

#include "dui/Core/UiTypes.h"
#include <ctime>

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

namespace ui
{
class DateTime;
class WindowBase;

/** The implementation of the date time selection control window (Windows platform)
*/
class DateTimeWnd
{
public:
    explicit DateTimeWnd(DateTime* pOwner);
    ~DateTimeWnd();

public:
    //Initialize
    bool Init(DateTime* pOwner);

    //Update the position of the window
    void UpdateWndPos();

    //Show the window
    void ShowWindow();

private:
    //Register the control window class
    bool RegisterSuperClass();

    //The window class name
    DString GetWindowClassName() const;

    //The window procedure function
    static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    //Called when the window is destroyed; this is the last message of the window (the default implementation of this class cleans up resources and calls the OnDeleteSelf function to destroy the window object)
    void OnFinalMessage();

    //The handler of window messages; the first handler entered after receiving a message from the system
    LRESULT WindowMessageProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    UiRect CalPos();
    HFONT CreateHFont() const;
    struct tm SystemTimeToStdTime(const SYSTEMTIME& sysTime) const;
    SYSTEMTIME StdTimeToSystemTime(const struct tm& tmTime) const;

private:
    HWND m_hDateTimeWnd;
    WNDPROC m_OldWndProc; //The original window procedure function
    DateTime* m_pOwner;
    bool m_bInit;
    bool m_bDropOpen;
    SYSTEMTIME m_oldSysTime;
    HFONT m_hFont;
};

} //namespace ui

#endif // (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

#endif // _UI_CONTROL_DATETIME_WND_WINDOWS_H_
