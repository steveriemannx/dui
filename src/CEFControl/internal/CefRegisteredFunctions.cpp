#include "dui/CEFControl/internal/CefRegisteredFunctions.h"

#ifdef DUI_BUILD_FOR_CEF

#include "dui/Utils/StringUtil.h"

namespace ui
{
bool RenderRegisteredFunctions::AddJsFunction(const CefString& function_name, const CefString& frame_id,
                                              const CefRefPtr<CefV8Value>& function, bool enable_replace)
{
    ASSERT(!function_name.empty() && !frame_id.empty() && (function != nullptr));
    if (function_name.empty() || frame_id.empty() || (function == nullptr)) {
        return false;
    }
    bool bRet = true;
    FrameFunctionMap& frameFunctionMap = m_renderJsFunctionMap[function_name];
    auto iter = frameFunctionMap.find(frame_id);
    if (iter != frameFunctionMap.end()) {
        // Already exists
        if (enable_replace) {
            frameFunctionMap[frame_id] = function; // Overwrite is allowed
        }
        else {
            bRet = false;//Overwriting is not allowed
        }
    }
    else {
        // Does not exist, add it
        frameFunctionMap[frame_id] = function;
    }
    return bRet;
}

CefRefPtr<CefV8Value> RenderRegisteredFunctions::FindJsFunction(const CefString& function_name, const CefString& frame_id)
{
    CefRefPtr<CefV8Value> function = nullptr;
    auto iter = m_renderJsFunctionMap.find(function_name);
    if (iter != m_renderJsFunctionMap.end()) {
        const FrameFunctionMap& frameFunctionMap = iter->second;
        auto pos = frameFunctionMap.find(frame_id);
        if (pos != frameFunctionMap.end()) {
            function = pos->second;
        }
    }
    return function;
}

void RenderRegisteredFunctions::RemoveJsFunction(const CefString& function_name)
{
    auto iter = m_renderJsFunctionMap.find(function_name);
    if (iter != m_renderJsFunctionMap.end()) {
        m_renderJsFunctionMap.erase(iter);
    }
}

void RenderRegisteredFunctions::RemoveJsFunction(const CefString& function_name, const CefString& frame_id)
{
    auto iter = m_renderJsFunctionMap.find(function_name);
    if (iter != m_renderJsFunctionMap.end()) {
        FrameFunctionMap& frameFunctionMap = iter->second;
        auto pos = frameFunctionMap.find(frame_id);
        if (pos != frameFunctionMap.end()) {
            frameFunctionMap.erase(pos);
        }
        if (frameFunctionMap.empty()) {
            m_renderJsFunctionMap.erase(iter);
        }
    }
}

void RenderRegisteredFunctions::RemoveJsFunctionByFrameId(const CefString& frame_id)
{
    auto iter = m_renderJsFunctionMap.begin();
    while (iter != m_renderJsFunctionMap.end()) {
        FrameFunctionMap& frameFunctionMap = iter->second;
        auto pos = frameFunctionMap.find(frame_id);
        if (pos != frameFunctionMap.end()) {
            frameFunctionMap.erase(pos);
        }
        if (frameFunctionMap.empty()) {
            iter = m_renderJsFunctionMap.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

void RenderRegisteredFunctions::RemoveJsFunctionByFrame(CefRefPtr<CefFrame> frame)
{
    ASSERT(frame != nullptr);
    if (frame == nullptr) {
        return;
    }

    // Each render and browser process has its own exclusive instance of this class, rather than a singleton
    // So the browser obtained here is globally unique, and all frames and contexts can be obtained through this browser
    auto browser = frame->GetBrowser();
    ASSERT(browser != nullptr);
    if (browser == nullptr) {
        return;
    }

    if (!m_renderJsFunctionMap.empty()) {
        for (auto iter = m_renderJsFunctionMap.begin(); iter != m_renderJsFunctionMap.end();) {
            FrameFunctionMap& frameFunctionMap = iter->second;            
            for (auto pos = frameFunctionMap.begin(); pos != frameFunctionMap.end();) {
#if CEF_VERSION_MAJOR <= 109
                //CEF 109 version
                int64 identifier = StringUtil::StringToInt64(pos->first.c_str());
                auto child_frame = browser->GetFrame(identifier);
#else
                //CEF newer versions
                auto child_frame = browser->GetFrameByIdentifier(pos->first);
#endif
                if (child_frame.get() && child_frame->GetV8Context()->IsSame(frame->GetV8Context())) {
                    pos = frameFunctionMap.erase(pos);
                }
                else {
                    ++pos;
                }
            }

            if (frameFunctionMap.empty()) {
                iter = m_renderJsFunctionMap.erase(iter);
            }
            else {
                ++iter;
            }
        }
    }
}

void RenderRegisteredFunctions::ClearAllJsFunctions()
{
    m_renderJsFunctionMap.clear();
}

/////////////////////////////////////////////////////////////
bool BrowserRegisteredFunctions::AddCppFunction(const CefString& function_name, int64_t browser_id,
                                                CppFunction function, bool enable_replace)
{
    ASSERT(!function_name.empty() && (function != nullptr));
    if (function_name.empty() || (function == nullptr)) {
        return false;
    }
    bool bRet = true;
    BrowserFunctionMap& browserFunctionMap = m_browserCppFunctionMap[function_name];
    auto iter = browserFunctionMap.find(browser_id);
    if (iter != browserFunctionMap.end()) {
        // Already exists
        if (enable_replace) {
            browserFunctionMap[browser_id] = function; // Overwrite is allowed
        }
        else {
            bRet = false;//Overwriting is not allowed
        }
    }
    else {
        // Does not exist, add it
        browserFunctionMap[browser_id] = function;
    }
    return bRet;
}

CppFunction BrowserRegisteredFunctions::FindCppFunction(const CefString& function_name, int64_t browser_id)
{
    CppFunction function = nullptr;
    auto iter = m_browserCppFunctionMap.find(function_name);
    if (iter != m_browserCppFunctionMap.end()) {
        const BrowserFunctionMap& browserFunctionMap = iter->second;
        auto pos = browserFunctionMap.find(browser_id);
        if (pos != browserFunctionMap.end()) {
            function = pos->second;
        }
    }
    return function;
}

void BrowserRegisteredFunctions::RemoveCppFunction(const CefString& function_name)
{
    auto iter = m_browserCppFunctionMap.find(function_name);
    if (iter != m_browserCppFunctionMap.end()) {
        m_browserCppFunctionMap.erase(iter);
    }
}

void BrowserRegisteredFunctions::RemoveCppFunction(const CefString& function_name, int64_t browser_id)
{
    auto iter = m_browserCppFunctionMap.find(function_name);
    if (iter != m_browserCppFunctionMap.end()) {
        BrowserFunctionMap& browserFunctionMap = iter->second;
        auto pos = browserFunctionMap.find(browser_id);
        if (pos != browserFunctionMap.end()) {
            browserFunctionMap.erase(pos);
        }
        if (browserFunctionMap.empty()) {
            m_browserCppFunctionMap.erase(iter);
        }
    }
}

void BrowserRegisteredFunctions::ClearAllCppFunctions()
{
    m_browserCppFunctionMap.clear();
}

} //namespace ui

#endif //DUI_BUILD_FOR_CEF
