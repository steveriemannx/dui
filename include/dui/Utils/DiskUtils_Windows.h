#ifndef EXAMPLES_DISK_UTILS_WINDOWS_H_
#define EXAMPLES_DISK_UTILS_WINDOWS_H_

#include "dui/dui_config.h"

#ifdef DUI_BUILD_FOR_WIN
#include <string>
#include <vector>

/** Disk partition information helper class (string encoding type: UTF-16 or UTF-8)
*/
class DUI_API DiskUtils
{
public:

    /** Detailed information of a disk partition
    */
    struct DiskInfo
    { 
        /** The volume label of the partition
        */
        DString m_volumeName;
        
        /** The partition type
        */
        DString m_volumeType;
        
        /** The file system type, such as NTFS, etc.
        */
        DString m_fileSystem;
        
        /** The cluster size
        */
        uint32_t m_clusterBytes = 0;
        
        /** The total size
        */
        uint64_t m_totalBytes = 0;
        
        /** The size of the available space
        */
        uint64_t m_freeBytes = 0;
        
        /** Whether it contains a file system
        */
        bool m_hasFileSystem = false;
    };
    
public:
    
    /** Get the list of local disks
    @param [out] Returns the list of local disks; the disk format is like: "C:\\", etc.
    @return Returns true on success, otherwise returns false
    */
    static bool GetLogicalDriveList(std::vector<DString>& driveList);
    
    /** Get the partition information
    @param [in] driveString The drive name, in a format like: "C:\\"
    @param [out] diskInfo Returns the corresponding disk information
    @return Returns true on success, otherwise returns false
    */
    static bool GetLogicalDriveInfo(const DString& driveString, DiskInfo& diskInfo);

    /** Get the current drive letter from a path
    @param [in] path The current path
    @return The drive letter name
    */
    static DString GetDriveFromDirectoryPath(const DString& path);

    /** Get the drive letter of the local disk with the largest free space
    @return Returns the drive letter of the disk, e.g.: "C:\\"
    */
    static DString GetMaxFreeSpaceLocalDisk();

    /** Get the remaining disk space of the specified directory
    @param[in] fullDirectory The file directory
    @return The remaining disk space size (in BYTEs)
    */
    static uint64_t GetFreeDiskSpace(const DString& fullDirectory);
};

#endif //DUI_BUILD_FOR_WIN

#endif //EXAMPLES_DISK_UTILS_WINDOWS_H_
