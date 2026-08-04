#ifndef UI_UTILS_PERFORMANCE_UTIL_H_
#define UI_UTILS_PERFORMANCE_UTIL_H_

#include "dui/dui_defs.h"
#include <string>
#include <map>
#include <chrono>
#include <algorithm>

namespace ui 
{

/** Code execution performance analysis utility
*/
class DUI_API PerformanceUtil
{
public:
    PerformanceUtil();
    ~PerformanceUtil();

    /** Singleton instance
    */
    static PerformanceUtil& Instance();

    /** Code starts executing; start timing
    * @param [in] name Name of the statistic item
    */
    void BeginStat(const DString& name);

    /** Code finishes executing; measure execution performance
    * @param [in] name Name of the statistic item
    */
    void EndStat(const DString& name);
    
private:
    /** Records the result of each statistic
    */
    struct TStat
    {
        // Whether there is start data
        int32_t nStartRefCount = 0;

        // Start time
        std::chrono::steady_clock::time_point startTime;

        // End time
        std::chrono::steady_clock::time_point endTime;

        // Total code execution time: microseconds (one thousandth of a millisecond)
        std::chrono::microseconds totalTimes = std::chrono::microseconds::zero();

        // Total number of statistics
        uint32_t totalCount = 0;

        /** Single maximum: microseconds (one thousandth of a millisecond)
        */
        std::chrono::microseconds maxTime = std::chrono::microseconds::zero();
    };

    std::map<DString, TStat> m_stat;
};

class PerformanceStat
{
public:
    explicit PerformanceStat(const DString& statName):
        m_statName(statName)
    {
        PerformanceUtil::Instance().BeginStat(m_statName);
    }
    ~PerformanceStat()
    {
        PerformanceUtil::Instance().EndStat(m_statName);
    }
private:
    DString m_statName;
};

}

#endif // UI_UTILS_PERFORMANCE_UTIL_H_
