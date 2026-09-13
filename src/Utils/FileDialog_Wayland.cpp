#include "dui/Utils/FileDialog.h"
#include "dui/Core/Window.h"
#include "dui/Core/Control.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/StringUtil.h"

// This file only provides FileDialog definitions for Wayland
// builds; on native Windows, FileDialog_Windows.cpp is the implementation
// (compiling the stubs here too defined both, producing duplicate symbols).
#if defined(DUI_BUILD_FOR_WAYLAND)

namespace ui
{

bool FileDialog::BrowseForFolder(Window* pWindow, FilePath& folderPath, const FilePath& defaultLocation)
{
    (void)folderPath;
    (void)pWindow; (void)folderPath; (void)defaultLocation;
    return false;
}

bool FileDialog::BrowseForFolders(Window* pWindow, std::vector<FilePath>& folderPaths, const FilePath& defaultLocation)
{
    (void)folderPaths;
    (void)pWindow; (void)folderPaths; (void)defaultLocation;
    return false;
}

bool FileDialog::BrowseForFile(Window* pWindow,
    FilePath& filePath,
    bool bOpenFileDialog,
    const std::vector<FileType>& fileTypes,
    int32_t nFileTypeIndex,
    const std::string& defaultExt,
    const std::string& fileName,
    const FilePath& defaultFilePath)
{
    (void)pWindow; (void)filePath; (void)bOpenFileDialog; (void)fileTypes;
    (void)nFileTypeIndex; (void)defaultExt; (void)fileName; (void)defaultFilePath;
    return false;
}

bool FileDialog::BrowseForFiles(Window* pWindow,
    std::vector<FilePath>& filePaths,
    const std::vector<FileType>& fileTypes,
    int32_t nFileTypeIndex,
    const std::string& defaultExt,
    const FilePath& defaultLocation)
{
    (void)pWindow; (void)filePaths; (void)fileTypes;
    (void)nFileTypeIndex; (void)defaultExt; (void)defaultLocation;
    return false;
}

} // namespace ui

#endif // DUI_BUILD_FOR_WAYLAND
