#include "ComputerView.h"
#include "MainForm.h"

ComputerView::ComputerView(MainForm* pMainForm, ui::ListCtrl* pListCtrl):
    m_pMainForm(pMainForm),
    m_pComputerListCtrl(pListCtrl),
    m_nRemoveIconCallbackId(0)
{
    Initialize();
}

ComputerView::~ComputerView()
{
    ui::GlobalManager::Instance().Icon().DetachRemoveIconEvent(m_nRemoveIconCallbackId);
    ui::DirectoryTree::ClearDiskInfoList(m_diskInfoList);
    m_diskInfoList.clear();
}

void ComputerView::Initialize()
{
    ASSERT(m_pMainForm != nullptr);
    ASSERT(m_pComputerListCtrl != nullptr);
    if ((m_pComputerListCtrl == nullptr) || (m_pMainForm == nullptr)) {
        return;
    }

    // Bind the icon deletion event
    m_nRemoveIconCallbackId = ui::GlobalManager::Instance().Icon().AttachRemoveIconEvent(ui::UiBind(&ComputerView::OnRemoveIcon, this, std::placeholders::_1));

    // Initialize the header
    InitComputerViewHeader();

    if (m_pComputerListCtrl != nullptr) {
        // Associate the image list
        ui::ImageListPtr pImageList = std::make_shared<ui::ImageList>();
        pImageList->SetImageSize(ui::UiSize(20, 20), m_pMainForm->Dpi(), true);
        m_pComputerListCtrl->SetImageList(ui::ListCtrlType::Report, pImageList);// The resource lifecycle of this pointer is managed internally by ListCtrl

        // Bind the list item mouse double-click event
        m_pComputerListCtrl->AttachDoubleClick(UiBind(&ComputerView::OnComuterViewDoubleClick, this, std::placeholders::_1));
    }
}

bool ComputerView::OnComuterViewDoubleClick(const ui::EventArgs& msg)
{
    if ((m_pComputerListCtrl != nullptr) && (msg.wParam != 0) && (m_pMainForm != nullptr)) {
        size_t nItemIndex = msg.lParam;
        size_t nIndex = m_pComputerListCtrl->GetDataItemUserData(nItemIndex);
        if (nIndex < m_diskInfoList.size()) {
            const ui::DirectoryTree::DiskInfo& diskInfo = m_diskInfoList[nIndex];
            if (!diskInfo.m_filePath.IsEmpty() && diskInfo.m_filePath.IsExistsDirectory()) {
                // Enter the selected directory
                m_pMainForm->SelectSubPath(diskInfo.m_filePath);
            }
        }
    }
    return true;
}

void ComputerView::OnRemoveIcon(uint32_t nIconId)
{
    if (!ui::GlobalManager::Instance().IsInUIThread()) {
        // If not running on the main thread, switch to the main thread to execute
        ui::GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, this->ToWeakCallback([this, nIconId]() {
                OnRemoveIcon(nIconId);
            }));
        return;
    }

    // Execute on the main thread
    ui::GlobalManager::Instance().AssertUIThread();
    auto iter = m_iconToImageMap.find(nIconId);
    if (iter != m_iconToImageMap.end()) {
        int32_t nImageId = iter->second;
        m_iconToImageMap.erase(iter);

        ui::ImageListPtr pImageList;
        if (m_pComputerListCtrl != nullptr) {
            pImageList = m_pComputerListCtrl->GetImageList(ui::ListCtrlType::Report);
        }
        if (pImageList != nullptr) {
            pImageList->RemoveImageString(nImageId);
        }
    }
}

size_t ComputerView::GetColumnId(ComputerViewColumn nOriginIndex) const
{
    size_t nColumnIndex = ui::Box::InvalidIndex;
    auto iter = m_columnIdMap.find(nOriginIndex);
    if (iter != m_columnIdMap.end()) {
        return iter->second;
    }
    return nColumnIndex;
}

