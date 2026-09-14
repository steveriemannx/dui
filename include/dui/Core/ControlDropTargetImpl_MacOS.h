#ifndef UI_CORE_CONTROL_DROP_TARGET_IMPL_MACOS_H_
#define UI_CORE_CONTROL_DROP_TARGET_IMPL_MACOS_H_

#include "dui/Core/ControlDropTarget.h"

#if defined(DUI_BUILD_FOR_MACOS)

namespace ui 
{
/** The implementation of the drag and drop support interface of the control (macOS native)
*/
class ControlDropTargetImpl_MacOS : public ControlDropTarget_Wayland
{
public:
    explicit ControlDropTargetImpl_MacOS(Control* pControl);
    virtual ~ControlDropTargetImpl_MacOS();

public:
    /** DROP_BEGIN
    * @param [in] pt The client area coordinates
    * @return 0 on success, non-zero on failure
    */
    virtual int32_t OnDropBegin(const UiPoint& pt) override;

    /** DROP_POSITION
    *@param [in] pt The client area coordinates
    */
    virtual void OnDropPosition(const UiPoint& pt) override;

    /** DROP_COMPLETE + DROP_TEXT; there will be no more OnDropLeave afterwards
    *@param [in] textList The text content; each element in the list represents one line of text
    */
    virtual void OnDropTexts(const std::vector<std::string>& textList, const UiPoint& pt) override;

    /** DROP_COMPLETE + DROP_FILE; there will be no more OnDropLeave afterwards
    *@param [in] source The drag and drop source
    *@param [in] fileList The file paths; each element in the list represents one file
    */
    virtual void OnDropFiles(const std::string& source, const std::vector<std::string>& fileList, const UiPoint& pt) override;

    /** DROP_COMPLETE or other messages causing the leave
    */
    virtual void OnDropLeave() override;

private:
    /** The associated control
    */
    ControlPtr m_pControl;
};

} // namespace ui

#endif //defined (DUI_BUILD_FOR_MACOS)

#endif // UI_CORE_CONTROL_DROP_TARGET_IMPL_MACOS_H_
