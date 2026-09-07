#include "MainForm.h"
#include "FindForm.h"
#include "ReplaceForm.h"
#include "dui/Utils/UiBuilder.h"
#include <fstream>

#if defined (DUI_BUILD_FOR_WIN)
#include <ShellApi.h>
#include <commdlg.h>
#endif

static void BuildRichEditUI(ui::Window* pWindow);

#ifndef LY_PER_INCH
    #define LY_PER_INCH 1440
#endif

MainForm::MainForm():
    m_pRichEdit(nullptr),
    m_pFindForm(nullptr),
    m_pReplaceForm(nullptr)
{
}

void MainForm::SetupWindow()
{
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    SetWindowSize((int32_t)(rcWork.Width() * 0.90f), (int32_t)(rcWork.Height() * 0.80f));
    CenterWindow();
    SetShadowAttached(true);
#if defined(DUI_BUILD_FOR_LINUX)
    SetShadowType(ui::Shadow::ShadowType::kShadowDrawDefault);
#else
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
#endif
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildRichEditUI(this);

    m_pRichEdit = ui::Find<ui::RichEdit>(this, DUI_T("rich_edit"));
    ASSERT(m_pRichEdit != nullptr);
    m_findReplace.SetRichEdit(m_pRichEdit);
    LoadRichEditData();

    // Initialize font information
    ui::Combo* pFontNameCombo = ui::Find<ui::Combo>(this, DUI_T("combo_font_name"));
    if (pFontNameCombo != nullptr) {
        m_fontList.clear();
        ui::GlobalManager::Instance().Font().GetFontNameList(m_fontList);
        for (size_t nIndex = 0; nIndex < m_fontList.size(); ++nIndex) {
            const DString& fontName = m_fontList[nIndex];
            size_t nItemIndex = pFontNameCombo->AddTextItem(fontName);
            if (ui::Box::IsValidItemIndex(nItemIndex)) {
                pFontNameCombo->SetItemData(nItemIndex, nIndex);
            }
        }
    }
    ui::Combo* pFontSizeCombo = ui::Find<ui::Combo>(this, DUI_T("combo_font_size"));
    if (pFontSizeCombo != nullptr) {
        ui::GlobalManager::Instance().Font().GetFontSizeList(Dpi(), m_fontSizeList);
        for (size_t nIndex = 0; nIndex < m_fontSizeList.size(); ++nIndex) {
            const ui::FontSizeInfo& fontSize = m_fontSizeList[nIndex];
            size_t nItemIndex = pFontSizeCombo->AddTextItem(fontSize.fontSizeName);
            if (ui::Box::IsValidItemIndex(nItemIndex)) {
                pFontSizeCombo->SetItemData(nItemIndex, nIndex);
            }
        }
    }

    // Set color
    InitColorCombo();
    ui::ComboButton* pColorComboBtn = ui::Find<ui::ComboButton>(this, DUI_T("color_combo_button"));
    if (pColorComboBtn != nullptr) {
        DString textColor;
        if (m_pRichEdit != nullptr) {
            textColor = m_pRichEdit->GetTextColor();
        }
        ui::Label* pLeftColorLabel = pColorComboBtn->GetLabelBottom();
        if (pLeftColorLabel != nullptr) {
            pLeftColorLabel->SetBkColor(textColor);
        }
    }

    UpdateZoomValue();

    // Whether to wrap text automatically
    ui::CheckBox* pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_word_wrap"));
    if ((pCheckBox != nullptr) && (m_pRichEdit != nullptr)) {
        pCheckBox->SetSelected(m_pRichEdit->IsWordWrap());
    }

    // Whether rich text format is supported
    pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_rich_text"));
    if ((pCheckBox != nullptr) && (m_pRichEdit != nullptr)) {
        pCheckBox->SetSelected(m_pRichEdit->IsRichText());
#if defined (DUI_BUILD_FOR_WIN)
#else
        // Rich text format is not supported in the native backend implementation
        pCheckBox->SetEnabled(false);
#endif
    }

#ifdef DUI_BUILD_FOR_WAYLAND
    ui::Control* pRowSpacingTips = ui::Find<ui::Control>(this, DUI_T("row_spacing_tips"));
    if (pRowSpacingTips != nullptr) {
        pRowSpacingTips->SetVisible(false);
    }
#endif

    // Update the state of the font buttons
    UpdateFontStatus();

    BindEvents();
    BaseClass::OnInitWindow();
}

void MainForm::BindEvents()
{
    // Hyperlink clicked in the test URL display
    ui::RichEdit* pTestUrl = ui::Find<ui::RichEdit>(this, DUI_T("test_url"));
    if (pTestUrl != nullptr) {
        pTestUrl->AttachLinkClick([this, pTestUrl](const ui::EventArgs& args) {
                if (args.GetSender() == pTestUrl) {
                    const DString::value_type* pUrl = (const DString::value_type*)args.wParam;
                    if (pUrl != nullptr) {
#if defined (DUI_BUILD_FOR_WIN)
                        ::ShellExecuteW(NativeWnd()->GetHWND(), L"open", ui::StringConvert::TToWString(pUrl).c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#endif
                    }
                }
                return true;
            });
    }

    // File operations: open, save, save as
    ui::Button* pButton = ui::Find<ui::Button>(this, DUI_T("open_file"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    this->OnOpenFile();
                }
                return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, DUI_T("save_file"));
    if (pButton != nullptr) {
        m_saveBtnText = pButton->GetText();
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    this->OnSaveFile();
                }
                return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, DUI_T("save_as_file"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    this->OnSaveAsFile();
                }
                return true;
            });
    }

    // Edit operations
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_copy"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                if (m_pRichEdit != nullptr) {
                    m_pRichEdit->Copy();
                    UpdateSaveStatus();
                }
            }
            return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_cut"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                if (m_pRichEdit != nullptr) {
                    m_pRichEdit->Cut();
                    UpdateSaveStatus();
                }
            }
            return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_paste"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                if (m_pRichEdit != nullptr) {
                    m_pRichEdit->Paste();
                    UpdateSaveStatus();
                }
            }
            return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_delete"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                if (m_pRichEdit != nullptr) {
                    m_pRichEdit->Clear();
                    UpdateSaveStatus();
                }
            }
            return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_sel_all"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                if (m_pRichEdit != nullptr) {
                    m_pRichEdit->SetSelAll();
                    UpdateSaveStatus();
                }
            }
            return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_sel_none"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                if (m_pRichEdit != nullptr) {
                    m_pRichEdit->SetSelNone();
                    UpdateSaveStatus();
                }
            }
            return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_undo"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                if (m_pRichEdit != nullptr) {
                    m_pRichEdit->Undo();
                    UpdateSaveStatus();
                }
            }
            return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_redo"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                if (m_pRichEdit != nullptr) {
                    m_pRichEdit->Redo();
                    UpdateSaveStatus();
                }
            }
            return true;
            });
    }

    // Find operations
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_find_text"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                OnFindText();
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
    pButton = ui::Find<ui::Button>(this, DUI_T("btn_replace_text"));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                OnReplaceText();
            }
            return true;
            });
    }

    // Set font
    pButton = ui::Find<ui::Button>(this, DUI_T("set_font"));
    if (pButton != nullptr) {
#if defined (DUI_BUILD_FOR_WIN)
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                OnSetFont();
            }
            return true;
            });
#else
        pButton->SetEnabled(false);
