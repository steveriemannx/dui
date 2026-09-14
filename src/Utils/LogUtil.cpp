#include "dui/Utils/LogUtil.h"
#include "dui/Utils/StringUtil.h"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <mutex>

#if defined(DUI_BUILD_FOR_MACOS) && defined(__has_include)
    #if __has_include(<os/log.h>)
        #include <os/log.h>
        /** The unified log is available; log messages go there as well as to stderr
        */
        #define DUI_LOG_HAS_OS_LOG 1
    #endif
#endif

namespace ui
{
/** Timestamp of program startup
*/
static std::chrono::steady_clock::time_point s_startTime = std::chrono::steady_clock::now();

/** Value of s_logLevel before DUI_LOG_LEVEL has been read
*/
static const int32_t kLogLevelUnset = -1;

/** The active log level. Written by SetLevel() and by the one-time environment
parse, read by every log call on every thread, hence atomic.
*/
static std::atomic<int32_t> s_logLevel(kLogLevelUnset);
static std::once_flag s_envLevelFlag;

/** Set when DUI_LOG_LEVEL holds a value that could not be parsed, so that the
mistake is reported once, as soon as there is a level to report it at.
*/
static std::atomic<bool> s_bBadEnvLevel(false);

#if defined(DUI_BUILD_FOR_WIN)
/** The debugger's output window wants CRLF
*/
static const char* const kLogLineEnding = "\r\n";
#else
static const char* const kLogLineEnding = "\n";
#endif

/** The lock that keeps one log line whole.

    This library logs from the UI thread, from the worker and image-decode
    threads, and (from a TimerManager callback) from whichever thread owns the
    timer, so a message must not be able to land in the middle of another one.
    The message is composed before the lock is taken and handed to the sink in a
    single call, so the lock is held for one write, not for one format.

    The mutex is deliberately leaked. A log call may come from another static
    object's destructor, which runs after a function-local static of this
    function would have been destroyed; locking a destroyed mutex throws out of
    a destructor, and that terminates the program.
*/
static std::mutex& GetSinkMutex()
{
    static std::mutex* const sinkMutex = new std::mutex;
    return *sinkMutex;
}

#if defined(DUI_LOG_HAS_OS_LOG)
/** The unified-log handle. os_log_create is safe to call from several threads;
the handle is created once, on the first message that is written.
*/
static os_log_t GetOsLogHandle()
{
    static os_log_t s_osLogHandle = os_log_create("com.dui.framework", "dui");
    return s_osLogHandle;
}

/** The unified-log type for a level. TRACE and DEBUG are OS_LOG_TYPE_DEBUG and
INFO is OS_LOG_TYPE_INFO: neither is persisted by default, which is the right
default for the levels that are off by default here as well.
*/
static os_log_type_t GetOsLogType(LogLevel level)
{
    switch (level) {
    case LogLevel::kTrace:
    case LogLevel::kDebug:
        return OS_LOG_TYPE_DEBUG;
    case LogLevel::kInfo:
        return OS_LOG_TYPE_INFO;
    case LogLevel::kError:
        return OS_LOG_TYPE_ERROR;
    case LogLevel::kWarn:
    default:
        return OS_LOG_TYPE_DEFAULT;
    }
}
#endif

/** Write an already-composed log line to the platform sink

* @param [in] logMsg The complete line, ending in the platform line ending
* @param [in] level The level the line was logged at; used only to pick the
*             unified-log type, the filtering has already happened
* @note The sink exists on every platform. The version of this function that had
*       a DUI_BUILD_FOR_WIN branch and a DUI_BUILD_FOR_WAYLAND branch and no
*       else silently discarded every message on macOS and on X11, which is what
*       this logging system exists to stop.
*/
static void WriteToSink(const std::string& logMsg, LogLevel level)
{
    std::lock_guard<std::mutex> lock(GetSinkMutex());
#if defined(DUI_BUILD_FOR_WIN)
    //OutputDebugStringA explicitly: with UNICODE defined the unsuffixed name is
    //the wide version, and the log is UTF-8
    ::OutputDebugStringA(logMsg.c_str());
#else
    //stderr on macOS, Linux, FreeBSD, and under both the X11 and the Wayland
    //backend: it reaches the terminal, a launcher's log file, and CI
    std::fwrite(logMsg.data(), 1, logMsg.size(), stderr);
    std::fflush(stderr);
#endif
#if defined(DUI_LOG_HAS_OS_LOG)
    //Hand it to the unified log too: an application launched from Finder has no
    //terminal, and this is the only place its failures can be read afterwards
    //    log stream --predicate 'subsystem == "com.dui.framework"'
    os_log_with_type(GetOsLogHandle(), GetOsLogType(level), "%{public}s", logMsg.c_str());
#endif
}

/** Read DUI_LOG_LEVEL once, on the first log call

* @note The default is WARN. A UI library that announces every resource it loads
*       buries the two lines that matter, so the default only speaks up when
*       something was skipped or failed.
*/
static void InitLogLevel()
{
    std::call_once(s_envLevelFlag, []() {
        LogLevel level = LogLevel::kWarn;
        const char* pEnvLevel = std::getenv("DUI_LOG_LEVEL");
        if (pEnvLevel != nullptr) {
            LogLevel parsedLevel = level;
            if (LogUtil::ParseLevel(pEnvLevel, parsedLevel)) {
                level = parsedLevel;
            }
            else {
                s_bBadEnvLevel.store(true, std::memory_order_relaxed);
            }
        }
        s_logLevel.store(static_cast<int32_t>(level), std::memory_order_relaxed);
    });
    if (s_bBadEnvLevel.exchange(false, std::memory_order_relaxed)) {
        //Report it once, and from here rather than from inside the call_once:
        //Write() calls IsLevelEnabled(), which comes back through this function,
        //and std::call_once does not re-enter
        LogUtil::Write(LogLevel::kWarn,
                       "DUI_LOG_LEVEL is not a level name; expected trace, debug, info, warn, error or off. Using the default (warn)",
                       __FILE__, __LINE__);
    }
}

std::string LogUtil::GetTimeStamp()
{
    //The time interval since system startup, accurate to milliseconds
    std::chrono::steady_clock::time_point nowTime = std::chrono::steady_clock::now();
    auto thisTime = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - s_startTime);
    uint64_t nTimeMs = (uint64_t)thisTime.count();
    //Each field is the part of the elapsed time that belongs to it: the minutes are
    //the minutes past the hour, not the total number of minutes. The earlier
    //version printed the totals, so one hour of uptime read "01:60:3600".
    uint32_t nHH = (uint32_t)(nTimeMs / 1000 / 60 / 60);
    uint32_t nMM = (uint32_t)((nTimeMs / 1000 / 60) % 60);
    uint32_t nSS = (uint32_t)((nTimeMs / 1000) % 60);
    uint32_t nMS = (uint32_t)(nTimeMs % 1000);
    return StringUtil::Printf("%02u:%02u:%02u.%03u ", nHH, nMM, nSS, nMS);
}

