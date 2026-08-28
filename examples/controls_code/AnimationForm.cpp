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
    return _T("");
}

DString AnimationForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void AnimationForm::BuildUI()
{
    // Corresponds to the animation.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{_T("name"), _T("window_caption_bar")}, {_T("width"), _T("stretch")}, {_T("height"), _T("36")}});
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pSpacer = ui::Create<ui::Control>(this, {{_T("mouse_enabled"), _T("false")}});
    pCaption->AddItem(pSpacer);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pFullscreenBtn->SetClass(_T("btn_wnd_fullscreen_11"));
    pFullscreenBtn->SetName(_T("fullscreenbtn"));
    pFullscreenBtn->SetToolTipText(_T("Fullscreen, press ESC to exit fullscreen"));
    pCaption->AddItem(pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaption->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,2,0,2")}});
    pCaption->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("stretch")}});
    pMaxBtn->SetClass(_T("btn_wnd_max_11"));
    pMaxBtn->SetName(_T("maxbtn"));
    pMaxBtn->SetToolTipText(_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("32")}, {_T("width"), _T("stretch")}});
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{_T("height"), _T("stretch")}, {_T("width"), _T("40")}, {_T("margin"), _T("0,0,0,2")}});
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetToolTipText(_T("Close"));
    pCaption->AddItem(pCloseBtn);

    // Work area
    auto* pContent = ui::Create<ui::VBox>(this, {{_T("child_halign"), _T("center")}, {_T("child_valign"), _T("center")}});
    pRoot->AddItem(pContent);

    auto* pControlRow = ui::Create<ui::HBox>(this, {{_T("height"), _T("40")}, {_T("margin"), _T("8,0,0,0")}});
    pContent->AddItem(pControlRow);

    auto* pAnimBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("96")}, {_T("height"), _T("30")}, {_T("border_round"), _T("3,3")}, {_T("margin"), _T("8,0,0,0")}, {_T("valign"), _T("center")}});
    pAnimBtn->SetClass(_T("btn_global_color_gray"));
    pAnimBtn->SetName(_T("animation_btn"));
    pAnimBtn->SetText(_T("Play/Pause"));
    pControlRow->AddItem(pAnimBtn);

    auto* pFrameText = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}, {_T("margin"), _T("8,0,0,0")}});
    pFrameText->SetText(_T("Current Frame/Total:"));
    pControlRow->AddItem(pFrameText);

    auto* pFrameValue = ui::Create<ui::Label>(this, {{_T("width"), _T("120")}, {_T("valign"), _T("center")}, {_T("margin"), _T("8,0,0,0")}});
    pFrameValue->SetName(_T("animation_frame"));
    pFrameValue->SetText(_T("[000/100]"));
    pControlRow->AddItem(pFrameValue);

    auto* pGotoBtn = ui::Create<ui::Button>(this, {{_T("width"), _T("56")}, {_T("height"), _T("30")}, {_T("border_round"), _T("3,3")}, {_T("margin"), _T("8,0,0,0")}, {_T("valign"), _T("center")}});
    pGotoBtn->SetClass(_T("btn_global_color_gray"));
    pGotoBtn->SetName(_T("goto_frame_btn"));
    pGotoBtn->SetText(_T("Go to"));
    pControlRow->AddItem(pGotoBtn);

    auto* pFrameNoText = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}, {_T("margin"), _T("8,0,0,0")}});
    pFrameNoText->SetText(_T("No."));
    pControlRow->AddItem(pFrameNoText);

    auto* pGotoNumber = ui::Create<ui::RichEdit>(this, {{_T("number"), _T("true")}, {_T("width"), _T("80")}, {_T("min_number"), _T("0")}, {_T("max_number"), _T("10000")}, {_T("valign"), _T("center")}, {_T("text_align"), _T("vcenter")}, {_T("margin"), _T("8,0,0,0")}});
    pGotoNumber->SetClass(_T("simple simple_border rich_edit_spin"));
    pGotoNumber->SetName(_T("goto_frame_number"));
    pGotoNumber->SetBkColor(_T("white"));
    pControlRow->AddItem(pGotoNumber);

    auto* pFrameEndText = ui::Create<ui::Label>(this, {{_T("valign"), _T("center")}, {_T("margin"), _T("8,0,0,0")}});
    pFrameEndText->SetText(_T("Frame"));
    pControlRow->AddItem(pFrameEndText);

    auto* pAnimationTest = ui::Create<ui::Control>(this, {{_T("halign"), _T("center")}, {_T("valign"), _T("center")}});
    pAnimationTest->SetName(_T("animation_test"));
    pAnimationTest->SetBkImage(_T("file='render/apng_test.png' name='bk_animation_test' auto_play='true' icon_as_animation='true' icon_frame_delay='2000' valign='center' halign='center'"));
    pContent->AddItem(pAnimationTest);

    AttachBox(pRoot);
}

void AnimationForm::OnInitWindow()
{
    BuildUI();

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
