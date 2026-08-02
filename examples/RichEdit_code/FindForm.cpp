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
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("35"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::Control* pLogo = new ui::Control(this);
    pLogo->SetAttribute(_T("width"), _T("18"));
    pLogo->SetAttribute(_T("height"), _T("18"));
    pLogo->SetBkImage(_T("public/caption/logo.svg"));
    pLogo->SetAttribute(_T("valign"), _T("center"));
    pLogo->SetAttribute(_T("margin"), _T("8"));
    pCaption->AddItem(pLogo);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetText(_T("Find"));
    pTitle->SetAttribute(_T("valign"), _T("center"));
    pTitle->SetAttribute(_T("margin"), _T("8"));
    pCaption->AddItem(pTitle);

    ui::Control* pSpacer = new ui::Control(this);
    pCaption->AddItem(pSpacer);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetAttribute(_T("width"), _T("40"));
    pCloseBtn->SetAttribute(_T("height"), _T("32"));
    pCloseBtn->SetAttribute(_T("margin"), _T("4,0,0,0"));
    pCaption->AddItem(pCloseBtn);

    // Find content row
    ui::VBox* pContent = new ui::VBox(this);
    pRoot->AddItem(pContent);

    ui::HBox* pFindRow = new ui::HBox(this);
    pFindRow->SetAttribute(_T("height"), _T("auto"));
    pContent->AddItem(pFindRow);

    ui::Label* pFindLabel = new ui::Label(this);
    pFindLabel->SetText(_T("Find What:"));
    pFindLabel->SetAttribute(_T("height"), _T("28"));
    pFindLabel->SetAttribute(_T("text_align"), _T("vcenter"));
    pFindLabel->SetAttribute(_T("margin"), _T("8,8,0,0"));
    pFindRow->AddItem(pFindLabel);

    ui::RichEdit* pFindText = new ui::RichEdit(this);
    pFindText->SetClass(_T("simple prompt simple_border"));
    pFindText->SetName(_T("btn_find_text"));
    pFindText->SetAttribute(_T("width"), _T("stretch"));
    pFindText->SetAttribute(_T("height"), _T("28"));
    pFindText->SetAttribute(_T("prompttext"), _T("Find"));
    pFindText->SetAttribute(_T("text_padding"), _T("2,0,0,0"));
    pFindText->SetAttribute(_T("text_align"), _T("vcenter"));
    pFindText->SetBkColor(_T("white"));
    pFindText->SetAttribute(_T("margin"), _T("2,8,4,0"));
    pFindRow->AddItem(pFindText);

    ui::Button* pFindNextBtn = new ui::Button(this);
    pFindNextBtn->SetClass(_T("btn_global_white_80x30"));
    pFindNextBtn->SetName(_T("btn_find_next"));
    pFindNextBtn->SetText(_T("Find Next"));
    pFindNextBtn->SetAttribute(_T("width"), _T("80"));
    pFindNextBtn->SetAttribute(_T("height"), _T("28"));
    pFindNextBtn->SetAttribute(_T("margin"), _T("4,8,8,0"));
    pFindRow->AddItem(pFindNextBtn);

    // Search direction
    ui::GroupVBox* pDirectionGroup = new ui::GroupVBox(this);
    pDirectionGroup->SetAttribute(_T("margin"), _T("10,6,10,6"));
    pDirectionGroup->SetAttribute(_T("text"), _T(" Search Direction "));
    pDirectionGroup->SetAttribute(_T("height"), _T("auto"));
    pDirectionGroup->SetAttribute(_T("corner_size"), _T("4,4"));
    pContent->AddItem(pDirectionGroup);

    ui::HBox* pDirectionRow = new ui::HBox(this);
    pDirectionRow->SetAttribute(_T("height"), _T("40"));
    pDirectionRow->SetAttribute(_T("margin"), _T("24,12,8,4"));
    pDirectionRow->SetAttribute(_T("padding"), _T("12,0,0,0"));
    pDirectionGroup->AddItem(pDirectionRow);

    ui::Option* pOptionUp = new ui::Option(this);
    pOptionUp->SetClass(_T("option_1"));
    pOptionUp->SetAttribute(_T("group"), _T("option_direction_group"));
    pOptionUp->SetText(_T("Search Up"));
    pOptionUp->SetAttribute(_T("margin"), _T("0,6,0,0"));
    pDirectionRow->AddItem(pOptionUp);

    ui::Option* pOptionDown = new ui::Option(this);
    pOptionDown->SetClass(_T("option_1"));
    pOptionDown->SetAttribute(_T("group"), _T("option_direction_group"));
    pOptionDown->SetText(_T("Search Down"));
    pOptionDown->SetName(_T("option_direction_down"));
    pOptionDown->SetAttribute(_T("margin"), _T("16,6,0,0"));
    pOptionDown->Selected(true);
    pDirectionRow->AddItem(pOptionDown);

    // Options row
    ui::HBox* pOptionRow = new ui::HBox(this);
    pOptionRow->SetAttribute(_T("height"), _T("auto"));
    pContent->AddItem(pOptionRow);

    ui::CheckBox* pCaseSensitive = new ui::CheckBox(this);
    pCaseSensitive->SetClass(_T("checkbox_1"));
    pCaseSensitive->SetName(_T("check_box_case_sensitive"));
    pCaseSensitive->SetText(_T("Case Sensitive"));
    pCaseSensitive->SetAttribute(_T("valign"), _T("center"));
    pCaseSensitive->SetAttribute(_T("margin"), _T("8,8,8,0"));
    pCaseSensitive->Selected(true);
    pOptionRow->AddItem(pCaseSensitive);

    ui::CheckBox* pMatchWholeWord = new ui::CheckBox(this);
    pMatchWholeWord->SetClass(_T("checkbox_1"));
    pMatchWholeWord->SetName(_T("check_box_match_whole_word"));
    pMatchWholeWord->SetText(_T("Match Whole Word"));
    pMatchWholeWord->SetAttribute(_T("valign"), _T("center"));
    pMatchWholeWord->SetAttribute(_T("margin"), _T("8,8,8,0"));
    pOptionRow->AddItem(pMatchWholeWord);

    ui::Control* pOptionSpacer = new ui::Control(this);
    pOptionRow->AddItem(pOptionSpacer);

    ui::Button* pCancelBtn = new ui::Button(this);
    pCancelBtn->SetClass(_T("btn_global_white_80x30"));
    pCancelBtn->SetName(_T("btn_cancel"));
    pCancelBtn->SetText(_T("Cancel"));
    pCancelBtn->SetAttribute(_T("width"), _T("80"));
    pCancelBtn->SetAttribute(_T("height"), _T("28"));
    pCancelBtn->SetAttribute(_T("margin"), _T("4,8,8,0"));
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
