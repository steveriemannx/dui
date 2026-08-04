#include "MainForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code (from rich_edit.xml)
#include "FindForm.h"
#include "ReplaceForm.h"
#include <fstream>

#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
#include <ShellApi.h>
#include <commdlg.h>
#endif

#ifndef LY_PER_INCH
    #define LY_PER_INCH 1440
#endif

MainForm::MainForm():
    m_pRichEdit(nullptr),
    m_pFindForm(nullptr),
    m_pReplaceForm(nullptr)
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("rich_edit");
}

DString MainForm::GetSkinFile()
{
    // No XML file - UI is generated at build time from rich_edit.xml
    return _T("");
}

void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    // Build-time generated from rich_edit.xml
    InitRich_edit(this);

    ui::RichEdit* pRichEdit = dynamic_cast<ui::RichEdit*>(FindControl(_T("test_url")));
    if (pRichEdit != nullptr) {
        pRichEdit->AttachLinkClick([this, pRichEdit](const ui::EventArgs& args) {
                // Hyperlink clicked
                if (args.GetSender() == pRichEdit) {
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
    m_pRichEdit = dynamic_cast<ui::RichEdit*>(FindControl(_T("rich_edit")));
    ASSERT(m_pRichEdit != nullptr);
    m_findReplace.SetRichEdit(m_pRichEdit);
    LoadRichEditData();

    // File operations: open, save, save as
    ui::Button* pButton = dynamic_cast<ui::Button*>(FindControl(_T("open_file")));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    this->OnOpenFile();
                }
                return true;
            });
    }
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("save_file")));
    if (pButton != nullptr) {
        m_saveBtnText = pButton->GetText();
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    this->OnSaveFile();
                }
                return true;
            });
    }
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("save_as_file")));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
                if (args.GetSender() == pButton) {
                    this->OnSaveAsFile();
                }
                return true;
            });
    }

    // Edit operations
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_copy")));
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
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_cut")));
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
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_paste")));
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
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_delete")));
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
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_sel_all")));
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
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_sel_none")));
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
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_undo")));
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
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_redo")));
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
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_find_text")));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                OnFindText();
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
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_replace_text")));
    if (pButton != nullptr) {
        pButton->AttachClick([this, pButton](const ui::EventArgs& args) {
            if (args.GetSender() == pButton) {
                OnReplaceText();
            }
            return true;
            });
    }

    // Set font
    pButton = dynamic_cast<ui::Button*>(FindControl(_T("set_font")));
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

    // Initialize font information
    ui::Combo* pFontNameCombo = dynamic_cast<ui::Combo*>(FindControl(_T("combo_font_name"))); 
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
    ui::Combo* pFontSizeCombo = dynamic_cast<ui::Combo*>(FindControl(_T("combo_font_size")));
    if (pFontSizeCombo != nullptr) {
        ui::GlobalManager::Instance().Font().GetFontSizeList(Dpi(), m_fontSizeList);
        for (size_t nIndex = 0; nIndex < m_fontSizeList.size(); ++nIndex) {
            const ui::FontSizeInfo& fontSize = m_fontSizeList[nIndex];
            size_t nItemIndex = pFontSizeCombo->AddTextItem(fontSize.fontSizeName);
            if (ui::Box::IsValidItemIndex(nItemIndex)) {
                pFontSizeCombo->SetItemData(nItemIndex, nIndex);
            }
        }
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

    // Update bold state
    ui::CheckBox* pCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("btn_font_bold")));
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

    // Update italic state
    pCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("btn_font_italic")));
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

    // Update underline state
    pCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("btn_font_underline")));
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

    // Update strikethrough state
    pCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("btn_font_strikeout")));
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

    // Increase font size
    ui::Button* pFontButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_font_size_increase")));
    if (pFontButton != nullptr) {
        pFontButton->AttachClick([this](const ui::EventArgs& args) {
            AdjustFontSize(true);
            return true;
            });
    }
    // Decrease font size
    pFontButton = dynamic_cast<ui::Button*>(FindControl(_T("btn_font_size_decrease")));
    if (pFontButton != nullptr) {
        pFontButton->AttachClick([this](const ui::EventArgs& args) {
            AdjustFontSize(false);
            return true;
            });
    }

    // Set color
    InitColorCombo();
    ui::ComboButton* pColorComboBtn = dynamic_cast<ui::ComboButton*>(FindControl(_T("color_combo_button")));
    if (pColorComboBtn != nullptr) {
        DString textColor;
        if (m_pRichEdit != nullptr) {
            textColor = m_pRichEdit->GetTextColor();
        }
        // Set the color after selection
        ui::Label* pLeftColorLabel = pColorComboBtn->GetLabelBottom();
        if (pLeftColorLabel != nullptr) {
            pLeftColorLabel->SetBkColor(textColor);
        }

        // Left button click event
        pColorComboBtn->AttachClick([this, pLeftColorLabel](const ui::EventArgs& args) {
            if (pLeftColorLabel != nullptr) {
                SetTextColor(pLeftColorLabel->GetBkColor());
            }
            return true;
            });
    }

    UpdateZoomValue();
    if (m_pRichEdit != nullptr) {
        m_pRichEdit->AttachZoom([this](const ui::EventArgs& args) {
            UpdateZoomValue();
            return true;
            });
    }
    ui::Button* pZoomButtom = dynamic_cast<ui::Button*>(FindControl(_T("btn_zoom_in")));
    if (pZoomButtom != nullptr) {
        pZoomButtom->AttachClick([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                // Zoom in: 10% each time
                uint32_t nZoomPercent = GetNextZoomPercent(m_pRichEdit->GetZoomPercent(), true);
                m_pRichEdit->SetZoomPercent(nZoomPercent);
                UpdateZoomValue();
            }
            return true;
            });
    }
    pZoomButtom = dynamic_cast<ui::Button*>(FindControl(_T("btn_zoom_out")));
    if (pZoomButtom != nullptr) {
        pZoomButtom->AttachClick([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                // Zoom out: 10% each time
                uint32_t nZoomPercent = GetNextZoomPercent(m_pRichEdit->GetZoomPercent(), false);
                m_pRichEdit->SetZoomPercent(nZoomPercent);
                UpdateZoomValue();
            }
            return true;
            });
    }
    pZoomButtom = dynamic_cast<ui::Button*>(FindControl(_T("btn_zoom_off")));
    if (pZoomButtom != nullptr) {
        pZoomButtom->AttachClick([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                // Restore
                m_pRichEdit->SetZoomPercent(100);
                UpdateZoomValue();
            }
            return true;
            });
    }

    // Whether to wrap text automatically
    pCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("btn_word_wrap")));
    if ((pCheckBox != nullptr) && (m_pRichEdit != nullptr)) {
        pCheckBox->SetSelected(m_pRichEdit->IsWordWrap());
        pCheckBox->AttachSelect([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                m_pRichEdit->SetWordWrap(true);
                m_pRichEdit->SetAttribute(_T("hscrollbar"), _T("false"));
            }
            return true;
            });
        pCheckBox->AttachUnSelect([this](const ui::EventArgs& args) {
            if (m_pRichEdit != nullptr) {
                m_pRichEdit->SetWordWrap(false);
                m_pRichEdit->SetAttribute(_T("hscrollbar"), _T("true"));
            }
            return true;
            });
    }

    // Whether rich text format is supported
    pCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("btn_rich_text")));
    if ((pCheckBox != nullptr) && (m_pRichEdit != nullptr)) {
        pCheckBox->SetSelected(m_pRichEdit->IsRichText());
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
#else
        // Rich text format is not supported in the SDL implementation
        pCheckBox->SetEnabled(false);
#endif
    }

