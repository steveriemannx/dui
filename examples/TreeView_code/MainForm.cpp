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

void MainForm::BuildUI()
{
    BuildUIFromXml(this);
}

void MainForm::OnInitWindow()
{
    SetupWindow();
    BuildUI();
    BindEvents();

    m_pTree = ui::Find<ui::DirectoryTree>(this, DUI_T("tree"));
    ASSERT(m_pTree != nullptr);
    if (m_pTree == nullptr) {
        return;
    }
    m_pAddressBar = ui::Find<ui::AddressBar>(this, DUI_T("file_path"));
    if (m_pAddressBar != nullptr) {
        m_pAddressBar->AttachPathChanged(UiBind(&MainForm::OnAddressBarPathChanged, this, std::placeholders::_1));
        m_pAddressBar->AttachPathClick(UiBind(&MainForm::OnAddressBarPathClick, this, std::placeholders::_1));
    }
    m_pTabBox = ui::Find<ui::TabBox>(this, DUI_T("main_view_tab_box"));
    ui::ListCtrl* pComputerListCtrl = ui::Find<ui::ListCtrl>(this, DUI_T("computer_view"));
    m_pComputerView = std::make_unique<ComputerView>(this, pComputerListCtrl);
    ui::VirtualListBox* pListBox = ui::Find<ui::VirtualListBox>(this, DUI_T("simple_file_view"));
    m_pSimpleFileView = std::make_unique<SimpleFileView>(this, pListBox);
    ui::ListCtrl* pExplorerListCtrl = ui::Find<ui::ListCtrl>(this, DUI_T("explorer_view"));
    m_pExplorerView = std::make_unique<ExplorerView>(this, pExplorerListCtrl);

    // Up button
    m_pBtnUp = ui::Find<ui::Button>(this, DUI_T("btn_view_up"));
    // Back button
    m_pBtnBack = ui::Find<ui::Button>(this, DUI_T("btn_view_left"));
    // Forward button
    m_pBtnForward = ui::Find<ui::Button>(this, DUI_T("btn_view_right"));
    // Switch view mode
    m_pBtnViewListType = ui::Find<ui::ButtonHBox>(this, DUI_T("btn_view_list_type"));
    // Switch sort mode
    m_pBtnViewSort = ui::Find<ui::ButtonHBox>(this, DUI_T("btn_view_sort"));

    UpdateCommandUI();

    // Show the virtual path
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kUserHome, DUI_T("Home Folder"));
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kDesktop, DUI_T("Desktop"));
    ui::TreeNode* pDocumentsNode = m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kDocuments, DUI_T("Document"));
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kPictures, DUI_T("Image"));
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kMusic, DUI_T("Music"));
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kVideos, DUI_T("Video"));
    m_pTree->ShowVirtualDirectoryNode(ui::VirtualDirectoryType::kDownloads, DUI_T("Download"));

    // Show disks
    ui::TreeNode* pComputerNode = m_pTree->ShowAllDiskNodes(DUI_T("Computer"), DUI_T("File System"));
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
    if (auto* pRefreshBtn = ui::Find<ui::Button>(this, DUI_T("btn_view_refresh"))) {
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
        m_pAddressBar->SetAddressPath(DUI_T(""));
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
        DString errMsg = DUI_T("Path does not exist:");
        errMsg += filePath.ToString();
        ui::SystemUtil::ShowMessageBox(this, errMsg.c_str(), DUI_T("Error Info"));
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
        DString errMsg = DUI_T("The input path does not exist:");
        errMsg += text;
        ui::SystemUtil::ShowMessageBox(this, errMsg.c_str(), DUI_T("Error Info"));
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
    menu->ShowMenu(DUI_T(""), point);
    {
        // Add menu items (with icons and text, structure corresponds to list_type_menu.xml)
        struct ListTypeItem { DString name; DString btnName; DString image; DString text; };
        const ListTypeItem items[] = {
            { DUI_T("menu_item_icon_big"), DUI_T("btn_menu_item_icon_big"), DUI_T("data_icons_display-symbolic.svg"), DUI_T("Icon View (Large Icons)") },
            { DUI_T("menu_item_icon_medium"), DUI_T("btn_menu_item_icon_medium"), DUI_T("data_icons_display-symbolic.svg"), DUI_T("Icon View (Medium Icons)") },
            { DUI_T("menu_item_icon_small"), DUI_T("btn_menu_item_icon_small"), DUI_T("data_icons_display-symbolic.svg"), DUI_T("Icon View (Small Icons)") },
            { DUI_T("menu_item_list_big"), DUI_T("btn_menu_item_list_big"), DUI_T("view-list-symbolic.svg"), DUI_T("List View (Large Icons)") },
            { DUI_T("menu_item_list_medium"), DUI_T("btn_menu_item_list_medium"), DUI_T("view-list-symbolic.svg"), DUI_T("List View (Medium Icons)") },
            { DUI_T("menu_item_list_small"), DUI_T("btn_menu_item_list_small"), DUI_T("view-list-symbolic.svg"), DUI_T("List View (Small Icons)") },
            { DUI_T("menu_item_report"), DUI_T("btn_menu_item_report"), DUI_T("view-list-compact-symbolic.svg"), DUI_T("Detail View") },
            { DUI_T("menu_item_picture"), DUI_T("btn_menu_item_picture"), DUI_T("view-list-images-symbolic.svg"), DUI_T("Picture List View") },
        };
        for (const auto& item : items) {
            ui::MenuItem* pMenuItem = new ui::MenuItem(menu);
            pMenuItem->SetClass(DUI_T("menu_element"));
            pMenuItem->SetName(item.name);
            pMenuItem->SetFixedWidth(ui::UiFixedInt(220), true, true);

            ui::HBox* pIconBox = new ui::HBox(menu);
            pIconBox->SetAttribute(DUI_T("width"), DUI_T("44"));
            pIconBox->SetAttribute(DUI_T("mouse_enabled"), DUI_T("false"));
            pIconBox->SetAttribute(DUI_T("keyboard_enabled"), DUI_T("false"));
            pMenuItem->AddItem(pIconBox);

            ui::Button* pIconBtn = new ui::Button(menu);
            pIconBtn->SetName(item.btnName);
            pIconBtn->SetAttribute(DUI_T("width"), DUI_T("16"));
            pIconBtn->SetAttribute(DUI_T("height"), DUI_T("16"));
            pIconBtn->SetAttribute(DUI_T("valign"), DUI_T("center"));
            pIconBtn->SetAttribute(DUI_T("margin"), DUI_T("0,0,8,0"));
            pIconBtn->SetAttribute(DUI_T("mouse_enabled"), DUI_T("false"));
            pIconBtn->SetAttribute(DUI_T("keyboard_enabled"), DUI_T("false"));
            pIconBox->AddItem(pIconBtn);

            ui::Button* pImageBtn = new ui::Button(menu);
            pImageBtn->SetBkImage(item.image);
            pImageBtn->SetAttribute(DUI_T("width"), DUI_T("16"));
            pImageBtn->SetAttribute(DUI_T("height"), DUI_T("16"));
            pImageBtn->SetAttribute(DUI_T("valign"), DUI_T("center"));
            pImageBtn->SetAttribute(DUI_T("mouse_enabled"), DUI_T("false"));
            pImageBtn->SetAttribute(DUI_T("keyboard_enabled"), DUI_T("false"));
            pIconBox->AddItem(pImageBtn);

            ui::Label* pLabel = new ui::Label(menu);
            pLabel->SetClass(DUI_T("menu_text"));
            pLabel->SetText(item.text);
            pLabel->SetAttribute(DUI_T("margin"), DUI_T("50,0,0,0"));
            pLabel->SetAttribute(DUI_T("mouse_enabled"), DUI_T("false"));
            pLabel->SetAttribute(DUI_T("keyboard_enabled"), DUI_T("false"));
            pMenuItem->AddItem(pLabel);

            menu->AddMenuItem(pMenuItem);
        }
    }

    std::map<DataViewType, DString> btnNameMap;
    btnNameMap[DataViewType::kIconViewBig] = DUI_T("btn_menu_item_icon_big");
    btnNameMap[DataViewType::kIconViewMedium] = DUI_T("btn_menu_item_icon_medium");
    btnNameMap[DataViewType::kIconViewSmall] = DUI_T("btn_menu_item_icon_small");
    btnNameMap[DataViewType::kListViewBig] = DUI_T("btn_menu_item_list_big");
    btnNameMap[DataViewType::kListViewMedium] = DUI_T("btn_menu_item_list_medium");
    btnNameMap[DataViewType::kListViewSmall] = DUI_T("btn_menu_item_list_small");
    btnNameMap[DataViewType::kReprortView] = DUI_T("btn_menu_item_report");
    btnNameMap[DataViewType::kPictureView] = DUI_T("btn_menu_item_picture");

    DString selectBtnName = btnNameMap[GetDataViewType()];
    ui::Button* pSelectBtn = ui::Find<ui::Button>(menu, selectBtnName);
    if (pSelectBtn != nullptr) {
        pSelectBtn->SetBkImage(DUI_T("ui-item-symbolic.svg"));
    }

    // Bind the menu item selection event
    menu->AttachMenuItemActivated([this](const DString& /*menuName*/, int32_t /*nMenuLevel*/,
                                         const DString& itemName, size_t /*nItemIndex*/) {
            // Matches the menu item names in the XML
            std::map<DataViewType, DString> itemNameMap;
            itemNameMap[DataViewType::kIconViewBig] = DUI_T("menu_item_icon_big");
            itemNameMap[DataViewType::kIconViewMedium] = DUI_T("menu_item_icon_medium");
            itemNameMap[DataViewType::kIconViewSmall] = DUI_T("menu_item_icon_small");
            itemNameMap[DataViewType::kListViewBig] = DUI_T("menu_item_list_big");
            itemNameMap[DataViewType::kListViewMedium] = DUI_T("menu_item_list_medium");
            itemNameMap[DataViewType::kListViewSmall] = DUI_T("menu_item_list_small");
            itemNameMap[DataViewType::kReprortView] = DUI_T("menu_item_report");
            itemNameMap[DataViewType::kPictureView] = DUI_T("menu_item_picture");
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
    menu->ShowMenu(DUI_T(""), point);
    {
        // Add menu items (with icon buttons and text, structure corresponds to sort_mode_menu.xml)
        struct SortModeItem { DString name; DString btnName; DString text; };
        const SortModeItem items[] = {
            { DUI_T("menu_item_file_name"), DUI_T("btn_file_name"), DUI_T("File Name") },
            { DUI_T("menu_item_file_modify_time"), DUI_T("btn_file_modify_time"), DUI_T("Modified Date") },
            { DUI_T("menu_item_file_type"), DUI_T("btn_file_type"), DUI_T("File Type") },
            { DUI_T("menu_item_file_size"), DUI_T("btn_file_size"), DUI_T("File Size") },
        };
        for (const auto& item : items) {
            ui::MenuItem* pMenuItem = new ui::MenuItem(menu);
            pMenuItem->SetClass(DUI_T("menu_element"));
            pMenuItem->SetName(item.name);
            pMenuItem->SetFixedWidth(ui::UiFixedInt(160), true, true);

            ui::Button* pIconBtn = new ui::Button(menu);
            pIconBtn->SetName(item.btnName);
            pIconBtn->SetAttribute(DUI_T("width"), DUI_T("auto"));
            pIconBtn->SetAttribute(DUI_T("height"), DUI_T("auto"));
            pIconBtn->SetAttribute(DUI_T("valign"), DUI_T("center"));
            pIconBtn->SetAttribute(DUI_T("mouse_enabled"), DUI_T("false"));
            pIconBtn->SetAttribute(DUI_T("keyboard_enabled"), DUI_T("false"));
            pMenuItem->AddItem(pIconBtn);

            ui::Label* pLabel = new ui::Label(menu);
            pLabel->SetClass(DUI_T("menu_text"));
            pLabel->SetText(item.text);
            pLabel->SetAttribute(DUI_T("margin"), DUI_T("30,0,0,0"));
            pLabel->SetAttribute(DUI_T("mouse_enabled"), DUI_T("false"));
            pLabel->SetAttribute(DUI_T("keyboard_enabled"), DUI_T("false"));
            pMenuItem->AddItem(pLabel);

            menu->AddMenuItem(pMenuItem);
        }

        // Separator
        {
            ui::Box* pSplitBox = new ui::Box(menu);
            pSplitBox->SetClass(DUI_T("menu_split_box"));
            pSplitBox->SetAttribute(DUI_T("margin"), DUI_T("0,4,0,4"));
            ui::Control* pSplitLine = new ui::Control(menu);
            pSplitLine->SetClass(DUI_T("menu_split_line"));
            pSplitLine->SetAttribute(DUI_T("mouse_enabled"), DUI_T("false"));
            pSplitLine->SetAttribute(DUI_T("keyboard_enabled"), DUI_T("false"));
            pSplitBox->AddItem(pSplitLine);
            // Note: the separator is for display only and is not added as a menu item
            ui::MenuItem* pSortAsc = new ui::MenuItem(menu);
            pSortAsc->SetClass(DUI_T("menu_element"));
            pSortAsc->SetName(DUI_T("menu_item_sort_ascending"));
            pSortAsc->SetFixedWidth(ui::UiFixedInt(160), true, true);
            ui::Button* pAscBtn = new ui::Button(menu);
            pAscBtn->SetName(DUI_T("btn_sort_ascending"));
            pAscBtn->SetAttribute(DUI_T("width"), DUI_T("auto"));
            pAscBtn->SetAttribute(DUI_T("height"), DUI_T("auto"));
            pAscBtn->SetAttribute(DUI_T("valign"), DUI_T("center"));
            pAscBtn->SetAttribute(DUI_T("mouse_enabled"), DUI_T("false"));
            pAscBtn->SetAttribute(DUI_T("keyboard_enabled"), DUI_T("false"));
            pSortAsc->AddItem(pAscBtn);
            ui::Label* pAscLabel = new ui::Label(menu);
            pAscLabel->SetClass(DUI_T("menu_text"));
            pAscLabel->SetText(DUI_T("Ascending Sort"));
            pAscLabel->SetAttribute(DUI_T("margin"), DUI_T("30,0,0,0"));
            pAscLabel->SetAttribute(DUI_T("mouse_enabled"), DUI_T("false"));
            pAscLabel->SetAttribute(DUI_T("keyboard_enabled"), DUI_T("false"));
            pSortAsc->AddItem(pAscLabel);
            menu->AddMenuItem(pSortAsc);

            ui::MenuItem* pSortDesc = new ui::MenuItem(menu);
            pSortDesc->SetClass(DUI_T("menu_element"));
            pSortDesc->SetName(DUI_T("menu_item_sort_descending"));
            pSortDesc->SetFixedWidth(ui::UiFixedInt(160), true, true);
            ui::Button* pDescBtn = new ui::Button(menu);
            pDescBtn->SetName(DUI_T("btn_sort_descending"));
            pDescBtn->SetAttribute(DUI_T("width"), DUI_T("auto"));
            pDescBtn->SetAttribute(DUI_T("height"), DUI_T("auto"));
            pDescBtn->SetAttribute(DUI_T("valign"), DUI_T("center"));
            pDescBtn->SetAttribute(DUI_T("mouse_enabled"), DUI_T("false"));
            pDescBtn->SetAttribute(DUI_T("keyboard_enabled"), DUI_T("false"));
            pSortDesc->AddItem(pDescBtn);
            ui::Label* pDescLabel = new ui::Label(menu);
            pDescLabel->SetClass(DUI_T("menu_text"));
            pDescLabel->SetText(DUI_T("Descending Sort"));
            pDescLabel->SetAttribute(DUI_T("margin"), DUI_T("30,0,0,0"));
            pDescLabel->SetAttribute(DUI_T("mouse_enabled"), DUI_T("false"));
            pDescLabel->SetAttribute(DUI_T("keyboard_enabled"), DUI_T("false"));
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
            pSortColumnBtn = ui::Find<ui::Button>(menu, DUI_T("btn_file_name"));
        }
        else if (sortColumn == ExplorerView::ExplorerViewColumn::kModifyDateTime) {
            pSortColumnBtn = ui::Find<ui::Button>(menu, DUI_T("btn_file_modify_time"));
        }
        else if (sortColumn == ExplorerView::ExplorerViewColumn::kType) {
            pSortColumnBtn = ui::Find<ui::Button>(menu, DUI_T("btn_file_type"));
        }
        else if (sortColumn == ExplorerView::ExplorerViewColumn::kSize) {
            pSortColumnBtn = ui::Find<ui::Button>(menu, DUI_T("btn_file_size"));
        }

        ui::Button* pSortBtn = nullptr;
        if (bSortUp) {
            // Ascending
            pSortBtn = ui::Find<ui::Button>(menu, DUI_T("btn_sort_ascending"));
        }
        else {
            // Descending
            pSortBtn = ui::Find<ui::Button>(menu, DUI_T("btn_sort_descending"));
        }
        if (pSortBtn != nullptr) {
            pSortBtn->SetBkImage(DUI_T("ui-item-symbolic.svg"));
        }
        if (pSortColumnBtn != nullptr) {
            pSortColumnBtn->SetBkImage(DUI_T("ui-item-symbolic.svg"));
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
            if (itemName == DUI_T("menu_item_file_name")) {
                // File name
                if (m_pExplorerView != nullptr) {
                    m_pExplorerView->SortByColumn(ExplorerView::ExplorerViewColumn::kName, bSortUp);
                }
            }
            else if (itemName == DUI_T("menu_item_file_modify_time")) {
                // Modified date
                if (m_pExplorerView != nullptr) {
                    m_pExplorerView->SortByColumn(ExplorerView::ExplorerViewColumn::kModifyDateTime, bSortUp);
                }
            }
            else if (itemName == DUI_T("menu_item_file_type")) {
                // File type
                if (m_pExplorerView != nullptr) {
                    m_pExplorerView->SortByColumn(ExplorerView::ExplorerViewColumn::kType, bSortUp);
                }
            }
            else if (itemName == DUI_T("menu_item_file_size")) {
                // File size
                if (m_pExplorerView != nullptr) {
                    m_pExplorerView->SortByColumn(ExplorerView::ExplorerViewColumn::kSize, bSortUp);
                }
            }
            else if (itemName == DUI_T("menu_item_sort_ascending")) {
                // Ascending sort
                if (!bSorted || !bSortUp) {
                    if (m_pExplorerView != nullptr) {
                        m_pExplorerView->SortByColumn(sortColumn, true);
                    }
                }
            }
            else if (itemName == DUI_T("menu_item_sort_descending")) {
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
    auto* p0 = ui::Create<ui::VBox>(pWindow, {{DUI_T("bkcolor"), DUI_T("bk_wnd_darkcolor")}});
    auto* p1 = ui::Attach<ui::HBox>(p0, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}, {DUI_T("bkcolor"), DUI_T("bk_wnd_lightcolor")}});
    auto* p2 = ui::Attach<ui::HBox>(p1, {{DUI_T("margin"), DUI_T("0,0,30,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("auto")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    auto* p3 = ui::Attach<ui::Control>(p2, {{DUI_T("width"), DUI_T("18")}, {DUI_T("height"), DUI_T("18")}, {DUI_T("bkimage"), DUI_T("public/caption/logo.svg")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}});
    auto* p4 = ui::Attach<ui::Label>(p2, {{DUI_T("text"), DUI_T("TreeView控件测试程序")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("mouse_enabled"), DUI_T("false")}});
    auto* p5 = ui::Attach<ui::Control>(p1, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    auto* p6 = ui::Attach<ui::HBox>(p1, {{DUI_T("margin"), DUI_T("0,0,0,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("36")}});
    auto* p7 = ui::Attach<ui::Button>(p6, {{DUI_T("class"), DUI_T("btn_wnd_min_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("minbtn")}, {DUI_T("margin"), DUI_T("0,2,0,2")}, {DUI_T("tooltip_text"), DUI_T("最小化")}});
    auto* p8 = ui::Attach<ui::Box>(p6, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    auto* p9 = ui::Attach<ui::Button>(p8, {{DUI_T("class"), DUI_T("btn_wnd_max_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("name"), DUI_T("maxbtn")}, {DUI_T("tooltip_text"), DUI_T("最大化")}});
    auto* p10 = ui::Attach<ui::Button>(p8, {{DUI_T("class"), DUI_T("btn_wnd_restore_11")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("name"), DUI_T("restorebtn")}, {DUI_T("visible"), DUI_T("false")}, {DUI_T("tooltip_text"), DUI_T("还原")}});
    auto* p11 = ui::Attach<ui::Button>(p6, {{DUI_T("class"), DUI_T("btn_wnd_close_11")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("name"), DUI_T("closebtn")}, {DUI_T("margin"), DUI_T("0,0,0,2")}, {DUI_T("tooltip_text"), DUI_T("关闭")}});
    auto* p12 = ui::Attach<ui::VBox>(p0, {});
    auto* p13 = ui::Attach<ui::HBox>(p12, {{DUI_T("minheight"), DUI_T("18")}, {DUI_T("bkcolor"), DUI_T("#FFCCD5F0")}, {DUI_T("height"), DUI_T("auto")}});
    auto* p14 = ui::Attach<ui::Label>(p13, {{DUI_T("text"), DUI_T("功能控制：")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("12,0,10,0")}});
    auto* p15 = ui::Attach<ui::CheckBox>(p13, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("text"), DUI_T("显示展开标志")}, {DUI_T("selected"), DUI_T("true")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("10,4,0,2")}});
    auto* p16 = ui::Attach<ui::CheckBox>(p13, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("text"), DUI_T("显示CheckBox")}, {DUI_T("selected"), DUI_T("true")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("10,4,0,2")}});
    auto* p17 = ui::Attach<ui::CheckBox>(p13, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("text"), DUI_T("显示图标")}, {DUI_T("selected"), DUI_T("true")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("10,4,0,2")}});
    auto* p18 = ui::Attach<ui::CheckBox>(p13, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("text"), DUI_T("支持多选")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("10,4,0,2")}});
    auto* p19 = ui::Attach<ui::CheckBox>(p13, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("text"), DUI_T("显示隐藏文件")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("10,4,0,2")}});
    auto* p20 = ui::Attach<ui::CheckBox>(p13, {{DUI_T("class"), DUI_T("checkbox_2")}, {DUI_T("text"), DUI_T("显示系统文件")}, {DUI_T("selected"), DUI_T("false")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("10,4,0,2")}});
    auto* p21 = ui::Attach<ui::HBox>(p12, {{DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("32")}, {DUI_T("bkcolor"), DUI_T("#FFF8F8F8")}});
    auto* p22 = ui::Attach<ui::Button>(p21, {{DUI_T("class"), DUI_T("btn_view_common btn_view_up")}, {DUI_T("name"), DUI_T("btn_view_up")}, {DUI_T("margin"), DUI_T("4,0,0,0")}, {DUI_T("tooltip_text"), DUI_T("上移到父目录")}});
    auto* p23 = ui::Attach<ui::Button>(p21, {{DUI_T("class"), DUI_T("btn_view_common btn_view_left")}, {DUI_T("name"), DUI_T("btn_view_left")}, {DUI_T("tooltip_text"), DUI_T("返回")}});
    auto* p24 = ui::Attach<ui::Button>(p21, {{DUI_T("class"), DUI_T("btn_view_common btn_view_right")}, {DUI_T("name"), DUI_T("btn_view_right")}, {DUI_T("tooltip_text"), DUI_T("前进")}});
    auto* p25 = ui::Attach<ui::Button>(p21, {{DUI_T("class"), DUI_T("btn_view_common btn_view_refresh")}, {DUI_T("name"), DUI_T("btn_view_refresh")}, {DUI_T("tooltip_text"), DUI_T("刷新左侧的目录树和文件显示区")}});
    auto* p26 = ui::Attach<ui::ButtonHBox>(p21, {{DUI_T("name"), DUI_T("btn_view_sort")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("border_round"), DUI_T("5,5")}, {DUI_T("padding"), DUI_T("6,0,6,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("tooltip_text"), DUI_T("排序方式")}, {DUI_T("hot_color"), DUI_T("#AAB2B4B8")}, {DUI_T("pushed_color"), DUI_T("#FFB2B4B8")}});
    auto* p27 = ui::Attach<ui::Control>(p26, {{DUI_T("width"), DUI_T("auto")}, {DUI_T("bkimage"), DUI_T("file='../tree_view/view-sort.svg' width='16' height='16' valign='center' halign='center'")}, {DUI_T("mouse_enabled"), DUI_T("false")}, {DUI_T("keyboard_enabled"), DUI_T("false")}});
    auto* p28 = ui::Attach<ui::Label>(p26, {{DUI_T("text"), DUI_T("排序")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("margin"), DUI_T("4,0,4,0")}, {DUI_T("font"), DUI_T("system_12")}, {DUI_T("text_align"), DUI_T("hcenter,vcenter")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("mouse_enabled"), DUI_T("false")}, {DUI_T("keyboard_enabled"), DUI_T("false")}});
    auto* p29 = ui::Attach<ui::Control>(p26, {{DUI_T("width"), DUI_T("auto")}, {DUI_T("bkimage"), DUI_T("file='../tree_view/chevron-bottom.svg' width='8' height='8' valign='center' halign='center'")}, {DUI_T("mouse_enabled"), DUI_T("false")}, {DUI_T("keyboard_enabled"), DUI_T("false")}});
    auto* p30 = ui::Attach<ui::ButtonHBox>(p21, {{DUI_T("name"), DUI_T("btn_view_list_type")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("26")}, {DUI_T("border_round"), DUI_T("5,5")}, {DUI_T("padding"), DUI_T("6,0,6,0")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("tooltip_text"), DUI_T("切换视图模式")}, {DUI_T("hot_color"), DUI_T("#AAB2B4B8")}, {DUI_T("pushed_color"), DUI_T("#FFB2B4B8")}});
    auto* p31 = ui::Attach<ui::Control>(p30, {{DUI_T("width"), DUI_T("auto")}, {DUI_T("bkimage"), DUI_T("file='../tree_view/view-list-symbolic.svg' width='16' height='16' valign='center' halign='center'")}, {DUI_T("mouse_enabled"), DUI_T("false")}, {DUI_T("keyboard_enabled"), DUI_T("false")}});
    auto* p32 = ui::Attach<ui::Label>(p30, {{DUI_T("text"), DUI_T("查看")}, {DUI_T("width"), DUI_T("auto")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("margin"), DUI_T("4,0,4,0")}, {DUI_T("font"), DUI_T("system_12")}, {DUI_T("text_align"), DUI_T("hcenter,vcenter")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("mouse_enabled"), DUI_T("false")}, {DUI_T("keyboard_enabled"), DUI_T("false")}});
    auto* p33 = ui::Attach<ui::Control>(p30, {{DUI_T("width"), DUI_T("auto")}, {DUI_T("bkimage"), DUI_T("file='../tree_view/chevron-bottom.svg' width='8' height='8' valign='center' halign='center'")}, {DUI_T("mouse_enabled"), DUI_T("false")}, {DUI_T("keyboard_enabled"), DUI_T("false")}});
    auto* p34 = ui::Attach<ui::Label>(p21, {{DUI_T("text"), DUI_T("当前路径：")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("10,0,0,0")}});
    auto* p35 = ui::Attach<ui::AddressBar>(p21, {{DUI_T("class"), DUI_T("address_bar")}, {DUI_T("name"), DUI_T("file_path")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("28")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,10,0")}, {DUI_T("kill_focus_update_ui"), DUI_T("false")}});
    auto* p36 = ui::Attach<ui::HBox>(p12, {{DUI_T("minheight"), DUI_T("60")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("border_size"), DUI_T("0,2,0,0")}, {DUI_T("border_color"), DUI_T("splitline_level2")}, {DUI_T("padding"), DUI_T("0,4,0,0")}});
    auto* p37 = ui::Attach<ui::HBox>(p36, {{DUI_T("width"), DUI_T("300")}, {DUI_T("minwidth"), DUI_T("60")}});
    auto* p38 = ui::Attach<ui::DirectoryTree>(p37, {{DUI_T("class"), DUI_T("tree_view")}, {DUI_T("name"), DUI_T("tree")}, {DUI_T("multi_select"), DUI_T("false")}, {DUI_T("check_box_class"), DUI_T("tree_node_checkbox")}, {DUI_T("expand_image_class"), DUI_T("tree_node_expand")}, {DUI_T("padding"), DUI_T("5,3,5,3")}, {DUI_T("indent"), DUI_T("20")}});
    auto* p39 = ui::Attach<ui::Split>(p36, {{DUI_T("bkcolor"), DUI_T("splitline_level1")}, {DUI_T("width"), DUI_T("5")}, {DUI_T("border_color"), DUI_T("white")}, {DUI_T("border_size"), DUI_T("2,0,2,0")}});
    auto* p40 = ui::Attach<ui::TabBox>(p36, {{DUI_T("name"), DUI_T("main_view_tab_box")}, {DUI_T("bkcolor"), DUI_T("white")}, {DUI_T("selected_id"), DUI_T("0")}});
    auto* p41 = ui::Attach<ui::VirtualVTileListBox>(p40, {{DUI_T("class"), DUI_T("list")}, {DUI_T("name"), DUI_T("simple_file_view")}, {DUI_T("columns"), DUI_T("auto")}, {DUI_T("item_size"), DUI_T("200,120")}, {DUI_T("multi_select"), DUI_T("false")}, {DUI_T("vscrollbar"), DUI_T("true")}, {DUI_T("hscrollbar"), DUI_T("true")}, {DUI_T("child_halign"), DUI_T("left")}});
    auto* p42 = ui::Attach<ui::ListCtrl>(p40, {{DUI_T("name"), DUI_T("explorer_view")}, {DUI_T("bkcolor"), DUI_T("YellowGreen")}, {DUI_T("type"), DUI_T("report")}, {DUI_T("show_header"), DUI_T("true")}, {DUI_T("header_class"), DUI_T("list_ctrl_header")}, {DUI_T("header_item_class"), DUI_T("list_ctrl_header_item")}, {DUI_T("header_split_box_class"), DUI_T("list_ctrl_header_split_box")}, {DUI_T("header_split_control_class"), DUI_T("list_ctrl_header_split_control")}, {DUI_T("header_height"), DUI_T("32")}, {DUI_T("enable_header_drag_order"), DUI_T("true")}, {DUI_T("check_box_class"), DUI_T("list_ctrl_checkbox")}, {DUI_T("data_item_class"), DUI_T("list_ctrl_item")}, {DUI_T("data_sub_item_class"), DUI_T("list_ctrl_sub_item")}, {DUI_T("report_view_class"), DUI_T("list_ctrl_report_view")}, {DUI_T("data_item_height"), DUI_T("46")}, {DUI_T("row_grid_line_width"), DUI_T("0")}, {DUI_T("row_grid_line_color"), DUI_T("lightgray")}, {DUI_T("column_grid_line_width"), DUI_T("0")}, {DUI_T("column_grid_line_color"), DUI_T("lightgray")}, {DUI_T("multi_select"), DUI_T("true")}, {DUI_T("auto_check_select"), DUI_T("false")}, {DUI_T("show_header_checkbox"), DUI_T("false")}, {DUI_T("show_data_item_checkbox"), DUI_T("false")}, {DUI_T("icon_view_class"), DUI_T("list_ctrl_icon_view")}, {DUI_T("icon_view_item_class"), DUI_T("list_ctrl_icon_view_item")}, {DUI_T("icon_view_item_image_class"), DUI_T("list_ctrl_icon_view_item_image")}, {DUI_T("icon_view_item_label_class"), DUI_T("list_ctrl_icon_view_item_label")}, {DUI_T("list_view_class"), DUI_T("list_ctrl_list_view")}, {DUI_T("list_view_item_class"), DUI_T("list_ctrl_list_view_item")}, {DUI_T("list_view_item_image_class"), DUI_T("list_ctrl_list_view_item_image")}, {DUI_T("list_view_item_label_class"), DUI_T("list_ctrl_list_view_item_label")}, {DUI_T("enable_item_edit"), DUI_T("true")}, {DUI_T("list_ctrl_richedit_class"), DUI_T("list_ctrl_richedit")}});
    auto* p43 = ui::Attach<ui::ListCtrl>(p40, {{DUI_T("name"), DUI_T("computer_view")}, {DUI_T("bkcolor"), DUI_T("YellowGreen")}, {DUI_T("type"), DUI_T("report")}, {DUI_T("show_header"), DUI_T("true")}, {DUI_T("header_class"), DUI_T("list_ctrl_header")}, {DUI_T("header_item_class"), DUI_T("list_ctrl_header_item")}, {DUI_T("header_split_box_class"), DUI_T("list_ctrl_header_split_box")}, {DUI_T("header_split_control_class"), DUI_T("list_ctrl_header_split_control")}, {DUI_T("header_height"), DUI_T("32")}, {DUI_T("enable_header_drag_order"), DUI_T("true")}, {DUI_T("check_box_class"), DUI_T("list_ctrl_checkbox")}, {DUI_T("data_item_class"), DUI_T("list_ctrl_item")}, {DUI_T("data_sub_item_class"), DUI_T("list_ctrl_sub_item")}, {DUI_T("report_view_class"), DUI_T("list_ctrl_report_view")}, {DUI_T("data_item_height"), DUI_T("46")}, {DUI_T("row_grid_line_width"), DUI_T("0")}, {DUI_T("row_grid_line_color"), DUI_T("lightgray")}, {DUI_T("column_grid_line_width"), DUI_T("0")}, {DUI_T("column_grid_line_color"), DUI_T("lightgray")}, {DUI_T("multi_select"), DUI_T("true")}, {DUI_T("auto_check_select"), DUI_T("false")}, {DUI_T("show_header_checkbox"), DUI_T("false")}, {DUI_T("show_data_item_checkbox"), DUI_T("false")}, {DUI_T("icon_view_class"), DUI_T("list_ctrl_icon_view")}, {DUI_T("icon_view_item_class"), DUI_T("list_ctrl_icon_view_item")}, {DUI_T("icon_view_item_image_class"), DUI_T("list_ctrl_icon_view_item_image")}, {DUI_T("icon_view_item_label_class"), DUI_T("list_ctrl_icon_view_item_label")}, {DUI_T("list_view_class"), DUI_T("list_ctrl_list_view")}, {DUI_T("list_view_item_class"), DUI_T("list_ctrl_list_view_item")}, {DUI_T("list_view_item_image_class"), DUI_T("list_ctrl_list_view_item_image")}, {DUI_T("list_view_item_label_class"), DUI_T("list_ctrl_list_view_item_label")}, {DUI_T("enable_item_edit"), DUI_T("true")}, {DUI_T("list_ctrl_richedit_class"), DUI_T("list_ctrl_richedit")}});
    auto* p44 = ui::Attach<ui::Box>(p40, {{DUI_T("name"), DUI_T("error_view")}, {DUI_T("bkcolor"), DUI_T("red")}});
    ui::Attach(pWindow, p0);
}
