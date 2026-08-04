#ifndef EXAMPLES_SIMPLE_FILE_VIEW_H_
#define EXAMPLES_SIMPLE_FILE_VIEW_H_

// dui
#include "dui/dui.h"
#include <unordered_set>

/** File information
*/
typedef ui::DirectoryTree::PathInfo PathInfo;

class MainForm;

/** Simple file view: displays folders and file lists
*/
class SimpleFileView : public ui::VirtualListBoxElement
{
public:
    SimpleFileView(MainForm* pMainForm, ui::VirtualListBox* pListBox);
    virtual ~SimpleFileView() override;

    /** Create a data item
    * @param [in] pVirtualListBox interface of the associated virtual list box
    * @return the pointer to the created data item
    */
    virtual ui::Control* CreateElement(ui::VirtualListBox* pVirtualListBox) override;

    /** Fill the specified data item
    * @param [in] pControl pointer to the data item control
    * @param [in] nElementIndex the index ID of the data element, range: [0, GetElementCount())
    */
    virtual bool FillElement(ui::Control* pControl, size_t nElementIndex) override;

    /** Get the total number of data items
    * @return the total number of data items
    */
    virtual size_t GetElementCount() const override;

    /** Set the selection state
    * @param [in] nElementIndex the index ID of the data element, range: [0, GetElementCount())
    * @param [in] bSelected true means selected, false means not selected
    */
    virtual void SetElementSelected(size_t nElementIndex, bool bSelected) override;

    /** Get the selection state
    * @param [in] nElementIndex the index ID of the data element, range: [0, GetElementCount())
    * @return true means selected, false means not selected
    */
    virtual bool IsElementSelected(size_t nElementIndex) const override;

    /** Get the list of selected elements
    * @param [in] selectedIndexs returns the list of currently selected elements, valid range: [0, GetElementCount())
    */
    virtual void GetSelectedElements(std::vector<size_t>& selectedIndexs) const override;

    /** Whether multi-selection is supported
    */
    virtual bool IsMultiSelect() const override;

    /** Set whether multi-selection is supported, called by the UI layer, keeping it consistent with the UI control
    * @return bMultiSelect true means multi-selection is supported, false means it is not
    */
    virtual void SetMultiSelect(bool bMultiSelect) override;

public:
    /** Replace the original list with a new file list
    * @param [in] currentPath the path of the currently displayed content
    * @param [in] pathList the file list
    * @param [in] selectedPath the path to select
    */
    void SetFileList(const ui::FilePath& currentPath, const std::vector<PathInfo>& pathList, const ui::FilePath& selectedPath);

    /** Get the path of the currently displayed content and the selected path
    * @param [out] currentPath the path of the currently displayed content
    * @param [out] selectedPath the path selected in the current view
    */
    void GetCurrentPath(ui::FilePath& currentPath, ui::FilePath& selectedPath) const;

private:
    /** Triggered when a child item is double-clicked
     * @param[in] args the message body
     * @return always returns true
     */
    bool OnDoubleClickItem(const ui::EventArgs& args);

private:
    /** The associated MainForm
    */
    MainForm* m_pMainForm;

    /** Interface of the file list (the UI shown by the virtual list on the right)
    */
    ui::ControlPtrT<ui::VirtualListBox> m_pListBox;

    /** The path of the currently displayed content
    */
    ui::FilePath m_currentPath;

    /** File list
    */
    std::vector<PathInfo> m_pathList;

    /** Selection state of elements in the file list
    */
    std::vector<bool> m_pathSelectList;

    /** Error image paths (including image load failures or image data decode failures)
    */
    std::unordered_set<DString> m_errorImagePathSet;
};

#endif //EXAMPLES_SIMPLE_FILE_VIEW_H_
