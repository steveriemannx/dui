#include "dui/Utils/MessageBoxWnd.h"
#include "dui/Core/GlobalManager.h"

namespace ui
{

MessageBoxWnd::MessageBoxWnd() :
    m_buttonFlags(kButtonsOK),
    m_iconType(kIconNone),
    m_pTextLabel(nullptr),
    m_pTitleLabel(nullptr),
    m_pIconControl(nullptr),
    m_bTextId(false)
{
}

MessageBoxWnd::~MessageBoxWnd()
{
}

std::string MessageBoxWnd::GetSkinFolder()
{
    return "public/messagebox";
}

std::string MessageBoxWnd::GetSkinFile()
{
    return "messagebox.xml";
}

int32_t MessageBoxWnd::Show(ui::Window* pParentWindow,
                             const std::string& text,
                             const std::string& title,
                             uint32_t buttonFlags,
                             IconType iconType,
                             const ButtonText* pButtonText,
                             bool bTextId)
{
    //Fallback: when no button is specified, show at least "OK"
    if ((buttonFlags & (kButtonOK | kButtonCancel | kButtonYes | kButtonNo | kButtonRetry)) == 0) {
        buttonFlags = kButtonsOK;
    }

    //A DoModal window does not delete itself when it closes; the caller manages its lifetime
    MessageBoxWnd* pMsgBox = new MessageBoxWnd();
    pMsgBox->m_text = text;
    pMsgBox->m_title = title;
    pMsgBox->m_buttonFlags = buttonFlags;
    pMsgBox->m_iconType = iconType;
    if (pButtonText != nullptr) {
        pMsgBox->m_buttonText = *pButtonText;
    }
    pMsgBox->m_bTextId = bTextId;

    //Center the window. In bTextId mode, the native window title used by the taskbar/Alt+Tab
    //must be the translated text, and the multi-language ID must not be passed in directly
    //(otherwise the taskbar would show the raw "STRID_XXX" string);
    //the third parameter windowId receives the text ID, which is used by mechanisms such as
    //window state remembering.
    std::string strWindowTitle = title;
    if (bTextId) {
        strWindowTitle = ui::GlobalManager::Instance().Lang().GetStringViaID(title);
    }
    ui::WindowCreateParam createParam(strWindowTitle, true, bTextId ? title : "");

    //bCloseByEsc=true: ESC closes the window, and DoModal returns kWindowCloseCancel(2)
    //bCloseByEnter=true: Enter closes the window, and DoModal returns kWindowCloseOK(1), which is then
    //                   normalized according to the button combination
    //Title bar close button: the framework CloseWnd() passes kWindowCloseNormal(0) by default, which is
    //                        normalized in NormalizeResult as well
    int32_t nResult = pMsgBox->DoModal(pParentWindow, createParam, true, true);
    delete pMsgBox;

    if (nResult < 0) {
        return nResult;
    }
    return NormalizeResult(nResult, buttonFlags);
}

void MessageBoxWnd::OnInitWindow()
{
    BaseClass::OnInitWindow();

    m_pTitleLabel = dynamic_cast<ui::Label*>(FindControl("msg_title"));
    if (m_pTitleLabel != nullptr) {
        if (m_bTextId) {
            m_pTitleLabel->SetTextId(m_title);
        }
        else {
            m_pTitleLabel->SetText(m_title);
        }        
    }

    m_pTextLabel = dynamic_cast<ui::Label*>(FindControl("msg_text"));
    if (m_pTextLabel != nullptr) {
        if (m_bTextId) {
            m_pTextLabel->SetTextId(m_text);
        }
        else {
            m_pTextLabel->SetText(m_text);
        }        
    }

    //The title bar close button does not take part in the TAB focus cycle
    //(only the function buttons switch among themselves)
    ui::Button* pCloseButton = dynamic_cast<ui::Button*>(FindControl("btn_window_close"));
    if (pCloseButton != nullptr) {
        pCloseButton->SetTabStop(false);
    }

    m_pIconControl = FindControl("msg_icon");
    if (m_pIconControl != nullptr) {
        std::string strIcon;
        switch (m_iconType) {
        case kIconInfo:
            strIcon = "file='msg_info.svg'";
            break;
        case kIconWarning:
            strIcon = "file='msg_warning.svg'";
            break;
        case kIconError:
            strIcon = "file='msg_error.svg'";
            break;
        case kIconQuestion:
            strIcon = "file='msg_question.svg'";
            break;
        default:
            break;
        }
        if (strIcon.empty()) {
            m_pIconControl->SetVisible(false);
        }
        else {
            m_pIconControl->SetBkImage(strIcon);
        }
    }

    InitButtons();

#if defined (DUI_BUILD_FOR_WIN)
    //Register with the Enter key passthrough list of NativeWindow_Windows, so that IsDialogMessageDuiLib
    //lets VK_RETURN through for this window
    ui::NativeWindow_Windows::SetEnterKeyPassthrough(NativeWnd()->GetHWND(), true);
#endif
}

void MessageBoxWnd::OnPreCloseWindow()
{
#if defined (DUI_BUILD_FOR_WIN)
    //Unregister the Enter key passthrough, so that a reused window handle does not affect other modal dialogs
    ui::NativeWindow_Windows::SetEnterKeyPassthrough(NativeWnd()->GetHWND(), false);
#endif
    BaseClass::OnPreCloseWindow();
}

LRESULT MessageBoxWnd::OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
    if (vkCode == kVK_TAB) {
        //On the first TAB navigation, turn on the focus rectangle display for all function buttons
        //(only the focused button actually shows it)
        const std::string btnNames[] = { "btn_retry", "btn_yes", "btn_no", "btn_ok", "btn_cancel" };
        for (const std::string& name : btnNames) {
            ui::Button* pBtn = dynamic_cast<ui::Button*>(FindControl(name));
            if (pBtn != nullptr) {
                pBtn->SetShowFocusRect(true);
            }
        }
    }
    return BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
}

