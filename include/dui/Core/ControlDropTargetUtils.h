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
    static bool IsFilteredFileTypes(const DString& fileTypes, const std::vector<DString>& fileList);

    /** Filter out unsupported file types
    */
    static void RemoveUnsupportedFiles(std::vector<DString>& fileList, const DString& fileTypes);

    /** Determine whether two extensions are the same
    */
    static bool IsSameFileType(const DString& ext1, const DString& ext2);
};

} // namespace ui

#endif // UI_CORE_CONTROL_DROP_TARGET_UTILS_H_
