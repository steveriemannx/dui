//MainForm.cpp
#include "MainForm.h"
#include "dui/Utils/UiBuilder.h"

MainForm::MainForm():
    m_nIntervalSeconds(-1),
    m_timerId(0)
{
}

void MainForm::BuildUI()
{
    // Corresponding to the xml_preview.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor("bk_wnd_darkcolor");

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}});
    pCaption->SetBkColor("bk_wnd_lightcolor");
    pRoot->AddItem(pCaption);

    auto* pCaptionLeft = ui::Create<ui::HBox>(this, {{"margin", "0,0,30,0"}, {"valign", "center"}, {"width", "auto"}, {"height", "auto"}, {"mouse_enabled", "false"}});
    pCaption->AddItem(pCaptionLeft);

    auto* pTitle = ui::Create<ui::Label>(this, {{"valign", "center"}, {"margin", "8,0,0,0"}, {"mouse_enabled", "false"}});
    pTitle->SetText("XML File UI Preview Test");
    pCaptionLeft->AddItem(pTitle);

    auto* pSpacer = ui::Create<ui::Control>(this, {{"mouse_enabled", "false"}});
    pCaption->AddItem(pSpacer);

    auto* pCaptionRight = ui::Create<ui::HBox>(this, {{"margin", "0,0,0,0"}, {"valign", "center"}, {"width", "auto"}, {"height", "36"}});
    pCaption->AddItem(pCaptionRight);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pMinBtn->SetClass("btn_wnd_min_11");
    pMinBtn->SetName("minbtn");
    pMinBtn->SetToolTipText("Minimize");
    pCaptionRight->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pCaptionRight->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "stretch"}});
    pMaxBtn->SetClass("btn_wnd_max_11");
    pMaxBtn->SetName("maxbtn");
    pMaxBtn->SetToolTipText("Maximize");
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "stretch"}});
    pRestoreBtn->SetClass("btn_wnd_restore_11");
    pRestoreBtn->SetName("restorebtn");
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText("Restore");
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,0,0,2"}});
    pCloseBtn->SetClass("btn_wnd_close_11");
    pCloseBtn->SetName("closebtn");
    pCloseBtn->SetToolTipText("Close");
    pCaptionRight->AddItem(pCloseBtn);

    // Work area
    auto* pContent = ui::Create<ui::VBox>(this, {{"padding", "10,10,10,10"}});
    pRoot->AddItem(pContent);

    auto* pToolArea = ui::Create<ui::VBox>(this, {{"height", "auto"}});
    pContent->AddItem(pToolArea);

    // XML file path row
    auto* pPathRow = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    pToolArea->AddItem(pPathRow);

    auto* pPathLabel = ui::Create<ui::Label>(this, {{"valign", "center"}});
    pPathLabel->SetText("XML File Path:");
    pPathRow->AddItem(pPathLabel);

    auto* pPathValue = ui::Create<ui::Label>(this, {{"valign", "center"}, {"width", "100%"}, {"path_ellipsis", "true"}, {"single_line", "true"}, {"auto_tooltip", "true"}});
    pPathValue->SetName("xml_file_path");
    pPathRow->AddItem(pPathValue);

    // Load result row
    auto* pResultRow = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    pToolArea->AddItem(pResultRow);

    auto* pResultLabel = ui::Create<ui::Label>(this, {{"valign", "center"}});
    pResultLabel->SetText("XML Load Result:");
    pResultRow->AddItem(pResultLabel);

    auto* pResultValue = ui::Create<ui::Label>(this, {{"valign", "center"}, {"width", "100%"}});
    pResultValue->SetName("xml_file_load_result");
    pResultRow->AddItem(pResultValue);

    // Toolbar row
    auto* pToolRow = ui::Create<ui::HBox>(this, {{"height", "auto"}});
    pToolArea->AddItem(pToolRow);

    auto* pHint = ui::Create<ui::Label>(this, {{"text_align", "vcenter"}, {"valign", "center"}, {"height", "30"}});
    pHint->SetName("xml_file_path");
    pHint->SetText("You can drag an XML file into the display area below to preview it");
    pToolRow->AddItem(pHint);

    auto* pToolSpacer = ui::Create<ui::Control>(this, {});
    pToolRow->AddItem(pToolSpacer);

    auto* pAutoRefresh = ui::Create<ui::CheckBox>(this, {{"margin", "0,0,8,0"}});
    pAutoRefresh->SetClass("checkbox_2");
    pAutoRefresh->SetName("checkbox_auto_refresh");
    pAutoRefresh->SetText("Auto Refresh");
    pAutoRefresh->Selected(false);
    pToolRow->AddItem(pAutoRefresh);

    auto* pIntervalLabel = ui::Create<ui::Label>(this, {{"text_align", "vcenter"}, {"valign", "center"}});
    pIntervalLabel->SetText("Interval (seconds):");
    pToolRow->AddItem(pIntervalLabel);

    auto* pIntervalEdit = ui::Create<ui::RichEdit>(this, {{"min_number", "1"}, {"max_number", "60"}, {"limit_text", "2"}, {"height", "30"}, {"margin", "0,0,8,0"}});
    pIntervalEdit->SetClass("simple simple_border rich_edit_spin");
    pIntervalEdit->SetName("auto_refresh_interval");
    pIntervalEdit->SetText("5");
    pToolRow->AddItem(pIntervalEdit);

    auto* pClearBtn = ui::Create<ui::Button>(this, {{"width", "120"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "0,0,8,0"}});
    pClearBtn->SetClass("btn_global_color_gray");
    pClearBtn->SetName("btn_clear_xml_file");
    pClearBtn->SetText("Clear Preview");
    pToolRow->AddItem(pClearBtn);

    auto* pBrowseBtn = ui::Create<ui::Button>(this, {{"width", "120"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "0,0,10,0"}});
    pBrowseBtn->SetClass("btn_global_color_gray");
    pBrowseBtn->SetName("btn_browse_xml_file");
    pBrowseBtn->SetText("Open XML File...");
    pToolRow->AddItem(pBrowseBtn);

    // XML preview area (inside a scroll container so controls.xml can scroll)
    auto* pScroll = ui::Create<ui::VScrollBox>(this, {{"vscrollbar", "true"}, {"hscrollbar", "true"}, {"width", "stretch"}, {"height", "stretch"}, {"margin", "4,4,4,4"}});
    pScroll->SetName("xml_preview_scroll");
    pContent->AddItem(pScroll);

    auto* pXmlBox = ui::Create<ui::XmlBox>(this, {{"border_color", "splitline_level1"}, {"border_size", "1"}, {"border_dash_style", "dash"}, {"margin", "0"}, {"width", "1100"}, {"height", "800"}, {"mouse_child", "true"}, {"res_path", "controls"}, {"xml_file_path", "controls.xml"}});
    pXmlBox->SetName("xml_box_test");
    pXmlBox->SetBkColor("white");
    pScroll->AddItem(pXmlBox);

    AttachBox(pRoot);
}

