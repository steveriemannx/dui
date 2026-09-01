#include "TestForm.h"

TestForm::TestForm():
    m_nProgressValue(0.0)
{
}

TestForm::~TestForm()
{
}

DString TestForm::GetSkinFolder()
{
    return DUI_T("controls");
}

DString TestForm::GetSkinFile()
{
    return DUI_T("test.xml");
}

void TestForm::OnInitWindow()
{
    //Start a timer
    ui::GlobalManager::Instance().Thread().PostRepeatedTask(ui::kThreadUI,
        ui::UiBind(this, [this]() {
                OnTimer();
            }),
        100);
}

void TestForm::OnTimer()
{
    ui::Label* pLabel = ui::Find<ui::Label>(this, DUI_T("progress_text"));
    std::vector<DString> controlList = {DUI_T("progress11"), DUI_T("progress12"), DUI_T("progress13"), DUI_T("progress14"),
                                        DUI_T("progress21"), DUI_T("progress22"), DUI_T("progress23"), DUI_T("progress24") };
    for (const DString& name : controlList) {
        ui::Progress* pProgress = ui::Find<ui::Progress>(this, name);
        if (pProgress != nullptr) {
            if (pLabel != nullptr) {
                pLabel->SetText(ui::StringUtil::Printf(DUI_T("%d%%"), (int32_t)m_nProgressValue));                
            }
            pProgress->SetValue(m_nProgressValue);
        }
    }

    m_nProgressValue += 0.4;
    if (m_nProgressValue > 100.0) {
        m_nProgressValue = 0.0;
    }
}

