#ifndef UI_CONTROL_DIRECTORYTREE_IMPL_H_
#define UI_CONTROL_DIRECTORYTREE_IMPL_H_

#include "DirectoryTree.h"

namespace ui
{
class DirectoryTree;

/** Implementation interface for enumerating the directory tree structure of the file system
*/
class DirectoryTreeImpl
{   
public:
    explicit DirectoryTreeImpl(DirectoryTree* pTree);
    DirectoryTreeImpl(const DirectoryTreeImpl& r) = delete;
    DirectoryTreeImpl& operator=(const DirectoryTreeImpl& r) = delete;
    ~DirectoryTreeImpl();

public:
    /** Get the real path/display name/icon of a virtual directory (icon size: 16*16)
    * @param [in] type Type of the virtual directory
    * @param [out] filePath Real path of the virtual directory
    * @param [out] displayName Display name of the virtual directory    
    * @param [out] nIconID Icon associated with the virtual directory (the return value of GlobalManager::Instance().Icon().AddIcon; the icon needs to be added to the manager by the implementing class)
    * @return Returns true on success, false on failure
    */
    bool GetVirtualDirectoryInfo(VirtualDirectoryType type, FilePath& filePath, DString& displayName, uint32_t& nIconID);

    /** Show the list of path nodes under all root directories (on Windows, returns all disk node data)
    * @param [in] bLargeIcon Whether to get the large icon
    * @param [out] pathInfoList Returns all disk node data (icon size: 16*16)
    */
    void GetRootPathInfoList(bool bLargeIcon, std::vector<DirectoryTree::PathInfo>& pathInfoList);

    /** Get the list of subdirectories and files under the specified path
    * @param [in] path Directory
    * @param [in] weakFlag Cancellation flag; the operation is cancelled if the associated control has become invalid
    * @param [in] bLargeIcon Whether it is the large icon; the large icon is 32*32 and the small icon is 16*16
    * @param [out] folderList Returns the list of all subdirectories in the path directory
    * @param [out] fileList Returns the list of all files in the path directory
    */
    void GetFolderContents(const FilePath& path,
                           const std::weak_ptr<WeakFlag>& weakFlag,
                           bool bLargeIcon,
                           std::vector<DirectoryTree::PathInfo>& folderList,
                           std::vector<DirectoryTree::PathInfo>* fileList);

    /** Get the disk list in "Computer" (on Windows, get the drive list; on other platforms, get the list of mounted nodes)
    * @param [in] weakFlag Cancellation flag; the operation is cancelled if the associated control has become invalid
    * @param [in] bLargeIcon Whether it is the large icon; the large icon is 32*32 and the small icon is 16*16
    * @param [out] diskInfoList Returns the disk list
    */
    void GetDiskInfoList(const std::weak_ptr<WeakFlag>& weakFlag,
                         bool bLargeIcon,
                         std::vector<DirectoryTree::DiskInfo>& diskInfoList);

    /** Whether this directory needs to be shown (determined by the property settings)
    */
    bool NeedShowDirPath(const FilePath& path) const;

    /** Get the icon ID of the computer
    */
    uint32_t GetMyComputerIconID() const;

private:
    /** Directory tree UI interface
    */
    DirectoryTree* m_pTree;

private:
    /** Private implementation data
    */
    struct TImpl;
    TImpl* m_impl;
};

}

#endif // UI_CONTROL_DIRECTORYTREE_IMPL_H_
