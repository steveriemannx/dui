#include "AnimationForm.h"

AnimationForm::AnimationForm():
    m_bImagePlaying(false)
{
}

AnimationForm::~AnimationForm()
{
}

DString AnimationForm::GetSkinFolder()
{
    return _T("controls");
}

DString AnimationForm::GetSkinFile()
{
    return _T("animation.xml");
}

void AnimationForm::OnInitWindow()
{
    //Bind animation playback related events
    ui::Control* pControl = FindControl(_T("animation_test"));
    if (pControl != nullptr) {
        pControl->AttachImageAnimationStart([this](const ui::EventArgs& arg) {
            if (arg.wParam != 0) {
                const ui::ImageAnimationStatus& status = *((ui::ImageAnimationStatus*)arg.wParam);
                OnAnimationEvents(ui::EventType::kEventImageAnimationStart, status);
            }
            return true;
            });
        pControl->AttachImageAnimationStop([this](const ui::EventArgs& arg) {
            if (arg.wParam != 0) {
                const ui::ImageAnimationStatus& status = *((ui::ImageAnimationStatus*)arg.wParam);
                OnAnimationEvents(ui::EventType::kEventImageAnimationStop, status);
            }
            return true;
            });
        pControl->AttachImageAnimationPlayFrame([this](const ui::EventArgs& arg) {
            if (arg.wParam != 0) {
                const ui::ImageAnimationStatus& status = *((ui::ImageAnimationStatus*)arg.wParam);
                OnAnimationEvents(ui::EventType::kEventImageAnimationPlayFrame, status);
            }
            return true;
            });
    }

    m_bImagePlaying = false;
    ui::Button* pButton = dynamic_cast<ui::Button*>(FindControl(_T("animation_btn")));
    if (pButton != nullptr) {
        pButton->AttachClick([this](const ui::EventArgs& arg) {
            ui::Control* pControl = FindControl(_T("animation_test"));
            if (pControl != nullptr) {
                if (m_bImagePlaying) {
                    //Pause
                    pControl->StopImageAnimation();
                }
                else {
                    //Play
                    pControl->StartImageAnimation();
                }
            }
            return true;
            });
    }

    pButton = dynamic_cast<ui::Button*>(FindControl(_T("goto_frame_btn")));
    if (pButton != nullptr) {
        pButton->AttachClick([this](const ui::EventArgs& arg) {
            ui::Control* pControl = FindControl(_T("animation_test"));
            if (pControl != nullptr) {
                if (m_bImagePlaying) {
                    //Pause
                    pControl->StopImageAnimation();
                }
                int32_t nFrameIndex = 0;
                ui::RichEdit* pRichEdit = dynamic_cast<ui::RichEdit*>(FindControl(_T("goto_frame_number")));
                if (pRichEdit != nullptr) {
                    nFrameIndex = (int32_t)pRichEdit->GetTextNumber();
                }
                pControl->SetImageAnimationFrame(_T("bk_animation_test"), nFrameIndex);
            }
            return true;
            });
    }
}

void AnimationForm::OnAnimationEvents(ui::EventType eventType, const ui::ImageAnimationStatus& status)
{
    if (eventType == ui::EventType::kEventImageAnimationStart) {
        //Playback started
        m_bImagePlaying = true;
        ui::Button* pButton = dynamic_cast<ui::Button*>(FindControl(_T("animation_btn")));
        if (pButton != nullptr) {
            pButton->SetText(_T("Pause"));
        }
        ui::RichEdit* pRichEdit = dynamic_cast<ui::RichEdit*>(FindControl(_T("goto_frame_number")));
        if (pRichEdit != nullptr) {
            pRichEdit->SetMinNumber(0);
            pRichEdit->SetMaxNumber((int32_t)status.m_nFrameCount - 1);
        }
    }
    else if (eventType == ui::EventType::kEventImageAnimationStop) {
        //Playback stopped
        m_bImagePlaying = false;
        ui::Button* pButton = dynamic_cast<ui::Button*>(FindControl(_T("animation_btn")));
        if (pButton != nullptr) {
            pButton->SetText(_T("Play"));
        }
    }
    else if (eventType == ui::EventType::kEventImageAnimationPlayFrame) {
        //Playing
        m_bImagePlaying = true;
        ui::Button* pButton = dynamic_cast<ui::Button*>(FindControl(_T("animation_btn")));
        if (pButton != nullptr) {
            pButton->SetText(_T("Pause"));
        }

        ui::Label* pLabel = dynamic_cast<ui::Label*>(FindControl(_T("animation_frame")));
        if (pLabel != nullptr) {
            DString statusText = ui::StringUtil::Printf(_T("[%d/%d]"), status.m_nFrameIndex, status.m_nFrameCount);
            pLabel->SetText(statusText);
        }
    }
}
