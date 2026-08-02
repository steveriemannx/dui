#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// duilib
#include "duilib/duilib.h"

class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm();
    virtual ~MainForm() override;

    /** Resource-related interfaces
     * GetSkinFolder sets the skin resource path of the window to be drawn
     * GetSkinFile sets the XML description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;
};

#endif //EXAMPLES_MAIN_FORM_H_
