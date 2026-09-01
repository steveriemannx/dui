//MainForm.cpp
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

MainForm::MainForm():
    m_nIntervalSeconds(-1),
    m_timerId(0)
{
}

void MainForm::OnInitWindow()
{
    BindEvents();
    BaseClass::OnInitWindow();
}

void MainForm::BindEvents()
{
    ui::XmlBox* pXmlBox = ui::Find<ui::XmlBox>(this, DUI_T("xml_box_test"));
    if (pXmlBox == nullptr) {
        return;
    }
    m_xmlFilePath = pXmlBox->GetXmlFileFullPath();
    ui::Label* pXmlFilePath = ui::Find<ui::Label>(this, DUI_T("xml_file_path"));
    if (pXmlFilePath != nullptr) {
        pXmlFilePath->SetText(pXmlBox->GetXmlFileFullPath().ToString());
    }
    ui::Label* pXmlFileLoadResult = ui::Find<ui::Label>(this, DUI_T("xml_file_load_result"));
    if (pXmlFileLoadResult != nullptr) {
        if (!pXmlBox->GetXmlFileFullPath().IsEmpty()) {
            pXmlFileLoadResult->SetText(DUI_T("OK"));
        }
    }
    auto FillSourceEdits = [pXmlBox]() {
        std::vector<uint8_t> xmlData;
        if (ui::FileUtil::ReadFileData(pXmlBox->GetXmlFileFullPath(), xmlData) && !xmlData.empty()) {
            std::wstring xmlTextW;
            if (ui::StringCharset::GetDataAsString((const char*)xmlData.data(), (uint32_t)xmlData.size(), xmlTextW)) {
                DString xmlText = ui::StringConvert::WStringToT(xmlTextW);
                ui::RichEdit* pSourceEdit = dynamic_cast<ui::RichEdit*>(pXmlBox->FindSubControl(DUI_T("edit")));
                if (pSourceEdit != nullptr) {
                    pSourceEdit->SetText(xmlText);
                }
                pSourceEdit = dynamic_cast<ui::RichEdit*>(pXmlBox->FindSubControl(DUI_T("edit2")));
                if (pSourceEdit != nullptr) {
                    pSourceEdit->SetText(xmlText);
                }
            }
        }
    };
    FillSourceEdits();

    auto FillDemoCombos = [pXmlBox]() {
        ui::Combo* pCombo = dynamic_cast<ui::Combo*>(pXmlBox->FindSubControl(DUI_T("combo")));
        if ((pCombo != nullptr) && (pCombo->GetCount() == 0)) {
            ui::TreeView* pTreeView = pCombo->GetTreeView();
            ui::TreeNode* pTreeNode = pTreeView->GetRootNode();
            for (int32_t i = 0; i < 10; ++i) {
                ui::TreeNode* node = new ui::TreeNode(pXmlBox->GetWindow());
                node->SetClass(DUI_T("tree_node"));
                node->SetText(ui::StringUtil::Printf(DUI_T("ui::Combo::TreeNode %d"), i));
                pTreeNode->AddChildNode(node);
            }
            if (pCombo->GetComboWnd() != nullptr) {
                pCombo->GetComboWnd()->CloseWnd();
            }
        }

        ui::FilterCombo* pFilterCombo = dynamic_cast<ui::FilterCombo*>(pXmlBox->FindSubControl(DUI_T("filter_combo")));
        if ((pFilterCombo != nullptr) && (pFilterCombo->GetCount() == 0)) {
            for (int32_t i = 0; i < 10; ++i) {
                pFilterCombo->AddTextItem(ui::StringUtil::Printf(DUI_T("Item %d FilterCombo"), i));
            }
        }

        ui::CheckCombo* pCheckCombo = dynamic_cast<ui::CheckCombo*>(pXmlBox->FindSubControl(DUI_T("check_combo")));
        if ((pCheckCombo != nullptr) && (pCheckCombo->GetItemCount() == 0)) {
            pCheckCombo->AddTextItem(DUI_T("Monday"));
            pCheckCombo->AddTextItem(DUI_T("Tuesday"));
            pCheckCombo->AddTextItem(DUI_T("Wednesday"));
            pCheckCombo->AddTextItem(DUI_T("Thursday"));
            pCheckCombo->AddTextItem(DUI_T("Friday"));
            pCheckCombo->AddTextItem(DUI_T("Saturday"));
            pCheckCombo->AddTextItem(DUI_T("Sunday"));
        }
    };
    FillDemoCombos();

    auto FillDemoList = [pXmlBox]() {
        ui::ListBox* pList = dynamic_cast<ui::ListBox*>(pXmlBox->FindSubControl(DUI_T("list")));
        if ((pList != nullptr) && (pList->GetItemCount() <= 1)) {
            for (int32_t i = 0; i < 30; ++i) {
                ui::ListBoxItem* item = new ui::ListBoxItem(pXmlBox->GetWindow());
                item->SetClass(DUI_T("listitem"));
                item->SetText(ui::StringUtil::Printf(DUI_T("ui::VListBox::ListBoxItem %d"), i));
                item->SetFixedHeight(ui::UiFixedInt(20), true, true);
                pList->AddItem(item);
            }
        }
    };
    FillDemoList();

    pXmlBox->AddLoadXmlCallback([this, pXmlBox, pXmlFilePath, pXmlFileLoadResult, FillSourceEdits, FillDemoCombos, FillDemoList](const ui::FilePath& xmlPath, bool bSuccess) {
            if (bSuccess) {
                m_xmlFilePath = pXmlBox->GetXmlFileFullPath();
                if (pXmlFilePath != nullptr) {
                    pXmlFilePath->SetText(pXmlBox->GetXmlFileFullPath().ToString());
                }
                if (pXmlFileLoadResult != nullptr) {
                    pXmlFileLoadResult->SetText(DUI_T("OK"));
                }
                FillSourceEdits();
                FillDemoCombos();
                FillDemoList();
            }
            else {
                if (pXmlFileLoadResult != nullptr) {
                    DString errMsg = ui::StringUtil::Printf(DUI_T("Failed [%s]"), xmlPath.ToString().c_str());
                    pXmlFileLoadResult->SetText(errMsg);
                }
            }
        });

    // Clear the preview
    ui::Button* pClearXmlBtn = ui::Find<ui::Button>(this, DUI_T("btn_clear_xml_file"));
    if (pClearXmlBtn != nullptr) {
        pClearXmlBtn->AttachClick([this, pXmlBox, pXmlFilePath, pXmlFileLoadResult](const ui::EventArgs&) {
            // Show the Open XML File dialog
            pXmlBox->ClearXmlBox();
            m_xmlFilePath.Clear();
            if (pXmlFileLoadResult != nullptr) {
                pXmlFileLoadResult->SetText(DUI_T(""));
            }
            if (pXmlFilePath != nullptr) {
                pXmlFilePath->SetText(pXmlBox->GetXmlFileFullPath().ToString());
            }
            return true;
            });
    }

    // Open the XML file
    ui::Button* pBrowseXmlBtn = ui::Find<ui::Button>(this, DUI_T("btn_browse_xml_file"));
    if (pBrowseXmlBtn != nullptr) {
        pBrowseXmlBtn->AttachClick([this, pXmlBox](const ui::EventArgs&) {
            // Show the Open XML File dialog
            ui::FilePath xmlFilePath;
            std::vector<ui::FileDialog::FileType> fileTypes;
            ui::FileDialog::FileType xmlFileType;
            xmlFileType.szName = DUI_T("XML Files");
            xmlFileType.szExt = DUI_T("*.xml");
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
    pXmlBox->SetDropFileTypes(DUI_T(".xml"));
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
    ui::CheckBox* pCheckBoxRefresh = ui::Find<ui::CheckBox>(this, DUI_T("checkbox_auto_refresh"));
    ui::RichEdit* pRefreshInterval = ui::Find<ui::RichEdit>(this, DUI_T("auto_refresh_interval"));
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
                ui::XmlBox* pXmlBox = ui::Find<ui::XmlBox>(this, DUI_T("xml_box_test"));
                if (pXmlBox != nullptr) {
                    pXmlBox->SetXmlFilePath(m_xmlFilePath);
                }
                m_xmlFileData.swap(xmlFileData);
            }
        }
    }
}