#endif
    }

    // Font name combo
    ui::Combo* pFontNameCombo = ui::Find<ui::Combo>(this, DUI_T("combo_font_name"));
    if (pFontNameCombo != nullptr) {
        pFontNameCombo->AttachSelect([this, pFontNameCombo](const ui::EventArgs& args) {
            DString fontName = pFontNameCombo->GetText();
            SetFontName(fontName);
            return true;
            });
        pFontNameCombo->AttachWindowClose([this, pFontNameCombo](const ui::EventArgs& args) {
            DString fontName = pFontNameCombo->GetText();
            SetFontName(fontName);
            return true;
            });
    }
    ui::Combo* pFontSizeCombo = ui::Find<ui::Combo>(this, DUI_T("combo_font_size"));
    if (pFontSizeCombo != nullptr) {
        pFontSizeCombo->AttachSelect([this, pFontSizeCombo](const ui::EventArgs& args) {
            DString fontName = pFontSizeCombo->GetText();
            SetFontSize(fontName);
            return true;
            });
        pFontSizeCombo->AttachWindowClose([this, pFontSizeCombo](const ui::EventArgs& args) {
            DString fontName = pFontSizeCombo->GetText();
            SetFontSize(fontName);
            return true;
            });
    }

    // Font style: bold
    ui::CheckBox* pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_font_bold"));
    if (pCheckBox != nullptr) {
        pCheckBox->AttachSelect([this, pCheckBox](const ui::EventArgs& args) {
            SetFontBold(pCheckBox->IsSelected());
            return true;
            });
        pCheckBox->AttachUnSelect([this, pCheckBox](const ui::EventArgs& args) {
            SetFontBold(pCheckBox->IsSelected());
            return true;
            });
    }
    // Font style: italic
    pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_font_italic"));
    if (pCheckBox != nullptr) {
        pCheckBox->AttachSelect([this, pCheckBox](const ui::EventArgs& args) {
            SetFontItalic(pCheckBox->IsSelected());
            return true;
            });
        pCheckBox->AttachUnSelect([this, pCheckBox](const ui::EventArgs& args) {
            SetFontItalic(pCheckBox->IsSelected());
            return true;
            });
    }
    // Font style: underline
    pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_font_underline"));
    if (pCheckBox != nullptr) {
        pCheckBox->AttachSelect([this, pCheckBox](const ui::EventArgs& args) {
            SetFontUnderline(pCheckBox->IsSelected());
            return true;
            });
        pCheckBox->AttachUnSelect([this, pCheckBox](const ui::EventArgs& args) {
            SetFontUnderline(pCheckBox->IsSelected());
            return true;
            });
    }
    // Font style: strikethrough
    pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_font_strikeout"));
    if (pCheckBox != nullptr) {
        pCheckBox->AttachSelect([this, pCheckBox](const ui::EventArgs& args) {
            SetFontStrikeOut(pCheckBox->IsSelected());
            return true;
            });
        pCheckBox->AttachUnSelect([this, pCheckBox](const ui::EventArgs& args) {
            SetFontStrikeOut(pCheckBox->IsSelected());
            return true;
            });
    }

    // Increase/decrease font size
    ui::Button* pFontButton = ui::Find<ui::Button>(this, DUI_T("btn_font_size_increase"));
    if (pFontButton != nullptr) {
        pFontButton->AttachClick([this](const ui::EventArgs& args) {
            AdjustFontSize(true);
            return true;
            });
    }
    pFontButton = ui::Find<ui::Button>(this, DUI_T("btn_font_size_decrease"));
    if (pFontButton != nullptr) {
        pFontButton->AttachClick([this](const ui::EventArgs& args) {
            AdjustFontSize(false);
            return true;
            });
    }

    // Color: left button click
    ui::ComboButton* pColorComboBtn = ui::Find<ui::ComboButton>(this, DUI_T("color_combo_button"));
    if (pColorComboBtn != nullptr) {
        pColorComboBtn->AttachClick([this, pColorComboBtn](const ui::EventArgs& args) {
            ui::Label* pLeftColorLabel = pColorComboBtn->GetLabelBottom();
            if (pLeftColorLabel != nullptr) {
                SetTextColor(pLeftColorLabel->GetBkColor());
            }
            return true;
            });
    }

    // RichEdit zoom
    if (m_pRichEdit != nullptr) {
        m_pRichEdit->AttachZoom([this](const ui::EventArgs& args) {
            UpdateZoomValue();
            return true;
            });
    }

    // Zoom buttons
    ui::Button* pZoomButtom = ui::Find<ui::Button>(this, DUI_T("btn_zoom_in"));
    if (pZoomButtom != nullptr) {
        pZoomButtom->AttachClick([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                uint32_t nZoomPercent = GetNextZoomPercent(m_pRichEdit->GetZoomPercent(), true);
                m_pRichEdit->SetZoomPercent(nZoomPercent);
                UpdateZoomValue();
            }
            return true;
            });
    }
    pZoomButtom = ui::Find<ui::Button>(this, DUI_T("btn_zoom_out"));
    if (pZoomButtom != nullptr) {
        pZoomButtom->AttachClick([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                uint32_t nZoomPercent = GetNextZoomPercent(m_pRichEdit->GetZoomPercent(), false);
                m_pRichEdit->SetZoomPercent(nZoomPercent);
                UpdateZoomValue();
            }
            return true;
            });
    }
    pZoomButtom = ui::Find<ui::Button>(this, DUI_T("btn_zoom_off"));
    if (pZoomButtom != nullptr) {
        pZoomButtom->AttachClick([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                m_pRichEdit->SetZoomPercent(100);
                UpdateZoomValue();
            }
            return true;
            });
    }

    // Word wrap
    pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_word_wrap"));
    if ((pCheckBox != nullptr) && (m_pRichEdit != nullptr)) {
        pCheckBox->AttachSelect([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                m_pRichEdit->SetWordWrap(true);
                m_pRichEdit->SetAttribute(DUI_T("hscrollbar"), DUI_T("false"));
            }
            return true;
            });
        pCheckBox->AttachUnSelect([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                m_pRichEdit->SetWordWrap(false);
                m_pRichEdit->SetAttribute(DUI_T("hscrollbar"), DUI_T("true"));
            }
            return true;
            });
    }

    // Rich text format
    pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_rich_text"));
    if ((pCheckBox != nullptr) && (m_pRichEdit != nullptr)) {
#if defined (DUI_BUILD_FOR_WIN)
        pCheckBox->AttachSelect([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                m_pRichEdit->SetRichText(true);
            }
            return true;
            });
        pCheckBox->AttachUnSelect([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                m_pRichEdit->SetRichText(false);
            }
            return true;
            });
#endif
    }

    // Hyperlink (rich edit)
    if (m_pRichEdit != nullptr) {
        m_pRichEdit->AttachLinkClick([this](const ui::EventArgs& args) {
            const DString::value_type* url = (const DString::value_type*)args.wParam;
            if (url != nullptr) {
#if defined (DUI_BUILD_FOR_WIN)
                ::MessageBoxW(NativeWnd()->GetHWND(), ui::StringConvert::TToWString(url).c_str(), L"RichEdit Click HyperLink", MB_OK);
#endif
            }
            return true;
            });
    }
    // RichEdit text selection changed
    if (m_pRichEdit != nullptr) {
        m_pRichEdit->AttachSelChanged([this](const ui::EventArgs& args) {
            if ((m_pRichEdit != nullptr) && m_pRichEdit->IsRichText()) {
                UpdateFontStatus();
            }
            return true;
            });
    }
}

uint32_t MainForm::GetNextZoomPercent(uint32_t nOldZoomPercent, bool bZoomIn) const
{
    // Maximum zoom percentage
    const uint32_t MAX_ZOOM_PERCENT = 800;

    // Set the zoom ratio by looking up a table (so zooming in/out can restore the original ratio)
    std::vector<uint32_t> zoomPercentList;
    uint32_t nZoomPercent = 100;
    while (nZoomPercent > 1) {
        nZoomPercent = (uint32_t)(nZoomPercent * 0.90f);
        zoomPercentList.insert(zoomPercentList.begin(), nZoomPercent);
    }
    nZoomPercent = 100;
    while (nZoomPercent < MAX_ZOOM_PERCENT) {
        zoomPercentList.insert(zoomPercentList.end(), nZoomPercent);
        nZoomPercent = (uint32_t)(nZoomPercent * 1.10f);
    }
    zoomPercentList.insert(zoomPercentList.end(), MAX_ZOOM_PERCENT);

    bool bFound = false;
    nZoomPercent = nOldZoomPercent;
    const size_t nPercentCount = zoomPercentList.size();
    for (size_t nPercentIndex = 0; nPercentIndex < nPercentCount; ++nPercentIndex) {
        if ((zoomPercentList[nPercentIndex] > nZoomPercent) || (nPercentIndex == (nPercentCount - 1))) {
            if (nPercentIndex <= 1) {
                size_t nCurrentIndex = 0;// Currently the first element
                if (bZoomIn) {
                    // Zoom in
                    nZoomPercent = zoomPercentList[nCurrentIndex + 1];
                }
                else {
                    // Zoom out (already at the minimum, cannot zoom out further)
                    nZoomPercent = zoomPercentList[nCurrentIndex];
                }
            }
            else if (zoomPercentList[nPercentIndex] > nZoomPercent) {
                size_t nCurrentIndex = nPercentIndex - 1;// Middle element
                if (bZoomIn) {
                    // Zoom in
                    nZoomPercent = zoomPercentList[nCurrentIndex + 1];
                }
                else {
                    // Zoom out
                    nZoomPercent = zoomPercentList[nCurrentIndex - 1];
                }
            }
            else if (nPercentIndex == (nPercentCount - 1)) {
                size_t nCurrentIndex = nPercentCount - 1;// Currently the last element
                if (bZoomIn) {
                    // Zoom in (already at the maximum, cannot zoom in further)
                    nZoomPercent = zoomPercentList[nCurrentIndex];
                }
                else {
                    // Zoom out
                    nZoomPercent = zoomPercentList[nCurrentIndex - 1];
                }
            }
            bFound = true;
            break;
        }
    }

    if (!bFound) {
        // If the table lookup fails, zoom in or out proportionally
        if (bZoomIn) {
            // Zoom in
            nZoomPercent = (uint32_t)(nZoomPercent * 1.10f);
            if (nZoomPercent == nOldZoomPercent) {
                // Avoid being unable to zoom in when the value is too small
                ++nZoomPercent;
            }
        }
        else {
            // Zoom out
            nZoomPercent = (uint32_t)(nZoomPercent * 0.91f);
        }
    }
    if (nZoomPercent < 1) {
        nZoomPercent = 1;
    }
    return nZoomPercent;
}

void MainForm::InitColorCombo()
{
    ui::ComboButton* pColorComboBtn = ui::Find<ui::ComboButton>(this, DUI_T("color_combo_button"));
    if (pColorComboBtn == nullptr) {
        return;
    }
    ui::UiSize boxSize = pColorComboBtn->GetDropBoxSize();
    ui::Box* pComboBox = pColorComboBtn->GetComboBox();
    if (pComboBox == nullptr) {
        return;
    }
    pComboBox->SetWindow(this);
    ui::GlobalManager::Instance().FillBoxWithCache(pComboBox, ui::FilePath(DUI_T("rich_edit/color_combox.xml")));
    ui::ColorPickerRegular* pColorPicker = dynamic_cast<ui::ColorPickerRegular*>(pComboBox->FindSubControl(DUI_T("color_combo_picker")));
    if (pColorPicker != nullptr) {
        // Respond to the color selection event
        pColorPicker->AttachSelectColor([this, pColorComboBtn](const ui::EventArgs& args) {
            ui::UiColor newColor((uint32_t)args.wParam);
            // Set the color after selection
            ui::Label* pLeftColorLabel = pColorComboBtn->GetLabelBottom();
            if (pLeftColorLabel != nullptr) {
                pLeftColorLabel->SetBkColor(newColor);
                SetTextColor(pLeftColorLabel->GetBkColor());
            }
            return true;
            });
    }

    ui::Button* pMoreColorButton = dynamic_cast<ui::Button*>(pComboBox->FindSubControl(DUI_T("color_combo_picker_more")));
    if (pMoreColorButton != nullptr) {
        pMoreColorButton->AttachClick([this](const ui::EventArgs& args) {
            ShowColorPicker();
            return true;
            });
    }
}

