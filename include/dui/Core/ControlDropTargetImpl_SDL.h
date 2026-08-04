#ifndef UI_CORE_CONTROL_DROP_TARGET_IMPL_SDL_H_
#define UI_CORE_CONTROL_DROP_TARGET_IMPL_SDL_H_

#include "dui/Core/ControlDropTarget.h"

#ifdef DUI_BUILD_FOR_SDL

namespace ui 
{
/** The implementation of the drag and drop support interface of the control (SDL)
*/
class ControlDropTargetImpl_SDL : public ControlDropTarget_SDL
{
public:
    explicit ControlDropTargetImpl_SDL(Control* pControl);
    virtual ~ControlDropTargetImpl_SDL();

public:
    /** SDL_EVENT_DROP_BEGIN
    * @param [in] pt The client area coordinates
    * @return 0 on success, non-zero on failure
    */
    virtual int32_t OnDropBegin(const UiPoint& pt) override;

    /** SDL_EVENT_DROP_POSITION
    *@param [in] pt The client area coordinates
    */
    virtual void OnDropPosition(const UiPoint& pt) override;

    /** SDL_EVENT_DROP_COMPLETE + SDL_EVENT_DROP_TEXT; there will be no more OnDropLeave afterwards
    *@param [in] textList The text content; each element in the list represents one line of text
    */
    virtual void OnDropTexts(const std::vector<DString>& textList, const UiPoint& pt) override;

    /** SDL_EVENT_DROP_COMPLETE + SDL_EVENT_DROP_FILE; there will be no more OnDropLeave afterwards
    *@param [in] source The drag and drop source
    *@param [in] fileList The file paths; each element in the list represents one file
    */
    virtual void OnDropFiles(const DString& source, const std::vector<DString>& fileList, const UiPoint& pt) override;

    /** SDL_EVENT_DROP_COMPLETE or other messages causing the leave
    */
    virtual void OnDropLeave() override;

private:
    /** The associated control
    */
    ControlPtr m_pControl;
};

} // namespace ui

#endif //DUI_BUILD_FOR_SDL

#endif // UI_CORE_CONTROL_DROP_TARGET_IMPL_SDL_H_
