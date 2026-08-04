#ifndef UI_UTILS_FILE_PATH_H_
#define UI_UTILS_FILE_PATH_H_

#include "dui/dui_defs.h"
#include <filesystem>
#include <string>
#include <vector>

namespace ui
{
/** File path (wraps std::filesystem::path; none of the functions throw exceptions)
*/
class DUI_API FilePath
{
public:
    FilePath();
    FilePath(const FilePath&);
    FilePath(FilePath&&);
    ~FilePath();
    FilePath& operator=(const FilePath&);
    FilePath& operator=(FilePath&&) noexcept;

    /** Construct from a string
    * @param [in] filePath Path string, UTF8 encoded
    */
    explicit FilePath(const std::string& filePath);

    /** Construct from a string
    * @param [in] filePath Path string, UTF16 encoded
    */
    explicit FilePath(const std::wstring& filePath);

    /** Construct from a string
    * @param [in] filePath Path string, UTF8 encoded
    * @param [in] bLexicallyNormal Whether the path has already been normalized
    */
    FilePath(const std::string& filePath, bool bLexicallyNormal);

    /** Construct from a string
    * @param [in] filePath Path string, UTF16 encoded
    * @param [in] bLexicallyNormal Whether the path has already been normalized
    */
    FilePath(const std::wstring& filePath, bool bLexicallyNormal);

    /** Swap data with another object
    */
    void Swap(FilePath& r);

public:
    /** Whether it is empty
    */
    bool IsEmpty() const noexcept;

    /** Whether the path is a relative path
    */
    bool IsRelativePath() const noexcept;

    /** Whether the path is an absolute path
    */
    bool IsAbsolutePath() const noexcept;

    /** Whether a file or directory exists at the path in the local file system
    */
    bool IsExistsPath() const noexcept;

    /** Whether a file (not a directory) exists at the path in the local file system
    */
    bool IsExistsFile() const noexcept;

    /** Whether a directory (not a file) exists at the path in the local file system
    */
    bool IsExistsDirectory() const noexcept;

    /** Get the file size
    */
    uint64_t GetFileSize() const noexcept;

    /** Get the path separator (character)
    */
    static DString::value_type GetPathSeparator();

    /** Get the path separator (string)
    */
    static DString GetPathSeparatorStr();

    /** Get the native path as a string
    * @return If DString is the Unicode version, a UTF16 string is returned
    *         If DString is not the Unicode version: on Windows platforms, an Ansi-encoded string (MBCS) is returned; on other platforms, a UTF8-encoded string is returned
    */
#ifdef DUI_UNICODE
    const DString& NativePath() const;
#else
    DString NativePath() const;
#endif

    /** Get the native path as a string
    * @return On Windows platforms, an Ansi-encoded string (MBCS) is returned; on other platforms, a UTF8-encoded string is returned
    */
    DStringA NativePathA() const;

#ifdef DUI_UNICODE
    /** Convert to a string (UTF16 or UTF8 encoded)
    */
    const DString& ToString() const;
#else
#ifdef DUI_BUILD_FOR_WIN
    DString ToString() const;
#else
    const DString& ToString() const;
#endif
#endif

    /** Convert to a string (UTF16 encoded)
    */
    DStringW ToStringW() const;

    /** Convert to a string (UTF8 encoded)
    */
    DStringA ToStringA() const;

    /** Get the file name part of the current path (UTF16/UTF8 encoded)
    */
    DString GetFileName() const;

    /** Get the extension part of the file name in the current path (UTF16/UTF8 encoded)
    */
    DString GetFileExtension() const;

    /** Get the parent path
    */
    FilePath GetParentPath() const;

public:
    /** Ensure the path ends with a separator (on Windows, ends with a backslash "\\"; on other platforms, ends with a forward slash "/")
    *   For example, on Windows: "C:\\abc" becomes "C:\\abc\\"
    */
    void FormatPathAsDirectory();

    /** Remove the trailing path separator
    *   For example, on Windows: "C:\\abc\\" becomes "C:\\abc"
    */
    void TrimRightPathSeparator();

    /** Normalize the path, normalizing separator ("/\\.") characters in the path and ensuring the directory ends with a separator (for directories, absolute paths only)
     *  On Windows, ends with a backslash "\\"; on other platforms, ends with a forward slash "/"
     *  On Windows, forward slashes "/" are replaced with backslashes "\\"; on other platforms, backslashes "\\" are replaced with forward slashes "/"
     *  Also normalizes the relative path components ("." and "..") in the path, removing them to form an absolute path
     */
    void NormalizeDirectoryPath();

    /** Normalize separator ("/\\.") characters in the path (for files and directories, absolute paths only)
     *  On Windows, forward slashes "/" are replaced with backslashes "\\"; on other platforms, backslashes "\\" are replaced with forward slashes "/"
     *  Also normalizes the relative path components ("." and "..") in the path, removing them to form an absolute path
     */
    void NormalizeFilePath();

    /** Remove the file name, keeping only the directory that contains the file
    */
    void RemoveFileName() noexcept;

    /** Whether the current path is a subdirectory of another directory
    * @param [in] parentPath The parent directory
    */
    bool IsSubDirectory(const FilePath& parentPath) const;

    /** Get the list of parent directories
    * @param [in] parentPathList Returns the list of parent directories of this directory (full paths)
    */
    void GetParentPathList(std::vector<FilePath>& parentPathList) const;

    /** Assign a path
    * @param [in] rightPath Follows the DString encoding: the path is UTF8 or UTF16 encoded
    */
    FilePath& operator = (const DString& rightPath);

    /** Join paths: joins the current path with the path on the right to produce a new path
    */
    FilePath& JoinFilePath(const FilePath& rightPath);

    /** Join paths: joins the current path with the path on the right to produce a new path (same as JoinFilePath)
    */
    FilePath& operator /= (const FilePath& rightPath);

    /** Concatenate two paths into one path (concatenated as strings; no path separator is inserted between the two paths)
    * @param [in] rightPath The path on the right
    */
    FilePath& operator += (const FilePath& rightPath);

    /** Concatenate two paths into one path (concatenated as strings; no path separator is inserted between the two paths)
    * @param [in] rightPath Follows the DString encoding: the path is UTF8 or UTF16 encoded
    */
    FilePath& operator += (const DString& rightPath);

    /** Comparison operators
    */
    bool operator != (const FilePath& otherPath) const noexcept;
    bool operator == (const FilePath& otherPath) const noexcept;
    bool operator < (const FilePath& otherPath) const noexcept;

    /** Calculate the hash value of the path
    */
    size_t HashValue() const noexcept;

    /** Clear
    */
    void Clear() noexcept;

private:
    /** File path (UTF16 encoded)
    */
    std::filesystem::path m_filePath;

    /** Whether the file path has been normalized
    */
    bool m_bLexicallyNormal = false;
};

}

//Define a hash algorithm for FilePath so that it can be used as a KEY value for hash-table-based containers
namespace std {
    template <> struct hash<ui::FilePath> {
        size_t operator()(const ui::FilePath& p) const {
            return p.HashValue();
        }
    };
}

#endif // UI_UTILS_FILE_PATH_H_