void MainForm::ShowColorPicker()
{
    ui::ComboButton* pColorComboBtn = ui::Find<ui::ComboButton>(this, DUI_T("color_combo_button"));
    if (pColorComboBtn == nullptr) {
        return;
    }
    ui::Label* pLeftColorLabel = pColorComboBtn->GetLabelBottom();
    if (pLeftColorLabel == nullptr) {
        return;
    }
    DString oldTextColor = pLeftColorLabel->GetBkColor();

    ui::ColorPicker* pColorPicker = new ui::ColorPicker;
    ui::WindowCreateParam createParam;
    createParam.m_dwStyle = ui::kWS_POPUP;
    createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
    createParam.m_windowTitle = DUI_T("ColorPicker");
    createParam.m_bCenterWindow = true;
#ifdef DUI_BUILD_FOR_WIN
    pColorPicker->CreateWnd(nullptr, createParam);
    pColorPicker->ShowModalFake();
#else
    pColorPicker->CreateWnd(this, createParam);
    pColorPicker->ShowModalFake();
    pColorPicker->SetWindowForeground();
#endif

    ui::RichEdit* pEdit = m_pRichEdit;
    if (pEdit != nullptr) {
        if (!oldTextColor.empty()) {
            pColorPicker->SetSelectedColor(pEdit->GetUiColor(oldTextColor));
        }
        // If a color is selected in the UI, temporarily update the text color of the RichEdit control
        pColorPicker->AttachSelectColor([this, pEdit, pLeftColorLabel](const ui::EventArgs& args) {
            ui::UiColor newColor = ui::UiColor((uint32_t)args.wParam);
            pLeftColorLabel->SetBkColor(newColor);
            SetTextColor(pEdit->GetColorString(newColor));
            return true;
            });

        // Window close event
        pColorPicker->AttachWindowClose([this, pColorPicker, pEdit, oldTextColor, pLeftColorLabel](const ui::EventArgs& args) {
            ui::UiColor newColor = pColorPicker->GetSelectedColor();
            if ((args.wParam == ui::kWindowCloseOK) && !newColor.IsEmpty()) {
                // If it is "OK", set the text color of the RichEdit control
                pLeftColorLabel->SetBkColor(newColor);
                SetTextColor(pEdit->GetColorString(newColor));
            }
            else {
                // If it is "Cancel" or the window is closed, restore the original color
                pLeftColorLabel->SetBkColor(newColor);
                SetTextColor(oldTextColor);
            }
            return true;
            });
    }
}

void MainForm::UpdateFontStatus()
{
    ui::RichEdit* pRichEdit = GetRichEdit();
    if (pRichEdit == nullptr) {
        return;
    }
    const ui::UiFont fontInfo = pRichEdit->GetFontInfo();

    // Update the font name
    ui::Combo* pFontNameCombo = ui::Find<ui::Combo>(this, DUI_T("combo_font_name"));
    if (pFontNameCombo != nullptr) {
        pFontNameCombo->SelectTextItem(fontInfo.m_fontName.c_str(), false);
    }

    // Update the font size
    UpdateFontSizeStatus();

    // Update bold state
    ui::CheckBox* pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_font_bold"));
    if (pCheckBox != nullptr) {
        pCheckBox->SetSelected(fontInfo.m_bBold);
    }

    // Update italic state
    pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_font_italic"));
    if (pCheckBox != nullptr) {
        pCheckBox->SetSelected(fontInfo.m_bItalic);
    }

    // Update underline state
    pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_font_underline"));
    if (pCheckBox != nullptr) {
        pCheckBox->SetSelected(fontInfo.m_bUnderline);
    }

    // Update strikethrough state
    pCheckBox = ui::Find<ui::CheckBox>(this, DUI_T("btn_font_strikeout"));
    if (pCheckBox != nullptr) {
        pCheckBox->SetSelected(fontInfo.m_bStrikeOut);
    }
}

void MainForm::UpdateFontSizeStatus()
{
    ui::RichEdit* pRichEdit = GetRichEdit();
    if (pRichEdit == nullptr) {
        return;
    }
    ui::Combo* pFontSizeCombo = ui::Find<ui::Combo>(this, DUI_T("combo_font_size"));
    if (pFontSizeCombo == nullptr) {
        return;
    }
    const ui::UiFont fontInfo = pRichEdit->GetFontInfo();
    if (fontInfo.m_fontSize == 0) {
        // No valid font size information
        pFontSizeCombo->SetCurSel(ui::Box::InvalidIndex);
        return;
    }
   
    size_t maxItemIndex = 0;
    for (size_t nIndex = 0; nIndex < m_fontSizeList.size(); ++nIndex) {
        if (nIndex == (m_fontSizeList.size() - 1)) {
            break;
        }
        if (m_fontSizeList[nIndex].fFontSize > m_fontSizeList[nIndex + 1].fFontSize) {
            maxItemIndex = nIndex;
            break;
        }
    }

    bool bSelected = false;
    for (size_t nIndex = maxItemIndex; nIndex < m_fontSizeList.size(); ++nIndex) {
        // Prefer the font size for Chinese characters
        const ui::FontSizeInfo& fontSize = m_fontSizeList[nIndex];
        if (fontInfo.m_fontSize == (int32_t)std::roundf(fontSize.fDpiFontSize)) {
            if (pFontSizeCombo->SelectTextItem(fontSize.fontSizeName, false) != ui::Box::InvalidIndex) {
                bSelected = true;
            }
            break;
        }
    }
    if (!bSelected) {
        for (size_t nIndex = 0; nIndex <= maxItemIndex; ++nIndex) {
            // Select the font size of the numbers
            const ui::FontSizeInfo& fontSize = m_fontSizeList[nIndex];
            if ((int32_t)std::roundf(fontSize.fDpiFontSize) >= fontInfo.m_fontSize) {
                if (pFontSizeCombo->SelectTextItem(fontSize.fontSizeName, false) != ui::Box::InvalidIndex) {
                    bSelected = true;
                    break;
                }
            }
        }
    }
    if (!bSelected) {
        for (size_t nIndex = 0; nIndex <= maxItemIndex; ++nIndex) {
            // Select the font size of the numbers
            const ui::FontSizeInfo& fontSize = m_fontSizeList[nIndex];
            int32_t nFontSize = Dpi().GetScaleInt((int32_t)std::roundf(fontSize.fFontSize));
            if (nFontSize >= fontInfo.m_fontSize) {
                if (pFontSizeCombo->SelectTextItem(fontSize.fontSizeName, false) != ui::Box::InvalidIndex) {
                    bSelected = true;
                    break;
                }
            }
        }
    }
}

void MainForm::SetFontName(const DString& fontName)
{
    if (m_pRichEdit != nullptr) {
        ui::UiFont fontInfo = m_pRichEdit->GetFontInfo();
        fontInfo.m_fontName = fontName;
        m_pRichEdit->SetFontInfo(fontInfo);
    }
}

void MainForm::SetFontSize(const DString& fontSize)
{
    for (const ui::FontSizeInfo& fontSizeInfo : m_fontSizeList) {
        if (fontSize == fontSizeInfo.fontSizeName) {
            if (m_pRichEdit != nullptr) {
                ui::UiFont fontInfo = m_pRichEdit->GetFontInfo();
                fontInfo.m_fontSize = (int32_t)(std::roundf(fontSizeInfo.fDpiFontSize));
                m_pRichEdit->SetFontInfo(fontInfo);
            }
            break;
        }
    }
}

void MainForm::AdjustFontSize(bool bIncreaseFontSize)
{
    if (m_pRichEdit == nullptr) {
        return;
    }
    std::map<int32_t, int32_t> fontSizeMap;
    for (const ui::FontSizeInfo& fontSizeInfo : m_fontSizeList) {
        fontSizeMap[(int32_t)(std::roundf(fontSizeInfo.fDpiFontSize))] = (int32_t)(std::roundf(fontSizeInfo.fFontSize));
    }
    std::vector<int32_t> fontSizeList;
    for (auto fontSize : fontSizeMap) {
        fontSizeList.push_back(fontSize.second);
    }
    auto pos = std::unique(fontSizeList.begin(), fontSizeList.end());
    if (pos != fontSizeList.end()) {
        fontSizeList.erase(pos);
    }

    ui::UiFont fontInfo = m_pRichEdit->GetFontInfo();
    // Convert back to the original value
    auto iter = fontSizeMap.find(fontInfo.m_fontSize);
    if (iter != fontSizeMap.end()) {
        fontInfo.m_fontSize = iter->second;
    }
    else {
        bool bFound = false;
        if (bIncreaseFontSize) {
            for (iter = fontSizeMap.begin(); iter != fontSizeMap.end(); ++iter) {
                if (iter->first > fontInfo.m_fontSize) {
                    fontInfo.m_fontSize = iter->second;
                    bFound = true;
                    break;
                }
            }
        }
        else {
            auto pos = fontSizeMap.rbegin();
            for (; pos != fontSizeMap.rend(); ++pos) {
                if (pos->first < fontInfo.m_fontSize) {
                    fontInfo.m_fontSize = pos->second;
                    bFound = true;
                    break;
                }
            }
        }
        if (!bFound) {
            Dpi().UnscaleInt(fontInfo.m_fontSize);
        }
    }

    const size_t fontCount = fontSizeList.size();
    for (size_t index = 0; index < fontCount; ++index) {
        if (fontInfo.m_fontSize == fontSizeList[index]) {
            // Matched the current font size
            if (bIncreaseFontSize) {
                // Increase font
                if (index < (fontCount - 1)) {                  
                    fontInfo.m_fontSize = Dpi().GetScaleInt(fontSizeList[index + 1]);
                    if (m_pRichEdit->SetFontInfo(fontInfo)) {
                        UpdateFontSizeStatus();
                    }
                }
            }
            else {
                // Decrease font
                if (index > 0) {
                    fontInfo.m_fontSize = Dpi().GetScaleInt(fontSizeList[index - 1]);
                    if (m_pRichEdit->SetFontInfo(fontInfo)) {
                        UpdateFontSizeStatus();
                    }
                }
            }
            break;
        }
    }
}

void MainForm::SetFontBold(bool bBold)
{
    if (m_pRichEdit != nullptr) {
        ui::UiFont fontInfo = m_pRichEdit->GetFontInfo();
        fontInfo.m_bBold = bBold;
        m_pRichEdit->SetFontInfo(fontInfo);
    }
}

void MainForm::SetFontItalic(bool bItalic)
{
    if (m_pRichEdit != nullptr) {
        ui::UiFont fontInfo = m_pRichEdit->GetFontInfo();
        fontInfo.m_bItalic = bItalic;
        m_pRichEdit->SetFontInfo(fontInfo);
    }
}

void MainForm::SetFontUnderline(bool bUnderline)
{
    if (m_pRichEdit != nullptr) {
        ui::UiFont fontInfo = m_pRichEdit->GetFontInfo();
        fontInfo.m_bUnderline = bUnderline;
        m_pRichEdit->SetFontInfo(fontInfo);
    }
}

