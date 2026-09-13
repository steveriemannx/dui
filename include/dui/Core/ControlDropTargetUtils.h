#ifndef UI_CORE_CONTROL_DROP_TARGET_UTILS_H_
#define UI_CORE_CONTROL_DROP_TARGET_UTILS_H_

#include "dui/Core/UiTypes.h"

namespace ui 
{
/** Helper functions for the drag and drop support of controls
*/
class DUI_API ControlDropTargetUtils
{
public:
    /** Determine whether the file extensions meet the configuration requirements
    */
    static bool IsFilteredFileTypes(const std::string& fileTypes, const std::vector<std::string>& fileList);

    /** Filter out unsupported file types
    */
    static void RemoveUnsupportedFiles(std::vector<std::string>& fileList, const std::string& fileTypes);

    /** Determine whether two extensions are the same
    */
    static bool IsSameFileType(const std::string& ext1, const std::string& ext2);
};

} // namespace ui

#endif // UI_CORE_CONTROL_DROP_TARGET_UTILS_H_
