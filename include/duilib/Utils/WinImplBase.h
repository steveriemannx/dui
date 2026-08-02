#ifndef UI_UTILS_WINIMPLBASE_H_
#define UI_UTILS_WINIMPLBASE_H_

#include "duilib/duilib_defs.h"
#include "duilib/Core/Window.h"

namespace ui
{
/** Implements a window with a title bar, with support for maximize, minimize and restore buttons
* This implementation supports window shadows, but only provides window shadows for windows with the WS_EX_LAYERED attribute.
* This class encapsulates 2 window modes:
* (1) Normal mode, a window without an attached shadow: it can be configured in the XML configuration file <Window> as follows (example):
*              shadow_attached="false" layered_window="false" ; no alpha attribute is needed, and the alpha attribute is not supported;
*              The above configuration is equivalent to: layered_window="false", without setting the shadow_attached attribute
* (2) Window mode with an attached shadow, which requires a layered window (with the WS_EX_LAYERED attribute); it can be configured in the XML configuration file <Window> as follows (example):
*              shadow_attached="true" layered_window="true" ; alpha is optional, sets the window opacity, and if not set it defaults to 255;
*              The above configuration is equivalent to: layered_window="true", without setting the shadow_attached attribute
*/
class DUILIB_API WindowImplBase : public Window
{
    typedef Window BaseClass;
public:
    WindowImplBase();
    virtual ~WindowImplBase() override;

public:
    /** Called when the window is created; implemented by the subclass to obtain the window skin folder
    * @return The subclass must implement and return the window skin folder, as a relative path
    */
    virtual DString GetSkinFolder() override;

    /** Called when the window is created; implemented by the subclass to obtain the window skin XML description file
    * @return The subclass must implement and return the window skin XML description file
    *         The returned content can be the XML file content (a string starting with the character '<'),
    *         or a file path (a string not starting with the character '<'); the file must be findable under the GetSkinFolder() path
    */
    virtual DString GetSkinFile() override;

public:
    /** Called when the control to be created is not a standard control name
    * @param [in] strClass The control name
    * @return Returns a pointer to a custom control; in general, create the custom control according to the strClass parameter
    */
    virtual Control* CreateControl(const DString& strClass) override;

protected:
    /** Called after the window has been created, for the subclass to do some initialization work
    */
    virtual void OnInitWindow() override;

    /** Completes the initialization of the UI layout; the layout information such as the position and size of each control is initialized, for the subclass to adjust the layout after the UI starts
    */
    virtual void OnInitLayout() override;

    /** Called when the window is about to be closed, for the subclass to do some finishing work
    */
    virtual void OnPreCloseWindow() override;

    /** Called after the window has been closed, for the subclass to do some finishing work
    */
    virtual void OnCloseWindow() override;

    /** Called when the window is destroyed; this is the last message of the window (the default implementation of this class clears resources and destroys the window object)
    */
    virtual void OnFinalMessage() override;

protected:
    /** The window size has changed (WM_SIZE)
    * @param [in] sizeType The type that triggered the window size change
    * @param [in] newWindowSize The new window size (width and height)
    * @param [in] nativeMsg The raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true indicates the message has been handled successfully and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for processing
    * @return Returns the result of processing the message; if the application processes this message, it should return zero
    */
    virtual LRESULT OnSizeMsg(WindowSizeType sizeType, const UiSize& newWindowSize, const NativeMsg& nativeMsg, bool& bHandled) override;

    /** Enter the fullscreen state
    */
    virtual void OnWindowEnterFullscreen() override;

    /** Exit the fullscreen state
    */
    virtual void OnWindowExitFullscreen() override;

    /** Switch between the system title bar and the self-drawn title bar
    */
    virtual void OnUseSystemCaptionBarChanged() override;

    /** The DPI scale factor of the window has changed; update the control sizes and layout (for subclass use)
    * @param [in] nOldScaleFactor The old DPI scale percentage
    * @param [in] nNewScaleFactor The new DPI scale percentage, consistent with the value of Dpi().GetDisplayScaleFactor(); this value may be the same as nOldScaleFactor
    */
    virtual void OnWindowDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor) override;

protected:
    /** Enter the maximized state
    */
    virtual void OnWindowMaximized();

    /** Restore from the maximized state
    */
    virtual void OnWindowRestored();

    /** Enter the minimized state
    */
    virtual void OnWindowMinimized();

    /** Determine whether it has maximize and minimize buttons
    * @param [out] bMinimizeBox Returns true if it has a minimize button
    * @param [out] bMaximizeBox Returns true if it has a maximize button
    */
    virtual bool HasMinMaxBox(bool& bMinimizeBox, bool& bMaximizeBox) const override;

    /** Determine whether a point is on the maximize or restore button
    */
    virtual bool IsPtInMaximizeRestoreButton(const UiPoint& pt) const override;

protected:
    /** Initializing the window data (internal function)
    */
    virtual void PreInitWindow() override;

private:
    /** Called when a window function button is clicked
    * @param [in] param The carried parameters
    * @return Always returns true
    */
    bool OnButtonClick(const EventArgs& param);

    /** Called when the title bar is double-clicked
    * @param [in] param The carried parameters
    * @return Always returns true
    */
    bool OnTitleBarDoubleClick(const EventArgs& param);

    /** Handle the state of the maximize/restore button
    */
    void ProcessMaxRestoreStatus();

    /** Bind the caption buttons (close/min/max/restore/fullscreen) and the title bar
     *  double-click handling. Called from PreInitWindow and again from OnInitWindow
     *  (for pure-code windows whose control tree is built in OnInitWindow).
     *  Idempotent: the buttons are bound only once.
    */
    void BindCaptionButtons();

private:
    /** The interface of the maximize button
    */
    Control* m_pMaxButton;
    std::weak_ptr<WeakFlag> m_maxButtonFlag;

    /** The interface of the minimize button
    */
    Control* m_pMinButton;
    std::weak_ptr<WeakFlag> m_minButtonFlag;

    /** The interface of the restore button
    */
    Control* m_pRestoreButton;
    std::weak_ptr<WeakFlag> m_restoreButtonFlag;

    /** Whether the caption buttons have been bound
    */
    bool m_bCaptionButtonsBound = false;
};
}

#endif // UI_UTILS_WINIMPLBASE_H_
