#ifndef EXAMPLES_BINDING_COUNTER_VIEW_MODEL_H_
#define EXAMPLES_BINDING_COUNTER_VIEW_MODEL_H_

#include "dui/dui.h"
#include "dui/Binding/ObservableObject.h"

/** The view model for the binding example.

 Deriving from ui::binding::ObservableObject is the only requirement: the engine
 binds any object that can report "this property changed", so this same class
 would work as a plain MVC-style model bound to a couple of widgets. Nothing here
 is MVVM-specific.

 Note there is no type system: values cross the binding boundary as strings, the
 same way Control::SetAttribute() takes them. That is why the counter is exposed
 twice -- once formatted for labels, once as a bare number for the progress bar.
*/
class CounterViewModel : public ui::binding::ObservableObject
{
public:
    CounterViewModel()
    {
        // Registering a property makes it readable by name, which is what the
        // engine needs -- C++ has no reflection to fall back on. Properties that
        // are not DString override GetProperty/SetProperty instead.
        RegisterProperty(DUI_T("countText"), m_sCountText);
        RegisterProperty(DUI_T("countValue"), m_sCountValue);
        RegisterProperty(DUI_T("detailVisible"), m_sDetailVisible);
    }

    void Increase()
    {
        ++m_nCount;
        Refresh();
    }

    void Decrease()
    {
        --m_nCount;
        Refresh();
    }

    void ToggleDetail()
    {
        m_bDetailVisible = !m_bDetailVisible;
        m_sDetailVisible = m_bDetailVisible ? DUI_T("true") : DUI_T("false");
        RaisePropertyChanged(DUI_T("detailVisible"));
    }

    /** Used by the legacy, non-bound control to pull the current text. */
    const DString& GetCountText() const { return m_sCountText; }

private:
    /** The single place that raises. Every bound control refreshes from here; the
     window holds no refresh code at all. */
    void Refresh()
    {
        m_sCountText = ui::StringUtil::Printf(DUI_T("Count: %d"), m_nCount);
        RaisePropertyChanged(DUI_T("countText"));

        // The progress bar wants a number, and there is no conversion in the
        // engine, so the view model exposes one. That is the whole cost of not
        // having a type system, and it is deliberate.
        m_sCountValue = ui::StringUtil::Printf(DUI_T("%d"), m_nCount);
        RaisePropertyChanged(DUI_T("countValue"));
    }

    int     m_nCount = 0;
    bool    m_bDetailVisible = true;
    DString m_sCountText;
    DString m_sCountValue;
    DString m_sDetailVisible;
};

#endif // EXAMPLES_BINDING_COUNTER_VIEW_MODEL_H_
