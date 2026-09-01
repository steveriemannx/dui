#include "ReplaceForm.h"
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

ReplaceForm::ReplaceForm(MainForm* pMainForm):
    m_pMainForm(pMainForm),
    m_pDirectionOption(nullptr),
    m_pCaseSensitive(nullptr),
    m_pMatchWholeWord(nullptr),
    m_pFindText(nullptr),
    m_pReplaceText(nullptr)
{
}


ReplaceForm::~ReplaceForm()
{
}

DString ReplaceForm::GetSkinFolder()
{
    return "rich_edit";
}

DString ReplaceForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from replace.xml
    return "";
}

void ReplaceForm::BuildUI()
{
    // Corresponds to the replace.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor("bk_wnd_darkcolor");

    // Title bar
    auto* pCaption = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "35"}});
    pCaption->SetBkColor("bk_wnd_lightcolor");
    pRoot->AddItem(pCaption);

    auto* pLogo = ui::Create<ui::Control>(this, {{"width", "18"}, {"height", "18"}, {"valign", "center"}, {"margin", "8"}});
    pLogo->SetBkImage("public/caption/logo.svg");
    pCaption->AddItem(pLogo);

    auto* pTitle = ui::Create<ui::Label>(this, {{"valign", "center"}, {"margin", "8"}});
    pTitle->SetText("Replace");
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {});
    pCaption->AddItem(pSpacer);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"width", "40"}, {"height", "32"}, {"margin", "4,0,0,0"}});
    pCloseBtn->SetClass("btn_wnd_close_11");
    pCloseBtn->SetName("closebtn");
    pCaption->AddItem(pCloseBtn);

    // Content area
    auto* pContent = ui::Create<ui::VBox>(this, {});
    pRoot->AddItem(pContent);

    // Find content row
    auto* pFindRow = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    pContent->AddItem(pFindRow);

    auto* pFindLabel = ui::Create<ui::Label>(this, {{"height", "28"}, {"text_align", "vcenter"}, {"margin", "8,8,0,0"}});
    pFindLabel->SetText("Find What:");
    pFindRow->AddItem(pFindLabel);

    auto* pFindText = ui::Create<ui::RichEdit>(this, {{"width", "stretch"}, {"height", "28"}, {"prompttext", "Find"}, {"text_padding", "2,0,0,0"}, {"text_align", "vcenter"}, {"margin", "2,8,4,0"}});
    pFindText->SetClass("simple prompt simple_border");
    pFindText->SetName("btn_find_text");
    pFindText->SetBkColor("white");
    pFindRow->AddItem(pFindText);

    auto* pFindNextBtn = ui::Create<ui::Button>(this, {{"width", "80"}, {"height", "28"}, {"margin", "4,8,8,0"}});
    pFindNextBtn->SetClass("btn_global_white_80x30");
    pFindNextBtn->SetName("btn_find_next");
    pFindNextBtn->SetText("Find Next");
    pFindRow->AddItem(pFindNextBtn);

    // Replace-with row
    auto* pReplaceRow = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    pContent->AddItem(pReplaceRow);

    auto* pReplaceLabel = ui::Create<ui::Label>(this, {{"height", "28"}, {"text_align", "vcenter"}, {"margin", "8,8,0,0"}});
    pReplaceLabel->SetText(" Replace with:");
    pReplaceRow->AddItem(pReplaceLabel);

    auto* pReplaceText = ui::Create<ui::RichEdit>(this, {{"width", "stretch"}, {"height", "28"}, {"prompttext", "Replace"}, {"text_padding", "2,0,0,0"}, {"text_align", "vcenter"}, {"margin", "2,8,4,0"}});
    pReplaceText->SetClass("simple prompt simple_border");
    pReplaceText->SetName("btn_replace_text");
    pReplaceText->SetBkColor("white");
    pReplaceRow->AddItem(pReplaceText);

    auto* pReplaceBtn = ui::Create<ui::Button>(this, {{"width", "80"}, {"height", "28"}, {"margin", "4,8,4,0"}});
    pReplaceBtn->SetClass("btn_global_white_80x30");
    pReplaceBtn->SetName("btn_replace");
    pReplaceBtn->SetText("Replace");
    pReplaceRow->AddItem(pReplaceBtn);

    auto* pReplaceAllBtn = ui::Create<ui::Button>(this, {{"width", "80"}, {"height", "28"}, {"margin", "4,8,8,0"}});
    pReplaceAllBtn->SetClass("btn_global_white_80x30");
    pReplaceAllBtn->SetName("btn_replace_all");
    pReplaceAllBtn->SetText("Replace All");
    pReplaceRow->AddItem(pReplaceAllBtn);

    // Search direction
    auto* pDirectionGroup = ui::Create<ui::GroupVBox>(this, {{"margin", "10,6,10,6"}, {"text", " Search Direction "}, {"height", "auto"}, {"corner_size", "4,4"}});
    pContent->AddItem(pDirectionGroup);

    auto* pDirectionRow = ui::Create<ui::HBox>(this, {{"height", "40"}, {"margin", "24,12,8,4"}, {"padding", "12,0,0,0"}});
    pDirectionGroup->AddItem(pDirectionRow);

    auto* pOptionUp = ui::Create<ui::Option>(this, {{"group", "option_direction_group"}, {"margin", "0,6,0,0"}});
    pOptionUp->SetClass("option_1");
    pOptionUp->SetText("Search Up");
    pDirectionRow->AddItem(pOptionUp);

    auto* pOptionDown = ui::Create<ui::Option>(this, {{"group", "option_direction_group"}, {"margin", "16,6,0,0"}});
    pOptionDown->SetClass("option_1");
    pOptionDown->SetText("Search Down");
    pOptionDown->SetName("option_direction_down");
    pOptionDown->Selected(true);
    pDirectionRow->AddItem(pOptionDown);

    // Options row
    auto* pOptionRow = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    pContent->AddItem(pOptionRow);

    auto* pCaseSensitive = ui::Create<ui::CheckBox>(this, {{"valign", "center"}, {"margin", "8,8,8,0"}});
    pCaseSensitive->SetClass("checkbox_1");
    pCaseSensitive->SetName("check_box_case_sensitive");
    pCaseSensitive->SetText("Case Sensitive");
    pCaseSensitive->Selected(true);
    pOptionRow->AddItem(pCaseSensitive);

    auto* pMatchWholeWord = ui::Create<ui::CheckBox>(this, {{"valign", "center"}, {"margin", "8,8,8,0"}});
    pMatchWholeWord->SetClass("checkbox_1");
    pMatchWholeWord->SetName("check_box_match_whole_word");
    pMatchWholeWord->SetText("Match Whole Word");
    pOptionRow->AddItem(pMatchWholeWord);

    auto* pOptionSpacer = ui::Create<ui::Control>(this, {});
    pOptionRow->AddItem(pOptionSpacer);

    auto* pCancelBtn = ui::Create<ui::Button>(this, {{"width", "80"}, {"height", "28"}, {"margin", "4,8,8,0"}});
    pCancelBtn->SetClass("btn_global_white_80x30");
    pCancelBtn->SetName("btn_cancel");
    pCancelBtn->SetText("Cancel");
    pOptionRow->AddItem(pCancelBtn);

    AttachBox(pRoot);
}

