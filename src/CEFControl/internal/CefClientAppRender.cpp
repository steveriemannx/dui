#include "dui/CEFControl/internal/CefClientApp.h"

#ifdef DUI_BUILD_FOR_CEF

#include "dui/CEFControl/internal/CefJsHandler.h"
#include "dui/CEFControl/internal/CefIPCStringDefs.h"
#include "dui/CEFControl/internal/CefJSBridge.h"
#include "dui/Utils/StringUtil.h"

namespace ui
{
//////////////////////////////////////////////////////////////////////////////////////////

void CefClientApp::OnWebKitInitialized() 
{
    /**
     * JavaScript extension code: this defines a NimCefWebFunction object providing a call method for the Web side to trigger the native CefV8Handler processing code
     * param[in] functionName    The name of the C++ method to be called
     * param[in] params            The parameters passed when calling the method; on the front end it is specified as an Object, but it is converted to a string when passed to Native
     * param[in] callback        The callback function after executing the method
     * Front-end calling example
     * NimCefWebHelper.call('showMessage', { message: 'Hello C++' }, (arguments) => {
     *    console.log(arguments)
     * })
     */
    std::string extensionCode = R"(
        var NimCefWebInstance = {};
        (() => {
            NimCefWebInstance.call = (functionName, arg1, arg2) => {
                if (typeof arg1 === 'function') {
                    native function call(functionName, arg1);
                    return call(functionName, arg1);
                } else {
                    const jsonString = JSON.stringify(arg1);
                    native function call(functionName, jsonString, arg2);
                    return call(functionName, jsonString, arg2);
                }
            };
            NimCefWebInstance.register = (functionName, callback) => {
                native function register(functionName, callback);
                return register(functionName, callback);
            };
        })();
    )";
    CefRefPtr<CefJSHandler> handler = new CefJSHandler();

    if (!m_renderJsBridge.get()) {
        m_renderJsBridge.reset(new CefJSBridge);
    }
    handler->AttachJSBridge(m_renderJsBridge);
     CefRegisterExtension("v8/extern", extensionCode, handler);
}

void CefClientApp::OnBrowserCreated(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefDictionaryValue> /*extra_info*/)
{
    if (!m_renderJsBridge.get()) {
        m_renderJsBridge.reset(new CefJSBridge);
    }
}

void CefClientApp::OnBrowserDestroyed(CefRefPtr<CefBrowser> /*browser*/) 
{
}

CefRefPtr<CefLoadHandler> CefClientApp::GetLoadHandler()
{
    return nullptr;
}

void CefClientApp::OnContextCreated(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> /*frame*/, CefRefPtr<CefV8Context> /*context*/)
{

}

void CefClientApp::OnContextReleased(CefRefPtr<CefBrowser> /*browser*/, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> /*context*/)
{
    if (m_renderJsBridge != nullptr) {
        m_renderJsBridge->RemoveCallbackFuncWithFrame(frame);
        m_renderJsBridge->UnRegisterJSFuncWithFrame(frame);
    }
}

void CefClientApp::OnUncaughtException(CefRefPtr<CefBrowser> /*browser*/,
                                       CefRefPtr<CefFrame> /*frame*/,
                                       CefRefPtr<CefV8Context> /*context*/,
                                       CefRefPtr<CefV8Exception> /*exception*/,
                                       CefRefPtr<CefV8StackTrace> /*stackTrace*/)
{
}

void CefClientApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> /*browser*/,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefDOMNode> node) 
{
    if (frame == nullptr){
        return;
    }
    CefDOMNode::Type type = (node != nullptr) ? node->GetType() : CefDOMNode::Type::DOM_NODE_TYPE_UNSUPPORTED;
    bool bText = (node != nullptr) ? node->IsText() : false;
    bool bEditable = (node != nullptr) ? node->IsEditable() : false;
    CefRect nodeRect = (node != nullptr) ? node->GetElementBounds() : CefRect();

    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(kFocusedNodeChangedMessage);
    if (message != nullptr) {
        message->GetArgumentList()->SetInt(0, (int)type);
        message->GetArgumentList()->SetBool(1, bText);
        message->GetArgumentList()->SetBool(2, bEditable);

        message->GetArgumentList()->SetInt(3, nodeRect.x);
        message->GetArgumentList()->SetInt(4, nodeRect.y);
        message->GetArgumentList()->SetInt(5, nodeRect.width);
        message->GetArgumentList()->SetInt(6, nodeRect.height);

        frame->SendProcessMessage(PID_BROWSER, message);
    }
}

bool CefClientApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                            CefRefPtr<CefFrame> /*frame*/,
                                            CefProcessId source_process,
                                            CefRefPtr<CefProcessMessage> message)
{
    (void)source_process;
    ASSERT(source_process == PID_BROWSER);
    ASSERT(message != nullptr);
    if (message == nullptr) {
        return false;
    }
    ASSERT(browser != nullptr);
    if (browser == nullptr) {
        return false;
    }

    // Received the message reply from the browser
    const CefString& message_name = message->GetName();
    if (message_name == kExecuteJsCallbackMessage) {
        int callback_id = message->GetArgumentList()->GetInt(0);
        bool has_error = message->GetArgumentList()->GetBool(1);
        CefString json_string = message->GetArgumentList()->GetString(2);

        // Pass the received parameters through the manager to the callback function passed at call time
        m_renderJsBridge->ExecuteJSCallbackFunc(callback_id, has_error, json_string);
    }
    else if (message_name == kCallJsFunctionMessage) {
        CefString function_name = message->GetArgumentList()->GetString(0);
        CefString json_string = message->GetArgumentList()->GetString(1);
        int cpp_callback_id = message->GetArgumentList()->GetInt(2);
        CefString frame_id_string = message->GetArgumentList()->GetString(3);

        CefRefPtr<CefFrame> jsFrame;
        if (frame_id_string.empty()) {
            jsFrame = browser->GetMainFrame();
        }
        else {
#if CEF_VERSION_MAJOR <= 109
            //CEF 109 version
            jsFrame = browser->GetFrame(StringUtil::StringToInt64(frame_id_string.c_str()));
#else
            //CEF newer versions
            jsFrame = browser->GetFrameByIdentifier(frame_id_string);
#endif
        }

        // Execute an already registered JS method through C++
        // If frame_id is less than 0, the browser in the browser process may be invalid, so to avoid errors, a top-level frame is obtained here to execute the code
        m_renderJsBridge->ExecuteJSFunc(function_name, json_string, jsFrame, cpp_callback_id);
    }

    return false;
}
} //namespace ui

#endif //DUI_BUILD_FOR_CEF
