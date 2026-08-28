#include "MoveControlForm.h"

using namespace ui;
using namespace std;


MoveControlForm::MoveControlForm(const DString& theme_directory, const DString& layout_xml):
    m_theme_directory(theme_directory),
    m_layout_xml(layout_xml)
{
}

MoveControlForm::~MoveControlForm()
{
}

DString MoveControlForm::GetSkinFolder()
{
    return m_theme_directory;
}

DString MoveControlForm::GetSkinFile()
{
    return m_layout_xml;
}

void MoveControlForm::OnInitWindow()
{
    // Add apps. Apps may be pushed from the server; usually they are also saved locally
    //loadFromDb
    // getFromServer----> The backend can save to the db first, then post a message; the UI reloads from the db

    // Hard-coded for the demo
    std::vector<AppItem> applist;
    AppDb::GetInstance().LoadFromDb(applist);

    m_frequent_app = static_cast<ui::Box*>(FindControl(_T("frequent_app")));
    m_my_app = static_cast<ui::Box*>(FindControl(_T("my_app")));
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
}
