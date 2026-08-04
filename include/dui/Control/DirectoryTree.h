#ifndef UI_CONTROL_DIRECTORYTREE_H_
#define UI_CONTROL_DIRECTORYTREE_H_

#include "dui/Control/TreeView.h"
#include "dui/Utils/FileTime.h"
#include <vector>
#include <atomic>

namespace ui
{
/** Type of the virtual directory
*/
enum class DUI_API VirtualDirectoryType
{
    kUserHome,  //Home folder
    kDesktop,   //Desktop
    kDocuments, //Documents
    kPictures,  //Pictures
    kMusic,     //Music
    kVideos,    //Videos
    kDownloads, //Downloads
};

class DirectoryTreeImpl;

/** Directory tree structure of the file system
*/
class DUI_API DirectoryTree: public TreeView
{
public:
    typedef TreeView BaseClass;

    /** Attribute information of a file or path
    */
    struct PathInfo
    {
        FilePath m_filePath;        //File path        
        DString m_displayName;      //Display name
        DString m_typeName;         //File type
        uint64_t m_fileSize = 0;    //File size
        FileTime m_lastWriteTime;   //Last modified time
        uint32_t m_nIconID = 0;     //Associated icon ID (the return value of GlobalManager::Instance().Icon().AddIcon; the icon needs to be added to the manager by the implementing class)
        bool m_bIconShared = false; //Whether the icon associated with this icon ID is a shared icon (shared icons are not allowed to be released)
        bool m_bFolder = true;      //Whether it is a directory
    };

    /** Callback prototype: used to display data associated with folder contents
    */
    typedef std::function<void (ui::TreeNode* pTreeNode, const ui::FilePath& path,
                                const std::shared_ptr<std::vector<ui::DirectoryTree::PathInfo>>& folderList,
                                const std::shared_ptr<std::vector<ui::DirectoryTree::PathInfo>>& fileList)> ShowFolderContentsEvent;

#ifdef DUI_BUILD_FOR_WIN
    /** Disk attribute information
    */
    struct DiskInfo
    {
        DString m_displayName;  //Display name
        uint32_t m_nIconID = 0; //Associated icon ID (the return value of GlobalManager::Instance().Icon().AddIcon; the icon needs to be added to the manager by the implementing class)
        bool m_bIconShared = false; //Whether the icon associated with this icon ID is a shared icon (shared icons are not allowed to be released)
        FilePath m_filePath;        //Corresponding path

        DString m_volumeName;   //Volume label of the partition
        DString m_volumeType;   //Partition type, e.g., "Local Disk"
        DString m_fileSystem;   //File system type, e.g., "NTFS"

        uint64_t m_totalBytes = 0; //Total space size (bytes)
        uint64_t m_freeBytes = 0;  //Available space size (bytes)
    };
#else
    //Other platforms
    /** Device type enumeration
    */
    enum class DeviceType
    {
        UNKNOWN,    // Unknown device
        HDD,        // Mechanical hard disk (e.g., /dev/sdX)
        SSD,        // SATA solid-state drive
        NVME,       // NVMe solid-state drive (e.g., /dev/nvmeXn1)
        USB,        // USB storage device
        SD_CARD,    // SD card
        CDROM,      // CDROM DVDROM
        LOOP,       // LOOP virtual device
        VIRT_DISK,  // Virtual device
        RAMDISK,    // RAM disk (e.g., /dev/ramX)
        NFS,        // NFS
        SHARE       // Shared folder
    };

    /** Disk attribute information
    */
    struct DiskInfo
    {
        DString m_displayName;      //Display name
        uint32_t m_nIconID = 0;     //Associated icon ID (the return value of GlobalManager::Instance().Icon().AddIcon; the icon needs to be added to the manager by the implementing class)
        bool m_bIconShared = false; //Whether the icon associated with this icon ID is a shared icon (shared icons are not allowed to be released)
        FilePath m_filePath;        //Corresponding path

        DString m_volumeName;   //File system, e.g., "/dev/sda2"
        DeviceType m_deviceType = DeviceType::UNKNOWN;   //Device type, see the enum values
        DString m_fileSystem;   //File system type, e.g., "ext3"
        DString m_mountOn;      //Mount point, e.g., "/"

        uint64_t m_totalBytes = 0; //Total space size (bytes)
        uint64_t m_freeBytes = 0;  //Available space size (bytes)
    };
#endif

    /** Callback prototype: used to display the data of the computer view
    */
    typedef std::function<void (ui::TreeNode* pTreeNode, 
                                const std::vector<ui::DirectoryTree::DiskInfo>& diskInfoList)> ShowMyComputerContentsEvent;

public:
    explicit DirectoryTree(Window* pWindow);
    DirectoryTree(const DirectoryTree& r) = delete;
    DirectoryTree& operator=(const DirectoryTree& r) = delete;
    virtual ~DirectoryTree() override;

