#include "SimpleFileView.h"
#include "MainForm.h"
#include "MainForm.h"

class FileInfoItem : public ui::ListBoxItem
{
public:
    explicit FileInfoItem(ui::Window* pWindow):
        ui::ListBoxItem(pWindow),
        m_nElementIndex(ui::Box::InvalidIndex),
        m_pIconControl(nullptr),
        m_pTextControl(nullptr)
    {
        // Pure code node layout construction (corresponds to the tree_node.xml template, no longer loads the template XML)
        SetClass(_T("listitem"));

        ui::HBox* pRow = new ui::HBox(pWindow);
        pRow->SetAttribute(_T("mouse_enabled"), _T("false"));
        pRow->SetAttribute(_T("padding"), _T("4,4,4,4"));
        AddItem(pRow);

        m_pIconControl = new ui::Control(pWindow);
        m_pIconControl->SetName(_T("control_img"));
        m_pIconControl->SetAttribute(_T("width"), _T("auto"));
        m_pIconControl->SetAttribute(_T("height"), _T("stretch"));
        m_pIconControl->SetAttribute(_T("margin"), _T("4,0,4,0"));
        m_pIconControl->SetAttribute(_T("mouse_enabled"), _T("false"));
        pRow->AddItem(m_pIconControl);

        m_pTextControl = new ui::Label(pWindow);
        m_pTextControl->SetName(_T("control_text"));
        m_pTextControl->SetAttribute(_T("width"), _T("stretch"));
        m_pTextControl->SetAttribute(_T("single_line"), _T("false"));
        m_pTextControl->SetAttribute(_T("height"), _T("stretch"));
        m_pTextControl->SetAttribute(_T("mouse_enabled"), _T("false"));
        m_pTextControl->SetAttribute(_T("text_align"), _T("left,vcenter"));
        pRow->AddItem(m_pTextControl);
    }

    virtual ~FileInfoItem() override
    {
    }

