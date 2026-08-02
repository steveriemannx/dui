#ifndef EXAMPLES_ABOUT_FORM_H_
#define EXAMPLES_ABOUT_FORM_H_

// duilib
#include "duilib/duilib.h"

class AboutForm : public ui::WindowImplBase
{
public:
    AboutForm();
    virtual ~AboutForm() override;

    /** Resource-related interfaces
     * The GetSkinFolder interface sets the skin resource path of the window to be drawn
     * The GetSkinFile interface sets the xml description file of the window to be drawn
     */
    virtual DString GetSkinFolder() override;
    virtual DString GetSkinFile() override;

    /** Called after the window is created, for subclasses to do some initialization work
    */
    virtual void OnInitWindow() override;

private:
    /** Build the UI purely in code (corresponds to the about.xml layout)
    */
    void BuildUI();
};

#endif //EXAMPLES_ABOUT_FORM_H_
