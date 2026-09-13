#include "dui/Core/ControlDropTargetUtils.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/FilePath.h"

namespace ui 
{

bool ControlDropTargetUtils::IsFilteredFileTypes(const std::string& fileTypes, const std::vector<std::string>& fileList)
{
    if (fileTypes.empty() || fileList.empty()) {
        return true;
    }

    std::list<std::string> fileTypeList = StringUtil::Split(fileTypes, ";");
    for (std::string& fileExt : fileTypeList) {
        StringUtil::Trim(fileExt);
    }
    for (const std::string& fileName : fileList) {
        FilePath filePath(fileName);
        std::string dropFileExt = filePath.GetFileExtension();
        for (const std::string& fileExt : fileTypeList) {
            if (IsSameFileType(fileExt, dropFileExt)) {
                return true;
            }
        }
    }
    return false;
}

void ControlDropTargetUtils::RemoveUnsupportedFiles(std::vector<std::string>& fileList, const std::string& fileTypes)
{
    if (fileTypes.empty() || fileList.empty()) {
        return;
    }

    std::list<std::string> fileTypeList = StringUtil::Split(fileTypes, ";");
    for (std::string& fileExt : fileTypeList) {
        StringUtil::Trim(fileExt);
    }
    auto iter = fileList.begin();
    while (iter != fileList.end()) {
        const std::string& fileName = *iter;
        bool bMatch = false;
        FilePath filePath(fileName);
        std::string dropFileExt = filePath.GetFileExtension();
        for (const std::string& fileExt : fileTypeList) {
            if (IsSameFileType(fileExt, dropFileExt)) {
                bMatch = true;
                break;
            }
        }
        if (bMatch) {
            // Keep the ones that matched successfully
            ++iter;
        }
        else {
            iter = fileList.erase(iter);
        }
    }
}

bool ControlDropTargetUtils::IsSameFileType(const std::string& ext1, const std::string& ext2)
{
#if !defined (DUI_BUILD_FOR_LINUX) && !defined (DUI_BUILD_FOR_FREEBSD)
    // Windows/MacOS file names are case-insensitive; Linux/FreeBSD are case-sensitive
    return StringUtil::IsEqualNoCase(ext1.c_str(), ext2.c_str());
#else
    return ext1 == ext2;
#endif
}

} // namespace ui
