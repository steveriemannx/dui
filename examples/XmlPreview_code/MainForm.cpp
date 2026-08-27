//MainForm.cpp
#include "MainForm.h"

MainForm::MainForm():
    m_nIntervalSeconds(-1),
    m_timerId(0)
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("");
}

DString MainForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void MainForm::BuildUI()
{
    // Corresponding to the xml_preview.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{_T("name"), _T("window_caption_bar")}, {_T("width"), _T("stretch")}, {_T("height"), _T("36")}});
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pCaptionLeft = ui::Create<ui::HBox>(this, {{_T("margin"), _T("0,0,30,0")}, {_T("valign"), _T("center")}, {_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("mouse_enabled"), _T("false")}});
    pCaption->AddItem(pCaptionLeft);

    auto* pTitle = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}, {_T("margin"), _T("8,0,0,0")}, {_T("mouse_enabled"), _T("false")}});
    pTitle->SetText(_T("XML File UI Preview Test"));
    pCaptionLeft->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{_T("mouse_enabled"), _T("false")}});
    pCaption->AddItem(pSpacer);

    auto* pCaptionRight = ui::Create<ui::HBox>(this, {{_T("margin"), _T("0,0,0,0")}, {_T("valign"), _T("center")}, {_T("width"), _T("auto")}, {_T("height"), _T("36")}});
    pCaption->AddItem(pCaptionRight);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaptionRight->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pCaptionRight->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("stretch")}});
    pMaxBtn->SetClass(_T("btn_wnd_max_11"));
    pMaxBtn->SetName(_T("maxbtn"));
    pMaxBtn->SetToolTipText(_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("stretch")}});
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,0,0,2")}});
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetToolTipText(_T("Close"));
    pCaptionRight->AddItem(pCloseBtn);

    // Work area
    auto* pContent = ui::Create<ui::VBox>(this, {{_T("padding"), _T("10,10,10,10")}});
    pRoot->AddItem(pContent);

    auto* pToolArea = ui::Create<ui::VBox>(this, {{_T("height"), _T("auto")}});
    pContent->AddItem(pToolArea);

    // XML file path row
    auto* pPathRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}});
    pToolArea->AddItem(pPathRow);

    auto* pPathLabel = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}});
    pPathLabel->SetText(_T("XML File Path:"));
    pPathRow->AddItem(pPathLabel);

    auto* pPathValue = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}, {_T("width"), _T("100%")}, {_T("path_ellipsis"), _T("true")}, {_T("single_line"), _T("true")}, {_T("auto_tooltip"), _T("true")}});
    pPathValue->SetName(_T("xml_file_path"));
    pPathRow->AddItem(pPathValue);

    // Load result row
    auto* pResultRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}});
    pToolArea->AddItem(pResultRow);

    auto* pResultLabel = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}});
    pResultLabel->SetText(_T("XML Load Result:"));
    pResultRow->AddItem(pResultLabel);

    auto* pResultValue = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}, {_T("width"), _T("100%")}});
    pResultValue->SetName(_T("xml_file_load_result"));
    pResultRow->AddItem(pResultValue);

    // Toolbar row
    auto* pToolRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("auto")}});
    pToolArea->AddItem(pToolRow);

    auto* pHint = ui::Create<ui::Label>(this, {{_T("text_align"), _T("vcenter")}, {_T("valign"), _T("center")}, {_T("height"), _T("30")}});
    pHint->SetName(_T("xml_file_path"));
    pHint->SetText(_T("You can drag an XML file into the display area below to preview it"));
    pToolRow->AddItem(pHint);

    auto* pToolSpacer = ui::Create<ui::Control>(this, {});
    pToolRow->AddItem(pToolSpacer);

    auto* pAutoRefresh = ui::Create<ui::CheckBox>(this, {{_T("margin"), _T("0,0,8,0")}});
    pAutoRefresh->SetClass(_T("checkbox_2"));
    pAutoRefresh->SetName(_T("checkbox_auto_refresh"));
    pAutoRefresh->SetText(_T("Auto Refresh"));
    pAutoRefresh->Selected(false);
    pToolRow->AddItem(pAutoRefresh);

    auto* pIntervalLabel = ui::Create<ui::Label>(this, {{_T("text_align"), _T("vcenter")}, {_T("valign"), _T("center")}});
    pIntervalLabel->SetText(_T("Interval (seconds):"));
    pToolRow->AddItem(pIntervalLabel);

    auto* pIntervalEdit = ui::Create<ui::RichEdit>(this, {{_T("min_number"), _T("1")}, {_T("max_number"), _T("60")}, {_T("limit_text"), _T("2")}, {_T("height"), _T("30")}, {_T("margin"), _T("0,0,8,0")}});
    pIntervalEdit->SetClass(_T("simple simple_border rich_edit_spin"));
    pIntervalEdit->SetName(_T("auto_refresh_interval"));
    pIntervalEdit->SetText(_T("5"));
    pToolRow->AddItem(pIntervalEdit);

    auto* pClearBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("120")}, {_T("height"), _T("30")}, {_T("border_round"), _T("3,3")}, {_T("margin"), _T("0,0,8,0")}});
    pClearBtn->SetClass(_T("btn_global_color_gray"));
    pClearBtn->SetName(_T("btn_clear_xml_file"));
    pClearBtn->SetText(_T("Clear Preview"));
    pToolRow->AddItem(pClearBtn);

    auto* pBrowseBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("120")}, {_T("height"), _T("30")}, {_T("border_round"), _T("3,3")}, {_T("margin"), _T("0,0,10,0")}});
    pBrowseBtn->SetClass(_T("btn_global_color_gray"));
    pBrowseBtn->SetName(_T("btn_browse_xml_file"));
    pBrowseBtn->SetText(_T("Open XML File..."));
    pToolRow->AddItem(pBrowseBtn);

    // XML preview area
    auto* pXmlBox = ui::Create<ui::XmlBox>(this, {{_T("border_color"), _T("blue")}, {_T("border_size"), _T("1")}, {_T("border_dash_style"), _T("dash")}, {_T("margin"), _T("4,4,4,4")}, {_T("mouse_child"), _T("true")}, {_T("res_path"), _T("controls")}, {_T("xml_file_path"), _T("controls.xml")}});
    pXmlBox->SetName(_T("xml_box_test"));
    pXmlBox->SetBkColor(_T("white"));
    pContent->AddItem(pXmlBox);

    AttachBox(pRoot);
}


