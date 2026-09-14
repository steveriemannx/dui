#include "MoveControlForm.h"
#include "dui/Utils/UiBuilder.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from main.xml)

using namespace ui;
using namespace std;


MoveControlForm::MoveControlForm()
{
}

void MoveControlForm::BuildUI()
{
    // Build-time generated from main.xml
    InitMain(this);
}

void MoveControlForm::OnInitWindow()
{
    BuildUI();

    // Add apps. Apps may be pushed from the server; usually they are also saved locally
    //loadFromDb
    // getFromServer----> The backend can save to the db first, then post a message; the UI reloads from the db

    // Hard-coded for the demo
    std::vector<AppItem> applist;
    AppDb::GetInstance().LoadFromDb(applist);

    m_frequent_app = ui::Find<ui::Box>(this, "frequent_app");
    m_my_app = ui::Find<ui::Box>(this, "my_app");
    ASSERT(m_frequent_app != nullptr);
    ASSERT(m_my_app != nullptr);
    
    for (const auto& item: applist) {
        AppItemUi* pAppUi = AppItemUi::Create(item, m_frequent_app);
        pAppUi->SetFrequentBox(m_frequent_app);
        if (item.m_isFrequent) {
            m_frequent_app->AddItem(pAppUi);
        }
        else
        {
            m_my_app->AddItem(pAppUi);
        }
    }

    BindEvents();
    BaseClass::OnInitWindow();
}

void MoveControlForm::BindEvents()
{
}