void ReplaceForm::OnInitWindow()
{
    // Hand-written pure code UI (corresponds to the replace.xml layout, equivalent to the generator output)
    BuildUI();

    m_pFindText = ui::Find<ui::RichEdit>(this, "btn_find_text");
    m_pReplaceText = ui::Find<ui::RichEdit>(this, "btn_replace_text");
    m_pDirectionOption = ui::Find<ui::Option>(this, "option_direction_down");
    m_pCaseSensitive = ui::Find<ui::CheckBox>(this, "check_box_case_sensitive");
    m_pMatchWholeWord = ui::Find<ui::CheckBox>(this, "check_box_match_whole_word");
    ASSERT(m_pFindText != nullptr);
    ASSERT(m_pReplaceText != nullptr);
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

void ReplaceForm::BindEvents()
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
    pButton = ui::Find<ui::Button>(this, "btn_replace");
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    OnReplace();
                }
                return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, "btn_replace_all");
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    OnReplaceAll();
                }
                return true;
            });
    }
}

void ReplaceForm::OnFindNext()
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

void ReplaceForm::OnReplace()
{
    if (m_pFindText == nullptr) {
        return;
    }
    DString findText = m_pFindText->GetText();
    if (findText.empty()) {
        return;
    }
    if (m_pReplaceText == nullptr) {
        return;
    }
    DString replaceText = m_pReplaceText->GetText();
    if (replaceText.empty()) {
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
        m_pMainForm->ReplaceRichText(findText, replaceText, bFindDown, bMatchCase, bMatchWholeWord, this);
    }
}

void ReplaceForm::OnReplaceAll()
{
    if (m_pFindText == nullptr) {
        return;
    }
    DString findText = m_pFindText->GetText();
    if (findText.empty()) {
        return;
    }
    if (m_pReplaceText == nullptr) {
        return;
    }
    DString replaceText = m_pReplaceText->GetText();
    if (replaceText.empty()) {
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
        m_pMainForm->ReplaceAllRichText(findText, replaceText, bFindDown, bMatchCase, bMatchWholeWord, this);
    }
}
