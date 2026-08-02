#ifndef UI_CEF_CONTROL_CEF_REGISTERED_FUNCTIONS_H_
#define UI_CEF_CONTROL_CEF_REGISTERED_FUNCTIONS_H_

#include "duilib/CEFControl/CefControlEvent.h"
#ifdef DUILIB_BUILD_FOR_CEF

#pragma warning (push)
#pragma warning (disable:4100)
    #include "include/cef_v8.h"
    #include "include/cef_version.h"
#pragma warning (pop)

#include <functional>
#include <map>

namespace ui
{
/** Render: register a JS function
*/
class RenderRegisteredFunctions
{
public:
    /** Add a JS function
     * @param [in] function_name the function name
     * @param [in] frame_id the ID of the CefFrame object
     * @param [in] function the JS function to register
     * @param [in] enable_replace whether overwriting is allowed
     */
    bool AddJsFunction(const CefString& function_name, const CefString& frame_id,
                       const CefRefPtr<CefV8Value>& function, bool enable_replace);

    /** Find an already registered JS function
     * @param [in] function_name the function name
     * @param [in] frame_id the ID of the CefFrame object
     */
    CefRefPtr<CefV8Value> FindJsFunction(const CefString& function_name, const CefString& frame_id);

    /** Remove an already registered JS function
    * @param [in] function_name the function name
    */
    void RemoveJsFunction(const CefString& function_name);

    /** Remove an already registered JS function
    * @param [in] function_name the function name
    * @param [in] frame_id the ID of the CefFrame object
    */
    void RemoveJsFunction(const CefString& function_name, const CefString& frame_id);

    /** Remove an already registered JS function
    * @param [in] frame_id the ID of the CefFrame object
    */
    void RemoveJsFunctionByFrameId(const CefString& frame_id);

    /** Remove an already registered JS function
    * @param [in] frame the CefFrame object
    */
    void RemoveJsFunctionByFrame(CefRefPtr<CefFrame> frame);

    /** Clear all registered functions
    */
    void ClearAllJsFunctions();

private:
    typedef std::map<CefString/* frame_id*/, CefRefPtr<CefV8Value>/* function*/> FrameFunctionMap;
    typedef std::map<CefString/* function_name*/, FrameFunctionMap> RenderRegisteredFunctionMap;

    // Function map
    RenderRegisteredFunctionMap m_renderJsFunctionMap;
};

/** Browser: register a Cpp function
*/
class BrowserRegisteredFunctions
{
public:
    /** Add a Cpp function
     * @param [in] function_name the function name
     * @param [in] browser_id the ID of the Browser object
     * @param [in] function the Cpp function to register
     * @param [in] enable_replace whether overwriting is allowed
     */
    bool AddCppFunction(const CefString& function_name, int64_t browser_id,
                        CppFunction function, bool enable_replace);

    /** Find an already registered Cpp function
     * @param [in] function_name the function name
     * @param [in] browser_id the ID of the Browser object
     */
    CppFunction FindCppFunction(const CefString& function_name, int64_t browser_id);

    /** Remove an already registered Cpp function
    * @param [in] function_name the function name
    */
    void RemoveCppFunction(const CefString& function_name);

    /** Remove an already registered Cpp function
    * @param [in] function_name the function name
    * @param [in] browser_id the ID of the Browser object
    */
    void RemoveCppFunction(const CefString& function_name, int64_t browser_id);

    /** Clear all registered functions
    */
    void ClearAllCppFunctions();

private:
    typedef std::map<int64_t/*browser_id*/, CppFunction/* function*/> BrowserFunctionMap;
    typedef std::map<CefString/* function_name*/, BrowserFunctionMap> BrowserRegisteredFunctionMap;

    // Function map
    BrowserRegisteredFunctionMap m_browserCppFunctionMap;
};

}//namespace ui

#endif //DUILIB_BUILD_FOR_CEF
#endif //UI_CEF_CONTROL_CEF_REGISTERED_FUNCTIONS_H_
