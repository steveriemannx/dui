#include "dui/Utils/FileDialog.h"
#include "dui/Core/Window.h"
#include "dui/Core/Control.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/StringUtil.h"

#if defined(DUI_BUILD_FOR_MACOS)

#import <Cocoa/Cocoa.h>

namespace ui
{

/** Converts a dui FilePath to an NSString path
*/
static NSString* FilePathToNSString(const FilePath& filePath)
{
    if (filePath.IsEmpty()) {
        return nil;
    }
    const std::string strPath = StringConvert::TToUTF8(filePath.ToString());
    if (strPath.empty()) {
        return nil;
    }
    return [NSString stringWithUTF8String:strPath.c_str()];
}

/** Converts an NSString path to a dui FilePath
*/
static FilePath NSStringToFilePath(NSString* strPath)
{
    if ((strPath == nil) || (strPath.length == 0)) {
        return FilePath();
    }
    return FilePath(StringConvert::UTF8ToT(std::string(strPath.UTF8String)));
}

/** Applies the file type filters to an NSOpenPanel
*/
static void ApplyFileTypes(NSOpenPanel* panel, const std::vector<FileDialog::FileType>& fileTypes, int32_t nFileTypeIndex)
{
    if (panel == nil) {
        return;
    }
    NSMutableArray<NSString*>* allowedTypes = [NSMutableArray array];
    for (const FileDialog::FileType& fileType : fileTypes) {
        if (!fileType.szExt.empty()) {
            //Strip a leading dot, if any (e.g. "*.txt" -> "txt")
            DString ext = fileType.szExt;
            if (!ext.empty() && (ext[0] == _T('.'))) {
                ext.erase(ext.begin());
            }
            std::string utf8Ext = StringConvert::TToUTF8(ext);
            if (!utf8Ext.empty()) {
                [allowedTypes addObject:[NSString stringWithUTF8String:utf8Ext.c_str()]];
            }
        }
    }
    if (allowedTypes.count > 0) {
        //allowedFileTypes is deprecated since macOS 11 but works on every version;
        //using it avoids linking the UniformTypeIdentifiers framework.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        panel.allowedFileTypes = allowedTypes;
#pragma clang diagnostic pop
    }
    UNUSED_VARIABLE(nFileTypeIndex);
}

bool FileDialog::BrowseForFolder(Window* pWindow, FilePath& folderPath, const FilePath& defaultLocation)
{
    UNUSED_VARIABLE(pWindow);
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    panel.canChooseFiles = NO;
    panel.canChooseDirectories = YES;
    panel.allowsMultipleSelection = NO;
    if (!defaultLocation.IsEmpty()) {
        NSString* dirPath = FilePathToNSString(defaultLocation);
        if (dirPath != nil) {
            panel.directoryURL = [NSURL fileURLWithPath:dirPath isDirectory:YES];
        }
    }
    if ([panel runModal] == NSModalResponseOK) {
        NSURL* url = panel.URL;
        if (url != nil) {
            folderPath = NSStringToFilePath(url.path);
            return true;
        }
    }
    return false;
}

bool FileDialog::BrowseForFolders(Window* pWindow, std::vector<FilePath>& folderPaths, const FilePath& defaultLocation)
{
    UNUSED_VARIABLE(pWindow);
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    panel.canChooseFiles = NO;
    panel.canChooseDirectories = YES;
    panel.allowsMultipleSelection = YES;
    if (!defaultLocation.IsEmpty()) {
        NSString* dirPath = FilePathToNSString(defaultLocation);
        if (dirPath != nil) {
            panel.directoryURL = [NSURL fileURLWithPath:dirPath isDirectory:YES];
        }
    }
    if ([panel runModal] == NSModalResponseOK) {
        folderPaths.clear();
        for (NSURL* url in panel.URLs) {
            if (url != nil) {
                folderPaths.push_back(NSStringToFilePath(url.path));
            }
        }
        return !folderPaths.empty();
    }
    return false;
}

bool FileDialog::BrowseForFile(Window* pWindow,
    FilePath& filePath,
    bool bOpenFileDialog,
    const std::vector<FileDialog::FileType>& fileTypes,
    int32_t nFileTypeIndex,
    const DString& defaultExt,
    const DString& fileName,
    const FilePath& defaultFilePath)
{
    UNUSED_VARIABLE(pWindow);
    if (bOpenFileDialog) {
        //Open file dialog
        NSOpenPanel* panel = [NSOpenPanel openPanel];
        panel.canChooseFiles = YES;
        panel.canChooseDirectories = NO;
        panel.allowsMultipleSelection = NO;
        ApplyFileTypes(panel, fileTypes, nFileTypeIndex);
        if (!defaultFilePath.IsEmpty()) {
            NSString* dirPath = FilePathToNSString(defaultFilePath);
            if (dirPath != nil) {
                panel.directoryURL = [NSURL fileURLWithPath:dirPath isDirectory:YES];
            }
        }
        if ([panel runModal] == NSModalResponseOK) {
            NSURL* url = panel.URL;
            if (url != nil) {
                filePath = NSStringToFilePath(url.path);
                return true;
            }
        }
        return false;
    }

    //Save file dialog
    NSSavePanel* panel = [NSSavePanel savePanel];
    if (!fileName.empty()) {
        std::string utf8Name = StringConvert::TToUTF8(fileName);
        if (!utf8Name.empty()) {
            panel.nameFieldStringValue = [NSString stringWithUTF8String:utf8Name.c_str()];
        }
    }
    if (!defaultExt.empty()) {
        std::string utf8Ext = StringConvert::TToUTF8(defaultExt);
        if (!utf8Ext.empty()) {
            panel.allowedFileTypes = @[[NSString stringWithUTF8String:utf8Ext.c_str()]];
        }
    }
    if (!defaultFilePath.IsEmpty()) {
        NSString* dirPath = FilePathToNSString(defaultFilePath);
        if (dirPath != nil) {
            panel.directoryURL = [NSURL fileURLWithPath:dirPath isDirectory:YES];
        }
    }
    if ([panel runModal] == NSModalResponseOK) {
        NSURL* url = panel.URL;
        if (url != nil) {
            filePath = NSStringToFilePath(url.path);
            return true;
        }
    }
    return false;
}

bool FileDialog::BrowseForFiles(Window* pWindow,
    std::vector<FilePath>& filePaths,
    const std::vector<FileDialog::FileType>& fileTypes,
    int32_t nFileTypeIndex,
    const DString& defaultExt,
    const FilePath& defaultLocation)
{
    UNUSED_VARIABLE(pWindow);
    UNUSED_VARIABLE(defaultExt);
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    panel.canChooseFiles = YES;
    panel.canChooseDirectories = NO;
    panel.allowsMultipleSelection = YES;
    ApplyFileTypes(panel, fileTypes, nFileTypeIndex);
    if (!defaultLocation.IsEmpty()) {
        NSString* dirPath = FilePathToNSString(defaultLocation);
        if (dirPath != nil) {
            panel.directoryURL = [NSURL fileURLWithPath:dirPath isDirectory:YES];
        }
    }
    if ([panel runModal] == NSModalResponseOK) {
        filePaths.clear();
        for (NSURL* url in panel.URLs) {
            if (url != nil) {
                filePaths.push_back(NSStringToFilePath(url.path));
            }
        }
        return !filePaths.empty();
    }
    return false;
}

} // namespace ui

#endif // defined(DUI_BUILD_FOR_MACOS)