void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    BuildUI();

    BaseClass::OnInitWindow();
    ui::XmlBox* pXmlBox = dynamic_cast<ui::XmlBox*>(FindControl(_T("xml_box_test")));
    if (pXmlBox == nullptr) {
        return;
    }
    m_xmlFilePath = pXmlBox->GetXmlFileFullPath();
    ui::Label* pXmlFilePath = dynamic_cast<ui::Label*>(FindControl(_T("xml_file_path")));
    if (pXmlFilePath != nullptr) {
        pXmlFilePath->SetText(pXmlBox->GetXmlFileFullPath().ToString());
    }
    ui::Label* pXmlFileLoadResult = dynamic_cast<ui::Label*>(FindControl(_T("xml_file_load_result")));
    if (pXmlFileLoadResult != nullptr) {
        if (!pXmlBox->GetXmlFileFullPath().IsEmpty()) {
            pXmlFileLoadResult->SetText(_T("OK"));
        }
    }
    pXmlBox->AddLoadXmlCallback([this, pXmlBox, pXmlFilePath, pXmlFileLoadResult](const ui::FilePath& xmlPath, bool bSuccess) {
            if (bSuccess) {
                m_xmlFilePath = pXmlBox->GetXmlFileFullPath();
                if (pXmlFilePath != nullptr) {
                    pXmlFilePath->SetText(pXmlBox->GetXmlFileFullPath().ToString());
                }
                if (pXmlFileLoadResult != nullptr) {
                    pXmlFileLoadResult->SetText(_T("OK"));
                }
            }
            else {
                if (pXmlFileLoadResult != nullptr) {
                    DString errMsg = ui::StringUtil::Printf(_T("Failed [%s]"), xmlPath.ToString().c_str());
                    pXmlFileLoadResult->SetText(errMsg);
                }
            }
        });

    // Clear the preview
    ui::Button* pClearXmlBtn = dynamic_cast<ui::Button*>(FindControl(_T("btn_clear_xml_file")));
    if (pClearXmlBtn != nullptr) {
        pClearXmlBtn->AttachClick([this, pXmlBox, pXmlFilePath, pXmlFileLoadResult](const ui::EventArgs&) {
            // Show the Open XML File dialog
            pXmlBox->ClearXmlBox();
            m_xmlFilePath.Clear();
            if (pXmlFileLoadResult != nullptr) {
                pXmlFileLoadResult->SetText(_T(""));
            }
            if (pXmlFilePath != nullptr) {
                pXmlFilePath->SetText(pXmlBox->GetXmlFileFullPath().ToString());
            }
            return true;
            });
    }

    // Open the XML file
    ui::Button* pBrowseXmlBtn = dynamic_cast<ui::Button*>(FindControl(_T("btn_browse_xml_file")));
    if (pBrowseXmlBtn != nullptr) {
        pBrowseXmlBtn->AttachClick([this, pXmlBox](const ui::EventArgs&) {
            // Show the Open XML File dialog
            ui::FilePath xmlFilePath;
            std::vector<ui::FileDialog::FileType> fileTypes;
            ui::FileDialog::FileType xmlFileType;
            xmlFileType.szName = _T("XML Files");
            xmlFileType.szExt = _T("*.xml");
            fileTypes.push_back(xmlFileType);
            ui::FileDialog dlg;
            if (dlg.BrowseForFile(this, xmlFilePath, true, fileTypes)) {
                if (!xmlFilePath.IsEmpty()) {
                    pXmlBox->SetXmlFilePath(xmlFilePath);
                }
            }
            return true;
            });
    }

    // Support dragging an XML file directly in
    pXmlBox->SetEnableDragDrop(true);
    pXmlBox->SetEnableDropFile(true);
    pXmlBox->SetDropFileTypes(_T(".xml"));
    pXmlBox->AttachDropData([this, pXmlBox](const ui::EventArgs& args) {
        if (args.wParam == ui::kControlDropTypeWindows) {
            // Windows SDK implementation, not cross-platform
            ui::ControlDropData_Windows* dropData = (ui::ControlDropData_Windows*)args.lParam;
            if ((dropData != nullptr) && !dropData->m_fileList.empty()) {
                ui::FilePath xmlFilePath = ui::FilePath(dropData->m_fileList[0]);
                pXmlBox->SetXmlFilePath(xmlFilePath);
            }
        }
        else if (args.wParam == ui::kControlDropTypeSDL) {
            // SDL implementation, cross-platform
            ui::ControlDropData_SDL* dropData = (ui::ControlDropData_SDL*)args.lParam;
            if ((dropData != nullptr) && !dropData->m_fileList.empty()) {
                ui::FilePath xmlFilePath = ui::FilePath(dropData->m_fileList[0]);
                pXmlBox->SetXmlFilePath(xmlFilePath);
            }
        }
        return true;
        });

    // Auto-refresh feature
    ui::CheckBox* pCheckBoxRefresh = dynamic_cast<ui::CheckBox*>(FindControl(_T("checkbox_auto_refresh")));
    ui::RichEdit* pRefreshInterval = dynamic_cast<ui::RichEdit*>(FindControl(_T("auto_refresh_interval")));
    if (pCheckBoxRefresh != nullptr) {
        pCheckBoxRefresh->SetSelected(false);
    }
    if (pRefreshInterval != nullptr) {
        pRefreshInterval->SetEnabled(false);
    }

    if (pCheckBoxRefresh != nullptr) {
        pCheckBoxRefresh->AttachSelect([this, pRefreshInterval](const ui::EventArgs&) {
            if (pRefreshInterval != nullptr) {
                pRefreshInterval->SetEnabled(true);

                int32_t nSeconds = (int32_t)pRefreshInterval->GetTextNumber();
                // Adjust the timer event interval
                StartRefreshTimer(nSeconds);
            }
            return true;
            });
        pCheckBoxRefresh->AttachUnSelect([this, pRefreshInterval](const ui::EventArgs&) {
            if (pRefreshInterval != nullptr) {
                pRefreshInterval->SetEnabled(false);
            }
            StopRefreshTimer();
            return true;
            });
    }

    if (pRefreshInterval != nullptr) {
        pRefreshInterval->AttachTextChanged([this, pRefreshInterval](const ui::EventArgs&) {
            int32_t nSeconds = (int32_t)pRefreshInterval->GetTextNumber();
            // Adjust the timer event interval
            StartRefreshTimer(nSeconds);
            return true;
            });
    }
}

