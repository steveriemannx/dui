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
    return "rich_edit";
}

DString FindForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from find.xml
    return "";
}

void FindForm::BuildUI()
{
    // Corresponds to the find.xml layout
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor("bk_wnd_darkcolor");

    // Title bar
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute("name", "window_caption_bar");
    pCaption->SetAttribute("width", "stretch");
    pCaption->SetAttribute("height", "35");
    pCaption->SetBkColor("bk_wnd_lightcolor");
    pRoot->AddItem(pCaption);

    ui::Control* pLogo = new ui::Control(this);
    pLogo->SetAttribute("width", "18");
    pLogo->SetAttribute("height", "18");
    pLogo->SetBkImage("public/caption/logo.svg");
    pLogo->SetAttribute("valign", "center");
    pLogo->SetAttribute("margin", "8");
    pCaption->AddItem(pLogo);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetText("Find");
    pTitle->SetAttribute("valign", "center");
    pTitle->SetAttribute("margin", "8");
    pCaption->AddItem(pTitle);

    ui::Control* pSpacer = new ui::Control(this);
    pCaption->AddItem(pSpacer);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass("btn_wnd_close_11");
    pCloseBtn->SetName("closebtn");
    pCloseBtn->SetAttribute("width", "40");
    pCloseBtn->SetAttribute("height", "32");
    pCloseBtn->SetAttribute("margin", "4,0,0,0");
    pCaption->AddItem(pCloseBtn);

    // Find content row
    ui::VBox* pContent = new ui::VBox(this);
    pRoot->AddItem(pContent);

    ui::HBox* pFindRow = new ui::HBox(this);
    pFindRow->SetAttribute("height", "auto");
    pContent->AddItem(pFindRow);

    ui::Label* pFindLabel = new ui::Label(this);
    pFindLabel->SetText("Find What:");
    pFindLabel->SetAttribute("height", "28");
    pFindLabel->SetAttribute("text_align", "vcenter");
    pFindLabel->SetAttribute("margin", "8,8,0,0");
    pFindRow->AddItem(pFindLabel);

    ui::RichEdit* pFindText = new ui::RichEdit(this);
    pFindText->SetClass("simple prompt simple_border");
    pFindText->SetName("btn_find_text");
    pFindText->SetAttribute("width", "stretch");
    pFindText->SetAttribute("height", "28");
    pFindText->SetAttribute("prompttext", "Find");
    pFindText->SetAttribute("text_padding", "2,0,0,0");
    pFindText->SetAttribute("text_align", "vcenter");
    pFindText->SetBkColor("white");
    pFindText->SetAttribute("margin", "2,8,4,0");
    pFindRow->AddItem(pFindText);

    ui::Button* pFindNextBtn = new ui::Button(this);
    pFindNextBtn->SetClass("btn_global_white_80x30");
    pFindNextBtn->SetName("btn_find_next");
    pFindNextBtn->SetText("Find Next");
    pFindNextBtn->SetAttribute("width", "80");
    pFindNextBtn->SetAttribute("height", "28");
    pFindNextBtn->SetAttribute("margin", "4,8,8,0");
    pFindRow->AddItem(pFindNextBtn);

    // Search direction
    ui::GroupVBox* pDirectionGroup = new ui::GroupVBox(this);
    pDirectionGroup->SetAttribute("margin", "10,6,10,6");
    pDirectionGroup->SetAttribute("text", " Search Direction ");
    pDirectionGroup->SetAttribute("height", "auto");
    pDirectionGroup->SetAttribute("corner_size", "4,4");
    pContent->AddItem(pDirectionGroup);

    ui::HBox* pDirectionRow = new ui::HBox(this);
    pDirectionRow->SetAttribute("height", "40");
    pDirectionRow->SetAttribute("margin", "24,12,8,4");
    pDirectionRow->SetAttribute("padding", "12,0,0,0");
    pDirectionGroup->AddItem(pDirectionRow);

    ui::Option* pOptionUp = new ui::Option(this);
    pOptionUp->SetClass("option_1");
    pOptionUp->SetAttribute("group", "option_direction_group");
    pOptionUp->SetText("Search Up");
    pOptionUp->SetAttribute("margin", "0,6,0,0");
    pDirectionRow->AddItem(pOptionUp);

    ui::Option* pOptionDown = new ui::Option(this);
    pOptionDown->SetClass("option_1");
    pOptionDown->SetAttribute("group", "option_direction_group");
    pOptionDown->SetText("Search Down");
    pOptionDown->SetName("option_direction_down");
    pOptionDown->SetAttribute("margin", "16,6,0,0");
    pOptionDown->Selected(true);
    pDirectionRow->AddItem(pOptionDown);

    // Options row
    ui::HBox* pOptionRow = new ui::HBox(this);
    pOptionRow->SetAttribute("height", "auto");
    pContent->AddItem(pOptionRow);

    ui::CheckBox* pCaseSensitive = new ui::CheckBox(this);
    pCaseSensitive->SetClass("checkbox_1");
    pCaseSensitive->SetName("check_box_case_sensitive");
    pCaseSensitive->SetText("Case Sensitive");
    pCaseSensitive->SetAttribute("valign", "center");
    pCaseSensitive->SetAttribute("margin", "8,8,8,0");
    pCaseSensitive->Selected(true);
    pOptionRow->AddItem(pCaseSensitive);

    ui::CheckBox* pMatchWholeWord = new ui::CheckBox(this);
    pMatchWholeWord->SetClass("checkbox_1");
    pMatchWholeWord->SetName("check_box_match_whole_word");
    pMatchWholeWord->SetText("Match Whole Word");
    pMatchWholeWord->SetAttribute("valign", "center");
    pMatchWholeWord->SetAttribute("margin", "8,8,8,0");
    pOptionRow->AddItem(pMatchWholeWord);

    ui::Control* pOptionSpacer = new ui::Control(this);
    pOptionRow->AddItem(pOptionSpacer);

    ui::Button* pCancelBtn = new ui::Button(this);
    pCancelBtn->SetClass("btn_global_white_80x30");
    pCancelBtn->SetName("btn_cancel");
    pCancelBtn->SetText("Cancel");
    pCancelBtn->SetAttribute("width", "80");
    pCancelBtn->SetAttribute("height", "28");
    pCancelBtn->SetAttribute("margin", "4,8,8,0");
    pOptionRow->AddItem(pCancelBtn);

    AttachBox(pRoot);
}

void FindForm::OnInitWindow()
{
    // Hand-written pure code UI (corresponds to the find.xml layout, equivalent to the generator output)
    BuildUI();

    m_pFindText = ui::Find<ui::RichEdit>(this, "btn_find_text");
    m_pDirectionOption = ui::Find<ui::Option>(this, "option_direction_down");
    m_pCaseSensitive = ui::Find<ui::CheckBox>(this, "check_box_case_sensitive");
    m_pMatchWholeWord = ui::Find<ui::CheckBox>(this, "check_box_match_whole_word");
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
    ui::Button* pButton = ui::Find<ui::Button>(this, "btn_cancel");
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    CloseWnd();
                }
                return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, "btn_find_next");
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
