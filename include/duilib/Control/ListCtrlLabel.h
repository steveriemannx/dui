#ifndef UI_CONTROL_LIST_CTRL_LABEL_H_
#define UI_CONTROL_LIST_CTRL_LABEL_H_

#include "duilib/Control/CheckBox.h"
#include "duilib/Box/HBox.h"

namespace ui
{
/** The Label control used in lists, which displays text and provides text editing functionality events
*/
class DUILIB_API ListCtrlLabel: public CheckBoxTemplate<HBox>
{
    typedef CheckBoxTemplate<HBox> BaseClass;
public:
    explicit ListCtrlLabel(Window* pWindow);

    /** Get the control type
    */
    virtual DString GetType() const override;

    /** Message handling
    */
    virtual void HandleEvent(const EventArgs& msg) override;

    /** DPI has changed; update the control size and layout
    * @param [in] nOldDpiScale the old DPI scale percentage
    * @param [in] nNewDpiScale the new DPI scale percentage, consistent with the value of Dpi().GetScale()
    */
    virtual void ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale) override;

public:
    /** Set the rectangle area where the text is located
    */
    void SetTextRect(const UiRect& rect);

    /** Get the rectangle area where the text is located
    */
    UiRect GetTextRect() const;

    /** Set the associated list item
    */
    void SetListBoxItem(Control* pListBoxItem);

    /** Set whether text editing is supported
    */
    void SetEnableEdit(bool bEnableEdit);

    /** Get whether text editing is supported
    */
    bool IsEnableEdit() const;

private:
    /** Mouse event (from message forwarding of the sub-control)
    * @param [in] msg the content of the mouse event
    */
    bool OnMouseEvent(const EventArgs& msg);

    /** Enter the editing state
    */
    void OnItemEnterEditMode();

private:
    /** Associated list item
    */
    Control* m_pListBoxItem;

    /** Rectangle area where the text is located
    */
    UiRect m_textRect;

    /** Whether the mouse click is within the control range
    */
    bool m_bMouseDown;

    /** Whether text editing is supported
    */
    bool m_bEnableEdit;
};
}//namespace ui

#endif //UI_CONTROL_LIST_CTRL_LABEL_H_
