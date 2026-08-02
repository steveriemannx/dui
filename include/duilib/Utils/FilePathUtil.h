#ifndef UI_UTILS_FILE_PATH_UTIL_H_
#define UI_UTILS_FILE_PATH_UTIL_H_

#include "duilib/Utils/FilePath.h"

namespace ui
{
/** Helper class for path operations
*/
class DUILIB_API FilePathUtil
{
public:
    /** Join two paths to produce a new path, used for path concatenation
    * @param [in] path1 The first path
    * @param [in] path2 The second path
    * @return Returns the joined path
    */
    static FilePath JoinFilePath(const FilePath& path1, const FilePath& path2);

    /** Normalize separator/dot ("/\\.") characters in the path (for files, supports both absolute and relative paths)
    */
    static FilePath NormalizeFilePath(const FilePath& filePath);

    /** Normalize separator/dot ("/\\.") characters in the path (for files, supports both absolute and relative paths)
    */
    static DString NormalizeFilePath(const DString& filePath);

    /** Create a directory (does not include multiple levels of directories)
    * @param [in] filePath The directory to create
    */
    static bool CreateOneDirectory(const DString& filePath);

    /** Create a directory (including multiple levels of subdirectories)
    * @param [in] filePath The directory to create
    */
    static bool CreateDirectories(const DString& filePath);

    /** Get the extension part of the file name in the current path (UTF16/UTF8 encoded)
    * @param [in] filePath The file path
    * @return Returns the extension part of the file, without "."
    */
    static DString GetFileExtension(const DString& filePath);

    /** Get the directory where the program of the current process is located
    * @return Returns the obtained directory
    */
    static FilePath GetCurrentModuleDirectory();

#ifdef DUILIB_BUILD_FOR_MACOS
    /** Get the directory where the app bundle resources of the current process's program are located
    * @return Returns the obtained directory
    */
    static FilePath GetBundleResourcesPath();
#endif
};

} // namespace ui

#endif // UI_UTILS_FILE_PATH_UTIL_H_
