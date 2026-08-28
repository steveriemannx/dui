#include "FindForm.h"
#include "MainForm.h"

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
    return _T("rich_edit");
}

DString FindForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from find.xml
    return _T("");
}

void FindForm::BuildUI()
{
    // Corresponds to the find.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar
    auto* pCaption = ui::Create<ui::HBox>(this, {{_T("name"), _T("window_caption_bar")}, {_T("width"), _T("stretch")}, {_T("height"), _T("35")}});
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pLogo = ui::Create<ui::Control>(this, {{_T("width"), _T("18")}, {_T("height"), _T("18")}, {_T("valign"), _T("center")}, {_T("margin"), _T("8")}});
    pLogo->SetBkImage(_T("public/caption/logo.svg"));
    pCaption->AddItem(pLogo);

    auto* pTitle = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}, {_T("margin"), _T("8")}});
    pTitle->SetText(_T("Find"));
    pCaption->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {});
    pCaption->AddItem(pSpacer);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("40")}, {_T("height"), _T("32")}, {_T("margin"), _T("4,0,0,0")}});
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetName(_T("closebtn"));
    pCaption->AddItem(pCloseBtn);

    // Find content row
    auto* pContent = ui::Create<ui::VBox>(this, {});
    pRoot->AddItem(pContent);

    auto* pFindRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}});
    pContent->AddItem(pFindRow);

    auto* pFindLabel = ui::Create<ui::Label>(this, {{_T("height"), _T("28")}, {_T("text_align"), _T("vcenter")}, {_T("margin"), _T("8,8,0,0")}});
    pFindLabel->SetText(_T("Find What:"));
    pFindRow->AddItem(pFindLabel);

    auto* pFindText = ui::Create<ui::RichEdit>(this, {{_T("width"), _T("stretch")}, {_T("height"), _T("28")}, {_T("prompttext"), _T("Find")}, {_T("text_padding"), _T("2,0,0,0")}, {_T("text_align"), _T("vcenter")}, {_T("margin"), _T("2,8,4,0")}});
    pFindText->SetClass(_T("simple prompt simple_border"));
    pFindText->SetName(_T("btn_find_text"));
    pFindText->SetBkColor(_T("white"));
    pFindRow->AddItem(pFindText);

    auto* pFindNextBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("80")}, {_T("height"), _T("28")}, {_T("margin"), _T("4,8,8,0")}});
    pFindNextBtn->SetClass(_T("btn_global_white_80x30"));
    pFindNextBtn->SetName(_T("btn_find_next"));
    pFindNextBtn->SetText(_T("Find Next"));
    pFindRow->AddItem(pFindNextBtn);

    // Search direction
    auto* pDirectionGroup = ui::Create<ui::GroupVBox>(this, {{_T("margin"), _T("10,6,10,6")}, {_T("text"), _T(" Search Direction ")}, {_T("height"), _T("auto")}, {_T("corner_size"), _T("4,4")}});
    pContent->AddItem(pDirectionGroup);

    auto* pDirectionRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("40")}, {_T("margin"), _T("24,12,8,4")}, {_T("padding"), _T("12,0,0,0")}});
    pDirectionGroup->AddItem(pDirectionRow);

    auto* pOptionUp = ui::Create<ui::Option>(this, {{_T("group"), _T("option_direction_group")}, {_T("margin"), _T("0,6,0,0")}});
    pOptionUp->SetClass(_T("option_1"));
    pOptionUp->SetText(_T("Search Up"));
    pDirectionRow->AddItem(pOptionUp);

    auto* pOptionDown = ui::Create<ui::Option>(this, {{_T("group"), _T("option_direction_group")}, {_T("margin"), _T("16,6,0,0")}});
    pOptionDown->SetClass(_T("option_1"));
    pOptionDown->SetText(_T("Search Down"));
    pOptionDown->SetName(_T("option_direction_down"));
    pOptionDown->Selected(true);
    pDirectionRow->AddItem(pOptionDown);

    // Options row
    auto* pOptionRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}});
    pContent->AddItem(pOptionRow);

    auto* pCaseSensitive = ui::Create<ui::CheckBox>(this, {{_T("valign"), _T("center")}, {_T("margin"), _T("8,8,8,0")}});
    pCaseSensitive->SetClass(_T("checkbox_1"));
    pCaseSensitive->SetName(_T("check_box_case_sensitive"));
    pCaseSensitive->SetText(_T("Case Sensitive"));
    pCaseSensitive->Selected(true);
    pOptionRow->AddItem(pCaseSensitive);

    auto* pMatchWholeWord = ui::Create<ui::CheckBox>(this, {{_T("valign"), _T("center")}, {_T("margin"), _T("8,8,8,0")}});
    pMatchWholeWord->SetClass(_T("checkbox_1"));
    pMatchWholeWord->SetName(_T("check_box_match_whole_word"));
    pMatchWholeWord->SetText(_T("Match Whole Word"));
    pOptionRow->AddItem(pMatchWholeWord);

    auto* pOptionSpacer = ui::Create<ui::Control>(this, {});
    pOptionRow->AddItem(pOptionSpacer);

    auto* pCancelBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("80")}, {_T("height"), _T("28")}, {_T("margin"), _T("4,8,8,0")}});
    pCancelBtn->SetClass(_T("btn_global_white_80x30"));
    pCancelBtn->SetName(_T("btn_cancel"));
    pCancelBtn->SetText(_T("Cancel"));
    pOptionRow->AddItem(pCancelBtn);

    AttachBox(pRoot);
}

void FindForm::OnInitWindow()
{
    // Hand-written pure code UI (corresponds to the find.xml layout, equivalent to the generator output)
    BuildUI();

    m_pFindText = dynamic_cast<ui::RichEdit*>(FindControl(_T("btn_find_text")));
    m_pDirectionOption = dynamic_cast<ui::Option*>(FindControl(_T("option_direction_down")));
    m_pCaseSensitive = dynamic_cast<ui::CheckBox*>(FindControl(_T("check_box_case_sensitive")));
    m_pMatchWholeWord = dynamic_cast<ui::CheckBox*>(FindControl(_T("check_box_match_whole_word")));
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

    ui::Button* pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_cancel")));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    CloseWnd();
                }
                return true;
            });
    }
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_find_next")));
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