void MainForm::SetFontStrikeOut(bool bStrikeOut)
{
    if (m_pRichEdit != nullptr) {
        ui::UiFont fontInfo = m_pRichEdit->GetFontInfo();
        fontInfo.m_bStrikeOut = bStrikeOut;
        m_pRichEdit->SetFontInfo(fontInfo);
    }
}

void MainForm::SetTextColor(const DString& newColor)
{
    if (m_pRichEdit != nullptr) {
        if (m_pRichEdit->IsRichText()) {
            m_pRichEdit->SetSelectionTextColor(newColor);
        }
        else {
            m_pRichEdit->SetTextColor(newColor);
        }        
    }
}

void MainForm::OnCloseWindow()
{
    // After the window closes, exit the main thread's message loop and quit the program
    PostQuitMsg(0);
}

LRESULT MainForm::OnKeyDownMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnKeyDownMsg(vkCode, modifierKey, nativeMsg, bHandled);
    bool bControlDown = ui::Keyboard::IsKeyDown(ui::kVK_CONTROL);
    bool bShiftDown = ui::Keyboard::IsKeyDown(ui::kVK_SHIFT);
    if (bControlDown) {
        if ((vkCode == 'O') && !bShiftDown){
            // Open
            OnOpenFile();
        }
        else if (vkCode == 'S') {
            if (bShiftDown) {
                // Save As
                OnSaveAsFile();                
            }
            else {
                // Save
                OnSaveFile();
            }
        }
        if (!bShiftDown) {
            if (vkCode == 'F') {
                // Find
                OnFindText();
            }
            else if (vkCode == 'H') {
                // Replace
                OnReplaceText();
            }
        }
    }
    if (!bControlDown && !bShiftDown && (vkCode == ui::kVK_F3)) {
        // Find Next
        OnFindNext();
    }
    return lResult;
}

LRESULT MainForm::OnKeyUpMsg(ui::VirtualKeyCode vkCode, uint32_t modifierKey, const ui::NativeMsg& nativeMsg, bool& bHandled)
{
    LRESULT lResult = BaseClass::OnKeyUpMsg(vkCode, modifierKey, nativeMsg, bHandled);
    UpdateSaveStatus();
    return lResult;
}

void MainForm::UpdateSaveStatus()
{
    if (m_pRichEdit != nullptr) {
        ui::Button* pButton = ui::Find<ui::Button>(this, DUI_T("save_file"));
        if (m_pRichEdit->GetModify()) {
            if (pButton != nullptr) {
                pButton->SetText(m_saveBtnText + DUI_T("*"));
            }
        }
        else {
            if (pButton != nullptr) {
                pButton->SetText(m_saveBtnText);
            }
        }
    }
}

void MainForm::LoadRichEditData()
{
    std::string xml;
    ui::FilePath controls_xml = ui::GlobalManager::Instance().GetResourcePath();
    controls_xml += DUI_T("rich_edit/rich_edit.xml");

    // Try embedded resources first (code/gen versions)
    std::vector<unsigned char> fileData;
    if (ui::GlobalManager::Instance().MemoryResources().GetData(controls_xml, fileData)) {
        xml.assign(fileData.begin(), fileData.end());
    } else {
        // Fallback to filesystem (XML version)
        std::ifstream ifs(controls_xml.NativePath().c_str(), std::ios::binary);
        if (ifs.is_open()) {
            ifs.seekg(0, std::ios_base::end);
            std::streamoff length = ifs.tellg();
            ifs.seekg(0, std::ios_base::beg);
            xml.resize(static_cast<unsigned int>(length));
            ifs.read(&xml[0], length);
            ifs.close();
        }
    }
    DString xmlU = ui::StringConvert::UTF8ToT(xml);

    if (m_pRichEdit != nullptr) {
        m_pRichEdit->SetText(xmlU);
        m_pRichEdit->SetFocus();
        m_pRichEdit->HomeUp();
        m_pRichEdit->SetModify(false);
        m_filePath = controls_xml;
    }
}

void MainForm::OnFindText()
{
    if (m_pFindForm == nullptr) {
        m_pFindForm = new FindForm(this);
        ui::WindowCreateParam createParam;
        createParam.m_dwStyle = ui::kWS_POPUP;
        createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
        createParam.m_windowTitle = DUI_T("FindForm");
        createParam.m_bCenterWindow = true;
        m_pFindForm->CreateWnd(this, createParam);
        m_pFindForm->ShowWindow(ui::kSW_SHOW);
        m_pFindForm->AttachWindowCloseMsg([this](const ui::EventArgs& args) {
                m_pFindForm = nullptr;
                return true;
            });
    }
    else {
        if (m_pFindForm->IsWindowMinimized()) {
            m_pFindForm->ShowWindow(ui::kSW_RESTORE);
        }
        else {
            m_pFindForm->ShowWindow(ui::kSW_SHOW);
        }
    }
}

void MainForm::OnFindNext()
{
    m_findReplace.FindNext();
}

void MainForm::OnReplaceText()
{
    if (m_pReplaceForm == nullptr) {
        m_pReplaceForm = new ReplaceForm(this);
        ui::WindowCreateParam createParam;
        createParam.m_dwStyle = ui::kWS_POPUP;
        createParam.m_dwExStyle = ui::kWS_EX_LAYERED;
        createParam.m_windowTitle = DUI_T("ReplaceForm");
        createParam.m_bCenterWindow = true;
        m_pReplaceForm->CreateWnd(this, createParam);
        m_pReplaceForm->ShowWindow(ui::kSW_SHOW);
        m_pReplaceForm->AttachWindowCloseMsg([this](const ui::EventArgs& args) {
                m_pReplaceForm = nullptr;
                return true;
            });
    }
    else {
        if (m_pReplaceForm->IsWindowMinimized()) {
            m_pReplaceForm->ShowWindow(ui::kSW_RESTORE);
        }
        else {
            m_pReplaceForm->ShowWindow(ui::kSW_SHOW);
        }
    }
}

void MainForm::FindRichText(const DString& findText, bool bFindDown, bool bMatchCase, bool bMatchWholeWord, ui::Window* pWndDialog)
{
    m_findReplace.FindRichText(findText, bFindDown, bMatchCase, bMatchWholeWord, pWndDialog);
}

void MainForm::ReplaceRichText(const DString& findText, const DString& replaceText, bool bFindDown, bool bMatchCase, bool bMatchWholeWord, ui::Window* pWndDialog)
{
    if (m_findReplace.ReplaceRichText(findText, replaceText, bFindDown, bMatchCase, bMatchWholeWord, pWndDialog)) {
        if (m_pRichEdit != nullptr) {
            m_pRichEdit->SetModify(true);
            UpdateSaveStatus();
        }
    }
}

void MainForm::ReplaceAllRichText(const DString& findText, const DString& replaceText, bool bFindDown, bool bMatchCase, bool bMatchWholeWord, ui::Window* pWndDialog)
{
    if (m_findReplace.ReplaceAllRichText(findText, replaceText, bFindDown, bMatchCase, bMatchWholeWord, pWndDialog)) {
        if (m_pRichEdit != nullptr) {
            m_pRichEdit->SetModify(true);
            UpdateSaveStatus();
        }
    }
}

ui::RichEdit* MainForm::GetRichEdit() const
{
    return m_pRichEdit;
}

void MainForm::UpdateZoomValue()
{
    ui::RichEdit* pRichEdit = GetRichEdit();
    if (pRichEdit == nullptr) {
        return;
    }

    ui::Label* pZoomLabel = ui::Find<ui::Label>(this, DUI_T("lavel_zoom_value"));
    if (pZoomLabel != nullptr) {
        uint32_t nZoomPercent = pRichEdit->GetZoomPercent();
        DString strZoom = ui::StringUtil::Printf(DUI_T("%u%%"), nZoomPercent);
        pZoomLabel->SetText(strZoom);
    }
}

void MainForm::OnOpenFile()
{
    std::vector<ui::FileDialog::FileType> fileTypes;
    fileTypes.push_back({ DUI_T("All Files (*.*)"), DUI_T("*.*")});
    fileTypes.push_back({ DUI_T("Text Files (*.txt)"), DUI_T("*.txt") });
#if defined (DUI_BUILD_FOR_WIN)
    fileTypes.push_back({ DUI_T("RTF Files (*.rtf)"), DUI_T("*.rtf") });
#endif

    DString defaultExt;
    int32_t nFileTypeIndex = 1;
#if defined (DUI_BUILD_FOR_WIN)
    if ((m_pRichEdit != nullptr) && m_pRichEdit->IsRichText()) {
        nFileTypeIndex = 2;
    }
#endif
    DString fileName = m_filePath.GetFileName();

    ui::FilePath filePath;
    ui::FileDialog openFileDlg;
    if (openFileDlg.BrowseForFile(this, filePath, true, fileTypes, nFileTypeIndex, defaultExt, fileName)) {
        if (LoadFile(filePath)) {
            m_filePath = filePath;
            if (m_pRichEdit != nullptr) {
                m_pRichEdit->SetModify(false);
                UpdateSaveStatus();
            }
        }        
    }
}

void MainForm::OnSaveFile()
{
    if (m_pRichEdit != nullptr) {
        if (m_pRichEdit->GetModify()) {
            if (SaveFile(m_filePath)) {
                m_pRichEdit->SetModify(false);
                UpdateSaveStatus();
            }
        }
    }
}

void MainForm::OnSaveAsFile()
{
    std::vector<ui::FileDialog::FileType> fileTypes;
    fileTypes.push_back({ DUI_T("All Files (*.*)"), DUI_T("*.*") });
    fileTypes.push_back({ DUI_T("Text Files (*.txt)"), DUI_T("*.txt") });
#if defined (DUI_BUILD_FOR_WIN)
    fileTypes.push_back({ DUI_T("RTF Files (*.rtf)"), DUI_T("*.rtf") });
#endif

    DString defaultExt;
    int32_t nFileTypeIndex = 1;
#if defined (DUI_BUILD_FOR_WIN)
    if ((m_pRichEdit != nullptr) && m_pRichEdit->IsRichText()) {
        nFileTypeIndex = 2;
    }
#endif
    DString fileName = m_filePath.GetFileName();

    ui::FilePath filePath;
    ui::FileDialog openFileDlg;
    if (openFileDlg.BrowseForFile(this, filePath, false, fileTypes, nFileTypeIndex, defaultExt, fileName)) {
        if (SaveFile(filePath)) {
            m_filePath = filePath;
            if (m_pRichEdit != nullptr) {
                m_pRichEdit->SetModify(false);
                UpdateSaveStatus();
            }
        }        
    }
}

