#ifndef EXAMPLES_DRAG_FROM_H_
#define EXAMPLES_DRAG_FROM_H_

// dui
#include "dui/dui.h"

/** The small window displayed while a tab is being dragged out
*/
class DragForm: public ui::DragWindow
{
    typedef ui::DragWindow BaseClass;
public:
    DragForm();
    virtual ~DragForm() override;

    /**  Called when the window is created; implemented by subclasses to get the window skin folder
    * @return subclasses need to implement and return the window skin folder
    */
    virtual DString GetSkinFolder() override;

    /**  Called when the window is created; implemented by subclasses to get the window skin XML description file
    * @return subclasses need to implement and return the window skin XML description file
    *         The returned content can be the XML file content (a string starting with the character '<'),
    *         or a file path (a string not starting with the character '<'); the file must be findable in the GetSkinFolder() path
    */
    virtual DString GetSkinFile() override;
};

#endif //EXAMPLES_DRAG_FROM_H_
