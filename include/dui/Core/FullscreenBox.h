#ifndef UI_CORE_FULLSCREEN_BOX_H_
#define UI_CORE_FULLSCREEN_BOX_H_

#include "dui/Core/Box.h"
#include "dui/Core/ControlPtrT.h"

namespace ui 
{
class Window;

/** The root container when in fullscreen
*/
class DUI_API FullscreenBox: public Box
{
    typedef Box BaseClass;
public:
    FullscreenBox(Window* pWindow);
    virtual ~FullscreenBox() override;

    //The control type
    virtual DString GetType() const override;

public:
    /** Enter control fullscreen
    * @param [in] pOldRoot The original root container interface
    * @param [in] pFullscreenControl The interface of the control to be fullscreen
    * @param [in] exitButtonClass The Class name of the exit fullscreen button; if empty, the exit fullscreen button is not shown
    */
    bool EnterControlFullscreen(Box* pOldRoot, Control* pFullscreenControl, const DString& exitButtonClass);

    /** Update the fullscreen control (when already in the control fullscreen state)
    * @param [in] pFullscreenControl The interface of the control to be fullscreen
    * @param [in] exitButtonClass The Class name of the exit fullscreen button; if empty, the exit fullscreen button is not shown
    */
    bool UpdateControlFullscreen(Control* pFullscreenControl, const DString& exitButtonClass);

    /** Exit control fullscreen
    */
    void ExitControlFullscreen();

    /** Handle the dynamic display of the fullscreen button
    * @param [in] pt The current mouse position, client area coordinates
    */
    void ProcessFullscreenButtonMouseMove(const UiPoint& pt);

    /** Get the control displayed in fullscreen
    * @return Returns the interface of the control displayed in fullscreen, or nullptr if there is no fullscreen control
    */
    Control* GetFullscreenControl() const;

    /** Get the original Root container
    */
    Box* GetOldRoot() const;

    /** Get whether the original state of the window was maximized
    */
    bool IsWindowOldMaximized() const;

    /** Get whether the original state of the window was fullscreen
    */
    bool IsWindowOldFullscreen() const;

private:
    /** Extract the control from its original container
    */
    void RemoveControlFromBox(Control* pFullscreenControl);

    /** Restore the fullscreen control to its original container
    */
    void RestoreControlToBox();

    /** Update the "exit fullscreen" button
    */
    void UpdateExitFullscreenBtn(const DString& exitButtonClass);

private:
    /** The original Root container
    */
    BoxPtr m_pOldRoot;

private:
    /** The control in the current fullscreen state
    */
    ControlPtr m_pFullscreenControl;

    /** The original parent control
    */
    BoxPtr m_pOldParent;

    /** The original index in the parent control
    */
    size_t m_nOldItemIndex;

    /** The original outer margin of the control
    */
    UiMargin m_rcOldMargin;

private:
    /** The exit fullscreen button
    */
    ControlPtr m_pExitFullscreenBtn;

    /** The Class attribute of the exit fullscreen button
    */
    DString m_exitButtonClass;

private:
    /** The original state of the window was maximized
    */
    bool m_bWindowMaximized;

    /** The original state of the window was fullscreen
    */
    bool m_bWindowFullscreen;
};

} // namespace ui

#endif // UI_CORE_FULLSCREEN_BOX_H_
