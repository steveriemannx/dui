#ifndef UI_CONTROL_FILTERCOMBO_H_
#define UI_CONTROL_FILTERCOMBO_H_

#include "dui/Control/Combo.h"

namespace ui 
{

/** Combo box with filtering capability
*/
class DUI_API FilterCombo : public Combo
{
    typedef Combo BaseClass;
public:
    explicit FilterCombo(Window* pWindow);
    FilterCombo(const FilterCombo& r) = delete;
    Combo& operator=(const FilterCombo& r) = delete;
    virtual ~FilterCombo() override;

    /// Override the base class method to provide customized functionality; refer to the base class declaration
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

protected:
    virtual void OnInit() override;
    
protected:
    /** The mouse presses the button on the Edit control
     * @param[in] args Argument list
     * @return Always returns true
     */
    virtual bool OnEditButtonDown(const EventArgs& args) override;

    /** The mouse releases the button on the Edit control
     * @param[in] args Argument list
     * @return Always returns true
     */
    virtual bool OnEditButtonUp(const EventArgs& args) override;

    /** The text content of the Edit control has changed
     * @param[in] args Argument list
     * @return Always returns true
     */
    virtual bool OnEditTextChanged(const ui::EventArgs& args) override;

private:

    /** Filter the contents of the drop-down list
    */
    void FilterComboList(const DString& filterText);

    /** Determine whether the text satisfies the filter condition
    */
    bool IsFilterText(const DString& filterText, const DString& itemText) const;
};

} // namespace ui

#endif // UI_CONTROL_FILTERCOMBO_H_
