#include "dui/Utils/FilePathUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/StringUtil.h"
#include <filesystem>

#ifdef DUI_BUILD_FOR_LINUX
    #include <unistd.h>
    #include <limits.h>
#endif

#ifdef DUI_BUILD_FOR_FREEBSD
    #include <sys/types.h>
    #include <sys/sysctl.h>
    #include <unistd.h>
#endif

#ifdef DUI_BUILD_FOR_MACOS
    #include <mach-o/dyld.h>
    #include <CoreFoundation/CoreFoundation.h>
#endif

namespace ui
{

FilePath FilePathUtil::JoinFilePath(const FilePath& path1, const FilePath& path2)
{
    FilePath filePath(path1);
    filePath.JoinFilePath(path2);
    return filePath;
}

FilePath FilePathUtil::NormalizeFilePath(const FilePath& filePath)
{
#ifdef DUI_BUILD_FOR_WIN
    std::wstring nativePath;
#else
    std::string nativePath;
#endif
    try {
#ifdef DUI_BUILD_FOR_WIN
        std::filesystem::path file_path(filePath.ToStringW());
#else
        std::filesystem::path file_path(filePath.ToStringA());
#endif
        nativePath = file_path.lexically_normal().native();
    }
    catch (...) {
    }
    return FilePath(nativePath, true);
}

std::string FilePathUtil::NormalizeFilePath(const std::string& filePath)
{
#ifdef DUI_BUILD_FOR_WIN
    //Windows platform
    std::wstring nativePath;
    try {
#ifdef DUI_BUILD_FOR_WIN
        std::filesystem::path file_path(StringConvert::UTF8ToWString(filePath));
#else
        std::filesystem::path file_path(filePath);
#endif
        nativePath = file_path.lexically_normal().native();
    }
    catch (...) {
    }
    return StringConvert::WStringToUTF8(nativePath);

#else
    //Linux platform
    std::string nativePath;
    try {
        std::filesystem::path file_path(filePath);
        nativePath = file_path.lexically_normal().native();
    }
    catch (...) {
    }
    return nativePath;
#endif
}

bool FilePathUtil::CreateOneDirectory(const std::string& filePath)
{
    bool bCreated = false;
    try {
        if (filePath.empty()) {
            return false;
        }
        bCreated = std::filesystem::create_directory(std::filesystem::path(filePath));
    }
    catch (...) {
    }
    return bCreated;
}

bool FilePathUtil::CreateDirectories(const std::string& filePath)
{
    bool bCreated = false;
    try {
        if (filePath.empty()) {
            return false;
        }
        bCreated = std::filesystem::create_directories(std::filesystem::path(filePath));
    }
    catch (...) {
    }
    return bCreated;
}

std::string FilePathUtil::GetFileExtension(const std::string& filePath)
{
    std::string path = filePath;
#ifdef DUI_BUILD_FOR_WIN
    size_t pos = filePath.find_last_of("/\\");
#else
    size_t pos = filePath.find_last_of("/");
#endif
    if ((pos != std::string::npos) && ((pos + 1) < filePath.size())) {
        path = filePath.substr(pos + 1, std::string::npos);
    }

    std::string fileExt;
    pos = path.rfind(".");
    if ((pos != std::string::npos) && ((pos + 1) < path.size())) {
        fileExt = path.substr(pos + 1, std::string::npos);
        fileExt = StringUtil::MakeUpperString(fileExt);
    }
    return fileExt;
}

FilePath FilePathUtil::GetCurrentModuleDirectory()
{
#ifdef DUI_BUILD_FOR_WIN
    std::wstring dirPath;
    dirPath.resize(1024, 0);
    dirPath.resize(::GetModuleFileNameW(nullptr, &dirPath[0], (uint32_t)dirPath.size()));
    FilePath currentDir(dirPath);
    currentDir.RemoveFileName();
    return currentDir;
#elif defined (DUI_BUILD_FOR_LINUX)
    std::error_code ec;
    std::filesystem::path exeFullPath = std::filesystem::canonical("/proc/self/exe", ec);
    std::string dirPath = exeFullPath.parent_path().native();
    if (dirPath.empty()) {
        dirPath = std::filesystem::current_path().native();
    }
    FilePath filePath(dirPath);
    filePath.NormalizeDirectoryPath();
    return filePath;
#elif defined (DUI_BUILD_FOR_FREEBSD)
    int mib[4];
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PATHNAME;
    mib[3] = -1;  // -1 indicates the current process

    // First, get the buffer size required for the path
    size_t len = 0;
    if ((sysctl(mib, 4, nullptr, &len, nullptr, 0) == -1) || (len == 0)) {
        // On error, use the current working directory
        std::string dirPath = std::filesystem::current_path().native();
        FilePath filePath(dirPath);
        filePath.NormalizeDirectoryPath();
        return filePath;
    }
    // Allocate the buffer and get the actual path
    char* path = new char[len];
    if (sysctl(mib, 4, path, &len, nullptr, 0) == -1) {
        delete[] path;
        std::string dirPath = std::filesystem::current_path().native();
        FilePath filePath(dirPath);
        filePath.NormalizeDirectoryPath();
        return filePath;
    }

    // Process the obtained path
    std::filesystem::path exeFullPath(path);
    delete[] path;

    std::string dirPath = exeFullPath.parent_path().native();
    if (dirPath.empty()) {
        dirPath = std::filesystem::current_path().native();
    }

    FilePath filePath(dirPath);
    filePath.NormalizeDirectoryPath();
    return filePath;
#elif defined (DUI_BUILD_FOR_MACOS)
    std::filesystem::path exeFullPath;
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        std::error_code ec;
        exeFullPath = std::filesystem::canonical(path, ec);
    }
    std::string dirPath = exeFullPath.parent_path().native();
    if (dirPath.empty()) {
        dirPath = std::filesystem::current_path().native();
    }
    FilePath filePath(dirPath);
    filePath.NormalizeDirectoryPath();
    return filePath;
#else
    std::string dirPath = std::filesystem::current_path().native(); 
    FilePath filePath(dirPath);
    filePath.NormalizeDirectoryPath();
    return filePath;
#endif
}

#ifdef DUI_BUILD_FOR_MACOS
FilePath FilePathUtil::GetBundleResourcesPath()
{
    std::string dirPath;
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    if (mainBundle) {
        CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
        if (resourcesURL) {
            char path[PATH_MAX];
            if (CFURLGetFileSystemRepresentation(resourcesURL, true, (UInt8*)path, PATH_MAX)) {
                dirPath = std::string(path);
            }
            CFRelease(resourcesURL);
        }
    }
    FilePath filePath(dirPath);
    filePath.NormalizeDirectoryPath();
    return filePath;
}
#endif

} // namespace ui
