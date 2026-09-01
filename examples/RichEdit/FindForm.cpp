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
    return DUI_T("find.xml");
}

void FindForm::OnInitWindow()
{
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
