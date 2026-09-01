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
    return DUI_T("");
}

DString AnimationForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return DUI_T("");
}

void AnimationForm::BuildUI()
{
    // Corresponds to the animation.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor(DUI_T("bk_wnd_darkcolor"));

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{DUI_T("name"), DUI_T("window_caption_bar")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("height"), DUI_T("36")}});
    pCaption->SetBkColor(DUI_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    auto* pSpacer = ui::Create<ui::Control>(this, {{DUI_T("mouse_enabled"), DUI_T("false")}});
    pCaption->AddItem(pSpacer);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    pFullscreenBtn->SetClass(DUI_T("btn_wnd_fullscreen_11"));
    pFullscreenBtn->SetName(DUI_T("fullscreenbtn"));
    pFullscreenBtn->SetToolTipText(DUI_T("Fullscreen, press ESC to exit fullscreen"));
    pCaption->AddItem(pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    pMinBtn->SetClass(DUI_T("btn_wnd_min_11"));
    pMinBtn->SetName(DUI_T("minbtn"));
    pMinBtn->SetToolTipText(DUI_T("Minimize"));
    pCaption->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,2,0,2")}});
    pCaption->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}});
    pMaxBtn->SetClass(DUI_T("btn_wnd_max_11"));
    pMaxBtn->SetName(DUI_T("maxbtn"));
    pMaxBtn->SetToolTipText(DUI_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("32")}, {DUI_T("width"), DUI_T("stretch")}});
    pRestoreBtn->SetClass(DUI_T("btn_wnd_restore_11"));
    pRestoreBtn->SetName(DUI_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(DUI_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("40")}, {DUI_T("margin"), DUI_T("0,0,0,2")}});
    pCloseBtn->SetClass(DUI_T("btn_wnd_close_11"));
    pCloseBtn->SetName(DUI_T("closebtn"));
    pCloseBtn->SetToolTipText(DUI_T("Close"));
    pCaption->AddItem(pCloseBtn);

    // Work area
    auto* pContent = ui::Create<ui::VBox>(this, {{DUI_T("child_halign"), DUI_T("center")}, {DUI_T("child_valign"), DUI_T("center")}});
    pRoot->AddItem(pContent);

    auto* pControlRow = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("40")}, {DUI_T("margin"), DUI_T("8,0,0,0")}});
    pContent->AddItem(pControlRow);

    auto* pAnimBtn = ui::Create<ui::Button>(this, {{DUI_T("width"), DUI_T("96")}, {DUI_T("height"), DUI_T("30")}, {DUI_T("border_round"), DUI_T("3,3")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("valign"), DUI_T("center")}});
    pAnimBtn->SetClass(DUI_T("btn_global_color_gray"));
    pAnimBtn->SetName(DUI_T("animation_btn"));
    pAnimBtn->SetText(DUI_T("Play/Pause"));
    pControlRow->AddItem(pAnimBtn);

    auto* pFrameText = ui::Create<ui::Label>(this, {{DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}});
    pFrameText->SetText(DUI_T("Current Frame/Total:"));
    pControlRow->AddItem(pFrameText);

    auto* pFrameValue = ui::Create<ui::Label>(this, {{DUI_T("width"), DUI_T("120")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}});
    pFrameValue->SetName(DUI_T("animation_frame"));
    pFrameValue->SetText(DUI_T("[000/100]"));
    pControlRow->AddItem(pFrameValue);

    auto* pGotoBtn = ui::Create<ui::Button>(this, {{DUI_T("width"), DUI_T("56")}, {DUI_T("height"), DUI_T("30")}, {DUI_T("border_round"), DUI_T("3,3")}, {DUI_T("margin"), DUI_T("8,0,0,0")}, {DUI_T("valign"), DUI_T("center")}});
    pGotoBtn->SetClass(DUI_T("btn_global_color_gray"));
    pGotoBtn->SetName(DUI_T("goto_frame_btn"));
    pGotoBtn->SetText(DUI_T("Go to"));
    pControlRow->AddItem(pGotoBtn);

    auto* pFrameNoText = ui::Create<ui::Label>(this, {{DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}});
    pFrameNoText->SetText(DUI_T("No."));
    pControlRow->AddItem(pFrameNoText);

    auto* pGotoNumber = ui::Create<ui::RichEdit>(this, {{DUI_T("number"), DUI_T("true")}, {DUI_T("width"), DUI_T("80")}, {DUI_T("min_number"), DUI_T("0")}, {DUI_T("max_number"), DUI_T("10000")}, {DUI_T("valign"), DUI_T("center")}, {DUI_T("text_align"), DUI_T("vcenter")}, {DUI_T("margin"), DUI_T("8,0,0,0")}});
    pGotoNumber->SetClass(DUI_T("simple simple_border rich_edit_spin"));
    pGotoNumber->SetName(DUI_T("goto_frame_number"));
    pGotoNumber->SetBkColor(DUI_T("white"));
    pControlRow->AddItem(pGotoNumber);

    auto* pFrameEndText = ui::Create<ui::Label>(this, {{DUI_T("valign"), DUI_T("center")}, {DUI_T("margin"), DUI_T("8,0,0,0")}});
    pFrameEndText->SetText(DUI_T("Frame"));
    pControlRow->AddItem(pFrameEndText);

    auto* pAnimationTest = ui::Create<ui::Control>(this, {{DUI_T("halign"), DUI_T("center")}, {DUI_T("valign"), DUI_T("center")}});
    pAnimationTest->SetName(DUI_T("animation_test"));
    pAnimationTest->SetBkImage(DUI_T("file='render/apng_test.png' name='bk_animation_test' auto_play='true' icon_as_animation='true' icon_frame_delay='2000' valign='center' halign='center'"));
    pContent->AddItem(pAnimationTest);

    AttachBox(pRoot);
}

