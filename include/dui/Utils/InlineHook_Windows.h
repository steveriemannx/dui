#ifndef UI_UTILS_INLINE_HOOK_WINDOWS_H_
#define UI_UTILS_INLINE_HOOK_WINDOWS_H_

#include "dui/dui_defs.h"

//Feature switch (if this macro is not defined, the feature is disabled)
#define DUI_ENABLE_INLINE_HOOK 1

#ifdef DUI_BUILD_FOR_WIN
#include "dui/dui_config_windows.h"
#include <vector>
#include <atomic>

namespace ui 
{
/** Encapsulation of inline hook functionality
*/
class InlineHook
{
public:
    InlineHook();
    ~InlineHook();
    InlineHook(const InlineHook&) = delete;
    InlineHook& operator = (const InlineHook&) = delete;

public:
    //Install the hook
    bool Install(void* targetFunc, void* hookFunc);

    //Remove the hook
    bool Uninstall();

    // Get the trampoline address (used to call the original function)
    template<typename T>
    T GetTrampoline() const {
        return reinterpret_cast<T>((char*)m_trampoline);
    }

private:
    //Install the hook
    bool DoInstall(void);

    //Remove the hook
    bool DoUninstall();

private:
    /** Generate the jump code
    */
    void GenerateJump();

    /** Create the trampoline function
    */
    bool CreateTrampoline();

    /** Clear internal state
    */
    void Clear();

private:
    void* m_target;         // Address of the target function
    void* m_hook;           // Address of the hook function
    void* m_trampoline;     // Address of the trampoline
    std::vector<uint8_t> m_original;    // Backup of the original instructions
    std::vector<uint8_t> m_jumpCode;    // Jump instructions
    std::atomic<int32_t> m_installRef;  // Installation state
    size_t m_replaceLen;    // Length of instructions to be replaced
};
    
} //namespace ui 

#endif //DUI_BUILD_FOR_WIN

#endif //UI_UTILS_INLINE_HOOK_WINDOWS_H_

