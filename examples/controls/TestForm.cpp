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
    return "controls";
}

DString TestForm::GetSkinFile()
{
    return "test.xml";
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
    ui::Label* pLabel = ui::Find<ui::Label>(this, "progress_text");
    std::vector<DString> controlList = {"progress11", "progress12", "progress13", "progress14",
                                        "progress21", "progress22", "progress23", "progress24" };
    for (const DString& name : controlList) {
        ui::Progress* pProgress = ui::Find<ui::Progress>(this, name);
        if (pProgress != nullptr) {
            if (pLabel != nullptr) {
                pLabel->SetText(ui::StringUtil::Printf("%d%%", (int32_t)m_nProgressValue));                
            }
            pProgress->SetValue(m_nProgressValue);
        }
    }

    m_nProgressValue += 0.4;
    if (m_nProgressValue > 100.0) {
        m_nProgressValue = 0.0;
    }
}