#ifdef DUI_BUILD_FOR_SDL
    ui::Control* pRowSpacingTips = FindControl(_T("row_spacing_tips"));
    if (pRowSpacingTips != nullptr) {
        pRowSpacingTips->SetVisible(false);
    }
#endif

    // Update the state of the font buttons
    UpdateFontStatus();

    // Hyperlink
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
    ui::ComboButton* pColorComboBtn = dynamic_cast<ui::ComboButton*>(FindControl(_T("color_combo_button")));
    if (pColorComboBtn == nullptr) {
        return;
    }
    ui::UiSize boxSize = pColorComboBtn->GetDropBoxSize();
    ui::Box* pComboBox = pColorComboBtn->GetComboBox();
    if (pComboBox == nullptr) {
        return;
    }
    pComboBox->SetWindow(this);
    // The color combo box template is built by code (no longer loads color_combox.xml)
    AddClass(_T("color_combo_picker_btn"),
             _T(" font=\"system_14\" normal_text_color=\"black\" disabled_text_color=\"gray\" text_align=\"hcenter,vcenter\" border_size=\"1\" hot_border_color=\"#FFB3D0EE\" pushed_border_color=\"#FF82B4E8\" hot_color=\"#FFE8EFF7\" pushed_color=\"#FFC9E0F7\""));
    ui::VBox* pColorBox = new ui::VBox(this);
    pColorBox->SetBkColor(_T("bk_wnd_darkcolor"));
    pComboBox->AddItem(pColorBox);

    ui::ColorPickerRegular* pNewColorPicker = new ui::ColorPickerRegular(this);
    pNewColorPicker->SetName(_T("color_combo_picker"));
    pNewColorPicker->SetAttribute(_T("color_type"), _T("default"));
    pNewColorPicker->SetAttribute(_T("item_size"), _T("40,20"));
    pNewColorPicker->SetAttribute(_T("columns"), _T("10"));
    pNewColorPicker->SetAttribute(_T("child_margin"), _T("2"));
    pNewColorPicker->SetAttribute(_T("padding"), _T("2,2,2,2"));
    pNewColorPicker->SetAttribute(_T("halign"), _T("center"));
    pNewColorPicker->SetAttribute(_T("valign"), _T("center"));
    pColorBox->AddItem(pNewColorPicker);

    ui::Button* pNewMoreColorButton = new ui::Button(this);
    pNewMoreColorButton->SetClass(_T("color_combo_picker_btn"));
    pNewMoreColorButton->SetName(_T("color_combo_picker_more"));
    pNewMoreColorButton->SetText(_T("  More Colors ...  "));
    pNewMoreColorButton->SetAttribute(_T("width"), _T("auto"));
    pNewMoreColorButton->SetAttribute(_T("height"), _T("30"));
    pNewMoreColorButton->SetAttribute(_T("margin"), _T("2,0,2,2"));
    pNewMoreColorButton->SetAttribute(_T("halign"), _T("center"));
    pColorBox->AddItem(pNewMoreColorButton);
    ui::ColorPickerRegular* pColorPicker = dynamic_cast<ui::ColorPickerRegular*>(pComboBox->FindSubControl(_T("color_combo_picker")));
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

    ui::Button* pMoreColorButton = dynamic_cast<ui::Button*>(pComboBox->FindSubControl(_T("color_combo_picker_more")));
    if (pMoreColorButton != nullptr) {
        pMoreColorButton->AttachClick([this](const ui::EventArgs& args) {
            ShowColorPicker();
            return true;
            });
    }
}

