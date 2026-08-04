#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// dui
#include "dui/dui.h"

#include <stack>
#include <unordered_map>

class ComputerView;
class SimpleFileView;
class ExplorerView;

class MainForm : public ui::WindowImplBase
{
public:
    MainForm();
    virtual ~MainForm() override;

    /** Resource-related interface
     * The GetSkinFolder interface sets the skin resource path of the window you are drawing
     * The GetSkinFile interface sets the xml description file of the window you are drawing
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, allowing subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

    /** Trigger the tree node click event to select the corresponding directory
     */
    void SelectSubPath(const ui::FilePath& filePath);

private:
    /** The content of the specified directory has been retrieved
    * @param [in] pTreeNode the current node
    * @param [in] currentPath the path of the currently displayed content
    * @param [in] folderList returns the list of all subdirectories in the path directory
    * @param [in] fileList returns the list of all files in the path directory
    */
    void OnShowFolderContents(ui::TreeNode* pTreeNode, const ui::FilePath& currentPath,
                              const std::shared_ptr<std::vector<ui::DirectoryTree::PathInfo>>& folderList,
                              const std::shared_ptr<std::vector<ui::DirectoryTree::PathInfo>>& fileList);

    /** Show the content of the "Computer" node
    * @param [in] pTreeNode the current node
    * @param [in] diskInfoList the information list of all disks
    */
    void OnShowMyComputerContents(ui::TreeNode* pTreeNode,
                                  const std::vector<ui::DirectoryTree::DiskInfo>& diskInfoList);

    /** The path in the address bar has changed
    */
    bool OnAddressBarPathChanged(const ui::EventArgs& msg);

    /** A sub-path button in the address bar was clicked
    */
    bool OnAddressBarPathClick(const ui::EventArgs& msg);

    /** Refresh
    */
    void Refresh();

    /** After refreshing the left tree completes, update the file display area
    */
    void OnRefresh();

    /** Go up
    */
    void ShowUp();

    /** Go back
    */
    void ShowBack();

    /** Go forward
    */
    void ShowForward();

    /** Switch view mode
    */
    void SwithListType(const ui::UiPoint& point, ui::Control* pRelatedControl);

    /** Switch sort order
    */
    void SwithSortMode(const ui::UiPoint& point, ui::Control* pRelatedControl);

    /** Update the UI state
    */
    void UpdateCommandUI();

private:
    /** Set the currently selected tree node
    */
    void SetShowTreeNode(ui::TreeNode* pTreeNode);

    /** Show the address in the address bar
    */
    bool OnShowAddressPath(const DString& newFilePath);

private:
    /** View types of the TabBox (enum values follow the same order as defined in the XML)
    */
    enum TabBoxViewType
    {
        kFileView       = 0,    // File list view
        kExplorerView   = 1,    // File browser view
        kComputerView   = 2,    // "Computer" view
        kErrorView      = 3     // Error view
    };

    // Data view type
    enum DataViewType
    {
        kIconViewBig    = 0,    // Icon view (large icons)
        kIconViewMedium = 1,    // Icon view (medium icons)
        kIconViewSmall  = 2,    // Icon view (small icons)
        kListViewBig    = 3,    // List view (large icons)
        kListViewMedium = 4,    // List view (medium icons)
        kListViewSmall  = 5,    // List view (small icons)
        kReprortView    = 6,    // Details view
        kPictureView    = 7     // Image list view
    };

    /** Switch the view of the TabBox
    */
    void SwitchToTabBoxViewType(TabBoxViewType tabBoxViewType);

    /** Switch
    */
    void SwitchToDataViewType(DataViewType dataViewType);

    /** Get the current data view type
    */
    DataViewType GetDataViewType() const;

private:
    /** Interface of the left tree node
    */
    ui::DirectoryTree* m_pTree;

    /** Current path display
    */
    ui::AddressBar* m_pAddressBar;

    /** TabBox container
    */
    ui::TabBox* m_pTabBox;

private:
    /** "Computer" view
    */
    std::unique_ptr<ComputerView> m_pComputerView;

    /** Right file list view: simple view
    */
    std::unique_ptr<SimpleFileView> m_pSimpleFileView;

    /** Right file list view: file browser view
    */
    std::unique_ptr<ExplorerView> m_pExplorerView;

    /** View types of the TabBox (enum values follow the same order as defined in the XML)
    */
    TabBoxViewType m_tabBoxViewType;

    /** The current data view type
    */
    DataViewType m_dataViewType;

private:
    /** The currently displayed tree node
    */
    ui::TreeNode* m_pTreeNode;

    /** Parent node list of the currently selected tree node (used by the refresh mechanism)
    */
    std::vector<ui::TreeNode*> m_parentTreeNodes;

private:
    /** Up button
    */
    ui::Button* m_pBtnUp;

    /** Back button
    */
    ui::Button* m_pBtnBack;

    /** Forward button
    */
    ui::Button* m_pBtnForward;

    /** Back list
    */
    std::stack<ui::TreeNode*> m_backStack;

    /** Forward list
    */
    std::stack<ui::TreeNode*> m_forwardStack;

    /** Save the file or directory selected under each path, preserving the selection when navigating back/forward to improve the experience
    */
    std::unordered_map<ui::FilePath, ui::FilePath> m_selectedPathInfo;

    /** Whether it can be added to the back/forward list
    */
    bool m_bCanAddBackForward;

    /** Switch list type
    */
    ui::ButtonHBox* m_pBtnViewListType;

    /** Switch sort mode
    */
    ui::ButtonHBox* m_pBtnViewSort;
};

#endif //EXAMPLES_MAIN_FORM_H_