    /** Fill child control information
    * @param [in] errorImagePathSet container that stores error image paths
    * @param [in] fileInfo the path information to display
    * @param [in] nElementIndex the data element index
    */
    void FillSubControls(std::unordered_set<DString>& errorImagePathSet,const ui::DirectoryTree::PathInfo& fileInfo, size_t nElementIndex)
    {
        m_nElementIndex = nElementIndex;
        if (m_pIconControl == nullptr) {
            m_pIconControl = FindSubControl(_T("control_img"));
        }
        if (m_pTextControl == nullptr) {
            m_pTextControl = dynamic_cast<ui::Label*>(FindSubControl(_T("control_text")));
        }
        if (m_pTextControl != nullptr) {
            m_pTextControl->SetAutoToolTip(true);
            m_pTextControl->SetText(fileInfo.m_displayName);
        }
        if (m_pIconControl != nullptr) {
            // Remove the response functions of the image loading and decoding events
            m_pIconControl->DetachEvent(ui::kEventImageLoad);
            m_pIconControl->DetachEvent(ui::kEventImageDecode);

            if (!fileInfo.m_bFolder && IsImageFile(fileInfo.m_displayName)) {
                // Image files: show the image directly (disabled, too slow to run, to be optimized)
                int32_t itemWidth = this->GetWidth() / 2;
                // Restore the width to its original value without DPI scaling (because image loading applies DPI scaling to the width attribute)
                Dpi().UnscaleInt(itemWidth);

                // When an error occurs (image load failure or image decode failure), show a default image
                const DString defaultImage = ui::StringUtil::Printf(_T("file='image-photo.svg' halign='center' valign='center' width='%d'"), itemWidth);

                if (errorImagePathSet.find(fileInfo.m_filePath.ToString()) == errorImagePathSet.end()) {
                    DString imageString = fileInfo.m_filePath.ToString();
                    if (itemWidth > 0) {
                        imageString = ui::StringUtil::Printf(_T("file='%s' halign='center' valign='center' width='%d' assert='false'"), imageString.c_str(), itemWidth);
                    }
                    else {
                        imageString = ui::StringUtil::Printf(_T("file='%s' halign='center' valign='center'"), imageString.c_str());
                    }
                    m_pIconControl->SetBkImage(imageString);
                }
                else {
                    // This is a faulty image; show the error image directly (to avoid flickering of the error image when displayed)
                    m_pIconControl->SetBkImage(defaultImage);
                }

                // When image loading fails, show a default image
                m_pIconControl->AttachImageLoad([this, defaultImage, &errorImagePathSet](const ui::EventArgs& args) {
                    ui::ImageDecodeResult* pImageDecodeResult = (ui::ImageDecodeResult*)args.wParam;
                    if ((pImageDecodeResult != nullptr) && pImageDecodeResult->m_bLoadError) {
                        errorImagePathSet.insert(pImageDecodeResult->m_imageFilePath);
                        ui::Control* pIconControl = FindSubControl(_T("control_img"));
                        if (pIconControl != nullptr) {
                            pIconControl->SetBkImage(defaultImage);
                        }
                    }
                    return true;
                    });

                m_pIconControl->AttachImageDecode([this, defaultImage, &errorImagePathSet](const ui::EventArgs& args) {
                    ui::ImageDecodeResult* pImageDecodeResult = (ui::ImageDecodeResult*)args.wParam;
                    if ((pImageDecodeResult != nullptr) && pImageDecodeResult->m_bDecodeError) {
                        errorImagePathSet.insert(pImageDecodeResult->m_imageFilePath);
                        ui::Control* pIconControl = FindSubControl(_T("control_img"));
                        if (pIconControl != nullptr) {
                            pIconControl->SetBkImage(defaultImage);
                        }
                    }
                    return true;
                    });
            }
            else {
                // For non-image files or folders, show an icon
                DString iconString = ui::GlobalManager::Instance().Icon().GetIconString(fileInfo.m_nIconID);
                if (!iconString.empty()) {
                    iconString = ui::StringUtil::Printf(_T("file='%s' width='64' height='64' halign='center' valign='center'"), iconString.c_str());
                    m_pIconControl->SetBkImage(iconString);
                }
                else {
                    m_pIconControl->SetBkImage(_T(""));
                }
            }
        }
    }

private:
    /** Whether it is an image file
    */
    bool IsImageFile(const DString& filePath) const
    {
        DString fileExt;
        size_t pos = filePath.rfind(_T('.'));
        if (pos != DString::npos) {
            fileExt = filePath.substr(pos);
        }
        fileExt = ui::StringUtil::MakeLowerString(fileExt);
        if (fileExt == _T(".svg")) {
            return true;
        }
        if ((fileExt == _T(".svg")) ||
            (fileExt == _T(".jpg")) ||
            (fileExt == _T(".jpeg")) ||
            (fileExt == _T(".jpe")) ||
            (fileExt == _T(".jif")) ||
            (fileExt == _T(".jfif")) ||
            (fileExt == _T(".jfi")) ||
            (fileExt == _T(".gif")) ||
            (fileExt == _T(".png")) ||
            (fileExt == _T(".bmp")) ||
            (fileExt == _T(".dib")) ||
            (fileExt == _T(".webp")) ||
            (fileExt == _T(".json")) ||
            (fileExt == _T(".pag")) ||
            (fileExt == _T(".ico")) ||
            (fileExt == _T(".cur")) ) {
            return true;
        }
        return false;
    }

private:
    // The element index
    size_t m_nElementIndex;

    // Icon control
    ui::Control* m_pIconControl;

    // Text control
    ui::Label* m_pTextControl;
};

SimpleFileView::SimpleFileView(MainForm* pMainForm, ui::VirtualListBox* pListBox):
    m_pMainForm(pMainForm),
    m_pListBox(pListBox)
{
    if (m_pListBox != nullptr) {
        m_pListBox->SetDataProvider(this);
    }
}

SimpleFileView::~SimpleFileView()
{
    ui::DirectoryTree::ClearPathInfoList(m_pathList);
    if (m_pListBox != nullptr) {
        m_pListBox->SetDataProvider(nullptr);
    }
}

ui::Control* SimpleFileView::CreateElement(ui::VirtualListBox* pVirtualListBox)
{
    ASSERT(pVirtualListBox != nullptr);
    if (pVirtualListBox == nullptr) {
        return nullptr;
    }
    ASSERT(pVirtualListBox->GetWindow() != nullptr);
    // The node template is built by code (no longer loads tree_node.xml)
    FileInfoItem* item = new FileInfoItem(pVirtualListBox->GetWindow());
    item->AttachDoubleClick(UiBind(&SimpleFileView::OnDoubleClickItem, this, std::placeholders::_1));
    return item;
}