void MainForm::ShowColorPicker()
{
    ui::ComboButton* pColorComboBtn = dynamic_cast<ui::ComboButton*>(FindControl(_T("color_combo_button")));
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
    createParam.m_windowTitle = _T("ColorPicker");
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
    ui::Combo* pFontNameCombo = dynamic_cast<ui::Combo*>(FindControl(_T("combo_font_name")));
    if (pFontNameCombo != nullptr) {
        pFontNameCombo->SelectTextItem(fontInfo.m_fontName.c_str(), false);
    }

    // Update the font size
    UpdateFontSizeStatus();

    // Update bold state
    ui::CheckBox* pCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("btn_font_bold")));
    if (pCheckBox != nullptr) {
        pCheckBox->SetSelected(fontInfo.m_bBold);
    }

    // Update italic state
    pCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("btn_font_italic")));
    if (pCheckBox != nullptr) {
        pCheckBox->SetSelected(fontInfo.m_bItalic);
    }

    // Update underline state
    pCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("btn_font_underline")));
    if (pCheckBox != nullptr) {
        pCheckBox->SetSelected(fontInfo.m_bUnderline);
    }

    // Update strikethrough state
    pCheckBox = dynamic_cast<ui::CheckBox*>(FindControl(_T("btn_font_strikeout")));
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
    ui::Combo* pFontSizeCombo = dynamic_cast<ui::Combo*>(FindControl(_T("combo_font_size")));
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
        ui::Button* pButton = dynamic_cast<ui::Button*>(FindControl(_T("save_file")));
        if (m_pRichEdit->GetModify()) {
            if (pButton != nullptr) {
                pButton->SetText(m_saveBtnText + _T("*"));
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
    std::streamoff length = 0;
    std::string xml;
    ui::FilePath controls_xml = ui::GlobalManager::Instance().GetResourcePath();
    controls_xml += GetResourcePath();
    controls_xml += GetSkinFile();

    std::ifstream ifs(controls_xml.NativePath().c_str(), std::ios::binary);
    if (ifs.is_open()) {
        ifs.seekg(0, std::ios_base::end);
        length = ifs.tellg();
        ifs.seekg(0, std::ios_base::beg);

        xml.resize(static_cast<unsigned int>(length));
        ifs.read(&xml[0], length);
        ifs.close();
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
        createParam.m_windowTitle = _T("FindForm");
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
        createParam.m_windowTitle = _T("ReplaceForm");
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

    ui::Label* pZoomLabel = dynamic_cast<ui::Label*>(FindControl(_T("lavel_zoom_value")));
    if (pZoomLabel != nullptr) {
        uint32_t nZoomPercent = pRichEdit->GetZoomPercent();
        DString strZoom = ui::StringUtil::Printf(_T("%u%%"), nZoomPercent);
        pZoomLabel->SetText(strZoom);
    }
}

void MainForm::OnOpenFile()
{
    std::vector<ui::FileDialog::FileType> fileTypes;
    fileTypes.push_back({ _T("All Files (*.*)"), _T("*.*")});
    fileTypes.push_back({ _T("Text Files (*.txt)"), _T("*.txt") });
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
    fileTypes.push_back({ _T("RTF Files (*.rtf)"), _T("*.rtf") });
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
    fileTypes.push_back({ _T("All Files (*.*)"), _T("*.*") });
    fileTypes.push_back({ _T("Text Files (*.txt)"), _T("*.txt") });
#if defined (DUI_BUILD_FOR_WIN) && !defined (DUI_BUILD_FOR_SDL)
    fileTypes.push_back({ _T("RTF Files (*.rtf)"), _T("*.rtf") });
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
    size_t pos = filePath.find_last_of(_T("."));
    if (pos != DString::npos) {
        fileExt = filePath.substr(pos);
        fileExt = ui::StringUtil::MakeLowerString(fileExt);
    }
    return fileExt == _T(".rtf");
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
        ui::ComboButton* pColorComboBtn = dynamic_cast<ui::ComboButton*>(FindControl(_T("color_combo_button")));
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