    /// Override the base class method to provide customized functionality; refer to the base class declaration
    virtual DString GetType() const override;
    virtual void SetAttribute(const DString& strName, const DString& strValue) override;

    /** Clear the path list and release icon resources as needed
    * @param [in,out] pathList Input file list; it is cleared on output
    */
    static void ClearPathInfoList(std::vector<PathInfo>& pathList);

    /** Clear the disk info list and release icon resources as needed
    * @param [in,out] pathList Input file list; it is cleared on output
    */
    static void ClearDiskInfoList(std::vector<DirectoryTree::DiskInfo>& diskList);

public:
    /** Set the thread identifier of the worker thread (directory enumeration is done in the worker thread to avoid making the UI unresponsive)
    * @param [in] nThreadIdentifier The thread identifier of the worker thread
    */
    void SetThreadIdentifier(int32_t nThreadIdentifier);

    /** Show the virtual directory (completed synchronously)
    * @param [in] type Type of the virtual directory
    * @param [in] displayName Display name of the virtual directory
    * @param [in] bDisplayNameIsID Whether the display name of the virtual directory is a language ID to support multiple languages
    */
    TreeNode* ShowVirtualDirectoryNode(VirtualDirectoryType type, const DString& displayName, bool bDisplayNameIsID = false);

    /** Show all disk nodes and return the first new node interface (completed synchronously)
    * @param [in] computerName Display name of the computer node
    * @param [in] fileSystemName Display name of the file system
    */
    TreeNode* ShowAllDiskNodes(const DString& computerName, const DString& fileSystemName);

    /** Insert a horizontal separator line before the specified node (completed synchronously)
    * @param [in] pNode Node interface
    * @param [in] lineClassName Class name of the horizontal separator line (optional)
    */
    bool InsertLineBeforeNode(TreeNode* pNode, const DString& lineClassName = _T(""));

    /** Add a node to the tree and return the interface of the newly added node (completed synchronously)
    * @param [in] pParentTreeNode Interface of the parent node; if nullptr, the new node is added under the root node
    * @param [in] displayName Display name of the virtual directory
    * @param [in] bDisplayNameIsID Whether the display name of the virtual directory is a language ID to support multiple languages
    * @param [in] path Corresponding path
    * @param [in] isFolder true means it is a folder; otherwise it is a regular file
    * @param [in] nIconID Icon ID (managed by GlobalManager::Instance().Icon()); if 0, no icon is associated
    * @param [in] bIconShared Whether the icon associated with this icon ID is a shared icon (shared icons are not allowed to be released)
    * @param [in] bVirtualNode true means this node is a virtual node; otherwise it is a normal path node
    */
    TreeNode* InsertTreeNode(TreeNode* pParentTreeNode,
                             const DString& displayName,
                             bool bDisplayNameIsID,
                             const ui::FilePath& path,
                             bool isFolder,
                             bool bVirtualNode,
                             uint32_t nIconID,
                             bool bIconShared);

    /** Get the tree node corresponding to a path (completed synchronously)
    * @param [in] filePath The path to look up
    */
    TreeNode* FindPathTreeNode(FilePath filePath) const;

    /** Get the path corresponding to a tree node (completed synchronously)
    * @param [in] pTreeNode A tree node
    */
    FilePath FindTreeNodePath(TreeNode* pTreeNode);

    /** Determine whether a tree node is the "Computer" node, which requires special handling
    * @param [in] pTreeNode The current node
    */
    bool IsMyComputerNode(TreeNode* pTreeNode) const;

public:
    /** Set the callback function used to display the associated data
    * @param [in] callback Callback function
    */
    void AttachShowFolderContents(ShowFolderContentsEvent callback);

    /** Set the callback function used to display the "Computer" node info view
    */
    void AttachShowMyComputerContents(ShowMyComputerContentsEvent callback);

public:
    /** Select a tree node (if the parent node is not expanded, expand it cascadingly)
    * @param [in] pTreeNode A tree node
    */
    virtual bool SelectTreeNode(TreeNode* pTreeNode) override;

    /** Expand the tree node and select its subdirectory (completed asynchronously)
    * @param [in] pTreeNode A tree node
    * @param [in] subPath Subdirectory or multi-level subdirectory; the path is a subdirectory of the path corresponding to pTreeNode
    * @param [in] finishCallback Callback function invoked after the refresh is completed
    */
    bool SelectSubPath(TreeNode* pTreeNode, FilePath subPath, StdClosure finishCallback);

    /** Select a path (expand directories level by level, select the final directory, and ensure it is visible) (completed asynchronously)
    * @param [in] filePath The path to select
    * @param [in] finishCallback Callback function invoked after the refresh is completed
    */
    bool SelectPath(FilePath filePath, StdClosure finishCallback);

