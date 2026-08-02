#ifndef EXAMPLES_CONTROLS_FORM_H_
#define EXAMPLES_CONTROLS_FORM_H_

// duilib
#include "duilib/duilib.h"

class ControlForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    ControlForm();
    virtual ~ControlForm() override;

    /** Resource-related interfaces
     * The GetSkinFolder interface sets the skin resource path of the window to be drawn
     * The GetSkinFile interface sets the xml description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

    /** Called after the window is closed, for subclasses to do some cleanup work
    */
    virtual void OnCloseWindow() override;

private:

    /** Show the menu
    * @param [in] point Display position coordinates, in screen coordinates
    */
    void ShowPopupMenu(const ui::UiPoint& point, ui::Control* pRelatedControl);

    /** Register edit box events
    */
    void AttachRichEditEvents(ui::RichEdit* edit);

    /**
     * Task function dispatched to the worker thread to read xml data
     */
    void LoadRichEditData();

    /**
     * Interface used to update the UI content after the worker thread finishes reading the xml
     */
    void OnResourceFileLoaded(const DString& xml);

    /**
     * Interface for dynamically updating the progress bar
     */
    void OnProgressValueChagned(float value);

    /** Show the color picker window
    * @param [in] bDoModal Whether to use a modal dialog
    */
    void ShowColorPicker(bool bDoModal);

    /** Show a modal dialog
    */
    void ShowDoModalDlg();

    /** Show the control test window
    */
    void ShowTestWindow();

    /** Show the control test window
    */
    void ShowAnimationWindow();

private:
    /** Hotkey message (WM_HOTKEY)
    * @param [in] hotkeyId ID of the hotkey
    * @param [in] vkCode Virtual key code
    * @param [in] modifierKey Modifier key flags, valid values: ModifierKey::kAlt, ModifierKey::kControl, ModifierKey::kShift, ModifierKey::kWin
    * @param [in] nativeMsg Raw message content received from the system
    * @param [out] bHandled Whether the message has been handled; returning true means the message has been handled successfully and does not need to be passed to the window procedure; returning false means the message continues to be passed to the window procedure for handling
    * @return Returns the result of message processing; if the application handles the message, it should return zero
    */
    virtual LRESULT OnHotKeyMsg(int32_t hotkeyId, ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled) override;

private:
    /** Show/hide the tray icon
    */
    void ShowTrayIcon(bool bShow);

    /** Tray icon message callback function
     * @param [in] msgType Message type
     * @param [in] x Mouse X coordinate (screen coordinates)
     * @param [in] y Mouse Y coordinate (screen coordinates)
     */
    void OnTrayIconMessage(ui::TrayIconMessageType msgType, int32_t x, int32_t y);

    /** Show the tray icon menu
    */
    void ShowTrayMenu(int32_t x, int32_t y);

private:
    /** Tray icon interface
    */
    std::unique_ptr<ui::TrayIcon> m_pTrayIcon;
};

#endif //EXAMPLES_CONTROLS_FORM_H_
