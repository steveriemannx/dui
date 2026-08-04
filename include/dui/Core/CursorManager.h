#ifndef UI_CORE_CURSOR_MANAGER_H_
#define UI_CORE_CURSOR_MANAGER_H_

#include "dui/dui_defs.h"
#include "dui/Utils/FilePath.h"

namespace ui {

/** The ID of the cursor (a valid value is a non-zero value)
*/
typedef std::size_t CursorID;

class Window;

/** Set the cursor
*/
class DUI_API CursorManager
{
public:
    CursorManager();
    ~CursorManager();

    /** Set the cursor
    * @param [in] cursorType The cursor type
    */
    bool SetCursor(CursorType cursorType);

    /** Set an image as the cursor
    * @param [in] pWindow The associated window
    * @param [in] imagePath The path of the cursor image (with the .cur extension)
    */
    bool SetImageCursor(const Window* pWindow, const FilePath& curImagePath);

    /** Show or hide the cursor
    * @param [in] bShow true means show the cursor, false means hide the cursor
    */
    bool ShowCursor(bool bShow);

    /** Get the cursor ID of the current cursor
    */
    CursorID GetCursorID() const;

    /** Set the cursor according to the ID
    * @param [in] cursorId The ID of the cursor, returned by GetCursorID()
    */
    bool SetCursorByID(CursorID cursorId);

private:
    //Internal implementation
    class TImpl;
    TImpl* m_impl;
};

} // namespace ui

#endif // UI_CORE_CURSOR_MANAGER_H_

