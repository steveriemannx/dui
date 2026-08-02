//MainForm.cpp
#include "MainForm.h"
#include "generated_ui.inc"  // Build-time generated pure C++ UI code

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
    return _T("xml_preview");
}

DString MainForm::GetSkinFile()
{
    // No XML file - UI is generated at build time
    return _T("");
}

void MainForm::OnInitWindow()
{
    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);

    // Build-time generated from xml_preview.xml
    InitXml_preview(this);

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

