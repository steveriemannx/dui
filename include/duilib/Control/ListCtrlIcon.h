#ifndef UI_CONTROL_LIST_CTRL_ICON_H_
#define UI_CONTROL_LIST_CTRL_ICON_H_

#include "duilib/Control/CheckBox.h"
#include "duilib/Box/HBox.h"

namespace ui
{
/** The icon control used in the list, used to display icons
*/
class DUILIB_API ListCtrlIcon: public CheckBoxTemplate<HBox>
{
    typedef CheckBoxTemplate<HBox> BaseClass;
public:
    explicit ListCtrlIcon(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;

    /** Message handling
    */
    virtual void HandleEvent(const EventArgs& msg) override;

    /** DPI changed, update the control size and layout
    * @param [in] nOldDpiScale The old DPI scale percentage
    * @param [in] nNewDpiScale The new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:
    /** Set the associated list item
    */
    void SetListBoxItem(Control* pListBoxItem);

private:
    /** Mouse event (from message forwarding of the child control)
    * @param [in] msg The content of the mouse event
    */
    bool OnMouseEvent(const EventArgs& msg);

private:
    /** The associated list item
    */
    Control* m_pListBoxItem;
};

}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_ICON_H_
