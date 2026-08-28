#include "MainForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from MultiLang.xml)

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("MultiLang");
}

DString MainForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from MultiLang.xml
    return _T("");
}

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = 600;
    attrs.m_szInitSize.cy = 400;
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = true;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcSizeBox = ui::UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;
    // Shadow nine-patch parameters, corresponding to shadow_type="default" in
    // the XML layout (WindowBuilder adds the shadow corner to the size).
    ui::Shadow::ShadowType nShadowType = ui::Shadow::ShadowType::kShadowDefault;
    ui::UiSize szBorderRound;
    ui::UiPadding rcShadowCorner;
    DString shadowImage;
    if (ui::Shadow::GetShadowParam(nShadowType, szBorderRound, rcShadowCorner, shadowImage)) {
        attrs.m_rcShadowCorner = rcShadowCorner;
        if (attrs.m_bInitSizeDefined) {
            attrs.m_szInitSize.cx += rcShadowCorner.left + rcShadowCorner.right;
            attrs.m_szInitSize.cy += rcShadowCorner.top + rcShadowCorner.bottom;
        }
    }

    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::OnInitWindow()
{
    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    // Build-time generated from MultiLang.xml
    InitMultiLang(this);

    /* Show select language menu */
    ui::Button* select = dynamic_cast<ui::Button*>(FindControl(_T("language")));
    ASSERT(select != nullptr);
    if (select == nullptr) {
        return;
    }
    select->AttachClick([this](const ui::EventArgs& args) {
        ui::UiRect rect = args.GetSender()->GetPos();
        ui::UiPoint point;
        //Pop up the menu right below the language button: align the menu's
        //right edge with the button's right edge (RIGHT_TOP alignment)
        point.x = rect.right;
        point.y = rect.bottom;
        ClientToScreen(point);

        ShowPopupMenu(point);
        return true;
    });

    BaseClass::OnInitWindow();
}

void MainForm::ShowPopupMenu(const ui::UiPoint& point)
{
    ui::Menu* menu = new ui::Menu(this);// The parent window must be set; otherwise, when the menu pops up, the program status bar becomes inactive
    // Pure code menu: no XML template; all menu items are added by code
    menu->ShowMenu(_T(""), point);

    // Current language file
    DString currentLangFileName = ui::GlobalManager::Instance().GetLanguageFileName();

    // The list of available language files and their display names
    std::vector<std::pair<DString, DString>> languageList;
    ui::GlobalManager::Instance().GetLanguageList(languageList);
    if (languageList.empty()) {
        languageList.push_back({ currentLangFileName , _T("")});
    }

    // Match the XML lang_menu.xml header row: "选择语言" plus a separator,
    // before the dynamically added language items.
    {
        ui::HBox* pHeader = new ui::HBox(this);
        pHeader->SetClass(_T("menu_split_box"));
        pHeader->SetAttribute(_T("height"), _T("36"));
        pHeader->SetAttribute(_T("width"), _T("256"));
        ui::Label* pLabel = new ui::Label(this);
        pLabel->SetClass(_T("menu_text"));
        pLabel->SetTextId(_T("MULTI_LANG_SELECT_LANGUAGE"));
        pLabel->SetAttribute(_T("text_padding"), _T("0,0,6,0"));
        pHeader->AddItem(pLabel);
        menu->AddMenuControl(pHeader);

        ui::Box* pSeparator = new ui::Box(this);
        pSeparator->SetClass(_T("menu_split_box"));
        ui::Control* pLine = new ui::Control(this);
        pLine->SetClass(_T("menu_split_line"));
        pSeparator->AddItem(pLine);
        menu->AddMenuControl(pSeparator);
    }

    // Add menu items dynamically
    for (auto& lang : languageList) {
        const DString fileName = lang.first;
        DString& displayName = lang.second;

        ui::MenuItem* pMenuItem = new ui::MenuItem(this);
        pMenuItem->SetClass(_T("menu_element"));
        //Pure-code menus have no XML width; give the items the same width as
        //the XML lang_menu.xml header row (256) so the popup window is visible.
        pMenuItem->SetFixedWidth(ui::UiFixedInt(256), true, true);
        ui::CheckBox* pCheckBox = new ui::CheckBox(this);
        pCheckBox->SetClass(_T("menu_checkbox"));
        pCheckBox->SetAttribute(_T("margin"), _T("0,5,0,10"));
        pCheckBox->SetText(!displayName.empty() ? displayName : fileName);
        pMenuItem->AddItem(pCheckBox);
        menu->AddMenuItem(pMenuItem);

        if (ui::StringUtil::IsEqualNoCase(fileName, currentLangFileName)) {
            pCheckBox->Selected(true);
        }

        // Attach the language selection event
        pMenuItem->AttachClick([fileName](const ui::EventArgs& args) {
            // Switch language
            ui::GlobalManager& globalManager = ui::GlobalManager::Instance();
            if (globalManager.GetLanguageFileName() != fileName) {
                globalManager.ReloadLanguage(ui::FilePath(), fileName, true);
            }
            return true;
        });
    }
}
