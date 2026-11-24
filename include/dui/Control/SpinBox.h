#ifndef UI_CONTROL_SPINBOX_H_
#define UI_CONTROL_SPINBOX_H_

#include "dui/Control/RichEdit.h"

namespace ui
{

/** Numeric input control (SpinBox): a wrapper around RichEdit that adds number input plus spin buttons
*
*   Compared with using a RichEdit spin_class directly, this control provides:
*   - number_only mode enabled by default, no extra configuration needed
*   - step support: the spin buttons and the up/down arrow keys change the value by the step
*   - semantic interfaces such as SetValue/GetValue/SetRange
*
*   XML attributes (in addition to the common RichEdit attributes):
*     step        Step value, a positive integer, default 1
*     value       Initial value (integer)
*     spin_class  Style of the spin buttons (same as RichEdit, format: "spin container class,up button class,down button class",
*                 see the rich_edit_spin setting in global.xml)
*
*   kEventTextChanged is fired when the value changes (same as RichEdit).
*/
class DUI_API SpinBox : public RichEdit
{
    typedef RichEdit BaseClass;
public:
    explicit SpinBox(Window* pWindow);
    SpinBox(const SpinBox& r) = delete;
    SpinBox& operator=(const SpinBox& r) = delete;
    virtual ~SpinBox() override = default;

    virtual std::string GetType() const override;
    virtual void SetAttribute(const std::string& strName, const std::string& strValue) override;

    /** Sets the step (the amount the spin buttons and the up/down arrow keys add or subtract)
    * @param [in] nStep Step value, must be greater than 0, default is 1
    */
    void SetStep(int32_t nStep);

    /** Gets the step
    */
    int32_t GetStep() const;

    /** Sets the current value (clamped to the min/max range)
    */
    void SetValue(int64_t nValue);

    /** Gets the current value
    */
    int64_t GetValue() const;

    /** Sets the value range
    * @param [in] nMin Minimum value
    * @param [in] nMax Maximum value
    */
    void SetRange(int64_t nMin, int64_t nMax);
};

} // namespace ui

#endif // UI_CONTROL_SPINBOX_H_
