#include "dui/Control/DirectoryTreeImpl.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/FilePath.h"
#include <filesystem>
#include <cstdlib>

namespace ui
{
struct DirectoryTreeImpl::TImpl
{
    uint32_t m_nLargeFolderIconID = 0;
    uint32_t m_nSmallFolderIconID = 0;
    uint32_t m_nLargeFileIconID = 0;
    uint32_t m_nSmallFileIconID = 0;
};

DirectoryTreeImpl::DirectoryTreeImpl(DirectoryTree* pTree): m_pTree(pTree)
{
    m_impl = new TImpl;
}

DirectoryTreeImpl::~DirectoryTreeImpl()
{
    if (m_impl->m_nLargeFolderIconID != 0) GlobalManager::Instance().Icon().RemoveIcon(m_impl->m_nLargeFolderIconID);
    if (m_impl->m_nSmallFolderIconID != 0) GlobalManager::Instance().Icon().RemoveIcon(m_impl->m_nSmallFolderIconID);
    if (m_impl->m_nLargeFileIconID != 0) GlobalManager::Instance().Icon().RemoveIcon(m_impl->m_nLargeFileIconID);
    if (m_impl->m_nSmallFileIconID != 0) GlobalManager::Instance().Icon().RemoveIcon(m_impl->m_nSmallFileIconID);
    delete m_impl;
    m_impl = nullptr;
}

static U8String DirectoryTreeImplGetImageString(DirectoryTree* pTree, bool bLargeFile, const U8String& imageFileName)
{
    if (imageFileName.empty()) return U8String();
    int32_t nSmallIconSize = 20;
    int32_t nLargeIconSize = 32;
    if (pTree != nullptr) {
        nSmallIconSize = pTree->GetSmallIconSize();
        nLargeIconSize = pTree->GetLargeIconSize();
    }
    if (nSmallIconSize < 1) nSmallIconSize = 20;
    if (nLargeIconSize < 1) nLargeIconSize = 32;
    int32_t nIconSize = bLargeFile ? nLargeIconSize : nSmallIconSize;
    U8String imageString = StringUtil::Printf(("file='public/filesystem/%s' width='%d' height='%d' valign='center'"), imageFileName.c_str(), nIconSize, nIconSize);
    return imageString;
}

bool DirectoryTreeImpl::GetVirtualDirectoryInfo(VirtualDirectoryType type, FilePath& filePath, U8String& displayName, uint32_t& nIconID)
{
    filePath.Clear();
    displayName.clear();
    nIconID = 0;
    FilePath userHomeDir;
    const char* home = std::getenv("HOME");
#ifdef _WIN32
    if (home == nullptr) home = std::getenv("USERPROFILE");
#endif
    if (home != nullptr) userHomeDir = FilePath(home);
    else userHomeDir = ("/");
    userHomeDir.NormalizeDirectoryPath();
    switch (type) {
    case VirtualDirectoryType::kUserHome:
        filePath = userHomeDir;
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, ("folder-home.svg")));
        break;
    case VirtualDirectoryType::kDesktop:
        filePath = userHomeDir; filePath += ("Desktop");
        if (!filePath.IsExistsDirectory()) filePath = userHomeDir;
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, ("folder-desktop.svg")));
        break;
    case VirtualDirectoryType::kDocuments:
        filePath = userHomeDir; filePath += ("Documents");
        if (!filePath.IsExistsDirectory()) filePath = userHomeDir;
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, ("folder-documents.svg")));
        break;
    case VirtualDirectoryType::kPictures:
        filePath = userHomeDir; filePath += ("Pictures");
        if (!filePath.IsExistsDirectory()) filePath = userHomeDir;
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, ("folder-pictures.svg")));
        break;
    case VirtualDirectoryType::kMusic:
        filePath = userHomeDir; filePath += ("Music");
        if (!filePath.IsExistsDirectory()) filePath = userHomeDir;
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, ("folder-music.svg")));
        break;
    case VirtualDirectoryType::kVideos:
        filePath = userHomeDir; filePath += ("Videos");
        if (!filePath.IsExistsDirectory()) filePath = userHomeDir;
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, ("folder-videos.svg")));
        break;
    case VirtualDirectoryType::kDownloads:
        filePath = userHomeDir; filePath += ("Downloads");
        if (!filePath.IsExistsDirectory()) filePath = userHomeDir;
        nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, ("folder-download.svg")));
        break;
    default: break;
    }
    return true;
}

void DirectoryTreeImpl::GetRootPathInfoList(bool bLargeIcon, std::vector<DirectoryTree::PathInfo>& pathInfoList)
{
    pathInfoList.clear();
#ifdef _WIN32
    // Windows: enumerate drives A-Z via filesystem
    for (char d = 'C'; d <= 'Z'; ++d) {
        std::string drive = std::string(1, d) + ":\\";
        std::error_code ec;
        if (std::filesystem::exists(drive, ec)) {
            DirectoryTree::PathInfo pi;
            pi.m_bFolder = true;
            pi.m_filePath = FilePath(StringConvert::UTF8ToT(drive));
            pi.m_displayName = pi.m_filePath.ToString();
            pi.m_bIconShared = false;
            pi.m_nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, ("drive-harddisk.svg")));
            pathInfoList.push_back(pi);
        }
    }
    if (pathInfoList.empty()) {
        DirectoryTree::PathInfo pi;
        pi.m_bFolder = true;
        pi.m_filePath = FilePath(("C:\\"));
        pi.m_displayName = pi.m_filePath.ToString();
        pi.m_bIconShared = false;
        pi.m_nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, ("drive-harddisk.svg")));
        pathInfoList.push_back(pi);
    }
