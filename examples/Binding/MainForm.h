#ifndef EXAMPLES_BINDING_MAIN_FORM_H_
#define EXAMPLES_BINDING_MAIN_FORM_H_

// dui
#include "dui/dui.h"
#include "dui/Binding/BindingGroup.h"

#include "CounterViewModel.h"

#include <memory>

/** Demonstrates the data-binding module, and its coexistence with the
 imperative style that predates it.

 The window shows both at once: most controls follow the view model through
 bindings, and one button/label pair is still wired the old way -- FindControl,
 AttachClick, SetText. Neither style interferes with the other, and neither has to
 be migrated for the other to work.
*/
class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm() = default;
    virtual ~MainForm() override = default;

    /** Resource-related interfaces
     * GetSkinFolder sets the skin resource path of the window to be drawn
     * GetSkinFile sets the XML description file of the window to be drawn
     */
    virtual std::string GetSkinFolder() override { return "binding"; }
    virtual std::string GetSkinFile() override { return "binding.xml"; }

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    /** Every binding is declared here, so the data flow reads in one place --
     the structural counterpart to the layout XML. */
    void BindViewModel();

    /** The pre-binding style, kept deliberately: it still works. */
    void BindLegacyControls();
    void UpdateLegacyLabel();

    std::shared_ptr<CounterViewModel> m_pViewModel;

    /** A member, so every binding is detached when this object's members are
     destroyed -- which happens before the framework frees the control tree. */
    ui::binding::BindingGroup m_bindings;

    /** Only needed by the legacy path; bound controls never need a pointer. */
    ui::Label* m_pLegacyLabel = nullptr;
};

#endif // EXAMPLES_BINDING_MAIN_FORM_H_