void MainForm::StartRefreshTimer(int32_t nIntervalSeconds)
{
    if (nIntervalSeconds < 0) {
        return;
    }
    if (m_nIntervalSeconds == nIntervalSeconds) {
        return;
    }
    StopRefreshTimer();
    m_nIntervalSeconds = nIntervalSeconds;
    m_timerId = ui::GlobalManager::Instance().Timer().AddTimer(GetWeakFlag(), [this]() {
        // Periodically check for refresh
        CheckXmlPreview();
        }, nIntervalSeconds * 1000);
}

void MainForm::StopRefreshTimer()
{
    if (m_timerId > 0) {
        ui::GlobalManager::Instance().Timer().RemoveTimer(m_timerId);
        m_timerId = 0;
    }
}

void MainForm::CheckXmlPreview()
{
    if (!m_xmlFilePath.IsEmpty()) {
        std::vector<uint8_t> xmlFileData;
        if (ui::FileUtil::ReadFileData(m_xmlFilePath, xmlFileData)) {
            if (xmlFileData != m_xmlFileData) {
                // The file content changed; refresh the preview
                ui::XmlBox* pXmlBox = dynamic_cast<ui::XmlBox*>(FindControl(_T("xml_box_test")));
                if (pXmlBox != nullptr) {
                    pXmlBox->SetXmlFilePath(m_xmlFilePath);
                }
                m_xmlFileData.swap(xmlFileData);
            }
        }
    }
}

