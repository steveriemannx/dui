#ifndef EXAMPLES_DRAGDROP_MANAGER_H_
#define EXAMPLES_DRAGDROP_MANAGER_H_

#include "browser/BrowserBox.h"

class DragForm;

/** Multi-tab browser manager class, used to uniformly manage the multi-tab drag feature
*/
class DragDropManager : public virtual ui::SupportWeakCallback
{
public:
    DragDropManager();
    DragDropManager(const DragDropManager&) = delete;
    DragDropManager& operator=(const DragDropManager&) = delete;

    /** Singleton object
    */
    static DragDropManager* GetInstance();

private:
    ~DragDropManager();

public:
    /** Whether a browser box is currently being dragged
    */
    bool IsDragingBorwserBox() const;

    /** Start the operation of dragging a browser box
    * @param [in] browserBox the browser box
    * @param [in] spIBitmap the bitmap generated from the browser box
    * @param [in] ptOffset the offset of the drag image relative to the mouse
    * @return bool true success, false failure
    */
    bool StartDragBorwserBox(BrowserBox* browserBox, std::shared_ptr<ui::IBitmap> spIBitmap, const ui::UiPoint& ptOffset);

    /** End the operation of dragging a browser box
    * @param [in] bSuccess true means the operation succeeded, false means the drag operation was cancelled
    */
    void EndDragBorwserBox(bool bSuccess);

    /** Update the position of the drag window
    */
    void UpdateDragFormPos();

private:
    /** The browser box currently being dragged
    */
    BrowserBox* m_pDragingBox;

    /** The small window during the drag
    */
    DragForm* m_pDragForm;
};

#endif //EXAMPLES_DRAGDROP_MANAGER_H_
