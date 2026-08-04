#ifndef UI_CORE_CONTROL_DROP_TARGET_IMPL_WINDOWS_H_
#define UI_CORE_CONTROL_DROP_TARGET_IMPL_WINDOWS_H_

#include "dui/Core/ControlDropTarget.h"

#ifdef DUI_BUILD_FOR_WIN

namespace ui 
{
/** The implementation of the drag and drop support interface of the control (Windows)
*/
class ControlDropTargetImpl_Windows : public ControlDropTarget_Windows
{
public:
    explicit ControlDropTargetImpl_Windows(Control* pControl);
    virtual ~ControlDropTargetImpl_Windows();

public:
    /** IDropTarget::DragEnter
    */
    virtual int32_t DragEnter(void* pDataObj, uint32_t grfKeyState, const UiPoint& pt, uint32_t* pdwEffect) override;

    /** IDropTarget::DragOver
    */
    virtual int32_t DragOver(uint32_t grfKeyState, const UiPoint& pt, uint32_t* pdwEffect) override;

    /** IDropTarget::DragLeave
    */
    virtual int32_t DragLeave(void) override;

    /** IDropTarget::Drop
    */
    virtual int32_t Drop(void* pDataObj, uint32_t grfKeyState, const UiPoint& pt, uint32_t* pdwEffect) override;

public:
    /** Parse the content of pDataObj and separate the text and file path data
    */
    static void ParseWindowsDataObject(void* pDataObj, std::vector<DString>& textList, std::vector<DString>& fileList);

private:
    /** Clear the drag and drop state
    */
    void ClearDragStatus();

private:
    /** The associated control
    */
    ControlPtr m_pControl;

    /** The associated IDataObject object
    */
    void* m_pDataObj;

    /** The associated text data
    */
    std::vector<DString> m_textList;

    /** The associated file data
    */
    std::vector<DString> m_fileList;
};

} // namespace ui

#endif //DUI_BUILD_FOR_WIN

#endif // UI_CORE_CONTROL_DROP_TARGET_IMPL_WINDOWS_H_
