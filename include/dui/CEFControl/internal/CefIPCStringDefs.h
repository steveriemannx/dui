/** @brief Defines the constants used for communication between the Render process and the Browser process
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author Redrain
  * @date 2016/7/19
*/
#ifndef UI_CEF_CONTROL_IPC_STRING_DEFINE_H_
#define UI_CEF_CONTROL_IPC_STRING_DEFINE_H_

#include "dui/dui_config.h"

#ifdef DUI_BUILD_FOR_CEF

namespace ui
{
    static const char kFocusedNodeChangedMessage[]      = "FocusedNodeChanged";     // The element that gained focus in the web page has changed
    static const char kExecuteCppCallbackMessage[]      = "ExecuteCppCallback";     // Execute the C++ message callback function
    static const char kCallCppFunctionMessage[]         = "CallCppFunction";        // Notification that the web calls a C++ interface
    static const char kExecuteJsCallbackMessage[]       = "ExecuteJsCallback";      // Notification that the web calls a C++ interface
    static const char kCallJsFunctionMessage[]          = "CallJsFunction";         // Notification that C++ calls JavaScript
}

#endif //DUI_BUILD_FOR_CEF

#endif //UI_CEF_CONTROL_IPC_STRING_DEFINE_H_