#if defined (DUI_BUILD_FOR_WIN)

bool MainForm::LoadFile(const ui::FilePath& filePath)
{
    if (m_pRichEdit == nullptr) {
        return false;
    }
    DString filePathLocal = filePath.NativePath();
    HANDLE hFile = ::CreateFile(filePathLocal.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    EDITSTREAM es;
    es.dwCookie = (DWORD_PTR)hFile;
    es.dwError = 0;
    es.pfnCallback = StreamReadCallback;
    UINT nFormat = SF_TEXT;
    if (m_pRichEdit->IsRichText()) {
        nFormat = IsRtfFile(filePathLocal) ? SF_RTF : SF_TEXT;
    }
    m_pRichEdit->StreamIn(nFormat, es);
    ::CloseHandle(hFile);
    return !(BOOL)es.dwError;
}

bool MainForm::SaveFile(const ui::FilePath& filePath)
{
    if (m_pRichEdit == nullptr) {
        return false;
    }
    DString filePathLocal = filePath.NativePath();
    HANDLE hFile = ::CreateFile(filePathLocal.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    EDITSTREAM es;
    es.dwCookie = (DWORD_PTR)hFile;
    es.dwError = 0;
    es.pfnCallback = StreamWriteCallback;
    UINT nFormat = SF_TEXT;
    if (m_pRichEdit->IsRichText()) {
        nFormat = IsRtfFile(filePathLocal) ? SF_RTF : SF_TEXT;
    }
    m_pRichEdit->StreamOut(nFormat, es);
    ::CloseHandle(hFile);
    return !(BOOL)es.dwError;
}

bool MainForm::IsRtfFile(const DString& filePath) const
{
    DString fileExt;
    size_t pos = filePath.find_last_of(DUI_T("."));
    if (pos != DString::npos) {
        fileExt = filePath.substr(pos);
        fileExt = ui::StringUtil::MakeLowerString(fileExt);
    }
    return fileExt == DUI_T(".rtf");
}

DWORD MainForm::StreamReadCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR* pcb)
{
    ASSERT(dwCookie != 0);
    ASSERT(pcb != nullptr);

    return !::ReadFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, nullptr);
}

DWORD MainForm::StreamWriteCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR* pcb)
{
    ASSERT(dwCookie != 0);
    ASSERT(pcb != nullptr);

    return !::WriteFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, nullptr);
}

int32_t MainForm::ConvertToFontHeight(int32_t fontSize) const
{
    if (m_pRichEdit != nullptr) {
        return m_pRichEdit->ConvertToFontHeight(fontSize);
    }
    return fontSize;
}

bool MainForm::GetRichEditLogFont(LOGFONTW& lf) const
{
    ui::RichEdit* pRichEdit = GetRichEdit();
    if (pRichEdit == nullptr) {
        return false;
    }

    CHARFORMAT2W cf = {};
    GetCharFormat(cf);

    if (cf.dwMask & CFM_SIZE) {
        HWND hWnd = NativeWnd()->GetHWND();
        HDC hDC = ::GetDC(hWnd);
        lf.lfHeight = -MulDiv(cf.yHeight, ::GetDeviceCaps(hDC, LOGPIXELSY), LY_PER_INCH);
        lf.lfWidth = 0;
        ::ReleaseDC(hWnd, hDC);
    }

    if (cf.dwMask & CFM_BOLD) {
        lf.lfWeight = (cf.dwEffects & CFE_BOLD) ? FW_BOLD : 0;
    }

    if (cf.dwMask & CFM_WEIGHT) {
        lf.lfWeight = cf.wWeight;
    }

    if (cf.dwMask & CFM_ITALIC) {
        lf.lfItalic = (cf.dwEffects & CFE_ITALIC) ? TRUE : FALSE;
    }

    if (cf.dwMask & CFM_UNDERLINE) {
        lf.lfUnderline = (cf.dwEffects & CFE_UNDERLINE) ? TRUE : FALSE;
    }

    if (cf.dwMask & CFM_STRIKEOUT) {
        lf.lfStrikeOut = (cf.dwEffects & CFE_STRIKEOUT) ? TRUE : FALSE;
    }

    if (cf.dwMask & CFM_CHARSET) {
        lf.lfCharSet = cf.bCharSet;
    }

    if (cf.dwMask & CFM_FACE) {
        lf.lfPitchAndFamily = cf.bPitchAndFamily;

        // Replace with the system font name
        DStringW fontName = cf.szFaceName;
        ui::StringUtil::StringCopy(lf.lfFaceName, fontName.c_str());
    }
    return true;
}

void MainForm::InitCharFormat(const LOGFONTW& lf, CHARFORMAT2W& charFormat) const
{
    // The font size needs conversion, otherwise the font size display is incorrect
    LONG lfHeight = ConvertToFontHeight(lf.lfHeight);

    charFormat.cbSize = sizeof(CHARFORMAT2W);
    charFormat.dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_STRIKEOUT;
    charFormat.yHeight = -lfHeight;
    if (lf.lfWeight >= FW_BOLD) {
        charFormat.dwEffects |= CFE_BOLD;
    }
    else {
        charFormat.dwEffects &= ~CFE_BOLD;
    }
    if (lf.lfItalic) {
        charFormat.dwEffects |= CFE_ITALIC;
    }
    else {
        charFormat.dwEffects &= ~CFE_ITALIC;
    }
    if (lf.lfUnderline) {
        charFormat.dwEffects |= CFE_UNDERLINE;
    }
    else {
        charFormat.dwEffects &= ~CFE_UNDERLINE;
    }
    if (lf.lfStrikeOut) {
        charFormat.dwEffects |= CFE_STRIKEOUT;
    }
    else {
        charFormat.dwEffects &= ~CFE_STRIKEOUT;
    }
    charFormat.bCharSet = lf.lfCharSet;
    charFormat.bPitchAndFamily = lf.lfPitchAndFamily;
    ui::StringUtil::StringCopy(charFormat.szFaceName, lf.lfFaceName);
}

void MainForm::OnSetFont()
{
    ui::RichEdit* pRichEdit = GetRichEdit();
    if (pRichEdit == nullptr) {
        return;
    }
    // Text color
    ui::UiColor textColor;
    if (pRichEdit->IsRichText()) {
        textColor = pRichEdit->GetUiColor(pRichEdit->GetSelectionTextColor());
    }
    else {
        textColor = pRichEdit->GetUiColor(pRichEdit->GetTextColor());
    }

    LOGFONTW logFont = {};
    GetRichEditLogFont(logFont);

    CHOOSEFONTW cf;
    WCHAR szStyleName[64];  // contains style name after return
    LOGFONTW lf;                // default LOGFONTW to store the info

    HWND hWndParent = NativeWnd()->GetHWND();
    LPLOGFONTW lplfInitial = &logFont;
    DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS;
    memset(&cf, 0, sizeof(cf));
    memset(&lf, 0, sizeof(lf));
    memset(&szStyleName, 0, sizeof(szStyleName));

    cf.lStructSize = sizeof(cf);
    cf.hwndOwner = hWndParent;
    cf.rgbColors = textColor.ToCOLORREF();
    cf.lpszStyle = (LPWSTR)&szStyleName;
    cf.Flags = dwFlags;

    if (lplfInitial != nullptr) {
        cf.lpLogFont = lplfInitial;
        cf.Flags |= CF_INITTOLOGFONTSTRUCT;
        lf = *lplfInitial;
    }
    else {
        cf.lpLogFont = &lf;
    }

    BOOL bRet = ::ChooseFontW(&cf);
    if (bRet) {
        memcpy_s(&lf, sizeof(lf), cf.lpLogFont, sizeof(lf));

        // Set the RichEdit font
        CHARFORMAT2W charFormat = {};
        GetCharFormat(charFormat);
        InitCharFormat(lf, charFormat);

        // Set the font color
        charFormat.dwMask |= CFM_COLOR;
        charFormat.crTextColor = cf.rgbColors;
        charFormat.dwEffects &= ~CFE_AUTOCOLOR;

        SetCharFormat(charFormat);

        // Update color
        ui::ComboButton* pColorComboBtn = ui::Find<ui::ComboButton>(this, DUI_T("color_combo_button"));
        if (pColorComboBtn != nullptr) {
            if (pColorComboBtn->GetLabelBottom() != nullptr) {
                ui::UiColor textColor;
                textColor.SetFromCOLORREF(charFormat.crTextColor);
                pColorComboBtn->GetLabelBottom()->SetBkColor(textColor);
            }
        }
    }

    // Update the state of the font buttons
    UpdateFontStatus();
}

void MainForm::GetCharFormat(CHARFORMAT2W& charFormat) const
{
    charFormat = {};
    charFormat.cbSize = sizeof(CHARFORMAT2W);
    ui::RichEdit* pRichEdit = GetRichEdit();
    ASSERT(pRichEdit != nullptr);
    if (pRichEdit != nullptr) {
        if (pRichEdit->IsRichText()) {
            pRichEdit->GetSelectionCharFormat(charFormat);
        }
        else {
            pRichEdit->GetDefaultCharFormat(charFormat);
        }
    }
}

void MainForm::SetCharFormat(CHARFORMAT2W& charFormat)
{
    ui::RichEdit* pRichEdit = GetRichEdit();
    ASSERT(pRichEdit != nullptr);
    if (pRichEdit != nullptr) {
        if (pRichEdit->IsRichText()) {
            pRichEdit->SetSelectionCharFormat(charFormat);
        }
        else {
            pRichEdit->SetDefaultCharFormat(charFormat);
        }
    }
}

#else //defined (DUI_BUILD_FOR_WIN)

bool MainForm::LoadFile(const ui::FilePath& filePath)
{
    // Open file
    if (m_pRichEdit == nullptr) {
        return false;
    }
    bool bRet = false;
    std::vector<uint8_t> fileData;
    ui::FileUtil::ReadFileData(filePath, fileData);
    if (!fileData.empty()) {
        DStringW text;
        if (ui::StringCharset::GetDataAsString((const char*)fileData.data(), (uint32_t)fileData.size(), text)) {
            m_pRichEdit->SetText(text);
            bRet = true;
        }
    }
    return bRet;
}

