#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// dui
#include "dui/dui.h"

/** Main window implementation of the application
*/
class MainForm : public ui::WindowImplBase
{
public:
    MainForm();
    virtual ~MainForm() override;

    /**  Called when the window is created; implemented by subclasses to get the window skin directory
    * @return The subclass must implement and return the window skin directory
    */
    virtual DString GetSkinFolder() override;

    /**  Called when the window is created; implemented by subclasses to get the window skin XML description file
    * @return The subclass must implement and return the window skin XML description file
    *         The returned content can be the XML file content (a string starting with the character '<'),
    *         or a file path (a string not starting with the character '<'); the file must be found under the GetSkinFolder() path
    */
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    /** Show the menu
    * @param [in] point The display position, in screen coordinates
    */
    void ShowPopupMenu(const ui::UiPoint& point);
};

#endif //EXAMPLES_MAIN_FORM_H_

