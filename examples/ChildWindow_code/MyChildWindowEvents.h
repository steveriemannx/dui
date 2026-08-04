#ifndef EXAMPLES_MY_CHILD_WINDOW_EVENTS_H_
#define EXAMPLES_MY_CHILD_WINDOW_EVENTS_H_

// dui
#include "dui/dui.h"

#include <chrono>
#include <memory>

// FPS calculation utility for drawing
class FPSCounter;

// Window drawing implementation
class ChildWindowPaint;

// Window drawing manager
class ChildWindowPaintScheduler;

/** Event interface of the child window
 */
class MyChildWindowEvents : public ui::ChildWindowEvents
{
public:
    MyChildWindowEvents(ui::ChildWindow* pChildWindow, size_t nChildWindowIndex, ChildWindowPaintScheduler* pPaintScheduler);
    virtual ~MyChildWindowEvents() override;

    /** Get the associated child window
    */
    ui::ChildWindow* GetChildWindow() const;

    /** Whether it is currently in the continuous drawing state
    */
    bool IsPaintFps() const;

protected:
    /** Window drawing (SDL_EVENT_WINDOW_EXPOSED/WM_PAINT)
     * @param [in] rcPaint The rectangular area that needs to be updated for this drawing
     * @param [in] nativeMsg The raw message content received from the system
     *             SDL implementation: nativeMsg.uMsg is SDL_EVENT_WINDOW_EXPOSED, and nativeMsg.wParam is an SDL_Window* pointer
     *             Windows implementation: nativeMsg.uMsg is WM_PAINT, and nativeMsg.wParam is the HWND handle of the window
     * @param [out] bHandled Whether the message has been handled; returning true means the message was handled successfully and need not be passed to the window procedure; returning false means the message is passed on to the window procedure
     * @return The message handling result; if the application handled this message, zero should be returned
     */
    virtual LRESULT OnPaintMsg(const ui::UiRect& rcPaint, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

private:
    // The associated child window
    ui::ControlPtrT<ui::ChildWindow> m_pChildWindow;

    // The associated drawing implementation object
    std::unique_ptr<ChildWindowPaint> m_childWindowPaint;

    // Window drawing manager
    ChildWindowPaintScheduler* m_pPaintScheduler;

    // FPS counter
    std::unique_ptr<FPSCounter> m_fps;

    // Time of the last FPS display
    std::chrono::high_resolution_clock::time_point m_lastPaintTime;

    // FPS display control
    ui::ControlPtrT<ui::Label> m_pFpsLabel;

    // Whether to start dynamic drawing
    bool m_bPaintFps;
};

#endif // EXAMPLES_MY_CHILD_WINDOW_EVENTS_H_
