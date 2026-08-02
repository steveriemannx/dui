#include "duilib/CEFControl/internal/CefJsHandler.h"

#ifdef DUILIB_BUILD_FOR_CEF

#include "duilib/CEFControl/internal/CefIPCStringDefs.h"
#include "duilib/CEFControl/internal/CefJSBridge.h"
#include "duilib/Utils/StringUtil.h"

namespace ui
{

bool CefJSHandler::Execute(const CefString& name, CefRefPtr<CefV8Value> /*object*/, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& /*retval*/, CefString& exception)
{
    // When the "NimCefWebFunction" function is called in the Web, execution reaches here, and the parameters are saved and forwarded to the Browser process
    // The CefBrowserHandler class in the Browser process receives this message by handling the kJsCallbackMessage in the OnProcessMessageReceived interface

    if (arguments.size() < 2) {
        exception = "Invalid arguments.";
        return false;
    }

    CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
    ASSERT(context != nullptr);
    if (context == nullptr) {
        return false;
    }

    CefRefPtr<CefFrame> frame = context->GetFrame();
    ASSERT(frame != nullptr);
    if (frame == nullptr) {
        return false;
    }

    CefRefPtr<CefBrowser> browser = context->GetBrowser();
    ASSERT(browser != nullptr);
    if (browser == nullptr) {
        return false;
    }

    int64_t browserId = browser->GetIdentifier();
#if CEF_VERSION_MAJOR <= 109
    //CEF 109 version
    CefString frameId = CefJSBridge::Int64ToCefString(frame->GetIdentifier());
#else
    //CEF newer versions
    CefString frameId = frame->GetIdentifier();
#endif
    (void)browserId;
    (void)frameId;

    if (name == "call") {
        // Calls without an argument list are allowed; the second argument is the callback
        // If an argument list is passed, the callback is the third argument
        CefString function_name = arguments[0]->GetStringValue();
        CefString params = "{}";
        CefRefPtr<CefV8Value> callback;
        if (arguments[0]->IsString() && arguments[1]->IsFunction()) {
            callback = arguments[1];
        }
        else if (arguments[0]->IsString() && arguments[1]->IsString() && arguments[2]->IsFunction()) {
            params = arguments[1]->GetStringValue();
            callback = arguments[2];
        }
        else {
            exception = "Invalid arguments.";
            return false;
        }

        // Execute the C++ method
        if (!m_jsBridge->CallCppFunction(function_name, params, callback)) {
            exception = ui::StringUtil::Printf("Failed to call function %s.", function_name.c_str()).c_str();
            return false;
        }
        return true;
    }
    else if (name == "register") {
        if (arguments[0]->IsString() && arguments[1]->IsFunction())
        {
            std::string function_name = arguments[0]->GetStringValue();
            CefRefPtr<CefV8Value> callback = arguments[1];
            if (!m_jsBridge->RegisterJSFunc(function_name, callback)) {
                exception = "Failed to register function.";
                return false;
            }
            return true;
        }
        else {
            exception = "Invalid arguments.";
            return false;
        }
    }

    return false;
}

} //namespace ui

#endif //DUILIB_BUILD_FOR_CEF