void MessageBoxWnd::InitButtons()
{
    //Default button text (read from the multi-language resource table of the framework, and falls back
    //to the built-in text when it is missing)
    if (m_buttonText.ok.empty()) {
        if (m_bTextId) {
            m_buttonText.ok = "STRID_MSGBOX_OK";
        }
        else {
            m_buttonText.ok = ui::GlobalManager::Instance().Lang().GetStringViaID("STRID_MSGBOX_OK");
        }        
        if (m_buttonText.ok.empty()) { m_buttonText.ok = "Ok"; }
    }
    if (m_buttonText.cancel.empty()) {
        if (m_bTextId) {
            m_buttonText.cancel = "STRID_MSGBOX_CANCEL";
        }
        else {
            m_buttonText.cancel = ui::GlobalManager::Instance().Lang().GetStringViaID("STRID_MSGBOX_CANCEL");
        }        
        if (m_buttonText.cancel.empty()) { m_buttonText.cancel = "Cancel"; }
    }
    if (m_buttonText.yes.empty()) {
        if (m_bTextId) {
            m_buttonText.yes = "STRID_MSGBOX_YES";
        }
        else {
            m_buttonText.yes = ui::GlobalManager::Instance().Lang().GetStringViaID("STRID_MSGBOX_YES");
        }        
        if (m_buttonText.yes.empty()) { m_buttonText.yes = "Yes"; }
    }
    if (m_buttonText.no.empty()) {
        if (m_bTextId) {
            m_buttonText.no = "STRID_MSGBOX_NO";
        }
        else {
            m_buttonText.no = ui::GlobalManager::Instance().Lang().GetStringViaID("STRID_MSGBOX_NO");
        }        
        if (m_buttonText.no.empty()) { m_buttonText.no = "No"; }
    }
    if (m_buttonText.retry.empty()) {
        if (m_bTextId) {
            m_buttonText.retry = "STRID_MSGBOX_RETRY";
        }
        else {
            m_buttonText.retry = ui::GlobalManager::Instance().Lang().GetStringViaID("STRID_MSGBOX_RETRY");
        }        
        if (m_buttonText.retry.empty()) { m_buttonText.retry = "Retry"; }
    }

    //The button definitions: the display order is the same as the layout order in the XML
    struct ButtonInit
    {
        const std::string* pName;
        uint32_t flag;
        const std::string* pText;
        int32_t result;
    };
    const std::string nameRetry  = "btn_retry";
    const std::string nameYes    = "btn_yes";
    const std::string nameNo     = "btn_no";
    const std::string nameOK     = "btn_ok";
    const std::string nameCancel = "btn_cancel";

    const ButtonInit buttons[] = {
        { &nameRetry,  kButtonRetry,  &m_buttonText.retry,  kResultRetry  },
        { &nameYes,    kButtonYes,    &m_buttonText.yes,    kResultYes    },
        { &nameNo,     kButtonNo,     &m_buttonText.no,     kResultNo     },
        { &nameOK,     kButtonOK,     &m_buttonText.ok,     kResultOK     },
        { &nameCancel, kButtonCancel, &m_buttonText.cancel, kResultCancel }
    };

    const int32_t nDefaultResult = GetDefaultResult();
    ui::Button* pDefaultButton = nullptr;

    for (const ButtonInit& item : buttons) {
        ui::Button* pButton = dynamic_cast<ui::Button*>(FindControl(*item.pName));
        if (pButton == nullptr) {
            continue;
        }
        if ((m_buttonFlags & item.flag) == 0) {
            //Not part of the current button combination: hide it and do not respond
            pButton->SetVisible(false);
            continue;
        }

        //The buttons are hidden by default in the XML (to avoid all of them flashing), and are shown
        //here when they match the combination
        pButton->SetVisible(true);
        if (m_bTextId) {
            if (!GlobalManager::Instance().Lang().GetStringViaID(*item.pText).empty()) {
                pButton->SetTextId(*item.pText);
            }
            else {
                pButton->SetText(*item.pText);
            }
        }
        else {
            pButton->SetText(*item.pText);
        }        
        if (item.result == nDefaultResult) {
            //The default button uses the blue highlighted style
            pButton->SetClass("btn_global_blue_80x30");
            pDefaultButton = pButton;
        }
        else {
            pButton->SetClass("btn_global_white_80x30");
        }

        //Note: the pointers inside item point to stack variables of InitButtons, so item itself must not
        //be captured by the asynchronous callback; only the return value needed by the callback is copied
        //here, by value.
        const int32_t nItemResult = item.result;
        pButton->AttachClick([this, nItemResult](const ui::EventArgs& /*args*/) {
            CloseWnd(nItemResult);
            return true;
        });
    }

    //The default button gets the focus, so pressing Enter is equivalent to clicking it
    if (pDefaultButton != nullptr) {
        pDefaultButton->SetFocus();
    }
}