void LogUtil::SetLevel(LogLevel level)
{
    //Read the environment first, so that the explicit level is not overwritten
    //by it later
    InitLogLevel();
    s_logLevel.store(static_cast<int32_t>(level), std::memory_order_relaxed);
}

LogLevel LogUtil::GetLevel()
{
    InitLogLevel();
    return static_cast<LogLevel>(s_logLevel.load(std::memory_order_relaxed));
}

bool LogUtil::ParseLevel(const std::string& levelName, LogLevel& level)
{
    std::string name = StringUtil::MakeLowerString(levelName);
    StringUtil::Trim(name);
    if (name == "trace") {
        level = LogLevel::kTrace;
    }
    else if (name == "debug") {
        level = LogLevel::kDebug;
    }
    else if (name == "info") {
        level = LogLevel::kInfo;
    }
    else if (name == "warn" || name == "warning") {
        level = LogLevel::kWarn;
    }
    else if (name == "error") {
        level = LogLevel::kError;
    }
    else if (name == "off" || name == "none") {
        level = LogLevel::kOff;
    }
    else if ((name.size() == 1) && (name[0] >= '0') && (name[0] <= '5')) {
        //The numeric form of the level, for those who prefer it
        level = static_cast<LogLevel>(name[0] - '0');
    }
    else {
        return false;
    }
    return true;
}

const char* LogUtil::GetLevelName(LogLevel level)
{
    switch (level) {
    case LogLevel::kTrace:
        return "TRACE";
    case LogLevel::kDebug:
        return "DEBUG";
    case LogLevel::kInfo:
        return "INFO";
    case LogLevel::kWarn:
        return "WARN";
    case LogLevel::kError:
        return "ERROR";
    case LogLevel::kOff:
        return "OFF";
    default:
        return "UNKNOWN";
    }
}

bool LogUtil::IsLevelEnabled(LogLevel level)
{
    if (!IsLevelCompiled(level) || (level == LogLevel::kOff)) {
        return false;
    }
    return static_cast<int32_t>(level) >= static_cast<int32_t>(GetLevel());
}

void LogUtil::Write(LogLevel level, const std::string& message, const char* file, int line)
{
    if (!IsLevelEnabled(level)) {
        return;
    }
    std::string logMsg;
    logMsg.reserve(message.size() + 64);
    logMsg += GetTimeStamp();
    logMsg += '[';
    logMsg += GetLevelName(level);
    logMsg += "] ";
    if (file != nullptr) {
        //Only the file name: the path is the build directory of whoever compiled
        //the library, which the reader of the log neither has nor needs
        const char* pFileName = file;
        for (const char* p = file; *p != '\0'; ++p) {
            if ((*p == '/') || (*p == '\\')) {
                pFileName = p + 1;
            }
        }
        logMsg += pFileName;
        logMsg += ':';
        logMsg += StringUtil::Int32ToString((int32_t)line);
        logMsg += ' ';
    }
    logMsg += message;
    logMsg += kLogLineEnding;
    WriteToSink(logMsg, level);
}

void LogUtil::Output(const std::string& log, bool bPrintTime)
{
    std::string logMsg;
    if (bPrintTime) {
        logMsg = GetTimeStamp() + log;
    }
    else {
        logMsg = log;
    }
    //Raw output has no level of its own and is not filtered; it is written at the
    //level the unified log shows by default
    WriteToSink(logMsg, LogLevel::kWarn);
}

void LogUtil::OutputLine(const std::string& log, bool bPrintTime)
{
    Output(log + kLogLineEnding, bPrintTime);
}

} // namespace ui
