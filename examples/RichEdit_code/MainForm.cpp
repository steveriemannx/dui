#include "MainForm.h"
#include "FindForm.h"
#include "ReplaceForm.h"
#include "dui/Utils/UiBuilder.h"
#include <fstream>

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
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
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildRichEditUI(this);

    m_pRichEdit = ui::Find<ui::RichEdit>(this, "rich_edit");
    ASSERT(m_pRichEdit != nullptr);
    m_findReplace.SetRichEdit(m_pRichEdit);
    LoadRichEditData();

    // Initialize font information
    ui::Combo* pFontNameCombo = ui::Find<ui::Combo>(this, "combo_font_name");
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
    ui::Combo* pFontSizeCombo = ui::Find<ui::Combo>(this, "combo_font_size");
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
    ui::ComboButton* pColorComboBtn = ui::Find<ui::ComboButton>(this, "color_combo_button");
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
    ui::CheckBox* pCheckBox = ui::Find<ui::CheckBox>(this, "btn_word_wrap");
    if ((pCheckBox != nullptr) && (m_pRichEdit != nullptr)) {
        pCheckBox->SetSelected(m_pRichEdit->IsWordWrap());
    }

    // Whether rich text format is supported
    pCheckBox = ui::Find<ui::CheckBox>(this, "btn_rich_text");
    if ((pCheckBox != nullptr) && (m_pRichEdit != nullptr)) {
        pCheckBox->SetSelected(m_pRichEdit->IsRichText());
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
#else
        // Rich text format is not supported in the SDL implementation
        pCheckBox->SetEnabled(false);
#endif
    }

#ifdef DUI_BUILD_FOR_SDL
    ui::Control* pRowSpacingTips = ui::Find<ui::Control>(this, "row_spacing_tips");
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
    ui::RichEdit* pTestUrl = ui::Find<ui::RichEdit>(this, "test_url");
    if (pTestUrl != nullptr) {
        pTestUrl->AttachLinkClick([this, pTestUrl](const ui::EventArgs& args) {
                if (args.GetSender() == pTestUrl) {
                    const DString::value_type* pUrl = (const DString::value_type*)args.wParam;
                    if (pUrl != nullptr) {
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
                        ::ShellExecuteW(NativeWnd()->GetHWND(), L"open", ui::StringConvert::TToWString(pUrl).c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#endif
                    }
                }
                return true;
            });
    }

    // File operations: open, save, save as
    ui::Button* pButton = ui::Find<ui::Button>(this, "open_file");
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    this->OnOpenFile();
                }
                return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, "save_file");
    if (pButton != nullptr) {
        m_saveBtnText = pButton->GetText();
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    this->OnSaveFile();
                }
                return true;
            });
    }
    pButton = ui::Find<ui::Button>(this, "save_as_file");
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    this->OnSaveAsFile();
                }
                return true;
            });
    }

    // Edit operations
    pButton = ui::Find<ui::Button>(this, "btn_copy");
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
    pButton = ui::Find<ui::Button>(this, "btn_cut");
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
    pButton = ui::Find<ui::Button>(this, "btn_paste");
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
    pButton = ui::Find<ui::Button>(this, "btn_delete");
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
    pButton = ui::Find<ui::Button>(this, "btn_sel_all");
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
    pButton = ui::Find<ui::Button>(this, "btn_sel_none");
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
    pButton = ui::Find<ui::Button>(this, "btn_undo");
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
    pButton = ui::Find<ui::Button>(this, "btn_redo");
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
    pButton = ui::Find<ui::Button>(this, "btn_find_text");
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                OnFindText();
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
    pButton = ui::Find<ui::Button>(this, "btn_replace_text");
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                OnReplaceText();
            }
            return true;
            });
    }

    // Set font
    pButton = ui::Find<ui::Button>(this, "set_font");
    if (pButton != nullptr) {
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
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
    ui::Combo* pFontNameCombo = ui::Find<ui::Combo>(this, "combo_font_name");
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
    ui::Combo* pFontSizeCombo = ui::Find<ui::Combo>(this, "combo_font_size");
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
    ui::CheckBox* pCheckBox = ui::Find<ui::CheckBox>(this, "btn_font_bold");
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
    pCheckBox = ui::Find<ui::CheckBox>(this, "btn_font_italic");
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
    pCheckBox = ui::Find<ui::CheckBox>(this, "btn_font_underline");
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
    pCheckBox = ui::Find<ui::CheckBox>(this, "btn_font_strikeout");
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
    ui::Button* pFontButton = ui::Find<ui::Button>(this, "btn_font_size_increase");
    if (pFontButton != nullptr) {
        pFontButton->AttachClick([this](const ui::EventArgs& args) {
            AdjustFontSize(true);
            return true;
            });
    }
    pFontButton = ui::Find<ui::Button>(this, "btn_font_size_decrease");
    if (pFontButton != nullptr) {
        pFontButton->AttachClick([this](const ui::EventArgs& args) {
            AdjustFontSize(false);
            return true;
            });
    }

    // Color: left button click
    ui::ComboButton* pColorComboBtn = ui::Find<ui::ComboButton>(this, "color_combo_button");
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
    ui::Button* pZoomButtom = ui::Find<ui::Button>(this, "btn_zoom_in");
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
    pZoomButtom = ui::Find<ui::Button>(this, "btn_zoom_out");
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
    pZoomButtom = ui::Find<ui::Button>(this, "btn_zoom_off");
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
    pCheckBox = ui::Find<ui::CheckBox>(this, "btn_word_wrap");
    if ((pCheckBox != nullptr) && (m_pRichEdit != nullptr)) {
        pCheckBox->AttachSelect([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                m_pRichEdit->SetWordWrap(true);
                m_pRichEdit->SetAttribute("hscrollbar", "false");
            }
            return true;
            });
        pCheckBox->AttachUnSelect([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                m_pRichEdit->SetWordWrap(false);
                m_pRichEdit->SetAttribute("hscrollbar", "true");
            }
            return true;
            });
    }

    // Rich text format
    pCheckBox = ui::Find<ui::CheckBox>(this, "btn_rich_text");
    if ((pCheckBox != nullptr) && (m_pRichEdit != nullptr)) {
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
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
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
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
    ui::ComboButton* pColorComboBtn = ui::Find<ui::ComboButton>(this, "color_combo_button");
    if (pColorComboBtn == nullptr) {
        return;
    }
    ui::UiSize boxSize = pColorComboBtn->GetDropBoxSize();
    ui::Box* pComboBox = pColorComboBtn->GetComboBox();
    if (pComboBox == nullptr) {
        return;
    }
    pComboBox->SetWindow(this);
    ui::GlobalManager::Instance().FillBoxWithCache(pComboBox, ui::FilePath("rich_edit/color_combox.xml"));
    ui::ColorPickerRegular* pColorPicker = dynamic_cast<ui::ColorPickerRegular*>(pComboBox->FindSubControl("color_combo_picker"));
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

    ui::Button* pMoreColorButton = dynamic_cast<ui::Button*>(pComboBox->FindSubControl("color_combo_picker_more"));
    if (pMoreColorButton != nullptr) {
        pMoreColorButton->AttachClick([this](const ui::EventArgs& args) {
            ShowColorPicker();
            return true;
            });
    }
}

