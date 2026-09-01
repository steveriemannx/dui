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
    return DUI_T("rich_edit");
}

DString ReplaceForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from replace.xml
    return DUI_T("");
}

void ReplaceForm::BuildUI()
{
    // Corresponds to the replace.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor(DUI_T("bk_wnd_darkcolor"));

    // Title bar
    auto* pCaption = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("35")}});
    pCaption->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pLogo = ui::Create<ui::Control>(this, {{DUI_T("width"), DUI_T("18")}, {DUI_T("height"), DUI_T("18")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8")}});
    pLogo->SetBkImage(DUI_T("public/caption/logo.svg"));
    pCaption->AddItem(pLogo);

    auto* pTitle = ui::Create<ui::Label>(this, {{DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8")}});
    pTitle->SetText(DUI_T("Replace"));
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {});
    pCaption->AddItem(pSpacer);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{DUI_T("width"), DUI_T("40")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("margin"), DUI_T("4,0,0,0")}});
    pCloseBtn->SetClass(DUI_T("btn_wnd_close_11"));
    pCloseBtn->SetName(DUI_T("closebtn"));
    pCaption->AddItem(pCloseBtn);

    // Content area
    auto* pContent = ui::Create<ui::VBox>(this, {});
    pRoot->AddItem(pContent);

    // Find content row
    auto* pFindRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    pContent->AddItem(pFindRow);

    auto* pFindLabel = ui::Create<ui::Label>(this, {{DUI_T("height"), DUI_T("28")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("margin"), DUI_T("8,8,0,0")}});
    pFindLabel->SetText(DUI_T("Find What:"));
    pFindRow->AddItem(pFindLabel);

    auto* pFindText = ui::Create<ui::RichEdit>(this, {{DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("prompttext"), DUI_T("Find")}, {DUI_T("text_padding"), DUI_T("2,0,0,0")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("margin"), DUI_T("2,8,4,0")}});
    pFindText->SetClass(DUI_T("simple prompt simple_border"));
    pFindText->SetName(DUI_T("btn_find_text"));
    pFindText->SetBkColor(DUI_T("white"));
    pFindRow->AddItem(pFindText);

    auto* pFindNextBtn = ui::Create<ui::Button>(this, {{DUI_T("width"), DUI_T("80")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("margin"), DUI_T("4,8,8,0")}});
    pFindNextBtn->SetClass(DUI_T("btn_global_white_80x30"));
    pFindNextBtn->SetName(DUI_T("btn_find_next"));
    pFindNextBtn->SetText(DUI_T("Find Next"));
    pFindRow->AddItem(pFindNextBtn);

    // Replace-with row
    auto* pReplaceRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    pContent->AddItem(pReplaceRow);

    auto* pReplaceLabel = ui::Create<ui::Label>(this, {{DUI_T("height"), DUI_T("28")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("margin"), DUI_T("8,8,0,0")}});
    pReplaceLabel->SetText(DUI_T(" Replace with:"));
    pReplaceRow->AddItem(pReplaceLabel);

    auto* pReplaceText = ui::Create<ui::RichEdit>(this, {{DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("prompttext"), DUI_T("Replace")}, {DUI_T("text_padding"), DUI_T("2,0,0,0")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("margin"), DUI_T("2,8,4,0")}});
    pReplaceText->SetClass(DUI_T("simple prompt simple_border"));
    pReplaceText->SetName(DUI_T("btn_replace_text"));
    pReplaceText->SetBkColor(DUI_T("white"));
    pReplaceRow->AddItem(pReplaceText);

    auto* pReplaceBtn = ui::Create<ui::Button>(this, {{DUI_T("width"), DUI_T("80")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("margin"), DUI_T("4,8,4,0")}});
    pReplaceBtn->SetClass(DUI_T("btn_global_white_80x30"));
    pReplaceBtn->SetName(DUI_T("btn_replace"));
    pReplaceBtn->SetText(DUI_T("Replace"));
    pReplaceRow->AddItem(pReplaceBtn);

    auto* pReplaceAllBtn = ui::Create<ui::Button>(this, {{DUI_T("width"), DUI_T("80")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("margin"), DUI_T("4,8,8,0")}});
    pReplaceAllBtn->SetClass(DUI_T("btn_global_white_80x30"));
    pReplaceAllBtn->SetName(DUI_T("btn_replace_all"));
    pReplaceAllBtn->SetText(DUI_T("Replace All"));
    pReplaceRow->AddItem(pReplaceAllBtn);

    // Search direction
    auto* pDirectionGroup = ui::Create<ui::GroupVBox>(this, {{DUI_T("margin"), DUI_T("10,6,10,6")}, {DUI_T("text"), DUI_T(" Search Direction ")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("corner_size"), DUI_T("4,4")}});
    pContent->AddItem(pDirectionGroup);

    auto* pDirectionRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("40")}, {DUI_T("margin"), DUI_T("24,12,8,4")}, {DUI_T("padding"), DUI_T("12,0,0,0")}});
    pDirectionGroup->AddItem(pDirectionRow);

    auto* pOptionUp = ui::Create<ui::Option>(this, {{DUI_T("group"), DUI_T("option_direction_group")}, {DUI_T("margin"), DUI_T("0,6,0,0")}});
    pOptionUp->SetClass(DUI_T("option_1"));
    pOptionUp->SetText(DUI_T("Search Up"));
    pDirectionRow->AddItem(pOptionUp);

    auto* pOptionDown = ui::Create<ui::Option>(this, {{DUI_T("group"), DUI_T("option_direction_group")}, {DUI_T("margin"), DUI_T("16,6,0,0")}});
    pOptionDown->SetClass(DUI_T("option_1"));
    pOptionDown->SetText(DUI_T("Search Down"));
    pOptionDown->SetName(DUI_T("option_direction_down"));
    pOptionDown->Selected(true);
    pDirectionRow->AddItem(pOptionDown);

    // Options row
    auto* pOptionRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("auto")}});
    pContent->AddItem(pOptionRow);

    auto* pCaseSensitive = ui::Create<ui::CheckBox>(this, {{DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,8,8,0")}});
    pCaseSensitive->SetClass(DUI_T("checkbox_1"));
    pCaseSensitive->SetName(DUI_T("check_box_case_sensitive"));
    pCaseSensitive->SetText(DUI_T("Case Sensitive"));
    pCaseSensitive->Selected(true);
    pOptionRow->AddItem(pCaseSensitive);

    auto* pMatchWholeWord = ui::Create<ui::CheckBox>(this, {{DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,8,8,0")}});
    pMatchWholeWord->SetClass(DUI_T("checkbox_1"));
    pMatchWholeWord->SetName(DUI_T("check_box_match_whole_word"));
    pMatchWholeWord->SetText(DUI_T("Match Whole Word"));
    pOptionRow->AddItem(pMatchWholeWord);

    auto* pOptionSpacer = ui::Create<ui::Control>(this, {});
    pOptionRow->AddItem(pOptionSpacer);

    auto* pCancelBtn = ui::Create<ui::Button>(this, {{DUI_T("width"), DUI_T("80")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("margin"), DUI_T("4,8,8,0")}});
    pCancelBtn->SetClass(DUI_T("btn_global_white_80x30"));
    pCancelBtn->SetName(DUI_T("btn_cancel"));
    pCancelBtn->SetText(DUI_T("Cancel"));
    pOptionRow->AddItem(pCancelBtn);

    AttachBox(pRoot);
}

void ReplaceForm::OnInitWindow()
{
    // Hand-written pure code UI (corresponds to the replace.xml layout, equivalent to the generator output)
    BuildUI();

    m_pFindText = ui::Find<ui::RichEdit>(this, DUI_T("btn_find_text"));
    m_pReplaceText = ui::Find<ui::RichEdit>(this, DUI_T("btn_replace_text"));
    m_pDirectionOption = ui::Find<ui::Option>(this, DUI_T("option_direction_down"));
    m_pCaseSensitive = ui::Find<ui::CheckBox>(this, DUI_T("check_box_case_sensitive"));
    m_pMatchWholeWord = ui::Find<ui::CheckBox>(this, DUI_T("check_box_match_whole_word"));
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
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_replace"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    OnReplace();
                }
                return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_replace_all"));
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