bool MainForm::SaveFile(const ui::FilePath& filePath)
{
    // Save
    if (m_pRichEdit == nullptr) {
        return false;
    }    
    DStringW text = m_pRichEdit->GetTextW();
    if (text.empty()) {
        return false;
    }

    bool bRet = false;
    ui::CharsetType charsetType = ui::CharsetType::UNKNOWN;
    std::vector<uint8_t> fileData;
    ui::FileUtil::ReadFileData(filePath, fileData);
    if (!fileData.empty()) {
        // Detect the original file's encoding type and keep it consistent
        charsetType = ui::StringCharset::GetDataCharsetByBOM((const char*)fileData.data(), (uint32_t)fileData.size());
        if (charsetType == ui::CharsetType::UNKNOWN) {
            charsetType = ui::StringCharset::GetDataCharset((const char*)fileData.data(), (uint32_t)fileData.size());
        }
        fileData.clear();
    }
    if (charsetType == ui::CharsetType::UTF16_LE) {
        bRet = ui::FileUtil::WriteFileData(filePath, text);
    }
    else if (charsetType == ui::CharsetType::ANSI) {
#ifdef DUI_BUILD_FOR_WIN
        DStringA textA = ui::StringConvert::UnicodeToMBCS(text);
        bRet = ui::FileUtil::WriteFileData(filePath, textA);
#else
        // Save as UTF-8
        DStringA textA = ui::StringConvert::WStringToUTF8(text);
        bRet = ui::FileUtil::WriteFileData(filePath, textA);
#endif        
    }
    else {
        // Save as UTF-8
        DStringA textA = ui::StringConvert::WStringToUTF8(text);
        bRet = ui::FileUtil::WriteFileData(filePath, textA);
    }
    return bRet;
}

#endif //defined (DUI_BUILD_FOR_WIN)

