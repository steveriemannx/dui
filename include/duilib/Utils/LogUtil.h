#ifndef UI_UTILS_LOG_UTIL_H_
#define UI_UTILS_LOG_UTIL_H_

#include "duilib/duilib_defs.h"

namespace ui 
{

/** Log output utility
*/
class DUILIB_API LogUtil
{
public:
    /** Output a Debug log
    * @param [in] log The log content
    * @param [in] bPrintTime true means the timestamp is printed, false means the timestamp is not printed
    */
    static void Output(const DString& log, bool bPrintTime = true);

    /** Output a Debug log (appends a line break)
    * @param [in] log The log content
    * @param [in] bPrintTime true means the timestamp is printed, false means the timestamp is not printed
    */
    static void OutputLine(const DString& log, bool bPrintTime = true);

private:
    /** Get the timestamp string
    */
    static DString GetTimeStamp();
};

}

#endif // UI_UTILS_LOG_UTIL_H_
