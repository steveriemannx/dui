#ifndef UI_CEF_CONTROL_CEF_JS_BRIDGE_H_
#define UI_CEF_CONTROL_CEF_JS_BRIDGE_H_

#include "duilib/CEFControl/internal/CefRegisteredFunctions.h"

#ifdef DUILIB_BUILD_FOR_CEF

namespace ui
{

typedef std::map<int/* js_callback_id*/, std::pair<CefRefPtr<CefV8Context>/* context*/, CefRefPtr<CefV8Value>/* callback*/>> RenderCallbackMap;
typedef std::map<int/* cpp_callback_id*/, CallJsFunctionCallback/* callback*/> BrowserCallbackMap;

class CefJSBridge
{
public:
    CefJSBridge();
    ~CefJSBridge();

// in render process
public:
    /** Execute an already registered C++ method
     * param [in] function_name the name of the function to call
     * param [in] params the JSON-format parameters passed to the function
     * param [in] callback the result callback function invoked after execution completes
     * return Returns true if the execution request was initiated successfully (this does not mean the execution itself succeeded; that depends on the callback); returns false if the registered callback function ID already exists
     */
    bool CallCppFunction(const CefString& function_name, const CefString& params, CefRefPtr<CefV8Value> callback);

    /**
     * Removes the specified callback function by checking the context environment (this method is triggered on page refresh)
     * param [in] frame the current running frame
     */
    void RemoveCallbackFuncWithFrame(CefRefPtr<CefFrame> frame);

    /** Execute the specified callback function by ID
     * param [in] js_callback_id the ID of the callback function
     * param [in] has_error whether there is an error, corresponding to the first argument of the callback function
     * param [in] json_string the JSON string data returned when there is no error, corresponding to the second argument of the callback function
     * return Returns true if the callback function was executed successfully; returns false if the callback function does not exist or its required execution context no longer exists
     */
    bool ExecuteJSCallbackFunc(int js_callback_id, bool has_error, const CefString& json_result);

    /** Register a persistent JS function for C++ to call
     * param [in] function_name the function name, provided as a string for C++ to call directly; the name must not be duplicated
     * param [in] context the execution context of the function
     * param [in] function the function body
     * param [in] replace whether to replace the function if one with the same name already exists; defaults to false
     * return When replace is true, returns true on successful replacement and false on unexpected behavior. When replace is false, returns true on successful registration and false if a function with the same name has already been registered.
     */
    bool RegisterJSFunc(const CefString& function_name, CefRefPtr<CefV8Value> function, bool replace = false);

    /** Unregister a persistent JS function
     * param [in] function_name the function name
     * param [in] frame the frame under which the related functions are to be unregistered
     */
    void UnRegisterJSFunc(const CefString& function_name, CefRefPtr<CefFrame> frame);

    /** Unregister one or more persistent JS functions by execution context
    * param [in] frame the frame to which the current run belongs
    */
    void UnRegisterJSFuncWithFrame(CefRefPtr<CefFrame> frame);

    /** Execute a specific JS function by name
     * param [in] function_name    the function name
     * param [in] json_params    the JSON-format parameters to pass
     * param [in] frame            the frame in which to execute the JS function
     * param [in] cpp_callback_id    the ID of the C++ callback function to invoke after execution completes
     * return Returns true if a JS function was executed successfully; returns false if the function to execute does not exist or its execution context is no longer valid
     */
    bool ExecuteJSFunc(const CefString& function_name, const CefString& json_params, CefRefPtr<CefFrame> frame, int cpp_callback_id);

// in browser process
public:
    /** Execute an already registered JS method
     * param [in] js_function_name the name of the JS function to call
     * param [in] params the JSON-format parameters passed when calling the JS method
     * param [in] frame the frame in which the JS code is called
     * param [in] callback the callback function that receives the data returned by the JS method
     * return Returns true if the command to execute the JS function was initiated successfully; returns false if the same callback id already exists
     */
    bool CallJSFunction(const CefString& js_function_name, const CefString& params, CefRefPtr<CefFrame> frame, CallJsFunctionCallback callback);

    /** Execute the specified callback function by ID
     * param [in] cpp_callback_id the id of the callback function
     * param [in] json_string the returned JSON-format data
     * return Returns true on success, false on failure; the callback may not exist
     */
    bool ExecuteCppCallbackFunc(int cpp_callback_id, const CefString& json_string);

    /** Register a persistent C++ function for the JS side to call
     * param[in] function_name the name of the function to be called from JS
     * param[in] function the function body
     * param[in] replace whether to replace a function body with the same name; defaults to not replacing
     * return When replace is true, returns true on successful registration or replacement and false on unexpected behavior. When replace is false, returns true on successful registration and false if the function name has already been registered
     */
    bool RegisterCppFunc(const CefString& function_name, CppFunction function, CefRefPtr<CefBrowser> browser, bool  replace = false);

    /** Unregister a persistent C++ function
     * param[in] function_name    the name of the function to unregister
     */
    void UnRegisterCppFunc(const CefString& function_name, CefRefPtr<CefBrowser> browser);

    /** Execute an already registered C++ method (called when an execution request from the JS side is received)
     * param [in] function_name the name of the function to execute
     * param [in] params the parameters carried
     * param [in] js_callback_id the callback function ID required to call back to the JS side
     * param [in] browser the browser instance handle
     * return Returns true on success; returns false on failure, possibly because the function name does not exist
     */
    bool ExecuteCppFunc(const CefString& function_name, const CefString& params, int js_callback_id, CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame);

public:
    /** Convert a 64-bit integer value to a CefString
    */
    static CefString Int64ToCefString(int64_t nValue);

private:
    // Index counter for JS-side callback functions
    uint32_t m_jsCallbackId = 0;
     
    // Index counter for C++-side callback functions
    uint32_t m_cppCallbackId = 0;

    // Map of JS-side callback functions
    RenderCallbackMap m_renderCallbackMap;
    
    // Map of C++-side callback functions
    BrowserCallbackMap m_browserCallbackMap;

    // Stores the list of persistent functions registered on the JS side
    RenderRegisteredFunctions m_renderRegisteredFunctions; 
    
    // Stores the list of persistent functions registered on the C++ side
    BrowserRegisteredFunctions m_browserRegisteredFunctions;
};

}

#endif //DUILIB_BUILD_FOR_CEF

#endif //UI_CEF_CONTROL_CEF_JS_BRIDGE_H_








