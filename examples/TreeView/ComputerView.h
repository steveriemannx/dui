#ifndef EXAMPLES_COMPUTER_VIEW_H_
#define EXAMPLES_COMPUTER_VIEW_H_

// duilib
#include "duilib/duilib.h"
#include <map>

class MainForm;

/** The "Computer" view, showing the disk list
*/
class ComputerView: public ui::SupportWeakCallback
{
public:
    /** Constructor
    * @param [in] pMainForm interface of the associated window
    * @param [in] pListCtrl interface of the associated list control
    */
    ComputerView(MainForm* pMainForm, ui::ListCtrl* pListCtrl);

    /** Destructor
    */
    virtual ~ComputerView() override;

public:
    /** Show the content of the "Computer" node
    * @param [in] diskInfoList the information list of all disks
    */
    void ShowMyComputerContents(const std::vector<ui::DirectoryTree::DiskInfo>& diskInfoList);

private:
    /** Initialize
    */
    void Initialize();

    /** Initialize the header of the "Computer" view
    */
    void InitComputerViewHeader();

    /** Double-click in the "Computer" view
    */
    bool OnComuterViewDoubleClick(const ui::EventArgs& msg);    

    /** When an icon is removed, it must also be removed from the Image List synchronously (otherwise the icon may fail to load)
    * @param [in] nIconId the icon ID (in IconManager)
    */
    void OnRemoveIcon(uint32_t nIconId);

    /** Convert the disk space size to a display string
    */
    DString FormatDiskSpace(uint64_t nSpace) const;

    /** Get the used percentage display string
    */
    DString FormatUsedPercent(uint64_t nTotalSpace, uint64_t nFreeSpace) const;

#if defined (DUILIB_BUILD_FOR_WIN)
    /** Initialize the header
    */
    void InitComputerViewHeader_Win();

    /** Show the content of the "Computer" node
    * @param [in] diskInfoList the information list of all disks
    */
    void ShowMyComputerContents_Win(ui::ImageListPtr pImageList, const std::vector<ui::DirectoryTree::DiskInfo>& diskInfoList);

#elif defined (DUILIB_BUILD_FOR_LINUX)
    /** Initialize the header
    */
    void InitComputerViewHeader_Linux();

    /** Show the content of the "Computer" node
    * @param [in] diskInfoList the information list of all disks
    */
    void ShowMyComputerContents_Linux(ui::ImageListPtr pImageList, const std::vector<ui::DirectoryTree::DiskInfo>& diskInfoList);

    /** Convert the device type to a string
    */
    DString GetDeviceTypeString(ui::DirectoryTree::DeviceType deviceType) const;

#endif

private:
    /** The header of the "Computer" view
    */
    enum class ComputerViewColumn
    {
        kName,         // Name
        kType,         // Type
        kPartitionType,// Partition type
        kTotalSpace,   // Total size
        kFreeSpace,    // Free space
        kUsedPercent,  // Used percentage
        kMountOn       // Mount point
    };

    /** Get the real column index
    */
    size_t GetColumnId(ComputerViewColumn nOriginIndex) const;

private:
    /** The associated MainForm
    */
    MainForm* m_pMainForm;

private:
    /** Data of the "Computer" view
    */
    std::vector<ui::DirectoryTree::DiskInfo> m_diskInfoList;

    /** List interface of the "Computer" view
    */
    ui::ControlPtrT<ui::ListCtrl> m_pComputerListCtrl;

    /** Mapping table of each column's initial index to column ID in the "Computer" view (since reordering columns changes their indexes, data can no longer be added by column index)
    */
    std::map<ComputerViewColumn, size_t> m_columnIdMap;

    /** Mapping table from IconID to ImageID
    */
    std::map<uint32_t, int32_t> m_iconToImageMap;

    /** The ID of the icon deletion callback function
    */
    uint32_t m_nRemoveIconCallbackId;
};

#endif //EXAMPLES_COMPUTER_VIEW_H_
