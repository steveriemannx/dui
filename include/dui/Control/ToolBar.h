#ifndef UI_CONTROL_TOOL_BAR_H_
#define UI_CONTROL_TOOL_BAR_H_

#include "dui/Box/HBox.h"
#include "dui/Core/ControlPtrT.h"
#include <functional>
#include <vector>

namespace ui
{

/** Toolbar control
*   It is derived from HBox and hosts a row of tool buttons, separator lines and similar elements; it is a common
*   shell element of editors / IDEs / office software.
*
*   Main capabilities:
*   1. Child controls are arranged horizontally, using all layout capabilities of HBox; separator lines in the
*      toolbar can use the Line control directly (vertical="true");
*   2. Overflow handling (overflow): when the available width is not enough for the trailing child controls, those
*      child controls are hidden automatically and a "more" button is shown at the right end; clicking that button
*      pops up a flyout listing the collected items, and a click on one of the items in the flyout notifies the
*      application through the OverflowItemActivatedEvent callback.
*
*   The XML node name is "ToolBar" and it inherits all attributes of HBox.
*/
class DUI_API ToolBar : public HBox
{
    typedef HBox BaseClass;
public:
    explicit ToolBar(Window* pWindow);
    virtual ~ToolBar() override;

public:
    /// Overrides of the base class interface for custom behavior; see the Control / Box documentation for the details
    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;
    virtual void SetPos(UiRect rc) override;

public:
    /** Prototype of the callback invoked when an overflow item is activated (clicked with the mouse)
    * @param [in] itemName Name of the clicked control (that is, the name attribute of the child control)
    */
    typedef std::function<void(const std::string& itemName)> OverflowItemActivatedEvent;

    /** Registers the callback invoked when an overflow item is activated
    * @param [in] callback Callback function
    */
    void AttachOverflowItemActivated(OverflowItemActivatedEvent callback);

    /** Clears all callback functions
    */
    void ClearOverflowItemActivated();

    /** Sets whether the overflow handling is enabled (enabled by default)
    */
    void SetOverflowEnabled(bool bEnabled);
    bool IsOverflowEnabled() const;

    /** Sets the style Class name of the "more" button (a style from global.xml; an empty string means no style)
    */
    void SetOverflowButtonClass(const std::string& strClass);
    std::string GetOverflowButtonClass() const;

    /** Sets the width of the "more" button (pixels; 0 means the width follows the content)
    */
    void SetOverflowButtonWidth(int32_t nWidth);
    int32_t GetOverflowButtonWidth() const;

    /** Sets the text shown by the "more" button (">>" by default)
    */
    void SetOverflowButtonText(const std::string& strText);
    std::string GetOverflowButtonText() const;

    /** Whether child controls are currently collected (hidden) automatically
    */
    bool IsOverflowing() const;

private:
    /** Creates the "more" button (idempotent: it is not created twice)
    */
    void CreateOverflowButton();

    /** Computes and applies the overflow handling (called while laying out)
    * @param [in] rc Layout area of this control
    */
    void UpdateOverflow(const UiRect& rc);

    /** Pops up the flyout list of the collected items
    */
    void ShowOverflowFlyout();

    /** Fires the overflow item activated callbacks
    */
    void FireOverflowItemActivated(const std::string& itemName);

    /** Decides whether a control is a child control of this container
    */
    bool IsChildControl(Control* pControl) const;

    /** Decides whether a collection contains a control
    */
    static bool ContainsItem(const std::vector<Control*>& items, Control* pControl);

private:
    //Whether the overflow handling is enabled
    bool m_bOverflowEnabled;

    //"More" button
    ControlPtr m_pOverflowButton;

    //Style Class name of the "more" button
    UiString m_overflowButtonClass;

    //Width of the "more" button (0 means the width follows the content)
    int32_t m_nOverflowButtonWidth;

    //Text shown by the "more" button
    UiString m_overflowButtonText;

    //Child controls currently collected (hidden) automatically
    std::vector<Control*> m_hiddenItems;

    //Callbacks invoked when an overflow item is activated
    std::vector<OverflowItemActivatedEvent> m_callbackList;

    //Layout re-entrancy guard flag
    bool m_bInLayout;
};

} // namespace ui

#endif // UI_CONTROL_TOOL_BAR_H_
