#ifndef EXAMPLES_ANIMATION_FORM_H_
#define EXAMPLES_ANIMATION_FORM_H_

// duilib
#include "duilib/duilib.h"

class AnimationForm : public ui::WindowImplBase
{
public:
    AnimationForm();
    virtual ~AnimationForm() override;

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
    //Animation playback related event handling
    void OnAnimationEvents(ui::EventType eventType, const ui::ImageAnimationStatus& status);

private:
    //Whether it is playing
    bool m_bImagePlaying;
};

#endif //EXAMPLES_ANIMATION_FORM_H_