void MainForm::SetupWindow()
{
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    int32_t cx = (int32_t)(rcWork.Width() * 0.75f);
    int32_t cy = (int32_t)(rcWork.Height() * 0.75f);
    if (cx < 750) cx = 750;
    if (cy < 500) cy = 500;
    SetWindowSize(cx, cy);
    CenterWindow();

    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    SetSizeBox(ui::UiRect(4, 4, 4, 4), true);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), true);
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();
    BindEvents();
    BaseClass::OnInitWindow();
}

void MainForm::BindEvents()
{
    ui::XmlBox* pXmlBox = ui::Find<ui::XmlBox>(this, "xml_box_test");
    if (pXmlBox == nullptr) {
        return;
    }
    m_xmlFilePath = pXmlBox->GetXmlFileFullPath();
    ui::Label* pXmlFilePath = ui::Find<ui::Label>(this, "xml_file_path");
    if (pXmlFilePath != nullptr) {
        pXmlFilePath->SetText(pXmlBox->GetXmlFileFullPath().ToString());
    }
    ui::Label* pXmlFileLoadResult = ui::Find<ui::Label>(this, "xml_file_load_result");
    if (pXmlFileLoadResult != nullptr) {
        if (!pXmlBox->GetXmlFileFullPath().IsEmpty()) {
            pXmlFileLoadResult->SetText("OK");
        }
    }

    auto FillSourceEdits = [pXmlBox]() {
        std::vector<uint8_t> xmlData;
        if (!ui::FileUtil::ReadFileData(pXmlBox->GetXmlFileFullPath(), xmlData) || xmlData.empty()) {
            if (ui::GlobalManager::Instance().MemoryResources().IsOpen()) {
                ui::GlobalManager::Instance().MemoryResources().GetData(pXmlBox->GetXmlFileFullPath(), xmlData);
            }
        }
        if (!xmlData.empty()) {
            std::wstring xmlTextW;
            if (ui::StringCharset::GetDataAsString((const char*)xmlData.data(), (uint32_t)xmlData.size(), xmlTextW)) {
                DString xmlText = ui::StringConvert::WStringToT(xmlTextW);
                ui::RichEdit* pSourceEdit = dynamic_cast<ui::RichEdit*>(pXmlBox->FindSubControl("edit"));
                if (pSourceEdit != nullptr) {
                    pSourceEdit->SetText(xmlText);
                }
                pSourceEdit = dynamic_cast<ui::RichEdit*>(pXmlBox->FindSubControl("edit2"));
                if (pSourceEdit != nullptr) {
                    pSourceEdit->SetText(xmlText);
                }
            }
        }
    };
    FillSourceEdits();

    auto FillDemoCombos = [pXmlBox]() {
        ui::Combo* pCombo = dynamic_cast<ui::Combo*>(pXmlBox->FindSubControl("combo"));
        if ((pCombo != nullptr) && (pCombo->GetCount() == 0)) {
            ui::TreeView* pTreeView = pCombo->GetTreeView();
            ui::TreeNode* pTreeNode = pTreeView->GetRootNode();
            for (int32_t i = 0; i < 10; ++i) {
                ui::TreeNode* node = new ui::TreeNode(pXmlBox->GetWindow());
                node->SetClass("tree_node");
                node->SetText(ui::StringUtil::Printf("ui::Combo::TreeNode %d", i));
                pTreeNode->AddChildNode(node);
            }
            // Adding items can create a hidden popup window; make sure it is
            // closed so it does not linger behind the preview and intercept
            // clicks.
            if (pCombo->GetComboWnd() != nullptr) {
                pCombo->GetComboWnd()->CloseWnd();
            }
        }

        ui::FilterCombo* pFilterCombo = dynamic_cast<ui::FilterCombo*>(pXmlBox->FindSubControl("filter_combo"));
        if ((pFilterCombo != nullptr) && (pFilterCombo->GetCount() == 0)) {
            for (int32_t i = 0; i < 10; ++i) {
                pFilterCombo->AddTextItem(ui::StringUtil::Printf("Item %d FilterCombo", i));
            }
        }

        ui::CheckCombo* pCheckCombo = dynamic_cast<ui::CheckCombo*>(pXmlBox->FindSubControl("check_combo"));
        if ((pCheckCombo != nullptr) && (pCheckCombo->GetItemCount() == 0)) {
            pCheckCombo->AddTextItem("Monday");
            pCheckCombo->AddTextItem("Tuesday");
            pCheckCombo->AddTextItem("Wednesday");
            pCheckCombo->AddTextItem("Thursday");
            pCheckCombo->AddTextItem("Friday");
            pCheckCombo->AddTextItem("Saturday");
            pCheckCombo->AddTextItem("Sunday");
        }
    };
    FillDemoCombos();

    auto FillDemoList = [pXmlBox]() {
        ui::ListBox* pList = dynamic_cast<ui::ListBox*>(pXmlBox->FindSubControl("list"));
        if ((pList != nullptr) && (pList->GetItemCount() <= 1)) {
            for (int32_t i = 0; i < 30; ++i) {
                ui::ListBoxItem* item = new ui::ListBoxItem(pXmlBox->GetWindow());
                item->SetClass("listitem");
                item->SetText(ui::StringUtil::Printf("ui::VListBox::ListBoxItem %d", i));
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
                    pXmlFileLoadResult->SetText("OK");
                }
                FillSourceEdits();
                FillDemoCombos();
                FillDemoList();
            }
            else {
                if (pXmlFileLoadResult != nullptr) {
                    DString errMsg = ui::StringUtil::Printf("Failed [%s]", xmlPath.ToString().c_str());
                    pXmlFileLoadResult->SetText(errMsg);
                }
            }
        });

    // Clear the preview
    ui::Button* pClearXmlBtn = ui::Find<ui::Button>(this, "btn_clear_xml_file");
    if (pClearXmlBtn != nullptr) {
        pClearXmlBtn->AttachClick([this, pXmlBox, pXmlFilePath, pXmlFileLoadResult](const ui::EventArgs&) {
            // Show the Open XML File dialog
            pXmlBox->ClearXmlBox();
            m_xmlFilePath.Clear();
            if (pXmlFileLoadResult != nullptr) {
                pXmlFileLoadResult->SetText("");
            }
            if (pXmlFilePath != nullptr) {
                pXmlFilePath->SetText(pXmlBox->GetXmlFileFullPath().ToString());
            }
            return true;
            });
    }

    // Open the XML file
    ui::Button* pBrowseXmlBtn = ui::Find<ui::Button>(this, "btn_browse_xml_file");
    if (pBrowseXmlBtn != nullptr) {
        pBrowseXmlBtn->AttachClick([this, pXmlBox](const ui::EventArgs&) {
            // Show the Open XML File dialog
            ui::FilePath xmlFilePath;
            std::vector<ui::FileDialog::FileType> fileTypes;
            ui::FileDialog::FileType xmlFileType;
            xmlFileType.szName = "XML Files";
            xmlFileType.szExt = "*.xml";
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
    pXmlBox->SetDropFileTypes(".xml");
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
    ui::CheckBox* pCheckBoxRefresh = ui::Find<ui::CheckBox>(this, "checkbox_auto_refresh");
    ui::RichEdit* pRefreshInterval = ui::Find<ui::RichEdit>(this, "auto_refresh_interval");
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
                ui::XmlBox* pXmlBox = ui::Find<ui::XmlBox>(this, "xml_box_test");
                if (pXmlBox != nullptr) {
                    pXmlBox->SetXmlFilePath(m_xmlFilePath);
                }
                m_xmlFileData.swap(xmlFileData);
            }
        }
    }
}
