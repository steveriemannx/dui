#include "FindForm.h"
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

FindForm::FindForm(MainForm* pMainForm):
    m_pMainForm(pMainForm),
    m_pDirectionOption(nullptr),
    m_pCaseSensitive(nullptr),
    m_pMatchWholeWord(nullptr),
    m_pFindText(nullptr)
{
}

FindForm::~FindForm()
{
}

DString FindForm::GetSkinFolder()
{
    return DUI_T("rich_edit");
}

DString FindForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from find.xml
    return DUI_T("");
}

void FindForm::BuildUI()
{
    // Corresponds to the find.xml layout
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(DUI_T("bk_wnd_darkcolor"));

    // Title bar
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(DUI_T("name"), DUI_T("window_caption_bar"));
    pCaption->SetAttribute(DUI_T("width"), DUI_T("stretch"));
    pCaption->SetAttribute(DUI_T("height"), DUI_T("35"));
    pCaption->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::Control* pLogo = new ui::Control(this);
    pLogo->SetAttribute(DUI_T("width"), DUI_T("18"));
    pLogo->SetAttribute(DUI_T("height"), DUI_T("18"));
    pLogo->SetBkImage(DUI_T("public/caption/logo.svg"));
    pLogo->SetAttribute(DUI_T("valign"), DUI_T("center"));
    pLogo->SetAttribute(DUI_T("margin"), DUI_T("8"));
    pCaption->AddItem(pLogo);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetText(DUI_T("Find"));
    pTitle->SetAttribute(DUI_T("valign"), DUI_T("center"));
    pTitle->SetAttribute(DUI_T("margin"), DUI_T("8"));
    pCaption->AddItem(pTitle);

    ui::Control* pSpacer = new ui::Control(this);
    pCaption->AddItem(pSpacer);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass(DUI_T("btn_wnd_close_11"));
    pCloseBtn->SetName(DUI_T("closebtn"));
    pCloseBtn->SetAttribute(DUI_T("width"), DUI_T("40"));
    pCloseBtn->SetAttribute(DUI_T("height"), DUI_T("32"));
    pCloseBtn->SetAttribute(DUI_T("margin"), DUI_T("4,0,0,0"));
    pCaption->AddItem(pCloseBtn);

    // Find content row
    ui::VBox* pContent = new ui::VBox(this);
    pRoot->AddItem(pContent);

    ui::HBox* pFindRow = new ui::HBox(this);
    pFindRow->SetAttribute(DUI_T("height"), DUI_T("auto"));
    pContent->AddItem(pFindRow);

    ui::Label* pFindLabel = new ui::Label(this);
    pFindLabel->SetText(DUI_T("Find What:"));
    pFindLabel->SetAttribute(DUI_T("height"), DUI_T("28"));
    pFindLabel->SetAttribute(DUI_T("text_align"), DUI_T("vcenter"));
    pFindLabel->SetAttribute(DUI_T("margin"), DUI_T("8,8,0,0"));
    pFindRow->AddItem(pFindLabel);

    ui::RichEdit* pFindText = new ui::RichEdit(this);
    pFindText->SetClass(DUI_T("simple prompt simple_border"));
    pFindText->SetName(DUI_T("btn_find_text"));
    pFindText->SetAttribute(DUI_T("width"), DUI_T("stretch"));
    pFindText->SetAttribute(DUI_T("height"), DUI_T("28"));
    pFindText->SetAttribute(DUI_T("prompttext"), DUI_T("Find"));
    pFindText->SetAttribute(DUI_T("text_padding"), DUI_T("2,0,0,0"));
    pFindText->SetAttribute(DUI_T("text_align"), DUI_T("vcenter"));
    pFindText->SetBkColor(DUI_T("white"));
    pFindText->SetAttribute(DUI_T("margin"), DUI_T("2,8,4,0"));
    pFindRow->AddItem(pFindText);

    ui::Button* pFindNextBtn = new ui::Button(this);
    pFindNextBtn->SetClass(DUI_T("btn_global_white_80x30"));
    pFindNextBtn->SetName(DUI_T("btn_find_next"));
    pFindNextBtn->SetText(DUI_T("Find Next"));
    pFindNextBtn->SetAttribute(DUI_T("width"), DUI_T("80"));
    pFindNextBtn->SetAttribute(DUI_T("height"), DUI_T("28"));
    pFindNextBtn->SetAttribute(DUI_T("margin"), DUI_T("4,8,8,0"));
    pFindRow->AddItem(pFindNextBtn);

    // Search direction
    ui::GroupVBox* pDirectionGroup = new ui::GroupVBox(this);
    pDirectionGroup->SetAttribute(DUI_T("margin"), DUI_T("10,6,10,6"));
    pDirectionGroup->SetAttribute(DUI_T("text"), DUI_T(" Search Direction "));
    pDirectionGroup->SetAttribute(DUI_T("height"), DUI_T("auto"));
    pDirectionGroup->SetAttribute(DUI_T("corner_size"), DUI_T("4,4"));
    pContent->AddItem(pDirectionGroup);

    ui::HBox* pDirectionRow = new ui::HBox(this);
    pDirectionRow->SetAttribute(DUI_T("height"), DUI_T("40"));
    pDirectionRow->SetAttribute(DUI_T("margin"), DUI_T("24,12,8,4"));
    pDirectionRow->SetAttribute(DUI_T("padding"), DUI_T("12,0,0,0"));
    pDirectionGroup->AddItem(pDirectionRow);

    ui::Option* pOptionUp = new ui::Option(this);
    pOptionUp->SetClass(DUI_T("option_1"));
    pOptionUp->SetAttribute(DUI_T("group"), DUI_T("option_direction_group"));
    pOptionUp->SetText(DUI_T("Search Up"));
    pOptionUp->SetAttribute(DUI_T("margin"), DUI_T("0,6,0,0"));
    pDirectionRow->AddItem(pOptionUp);

    ui::Option* pOptionDown = new ui::Option(this);
    pOptionDown->SetClass(DUI_T("option_1"));
    pOptionDown->SetAttribute(DUI_T("group"), DUI_T("option_direction_group"));
    pOptionDown->SetText(DUI_T("Search Down"));
    pOptionDown->SetName(DUI_T("option_direction_down"));
    pOptionDown->SetAttribute(DUI_T("margin"), DUI_T("16,6,0,0"));
    pOptionDown->Selected(true);
    pDirectionRow->AddItem(pOptionDown);

    // Options row
    ui::HBox* pOptionRow = new ui::HBox(this);
    pOptionRow->SetAttribute(DUI_T("height"), DUI_T("auto"));
    pContent->AddItem(pOptionRow);

    ui::CheckBox* pCaseSensitive = new ui::CheckBox(this);
    pCaseSensitive->SetClass(DUI_T("checkbox_1"));
    pCaseSensitive->SetName(DUI_T("check_box_case_sensitive"));
    pCaseSensitive->SetText(DUI_T("Case Sensitive"));
    pCaseSensitive->SetAttribute(DUI_T("valign"), DUI_T("center"));
    pCaseSensitive->SetAttribute(DUI_T("margin"), DUI_T("8,8,8,0"));
    pCaseSensitive->Selected(true);
    pOptionRow->AddItem(pCaseSensitive);

    ui::CheckBox* pMatchWholeWord = new ui::CheckBox(this);
    pMatchWholeWord->SetClass(DUI_T("checkbox_1"));
    pMatchWholeWord->SetName(DUI_T("check_box_match_whole_word"));
    pMatchWholeWord->SetText(DUI_T("Match Whole Word"));
    pMatchWholeWord->SetAttribute(DUI_T("valign"), DUI_T("center"));
    pMatchWholeWord->SetAttribute(DUI_T("margin"), DUI_T("8,8,8,0"));
    pOptionRow->AddItem(pMatchWholeWord);

    ui::Control* pOptionSpacer = new ui::Control(this);
    pOptionRow->AddItem(pOptionSpacer);

    ui::Button* pCancelBtn = new ui::Button(this);
    pCancelBtn->SetClass(DUI_T("btn_global_white_80x30"));
    pCancelBtn->SetName(DUI_T("btn_cancel"));
    pCancelBtn->SetText(DUI_T("Cancel"));
    pCancelBtn->SetAttribute(DUI_T("width"), DUI_T("80"));
    pCancelBtn->SetAttribute(DUI_T("height"), DUI_T("28"));
    pCancelBtn->SetAttribute(DUI_T("margin"), DUI_T("4,8,8,0"));
    pOptionRow->AddItem(pCancelBtn);

    AttachBox(pRoot);
}