DString ComputerView::FormatDiskSpace(uint64_t nSpace) const
{
    DString value;
    if (nSpace > 1 * 1024 * 1024 * 1024) {
        //GB
        double total_gb = static_cast<double>(nSpace) / (1024 * 1024 * 1024);
        value = ui::StringUtil::Printf(_T("%.01lf GB"), total_gb);
    }
    else if (nSpace > 1 * 1024 * 1024) {
        //MB
        double total_mb = static_cast<double>(nSpace) / (1024 * 1024);
        value = ui::StringUtil::Printf(_T("%.01lf MB"), total_mb);
    }
    else if (nSpace > 1 * 1024) {
        //KB
        double total_kb = static_cast<double>(nSpace) / (1024);
        value = ui::StringUtil::Printf(_T("%.01lf KB"), total_kb);
    }
    else if (nSpace == 0) {
        value = _T("0");
    }
    else {
        //B
        value = ui::StringUtil::Printf(_T("%d B"), (int32_t)nSpace);
    }
    return value;
}

DString ComputerView::FormatUsedPercent(uint64_t nTotalSpace, uint64_t nFreeSpace) const
{
    DString value;
    if ((nFreeSpace <= nTotalSpace) && (nTotalSpace != 0)) {
        double fPercent = static_cast<double>(nTotalSpace - nFreeSpace) / nTotalSpace;
        value = ui::StringUtil::Printf(_T("%.01lf%%"), fPercent * 100);
    }
    return value;
}

void ComputerView::InitComputerViewHeader()
{
#if defined (DUILIB_BUILD_FOR_WIN)
    InitComputerViewHeader_Win();
#elif defined (DUILIB_BUILD_FOR_LINUX)
    InitComputerViewHeader_Linux();
#endif
}

void ComputerView::ShowMyComputerContents(const std::vector<ui::DirectoryTree::DiskInfo>& diskInfoList)
{
    ASSERT(m_pMainForm != nullptr);
    if (m_pMainForm == nullptr) {
        return;
    }
    ui::GlobalManager::Instance().AssertUIThread();
    ui::DirectoryTree::ClearDiskInfoList(m_diskInfoList);
    m_diskInfoList = diskInfoList;
    if ((m_pComputerListCtrl == nullptr)) {
        return;
    }
    m_pComputerListCtrl->DeleteAllDataItems();
    ui::ImageListPtr pImageList;
    if (m_pComputerListCtrl != nullptr) {
        pImageList = m_pComputerListCtrl->GetImageList(ui::ListCtrlType::Report);
    }
    if (pImageList != nullptr) {
        pImageList->Clear();
    }
#if defined (DUILIB_BUILD_FOR_WIN)
    ShowMyComputerContents_Win(pImageList, diskInfoList);
#elif defined (DUILIB_BUILD_FOR_LINUX)
    ShowMyComputerContents_Linux(pImageList, diskInfoList);
#endif
}

#if defined (DUILIB_BUILD_FOR_WIN)
void ComputerView::InitComputerViewHeader_Win()
{
    if (m_pComputerListCtrl == nullptr) {
        return;
    }
    ui::ListCtrlHeaderItem* pHeaderItem = nullptr;
    ui::ListCtrlColumn columnInfo;
    columnInfo.text = _T("Name");
    columnInfo.nColumnWidth = 200;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kName] = pHeaderItem->GetColumnId();

    columnInfo.text = _T("Disk Type");
    columnInfo.nColumnWidth = 120;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kType] = pHeaderItem->GetColumnId();

    columnInfo.text = _T("Partition Type");
    columnInfo.nColumnWidth = 120;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kPartitionType] = pHeaderItem->GetColumnId();

    columnInfo.text = _T("Total Size");
    columnInfo.nColumnWidth = 120;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kTotalSpace] = pHeaderItem->GetColumnId();

    columnInfo.text = _T("Free Space");
    columnInfo.nColumnWidth = 120;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kFreeSpace] = pHeaderItem->GetColumnId();

    columnInfo.text = _T("Used");
    columnInfo.nColumnWidth = 80;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kUsedPercent] = pHeaderItem->GetColumnId();
}

