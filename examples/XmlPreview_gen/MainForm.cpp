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

void MainForm::GetCreateWindowAttributes(ui::WindowCreateAttributes& attrs)
{
    // Corresponding to the <Window> attributes in xml_preview.xml
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    attrs.m_bInitSizeDefined = true;
    attrs.m_szInitSize.cx = (int32_t)(rcWork.Width() * 0.75f);
    attrs.m_szInitSize.cy = (int32_t)(rcWork.Height() * 0.75f);
    attrs.m_bShadowAttached = true;
    attrs.m_bShadowAttachedDefined = true;
    attrs.m_bIsLayeredWindow = true;
    attrs.m_bIsLayeredWindowDefined = true;
    attrs.m_rcSizeBox = ui::UiRect(4, 4, 4, 4);
    attrs.m_bSizeBoxDefined = true;
    attrs.m_rcCaption = ui::UiRect(0, 0, 0, 36);
    attrs.m_bCaptionDefined = true;

    // WindowBuilder clamps the initial size to the XML min_size at parse time.
    if (attrs.m_szInitSize.cx < 750) {
        attrs.m_szInitSize.cx = 750;
    }
    if (attrs.m_szInitSize.cy < 500) {
        attrs.m_szInitSize.cy = 500;
    }

    BaseClass::GetCreateWindowAttributes(attrs);
}

void MainForm::PreInitWindow()
{
    BaseClass::PreInitWindow();

    // No layout XML is loaded, so Window::ParseWindowXml attempted to load an
    // empty XML and reset the window resource sub-path; restore it now so
    // resources resolve from the "xml_preview" folder.
    SetResourcePath(ui::FilePath(_T("xml_preview")));
    SetWindowMinimumSize(ui::UiSize(750, 500), true);
}

void MainForm::OnInitWindow()
{
    // Use the OS-provided system shadow on all platforms.
    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);


    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);

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
    auto FillSourceEdits = [pXmlBox]() {
        std::vector<uint8_t> xmlData;
        if (ui::FileUtil::ReadFileData(pXmlBox->GetXmlFileFullPath(), xmlData) && !xmlData.empty()) {
            std::wstring xmlTextW;
            if (ui::StringCharset::GetDataAsString((const char*)xmlData.data(), (uint32_t)xmlData.size(), xmlTextW)) {
                DString xmlText = ui::StringConvert::WStringToT(xmlTextW);
                ui::RichEdit* pSourceEdit = dynamic_cast<ui::RichEdit*>(pXmlBox->FindSubControl(_T("edit")));
                if (pSourceEdit != nullptr) {
                    pSourceEdit->SetText(xmlText);
                }
                pSourceEdit = dynamic_cast<ui::RichEdit*>(pXmlBox->FindSubControl(_T("edit2")));
                if (pSourceEdit != nullptr) {
                    pSourceEdit->SetText(xmlText);
                }
            }
        }
    };
    FillSourceEdits();

    auto FillDemoCombos = [pXmlBox]() {
        ui::Combo* pCombo = dynamic_cast<ui::Combo*>(pXmlBox->FindSubControl(_T("combo")));
        if ((pCombo != nullptr) && (pCombo->GetCount() == 0)) {
            ui::TreeView* pTreeView = pCombo->GetTreeView();
            ui::TreeNode* pTreeNode = pTreeView->GetRootNode();
            for (int32_t i = 0; i < 10; ++i) {
                ui::TreeNode* node = new ui::TreeNode(pXmlBox->GetWindow());
                node->SetClass(_T("tree_node"));
                node->SetText(ui::StringUtil::Printf(_T("ui::Combo::TreeNode %d"), i));
                pTreeNode->AddChildNode(node);
            }
            // Adding items can create a hidden popup window; make sure it is
            // closed so it does not linger behind the preview and intercept
            // clicks.
            if (pCombo->GetComboWnd() != nullptr) {
                pCombo->GetComboWnd()->CloseWnd();
            }
        }

        ui::FilterCombo* pFilterCombo = dynamic_cast<ui::FilterCombo*>(pXmlBox->FindSubControl(_T("filter_combo")));
        if ((pFilterCombo != nullptr) && (pFilterCombo->GetCount() == 0)) {
            for (int32_t i = 0; i < 10; ++i) {
                pFilterCombo->AddTextItem(ui::StringUtil::Printf(_T("Item %d FilterCombo"), i));
            }
        }

        ui::CheckCombo* pCheckCombo = dynamic_cast<ui::CheckCombo*>(pXmlBox->FindSubControl(_T("check_combo")));
        if ((pCheckCombo != nullptr) && (pCheckCombo->GetItemCount() == 0)) {
            pCheckCombo->AddTextItem(_T("Monday"));
            pCheckCombo->AddTextItem(_T("Tuesday"));
            pCheckCombo->AddTextItem(_T("Wednesday"));
            pCheckCombo->AddTextItem(_T("Thursday"));
            pCheckCombo->AddTextItem(_T("Friday"));
            pCheckCombo->AddTextItem(_T("Saturday"));
            pCheckCombo->AddTextItem(_T("Sunday"));
        }
    };
    FillDemoCombos();

    auto FillDemoList = [pXmlBox]() {
        ui::ListBox* pList = dynamic_cast<ui::ListBox*>(pXmlBox->FindSubControl(_T("list")));
        if ((pList != nullptr) && (pList->GetItemCount() <= 1)) {
            for (int32_t i = 0; i < 30; ++i) {
                ui::ListBoxItem* item = new ui::ListBoxItem(pXmlBox->GetWindow());
                item->SetClass(_T("listitem"));
                item->SetText(ui::StringUtil::Printf(_T("ui::VListBox::ListBoxItem %d"), i));
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
                    pXmlFileLoadResult->SetText(_T("OK"));
                }
                FillSourceEdits();
                FillDemoCombos();
                FillDemoList();
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