void FindForm::OnInitWindow()
{
    // Hand-written pure code UI (corresponds to the find.xml layout, equivalent to the generator output)
    BuildUI();

    m_pFindText = ui::Find<ui::RichEdit>(this, DUI_T("btn_find_text"));
    m_pDirectionOption = ui::Find<ui::Option>(this, DUI_T("option_direction_down"));
    m_pCaseSensitive = ui::Find<ui::CheckBox>(this, DUI_T("check_box_case_sensitive"));
    m_pMatchWholeWord = ui::Find<ui::CheckBox>(this, DUI_T("check_box_match_whole_word"));
    ASSERT(m_pFindText != nullptr);
    ASSERT(m_pDirectionOption != nullptr);
    ASSERT(m_pCaseSensitive != nullptr);
    ASSERT(m_pMatchWholeWord != nullptr);

    if (m_pFindText != nullptr) {
        m_pFindText->SetFocus();

        ui::RichEdit* pRichEdit = nullptr;
        if (m_pMainForm != nullptr) {
            pRichEdit = m_pMainForm->GetRichEdit();
        }
        DString selText;
        if (pRichEdit != nullptr) {
            selText = pRichEdit->GetSelText();
        }
        if (!selText.empty()) {
            m_pFindText->SetText(selText);
            m_pFindText->SetSelAll();
        }
    }

    BindEvents();
    BaseClass::OnInitWindow();
}

void FindForm::BindEvents()
{
    ui::Button* pButton = ui::Find<ui::Button>(this, DUI_T("btn_cancel"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    CloseWnd();
                }
                return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_find_next"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    OnFindNext();
                }
                return true;
            });
    }
}

void FindForm::OnFindNext()
{
    if (m_pFindText == nullptr) {
        return;
    }
    DString findText = m_pFindText->GetText();
    if (findText.empty()) {
        return;
    }

    bool bFindDown = true;
    if (m_pDirectionOption != nullptr) {
        bFindDown = m_pDirectionOption->IsSelected();
    }

    bool bMatchCase = true;
    if (m_pCaseSensitive != nullptr) {
        bMatchCase = m_pCaseSensitive->IsSelected();
    }

    bool bMatchWholeWord = false;
    if (m_pMatchWholeWord != nullptr) {
        bMatchWholeWord = m_pMatchWholeWord->IsSelected();
    }

    if (m_pMainForm != nullptr) {
        m_pMainForm->FindRichText(findText, bFindDown, bMatchCase, bMatchWholeWord, this);
    }
}