void ComputerView::ShowMyComputerContents_Win(ui::ImageListPtr pImageList, const std::vector<ui::DirectoryTree::DiskInfo>& diskInfoList)
{
    ui::ListCtrlSubItemData itemData;
    size_t nItemIndex = 0;
    size_t nColumnId = 0;
    for (size_t nIndex = 0; nIndex < diskInfoList.size(); ++nIndex) {
        const ui::DirectoryTree::DiskInfo& diskInfo = diskInfoList[nIndex];
        nItemIndex = m_pComputerListCtrl->AddDataItem(itemData);
        if (!ui::Box::IsValidItemIndex(nItemIndex)) {
            continue;
        }
        // Record the association
        m_pComputerListCtrl->SetDataItemUserData(nItemIndex, nIndex);
        // Set the icon
        if (pImageList != nullptr) {
            DString iconString = ui::GlobalManager::Instance().Icon().GetIconString(diskInfo.m_nIconID);
            if (!iconString.empty()) {
                int32_t nImageId = pImageList->AddImageString(iconString, m_pMainForm->Dpi());
                m_iconToImageMap[diskInfo.m_nIconID] = nImageId;
                m_pComputerListCtrl->SetDataItemImageId(nItemIndex, nImageId);
            }
        }

        ui::ListCtrlSubItemData subItemData;
        subItemData.nTextFormat = ui::DrawStringFormat::TEXT_LEFT | ui::DrawStringFormat::TEXT_VCENTER;

        subItemData.text = diskInfo.m_displayName;
        nColumnId = GetColumnId(ComputerViewColumn::kName);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Name
        // Set the sort order of the "Name" column
        m_pComputerListCtrl->SetSubItemUserDataNById(nItemIndex, nColumnId, nIndex);
        m_pComputerListCtrl->SetSubItemUserDataSById(nItemIndex, nColumnId, diskInfo.m_filePath.ToString());
        m_pComputerListCtrl->SetColumnSortFlagById(nColumnId, ui::ListCtrlSubItemSortFlag::kSortByUserDataN);

        subItemData.nTextFormat = ui::DrawStringFormat::TEXT_HCENTER | ui::DrawStringFormat::TEXT_VCENTER;

        subItemData.text = diskInfo.m_volumeType;
        nColumnId = GetColumnId(ComputerViewColumn::kType);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Disk type

        subItemData.text = diskInfo.m_fileSystem;
        nColumnId = GetColumnId(ComputerViewColumn::kPartitionType);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Partition type

        subItemData.text = FormatDiskSpace(diskInfo.m_totalBytes);
        nColumnId = GetColumnId(ComputerViewColumn::kTotalSpace);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Total size

        // Set the sort order of the "Total Size" column
        m_pComputerListCtrl->SetSubItemUserDataNById(nItemIndex, nColumnId, diskInfo.m_totalBytes);
        m_pComputerListCtrl->SetColumnSortFlagById(nColumnId, ui::ListCtrlSubItemSortFlag::kSortByUserDataN);

        subItemData.text = FormatDiskSpace(diskInfo.m_freeBytes);
        nColumnId = GetColumnId(ComputerViewColumn::kFreeSpace);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Free space

        // Set the sort order of the "Free Space" column
        m_pComputerListCtrl->SetSubItemUserDataNById(nItemIndex, nColumnId, diskInfo.m_freeBytes);
        m_pComputerListCtrl->SetColumnSortFlagById(nColumnId, ui::ListCtrlSubItemSortFlag::kSortByUserDataN);

        subItemData.text = FormatUsedPercent(diskInfo.m_totalBytes, diskInfo.m_freeBytes);
        nColumnId = GetColumnId(ComputerViewColumn::kUsedPercent);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Used percentage

        // Set the sort order of the "Used Percentage" column
        uint64_t nUsedPercent = 100 * 1000;
        if (diskInfo.m_totalBytes != 0) {
            nUsedPercent = (diskInfo.m_totalBytes - diskInfo.m_freeBytes) * 1000 / diskInfo.m_totalBytes;
        }
        m_pComputerListCtrl->SetSubItemUserDataNById(nItemIndex, nColumnId, (size_t)nUsedPercent);
        m_pComputerListCtrl->SetColumnSortFlagById(nColumnId, ui::ListCtrlSubItemSortFlag::kSortByUserDataN);
    }
}

