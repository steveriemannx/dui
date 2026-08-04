#ifndef UI_UTILS_FILE_TIME_H_
#define UI_UTILS_FILE_TIME_H_

#include "dui/Core/UiTypes.h"

namespace ui
{
/** File time class
*/
class DUI_API FileTime
{
public:
    FileTime();
    ~FileTime();

public:
#ifdef DUI_BUILD_FOR_WIN
    /** Convert from Windows FILETIME
    * @param [in] ft The file time on the Windows platform, calculated from January 1, 1601 UTC, in units of 100 nanoseconds
    */
    void FromFileTime(const FILETIME& ft);

    /** Convert to Windows FILETIME
    */
    FILETIME ToFileTime() const;
#else
    /** Convert from Linux system time
    * @param [in] secondsSinceEpoch The file time on the Linux system, with January 1, 1970 UTC as the starting point (Epoch), in units of seconds
    */
    void FromSecondsSinceEpoch(uint64_t secondsSinceEpoch);

    /** Convert to Linux system time
    * @return The file time on the Linux system, with January 1, 1970 UTC as the starting point (Epoch), in units of seconds
    */
    uint64_t ToSecondsSinceEpoch() const;

#endif

    /** Get the time value as an integer. The specific meaning of the file time value differs between systems:
     *  On Windows systems: calculated from January 1, 1601 UTC, in units of 100 nanoseconds
     *  On Linux systems: with January 1, 1970 UTC as the starting point (Epoch), in units of nanoseconds
     */
    uint64_t GetValue() const;

    /** Convert the file time to a string (local time: year-month-day hour:minute:second)
    */
    DString ToString() const;

private:
    /** The specific value of the file time, with different meanings on different systems:
     *    On Windows systems: calculated from January 1, 1601 UTC, in units of 100 nanoseconds
     *    On Linux systems: with January 1, 1970 UTC as the starting point (Epoch), in units of seconds
     */
    uint64_t m_uFileTime;
};

} //namespace ui

#endif // UI_UTILS_FILE_TIME_H_