    /** Refresh to keep the tree structure in sync with the file system (completed asynchronously)
    * @param [in] finishCallback Callback function invoked after the refresh is completed
    */
    bool RefreshTree(StdClosure finishCallback);

    /** Refresh tree nodes to keep the tree structure in sync with the file system (completed asynchronously)
    * @param [in] pTreeNode A tree node
    * @param [in] finishCallback Callback function invoked after the refresh is completed
    */
    bool RefreshTreeNode(TreeNode* pTreeNode, StdClosure finishCallback);

    /** Refresh tree nodes to keep the tree structure in sync with the file system (completed asynchronously)
    * @param [in] treeNodes List of tree nodes
    * @param [in] finishCallback Callback function invoked after the refresh is completed
    */
    bool RefreshTreeNodes(const std::vector<TreeNode*>& treeNodes, StdClosure finishCallback);

    /** Refresh the contents of the directory corresponding to the specified node (completed asynchronously)
    * @param [in] pTreeNode A tree node
    * @param [in] finishCallback Callback function invoked after the refresh is completed
    */
    bool RefreshFolderContents(TreeNode* pTreeNode, StdClosure finishCallback);

    /** Set the default refresh-completion event callback function
    * @param [in] finishCallback Callback function invoked after the refresh is completed
    */
    void SetRefreshFinishCallback(StdClosure finishCallback);

public:
    /** Set the small icon size; only applies to newly added nodes
    */
    void SetSmallIconSize(int32_t nIconSize);

    /** Get the small icon size (width and height are the same)
    */
    int32_t GetSmallIconSize() const;

    /** Set the large icon size; only applies to newly added nodes
    */
    void SetLargeIconSize(int32_t nIconSize);

    /** Get the large icon size (width and height are the same)
    */
    int32_t GetLargeIconSize() const;

    /** Set whether to show hidden files
    */
    void SetShowHidenFiles(bool bShowHidenFiles);

    /** Get whether hidden files are shown
    */
    bool IsShowHidenFiles() const;

    /** Set whether to show system files
    */
    void SetShowSystemFiles(bool bShowSystemFiles);

    /** Get whether system files are shown
    */
    bool IsShowSystemFiles() const;

private:
    /** Tree node expand event
     * @param[in] args Message body
     * @return Always returns true
     */
    bool OnTreeNodeExpand(const EventArgs& args);

    /** Tree node click event
     * @param[in] args Message body
     * @return Always returns true
     */
    bool OnTreeNodeClick(const EventArgs& args);

    /** Tree node destroy event
     * @param[in] args Message body
     * @return Always returns true
     */
    bool OnTreeNodeDestroy(const EventArgs& args);

    /** Show the subdirectories of the specified directory (completed asynchronously)
    * @param [in] pTreeNode The current node
    * @param [in] path Path
    */
    void ShowSubFolders(TreeNode* pTreeNode, const FilePath& path, StdClosure finishCallback);

    /** Show the contents of the specified directory (completed asynchronously)
    * @param [in] pTreeNode The current node
    * @param [in] path Path
    */
    void ShowFolderContents(TreeNode* pTreeNode, const FilePath& path, StdClosure finishCallback);

    /** Show the contents of the "Computer" node
    */
    void ShowMyComputerContents(TreeNode* pTreeNode, StdClosure finishCallback);

    /** Determine whether two tree nodes are in a parent-child relationship
    * @param [in] pTreeNode Parent node
    * @param [in] pChildTreeNode Child node
    */
    bool IsChildTreeNode(TreeNode* pTreeNode, TreeNode* pChildTreeNode) const;

    /** Determine whether a path is the same as the directory corresponding to the specified node
    * @param [in] pTreeNode The current node
    * @param [in] path Path
    */
    bool IsPathSame(TreeNode* pTreeNode, FilePath path) const;

private:
    /** Show the subdirectories of the specified directory
    * @param [in] pTreeNode The current node
    * @param [in] path Path
    * @param [in] folderList Returns the list of all subdirectories in the path directory
    */
    typedef std::shared_ptr<std::vector<DirectoryTree::PathInfo>> PathInfoListPtr;
    bool OnShowSubFolders(TreeNode* pTreeNode, const FilePath& path, const PathInfoListPtr& folderList);

    /** Show the subdirectories of the specified directory (multi-level subdirectories)
    * @param [in] pTreeNode The current node
    * @param [in] filePathList List of paths
    * @param [in] folderList Returns the list of all subdirectories in each directory
    */
    bool OnShowSubFoldersEx(TreeNode* pTreeNode,
                            const std::vector<FilePath>& filePathList,
                            const std::vector<PathInfoListPtr>& folderListArray);

