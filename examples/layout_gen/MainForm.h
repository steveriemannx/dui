#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// duilib
#include "duilib/duilib.h"

class MainForm : public ui::WindowImplBase
{
public:
    MainForm();
    virtual ~MainForm() override;

    /** Resource-related interfaces
     * The GetSkinFolder interface sets the skin resource path for the window to be drawn
     * The GetSkinFile interface sets the xml description file for the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

    /** Called after the window has been closed, for subclasses to do some cleanup work
    */
    virtual void OnCloseWindow() override;
};

#endif //EXAMPLES_MAIN_FORM_H_
