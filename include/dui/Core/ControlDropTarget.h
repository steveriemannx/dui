#ifndef UI_CORE_CONTROL_DROP_TARGET_H_
#define UI_CORE_CONTROL_DROP_TARGET_H_

#include "dui/Core/UiTypes.h"
#include "dui/Core/ControlPtrT.h"
#include "dui/Core/Callback.h"
#include <vector>

namespace ui 
{
class Control;

/** The drag and drop support interface of the control (Windows)
*/
class DUI_API ControlDropTarget_Windows : public virtual SupportWeakCallback
{
public:
    /** IDropTarget::DragEnter
    */
    virtual int32_t DragEnter(void* pDataObj, uint32_t grfKeyState, const UiPoint& pt, uint32_t* pdwEffect) = 0;

    /** IDropTarget::DragOver
    */
    virtual int32_t DragOver(uint32_t grfKeyState, const UiPoint& pt, uint32_t* pdwEffect) = 0;

    /** IDropTarget::DragLeave
    */
    virtual int32_t DragLeave(void) = 0;

    /** IDropTarget::Drop
    */
    virtual int32_t Drop(void* pDataObj, uint32_t grfKeyState, const UiPoint& pt, uint32_t* pdwEffect) = 0;
};

/** The drag and drop support interface of the control (SDL)
*/
class DUI_API ControlDropTarget_SDL : public virtual SupportWeakCallback
{
public:
    /** SDL_EVENT_DROP_BEGIN
    * @param [in] pt The client area coordinate
    * @return Returns 0 on success, non-zero on failure
    */
    virtual int32_t OnDropBegin(const UiPoint& pt) = 0;

    /** SDL_EVENT_DROP_POSITION
    *@param [in] pt The client area coordinate
    */
    virtual void OnDropPosition(const UiPoint& pt) = 0;

    /** SDL_EVENT_DROP_COMPLETE + SDL_EVENT_DROP_TEXT, OnDropLeave will not be called afterwards
    *@param [in] textList The text content, each element in the container represents one line of text
    */
    virtual void OnDropTexts(const std::vector<DString>& textList, const UiPoint& pt) = 0;

    /** SDL_EVENT_DROP_COMPLETE + SDL_EVENT_DROP_FILE, OnDropLeave will not be called afterwards
    *@param [in] source The drag and drop source
    *@param [in] fileList The file paths, each element in the container represents one file
    */
    virtual void OnDropFiles(const DString& source, const std::vector<DString>& fileList, const UiPoint& pt) = 0;

    /** SDL_EVENT_DROP_COMPLETE or other messages that cause a leave
    */
    virtual void OnDropLeave() = 0;
};

} // namespace ui

#endif // UI_CORE_CONTROL_DROP_TARGET_H_