    /** The contents of the specified directory have been obtained
    * @param [in] pTreeNode The current node
    * @param [in] path Path
    * @param [in] folderList List of all subdirectories in the path directory
    * @param [in] fileList List of all files in the path directory
    */
    bool OnShowFolderContents(ui::TreeNode* pTreeNode, const ui::FilePath& path,
                              const PathInfoListPtr& folderList,
                              const PathInfoListPtr& fileList);

    /** Show the contents of the "Computer" node
    * @param [in] pTreeNode The current node
    * @param [in] diskInfoList List of information for all disks
    */
    bool OnShowMyComputerContents(ui::TreeNode* pTreeNode,
                                  const std::vector<ui::DirectoryTree::DiskInfo>& diskInfoList);

    /** Expand subdirectories under a tree node and select the last-level directory
    * @param [in] pTreeNode The current node
    * @param [in] filePathList Directories to expand (these directories have not yet been added to the tree node)
    * @param [in] finishCallback Completion callback function
    */
    bool OnSelectSubPath(TreeNode* pTreeNode, std::vector<FilePath> filePathList, StdClosure finishCallback);

    /** Compare whether two paths are the same
    */
    bool IsSamePath(const UiString& p1, const UiString& p2) const;

private:

    /** Directory list data structure
    */
    struct FolderStatus
    {
        UiString m_filePath;                //Normalized file path
        TreeNode* m_pTreeNode = nullptr;    //Associated tree node pointer
        uint32_t m_nIconID = 0;             //Associated icon ID
        bool m_bContentLoaded = false;      //Whether the subdirectories of the current directory have been loaded
        bool m_bFolder = true;              //Whether it is a folder
        bool m_bIconShared = false;         //Whether the icon associated with this icon ID is a shared icon (shared icons are not allowed to be released)        
    };
    /** Delete directory list data
    */
    void DeleteFolderStatus(FolderStatus* pFolderStatus);

    /** Get directory structure data by key
    */
    FolderStatus* GetFolderData(TreeNode* pTreeNode) const;

    /** Find directory structure data by path
    */
    FolderStatus* GetFolderData(FilePath filePath) const;

private:
    /** Data structure for refreshing tree nodes
    */
    struct RefreshNodeData
    {
        TreeNode* m_pTreeNode = nullptr;            //Tree node
        std::weak_ptr<WeakFlag> m_weakFlag;         //Lifetime of the tree node
        bool m_bContentLoaded = false;              //Whether the subdirectories of the current directory have been loaded
        FilePath m_dirPath;                         //The directory corresponding to the tree node
        size_t m_nParentIndex = 0;                  //Index of the parent node in the container
        std::vector<FilePath> m_childPaths;         //Child node and subdirectory data, used to compare whether there are added directories

        //Whether the current path has been deleted
        bool m_bDeleted = false;

        //List of newly added paths in the current directory
        std::vector<DirectoryTree::PathInfo> m_newFolderList;
    };

    /** Recursively get tree node data from the UI and form a list
    */
    void GetTreeNodeData(size_t nParentIndex, TreeNode* pTreeNode, std::vector<std::shared_ptr<RefreshNodeData>>& refreshData) const;

    /** Read the latest state of the file system based on the directory tree (mark deleted directories and add newly created ones)
    */
    void RefreshPathInfo(std::vector<std::shared_ptr<RefreshNodeData>>& refreshData);

    /** Update the tree structure based on the latest state
    */
    void UpdateTreeNodeData(const std::vector<std::shared_ptr<RefreshNodeData>>& refreshData);

private:
    /** Internal implementation of directory enumeration (different implementations on different platforms)
    */
    DirectoryTreeImpl* m_impl;

    /** Thread identifier of the worker thread
    */
    int32_t m_nThreadIdentifier;

    /** Size of the small icon
    */
    int32_t m_nSmallIconSize;

    /** Size of the large icon
    */
    int32_t m_nLargeIconSize;

    /** Whether to show hidden files
    */
    bool m_bShowHidenFiles;

    /** Whether to show system files
    */
    bool m_bShowSystemFiles;

    /** Key value of the directory tree
    */
    size_t m_folderKey;

    /** Directory tree (shown as the tree on the left side)
    */
    std::map<size_t, FolderStatus*> m_folderMap;

    /** Callback functions used to display the associated folder data
    */
    std::vector<ShowFolderContentsEvent> m_callbackList;

    /** Callback functions used to display the associated "Computer" data
    */
    std::vector<ShowMyComputerContentsEvent> m_myComputerCallbackList;

    /** Default callback function invoked after a refresh is completed
    */
    StdClosure m_defaultRefreshFinishCallback;
};

}

#endif // UI_CONTROL_DIRECTORYTREE_H_
