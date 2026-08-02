#ifndef UI_UTILS_PROCESS_SINGLETON_H_
#define UI_UTILS_PROCESS_SINGLETON_H_

#include "duilib/duilib_defs.h"

#include <vector>
#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <memory>

namespace ui
{
/** Callback function for cross-process singleton control
 * @param [in] argumentList Startup argument list of the newly launched process (passed to the already-running process via cross-process communication)
 */
typedef std::function<void (const std::vector<DString>& argumentList)> OnAlreadyRunningAppRelaunchEvent;

/** Cross-process singleton implementation
*/
class DUILIB_API ProcessSingleton
{
protected:
    explicit ProcessSingleton(const std::string& strAppName);
    ProcessSingleton(const ProcessSingleton&) = delete;
    ProcessSingleton& operator=(const ProcessSingleton&) = delete;

public:
    virtual ~ProcessSingleton();

    /** Create an instance
    * @param strAppName A string identifying an application; the same identifier is treated as the same program
    */
    static std::unique_ptr<ProcessSingleton> Create(const DString& strAppName);

public:
    /** Whether another process is running
    */
    bool IsAnotherInstanceRunning();

    /** Send arguments to an already-running process
    * @param vecArgs The list of arguments to send; each item is at most 1024 bytes and the total data length is at most 4096 bytes
    */
    bool SendArgumentsToExistingInstance(const std::vector<std::string>& vecArgs);

    /** Set the callback function for the app-relaunch event
    * @param fnCallback Callback function used to receive the startup arguments of the newly launched process
    */
    void StartListener(OnAlreadyRunningAppRelaunchEvent fnCallback);

protected:
    /** Initialize platform components
    */
    virtual void InitializePlatformComponents() = 0;

    /** Clean up platform components
    */
    virtual void CleanupPlatformComponents() = 0;

    /** Check whether another process is running
    */
    virtual bool PlatformCheckInstance() = 0;

    /** Send data to an already-running process
    */
    virtual bool PlatformSendData(const std::string& strData) = 0;

    /** Listen for events of other processes starting
    */
    virtual void PlatformListen() = 0;

protected:    
    /** Record an error log
    */
    void LogError(const std::string& strMessage);

    /** A new process has started
    */
    void OnAlreadyRunningAppRelaunch(const std::vector<std::string>& args);

protected:
    /** A string identifying an application; the same identifier is treated as the same program
    */
    std::string m_strAppName;

    /** Thread listening for process start events
    */
    std::thread m_thListener;

    /** Whether it is running
    */
    std::atomic<bool> m_bRunning;

private:

    /** Callback function for the app-relaunch event, used to receive the startup arguments of the newly launched process
    */
    OnAlreadyRunningAppRelaunchEvent m_fnCallback;
};

}

#endif // UI_UTILS_PROCESS_SINGLETON_H_