void AnimationForm::OnInitWindow()
{
    BuildUI();

    //Bind animation playback related events
    ui::Control* pControl = ui::Find<ui::Control>(this, DUI_T("animation_test"));
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
    ui::Button* pButton = ui::Find<ui::Button>(this, DUI_T("animation_btn"));
    if (pButton != nullptr) {
        pButton->AttachClick([this](const ui::EventArgs& arg) {
            ui::Control* pControl = ui::Find<ui::Control>(this, DUI_T("animation_test"));
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

    pButton = ui::Find<ui::Button>(this, DUI_T("goto_frame_btn"));
    if (pButton != nullptr) {
        pButton->AttachClick([this](const ui::EventArgs& arg) {
            ui::Control* pControl = ui::Find<ui::Control>(this, DUI_T("animation_test"));
            if (pControl != nullptr) {
                if (m_bImagePlaying) {
                    //Pause
                    pControl->StopImageAnimation();
                }
                int32_t nFrameIndex = 0;
                ui::RichEdit* pRichEdit = ui::Find<ui::RichEdit>(this, DUI_T("goto_frame_number"));
                if (pRichEdit != nullptr) {
                    nFrameIndex = (int32_t)pRichEdit->GetTextNumber();
                }
                pControl->SetImageAnimationFrame(DUI_T("bk_animation_test"), nFrameIndex);
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
        ui::Button* pButton = ui::Find<ui::Button>(this, DUI_T("animation_btn"));
        if (pButton != nullptr) {
            pButton->SetText(DUI_T("Pause"));
        }
        ui::RichEdit* pRichEdit = ui::Find<ui::RichEdit>(this, DUI_T("goto_frame_number"));
        if (pRichEdit != nullptr) {
            pRichEdit->SetMinNumber(0);
            pRichEdit->SetMaxNumber((int32_t)status.m_nFrameCount - 1);
        }
    }
    else if (eventType == ui::EventType::kEventImageAnimationStop) {
        //Playback stopped
        m_bImagePlaying = false;
        ui::Button* pButton = ui::Find<ui::Button>(this, DUI_T("animation_btn"));
        if (pButton != nullptr) {
            pButton->SetText(DUI_T("Play"));
        }
    }
    else if (eventType == ui::EventType::kEventImageAnimationPlayFrame) {
        //Playing
        m_bImagePlaying = true;
        ui::Button* pButton = ui::Find<ui::Button>(this, DUI_T("animation_btn"));
        if (pButton != nullptr) {
            pButton->SetText(DUI_T("Pause"));
        }

        ui::Label* pLabel = ui::Find<ui::Label>(this, DUI_T("animation_frame"));
        if (pLabel != nullptr) {
            DString statusText = ui::StringUtil::Printf(DUI_T("[%d/%d]"), status.m_nFrameIndex, status.m_nFrameCount);
            pLabel->SetText(statusText);
        }
    }
}
