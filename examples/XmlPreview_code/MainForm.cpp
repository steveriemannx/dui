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
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("36"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::HBox* pCaptionLeft = new ui::HBox(this);
    pCaptionLeft->SetAttribute(_T("margin"), _T("0,0,30,0"));
    pCaptionLeft->SetAttribute(_T("valign"), _T("center"));
    pCaptionLeft->SetAttribute(_T("width"), _T("auto"));
    pCaptionLeft->SetAttribute(_T("height"), _T("auto"));
    pCaptionLeft->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pCaptionLeft);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetText(_T("XML File UI Preview Test"));
    pTitle->SetAttribute(_T("valign"), _T("center"));
    pTitle->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pTitle->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaptionLeft->AddItem(pTitle);

    ui::Control* pSpacer = new ui::Control(this);
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pSpacer);

    ui::HBox* pCaptionRight = new ui::HBox(this);
    pCaptionRight->SetAttribute(_T("margin"), _T("0,0,0,0"));
    pCaptionRight->SetAttribute(_T("valign"), _T("center"));
    pCaptionRight->SetAttribute(_T("width"), _T("auto"));
    pCaptionRight->SetAttribute(_T("height"), _T("36"));
    pCaption->AddItem(pCaptionRight);

    ui::Button* pMinBtn = new ui::Button(this);
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetAttribute(_T("height"), _T("32"));
    pMinBtn->SetAttribute(_T("width"), _T("40"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaptionRight->AddItem(pMinBtn);

    ui::Box* pMaxBox = new ui::Box(this);
    pMaxBox->SetAttribute(_T("height"), _T("stretch"));
    pMaxBox->SetAttribute(_T("width"), _T("40"));
    pMaxBox->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pCaptionRight->AddItem(pMaxBox);

    ui::Button* pMaxBtn = new ui::Button(this);
    pMaxBtn->SetClass(_T("btn_wnd_max_11"));
    pMaxBtn->SetAttribute(_T("height"), _T("32"));
    pMaxBtn->SetAttribute(_T("width"), _T("stretch"));
    pMaxBtn->SetName(_T("maxbtn"));
    pMaxBtn->SetToolTipText(_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    ui::Button* pRestoreBtn = new ui::Button(this);
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetAttribute(_T("height"), _T("32"));
    pRestoreBtn->SetAttribute(_T("width"), _T("stretch"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetAttribute(_T("height"), _T("stretch"));
    pCloseBtn->SetAttribute(_T("width"), _T("40"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetAttribute(_T("margin"), _T("0,0,0,2"));
    pCloseBtn->SetToolTipText(_T("Close"));
    pCaptionRight->AddItem(pCloseBtn);

    // Work area
    ui::VBox* pContent = new ui::VBox(this);
    pContent->SetAttribute(_T("padding"), _T("10,10,10,10"));
    pRoot->AddItem(pContent);

    ui::VBox* pToolArea = new ui::VBox(this);
    pToolArea->SetAttribute(_T("height"), _T("auto"));
    pContent->AddItem(pToolArea);

    // XML file path row
    ui::HBox* pPathRow = new ui::HBox(this);
    pPathRow->SetAttribute(_T("height"), _T("auto"));
    pToolArea->AddItem(pPathRow);

    ui::Label* pPathLabel = new ui::Label(this);
    pPathLabel->SetText(_T("XML File Path:"));
    pPathLabel->SetAttribute(_T("valign"), _T("center"));
    pPathRow->AddItem(pPathLabel);

    ui::Label* pPathValue = new ui::Label(this);
    pPathValue->SetName(_T("xml_file_path"));
    pPathValue->SetAttribute(_T("valign"), _T("center"));
    pPathValue->SetAttribute(_T("width"), _T("100%"));
    pPathValue->SetAttribute(_T("path_ellipsis"), _T("true"));
    pPathValue->SetAttribute(_T("single_line"), _T("true"));
    pPathValue->SetAttribute(_T("auto_tooltip"), _T("true"));
    pPathRow->AddItem(pPathValue);

    // Load result row
    ui::HBox* pResultRow = new ui::HBox(this);
    pResultRow->SetAttribute(_T("height"), _T("auto"));
    pToolArea->AddItem(pResultRow);

    ui::Label* pResultLabel = new ui::Label(this);
    pResultLabel->SetText(_T("XML Load Result:"));
    pResultLabel->SetAttribute(_T("valign"), _T("center"));
    pResultRow->AddItem(pResultLabel);

    ui::Label* pResultValue = new ui::Label(this);
    pResultValue->SetName(_T("xml_file_load_result"));
    pResultValue->SetAttribute(_T("valign"), _T("center"));
    pResultValue->SetAttribute(_T("width"), _T("100%"));
    pResultRow->AddItem(pResultValue);

    // Toolbar row
    ui::HBox* pToolRow = new ui::HBox(this);
    pToolRow->SetAttribute(_T("height"), _T("auto"));
    pToolArea->AddItem(pToolRow);

    ui::Label* pHint = new ui::Label(this);
    pHint->SetName(_T("xml_file_path"));
    pHint->SetText(_T("You can drag an XML file into the display area below to preview it"));
    pHint->SetAttribute(_T("text_align"), _T("vcenter"));
    pHint->SetAttribute(_T("valign"), _T("center"));
    pHint->SetAttribute(_T("height"), _T("30"));
    pToolRow->AddItem(pHint);

    ui::Control* pToolSpacer = new ui::Control(this);
    pToolRow->AddItem(pToolSpacer);

    ui::CheckBox* pAutoRefresh = new ui::CheckBox(this);
    pAutoRefresh->SetClass(_T("checkbox_2"));
    pAutoRefresh->SetName(_T("checkbox_auto_refresh"));
    pAutoRefresh->SetText(_T("Auto Refresh"));
    pAutoRefresh->SetAttribute(_T("margin"), _T("0,0,8,0"));
    pAutoRefresh->Selected(false);
    pToolRow->AddItem(pAutoRefresh);

    ui::Label* pIntervalLabel = new ui::Label(this);
    pIntervalLabel->SetText(_T("Interval (seconds):"));
    pIntervalLabel->SetAttribute(_T("text_align"), _T("vcenter"));
    pIntervalLabel->SetAttribute(_T("valign"), _T("center"));
    pToolRow->AddItem(pIntervalLabel);

    ui::RichEdit* pIntervalEdit = new ui::RichEdit(this);
    pIntervalEdit->SetClass(_T("simple simple_border rich_edit_spin"));
    pIntervalEdit->SetName(_T("auto_refresh_interval"));
    pIntervalEdit->SetAttribute(_T("min_number"), _T("1"));
    pIntervalEdit->SetAttribute(_T("max_number"), _T("60"));
    pIntervalEdit->SetAttribute(_T("limit_text"), _T("2"));
    pIntervalEdit->SetText(_T("5"));
    pIntervalEdit->SetAttribute(_T("height"), _T("30"));
    pIntervalEdit->SetAttribute(_T("margin"), _T("0,0,8,0"));
    pToolRow->AddItem(pIntervalEdit);

    ui::Button* pClearBtn = new ui::Button(this);
    pClearBtn->SetClass(_T("btn_global_color_gray"));
    pClearBtn->SetName(_T("btn_clear_xml_file"));
    pClearBtn->SetText(_T("Clear Preview"));
    pClearBtn->SetAttribute(_T("width"), _T("120"));
    pClearBtn->SetAttribute(_T("height"), _T("30"));
    pClearBtn->SetAttribute(_T("border_round"), _T("3,3"));
    pClearBtn->SetAttribute(_T("margin"), _T("0,0,8,0"));
    pToolRow->AddItem(pClearBtn);

    ui::Button* pBrowseBtn = new ui::Button(this);
    pBrowseBtn->SetClass(_T("btn_global_color_gray"));
    pBrowseBtn->SetName(_T("btn_browse_xml_file"));
    pBrowseBtn->SetText(_T("Open XML File..."));
    pBrowseBtn->SetAttribute(_T("width"), _T("120"));
    pBrowseBtn->SetAttribute(_T("height"), _T("30"));
    pBrowseBtn->SetAttribute(_T("border_round"), _T("3,3"));
    pBrowseBtn->SetAttribute(_T("margin"), _T("0,0,10,0"));
    pToolRow->AddItem(pBrowseBtn);

    // XML preview area
    ui::XmlBox* pXmlBox = new ui::XmlBox(this);
    pXmlBox->SetName(_T("xml_box_test"));
    pXmlBox->SetBkColor(_T("white"));
    pXmlBox->SetAttribute(_T("border_color"), _T("blue"));
    pXmlBox->SetAttribute(_T("border_size"), _T("1"));
    pXmlBox->SetAttribute(_T("border_dash_style"), _T("dash"));
    pXmlBox->SetAttribute(_T("margin"), _T("4,4,4,4"));
    pXmlBox->SetAttribute(_T("mouse_child"), _T("true"));
    pXmlBox->SetAttribute(_T("res_path"), _T("controls"));
    pXmlBox->SetAttribute(_T("xml_file_path"), _T("controls.xml"));
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

