#ifndef UI_CORE_WINDOW_DROP_TARGET_SDL_H_
#define UI_CORE_WINDOW_DROP_TARGET_SDL_H_

#include "duilib/Core/ControlDropTarget.h"

#if defined (DUILIB_BUILD_FOR_SDL) || defined (DUILIB_BUILD_FOR_WAYLAND)

namespace ui 
{
class NativeWindow_SDL;

/** Drag and drop support for the window
*/
class DUILIB_API WindowDropTarget
{
public:
    explicit WindowDropTarget(NativeWindow_SDL* pNativeWindow);
    WindowDropTarget(const WindowDropTarget& r) = delete;
    WindowDropTarget& operator=(const WindowDropTarget& r) = delete;
    virtual ~WindowDropTarget();

public:
    // SDL_EVENT_DROP_BEGIN
    void OnDropBegin();

    /** SDL_EVENT_DROP_POSITION
    *@param [in] pt Client area coordinates
    */
    void OnDropPosition(const UiPoint& pt);

    /** SDL_EVENT_DROP_TEXT
    *@param [in] utf8Text Text content; each call provides one line of text
    */
    void OnDropText(const DStringA& utf8Text);

    /** SDL_EVENT_DROP_FILE
    *@param [in] utf8Source Drag and drop source
    *@param [in] utf8File File path; each call provides one file
    */
    void OnDropFile(const DStringA& utf8Source, const DStringA& utf8File);
    
    // SDL_EVENT_DROP_COMPLETE
    void OnDropComplete();

protected: 
    /** Find the control interface corresponding to a point coordinate
    * @param [in] clientPt Client area coordinate point
    */
    ControlPtrT<ControlDropTarget_SDL> GetControlDropTarget(const UiPoint& clientPt) const;

    /** Clean up the state when the drag operation is complete
    */
    void ClearDropStatus();

private:
    /** The control interface currently in the Hover state
    */
    ControlPtrT<ControlDropTarget_SDL> m_pHoverDropTarget;

    /** The associated Native window implementation
    */
    NativeWindow_SDL* m_pNativeWindow;

    /** Client area coordinates of the drag operation
    */
    UiPoint m_dropPt;

    /** Text data
    */
    std::vector<DString> m_textList;

    /** File data
    */
    std::vector<DString> m_fileList;

    /** The source of the files
    */
    DString m_fileSource;
};

} // namespace ui

#endif //defined (DUILIB_BUILD_FOR_SDL)

#endif // UI_CORE_WINDOW_DROP_TARGET_SDL_H_