#endif //DUILIB_BUILD_FOR_WIN

#if defined (DUILIB_BUILD_FOR_LINUX)
void ComputerView::InitComputerViewHeader_Linux()
{
    if (m_pComputerListCtrl == nullptr) {
        return;
    }
    ui::ListCtrlHeaderItem* pHeaderItem = nullptr;
    ui::ListCtrlColumn columnInfo;
    columnInfo.text = _T("File System");
    columnInfo.nColumnWidth = 200;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kName] = pHeaderItem->GetColumnId();

    columnInfo.text = _T("Device Type");
    columnInfo.nColumnWidth = 120;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kType] = pHeaderItem->GetColumnId();

    columnInfo.text = _T("Partition Type");
    columnInfo.nColumnWidth = 120;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kPartitionType] = pHeaderItem->GetColumnId();

    columnInfo.text = _T("Total Size");
    columnInfo.nColumnWidth = 120;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kTotalSpace] = pHeaderItem->GetColumnId();

    columnInfo.text = _T("Free Space");
    columnInfo.nColumnWidth = 120;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kFreeSpace] = pHeaderItem->GetColumnId();

    columnInfo.text = _T("Used");
    columnInfo.nColumnWidth = 80;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kUsedPercent] = pHeaderItem->GetColumnId();

    columnInfo.text = _T("Mount Point");
    columnInfo.nColumnWidth = 200;
    pHeaderItem = m_pComputerListCtrl->InsertColumn(-1, columnInfo);
    ASSERT(pHeaderItem != nullptr);
    m_columnIdMap[ComputerViewColumn::kMountOn] = pHeaderItem->GetColumnId();
}

