#include "duilib/Utils/FileDialog.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/Control.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Utils/StringUtil.h"

#if defined(DUILIB_BUILD_FOR_SDL)
#include "duilib/Core/MessageLoop_SDL.h"
#include <SDL3/SDL.h>
#endif

namespace ui
{

bool FileDialog::BrowseForFolder(Window* pWindow, FilePath& folderPath, const FilePath& defaultLocation)
{
#if defined(DUILIB_BUILD_FOR_SDL)
    SDL_Window* sdlWindow = nullptr;
    if (pWindow != nullptr) {
        sdlWindow = (SDL_Window*)pWindow->NativeWnd()->GetWindowHandle();
    }
    // Use SDL dialog
    SDL_DialogFileCallback callback = NULL;
    void* userdata = NULL;
    const char* default_location = defaultLocation.IsEmpty() ? NULL : defaultLocation.ToString().c_str();
    SDL_ShowOpenFolderDialog(callback, userdata, sdlWindow, default_location, false);
    // SDL dialog is async, can't get result here
    (void)folderPath;
    return false;
#else
    (void)pWindow; (void)folderPath; (void)defaultLocation;
    return false;
#endif
}

bool FileDialog::BrowseForFolders(Window* pWindow, std::vector<FilePath>& folderPaths, const FilePath& defaultLocation)
{
#if defined(DUILIB_BUILD_FOR_SDL)
    SDL_Window* sdlWindow = nullptr;
    if (pWindow != nullptr) {
        sdlWindow = (SDL_Window*)pWindow->NativeWnd()->GetWindowHandle();
    }
    const char* default_location = defaultLocation.IsEmpty() ? NULL : defaultLocation.ToString().c_str();
    SDL_ShowOpenFolderDialog(NULL, NULL, sdlWindow, default_location, true);
    (void)folderPaths;
    return false;
#else
    (void)pWindow; (void)folderPaths; (void)defaultLocation;
    return false;
#endif
}

bool FileDialog::BrowseForFile(Window* pWindow,
    FilePath& filePath,
    bool bOpenFileDialog,
    const std::vector<FileType>& fileTypes,
    int32_t nFileTypeIndex,
    const DString& defaultExt,
    const DString& fileName,
    const FilePath& defaultFilePath)
{
#if defined(DUILIB_BUILD_FOR_SDL)
    SDL_Window* sdlWindow = nullptr;
    if (pWindow != nullptr) {
        sdlWindow = (SDL_Window*)pWindow->NativeWnd()->GetWindowHandle();
    }
    (void)bOpenFileDialog; (void)fileTypes; (void)nFileTypeIndex;
    (void)defaultExt; (void)fileName; (void)defaultFilePath;
    return false;
#else
    (void)pWindow; (void)filePath; (void)bOpenFileDialog; (void)fileTypes;
    (void)nFileTypeIndex; (void)defaultExt; (void)fileName; (void)defaultFilePath;
    return false;
#endif
}

bool FileDialog::BrowseForFiles(Window* pWindow,
    std::vector<FilePath>& filePaths,
    const std::vector<FileType>& fileTypes,
    int32_t nFileTypeIndex,
    const DString& defaultExt,
    const FilePath& defaultLocation)
{
#if defined(DUILIB_BUILD_FOR_SDL)
    SDL_Window* sdlWindow = nullptr;
    if (pWindow != nullptr) {
        sdlWindow = (SDL_Window*)pWindow->NativeWnd()->GetWindowHandle();
    }
    (void)fileTypes; (void)nFileTypeIndex; (void)defaultExt; (void)defaultLocation;
    return false;
#else
    (void)pWindow; (void)filePaths; (void)fileTypes;
    (void)nFileTypeIndex; (void)defaultExt; (void)defaultLocation;
    return false;
#endif
}

} // namespace ui

