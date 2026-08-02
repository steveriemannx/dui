#ifndef UI_CONTROL_IPADDRESS_H_
#define UI_CONTROL_IPADDRESS_H_

#include "duilib/Box/HBox.h"

namespace ui
{
/** IP address control
*/
class RichEdit;
class DUILIB_API IPAddress: public HBox
{
    typedef HBox BaseClass;
public:
    explicit IPAddress(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

    /** Let the control get focus
     */
    virtual void SetFocus() override;

    /** Set the IP address
    */
    void SetIPAddress(const DString& ipAddress);

    /** Get the IP address
    */
    DString GetIPAddress() const;

protected:

    /** Initialize interface
    */
    virtual void OnInit() override;

    /** Dispatch the message to the message handler
     * @param[in] msg The message content
     */
    virtual void SendEventMsg(const EventArgs& msg) override;

    /** Focus change event of the edit controls
    */
    void OnKillFocusEvent(RichEdit* pRichEdit, Control* pNewFocus);

private:
    /** List of edit controls
    */
    std::vector<RichEdit*> m_editList;

    /** The control that last had focus
    */
    RichEdit* m_pLastFocus;

    /** IP address
    */
    UiString m_ipAddress;
};

}//namespace ui

#endif //UI_CONTROL_IPADDRESS_H_
