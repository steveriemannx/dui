#ifndef UI_UTILS_FILE_DIALOG_H_
#define UI_UTILS_FILE_DIALOG_H_

#include "duilib/Utils/FilePath.h"
#include <string>
#include <vector>

namespace ui
{

/** File or directory selection dialog, supporting multiple selection
*/
class Window;
class DUILIB_API FileDialog
{
public:
    /** Select a folder (single selection)
    * @param [in] pWindow The parent window
    * @param [out] folderPath Returns the path of the selected folder
    * @param [in] defaultLocation The initially selected folder path (optional parameter)
    */
    bool BrowseForFolder(Window* pWindow, FilePath& folderPath,
                         const FilePath& defaultLocation = FilePath());

    /** Select folders (multiple selection)
    * @param [in] pWindow The parent window
    * @param [out] folderPaths Returns the paths of the selected folders
    * @param [in] defaultLocation The initially selected folder path (optional parameter)
    */
    bool BrowseForFolders(Window* pWindow, std::vector<FilePath>& folderPaths,
                          const FilePath& defaultLocation = FilePath());

public:
    /** File type filter
    */
    struct FileType
    {
        /** The display name of the file type filter, e.g.: "Text files"
        *   Encoding rule: the Unicode version uses UTF16 encoding, the non-Unicode version uses UTF8 encoding
        */
        DString szName;

        /** The file type filter, e.g.: "*.txt"
        *   Encoding rule: the Unicode version uses UTF16 encoding, the non-Unicode version uses UTF8 encoding
        */
        DString szExt;
    };

    /** Select a file (single selection)
    * @param [in] pWindow The parent window
    * @param [in] bOpenFileDialog true means open a file, false means save a file
    * @param [in] fileTypes The file types that the dialog can open or save
    * @param [in] nFileTypeIndex The selected file type; valid range: [0, fileTypes.size())
    * @param [in] defaultExt The default file type, e.g.: "doc;docx"; encoding rule: the Unicode version uses UTF16 encoding, the non-Unicode version uses UTF8 encoding
    * @param [in] fileName The current file name
    * @param [in] defaultLocation The initial folder path (optional parameter)
    * @param [out] filePath Returns the path of the selected file
    */
    bool BrowseForFile(Window* pWindow, 
                       FilePath& filePath,
                       bool bOpenFileDialog, 
                       const std::vector<FileType>& fileTypes = std::vector<FileType>(),
                       int32_t nFileTypeIndex = -1,
                       const DString& defaultExt = _T(""),
                       const DString& fileName = _T(""),
                       const FilePath& defaultFilePath = FilePath());

    /** Select files (multiple selection)
    * @param [in] pWindow The parent window
    * @param [in] fileTypes The file types that the dialog can open
    * @param [in] nFileTypeIndex The selected file type; valid range: [0, fileTypes.size())
    * @param [in] defaultExt The default file type, e.g.: "doc;docx"; encoding rule: the Unicode version uses UTF16 encoding, the non-Unicode version uses UTF8 encoding
    * @param [in] defaultLocation The initial folder path (optional parameter)
    * @param [out] filePaths Returns the paths of the selected files
    */
    bool BrowseForFiles(Window* pWindow, 
                        std::vector<FilePath>& filePaths,
                        const std::vector<FileType>& fileTypes = std::vector<FileType>(),
                        int32_t nFileTypeIndex = -1,
                        const DString& defaultExt = _T(""),
                        const FilePath& defaultLocation = FilePath());

};

}

#endif // UI_UTILS_FILE_DIALOG_H_
