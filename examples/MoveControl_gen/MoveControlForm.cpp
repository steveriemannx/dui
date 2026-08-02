#include "MoveControlForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from main.xml)

using namespace ui;
using namespace std;


MoveControlForm::MoveControlForm()
{
}

MoveControlForm::~MoveControlForm()
{
}

DString MoveControlForm::GetSkinFolder()
{
    return _T("move_control");
}

DString MoveControlForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from main.xml
    return _T("");
}

void MoveControlForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    // Corresponding to the <Window> attributes in main.xml
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 540;
    attrs.m_szInitSize.cy = 535;
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = true;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;
    BaseClass::GetCreateWindowAttributes(attrs);
}

void MoveControlForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    // Build-time generated from main.xml
    InitMain(this);

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
