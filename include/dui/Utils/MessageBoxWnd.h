#ifndef DUI_UTILS_MESSAGE_BOX_WND_H_
#define DUI_UTILS_MESSAGE_BOX_WND_H_

#include "dui/Utils/WinImplBase.h"
#include "dui/Control/Label.h"
#include "dui/Control/Button.h"

namespace ui
{

/** A modal message box with a custom-drawn skin
*   Features:
*   1. Supports 5 button combinations: OK / OK+Cancel / Yes+No / Yes+No+Cancel / Retry+Cancel
*   2. Supports 4 standard icons: information/warning/error/question, and no icon at all
*   3. Shown modally through Window::DoModal, blocks the parent window, and returns the button the user clicked
*   4. The ESC key and the title bar close button cancel it; the Enter key triggers the default button
*   5. The message text supports multiple lines (automatic word wrap), and the window height adapts to the text
*   6. The button text can be customized by the caller (for multi-language scenarios)
*/
class DUI_API MessageBoxWnd : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;

public:
    /** The flag bit of a single button; the flags can be combined bitwise
    */
    enum ButtonFlag : uint32_t
    {
        kButtonOK      = 0x01,  //The "OK" button
        kButtonCancel  = 0x02,  //The "Cancel" button
        kButtonYes     = 0x04,  //The "Yes" button
        kButtonNo      = 0x08,  //The "No" button
        kButtonRetry   = 0x10   //The "Retry" button
    };

    /** Common button combinations (corresponding to the MB_* style of the Win32 MessageBox)
    */
    enum ButtonFlags : uint32_t
    {
        kButtonsOK            = kButtonOK,
        kButtonsOKCancel      = kButtonOK | kButtonCancel,
        kButtonsYesNo         = kButtonYes | kButtonNo,
        kButtonsYesNoCancel   = kButtonYes | kButtonNo | kButtonCancel,
        kButtonsRetryCancel   = kButtonRetry | kButtonCancel
    };

    /** The icon type
    */
    enum IconType : uint32_t
    {
        kIconNone      = 0,  //No icon
        kIconInfo      = 1,  //Information (a blue i)
        kIconWarning   = 2,  //Warning (a yellow !)
        kIconError     = 3,  //Error (a red X)
        kIconQuestion  = 4   //Question (a blue ?)
    };

    /** The modal return value; the values are consistent with the ID* return values of the Win32 MessageBox:
    *   IDOK=1 IDCANCEL=2 IDRETRY=4 IDYES=6 IDNO=7
    */
    enum Result : int32_t
    {
        kResultOK      = 1,
        kResultCancel  = 2,
        kResultRetry   = 4,
        kResultYes     = 6,
        kResultNo      = 7
    };

    /** Custom button text (the default text is used when an item is empty)
    */
    struct ButtonText
    {
        std::string ok;       //OK
        std::string cancel;   //Cancel
        std::string yes;      //Yes
        std::string no;       //No
        std::string retry;    //Retry
    };

public:
    /** Show a message box modally
    * @param [in] pParentWindow The parent window (cannot be operated while the message box is modal); can be nullptr
    * @param [in] text The message content; supports '\n' line breaks, and long text is wrapped automatically
    * @param [in] title The window title
    * @param [in] buttonFlags The button combination, see enum ButtonFlags
    * @param [in] iconType The icon type, see enum IconType
    * @param [in] pButtonText The custom button text; can be nullptr (use the default text)
    * @param [in] bTextId All texts (text, title, pButtonText) are multi-language IDs, so they support language switching
    * @return Returns the button selected by the user (enum Result); returns -1 if the window cannot be created
    */
    static int32_t Show(ui::Window* pParentWindow,
                        const std::string& text,
                        const std::string& title,
                        uint32_t buttonFlags = kButtonsOK,
                        IconType iconType = kIconNone,
                        const ButtonText* pButtonText = nullptr,
                        bool bTextId = false);

private:
    MessageBoxWnd();
    virtual ~MessageBoxWnd() override;

    /** Resource related interfaces
    */
    virtual std::string GetSkinFolder() override;
    virtual std::string GetSkinFile() override;

    /** After the window is created, fill in the text/icon and show or hide the buttons according to the parameters
    */
    virtual void OnInitWindow() override;

    /** When the window is about to close, unregister the Enter key passthrough
    */
    virtual void OnPreCloseWindow() override;

    /** Key down: when TAB navigation starts, turn on the focus rectangle display of the function buttons as needed
    *   (so that no focus ring is shown when the window first pops up)
    */
    virtual LRESULT OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled) override;

private:
    /** According to the button combination, initialize the visibility, text, style and events of the buttons
    */
    void InitButtons();

    /** Return the return value of the default button: "OK" first, then "Yes", and finally "Retry"
    */
    int32_t GetDefaultResult() const;

    /** Normalize the return value of DoModal:
    *   The Enter key returns kResultOK, but if the combination has no "OK" it is mapped to the default button;
    *   The ESC key, the title bar close button and clicking the "Cancel" button all return the cancel semantics
    *   (kResultCancel); even if the combination has no "Cancel" button they must not be mapped to the default
    *   button, so that the user giving up the choice never triggers an affirmative action.
    */
    static int32_t NormalizeResult(int32_t nResult, uint32_t buttonFlags);

private:
    //The message content
    std::string m_text;

    //The window title
    std::string m_title;

    //The button combination flags
    uint32_t m_buttonFlags;

    //The icon type
    IconType m_iconType;

    //The button text
    ButtonText m_buttonText;

    //The message content control
    ui::Label* m_pTextLabel;

    //The title bar text control
    ui::Label* m_pTitleLabel;

    //The icon control
    ui::Control* m_pIconControl;

    //All texts (text, title, pButtonText) are multi-language IDs, so they support language switching
    bool m_bTextId;
};

} //namespace ui

#endif // DUI_UTILS_MESSAGE_BOX_WND_H_
