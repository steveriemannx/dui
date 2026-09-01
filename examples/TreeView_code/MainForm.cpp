#include "MainForm.h"
#include "ComputerView.h"
#include "SimpleFileView.h"
#include "ExplorerView.h"
#include "dui/Utils/UiBuilder.h"

static void BuildUIFromXml(ui::Window* pWindow);

MainForm::MainForm():
    m_pTree(nullptr),
    m_pAddressBar(nullptr),
    m_pBtnUp(nullptr),
    m_pBtnForward(nullptr),
    m_pBtnBack(nullptr),
    m_bCanAddBackForward(true),
    m_pTreeNode(nullptr),
    m_pTabBox(nullptr),
    m_pBtnViewListType(nullptr),
    m_pBtnViewSort(nullptr),
    m_dataViewType(DataViewType::kReprortView),
    m_tabBoxViewType(TabBoxViewType::kComputerView)
{
}

MainForm::~MainForm()
{
}

void MainForm::SetupWindow()
{
    ui::UiRect rcWork;
    ui::WindowBase::GetPrimaryMonitorWorkRect(rcWork);
    SetWindowSize((int32_t)(rcWork.Width() * 0.80f), (int32_t)(rcWork.Height() * 0.80f));
    CenterWindow();

    SetShadowAttached(true);
    SetShadowType(ui::Shadow::ShadowType::kShadowSystemDefault);
    SetLayeredWindow(false, false);
    SetEnableShadowSnap(true);
    SetShadowBorderSize(0);

    SetSizeBox(ui::UiRect(4, 4, 4, 4), false);
    SetCaptionRect(ui::UiRect(0, 0, 0, 36), false);
}

void MainForm::BuildUI()
{
    BuildUIFromXml(this);
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();
    BindEvents();

    m_pTree = ui::Find<ui::DirectoryTree>(this, "tree");
    ASSERT(m_pTree != nullptr);
    if (m_pTree == nullptr) {
        return;
    }
    m_pAddressBar = ui::Find<ui::AddressBar>(this, "file_path");
    if (m_pAddressBar != nullptr) {
        m_pAddressBar->AttachPathChanged(UiBind(&MainForm::OnAddressBarPathChanged, this, std::placeholders::_1));
        m_pAddressBar->AttachPathClick(UiBind(&MainForm::OnAddressBarPathClick, this, std::placeholders::_1));
    }
    m_pTabBox = ui::Find<ui::TabBox>(this, "main_view_tab_box");
    ui::ListCtrl* pComputerListCtrl = ui::Find<ui::ListCtrl>(this, "computer_view");
    m_pComputerView = std::make_unique<ComputerView>(this, pComputerListCtrl);
    ui::VirtualListBox* pListBox = ui::Find<ui::VirtualListBox>(this, "simple_file_view");
    m_pSimpleFileView = std::make_unique<SimpleFileView>(this, pListBox);
    ui::ListCtrl* pExplorerListCtrl = ui::Find<ui::ListCtrl>(this, "explorer_view");
    m_pExplorerView = std::make_unique<ExplorerView>(this, pExplorerListCtrl);

    // Up button
    m_pBtnUp = ui::Find<ui::Button>(this, "btn_view_up");
    // Back button
    m_pBtnBack = ui::Find<ui::Button>(this, "btn_view_left");
    // Forward button
    m_pBtnForward = ui::Find<ui::Button>(this, "btn_view_right");
    // Switch view mode
    m_pBtnViewListType = ui::Find<ui::ButtonHBox>(this, "btn_view_list_type");
    // Switch sort mode
    m_pBtnViewSort = ui::Find<ui::ButtonHBox>(this, "btn_view_sort");

    UpdateCommandUI();

    // Show the virtual path
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kUserHome, "Home Folder");
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kDesktop, "Desktop");
    ui::TreeNode* pDocumentsNode = m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kDocuments, "Document");
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kPictures, "Image");
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kMusic, "Music");
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kVideos, "Video");
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kDownloads, "Download");

    // Show disks
    ui::TreeNode* pComputerNode = m_pTree->ShowAllDiskNodes("Computer", "File System");
    if (pComputerNode != nullptr) {
        // Put a horizontal separator in front of the disks
        m_pTree->InsertLineBeforeNode(pComputerNode);
    }

    // The "Computer" view is the default at startup
    if (pComputerNode != nullptr) {
        m_pTree->SelectTreeNode(pComputerNode);
    }
    BaseClass::OnInitWindow();
}