// Simplified pure-code UI built with ui::Create / ui::Attach.
static void BuildRichEditUI(ui::Window* pWindow)
{
    auto* p0 = ui::Create<ui::VBox>(pWindow, {{DUI_T("bkcolor"), DUI_T("bk_wnd_darkcolor")}});
    auto* p1 = ui::Attach<ui::HBox>(p0, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}, {DUI_T("bkcolor"), DUI_T("bk_wnd_lightcolor")}});
    auto* p2 = ui::Attach<ui::HBox>(p1, {{DUI_T("margin"), DUI_T("0,0,30,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    auto* p3 = ui::Attach<ui::Control>(p2, {{DUI_T("width"), DUI_T("18")}, {DUI_T("height"), DUI_T("18")}, {DUI_T("bkimage"), DUI_T("public/caption/logo.svg")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}});
    auto* p4 = ui::Attach<ui::Label>(p2, {{DUI_T("text"), DUI_T("RichEdit控件测试程序")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    auto* p5 = ui::Attach<ui::Control>(p1, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    auto* p6 = ui::Attach<ui::HBox>(p1, {{DUI_T("margin"), DUI_T("0,0,0,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("36")}});
    auto* p7 = ui::Attach<ui::Button>(p6, {{DUI_T("class"), DUI_T("btn_wnd_min_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("minbtn")}, {DUI_T("margin"), DUI_T("0,2,0,2")}, {DUI_T("tooltip_text"), DUI_T("最小化")}});
    auto* p8 = ui::Attach<ui::Box>(p6, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    auto* p9 = ui::Attach<ui::Button>(p8, {{DUI_T("class"), DUI_T("btn_wnd_max_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("name"), DUI_T("maxbtn")}, {DUI_T("tooltip_text"), DUI_T("最大化")}});
    auto* p10 = ui::Attach<ui::Button>(p8, {{DUI_T("class"), DUI_T("btn_wnd_restore_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("name"), DUI_T("restorebtn")}, {DUI_T("visible"), DUI_T("false")}, {DUI_T("tooltip_text"), DUI_T("还原")}});
    auto* p11 = ui::Attach<ui::Button>(p6, {{DUI_T("class"), DUI_T("btn_wnd_close_11")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("closebtn")}, {DUI_T("margin"), DUI_T("0,0,0,2")}, {DUI_T("tooltip_text"), DUI_T("关闭")}});
    auto* p12 = ui::Attach<ui::HBox>(p0, {});
    auto* p13 = ui::Attach<ui::VScrollBox>(p12, {{DUI_T("width"), DUI_T("360")}, {DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("minwidth"), DUI_T("60")}, {DUI_T("bkcolor"), DUI_T("SeaShell")}, {DUI_T("border_size"), DUI_T("1,1,0,1")}, {DUI_T("border_color"), DUI_T("blue")}});
    auto* p14 = ui::Attach<ui::GroupVBox>(p13, {{DUI_T("margin"), DUI_T("6,6,6,6")}, {DUI_T("text"), DUI_T("密码相关功能")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("corner_size"), DUI_T("4,4")}});
    auto* p15 = ui::Attach<ui::HBox>(p14, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,16,8,0")}});
    auto* p16 = ui::Attach<ui::Label>(p15, {{DUI_T("text"), DUI_T("密码输入:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p17 = ui::Attach<ui::RichEdit>(p15, {{DUI_T("class"), DUI_T("simple")}, {DUI_T("text"), DUI_T("This is password!")}, {DUI_T("password"), DUI_T("true")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p18 = ui::Attach<ui::HBox>(p14, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p19 = ui::Attach<ui::Label>(p18, {{DUI_T("text"), DUI_T("显示密码:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p20 = ui::Attach<ui::RichEdit>(p18, {{DUI_T("class"), DUI_T("simple")}, {DUI_T("text"), DUI_T("This is password!")}, {DUI_T("password"), DUI_T("true")}, {DUI_T("show_password"), DUI_T("true")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p21 = ui::Attach<ui::HBox>(p14, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p22 = ui::Attach<ui::Label>(p21, {{DUI_T("text"), DUI_T("闪现密码字符:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p23 = ui::Attach<ui::RichEdit>(p21, {{DUI_T("class"), DUI_T("simple")}, {DUI_T("text"), DUI_T("This is password!")}, {DUI_T("password"), DUI_T("true")}, {DUI_T("flash_password_char"), DUI_T("true")}, {DUI_T("show_password"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p24 = ui::Attach<ui::HBox>(p14, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,8")}});
    auto* p25 = ui::Attach<ui::Label>(p24, {{DUI_T("text"), DUI_T("密码字符为#:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p26 = ui::Attach<ui::RichEdit>(p24, {{DUI_T("class"), DUI_T("simple")}, {DUI_T("text"), DUI_T("This is password!")}, {DUI_T("password"), DUI_T("true")}, {DUI_T("password_char"), DUI_T("#")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p27 = ui::Attach<ui::GroupVBox>(p13, {{DUI_T("margin"), DUI_T("6,6,6,6")}, {DUI_T("text"), DUI_T("基本功能")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("corner_size"), DUI_T("4,4")}});
    auto* p28 = ui::Attach<ui::HBox>(p27, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,16,8,0")}});
    auto* p29 = ui::Attach<ui::Label>(p28, {{DUI_T("text"), DUI_T("单行编辑:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p30 = ui::Attach<ui::RichEdit>(p28, {{DUI_T("class"), DUI_T("simple simple_border")}, {DUI_T("text"), DUI_T("RichEdit: simple")}, {DUI_T("default_context_menu"), DUI_T("true")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("text_padding"), DUI_T("2,0,0,0")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p31 = ui::Attach<ui::HBox>(p27, {{DUI_T("height"), DUI_T("44")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p32 = ui::Attach<ui::Label>(p31, {{DUI_T("text"), DUI_T("单行编辑:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p33 = ui::Attach<ui::RichEdit>(p31, {{DUI_T("class"), DUI_T("simple simple_border")}, {DUI_T("text"), DUI_T("RichEdit: edit")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("text_padding"), DUI_T("4,4,4,4")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p34 = ui::Attach<ui::HBox>(p27, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p35 = ui::Attach<ui::Label>(p34, {{DUI_T("text"), DUI_T("提示模式:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p36 = ui::Attach<ui::RichEdit>(p34, {{DUI_T("class"), DUI_T("prompt simple simple_border_bottom")}, {DUI_T("prompttext"), DUI_T("在这里可以输入文字")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p37 = ui::Attach<ui::HBox>(p27, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p38 = ui::Attach<ui::Label>(p37, {{DUI_T("text"), DUI_T("数字模式:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p39 = ui::Attach<ui::RichEdit>(p37, {{DUI_T("class"), DUI_T("simple")}, {DUI_T("text"), DUI_T("0123456789")}, {DUI_T("number"), DUI_T("true")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p40 = ui::Attach<ui::HBox>(p27, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p41 = ui::Attach<ui::Label>(p40, {{DUI_T("text"), DUI_T("只读模式:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p42 = ui::Attach<ui::RichEdit>(p40, {{DUI_T("class"), DUI_T("simple")}, {DUI_T("text"), DUI_T("RichEdit: readonly")}, {DUI_T("readonly"), DUI_T("true")}, {DUI_T("no_caret_readonly"), DUI_T("true")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p43 = ui::Attach<ui::HBox>(p27, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p44 = ui::Attach<ui::Label>(p43, {{DUI_T("text"), DUI_T("禁用模式:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p45 = ui::Attach<ui::RichEdit>(p43, {{DUI_T("class"), DUI_T("simple")}, {DUI_T("enabled"), DUI_T("false")}, {DUI_T("text"), DUI_T("RichEdit: disabled")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p46 = ui::Attach<ui::HBox>(p27, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p47 = ui::Attach<ui::Label>(p46, {{DUI_T("text"), DUI_T("超级链接:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p48 = ui::Attach<ui::RichEdit>(p46, {{DUI_T("name"), DUI_T("test_url")}, {DUI_T("auto_detect_url"), DUI_T("true")}, {DUI_T("text"), DUI_T("访问网址：http://www.baidu.com")}, {DUI_T("readonly"), DUI_T("true")}, {DUI_T("no_caret_readonly"), DUI_T("true")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("text_padding"), DUI_T("2,0,0,0")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p49 = ui::Attach<ui::HBox>(p27, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p50 = ui::Attach<ui::Label>(p49, {{DUI_T("text"), DUI_T("文本颜色:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p51 = ui::Attach<ui::RichEdit>(p49, {{DUI_T("class"), DUI_T("simple")}, {DUI_T("text"), DUI_T("RichEdit: 文本颜色为红色")}, {DUI_T("normal_text_color"), DUI_T("red")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p52 = ui::Attach<ui::HBox>(p27, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p53 = ui::Attach<ui::Label>(p52, {{DUI_T("text"), DUI_T("光标颜色:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p54 = ui::Attach<ui::RichEdit>(p52, {{DUI_T("class"), DUI_T("simple")}, {DUI_T("text"), DUI_T("RichEdit: 光标颜色为红色")}, {DUI_T("caret_color"), DUI_T("red")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p55 = ui::Attach<ui::HBox>(p27, {{DUI_T("height"), DUI_T("48")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p56 = ui::Attach<ui::Label>(p55, {{DUI_T("text"), DUI_T("文本对齐:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p57 = ui::Attach<ui::RichEdit>(p55, {{DUI_T("class"), DUI_T("simple")}, {DUI_T("text"), DUI_T("text_align={left/hcenter/right,top/vcenter/bottom}")}, {DUI_T("text_align"), DUI_T("hcenter,vcenter")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p58 = ui::Attach<ui::HBox>(p27, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,8")}});
    auto* p59 = ui::Attach<ui::Label>(p58, {{DUI_T("text"), DUI_T("最大字符数为8:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p60 = ui::Attach<ui::RichEdit>(p58, {{DUI_T("class"), DUI_T("simple")}, {DUI_T("text"), DUI_T("最大字符数为8")}, {DUI_T("max_char"), DUI_T("8")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p61 = ui::Attach<ui::GroupVBox>(p13, {{DUI_T("margin"), DUI_T("6,6,6,6")}, {DUI_T("text"), DUI_T("常用功能")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("corner_size"), DUI_T("4,4")}});
    auto* p62 = ui::Attach<ui::HBox>(p61, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,16,8,0")}});
    auto* p63 = ui::Attach<ui::Label>(p62, {{DUI_T("text"), DUI_T("Spin功能:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p64 = ui::Attach<ui::RichEdit>(p62, {{DUI_T("class"), DUI_T("simple simple_border rich_edit_spin")}, {DUI_T("min_number"), DUI_T("-64")}, {DUI_T("max_number"), DUI_T("64")}, {DUI_T("limit_text"), DUI_T("3")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p65 = ui::Attach<ui::Label>(p62, {{DUI_T("text"), DUI_T("(数字范围: -64 - 64)")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p66 = ui::Attach<ui::HBox>(p61, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p67 = ui::Attach<ui::Label>(p66, {{DUI_T("text"), DUI_T("清除文本:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p68 = ui::Attach<ui::RichEdit>(p66, {{DUI_T("class"), DUI_T("simple simple_border rich_edit_clear_btn")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p69 = ui::Attach<ui::HBox>(p61, {{DUI_T("height"), DUI_T("36")}, {DUI_T("margin"), DUI_T("8,0,8,8")}});
    auto* p70 = ui::Attach<ui::Label>(p69, {{DUI_T("text"), DUI_T("显示密码:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p71 = ui::Attach<ui::RichEdit>(p69, {{DUI_T("class"), DUI_T("simple simple_border rich_edit_show_password_btn")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p72 = ui::Attach<ui::GroupVBox>(p13, {{DUI_T("margin"), DUI_T("6,6,6,6")}, {DUI_T("text"), DUI_T("滚动条相关")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("corner_size"), DUI_T("4,4")}});
    auto* p73 = ui::Attach<ui::HBox>(p72, {{DUI_T("height"), DUI_T("48")}, {DUI_T("margin"), DUI_T("8,16,8,0")}});
    auto* p74 = ui::Attach<ui::Label>(p73, {{DUI_T("text"), DUI_T("横向滚动条:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p75 = ui::Attach<ui::RichEdit>(p73, {{DUI_T("text"), DUI_T("横向测试0横向测试1横向测试2横向测试3横向测试4横向测试5横向测试6横向测试7横向测试8")}, {DUI_T("multi_line"), DUI_T("true")}, {DUI_T("word_wrap"), DUI_T("false")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p76 = ui::Attach<ui::HBox>(p72, {{DUI_T("height"), DUI_T("48")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p77 = ui::Attach<ui::Label>(p76, {{DUI_T("text"), DUI_T("纵向滚动条:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p78 = ui::Attach<ui::RichEdit>(p76, {{DUI_T("text"), DUI_T("纵向测试0纵向测试1纵向测试2纵向测试3纵向测试4纵向测试5纵向测试6纵向测试7纵向测试8纵向测试9纵向测试10纵向测试11纵向测试12纵向测试13纵向测试14")}, {DUI_T("multi_line"), DUI_T("true")}, {DUI_T("want_return"), DUI_T("true")}, {DUI_T("word_wrap"), DUI_T("true")}, {DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p79 = ui::Attach<ui::HBox>(p72, {{DUI_T("height"), DUI_T("48")}, {DUI_T("margin"), DUI_T("8,0,8,8")}});
    auto* p80 = ui::Attach<ui::Label>(p79, {{DUI_T("text"), DUI_T("同时横向与纵向滚动条:  ")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p81 = ui::Attach<ui::RichEdit>(p79, {{DUI_T("text"), DUI_T("纵向测试0纵向测试1纵向测试2纵向测试3纵向测试4纵向测试5纵向测试6纵向测试7纵向测试8纵向测试9纵向测试10纵向测试11纵向测试12纵向测试13纵向测试14")}, {DUI_T("multi_line"), DUI_T("true")}, {DUI_T("want_return"), DUI_T("true")}, {DUI_T("word_wrap"), DUI_T("false")}, {DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("margin"), DUI_T("4,6,4,4")}});
    auto* p82 = ui::Attach<ui::Split>(p12, {{DUI_T("bkcolor"), DUI_T("splitline_level1")}, {DUI_T("width"), DUI_T("2")}});
    auto* p83 = ui::Attach<ui::VBox>(p12, {});
    auto* p84 = ui::Attach<ui::VBox>(p83, {{DUI_T("bkcolor"), DUI_T("bk_wnd_darkcolor")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("minheight"), DUI_T("40")}});
    auto* p85 = ui::Attach<ui::HBox>(p84, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("0,2,2,0")}});
    auto* p86 = ui::Attach<ui::Label>(p85, {{DUI_T("text"), DUI_T("文件：")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p87 = ui::Attach<ui::Button>(p85, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("open_file")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("打开(Ctrl+O)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p88 = ui::Attach<ui::Button>(p85, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("save_file")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("保存(Ctrl+S)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p89 = ui::Attach<ui::Button>(p85, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("save_as_file")}, {DUI_T("width"), DUI_T("160")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("另存为(Ctrl+Shift+S)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p90 = ui::Attach<ui::HBox>(p84, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("0,2,2,0")}});
    auto* p91 = ui::Attach<ui::Label>(p90, {{DUI_T("text"), DUI_T("编辑：")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p92 = ui::Attach<ui::Button>(p90, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("btn_copy")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("复制(Ctrl+C)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p93 = ui::Attach<ui::Button>(p90, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("btn_cut")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("剪切(Ctrl+X)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p94 = ui::Attach<ui::Button>(p90, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("btn_paste")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("粘贴(Ctrl+V)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p95 = ui::Attach<ui::Button>(p90, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("btn_delete")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("删除(Del)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p96 = ui::Attach<ui::Button>(p90, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("btn_sel_all")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("全选(Ctrl+A)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,8,0")}});
    auto* p97 = ui::Attach<ui::Button>(p90, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("btn_sel_none")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("取消选择")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,8,0")}});
    auto* p98 = ui::Attach<ui::Button>(p90, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("btn_undo")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("撤销(Ctrl+Z)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p99 = ui::Attach<ui::Button>(p90, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("btn_redo")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("重做(Ctrl+Y)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p100 = ui::Attach<ui::HBox>(p84, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("0,2,2,0")}});
    auto* p101 = ui::Attach<ui::Label>(p100, {{DUI_T("text"), DUI_T("查找：")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p102 = ui::Attach<ui::Button>(p100, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("btn_find_text")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("查找(Ctrl+F)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p103 = ui::Attach<ui::Button>(p100, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("btn_find_next")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("查找下一个(F3)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p104 = ui::Attach<ui::Button>(p100, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("btn_replace_text")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("替换(Ctrl+H)")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,0")}});
    auto* p105 = ui::Attach<ui::HBox>(p84, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("0,2,2,0")}});
    auto* p106 = ui::Attach<ui::Label>(p105, {{DUI_T("text"), DUI_T("字体：")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p107 = ui::Attach<ui::Button>(p105, {{DUI_T("class"), DUI_T("btn_global_gray_80x30")}, {DUI_T("name"), DUI_T("set_font")}, {DUI_T("width"), DUI_T("100")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("text"), DUI_T("设置字体")}, {DUI_T("borderround"), DUI_T("2,2")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,4,2")}});
    auto* p108 = ui::Attach<ui::Combo>(p105, {{DUI_T("class"), DUI_T("combo")}, {DUI_T("name"), DUI_T("combo_font_name")}, {DUI_T("combo_type"), DUI_T("drop_down")}, {DUI_T("dropbox_size"), DUI_T("0,300")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("width"), DUI_T("160")}, {DUI_T("tooltiptext"), DUI_T("选择字体")}, {DUI_T("margin"), DUI_T("2,0,2,2")}});
    auto* p109 = ui::Attach<ui::Combo>(p105, {{DUI_T("class"), DUI_T("combo")}, {DUI_T("name"), DUI_T("combo_font_size")}, {DUI_T("combo_type"), DUI_T("drop_down")}, {DUI_T("dropbox_size"), DUI_T("0,300")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("width"), DUI_T("60")}, {DUI_T("margin"), DUI_T("2,0,2,0")}, {DUI_T("tooltiptext"), DUI_T("选择字体")}});
    auto* p110 = ui::Attach<ui::CheckBox>(p105, {{DUI_T("class"), DUI_T("checkbox_font_class")}, {DUI_T("name"), DUI_T("btn_font_bold")}, {DUI_T("width"), DUI_T("28")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("text"), DUI_T("B")}, {DUI_T("font"), DUI_T("btn_font_bold_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("2,1,1,3")}, {DUI_T("tooltiptext"), DUI_T("粗体：改为较粗的字体")}});
    auto* p111 = ui::Attach<ui::CheckBox>(p105, {{DUI_T("class"), DUI_T("checkbox_font_class")}, {DUI_T("name"), DUI_T("btn_font_italic")}, {DUI_T("width"), DUI_T("28")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("text"), DUI_T("I")}, {DUI_T("font"), DUI_T("btn_font_italic_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("1,1,1,3")}, {DUI_T("tooltiptext"), DUI_T("斜体：改为倾斜的字体")}});
    auto* p112 = ui::Attach<ui::CheckBox>(p105, {{DUI_T("class"), DUI_T("checkbox_font_class")}, {DUI_T("name"), DUI_T("btn_font_underline")}, {DUI_T("width"), DUI_T("28")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("text"), DUI_T("U")}, {DUI_T("font"), DUI_T("system_underline_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("1,1,1,3")}, {DUI_T("tooltiptext"), DUI_T("下划线：在文本下面画一条线")}});
    auto* p113 = ui::Attach<ui::CheckBox>(p105, {{DUI_T("class"), DUI_T("checkbox_font_class")}, {DUI_T("name"), DUI_T("btn_font_strikeout")}, {DUI_T("width"), DUI_T("28")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("text"), DUI_T("abc")}, {DUI_T("font"), DUI_T("system_strikeout_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("1,1,1,3")}, {DUI_T("tooltiptext"), DUI_T("删除线：在文本上画一条线")}});
    auto* p114 = ui::Attach<ui::Button>(p105, {{DUI_T("class"), DUI_T("btn_font_class")}, {DUI_T("name"), DUI_T("btn_font_size_increase")}, {DUI_T("width"), DUI_T("32")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("text"), DUI_T("A+")}, {DUI_T("font"), DUI_T("btn_font_bold_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("1,1,1,3")}, {DUI_T("tooltiptext"), DUI_T("增加字体的大小")}});
    auto* p115 = ui::Attach<ui::Button>(p105, {{DUI_T("class"), DUI_T("btn_font_class")}, {DUI_T("name"), DUI_T("btn_font_size_decrease")}, {DUI_T("width"), DUI_T("32")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("text"), DUI_T("A-")}, {DUI_T("font"), DUI_T("btn_font_bold_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("1,1,1,3")}, {DUI_T("tooltiptext"), DUI_T("减小字体的大小")}});
    auto* p116 = ui::Attach<ui::ComboButton>(p105, {{DUI_T("class"), DUI_T("combo_button")}, {DUI_T("name"), DUI_T("color_combo_button")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("width"), DUI_T("48")}, {DUI_T("dropbox_size"), DUI_T("240,300")}, {DUI_T("left_button_top_label_text"), DUI_T("A")}, {DUI_T("left_button_bottom_label_bkcolor"), DUI_T("skyblue")}});
    auto* p117 = ui::Attach<ui::Label>(p105, {{DUI_T("text"), DUI_T("(富文本模式为设置当前选择文本的格式)")}, {DUI_T("font"), DUI_T("system_12")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("2,0,2,0")}, {DUI_T("tooltiptext"), DUI_T("富文本模式为设置当前选择文本的格式;纯文本模式为设置所有文本的格式。")}});
    auto* p118 = ui::Attach<ui::HBox>(p84, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("0,2,2,0")}});
    auto* p119 = ui::Attach<ui::Label>(p118, {{DUI_T("text"), DUI_T("缩放：")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p120 = ui::Attach<ui::Label>(p118, {{DUI_T("text"), DUI_T("当前缩放比例：")}, {DUI_T("valign"), DUI_T("center")}});
    auto* p121 = ui::Attach<ui::Label>(p118, {{DUI_T("name"), DUI_T("lavel_zoom_value")}, {DUI_T("text"), DUI_T("100.0%")}, {DUI_T("font"), DUI_T("system_bold_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("0,0,12,0")}});
    auto* p122 = ui::Attach<ui::Button>(p118, {{DUI_T("class"), DUI_T("btn_font_class")}, {DUI_T("name"), DUI_T("btn_zoom_in")}, {DUI_T("width"), DUI_T("64")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("text"), DUI_T("放大+")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("1,1,1,3")}, {DUI_T("tooltiptext"), DUI_T("增加字体的大小")}});
    auto* p123 = ui::Attach<ui::Button>(p118, {{DUI_T("class"), DUI_T("btn_font_class")}, {DUI_T("name"), DUI_T("btn_zoom_out")}, {DUI_T("width"), DUI_T("64")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("text"), DUI_T("缩小-")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("1,1,1,3")}, {DUI_T("tooltiptext"), DUI_T("减小字体的大小")}});
    auto* p124 = ui::Attach<ui::Button>(p118, {{DUI_T("class"), DUI_T("btn_font_class")}, {DUI_T("name"), DUI_T("btn_zoom_off")}, {DUI_T("width"), DUI_T("120")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("text"), DUI_T("复原到100%")}, {DUI_T("font"), DUI_T("system_14")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("1,1,1,3")}, {DUI_T("tooltiptext"), DUI_T("字体大小复原到100%")}});
    auto* p125 = ui::Attach<ui::HBox>(p84, {{DUI_T("height"), DUI_T("auto")}, {DUI_T("margin"), DUI_T("0,2,2,2")}});
    auto* p126 = ui::Attach<ui::Label>(p125, {{DUI_T("text"), DUI_T("其他：")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,8,0")}});
    auto* p127 = ui::Attach<ui::CheckBox>(p125, {{DUI_T("class"), DUI_T("checkbox_1")}, {DUI_T("name"), DUI_T("btn_word_wrap")}, {DUI_T("text"), DUI_T("自动换行")}, {DUI_T("margin"), DUI_T("4,0,0,0")}, {DUI_T("selected"), DUI_T("true")}, {DUI_T("tooltiptext"), DUI_T("是否自动换行")}});
    auto* p128 = ui::Attach<ui::CheckBox>(p125, {{DUI_T("class"), DUI_T("checkbox_1")}, {DUI_T("name"), DUI_T("btn_rich_text")}, {DUI_T("text"), DUI_T("富文本模式")}, {DUI_T("margin"), DUI_T("12,0,0,0")}, {DUI_T("selected"), DUI_T("true")}, {DUI_T("tooltiptext"), DUI_T("是否支持富文本格式")}});
    auto* p129 = ui::Attach<ui::Label>(p125, {{DUI_T("text"), DUI_T("行间距：")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("12,0,0,0")}});
    auto* p130 = ui::Attach<ui::Option>(p125, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("richedit_row_spacing_group")}, {DUI_T("text"), DUI_T("1.0")}, {DUI_T("margin"), DUI_T("6,0,0,0")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("selected"), DUI_T("true")}});
    auto* p131 = ui::Attach<ui::Option>(p125, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("richedit_row_spacing_group")}, {DUI_T("text"), DUI_T("1.15")}, {DUI_T("margin"), DUI_T("6,0,0,0")}, {DUI_T("text_align"), DUI_T("vcenter")}});
    auto* p132 = ui::Attach<ui::Option>(p125, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("richedit_row_spacing_group")}, {DUI_T("text"), DUI_T("1.5")}, {DUI_T("margin"), DUI_T("6,0,0,0")}, {DUI_T("text_align"), DUI_T("vcenter")}});
    auto* p133 = ui::Attach<ui::Option>(p125, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("richedit_row_spacing_group")}, {DUI_T("text"), DUI_T("2.0")}, {DUI_T("margin"), DUI_T("6,0,0,0")}, {DUI_T("text_align"), DUI_T("vcenter")}});
    auto* p134 = ui::Attach<ui::Option>(p125, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("richedit_row_spacing_group")}, {DUI_T("text"), DUI_T("2.5")}, {DUI_T("margin"), DUI_T("6,0,0,0")}, {DUI_T("text_align"), DUI_T("vcenter")}});
    auto* p135 = ui::Attach<ui::Option>(p125, {{DUI_T("class"), DUI_T("option_1")}, {DUI_T("group"), DUI_T("richedit_row_spacing_group")}, {DUI_T("text"), DUI_T("3.0")}, {DUI_T("margin"), DUI_T("6,0,0,0")}, {DUI_T("text_align"), DUI_T("vcenter")}});
    auto* p136 = ui::Attach<ui::Label>(p125, {{DUI_T("name"), DUI_T("row_spacing_tips")}, {DUI_T("text"), DUI_T("（行间距：仅在富文本模式下有效）")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("4,0,0,0")}});
    auto* p137 = ui::Attach<ui::Split>(p83, {{DUI_T("bkcolor"), DUI_T("splitline_level1")}, {DUI_T("height"), DUI_T("2")}});
    auto* p138 = ui::Attach<ui::HBox>(p83, {{DUI_T("bkcolor"), DUI_T("Pink")}, {DUI_T("border_size"), DUI_T("1")}, {DUI_T("border_color"), DUI_T("blue")}, {DUI_T("minwidth"), DUI_T("60")}});
    auto* p139 = ui::Attach<ui::RichEdit>(p138, {{DUI_T("name"), DUI_T("rich_edit")}, {DUI_T("enable_drag_drop"), DUI_T("true")}, {DUI_T("zoom"), DUI_T("0,0")}, {DUI_T("wheel_zoom"), DUI_T("true")}, {DUI_T("multi_line"), DUI_T("true")}, {DUI_T("want_return"), DUI_T("true")}, {DUI_T("word_wrap"), DUI_T("true")}, {DUI_T("rich_text"), DUI_T("false")}, {DUI_T("default_context_menu"), DUI_T("true")}, {DUI_T("save_selection"), DUI_T("true")}, {DUI_T("hide_selection"), DUI_T("false")}, {DUI_T("word_wrap"), DUI_T("true")}, {DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("auto_vscroll"), DUI_T("true")}, {DUI_T("bkcolor"), DUI_T("white")}});

    ui::Attach(pWindow, p0);
}