void ComputerView::ShowMyComputerContents_Linux(ui::ImageListPtr pImageList, const std::vector<ui::DirectoryTree::DiskInfo>& diskInfoList)
{
    ui::ListCtrlSubItemData itemData;
    size_t nItemIndex = 0;
    size_t nColumnId = 0;
    for (size_t nIndex = 0; nIndex < diskInfoList.size(); ++nIndex) {
        const ui::DirectoryTree::DiskInfo& diskInfo = diskInfoList[nIndex];
        nItemIndex = m_pComputerListCtrl->AddDataItem(itemData);
        if (!ui::Box::IsValidItemIndex(nItemIndex)) {
            continue;
        }
        // Record the association
        m_pComputerListCtrl->SetDataItemUserData(nItemIndex, nIndex);

        // Set the icon
        if (pImageList != nullptr) {
            DString iconString = ui::GlobalManager::Instance().Icon().GetIconString(diskInfo.m_nIconID);
            if (!iconString.empty()) {
                int32_t nImageId = pImageList->AddImageString(iconString, m_pMainForm->Dpi());
                m_iconToImageMap[diskInfo.m_nIconID] = nImageId;
                m_pComputerListCtrl->SetDataItemImageId(nItemIndex, nImageId);
            }
        }

        ui::ListCtrlSubItemData subItemData;
        subItemData.nTextFormat = ui::DrawStringFormat::TEXT_LEFT | ui::DrawStringFormat::TEXT_VCENTER;
        subItemData.text = diskInfo.m_displayName;
        nColumnId = GetColumnId(ComputerViewColumn::kName);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // File system

        subItemData.nTextFormat = ui::DrawStringFormat::TEXT_HCENTER | ui::DrawStringFormat::TEXT_VCENTER;

        subItemData.text = GetDeviceTypeString(diskInfo.m_deviceType);
        nColumnId = GetColumnId(ComputerViewColumn::kType);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Device type

        subItemData.text = diskInfo.m_fileSystem;
        nColumnId = GetColumnId(ComputerViewColumn::kPartitionType);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Partition type

        subItemData.text = FormatDiskSpace(diskInfo.m_totalBytes);
        nColumnId = GetColumnId(ComputerViewColumn::kTotalSpace);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Total size

        // Set the sort order of the "Total Size" column
        m_pComputerListCtrl->SetSubItemUserDataNById(nItemIndex, nColumnId, diskInfo.m_totalBytes);
        m_pComputerListCtrl->SetColumnSortFlagById(nColumnId, ui::ListCtrlSubItemSortFlag::kSortByUserDataN);

        subItemData.text = FormatDiskSpace(diskInfo.m_freeBytes);
        nColumnId = GetColumnId(ComputerViewColumn::kFreeSpace);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Free space

        // Set the sort order of the "Free Space" column
        m_pComputerListCtrl->SetSubItemUserDataNById(nItemIndex, nColumnId, diskInfo.m_freeBytes);
        m_pComputerListCtrl->SetColumnSortFlagById(nColumnId, ui::ListCtrlSubItemSortFlag::kSortByUserDataN);

        subItemData.text = FormatUsedPercent(diskInfo.m_totalBytes, diskInfo.m_freeBytes);
        nColumnId = GetColumnId(ComputerViewColumn::kUsedPercent);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Used percentage

        // Set the sort order of the "Used Percentage" column
        uint64_t nUsedPercent = 100 * 1000;
        if (diskInfo.m_totalBytes != 0) {
            nUsedPercent = (diskInfo.m_totalBytes - diskInfo.m_freeBytes) * 1000 / diskInfo.m_totalBytes;
        }
        m_pComputerListCtrl->SetSubItemUserDataNById(nItemIndex, nColumnId, (size_t)nUsedPercent);
        m_pComputerListCtrl->SetColumnSortFlagById(nColumnId, ui::ListCtrlSubItemSortFlag::kSortByUserDataN);

        subItemData.text = diskInfo.m_mountOn;
        subItemData.nTextFormat = ui::DrawStringFormat::TEXT_LEFT | ui::DrawStringFormat::TEXT_VCENTER;
        nColumnId = GetColumnId(ComputerViewColumn::kMountOn);
        m_pComputerListCtrl->SetSubItemDataById(nItemIndex, nColumnId, subItemData); // Mount point
    }
}

DString ComputerView::GetDeviceTypeString(ui::DirectoryTree::DeviceType deviceType) const
{
    DString deviceTypeString = _T("Unknown");
    switch (deviceType) {
    case ui::DirectoryTree::DeviceType::HDD:
        deviceTypeString = _T("HDD");
        break;
    case ui::DirectoryTree::DeviceType::SSD:
        deviceTypeString = _T("SATA SSD");
        break;
    case ui::DirectoryTree::DeviceType::NVME:
        deviceTypeString = _T("NVMe SSD");
        break;
    case ui::DirectoryTree::DeviceType::USB:
        deviceTypeString = _T("USB Storage");
        break;
    case ui::DirectoryTree::DeviceType::SD_CARD:
        deviceTypeString = _T("SD Card");
        break;
    case ui::DirectoryTree::DeviceType::CDROM:
        deviceTypeString = _T("CD/DVD");
        break;
    case ui::DirectoryTree::DeviceType::LOOP:
        deviceTypeString = _T("LOOP Virtual Storage");
        break;
    case ui::DirectoryTree::DeviceType::VIRT_DISK:
        deviceTypeString = _T("Virtual Storage");
        break;
    case ui::DirectoryTree::DeviceType::RAMDISK:
        deviceTypeString = _T("RAM Disk");
        break;
    case ui::DirectoryTree::DeviceType::NFS:
        deviceTypeString = _T("NFS");
        break;
    case ui::DirectoryTree::DeviceType::SHARE:
        deviceTypeString = _T("Shared Folder");
        break;
    default:
        break;
    }
    return deviceTypeString;
}

#endif //DUILIB_BUILD_FOR_LINUX
