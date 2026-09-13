#ifndef UI_CORE_WINDOW_DROP_TARGET_MACOS_H_
#define UI_CORE_WINDOW_DROP_TARGET_MACOS_H_

#include "dui/Core/ControlDropTarget.h"
#include "dui/Core/NativeWindow_MacOS.h"

#if defined (DUI_BUILD_FOR_MACOS)

namespace ui 
{
/** Drag and drop support for the window
*/
class DUI_API WindowDropTarget
{
public:
    explicit WindowDropTarget(NativeWindow* pNativeWindow);
    WindowDropTarget(const WindowDropTarget& r) = delete;
    WindowDropTarget& operator=(const WindowDropTarget& r) = delete;
    virtual ~WindowDropTarget();

public:
    // DROP_BEGIN
    void OnDropBegin();

    /** DROP_POSITION
    *@param [in] pt Client area coordinates
    */
    void OnDropPosition(const UiPoint& pt);

    /** DROP_TEXT
    *@param [in] utf8Text Text content; each call provides one line of text
    */
    void OnDropText(const std::string& utf8Text);

    /** DROP_FILE
    *@param [in] utf8Source Drag and drop source
    *@param [in] utf8File File path; each call provides one file
    */
    void OnDropFile(const std::string& utf8Source, const std::string& utf8File);
    
    // DROP_COMPLETE
    void OnDropComplete();

protected: 
    /** Find the control interface corresponding to a point coordinate
    * @param [in] clientPt Client area coordinate point
    */
    ControlPtrT<ControlDropTarget_Wayland> GetControlDropTarget(const UiPoint& clientPt) const;

    /** Clean up the state when the drag operation is complete
    */
    void ClearDropStatus();

private:
    /** The control interface currently in the Hover state
    */
    ControlPtrT<ControlDropTarget_Wayland> m_pHoverDropTarget;

    /** The associated Native window implementation
    */
    NativeWindow* m_pNativeWindow;

    /** Client area coordinates of the drag operation
    */
    UiPoint m_dropPt;

    /** Text data
    */
    std::vector<std::string> m_textList;

    /** File data
    */
    std::vector<std::string> m_fileList;

    /** The source of the files
    */
    std::string m_fileSource;
};

} // namespace ui

#endif //defined (DUI_BUILD_FOR_MACOS)

#endif // UI_CORE_WINDOW_DROP_TARGET_MACOS_H_