#else
    DirectoryTree::PathInfo pi;
    pi.m_bFolder = true;
    pi.m_filePath = FilePath(("/"));
    pi.m_displayName = pi.m_filePath.ToString();
    pi.m_bIconShared = false;
    pi.m_nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, ("drive-harddisk.svg")));
    pathInfoList.push_back(pi);
    std::weak_ptr<WeakFlag> weakFlag;
    std::vector<DirectoryTree::DiskInfo> disks;
    GetDiskInfoList(weakFlag, bLargeIcon, disks);
    for (auto &d : disks) {
        if (d.m_filePath == FilePath(("/"))) continue;
        DirectoryTree::PathInfo p2;
        p2.m_bFolder = true;
        p2.m_filePath = d.m_filePath;
        p2.m_displayName = d.m_volumeName;
        p2.m_bIconShared = d.m_bIconShared;
        p2.m_nIconID = d.m_nIconID;
        pathInfoList.push_back(p2);
    }
#endif
}

void DirectoryTreeImpl::GetFolderContents(const FilePath& path, const std::weak_ptr<WeakFlag>& weakFlag, bool bLargeIcon, std::vector<DirectoryTree::PathInfo>& folderList, std::vector<DirectoryTree::PathInfo>* fileList)
{
    folderList.clear();
    if (fileList) fileList->clear();
    try {
        std::filesystem::path stdPath(path.NativePathA());
        for (const auto& entry : std::filesystem::directory_iterator(stdPath)) {
            if (weakFlag.expired()) break;
            if (m_pTree && !m_pTree->IsShowHidenFiles()) {
                auto name = entry.path().filename().string();
                if (!name.empty() && name[0] == '.') continue;
            }
            std::error_code ec;
            if (entry.is_symlink(ec)) continue;
            bool isDir = entry.is_directory(ec);
            if (isDir) {
                DirectoryTree::PathInfo pi;
                pi.m_bFolder = true;
                pi.m_filePath = FilePath(entry.path().native());
                pi.m_displayName = pi.m_filePath.GetFileName();
                if (bLargeIcon) {
                    if (m_impl->m_nLargeFolderIconID == 0) m_impl->m_nLargeFolderIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, ("folder.svg")));
                    pi.m_nIconID = m_impl->m_nLargeFolderIconID;
                } else {
                    if (m_impl->m_nSmallFolderIconID == 0) m_impl->m_nSmallFolderIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, ("folder.svg")));
                    pi.m_nIconID = m_impl->m_nSmallFolderIconID;
                }
                pi.m_bIconShared = true;
                folderList.emplace_back(std::move(pi));
            } else if (fileList) {
                if (entry.is_regular_file(ec)) {
                    DirectoryTree::PathInfo pi;
                    pi.m_bFolder = false;
                    pi.m_filePath = FilePath(entry.path().native());
                    pi.m_displayName = pi.m_filePath.GetFileName();
                    if (bLargeIcon) {
                        if (m_impl->m_nLargeFileIconID == 0) m_impl->m_nLargeFileIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, ("file.svg")));
                        pi.m_nIconID = m_impl->m_nLargeFileIconID;
                    } else {
                        if (m_impl->m_nSmallFileIconID == 0) m_impl->m_nSmallFileIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, ("file.svg")));
                        pi.m_nIconID = m_impl->m_nSmallFileIconID;
                    }
                    pi.m_bIconShared = true;
                    try { pi.m_fileSize = (uint64_t)std::filesystem::file_size(entry.path(), ec); } catch(...) {}
                    folderList; // unused
                    fileList->emplace_back(std::move(pi));
                }
            }
        }
    } catch (...) {}
}

bool DirectoryTreeImpl::NeedShowDirPath(const FilePath& path) const
{
    if (m_pTree == nullptr || path.IsEmpty()) return false;
    if (!path.IsExistsDirectory()) return false;
    if (!m_pTree->IsShowHidenFiles()) {
        U8String s = path.NativePath();
        if (!s.empty() && s[0] == '.') return false;
    }
    return true;
}

uint32_t DirectoryTreeImpl::GetMyComputerIconID() const
{
    return GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, false, ("computer.svg")));
}

void DirectoryTreeImpl::GetDiskInfoList(const std::weak_ptr<WeakFlag>&, bool bLargeIcon, std::vector<DirectoryTree::DiskInfo>& diskInfoList)
{
    diskInfoList.clear();
#ifndef _WIN32
    // Generic fallback: report root only; platform-specific mount detection omitted for SDL cross-platform
    std::error_code ec;
    auto space = std::filesystem::space("/", ec);
    if (!ec) {
        DirectoryTree::DiskInfo d;
        d.m_displayName = ("/");
        d.m_filePath = FilePath(("/"));
        d.m_volumeName = ("/");
        d.m_deviceType = DirectoryTree::DeviceType::HDD;
        d.m_mountOn = ("/");
        d.m_fileSystem = ("unknown");
        d.m_totalBytes = space.capacity;
        d.m_freeBytes = space.available;
        d.m_bIconShared = false;
        d.m_nIconID = GlobalManager::Instance().Icon().AddIcon(DirectoryTreeImplGetImageString(m_pTree, bLargeIcon, ("drive-harddisk.svg")));
        diskInfoList.push_back(std::move(d));
    }
#endif
}

} // namespace ui