int32_t MessageBoxWnd::GetDefaultResult() const
{
    if ((m_buttonFlags & kButtonOK) != 0) {
        return kResultOK;
    }
    if ((m_buttonFlags & kButtonYes) != 0) {
        return kResultYes;
    }
    if ((m_buttonFlags & kButtonRetry) != 0) {
        return kResultRetry;
    }
    return kResultOK;
}

int32_t MessageBoxWnd::NormalizeResult(int32_t nResult, uint32_t buttonFlags)
{
    //A function button that was explicitly clicked: Retry/Yes/No/Cancel; return the corresponding result directly.
    //Note: kResultCancel(2) has the same value as kWindowCloseCancel(2), the return value when the window is
    //closed with the ESC key, so "clicking the Cancel button" and "pressing ESC" both hit this branch and both
    //return kResultCancel, which is semantically correct.
    if ((nResult == kResultRetry) || (nResult == kResultYes) ||
        (nResult == kResultNo) || (nResult == kResultCancel)) {
        return nResult;
    }

    //The Enter key returns kResultOK(1): if the combination has no "OK", map it to the default button
    if (nResult == kResultOK) {
        if ((buttonFlags & kButtonOK) != 0) {
            return kResultOK;
        }
        if ((buttonFlags & kButtonYes) != 0) {
            return kResultYes;
        }
        if ((buttonFlags & kButtonRetry) != 0) {
            return kResultRetry;
        }
        return kResultOK;
    }

    //The title bar close button (kWindowCloseNormal=0): the user gave up the choice, so the cancel semantics
    //is returned. Even if the combination has no "Cancel" button it must not be mapped to the default
    //"OK/Yes/Retry"; otherwise the user would merely close the window and still trigger an affirmative action
    //(for example, when "Yes+No" is shown for "Save?" and X is clicked, the old logic returned IDYES and saved
    //by mistake). This is consistent with the behavior of the Win32 MessageBox: X means giving up the choice.
    return kResultCancel;
}

} //namespace ui