void MainForm::BindEvents()
{
    // Refresh button
    if (auto* pRefreshBtn = ui::Find<ui::Button>(this, "btn_view_refresh")) {
        pRefreshBtn->AttachClick([this](const ui::EventArgs&) {
            Refresh();
            return true;
            });
    }

    // Up button
    if (m_pBtnUp != nullptr) {
        m_pBtnUp->AttachClick([this](const ui::EventArgs&) {
            ShowUp();
            return true;
            });
    }

    // Back button
    if (m_pBtnBack != nullptr) {
        m_pBtnBack->AttachClick([this](const ui::EventArgs&) {
            ShowBack();
            return true;
            });
    }

    // Forward button
    if (m_pBtnForward != nullptr) {
        m_pBtnForward->AttachClick([this](const ui::EventArgs&) {
            ShowForward();
            return true;
            });
    }

    // Switch view mode
    if (m_pBtnViewListType != nullptr) {
        m_pBtnViewListType->AttachClick([this](const ui::EventArgs& args) {
            ui::UiRect rect = args.GetSender()->GetPos();
            ui::UiPoint point;
            point.x = rect.left;
            point.y = rect.bottom;
            ClientToScreen(point);
            point.y += Dpi().GetScaleInt(4);
            SwithListType(point, m_pBtnViewListType);
            return true;
            });
    }

    // Switch sort mode
    if (m_pBtnViewSort != nullptr) {
        m_pBtnViewSort->AttachClick([this](const ui::EventArgs& args) {
            ui::UiRect rect = args.GetSender()->GetPos();
            ui::UiPoint point;
            point.x = rect.left;
            point.y = rect.bottom;
            ClientToScreen(point);
            point.y += Dpi().GetScaleInt(4);
            SwithSortMode(point, m_pBtnViewSort);
            return true;
            });
    }

    if (m_pTree != nullptr) {
        // Bind events
        m_pTree->AttachShowMyComputerContents(ui::UiBind(&MainForm::OnShowMyComputerContents, this, std::placeholders::_1, std::placeholders::_2));
        m_pTree->AttachShowFolderContents(ui::UiBind(&MainForm::OnShowFolderContents, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        ui::StdClosure finishCallback = ToWeakCallback([this]() {
            OnRefresh();
            });
        m_pTree->SetRefreshFinishCallback(finishCallback);
    }
}

void MainForm::Refresh()
{
    if (m_pTree == nullptr) {
        return;
    }
    ui::StdClosure finishCallback = ToWeakCallback([this]() {
            OnRefresh();
        });
    m_pTree->RefreshTree(finishCallback);
}

void MainForm::SetShowTreeNode(ui::TreeNode* pTreeNode)
{
    if (!m_pTree->IsValidTreeNode(pTreeNode)) {
        return;
    }
    if (m_bCanAddBackForward && m_pTree->IsValidTreeNode(m_pTreeNode)) {
        m_backStack.push(m_pTreeNode);
        m_forwardStack = std::stack<ui::TreeNode*>();
    }
    m_pTreeNode = pTreeNode;
    m_parentTreeNodes.clear();
    if (pTreeNode != nullptr) {
        ui::TreeNode* p = pTreeNode->GetParentNode();
        while (p != nullptr) {
            m_parentTreeNodes.push_back(p);
            p = p->GetParentNode();
            if (p == m_pTree->GetRootNode()) {
                break;
            }
        }
    }
    if (!m_parentTreeNodes.empty()) {
        std::reverse(m_parentTreeNodes.begin(), m_parentTreeNodes.end());
    }    
    m_bCanAddBackForward = true;
}

void MainForm::OnShowFolderContents(ui::TreeNode* pTreeNode, const ui::FilePath& currentPath,
                                    const std::shared_ptr<std::vector<ui::DirectoryTree::PathInfo>>& folderList,
                                    const std::shared_ptr<std::vector<ui::DirectoryTree::PathInfo>>& fileList)
{
    ui::GlobalManager::Instance().AssertUIThread();
    if ((pTreeNode == nullptr) || (m_pTree == nullptr) || !m_pTree->IsValidTreeNode(pTreeNode)) {
        if (folderList != nullptr) {
            ui::DirectoryTree::ClearPathInfoList(*fileList);
        }
        if (folderList != nullptr) {
            ui::DirectoryTree::ClearPathInfoList(*fileList);
        }
        return;
    }
    if (m_pAddressBar != nullptr) {
        m_pAddressBar->SetAddressPath(currentPath.ToString());
    }
    std::vector<ui::DirectoryTree::PathInfo> pathList;
    if (folderList != nullptr) {
        pathList = *folderList;
    }
    if (fileList != nullptr) {
        pathList.insert(pathList.end(), fileList->begin(), fileList->end());
    }
    if (!m_pTree->IsMultiSelect()) {
        // Single selection, with validation
        if (pTreeNode->IsSelected()) {
            SetShowTreeNode(pTreeNode);
        }
    }
    else {
        // Multi-selection, without validation
        SetShowTreeNode(pTreeNode);
    }

    if (m_dataViewType == kPictureView) {
        SwitchToTabBoxViewType(TabBoxViewType::kFileView);
        if (m_pSimpleFileView != nullptr) {
            // Keep the path of the current view and the selected path
            ui::FilePath oldCurrentPath;
            ui::FilePath oldSelectedPath;
            m_pSimpleFileView->GetCurrentPath(oldCurrentPath, oldSelectedPath);
            if (!oldCurrentPath.IsEmpty()) {
                m_selectedPathInfo[oldCurrentPath] = oldSelectedPath;
            }
            ui::FilePath selectedPath;
            auto iter = m_selectedPathInfo.find(currentPath);
            if (iter != m_selectedPathInfo.end()) {
                selectedPath = iter->second;
            }
            m_pSimpleFileView->SetFileList(currentPath, pathList, selectedPath);
        }
    }
    else {
        SwitchToTabBoxViewType(TabBoxViewType::kExplorerView);
        if (m_pExplorerView != nullptr) {
            // Keep the path of the current view and the selected path
            ui::FilePath oldCurrentPath;
            ui::FilePath oldSelectedPath;
            m_pExplorerView->GetCurrentPath(oldCurrentPath, oldSelectedPath);
            if (!oldCurrentPath.IsEmpty()) {
                m_selectedPathInfo[oldCurrentPath] = oldSelectedPath;
            }
            ui::FilePath selectedPath;
            auto iter = m_selectedPathInfo.find(currentPath);
            if (iter != m_selectedPathInfo.end()) {
                selectedPath = iter->second;
            }
            m_pExplorerView->SetFileList(currentPath, pathList, selectedPath);
        }
    }
    // Update the UI state
    UpdateCommandUI();
}

void MainForm::OnShowMyComputerContents(ui::TreeNode* pTreeNode,
                                        const std::vector<ui::DirectoryTree::DiskInfo>& diskInfoList)
{
    ui::GlobalManager::Instance().AssertUIThread();
    if ((pTreeNode == nullptr) || (m_pTree == nullptr) || !m_pTree->IsValidTreeNode(pTreeNode)) {
        std::vector<ui::DirectoryTree::DiskInfo> tempDiskInfoList(diskInfoList);
        ui::DirectoryTree::ClearDiskInfoList(tempDiskInfoList);
        return;
    }
    SwitchToTabBoxViewType(TabBoxViewType::kComputerView);
    if (m_pAddressBar != nullptr) {
        m_pAddressBar->SetAddressPath("");
    }

    // Show the content of the "Computer" view
    if (m_pComputerView != nullptr) {
        m_pComputerView->ShowMyComputerContents(diskInfoList);
    }   

    // Save the displayed tree node
    SetShowTreeNode(pTreeNode);   

    // Update the UI state
    UpdateCommandUI();
}

void MainForm::SelectSubPath(const ui::FilePath& filePath)
{
    if (!filePath.IsExistsDirectory()) {
        // Report an error if the folder does not exist
        DString errMsg = "Path does not exist:";
        errMsg += filePath.ToString();
        ui::SystemUtil::ShowMessageBox(this, errMsg.c_str(), "Error Info");
        return;
    }

    if (m_pTree != nullptr) {
        m_pTree->SelectSubPath(m_pTreeNode, filePath, nullptr);
    }
}

bool MainForm::OnAddressBarPathChanged(const ui::EventArgs& msg)
{
    if (m_pAddressBar != nullptr) {
        DString text = m_pAddressBar->GetAddressPath();
        if (!OnShowAddressPath(text)) {
            m_pAddressBar->SetAddressPath(m_pAddressBar->GetPreviousAddressPath());
        }
    }
    return true;
}

bool MainForm::OnAddressBarPathClick(const ui::EventArgs& msg)
{
    if (m_pAddressBar != nullptr) {
        DString text = m_pAddressBar->GetClickedAddressPath();
        OnShowAddressPath(text);
    }
    return true;
}

bool MainForm::OnShowAddressPath(const DString& newFilePath)
{
    DString text = newFilePath;
    ui::StringUtil::Trim(text);
    if (text.empty()) {
        return false;
    }
    ui::FilePath curFilePath;// The directory corresponding to the current tree node
    ui::TreeNode* pTreeNode = m_pTreeNode;
    if (pTreeNode != nullptr) {
        curFilePath = m_pTree->FindTreeNodePath(pTreeNode);
    }
    ui::FilePath inputFilePath(text); // The currently entered directory
    ui::TreeNode* pParentTreeNode = nullptr;
    if (!inputFilePath.IsAbsolutePath()) {
        // If it is a relative path, concatenate it with the path of the current tree node
        if (!curFilePath.IsEmpty()) {
            inputFilePath = curFilePath.JoinFilePath(inputFilePath);
            pParentTreeNode = m_pTreeNode;
        }
    }
    else if(!curFilePath.IsEmpty() && inputFilePath.IsSubDirectory(curFilePath)) {
        // The newly entered directory is under the current tree node's directory
        pParentTreeNode = m_pTreeNode;
    }
    if (inputFilePath.IsAbsolutePath() && inputFilePath.IsExistsDirectory()) {
        // The address bar contains a valid path; expand the corresponding path in the left tree and select it
        if (m_pTree != nullptr) {
            if (pParentTreeNode != nullptr) {
                m_pTree->SelectSubPath(pParentTreeNode, inputFilePath, nullptr);
            }
            else {
                m_pTree->SelectPath(inputFilePath, nullptr);
            }
        }
        return true;
    }
    else {
        // Report an error if the folder does not exist
        DString errMsg = "The input path does not exist:";
        errMsg += text;
        ui::SystemUtil::ShowMessageBox(this, errMsg.c_str(), "Error Info");
        return false;
    }
}

void MainForm::OnRefresh()
{
    if (m_pTree == nullptr) {
        return;
    }
    ui::TreeNode* pTreeNode = m_pTreeNode;
    if (!m_pTree->IsValidTreeNode(pTreeNode)) {
        // The tree node has been deleted
        pTreeNode = nullptr;
    }
    if (pTreeNode == nullptr) {
        // If the currently displayed node is deleted, find its parent node
        int32_t nCount = (int32_t)m_parentTreeNodes.size();
        for (int32_t nIndex = nCount - 1; nIndex >= 0; --nIndex) {
            ui::TreeNode* pParentTreeNode = m_parentTreeNodes[nIndex];
            if (m_pTree->IsValidTreeNode(pParentTreeNode)) {
                if (m_pTree->IsMyComputerNode(pParentTreeNode)) {
                    pTreeNode = pParentTreeNode;
                    break;
                }
                else {
                    ui::FilePath filePath = m_pTree->FindTreeNodePath(pParentTreeNode);
                    if (filePath.IsExistsDirectory()) {
                        pTreeNode = pParentTreeNode;
                        break;
                    }
                }
            }
        }
    }
    ASSERT(pTreeNode != nullptr);
    if ((m_pTree != nullptr) && (pTreeNode != nullptr)) {
        m_bCanAddBackForward = false;
        m_pTree->SelectTreeNode(pTreeNode);
    }
}

void MainForm::ShowUp()
{
    if (m_pTreeNode != nullptr) {
        ui::TreeNode* pTreeNode = m_pTreeNode->GetParentNode();
        if ((m_pTree != nullptr) && (pTreeNode != nullptr)) {
            m_bCanAddBackForward = true;
            m_pTree->SelectTreeNode(pTreeNode);
        }
    }
}

void MainForm::ShowBack()
{
    // Go back
    if (m_pTree == nullptr) {
        return;
    }
    if (m_pTree->IsValidTreeNode(m_pTreeNode)) {
        m_forwardStack.push(m_pTreeNode);
    }

    while (!m_backStack.empty()) {
        ui::TreeNode* pTreeNode = m_backStack.top();
        m_backStack.pop();
        if (m_pTree->IsValidTreeNode(pTreeNode)) {
            m_bCanAddBackForward = false;
            m_pTree->SelectTreeNode(pTreeNode);
            break;
        }
    }
}

void MainForm::ShowForward()
{
    if (m_pTree == nullptr) {
        return;
    }
    if (m_pTree->IsValidTreeNode(m_pTreeNode)) {
        m_backStack.push(m_pTreeNode);
    }

    while (!m_forwardStack.empty()) {
        ui::TreeNode* pTreeNode = m_forwardStack.top();
        m_forwardStack.pop();
        if (m_pTree->IsValidTreeNode(pTreeNode)) {
            m_bCanAddBackForward = false;
            m_pTree->SelectTreeNode(pTreeNode);
            break;
        }
    }
}

void MainForm::SwithListType(const ui::UiPoint& point, ui::Control* pRelatedControl)
{
    ui::Menu* menu = new ui::Menu(this, pRelatedControl);// Need to set the parent window, otherwise the program becomes inactive when the menu pops up
    // Pure code menu: no XML template, menu items are added by code (corresponds to list_type_menu.xml)
    menu->ShowMenu("", point);
    {
        // Add menu items (with icons and text, structure corresponds to list_type_menu.xml)
        struct ListTypeItem { DString name; DString btnName; DString image; DString text; };
        const ListTypeItem items[] = {
            { "menu_item_icon_big", "btn_menu_item_icon_big", "data_icons_display-symbolic.svg", "Icon View (Large Icons)" },
            { "menu_item_icon_medium", "btn_menu_item_icon_medium", "data_icons_display-symbolic.svg", "Icon View (Medium Icons)" },
            { "menu_item_icon_small", "btn_menu_item_icon_small", "data_icons_display-symbolic.svg", "Icon View (Small Icons)" },
            { "menu_item_list_big", "btn_menu_item_list_big", "view-list-symbolic.svg", "List View (Large Icons)" },
            { "menu_item_list_medium", "btn_menu_item_list_medium", "view-list-symbolic.svg", "List View (Medium Icons)" },
            { "menu_item_list_small", "btn_menu_item_list_small", "view-list-symbolic.svg", "List View (Small Icons)" },
            { "menu_item_report", "btn_menu_item_report", "view-list-compact-symbolic.svg", "Detail View" },
            { "menu_item_picture", "btn_menu_item_picture", "view-list-images-symbolic.svg", "Picture List View" },
        };
        for (const auto& item : items) {
            ui::MenuItem* pMenuItem = new ui::MenuItem(menu);
            pMenuItem->SetClass("menu_element");
            pMenuItem->SetName(item.name);
            pMenuItem->SetFixedWidth(ui::UiFixedInt(220), true, true);

            ui::HBox* pIconBox = new ui::HBox(menu);
            pIconBox->SetAttribute("width", "44");
            pIconBox->SetAttribute("mouse_enabled", "false");
            pIconBox->SetAttribute("keyboard_enabled", "false");
            pMenuItem->AddItem(pIconBox);

            ui::Button* pIconBtn = new ui::Button(menu);
            pIconBtn->SetName(item.btnName);
            pIconBtn->SetAttribute("width", "16");
            pIconBtn->SetAttribute("height", "16");
            pIconBtn->SetAttribute("valign", "center");
            pIconBtn->SetAttribute("margin", "0,0,8,0");
            pIconBtn->SetAttribute("mouse_enabled", "false");
            pIconBtn->SetAttribute("keyboard_enabled", "false");
            pIconBox->AddItem(pIconBtn);

            ui::Button* pImageBtn = new ui::Button(menu);
            pImageBtn->SetBkImage(item.image);
            pImageBtn->SetAttribute("width", "16");
            pImageBtn->SetAttribute("height", "16");
            pImageBtn->SetAttribute("valign", "center");
            pImageBtn->SetAttribute("mouse_enabled", "false");
            pImageBtn->SetAttribute("keyboard_enabled", "false");
            pIconBox->AddItem(pImageBtn);

            ui::Label* pLabel = new ui::Label(menu);
            pLabel->SetClass("menu_text");
            pLabel->SetText(item.text);
            pLabel->SetAttribute("margin", "50,0,0,0");
            pLabel->SetAttribute("mouse_enabled", "false");
            pLabel->SetAttribute("keyboard_enabled", "false");
            pMenuItem->AddItem(pLabel);

            menu->AddMenuItem(pMenuItem);
        }
    }

    std::map<DataViewType, DString> btnNameMap;
    btnNameMap[DataViewType::kIconViewBig] = "btn_menu_item_icon_big";
    btnNameMap[DataViewType::kIconViewMedium] = "btn_menu_item_icon_medium";
    btnNameMap[DataViewType::kIconViewSmall] = "btn_menu_item_icon_small";
    btnNameMap[DataViewType::kListViewBig] = "btn_menu_item_list_big";
    btnNameMap[DataViewType::kListViewMedium] = "btn_menu_item_list_medium";
    btnNameMap[DataViewType::kListViewSmall] = "btn_menu_item_list_small";
    btnNameMap[DataViewType::kReprortView] = "btn_menu_item_report";
    btnNameMap[DataViewType::kPictureView] = "btn_menu_item_picture";

    DString selectBtnName = btnNameMap[GetDataViewType()];
    ui::Button* pSelectBtn = ui::Find<ui::Button>(menu, selectBtnName);
    if (pSelectBtn != nullptr) {
        pSelectBtn->SetBkImage("ui-item-symbolic.svg");
    }

    // Bind the menu item selection event
    menu->AttachMenuItemActivated([this](const DString& /*menuName*/, int32_t /*nMenuLevel*/,
                                         const DString& itemName, size_t /*nItemIndex*/) {
            // Matches the menu item names in the XML
            std::map<DataViewType, DString> itemNameMap;
            itemNameMap[DataViewType::kIconViewBig] = "menu_item_icon_big";
            itemNameMap[DataViewType::kIconViewMedium] = "menu_item_icon_medium";
            itemNameMap[DataViewType::kIconViewSmall] = "menu_item_icon_small";
            itemNameMap[DataViewType::kListViewBig] = "menu_item_list_big";
            itemNameMap[DataViewType::kListViewMedium] = "menu_item_list_medium";
            itemNameMap[DataViewType::kListViewSmall] = "menu_item_list_small";
            itemNameMap[DataViewType::kReprortView] = "menu_item_report";
            itemNameMap[DataViewType::kPictureView] = "menu_item_picture";
            for (auto iter : itemNameMap) {
                if (iter.second == itemName) {
                    DataViewType dataViewType = iter.first;
                    SwitchToDataViewType(dataViewType);
                    break;
                }
            }
        });
}

void MainForm::SwithSortMode(const ui::UiPoint& point, ui::Control* pRelatedControl)
{
    if (m_pExplorerView == nullptr) {
        return;
    }
    ui::Menu* menu = new ui::Menu(this, pRelatedControl);// Need to set the parent window, otherwise the program becomes inactive when the menu pops up
    // Pure code menu: no XML template, menu items are added by code (corresponds to sort_mode_menu.xml)
    menu->ShowMenu("", point);
    {
        // Add menu items (with icon buttons and text, structure corresponds to sort_mode_menu.xml)
        struct SortModeItem { DString name; DString btnName; DString text; };
        const SortModeItem items[] = {
            { "menu_item_file_name", "btn_file_name", "File Name" },
            { "menu_item_file_modify_time", "btn_file_modify_time", "Modified Date" },
            { "menu_item_file_type", "btn_file_type", "File Type" },
            { "menu_item_file_size", "btn_file_size", "File Size" },
        };
        for (const auto& item : items) {
            ui::MenuItem* pMenuItem = new ui::MenuItem(menu);
            pMenuItem->SetClass("menu_element");
            pMenuItem->SetName(item.name);
            pMenuItem->SetFixedWidth(ui::UiFixedInt(160), true, true);

            ui::Button* pIconBtn = new ui::Button(menu);
            pIconBtn->SetName(item.btnName);
            pIconBtn->SetAttribute("width", "auto");
            pIconBtn->SetAttribute("height", "auto");
            pIconBtn->SetAttribute("valign", "center");
            pIconBtn->SetAttribute("mouse_enabled", "false");
            pIconBtn->SetAttribute("keyboard_enabled", "false");
            pMenuItem->AddItem(pIconBtn);

            ui::Label* pLabel = new ui::Label(menu);
            pLabel->SetClass("menu_text");
            pLabel->SetText(item.text);
            pLabel->SetAttribute("margin", "30,0,0,0");
            pLabel->SetAttribute("mouse_enabled", "false");
            pLabel->SetAttribute("keyboard_enabled", "false");
            pMenuItem->AddItem(pLabel);

            menu->AddMenuItem(pMenuItem);
        }

        // Separator
        {
            ui::Box* pSplitBox = new ui::Box(menu);
            pSplitBox->SetClass("menu_split_box");
            pSplitBox->SetAttribute("margin", "0,4,0,4");
            ui::Control* pSplitLine = new ui::Control(menu);
            pSplitLine->SetClass("menu_split_line");
            pSplitLine->SetAttribute("mouse_enabled", "false");
            pSplitLine->SetAttribute("keyboard_enabled", "false");
            pSplitBox->AddItem(pSplitLine);
            // Note: the separator is for display only and is not added as a menu item
            ui::MenuItem* pSortAsc = new ui::MenuItem(menu);
            pSortAsc->SetClass("menu_element");
            pSortAsc->SetName("menu_item_sort_ascending");
            pSortAsc->SetFixedWidth(ui::UiFixedInt(160), true, true);
            ui::Button* pAscBtn = new ui::Button(menu);
            pAscBtn->SetName("btn_sort_ascending");
            pAscBtn->SetAttribute("width", "auto");
            pAscBtn->SetAttribute("height", "auto");
            pAscBtn->SetAttribute("valign", "center");
            pAscBtn->SetAttribute("mouse_enabled", "false");
            pAscBtn->SetAttribute("keyboard_enabled", "false");
            pSortAsc->AddItem(pAscBtn);
            ui::Label* pAscLabel = new ui::Label(menu);
            pAscLabel->SetClass("menu_text");
            pAscLabel->SetText("Ascending Sort");
            pAscLabel->SetAttribute("margin", "30,0,0,0");
            pAscLabel->SetAttribute("mouse_enabled", "false");
            pAscLabel->SetAttribute("keyboard_enabled", "false");
            pSortAsc->AddItem(pAscLabel);
            menu->AddMenuItem(pSortAsc);

            ui::MenuItem* pSortDesc = new ui::MenuItem(menu);
            pSortDesc->SetClass("menu_element");
            pSortDesc->SetName("menu_item_sort_descending");
            pSortDesc->SetFixedWidth(ui::UiFixedInt(160), true, true);
            ui::Button* pDescBtn = new ui::Button(menu);
            pDescBtn->SetName("btn_sort_descending");
            pDescBtn->SetAttribute("width", "auto");
            pDescBtn->SetAttribute("height", "auto");
            pDescBtn->SetAttribute("valign", "center");
            pDescBtn->SetAttribute("mouse_enabled", "false");
            pDescBtn->SetAttribute("keyboard_enabled", "false");
            pSortDesc->AddItem(pDescBtn);
            ui::Label* pDescLabel = new ui::Label(menu);
            pDescLabel->SetClass("menu_text");
            pDescLabel->SetText("Descending Sort");
            pDescLabel->SetAttribute("margin", "30,0,0,0");
            pDescLabel->SetAttribute("mouse_enabled", "false");
            pDescLabel->SetAttribute("keyboard_enabled", "false");
            pSortDesc->AddItem(pDescLabel);
            menu->AddMenuItem(pSortDesc);
        }
    }

    // Get the sort order
    ExplorerView::ExplorerViewColumn sortColumn;
    bool bSortUp = false;
    bool bSorted = m_pExplorerView->GetSortColumnInfo(sortColumn, bSortUp);
    if (bSorted) {
        ui::Button* pSortColumnBtn = nullptr;
        if (sortColumn == ExplorerView::ExplorerViewColumn::kName) {
            pSortColumnBtn = ui::Find<ui::Button>(menu, "btn_file_name");
        }
        else if (sortColumn == ExplorerView::ExplorerViewColumn::kModifyDateTime) {
            pSortColumnBtn = ui::Find<ui::Button>(menu, "btn_file_modify_time");
        }
        else if (sortColumn == ExplorerView::ExplorerViewColumn::kType) {
            pSortColumnBtn = ui::Find<ui::Button>(menu, "btn_file_type");
        }
        else if (sortColumn == ExplorerView::ExplorerViewColumn::kSize) {
            pSortColumnBtn = ui::Find<ui::Button>(menu, "btn_file_size");
        }

        ui::Button* pSortBtn = nullptr;
        if (bSortUp) {
            // Ascending
            pSortBtn = ui::Find<ui::Button>(menu, "btn_sort_ascending");
        }
        else {
            // Descending
            pSortBtn = ui::Find<ui::Button>(menu, "btn_sort_descending");
        }
        if (pSortBtn != nullptr) {
            pSortBtn->SetBkImage("ui-item-symbolic.svg");
        }
        if (pSortColumnBtn != nullptr) {
            pSortColumnBtn->SetBkImage("ui-item-symbolic.svg");
        }
    }
    else {
        sortColumn = ExplorerView::ExplorerViewColumn::kName;
        bSortUp = false;
    }

    // Bind the menu item selection event
    menu->AttachMenuItemActivated([this, bSorted, bSortUp, sortColumn](const DString& menuName, int32_t nMenuLevel,
                                                                       const DString& itemName, size_t nItemIndex) {
            // Matches the menu item names in the XML
            if (itemName == "menu_item_file_name") {
                // File name
                if (m_pExplorerView != nullptr) {
                    m_pExplorerView->SortByColumn(ExplorerView::ExplorerViewColumn::kName, bSortUp);
                }
            }
            else if (itemName == "menu_item_file_modify_time") {
                // Modified date
                if (m_pExplorerView != nullptr) {
                    m_pExplorerView->SortByColumn(ExplorerView::ExplorerViewColumn::kModifyDateTime, bSortUp);
                }
            }
            else if (itemName == "menu_item_file_type") {
                // File type
                if (m_pExplorerView != nullptr) {
                    m_pExplorerView->SortByColumn(ExplorerView::ExplorerViewColumn::kType, bSortUp);
                }
            }
            else if (itemName == "menu_item_file_size") {
                // File size
                if (m_pExplorerView != nullptr) {
                    m_pExplorerView->SortByColumn(ExplorerView::ExplorerViewColumn::kSize, bSortUp);
                }
            }
            else if (itemName == "menu_item_sort_ascending") {
                // Ascending sort
                if (!bSorted || !bSortUp) {
                    if (m_pExplorerView != nullptr) {
                        m_pExplorerView->SortByColumn(sortColumn, true);
                    }
                }
            }
            else if (itemName == "menu_item_sort_descending") {
                // Descending sort
                if (!bSorted || bSortUp) {
                    if (m_pExplorerView != nullptr) {
                        m_pExplorerView->SortByColumn(sortColumn, false);
                    }
                }
            }
        });
}

void MainForm::UpdateCommandUI()
{
    bool bEnableUp = false;
    bool bEnableBack = !m_backStack.empty();
    bool bEnableForward = !m_forwardStack.empty();
    if ((m_pTreeNode != nullptr) && (m_pTree != nullptr)) {
        ui::TreeNode* pParentNode = m_pTreeNode->GetParentNode();
        if ((pParentNode != nullptr) && (pParentNode != m_pTree->GetRootNode())) {
            bEnableUp = true;
        }
    }
    if (m_pBtnUp != nullptr) {
        m_pBtnUp->SetEnabled(bEnableUp);
    }
    if (m_pBtnBack != nullptr) {
        m_pBtnBack->SetEnabled(bEnableBack);
    }
    if (m_pBtnForward != nullptr) {
        m_pBtnForward->SetEnabled(bEnableForward);
    }

    bool bIsComputerView = false;
    if (m_pTree != nullptr) {
        bIsComputerView = m_pTree->IsMyComputerNode(m_pTreeNode);
    }
    // Switch list type
    if (m_pBtnViewListType != nullptr) {
        m_pBtnViewListType->SetEnabled(!bIsComputerView);
    }

    // Switch sort mode
    if (m_pBtnViewSort != nullptr) {
        m_pBtnViewSort->SetEnabled(!bIsComputerView);
    }
}

void MainForm::SwitchToTabBoxViewType(TabBoxViewType tabBoxViewType)
{
    m_tabBoxViewType = tabBoxViewType;
    if (m_pTabBox != nullptr) {
        m_pTabBox->SelectItem((size_t)tabBoxViewType);
    }
}

void MainForm::SwitchToDataViewType(DataViewType dataViewType)
{
    if (m_dataViewType == dataViewType) {
        return;
    }
    m_dataViewType = dataViewType;
    // Update the image list and view styles
    ui::ImageListPtr spImageList;
    ui::ListCtrl* pListCtrl = nullptr;
    if (m_pExplorerView != nullptr) {
        pListCtrl = m_pExplorerView->GetListCtrl();
    }
    ui::UiSize szItemSize;
    if (pListCtrl != nullptr) {
        switch (m_dataViewType) {
        case DataViewType::kIconViewBig:
            pListCtrl->SetListCtrlType(ui::ListCtrlType::Icon);
            spImageList = pListCtrl->GetImageList(ui::ListCtrlType::Icon);
            if (spImageList != nullptr) {
                spImageList->SetImageSize(ui::UiSize(64, 64), Dpi(), true);
            }
            szItemSize = pListCtrl->GetIconView()->GetItemSize();
            szItemSize.cy = Dpi().GetScaleInt(40 + 4 + 4 + 64);
            pListCtrl->GetIconView()->SetItemSize(szItemSize);
            break;
        case DataViewType::kIconViewMedium:
            pListCtrl->SetListCtrlType(ui::ListCtrlType::Icon);
            spImageList = pListCtrl->GetImageList(ui::ListCtrlType::Icon);
            if (spImageList != nullptr) {
                spImageList->SetImageSize(ui::UiSize(32, 32), Dpi(), true);
            }
            szItemSize = pListCtrl->GetIconView()->GetItemSize();
            szItemSize.cy = Dpi().GetScaleInt(40 + 4 + 4 + 32);
            pListCtrl->GetIconView()->SetItemSize(szItemSize);
            break;
        case DataViewType::kIconViewSmall:
            pListCtrl->SetListCtrlType(ui::ListCtrlType::Icon);
            spImageList = pListCtrl->GetImageList(ui::ListCtrlType::Icon);
            if (spImageList != nullptr) {
                spImageList->SetImageSize(ui::UiSize(20, 20), Dpi(), true);
            }
            szItemSize = pListCtrl->GetIconView()->GetItemSize();
            szItemSize.cy = Dpi().GetScaleInt(40 + 4 + 4 + 20);
            pListCtrl->GetIconView()->SetItemSize(szItemSize);
            break;
        case DataViewType::kListViewBig:
            pListCtrl->SetListCtrlType(ui::ListCtrlType::List);
            spImageList = pListCtrl->GetImageList(ui::ListCtrlType::List);
            if (spImageList != nullptr) {
                spImageList->SetImageSize(ui::UiSize(64, 64), Dpi(), true);
            }
            szItemSize.cx = Dpi().GetScaleInt(300);
            szItemSize.cy = Dpi().GetScaleInt(8 + 64);
            pListCtrl->GetListView()->SetItemSize(szItemSize);
            pListCtrl->GetListView()->SetTextSingleLine(false);
            break;
        case DataViewType::kListViewMedium:
            pListCtrl->SetListCtrlType(ui::ListCtrlType::List);
            spImageList = pListCtrl->GetImageList(ui::ListCtrlType::List);
            if (spImageList != nullptr) {
                spImageList->SetImageSize(ui::UiSize(32, 32), Dpi(), true);
            }
            szItemSize.cx = Dpi().GetScaleInt(280);
            szItemSize.cy = Dpi().GetScaleInt(8 + 32);
            pListCtrl->GetListView()->SetItemSize(szItemSize);
            pListCtrl->GetListView()->SetTextSingleLine(false);
            break;
        case DataViewType::kListViewSmall:
            pListCtrl->SetListCtrlType(ui::ListCtrlType::List);
            spImageList = pListCtrl->GetImageList(ui::ListCtrlType::List);
            if (spImageList != nullptr) {
                spImageList->SetImageSize(ui::UiSize(20, 20), Dpi(), true);
            }
            szItemSize.cx = Dpi().GetScaleInt(260);
            szItemSize.cy = Dpi().GetScaleInt(36);
            pListCtrl->GetListView()->SetItemSize(szItemSize);
            pListCtrl->GetListView()->SetTextSingleLine(true);
            break;
        case DataViewType::kReprortView:
            pListCtrl->SetListCtrlType(ui::ListCtrlType::Report);
            break;
        case DataViewType::kPictureView:
            break;
        default:
            break;
        }
    }

    // Refresh the currently displayed content
    if ((m_pTree != nullptr) && (m_pTreeNode != nullptr)) {
        m_pTree->RefreshFolderContents(m_pTreeNode, nullptr);
    }
}

MainForm::DataViewType MainForm::GetDataViewType() const
{
    return m_dataViewType;
}

// Simplified pure-code UI built with ui::Create / ui::Attach.
static void BuildUIFromXml(ui::Window* pWindow)
{
    auto* p0 = ui::Create<ui::VBox>(pWindow, {{_T("bkcolor"), _T("bk_wnd_darkcolor")}});
    auto* p1 = ui::Attach<ui::HBox>(p0, {{_T("name"), _T("window_caption_bar")}, {_T("width"), _T("stretch")}, {_T("height"), _T("36")}, {_T("bkcolor"), _T("bk_wnd_lightcolor")}});
    auto* p2 = ui::Attach<ui::HBox>(p1, {{_T("margin"), _T("0,0,30,0")}, {_T("valign"), _T("center")}, {_T("width"), _T("auto")}, {_T("height"), _T("auto")}, {_T("mouse_enabled"), _T("false")}});
    auto* p3 = ui::Attach<ui::Control>(p2, {{_T("width"), _T("18")}, {_T("height"), _T("18")}, {_T("bkimage"), _T("public/caption/logo.svg")}, {_T("valign"), _T("center")}, {_T("margin"), _T("8,0,0,0")}});
    auto* p4 = ui::Attach<ui::Label>(p2, {{_T("text"), _T("TreeView控件测试程序")}, {_T("valign"), _T("center")}, {_T("margin"), _T("8,0,0,0")}, {_T("mouse_enabled"), _T("false")}});
    auto* p5 = ui::Attach<ui::Control>(p1, {{_T("mouse_enabled"), _T("false")}});
    auto* p6 = ui::Attach<ui::HBox>(p1, {{_T("margin"), _T("0,0,0,0")}, {_T("valign"), _T("center")}, {_T("width"), _T("auto")}, {_T("height"), _T("36")}});
    auto* p7 = ui::Attach<ui::Button>(p6, {{_T("class"), _T("btn_wnd_min_11")}, {_T("height"), _T("32")}, {_T("width"), _T("40")}, {_T("name"), _T("minbtn")}, {_T("margin"), _T("0,2,0,2")}, {_T("tooltip_text"), _T("最小化")}});
    auto* p8 = ui::Attach<ui::Box>(p6, {{_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    auto* p9 = ui::Attach<ui::Button>(p8, {{_T("class"), _T("btn_wnd_max_11")}, {_T("height"), _T("32")}, {_T("width"), _T("stretch")}, {_T("name"), _T("maxbtn")}, {_T("tooltip_text"), _T("最大化")}});
    auto* p10 = ui::Attach<ui::Button>(p8, {{_T("class"), _T("btn_wnd_restore_11")}, {_T("height"), _T("32")}, {_T("width"), _T("stretch")}, {_T("name"), _T("restorebtn")}, {_T("visible"), _T("false")}, {_T("tooltip_text"), _T("还原")}});
    auto* p11 = ui::Attach<ui::Button>(p6, {{_T("class"), _T("btn_wnd_close_11")}, {_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("name"), _T("closebtn")}, {_T("margin"), _T("0,0,0,2")}, {_T("tooltip_text"), _T("关闭")}});
    auto* p12 = ui::Attach<ui::VBox>(p0, {});
    auto* p13 = ui::Attach<ui::HBox>(p12, {{_T("minheight"), _T("18")}, {_T("bkcolor"), _T("#FFCCD5F0")}, {_T("height"), _T("auto")}});
    auto* p14 = ui::Attach<ui::Label>(p13, {{_T("text"), _T("功能控制：")}, {_T("valign"), _T("center")}, {_T("margin"), _T("12,0,10,0")}});
    auto* p15 = ui::Attach<ui::CheckBox>(p13, {{_T("class"), _T("checkbox_2")}, {_T("text"), _T("显示展开标志")}, {_T("selected"), _T("true")}, {_T("valign"), _T("center")}, {_T("margin"), _T("10,4,0,2")}});
    auto* p16 = ui::Attach<ui::CheckBox>(p13, {{_T("class"), _T("checkbox_2")}, {_T("text"), _T("显示CheckBox")}, {_T("selected"), _T("true")}, {_T("valign"), _T("center")}, {_T("margin"), _T("10,4,0,2")}});
    auto* p17 = ui::Attach<ui::CheckBox>(p13, {{_T("class"), _T("checkbox_2")}, {_T("text"), _T("显示图标")}, {_T("selected"), _T("true")}, {_T("valign"), _T("center")}, {_T("margin"), _T("10,4,0,2")}});
    auto* p18 = ui::Attach<ui::CheckBox>(p13, {{_T("class"), _T("checkbox_2")}, {_T("text"), _T("支持多选")}, {_T("selected"), _T("false")}, {_T("valign"), _T("center")}, {_T("margin"), _T("10,4,0,2")}});
    auto* p19 = ui::Attach<ui::CheckBox>(p13, {{_T("class"), _T("checkbox_2")}, {_T("text"), _T("显示隐藏文件")}, {_T("selected"), _T("false")}, {_T("valign"), _T("center")}, {_T("margin"), _T("10,4,0,2")}});
    auto* p20 = ui::Attach<ui::CheckBox>(p13, {{_T("class"), _T("checkbox_2")}, {_T("text"), _T("显示系统文件")}, {_T("selected"), _T("false")}, {_T("valign"), _T("center")}, {_T("margin"), _T("10,4,0,2")}});
    auto* p21 = ui::Attach<ui::HBox>(p12, {{_T("width"), _T("stretch")}, {_T("height"), _T("32")}, {_T("bkcolor"), _T("#FFF8F8F8")}});
    auto* p22 = ui::Attach<ui::Button>(p21, {{_T("class"), _T("btn_view_common btn_view_up")}, {_T("name"), _T("btn_view_up")}, {_T("margin"), _T("4,0,0,0")}, {_T("tooltip_text"), _T("上移到父目录")}});
    auto* p23 = ui::Attach<ui::Button>(p21, {{_T("class"), _T("btn_view_common btn_view_left")}, {_T("name"), _T("btn_view_left")}, {_T("tooltip_text"), _T("返回")}});
    auto* p24 = ui::Attach<ui::Button>(p21, {{_T("class"), _T("btn_view_common btn_view_right")}, {_T("name"), _T("btn_view_right")}, {_T("tooltip_text"), _T("前进")}});
    auto* p25 = ui::Attach<ui::Button>(p21, {{_T("class"), _T("btn_view_common btn_view_refresh")}, {_T("name"), _T("btn_view_refresh")}, {_T("tooltip_text"), _T("刷新左侧的目录树和文件显示区")}});
    auto* p26 = ui::Attach<ui::ButtonHBox>(p21, {{_T("name"), _T("btn_view_sort")}, {_T("width"), _T("auto")}, {_T("height"), _T("26")}, {_T("border_round"), _T("5,5")}, {_T("padding"), _T("6,0,6,0")}, {_T("valign"), _T("center")}, {_T("halign"), _T("center")}, {_T("tooltip_text"), _T("排序方式")}, {_T("hot_color"), _T("#AAB2B4B8")}, {_T("pushed_color"), _T("#FFB2B4B8")}});
    auto* p27 = ui::Attach<ui::Control>(p26, {{_T("width"), _T("auto")}, {_T("bkimage"), _T("file='../tree_view/view-sort.svg' width='16' height='16' valign='center' halign='center'")}, {_T("mouse_enabled"), _T("false")}, {_T("keyboard_enabled"), _T("false")}});
    auto* p28 = ui::Attach<ui::Label>(p26, {{_T("text"), _T("排序")}, {_T("width"), _T("auto")}, {_T("height"), _T("28")}, {_T("margin"), _T("4,0,4,0")}, {_T("font"), _T("system_12")}, {_T("text_align"), _T("hcenter,vcenter")}, {_T("valign"), _T("center")}, {_T("mouse_enabled"), _T("false")}, {_T("keyboard_enabled"), _T("false")}});
    auto* p29 = ui::Attach<ui::Control>(p26, {{_T("width"), _T("auto")}, {_T("bkimage"), _T("file='../tree_view/chevron-bottom.svg' width='8' height='8' valign='center' halign='center'")}, {_T("mouse_enabled"), _T("false")}, {_T("keyboard_enabled"), _T("false")}});
    auto* p30 = ui::Attach<ui::ButtonHBox>(p21, {{_T("name"), _T("btn_view_list_type")}, {_T("width"), _T("auto")}, {_T("height"), _T("26")}, {_T("border_round"), _T("5,5")}, {_T("padding"), _T("6,0,6,0")}, {_T("valign"), _T("center")}, {_T("halign"), _T("center")}, {_T("tooltip_text"), _T("切换视图模式")}, {_T("hot_color"), _T("#AAB2B4B8")}, {_T("pushed_color"), _T("#FFB2B4B8")}});
    auto* p31 = ui::Attach<ui::Control>(p30, {{_T("width"), _T("auto")}, {_T("bkimage"), _T("file='../tree_view/view-list-symbolic.svg' width='16' height='16' valign='center' halign='center'")}, {_T("mouse_enabled"), _T("false")}, {_T("keyboard_enabled"), _T("false")}});
    auto* p32 = ui::Attach<ui::Label>(p30, {{_T("text"), _T("查看")}, {_T("width"), _T("auto")}, {_T("height"), _T("28")}, {_T("margin"), _T("4,0,4,0")}, {_T("font"), _T("system_12")}, {_T("text_align"), _T("hcenter,vcenter")}, {_T("valign"), _T("center")}, {_T("mouse_enabled"), _T("false")}, {_T("keyboard_enabled"), _T("false")}});
    auto* p33 = ui::Attach<ui::Control>(p30, {{_T("width"), _T("auto")}, {_T("bkimage"), _T("file='../tree_view/chevron-bottom.svg' width='8' height='8' valign='center' halign='center'")}, {_T("mouse_enabled"), _T("false")}, {_T("keyboard_enabled"), _T("false")}});
    auto* p34 = ui::Attach<ui::Label>(p21, {{_T("text"), _T("当前路径：")}, {_T("valign"), _T("center")}, {_T("margin"), _T("10,0,0,0")}});
    auto* p35 = ui::Attach<ui::AddressBar>(p21, {{_T("class"), _T("address_bar")}, {_T("name"), _T("file_path")}, {_T("width"), _T("stretch")}, {_T("height"), _T("28")}, {_T("valign"), _T("center")}, {_T("margin"), _T("8,0,10,0")}, {_T("kill_focus_update_ui"), _T("false")}});
    auto* p36 = ui::Attach<ui::HBox>(p12, {{_T("minheight"), _T("60")}, {_T("bkcolor"), _T("white")}, {_T("border_size"), _T("0,2,0,0")}, {_T("border_color"), _T("splitline_level2")}, {_T("padding"), _T("0,4,0,0")}});
    auto* p37 = ui::Attach<ui::HBox>(p36, {{_T("width"), _T("300")}, {_T("minwidth"), _T("60")}});
    auto* p38 = ui::Attach<ui::DirectoryTree>(p37, {{_T("class"), _T("tree_view")}, {_T("name"), _T("tree")}, {_T("multi_select"), _T("false")}, {_T("check_box_class"), _T("tree_node_checkbox")}, {_T("expand_image_class"), _T("tree_node_expand")}, {_T("padding"), _T("5,3,5,3")}, {_T("indent"), _T("20")}});
    auto* p39 = ui::Attach<ui::Split>(p36, {{_T("bkcolor"), _T("splitline_level1")}, {_T("width"), _T("5")}, {_T("border_color"), _T("white")}, {_T("border_size"), _T("2,0,2,0")}});
    auto* p40 = ui::Attach<ui::TabBox>(p36, {{_T("name"), _T("main_view_tab_box")}, {_T("bkcolor"), _T("white")}, {_T("selected_id"), _T("0")}});
    auto* p41 = ui::Attach<ui::VirtualVTileListBox>(p40, {{_T("class"), _T("list")}, {_T("name"), _T("simple_file_view")}, {_T("columns"), _T("auto")}, {_T("item_size"), _T("200,120")}, {_T("multi_select"), _T("false")}, {_T("vscrollbar"), _T("true")}, {_T("hscrollbar"), _T("true")}, {_T("child_halign"), _T("left")}});
    auto* p42 = ui::Attach<ui::ListCtrl>(p40, {{_T("name"), _T("explorer_view")}, {_T("bkcolor"), _T("YellowGreen")}, {_T("type"), _T("report")}, {_T("show_header"), _T("true")}, {_T("header_class"), _T("list_ctrl_header")}, {_T("header_item_class"), _T("list_ctrl_header_item")}, {_T("header_split_box_class"), _T("list_ctrl_header_split_box")}, {_T("header_split_control_class"), _T("list_ctrl_header_split_control")}, {_T("header_height"), _T("32")}, {_T("enable_header_drag_order"), _T("true")}, {_T("check_box_class"), _T("list_ctrl_checkbox")}, {_T("data_item_class"), _T("list_ctrl_item")}, {_T("data_sub_item_class"), _T("list_ctrl_sub_item")}, {_T("report_view_class"), _T("list_ctrl_report_view")}, {_T("data_item_height"), _T("46")}, {_T("row_grid_line_width"), _T("0")}, {_T("row_grid_line_color"), _T("lightgray")}, {_T("column_grid_line_width"), _T("0")}, {_T("column_grid_line_color"), _T("lightgray")}, {_T("multi_select"), _T("true")}, {_T("auto_check_select"), _T("false")}, {_T("show_header_checkbox"), _T("false")}, {_T("show_data_item_checkbox"), _T("false")}, {_T("icon_view_class"), _T("list_ctrl_icon_view")}, {_T("icon_view_item_class"), _T("list_ctrl_icon_view_item")}, {_T("icon_view_item_image_class"), _T("list_ctrl_icon_view_item_image")}, {_T("icon_view_item_label_class"), _T("list_ctrl_icon_view_item_label")}, {_T("list_view_class"), _T("list_ctrl_list_view")}, {_T("list_view_item_class"), _T("list_ctrl_list_view_item")}, {_T("list_view_item_image_class"), _T("list_ctrl_list_view_item_image")}, {_T("list_view_item_label_class"), _T("list_ctrl_list_view_item_label")}, {_T("enable_item_edit"), _T("true")}, {_T("list_ctrl_richedit_class"), _T("list_ctrl_richedit")}});
    auto* p43 = ui::Attach<ui::ListCtrl>(p40, {{_T("name"), _T("computer_view")}, {_T("bkcolor"), _T("YellowGreen")}, {_T("type"), _T("report")}, {_T("show_header"), _T("true")}, {_T("header_class"), _T("list_ctrl_header")}, {_T("header_item_class"), _T("list_ctrl_header_item")}, {_T("header_split_box_class"), _T("list_ctrl_header_split_box")}, {_T("header_split_control_class"), _T("list_ctrl_header_split_control")}, {_T("header_height"), _T("32")}, {_T("enable_header_drag_order"), _T("true")}, {_T("check_box_class"), _T("list_ctrl_checkbox")}, {_T("data_item_class"), _T("list_ctrl_item")}, {_T("data_sub_item_class"), _T("list_ctrl_sub_item")}, {_T("report_view_class"), _T("list_ctrl_report_view")}, {_T("data_item_height"), _T("46")}, {_T("row_grid_line_width"), _T("0")}, {_T("row_grid_line_color"), _T("lightgray")}, {_T("column_grid_line_width"), _T("0")}, {_T("column_grid_line_color"), _T("lightgray")}, {_T("multi_select"), _T("true")}, {_T("auto_check_select"), _T("false")}, {_T("show_header_checkbox"), _T("false")}, {_T("show_data_item_checkbox"), _T("false")}, {_T("icon_view_class"), _T("list_ctrl_icon_view")}, {_T("icon_view_item_class"), _T("list_ctrl_icon_view_item")}, {_T("icon_view_item_image_class"), _T("list_ctrl_icon_view_item_image")}, {_T("icon_view_item_label_class"), _T("list_ctrl_icon_view_item_label")}, {_T("list_view_class"), _T("list_ctrl_list_view")}, {_T("list_view_item_class"), _T("list_ctrl_list_view_item")}, {_T("list_view_item_image_class"), _T("list_ctrl_list_view_item_image")}, {_T("list_view_item_label_class"), _T("list_ctrl_list_view_item_label")}, {_T("enable_item_edit"), _T("true")}, {_T("list_ctrl_richedit_class"), _T("list_ctrl_richedit")}});
    auto* p44 = ui::Attach<ui::Box>(p40, {{_T("name"), _T("error_view")}, {_T("bkcolor"), _T("red")}});
    ui::Attach(pWindow, p0);
}