void MainForm::ShowColorPicker()
{
    ui::ComboButton* pColorComboBtn = ui::Find<ui::ComboButton>(this, "color_combo_button");
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
    createParam.m_windowTitle = "ColorPicker";
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
    ui::Combo* pFontNameCombo = ui::Find<ui::Combo>(this, "combo_font_name");
    if (pFontNameCombo != nullptr) {
        pFontNameCombo->SelectTextItem(fontInfo.m_fontName.c_str(), false);
    }

    // Update the font size
    UpdateFontSizeStatus();

    // Update bold state
    ui::CheckBox* pCheckBox = ui::Find<ui::CheckBox>(this, "btn_font_bold");
    if (pCheckBox != nullptr) {
        pCheckBox->SetSelected(fontInfo.m_bBold);
    }

    // Update italic state
    pCheckBox = ui::Find<ui::CheckBox>(this, "btn_font_italic");
    if (pCheckBox != nullptr) {
        pCheckBox->SetSelected(fontInfo.m_bItalic);
    }

    // Update underline state
    pCheckBox = ui::Find<ui::CheckBox>(this, "btn_font_underline");
    if (pCheckBox != nullptr) {
        pCheckBox->SetSelected(fontInfo.m_bUnderline);
    }

    // Update strikethrough state
    pCheckBox = ui::Find<ui::CheckBox>(this, "btn_font_strikeout");
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
    ui::Combo* pFontSizeCombo = ui::Find<ui::Combo>(this, "combo_font_size");
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
        ui::Button* pButton = ui::Find<ui::Button>(this, "save_file");
        if (m_pRichEdit->GetModify()) {
            if (pButton != nullptr) {
                pButton->SetText(m_saveBtnText + "*");
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
        createParam.m_windowTitle = "FindForm";
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
        createParam.m_windowTitle = "ReplaceForm";
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

    ui::Label* pZoomLabel = ui::Find<ui::Label>(this, "lavel_zoom_value");
    if (pZoomLabel != nullptr) {
        uint32_t nZoomPercent = pRichEdit->GetZoomPercent();
        DString strZoom = ui::StringUtil::Printf("%u%%", nZoomPercent);
        pZoomLabel->SetText(strZoom);
    }
}

void MainForm::OnOpenFile()
{
    std::vector<ui::FileDialog::FileType> fileTypes;
    fileTypes.push_back({ "All Files (*.*)", "*.*"});
    fileTypes.push_back({ "Text Files (*.txt)", "*.txt" });
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
    fileTypes.push_back({ "RTF Files (*.rtf)", "*.rtf" });
#endif

    DString defaultExt;
    int32_t nFileTypeIndex = 1;
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
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
    fileTypes.push_back({ "All Files (*.*)", "*.*" });
    fileTypes.push_back({ "Text Files (*.txt)", "*.txt" });
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
    fileTypes.push_back({ "RTF Files (*.rtf)", "*.rtf" });
#endif

    DString defaultExt;
    int32_t nFileTypeIndex = 1;
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
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

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

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
    size_t pos = filePath.find_last_of(".");
    if (pos != DString::npos) {
        fileExt = filePath.substr(pos);
        fileExt = ui::StringUtil::MakeLowerString(fileExt);
    }
    return fileExt == ".rtf";
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
        ui::ComboButton* pColorComboBtn = ui::Find<ui::ComboButton>(this, "color_combo_button");
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

#else //defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

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

#endif //defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)

// Simplified pure-code UI built with ui::Create / ui::Attach.
static void BuildRichEditUI(ui::Window* pWindow)
{
    auto* p0 = ui::Create<ui::VBox>(pWindow, {{"bkcolor", "bk_wnd_darkcolor"}});
    auto* p1 = ui::Attach<ui::HBox>(p0, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}, {"bkcolor", "bk_wnd_lightcolor"}});
    auto* p2 = ui::Attach<ui::HBox>(p1, {{"margin", "0,0,30,0"}, {"valign", "center"}, {"width", "auto"}, {"height", "auto"}, {"mouse_enabled", "false"}});
    auto* p3 = ui::Attach<ui::Control>(p2, {{"width", "18"}, {"height", "18"}, {"bkimage", "public/caption/logo.svg"}, {"valign", "center"}, {"margin", "8,0,0,0"}});
    auto* p4 = ui::Attach<ui::Label>(p2, {{"text", "RichEdit控件测试程序"}, {"valign", "center"}, {"margin", "8,0,0,0"}, {"mouse_enabled", "false"}});
    auto* p5 = ui::Attach<ui::Control>(p1, {{"mouse_enabled", "false"}});
    auto* p6 = ui::Attach<ui::HBox>(p1, {{"margin", "0,0,0,0"}, {"valign", "center"}, {"width", "auto"}, {"height", "36"}});
    auto* p7 = ui::Attach<ui::Button>(p6, {{"class", "btn_wnd_min_11"}, {"height", "32"}, {"width", "40"}, {"name", "minbtn"}, {"margin", "0,2,0,2"}, {"tooltip_text", "最小化"}});
    auto* p8 = ui::Attach<ui::Box>(p6, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    auto* p9 = ui::Attach<ui::Button>(p8, {{"class", "btn_wnd_max_11"}, {"height", "32"}, {"width", "stretch"}, {"name", "maxbtn"}, {"tooltip_text", "最大化"}});
    auto* p10 = ui::Attach<ui::Button>(p8, {{"class", "btn_wnd_restore_11"}, {"height", "32"}, {"width", "stretch"}, {"name", "restorebtn"}, {"visible", "false"}, {"tooltip_text", "还原"}});
    auto* p11 = ui::Attach<ui::Button>(p6, {{"class", "btn_wnd_close_11"}, {"height", "stretch"}, {"width", "40"}, {"name", "closebtn"}, {"margin", "0,0,0,2"}, {"tooltip_text", "关闭"}});
    auto* p12 = ui::Attach<ui::HBox>(p0, {});
    auto* p13 = ui::Attach<ui::VScrollBox>(p12, {{"width", "360"}, {"vscrollbar", "true"}, {"minwidth", "60"}, {"bkcolor", "SeaShell"}, {"border_size", "1,1,0,1"}, {"border_color", "blue"}});
    auto* p14 = ui::Attach<ui::GroupVBox>(p13, {{"margin", "6,6,6,6"}, {"text", "密码相关功能"}, {"height", "auto"}, {"corner_size", "4,4"}});
    auto* p15 = ui::Attach<ui::HBox>(p14, {{"height", "36"}, {"margin", "8,16,8,0"}});
    auto* p16 = ui::Attach<ui::Label>(p15, {{"text", "密码输入:  "}, {"valign", "center"}});
    auto* p17 = ui::Attach<ui::RichEdit>(p15, {{"class", "simple"}, {"text", "This is password!"}, {"password", "true"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p18 = ui::Attach<ui::HBox>(p14, {{"height", "36"}, {"margin", "8,0,8,0"}});
    auto* p19 = ui::Attach<ui::Label>(p18, {{"text", "显示密码:  "}, {"valign", "center"}});
    auto* p20 = ui::Attach<ui::RichEdit>(p18, {{"class", "simple"}, {"text", "This is password!"}, {"password", "true"}, {"show_password", "true"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p21 = ui::Attach<ui::HBox>(p14, {{"height", "36"}, {"margin", "8,0,8,0"}});
    auto* p22 = ui::Attach<ui::Label>(p21, {{"text", "闪现密码字符:  "}, {"valign", "center"}});
    auto* p23 = ui::Attach<ui::RichEdit>(p21, {{"class", "simple"}, {"text", "This is password!"}, {"password", "true"}, {"flash_password_char", "true"}, {"show_password", "false"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p24 = ui::Attach<ui::HBox>(p14, {{"height", "36"}, {"margin", "8,0,8,8"}});
    auto* p25 = ui::Attach<ui::Label>(p24, {{"text", "密码字符为#:  "}, {"valign", "center"}});
    auto* p26 = ui::Attach<ui::RichEdit>(p24, {{"class", "simple"}, {"text", "This is password!"}, {"password", "true"}, {"password_char", "#"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p27 = ui::Attach<ui::GroupVBox>(p13, {{"margin", "6,6,6,6"}, {"text", "基本功能"}, {"height", "auto"}, {"corner_size", "4,4"}});
    auto* p28 = ui::Attach<ui::HBox>(p27, {{"height", "36"}, {"margin", "8,16,8,0"}});
    auto* p29 = ui::Attach<ui::Label>(p28, {{"text", "单行编辑:  "}, {"valign", "center"}});
    auto* p30 = ui::Attach<ui::RichEdit>(p28, {{"class", "simple simple_border"}, {"text", "RichEdit: simple"}, {"default_context_menu", "true"}, {"valign", "center"}, {"text_align", "vcenter"}, {"text_padding", "2,0,0,0"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p31 = ui::Attach<ui::HBox>(p27, {{"height", "44"}, {"margin", "8,0,8,0"}});
    auto* p32 = ui::Attach<ui::Label>(p31, {{"text", "单行编辑:  "}, {"valign", "center"}});
    auto* p33 = ui::Attach<ui::RichEdit>(p31, {{"class", "simple simple_border"}, {"text", "RichEdit: edit"}, {"valign", "center"}, {"text_align", "vcenter"}, {"text_padding", "4,4,4,4"}, {"margin", "4,6,4,4"}});
    auto* p34 = ui::Attach<ui::HBox>(p27, {{"height", "36"}, {"margin", "8,0,8,0"}});
    auto* p35 = ui::Attach<ui::Label>(p34, {{"text", "提示模式:  "}, {"valign", "center"}});
    auto* p36 = ui::Attach<ui::RichEdit>(p34, {{"class", "prompt simple simple_border_bottom"}, {"prompttext", "在这里可以输入文字"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p37 = ui::Attach<ui::HBox>(p27, {{"height", "36"}, {"margin", "8,0,8,0"}});
    auto* p38 = ui::Attach<ui::Label>(p37, {{"text", "数字模式:  "}, {"valign", "center"}});
    auto* p39 = ui::Attach<ui::RichEdit>(p37, {{"class", "simple"}, {"text", "0123456789"}, {"number", "true"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p40 = ui::Attach<ui::HBox>(p27, {{"height", "36"}, {"margin", "8,0,8,0"}});
    auto* p41 = ui::Attach<ui::Label>(p40, {{"text", "只读模式:  "}, {"valign", "center"}});
    auto* p42 = ui::Attach<ui::RichEdit>(p40, {{"class", "simple"}, {"text", "RichEdit: readonly"}, {"readonly", "true"}, {"no_caret_readonly", "true"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p43 = ui::Attach<ui::HBox>(p27, {{"height", "36"}, {"margin", "8,0,8,0"}});
    auto* p44 = ui::Attach<ui::Label>(p43, {{"text", "禁用模式:  "}, {"valign", "center"}});
    auto* p45 = ui::Attach<ui::RichEdit>(p43, {{"class", "simple"}, {"enabled", "false"}, {"text", "RichEdit: disabled"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p46 = ui::Attach<ui::HBox>(p27, {{"height", "36"}, {"margin", "8,0,8,0"}});
    auto* p47 = ui::Attach<ui::Label>(p46, {{"text", "超级链接:  "}, {"valign", "center"}});
    auto* p48 = ui::Attach<ui::RichEdit>(p46, {{"name", "test_url"}, {"auto_detect_url", "true"}, {"text", "访问网址：http://www.baidu.com"}, {"readonly", "true"}, {"no_caret_readonly", "true"}, {"valign", "center"}, {"text_align", "vcenter"}, {"text_padding", "2,0,0,0"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p49 = ui::Attach<ui::HBox>(p27, {{"height", "36"}, {"margin", "8,0,8,0"}});
    auto* p50 = ui::Attach<ui::Label>(p49, {{"text", "文本颜色:  "}, {"valign", "center"}});
    auto* p51 = ui::Attach<ui::RichEdit>(p49, {{"class", "simple"}, {"text", "RichEdit: 文本颜色为红色"}, {"normal_text_color", "red"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p52 = ui::Attach<ui::HBox>(p27, {{"height", "36"}, {"margin", "8,0,8,0"}});
    auto* p53 = ui::Attach<ui::Label>(p52, {{"text", "光标颜色:  "}, {"valign", "center"}});
    auto* p54 = ui::Attach<ui::RichEdit>(p52, {{"class", "simple"}, {"text", "RichEdit: 光标颜色为红色"}, {"caret_color", "red"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p55 = ui::Attach<ui::HBox>(p27, {{"height", "48"}, {"margin", "8,0,8,0"}});
    auto* p56 = ui::Attach<ui::Label>(p55, {{"text", "文本对齐:  "}, {"valign", "center"}});
    auto* p57 = ui::Attach<ui::RichEdit>(p55, {{"class", "simple"}, {"text", "text_align={left/hcenter/right,top/vcenter/bottom}"}, {"text_align", "hcenter,vcenter"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p58 = ui::Attach<ui::HBox>(p27, {{"height", "36"}, {"margin", "8,0,8,8"}});
    auto* p59 = ui::Attach<ui::Label>(p58, {{"text", "最大字符数为8:  "}, {"valign", "center"}});
    auto* p60 = ui::Attach<ui::RichEdit>(p58, {{"class", "simple"}, {"text", "最大字符数为8"}, {"max_char", "8"}, {"valign", "center"}, {"text_align", "vcenter"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p61 = ui::Attach<ui::GroupVBox>(p13, {{"margin", "6,6,6,6"}, {"text", "常用功能"}, {"height", "auto"}, {"corner_size", "4,4"}});
    auto* p62 = ui::Attach<ui::HBox>(p61, {{"height", "36"}, {"margin", "8,16,8,0"}});
    auto* p63 = ui::Attach<ui::Label>(p62, {{"text", "Spin功能:  "}, {"valign", "center"}});
    auto* p64 = ui::Attach<ui::RichEdit>(p62, {{"class", "simple simple_border rich_edit_spin"}, {"min_number", "-64"}, {"max_number", "64"}, {"limit_text", "3"}, {"margin", "4,6,4,4"}});
    auto* p65 = ui::Attach<ui::Label>(p62, {{"text", "(数字范围: -64 - 64)"}, {"valign", "center"}});
    auto* p66 = ui::Attach<ui::HBox>(p61, {{"height", "36"}, {"margin", "8,0,8,0"}});
    auto* p67 = ui::Attach<ui::Label>(p66, {{"text", "清除文本:  "}, {"valign", "center"}});
    auto* p68 = ui::Attach<ui::RichEdit>(p66, {{"class", "simple simple_border rich_edit_clear_btn"}, {"margin", "4,6,4,4"}});
    auto* p69 = ui::Attach<ui::HBox>(p61, {{"height", "36"}, {"margin", "8,0,8,8"}});
    auto* p70 = ui::Attach<ui::Label>(p69, {{"text", "显示密码:  "}, {"valign", "center"}});
    auto* p71 = ui::Attach<ui::RichEdit>(p69, {{"class", "simple simple_border rich_edit_show_password_btn"}, {"margin", "4,6,4,4"}});
    auto* p72 = ui::Attach<ui::GroupVBox>(p13, {{"margin", "6,6,6,6"}, {"text", "滚动条相关"}, {"height", "auto"}, {"corner_size", "4,4"}});
    auto* p73 = ui::Attach<ui::HBox>(p72, {{"height", "48"}, {"margin", "8,16,8,0"}});
    auto* p74 = ui::Attach<ui::Label>(p73, {{"text", "横向滚动条:  "}, {"valign", "center"}});
    auto* p75 = ui::Attach<ui::RichEdit>(p73, {{"text", "横向测试0横向测试1横向测试2横向测试3横向测试4横向测试5横向测试6横向测试7横向测试8"}, {"multi_line", "true"}, {"word_wrap", "false"}, {"hscrollbar", "true"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p76 = ui::Attach<ui::HBox>(p72, {{"height", "48"}, {"margin", "8,0,8,0"}});
    auto* p77 = ui::Attach<ui::Label>(p76, {{"text", "纵向滚动条:  "}, {"valign", "center"}});
    auto* p78 = ui::Attach<ui::RichEdit>(p76, {{"text", "纵向测试0纵向测试1纵向测试2纵向测试3纵向测试4纵向测试5纵向测试6纵向测试7纵向测试8纵向测试9纵向测试10纵向测试11纵向测试12纵向测试13纵向测试14"}, {"multi_line", "true"}, {"want_return", "true"}, {"word_wrap", "true"}, {"vscrollbar", "true"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p79 = ui::Attach<ui::HBox>(p72, {{"height", "48"}, {"margin", "8,0,8,8"}});
    auto* p80 = ui::Attach<ui::Label>(p79, {{"text", "同时横向与纵向滚动条:  "}, {"valign", "center"}});
    auto* p81 = ui::Attach<ui::RichEdit>(p79, {{"text", "纵向测试0纵向测试1纵向测试2纵向测试3纵向测试4纵向测试5纵向测试6纵向测试7纵向测试8纵向测试9纵向测试10纵向测试11纵向测试12纵向测试13纵向测试14"}, {"multi_line", "true"}, {"want_return", "true"}, {"word_wrap", "false"}, {"vscrollbar", "true"}, {"hscrollbar", "true"}, {"bkcolor", "white"}, {"margin", "4,6,4,4"}});
    auto* p82 = ui::Attach<ui::Split>(p12, {{"bkcolor", "splitline_level1"}, {"width", "2"}});
    auto* p83 = ui::Attach<ui::VBox>(p12, {});
    auto* p84 = ui::Attach<ui::VBox>(p83, {{"bkcolor", "bk_wnd_darkcolor"}, {"height", "auto"}, {"minheight", "40"}});
    auto* p85 = ui::Attach<ui::HBox>(p84, {{"height", "auto"}, {"margin", "0,2,2,0"}});
    auto* p86 = ui::Attach<ui::Label>(p85, {{"text", "文件："}, {"valign", "center"}, {"margin", "8,0,8,0"}});
    auto* p87 = ui::Attach<ui::Button>(p85, {{"class", "btn_global_gray_80x30"}, {"name", "open_file"}, {"width", "100"}, {"height", "28"}, {"text", "打开(Ctrl+O)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p88 = ui::Attach<ui::Button>(p85, {{"class", "btn_global_gray_80x30"}, {"name", "save_file"}, {"width", "100"}, {"height", "28"}, {"text", "保存(Ctrl+S)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p89 = ui::Attach<ui::Button>(p85, {{"class", "btn_global_gray_80x30"}, {"name", "save_as_file"}, {"width", "160"}, {"height", "28"}, {"text", "另存为(Ctrl+Shift+S)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p90 = ui::Attach<ui::HBox>(p84, {{"height", "auto"}, {"margin", "0,2,2,0"}});
    auto* p91 = ui::Attach<ui::Label>(p90, {{"text", "编辑："}, {"valign", "center"}, {"margin", "8,0,8,0"}});
    auto* p92 = ui::Attach<ui::Button>(p90, {{"class", "btn_global_gray_80x30"}, {"name", "btn_copy"}, {"width", "100"}, {"height", "28"}, {"text", "复制(Ctrl+C)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p93 = ui::Attach<ui::Button>(p90, {{"class", "btn_global_gray_80x30"}, {"name", "btn_cut"}, {"width", "100"}, {"height", "28"}, {"text", "剪切(Ctrl+X)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p94 = ui::Attach<ui::Button>(p90, {{"class", "btn_global_gray_80x30"}, {"name", "btn_paste"}, {"width", "100"}, {"height", "28"}, {"text", "粘贴(Ctrl+V)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p95 = ui::Attach<ui::Button>(p90, {{"class", "btn_global_gray_80x30"}, {"name", "btn_delete"}, {"width", "100"}, {"height", "28"}, {"text", "删除(Del)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p96 = ui::Attach<ui::Button>(p90, {{"class", "btn_global_gray_80x30"}, {"name", "btn_sel_all"}, {"width", "100"}, {"height", "28"}, {"text", "全选(Ctrl+A)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,8,0"}});
    auto* p97 = ui::Attach<ui::Button>(p90, {{"class", "btn_global_gray_80x30"}, {"name", "btn_sel_none"}, {"width", "100"}, {"height", "28"}, {"text", "取消选择"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,8,0"}});
    auto* p98 = ui::Attach<ui::Button>(p90, {{"class", "btn_global_gray_80x30"}, {"name", "btn_undo"}, {"width", "100"}, {"height", "28"}, {"text", "撤销(Ctrl+Z)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p99 = ui::Attach<ui::Button>(p90, {{"class", "btn_global_gray_80x30"}, {"name", "btn_redo"}, {"width", "100"}, {"height", "28"}, {"text", "重做(Ctrl+Y)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p100 = ui::Attach<ui::HBox>(p84, {{"height", "auto"}, {"margin", "0,2,2,0"}});
    auto* p101 = ui::Attach<ui::Label>(p100, {{"text", "查找："}, {"valign", "center"}, {"margin", "8,0,8,0"}});
    auto* p102 = ui::Attach<ui::Button>(p100, {{"class", "btn_global_gray_80x30"}, {"name", "btn_find_text"}, {"width", "100"}, {"height", "28"}, {"text", "查找(Ctrl+F)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p103 = ui::Attach<ui::Button>(p100, {{"class", "btn_global_gray_80x30"}, {"name", "btn_find_next"}, {"width", "100"}, {"height", "28"}, {"text", "查找下一个(F3)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p104 = ui::Attach<ui::Button>(p100, {{"class", "btn_global_gray_80x30"}, {"name", "btn_replace_text"}, {"width", "100"}, {"height", "28"}, {"text", "替换(Ctrl+H)"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,0"}});
    auto* p105 = ui::Attach<ui::HBox>(p84, {{"height", "auto"}, {"margin", "0,2,2,0"}});
    auto* p106 = ui::Attach<ui::Label>(p105, {{"text", "字体："}, {"valign", "center"}, {"margin", "8,0,8,0"}});
    auto* p107 = ui::Attach<ui::Button>(p105, {{"class", "btn_global_gray_80x30"}, {"name", "set_font"}, {"width", "100"}, {"height", "28"}, {"text", "设置字体"}, {"borderround", "2,2"}, {"valign", "center"}, {"margin", "4,0,4,2"}});
    auto* p108 = ui::Attach<ui::Combo>(p105, {{"class", "combo"}, {"name", "combo_font_name"}, {"combo_type", "drop_down"}, {"dropbox_size", "0,300"}, {"height", "28"}, {"width", "160"}, {"tooltiptext", "选择字体"}, {"margin", "2,0,2,2"}});
    auto* p109 = ui::Attach<ui::Combo>(p105, {{"class", "combo"}, {"name", "combo_font_size"}, {"combo_type", "drop_down"}, {"dropbox_size", "0,300"}, {"height", "28"}, {"width", "60"}, {"margin", "2,0,2,0"}, {"tooltiptext", "选择字体"}});
    auto* p110 = ui::Attach<ui::CheckBox>(p105, {{"class", "checkbox_font_class"}, {"name", "btn_font_bold"}, {"width", "28"}, {"height", "26"}, {"text", "B"}, {"font", "btn_font_bold_14"}, {"valign", "center"}, {"margin", "2,1,1,3"}, {"tooltiptext", "粗体：改为较粗的字体"}});
    auto* p111 = ui::Attach<ui::CheckBox>(p105, {{"class", "checkbox_font_class"}, {"name", "btn_font_italic"}, {"width", "28"}, {"height", "26"}, {"text", "I"}, {"font", "btn_font_italic_14"}, {"valign", "center"}, {"margin", "1,1,1,3"}, {"tooltiptext", "斜体：改为倾斜的字体"}});
    auto* p112 = ui::Attach<ui::CheckBox>(p105, {{"class", "checkbox_font_class"}, {"name", "btn_font_underline"}, {"width", "28"}, {"height", "26"}, {"text", "U"}, {"font", "system_underline_14"}, {"valign", "center"}, {"margin", "1,1,1,3"}, {"tooltiptext", "下划线：在文本下面画一条线"}});
    auto* p113 = ui::Attach<ui::CheckBox>(p105, {{"class", "checkbox_font_class"}, {"name", "btn_font_strikeout"}, {"width", "28"}, {"height", "26"}, {"text", "abc"}, {"font", "system_strikeout_14"}, {"valign", "center"}, {"margin", "1,1,1,3"}, {"tooltiptext", "删除线：在文本上画一条线"}});
    auto* p114 = ui::Attach<ui::Button>(p105, {{"class", "btn_font_class"}, {"name", "btn_font_size_increase"}, {"width", "32"}, {"height", "26"}, {"text", "A+"}, {"font", "btn_font_bold_14"}, {"valign", "center"}, {"margin", "1,1,1,3"}, {"tooltiptext", "增加字体的大小"}});
    auto* p115 = ui::Attach<ui::Button>(p105, {{"class", "btn_font_class"}, {"name", "btn_font_size_decrease"}, {"width", "32"}, {"height", "26"}, {"text", "A-"}, {"font", "btn_font_bold_14"}, {"valign", "center"}, {"margin", "1,1,1,3"}, {"tooltiptext", "减小字体的大小"}});
    auto* p116 = ui::Attach<ui::ComboButton>(p105, {{"class", "combo_button"}, {"name", "color_combo_button"}, {"height", "28"}, {"width", "48"}, {"dropbox_size", "240,300"}, {"left_button_top_label_text", "A"}, {"left_button_bottom_label_bkcolor", "skyblue"}});
    auto* p117 = ui::Attach<ui::Label>(p105, {{"text", "(富文本模式为设置当前选择文本的格式)"}, {"font", "system_12"}, {"valign", "center"}, {"margin", "2,0,2,0"}, {"tooltiptext", "富文本模式为设置当前选择文本的格式;纯文本模式为设置所有文本的格式。"}});
    auto* p118 = ui::Attach<ui::HBox>(p84, {{"height", "auto"}, {"margin", "0,2,2,0"}});
    auto* p119 = ui::Attach<ui::Label>(p118, {{"text", "缩放："}, {"valign", "center"}, {"margin", "8,0,8,0"}});
    auto* p120 = ui::Attach<ui::Label>(p118, {{"text", "当前缩放比例："}, {"valign", "center"}});
    auto* p121 = ui::Attach<ui::Label>(p118, {{"name", "lavel_zoom_value"}, {"text", "100.0%"}, {"font", "system_bold_14"}, {"valign", "center"}, {"margin", "0,0,12,0"}});
    auto* p122 = ui::Attach<ui::Button>(p118, {{"class", "btn_font_class"}, {"name", "btn_zoom_in"}, {"width", "64"}, {"height", "26"}, {"text", "放大+"}, {"font", "system_14"}, {"valign", "center"}, {"margin", "1,1,1,3"}, {"tooltiptext", "增加字体的大小"}});
    auto* p123 = ui::Attach<ui::Button>(p118, {{"class", "btn_font_class"}, {"name", "btn_zoom_out"}, {"width", "64"}, {"height", "26"}, {"text", "缩小-"}, {"font", "system_14"}, {"valign", "center"}, {"margin", "1,1,1,3"}, {"tooltiptext", "减小字体的大小"}});
    auto* p124 = ui::Attach<ui::Button>(p118, {{"class", "btn_font_class"}, {"name", "btn_zoom_off"}, {"width", "120"}, {"height", "26"}, {"text", "复原到100%"}, {"font", "system_14"}, {"valign", "center"}, {"margin", "1,1,1,3"}, {"tooltiptext", "字体大小复原到100%"}});
    auto* p125 = ui::Attach<ui::HBox>(p84, {{"height", "auto"}, {"margin", "0,2,2,2"}});
    auto* p126 = ui::Attach<ui::Label>(p125, {{"text", "其他："}, {"valign", "center"}, {"margin", "8,0,8,0"}});
    auto* p127 = ui::Attach<ui::CheckBox>(p125, {{"class", "checkbox_1"}, {"name", "btn_word_wrap"}, {"text", "自动换行"}, {"margin", "4,0,0,0"}, {"selected", "true"}, {"tooltiptext", "是否自动换行"}});
    auto* p128 = ui::Attach<ui::CheckBox>(p125, {{"class", "checkbox_1"}, {"name", "btn_rich_text"}, {"text", "富文本模式"}, {"margin", "12,0,0,0"}, {"selected", "true"}, {"tooltiptext", "是否支持富文本格式"}});
    auto* p129 = ui::Attach<ui::Label>(p125, {{"text", "行间距："}, {"valign", "center"}, {"margin", "12,0,0,0"}});
    auto* p130 = ui::Attach<ui::Option>(p125, {{"class", "option_1"}, {"group", "richedit_row_spacing_group"}, {"text", "1.0"}, {"margin", "6,0,0,0"}, {"text_align", "vcenter"}, {"selected", "true"}});
    auto* p131 = ui::Attach<ui::Option>(p125, {{"class", "option_1"}, {"group", "richedit_row_spacing_group"}, {"text", "1.15"}, {"margin", "6,0,0,0"}, {"text_align", "vcenter"}});
    auto* p132 = ui::Attach<ui::Option>(p125, {{"class", "option_1"}, {"group", "richedit_row_spacing_group"}, {"text", "1.5"}, {"margin", "6,0,0,0"}, {"text_align", "vcenter"}});
    auto* p133 = ui::Attach<ui::Option>(p125, {{"class", "option_1"}, {"group", "richedit_row_spacing_group"}, {"text", "2.0"}, {"margin", "6,0,0,0"}, {"text_align", "vcenter"}});
    auto* p134 = ui::Attach<ui::Option>(p125, {{"class", "option_1"}, {"group", "richedit_row_spacing_group"}, {"text", "2.5"}, {"margin", "6,0,0,0"}, {"text_align", "vcenter"}});
    auto* p135 = ui::Attach<ui::Option>(p125, {{"class", "option_1"}, {"group", "richedit_row_spacing_group"}, {"text", "3.0"}, {"margin", "6,0,0,0"}, {"text_align", "vcenter"}});
    auto* p136 = ui::Attach<ui::Label>(p125, {{"name", "row_spacing_tips"}, {"text", "（行间距：仅在富文本模式下有效）"}, {"valign", "center"}, {"margin", "4,0,0,0"}});
    auto* p137 = ui::Attach<ui::Split>(p83, {{"bkcolor", "splitline_level1"}, {"height", "2"}});
    auto* p138 = ui::Attach<ui::HBox>(p83, {{"bkcolor", "Pink"}, {"border_size", "1"}, {"border_color", "blue"}, {"minwidth", "60"}});
    auto* p139 = ui::Attach<ui::RichEdit>(p138, {{"name", "rich_edit"}, {"enable_drag_drop", "true"}, {"zoom", "0,0"}, {"wheel_zoom", "true"}, {"multi_line", "true"}, {"want_return", "true"}, {"word_wrap", "true"}, {"rich_text", "false"}, {"default_context_menu", "true"}, {"save_selection", "true"}, {"hide_selection", "false"}, {"word_wrap", "true"}, {"vscrollbar", "true"}, {"auto_vscroll", "true"}, {"bkcolor", "white"}});

    ui::Attach(pWindow, p0);
}
