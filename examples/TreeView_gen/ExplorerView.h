#ifndef EXAMPLES_EXPLORER_VIEW_H_
#define EXAMPLES_EXPLORER_VIEW_H_

// dui
#include "dui/dui.h"

class MainForm;

/** File information
*/
typedef ui::DirectoryTree::PathInfo PathInfo;

/** File browser view: displays folders and file lists
*/
class ExplorerView : public ui::SupportWeakCallback
{
public:
    /** Constructor
    * @param [in] pMainForm interface of the associated window
    * @param [in] pListCtrl interface of the associated list control
    */
    ExplorerView(MainForm* pMainForm, ui::ListCtrl* pListCtrl);

    /** Destructor
    */
    virtual ~ExplorerView() override;

public:
    /** Replace the original list with a new file list
    * @param [in] currentPath the path of the currently displayed content
    * @param [in] pathList the list of files and folders
    */
    void SetFileList(const ui::FilePath& currentPath, const std::vector<PathInfo>& pathList, const ui::FilePath& selectedPath);

    /** Get the path of the currently displayed content and the selected path
    * @param [out] currentPath the path of the currently displayed content
    * @param [out] selectedPath the path selected in the current view
    * @param [in] selectedPath the path to select
    */
    void GetCurrentPath(ui::FilePath& currentPath, ui::FilePath& selectedPath) const;

    /** Get the list interface of the view
    */
    ui::ListCtrl* GetListCtrl() const;

public:
    /** The header of the view
    */
    enum class ExplorerViewColumn
    {
        kName,           // File name
        kModifyDateTime, // Modified date
        kType,           // File type
        kSize            // File size
    };

    /** Get the sorted column
    * @param [out] viewColumn the currently sorted column
    * @param [out] bSortUp true means ascending, false means descending
    */
    bool GetSortColumnInfo(ExplorerViewColumn& viewColumn, bool& bSortUp) const;

    /** Sort
    */
    void SortByColumn(ExplorerViewColumn viewColumn, bool bSortUp);

private:
    /** Initialize
    */
    void Initialize();

    /** Initialize the header of the "Computer" view
    */
    void InitViewHeader();

    /** Double-click in the "Computer" view
    */
    bool OnExplorerViewDoubleClick(const ui::EventArgs& msg);

    /** When an icon is removed, it must also be removed from the Image List synchronously (otherwise the icon may fail to load)
    * @param [in] nIconId the icon ID (in IconManager)
    */
    void OnRemoveIcon(uint32_t nIconId);

    /** Convert the file size to a display string
    */
    DString FormatFileSize(bool bFolder, uint64_t nFileSize) const;

    /** Convert the file modification time to a display string
    */
    DString FormatFileTime(const ui::FileTime& fileTime) const;

private:
    /** Get the real column index
    */
    size_t GetColumnId(ExplorerViewColumn nOriginIndex) const;

    /** Sort grouping of list items
    */
    enum SortGroup
    {
        kFolder = 0, // Folder
        kFile   = 1, // File
    };

private:
    /** The associated MainForm
    */
    MainForm* m_pMainForm;

    /** The list interface of the view
    */
    ui::ControlPtrT<ui::ListCtrl> m_pListCtrl;

    /** Mapping table of each column's initial index to column ID in the "Computer" view (since reordering columns changes their indexes, data can no longer be added by column index)
    */
    std::map<ExplorerViewColumn, size_t> m_columnIdMap;

    /** Mapping table from IconID to ImageID
    */
    std::map<uint32_t, int32_t> m_iconToImageMap;

    /** The ID of the icon deletion callback function
    */
    uint32_t m_nRemoveIconCallbackId;

    /** The path of the currently displayed content
    */
    ui::FilePath m_currentPath;

    /** Path list (containing folders and files)
    */
    std::vector<PathInfo> m_pathInfoList;
};

#endif // EXAMPLES_EXPLORER_VIEW_H_
