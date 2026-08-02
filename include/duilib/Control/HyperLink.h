#ifndef UI_CONTROL_HYPER_LINK_H_
#define UI_CONTROL_HYPER_LINK_H_

#include "duilib/Control/Label.h"

namespace ui {

/** Text with a hyperlink; if the URL is empty, it can be used as a normal text button
*/
class DUILIB_API HyperLink : public Label
{
    typedef Label BaseClass;
public:
    explicit HyperLink(Window* pWindow):
        Label(pWindow)
    {
    }
    /// Override the parent class methods to provide custom features; please refer to the parent class declarations
    virtual DString GetType() const override { return DUI_CTR_HYPER_LINK; }
    virtual void SetAttribute(const DString& strName, const DString& strValue) override
    {
        if (strName == _T("url")) {
            m_url = strValue;
        }
        else {
            BaseClass::SetAttribute(strName, strValue);
        }
    }

    virtual void Activate(const EventArgs* /*pMsg*/) override
    {
        if (!IsActivatable()) {
            return;
        }
        DString url = m_url.c_str();
        SendEvent(kEventLinkClick, (WPARAM)url.c_str());
    }

public:
    /** Listen to the event of the hyperlink being clicked
     * @param [in] callback The callback function after the hyperlink is clicked
     * @param [in] callbackID The ID corresponding to this callback function (used to remove the callback function)
     */
    void AttachLinkClick(const EventCallback& callback, EventCallbackID callbackID = 0) { AttachEvent(kEventLinkClick, callback, callbackID); }

private:
    /** URL
    */
    UiString m_url;
};

}    // namespace ui

#endif // UI_CONTROL_HYPER_LINK_H_
