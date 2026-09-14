#ifndef UI_UTILS_LOG_UTIL_H_
#define UI_UTILS_LOG_UTIL_H_

#include "dui/dui_defs.h"

namespace ui
{

/** Log severity levels.

    The set is deliberately small. TRACE and DEBUG are developer-facing and off by
    default. INFO is for lifecycle events an application may want to see once.
    WARN and ERROR are the two levels a user of the library is expected to act on:
    WARN means something was skipped or fell back to a default (a missing font file,
    an unknown XML node name), ERROR means an operation failed and its result is
    missing (a layout file that could not be loaded).
*/
enum class LogLevel : int8_t
{
    kTrace = 0,     // Per-item detail: every resource loaded, every decode started
    kDebug = 1,     // Diagnostics for one subsystem, useful while working on it
    kInfo  = 2,     // Lifecycle: startup, theme switched, window created
    kWarn  = 3,     // Something was skipped or fell back; the UI still works
    kError = 4,     // An operation failed and its result is missing
    kOff   = 5      // Emit nothing; valid for SetLevel() and DUI_LOG_LEVEL only
};

/** The lowest level compiled into the binary.

    Call sites below it are removed by the compiler, arguments included, so a
    release build can drop TRACE/DEBUG entirely with
    -DDUI_LOG_COMPILE_LEVEL=2 (see LogUtil::IsLevelCompiled). The default keeps
    every level compiled in, leaving the runtime level (DUI_LOG_LEVEL) in control.
*/
#ifndef DUI_LOG_COMPILE_LEVEL
    #define DUI_LOG_COMPILE_LEVEL 0
#endif

/** Log output utility

    Output goes to a sink that exists on every platform:
      - Windows:      OutputDebugStringA (the debugger's output window)
      - macOS:        stderr, and the unified log (os_log, subsystem
                      "com.dui.framework", category "dui") so that a shipped .app
                      with no terminal still leaves a trace in Console.app
      - Linux/FreeBSD: stderr (X11 and Wayland backends alike)

    The default level is LogLevel::kWarn: a UI library that prints on every
    resource load is worse than one that prints nothing. The level can be changed
    at runtime with SetLevel(), or with the environment variable DUI_LOG_LEVEL,
    which is read once, on the first log call, and accepts "trace", "debug",
    "info", "warn", "error" and "off" (case-insensitive), or the matching number
    0..5. Debugging a resource that will not load is therefore:

        DUI_LOG_LEVEL=debug ./MyApp

    A log call may be made from any thread. The level is atomic and each message
    is written to the sink in a single locked call, so lines from the worker and
    image-decode threads never interleave.
*/
class DUI_API LogUtil
{
public:
    /** Set the active log level
    * @param [in] level The level to accept; kOff disables all output
    * @note Overrides DUI_LOG_LEVEL, whenever it is called
    */
    static void SetLevel(LogLevel level);

    /** Get the active log level
    * @return The level in effect (the environment variable on first use, kWarn if unset)
    */
    static LogLevel GetLevel();

    /** Parse a level name
    * @param [in] levelName "trace", "debug", "info", "warn", "error", "off" or "0".."5", case-insensitive
    * @param [out] level The parsed level; unchanged if the name is not recognized
    * @return true if the name was recognized
    */
    static bool ParseLevel(const std::string& levelName, LogLevel& level);

    /** Get the name of a level
    * @param [in] level The level
    * @return The upper-case name ("TRACE", "DEBUG", "INFO", "WARN", "ERROR", "OFF")
    */
    static const char* GetLevelName(LogLevel level);

    /** Whether a level is compiled into this binary
    * @param [in] level The level to test
    * @return true if level >= DUI_LOG_COMPILE_LEVEL
    * @note constexpr: with a constant level argument the check folds away, which is
    *       what lets the DUI_LOG_* macros drop a call site at compile time
    */
    static constexpr bool IsLevelCompiled(LogLevel level)
    {
        return static_cast<int32_t>(level) >= DUI_LOG_COMPILE_LEVEL;
    }

    /** Whether a level is currently emitted
    * @param [in] level The level to test
    * @return true if the level passes the compile-time floor and the active level
    * @note Cheap enough for a hot path: an atomic load and a compare, no I/O
    */
    static bool IsLevelEnabled(LogLevel level);

    /** Write one log message
    * @param [in] level The severity of the message
    * @param [in] message The message; it should name what failed and where
    * @param [in] file The source file of the call site, normally __FILE__
    * @param [in] line The source line of the call site, normally __LINE__
    * @note The DUI_LOG_* macros are the intended entry point: they skip the call
    *       entirely when the level is filtered, so the message is never built
    */
    static void Write(LogLevel level, const std::string& message, const char* file = nullptr, int line = 0);

    /** Output a raw log, ignoring the level filter (kept for compatibility)
    * @param [in] log The log content
    * @param [in] bPrintTime true means the timestamp is printed, false means the timestamp is not printed
    */
    static void Output(const std::string& log, bool bPrintTime = true);

    /** Output a raw log (appends a line break), ignoring the level filter
    * @param [in] log The log content
    * @param [in] bPrintTime true means the timestamp is printed, false means the timestamp is not printed
    */
    static void OutputLine(const std::string& log, bool bPrintTime = true);

private:
    /** Get the timestamp string
    */
    static std::string GetTimeStamp();
};

}

/** Write a log message if the level is enabled.

    The message expression is evaluated only when the level passes, so a call that
    is filtered out costs one atomic load and no allocation:

        DUI_LOG_WARN(StringUtil::Printf("image %s not found, using the fallback icon",
                                        imageName.c_str()));
*/
#define DUI_LOG(level, message)                                                         \
    do {                                                                                \
        if (::ui::LogUtil::IsLevelCompiled(level) &&                                    \
            ::ui::LogUtil::IsLevelEnabled(level)) {                                     \
            ::ui::LogUtil::Write((level), (message), __FILE__, __LINE__);               \
        }                                                                               \
    } while (false)

#define DUI_LOG_TRACE(message)  DUI_LOG(::ui::LogLevel::kTrace, message)
#define DUI_LOG_DEBUG(message)  DUI_LOG(::ui::LogLevel::kDebug, message)
#define DUI_LOG_INFO(message)   DUI_LOG(::ui::LogLevel::kInfo, message)
#define DUI_LOG_WARN(message)   DUI_LOG(::ui::LogLevel::kWarn, message)
#define DUI_LOG_ERROR(message)  DUI_LOG(::ui::LogLevel::kError, message)

#endif // UI_UTILS_LOG_UTIL_H_
