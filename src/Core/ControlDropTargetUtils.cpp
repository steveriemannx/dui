#include "dui/Core/ControlDropTargetUtils.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/StringUtil.h"
#include "dui/Utils/FilePath.h"

namespace ui 
{

bool ControlDropTargetUtils::IsFilteredFileTypes(const DString& fileTypes, const std::vector<DString>& fileList)
{
    if (fileTypes.empty() || fileList.empty()) {
        return true;
    }

    std::list<DString> fileTypeList = StringUtil::Split(fileTypes, _T(";"));
    for (DString& fileExt : fileTypeList) {
        StringUtil::Trim(fileExt);
    }
    for (const DString& fileName : fileList) {
        FilePath filePath(fileName);
        DString dropFileExt = filePath.GetFileExtension();
        for (const DString& fileExt : fileTypeList) {
            if (IsSameFileType(fileExt, dropFileExt)) {
                return true;
            }
        }
    }
    return false;
}

void ControlDropTargetUtils::RemoveUnsupportedFiles(std::vector<DString>& fileList, const DString& fileTypes)
{
    if (fileTypes.empty() || fileList.empty()) {
        return;
    }

    std::list<DString> fileTypeList = StringUtil::Split(fileTypes, _T(";"));
    for (DString& fileExt : fileTypeList) {
        StringUtil::Trim(fileExt);
    }
    auto iter = fileList.begin();
    while (iter != fileList.end()) {
        const DString& fileName = *iter;
        bool bMatch = false;
        FilePath filePath(fileName);
        DString dropFileExt = filePath.GetFileExtension();
        for (const DString& fileExt : fileTypeList) {
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

bool ControlDropTargetUtils::IsSameFileType(const DString& ext1, const DString& ext2)
{
#if !defined (DUI_BUILD_FOR_LINUX) && !defined (DUI_BUILD_FOR_FREEBSD)
    // Windows/MacOS file names are case-insensitive; Linux/FreeBSD are case-sensitive
    return StringUtil::IsEqualNoCase(ext1.c_str(), ext2.c_str());
#else
    return ext1 == ext2;
#endif
}

} // namespace ui
