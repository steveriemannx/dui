#ifndef UI_CORE_DRAG_WINDOW_H_
#define UI_CORE_DRAG_WINDOW_H_

#include "dui/Core/Control.h"
#include "dui/Core/Box.h"
#include "dui/Utils/WinImplBase.h"
#include "dui/Render/IRender.h"

namespace ui
{
/** The window displayed during dragging
*/
class DUI_API DragWindow : public WindowImplBase
{
public:
    DragWindow();

protected:
    //The destructor must not be called externally, because there is a delete this operation in the OnFinalMessage function
    virtual ~DragWindow() override;

public:
    /**   Called when the window is created, implemented by the subclass to obtain the window skin folder
    * @return The subclass needs to implement and return the window skin folder
    */
    virtual DString GetSkinFolder() override;

    /**   Called when the window is created, implemented by the subclass to obtain the window skin XML description file
    * @return The subclass needs to implement and return the window skin XML description file
    *         The returned content can be the XML file content (a string starting with the character '<'),
    *         or a file path (a string not starting with the character '<'); the file must be findable under the GetSkinFolder() path
    */
    virtual DString GetSkinFile() override;

    /** Called when the window receives the WM_NCDESTROY message
    * @param [in] hWnd The handle of the window to be destroyed
    */
    virtual void OnFinalMessage() override;

public:
    /** Set the image to be displayed
    * @param [in] pBitmap The interface of the image resource
    */
    virtual void SetDragImage(const std::shared_ptr<IBitmap>& pBitmap);

    /** Adjust the window position to follow the mouse
    */
    virtual void AdjustPos();

public:
    /** Increase the reference count
    */
    void AddRef();

    /** Decrease the reference count
    */
    void Release();

private:
    /** The reference count
    */
    int32_t m_nRefCount;
};
}

#endif // UI_CORE_DRAG_WINDOW_H_
