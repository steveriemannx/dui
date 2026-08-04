#ifndef UI_CONTROL_BUTTON_H_
#define UI_CONTROL_BUTTON_H_

#include "dui/Control/Label.h"
#include "dui/Core/Keyboard.h"
#include "dui/Core/Window.h"

namespace ui {

template<typename InheritType = Control>
class ButtonTemplate : public LabelTemplate<InheritType>
{
    typedef LabelTemplate<InheritType> BaseClass;
public:
    explicit ButtonTemplate(Window* pWindow);

    /// Override the parent class methods to provide personalized features; please refer to the parent class declarations
    virtual DString GetType() const override;
    virtual void Activate(const EventArgs* pMsg) override;
    virtual void HandleEvent(const EventArgs& msg) override;
    virtual uint32_t GetControlFlags() const override;

    /** Whether this control can be placed on the caption bar (to handle NC message responses)
     * @return Returns true if it can, false if it cannot; the default is false
     */
    virtual bool CanPlaceCaptionBar() const override;
};

template<typename InheritType>
ButtonTemplate<InheritType>::ButtonTemplate(Window* pWindow):
    LabelTemplate<InheritType>(pWindow)
{
    this->SetTextStyle(TEXT_VCENTER | TEXT_HCENTER | TEXT_END_ELLIPSIS | TEXT_NOCLIP | TEXT_SINGLELINE, false);
}

template<typename InheritType>
inline DString ButtonTemplate<InheritType>::GetType() const { return DUI_CTR_BUTTON; }

template<>
inline DString ButtonTemplate<Box>::GetType() const { return DUI_CTR_BUTTONBOX; }

template<>
inline DString ButtonTemplate<HBox>::GetType() const { return DUI_CTR_BUTTONHBOX; }

template<>
inline DString ButtonTemplate<VBox>::GetType() const { return DUI_CTR_BUTTONVBOX; }

template<typename InheritType>
uint32_t ui::ButtonTemplate<InheritType>::GetControlFlags() const
{
    return this->IsKeyboardEnabled() && this->IsEnabled() && this->IsAllowTabStop() ? UIFLAG_TABSTOP : UIFLAG_DEFAULT;
}

template<typename InheritType>
void ButtonTemplate<InheritType>::HandleEvent(const EventArgs& msg)
{
    if (msg.eventType == kEventKeyDown) {
        if (this->IsEnabled() && this->IsKeyboardEnabled()) {
            //When the Enter key or the Space key is pressed, trigger the button response action
            if (msg.vkCode == kVK_SPACE || msg.vkCode == kVK_RETURN) {
                Activate(&msg);
                return;
            }
        }
    }
    BaseClass::HandleEvent(msg);
}

template<typename InheritType>
void ButtonTemplate<InheritType>::Activate(const EventArgs* pMsg)
{
    if (!this->IsActivatable()) {
        return;
    }
    if (pMsg != nullptr) {
        EventArgs newMsg = *pMsg;
        newMsg.eventData = pMsg->eventType;
        newMsg.eventType = kEventNone;
        this->SendEvent(kEventClick, newMsg);
    }
    else {
        this->SendEvent(kEventClick);
    }
}

template<typename InheritType>
bool ButtonTemplate<InheritType>::CanPlaceCaptionBar() const
{
    return true;
}

typedef ButtonTemplate<Control> Button;
typedef ButtonTemplate<Box> ButtonBox;
typedef ButtonTemplate<HBox> ButtonHBox;
typedef ButtonTemplate<VBox> ButtonVBox;

}    // namespace ui

#endif // UI_CONTROL_BUTTON_H_