bool SimpleFileView::FillElement(ui::Control* pControl, size_t nElementIndex)
{
    FileInfoItem* pItem = dynamic_cast<FileInfoItem*>(pControl);
    ASSERT(pItem != nullptr);
    ASSERT(nElementIndex < m_pathList.size());
    if ((pItem == nullptr) || (nElementIndex >= m_pathList.size())) {
        return false;
    }
    const PathInfo& fileInfo = m_pathList[nElementIndex];
    pItem->FillSubControls(m_errorImagePathSet, fileInfo, nElementIndex);
    pItem->SetUserDataID(nElementIndex);
    return true;
}

size_t SimpleFileView::GetElementCount() const
{
    return m_pathList.size();
}

void SimpleFileView::SetElementSelected(size_t nElementIndex, bool bSelected)
{
    if (nElementIndex < m_pathSelectList.size()) {
        m_pathSelectList[nElementIndex] = bSelected;
    }
}

bool SimpleFileView::IsElementSelected(size_t nElementIndex) const
{
    if (nElementIndex < m_pathSelectList.size()) {
        return m_pathSelectList[nElementIndex];
    }
    return false;
}

void SimpleFileView::GetSelectedElements(std::vector<size_t>& selectedIndexs) const
{
    selectedIndexs.clear();
    for (size_t nElementIndex = 0; nElementIndex < m_pathSelectList.size(); ++nElementIndex) {
        if (m_pathSelectList[nElementIndex]) {
            selectedIndexs.push_back(nElementIndex);
        }
    }
}

bool SimpleFileView::IsMultiSelect() const
{
    // Multi-selection is not supported
    return false;
}

void SimpleFileView::SetMultiSelect(bool /*bMultiSelect*/)
{
    // Disable switching between single and multi selection; fixed to single selection mode
}

void SimpleFileView::SetFileList(const ui::FilePath& currentPath, const std::vector<PathInfo>& pathList, const ui::FilePath& selectedPath)
{
    m_errorImagePathSet.clear(); // Clear the error image list
    m_currentPath = currentPath;
    std::vector<PathInfo> oldPathList;
    oldPathList.swap(m_pathList);

    m_pathList = pathList;
    m_pathSelectList.clear();
    m_pathSelectList.resize(m_pathList.size(), false); // Initially all are unselected

    // Notify ListBox of the change in the total data count
    EmitCountChanged();

    // Clean up the original icon resources
    ui::DirectoryTree::ClearPathInfoList(oldPathList);

    // Select a child item
    size_t nSelectedItemIndex = ui::Box::InvalidIndex;
    if (!selectedPath.IsEmpty()) {
        for (size_t nIndex = 0; nIndex < pathList.size(); ++nIndex) {
            const ui::DirectoryTree::PathInfo& pathInfo = pathList[nIndex];
            if (selectedPath == pathInfo.m_filePath) {
                // Save the selection state
                m_pathSelectList[nIndex] = true;
                nSelectedItemIndex = nIndex;
                break;
            }
        }
    }

    if ((nSelectedItemIndex != ui::Box::InvalidIndex) && (m_pListBox != nullptr)) {
        m_pListBox->Refresh(true);
        m_pListBox->EnsureVisible(nSelectedItemIndex, false);
    }
}

void SimpleFileView::GetCurrentPath(ui::FilePath& currentPath, ui::FilePath& selectedPath) const
{
    currentPath = m_currentPath;
    std::vector<size_t> selectedIndexs;
    GetSelectedElements(selectedIndexs);
    for (size_t nIndex : selectedIndexs) {
        if (nIndex < m_pathList.size()) {
            const ui::DirectoryTree::PathInfo& pathInfo = m_pathList[nIndex];
            if (!pathInfo.m_filePath.IsEmpty()) {
                // Record the currently selected directory
                selectedPath = pathInfo.m_filePath;
                break;
            }
        }
    }
}

bool SimpleFileView::OnDoubleClickItem(const ui::EventArgs& args)
{
    if (m_pMainForm == nullptr) {
        return true;
    }

    FileInfoItem* pItem = dynamic_cast<FileInfoItem*>(args.GetSender());
    if (pItem != nullptr) {
        size_t nElementIndex = pItem->GetUserDataID();
        if (nElementIndex < m_pathList.size()) {
            const PathInfo& fileInfo = m_pathList[nElementIndex];
            if (fileInfo.m_bFolder) {
                // Double-click on a directory
                m_pMainForm->SelectSubPath(fileInfo.m_filePath);
            }
        }
    }
    return true;
}
