#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

void MainForm::OnInitWindow()
{
    BaseClass::OnInitWindow();

    m_pViewModel = std::make_shared<CounterViewModel>();

    BindViewModel();
    BindLegacyControls();
}

void MainForm::BindViewModel()
{
    // Hand the view model to the group. Bindings declared before this call would be
    // remembered and applied here, which is how a recycled list item declares its
    // bindings once and only swaps contexts.
    m_bindings.SetContext(m_pViewModel);

    // Two labels follow the same property. Adding a third display point is one more
    // line here -- not a hunt through every place that changes the counter.
    m_bindings.Bind(this, DUI_T("count_label"), DUI_T("text"), DUI_T("countText"));
    m_bindings.Bind(this, DUI_T("mirror_label"), DUI_T("text"), DUI_T("countText"));

    // One-way only. Progress reports no value-changed event, so there is nothing to
    // write back from -- BindTwoWay on "value" would return false here. It would
    // work on a Slider, which derives from Progress and does fire the event.
    m_bindings.Bind(this, DUI_T("count_progress"), DUI_T("value"), DUI_T("countValue"));

    // Visibility follows the view model too; the toggle button decides the value.
    m_bindings.Bind(this, DUI_T("detail_label"), DUI_T("visible"), DUI_T("detailVisible"));

    // Commands take a member pointer rather than a name, so the method is checked
    // by the compiler. The view model is held weakly, so a dead one is a no-op.
    m_bindings.BindCommand(this, DUI_T("btn_inc"), m_pViewModel, &CounterViewModel::Increase);
    m_bindings.BindCommand(this, DUI_T("btn_dec"), m_pViewModel, &CounterViewModel::Decrease);
    m_bindings.BindCommand(this, DUI_T("btn_toggle"), m_pViewModel, &CounterViewModel::ToggleDetail);

    // Note: the button names above must be unique in this window. The engine
    // resolves them exactly like FindControl() does -- case-sensitive, first match
    // in traversal order. Item templates repeat inner names across every instance,
    // so an item binds against itself (a Box), never against the window.
}

void MainForm::BindLegacyControls()
{
    // The imperative style that predates this module, kept here to show that the two
    // coexist: nothing above had to change for this to keep working.
    // Note what it costs -- a stored control pointer, and a refresh call that must
    // be remembered at every site that mutates the data. Bound controls need neither.
    m_pLegacyLabel = ui::Find<ui::Label>(this, DUI_T("legacy_label"));

    if (auto* pButton = ui::Find<ui::Button>(this, DUI_T("btn_legacy"))) {
        pButton->AttachClick([this](const ui::EventArgs&) {
            m_pViewModel->Increase();   // the very same view model
            UpdateLegacyLabel();        // but refreshed by hand
            return true;
        });
    }

    UpdateLegacyLabel();
}

void MainForm::UpdateLegacyLabel()
{
    if (m_pLegacyLabel != nullptr) {
        m_pLegacyLabel->SetText(m_pViewModel->GetCountText());
    }
}
