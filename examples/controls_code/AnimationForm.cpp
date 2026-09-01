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
    return "";
}

DString AnimationForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return "";
}

void AnimationForm::BuildUI()
{
    // Corresponds to the animation.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor("bk_wnd_darkcolor");

    // Title bar area
    auto* pCaption = ui::Create<ui::HBox>(this, {{"name", "window_caption_bar"}, {"width", "stretch"}, {"height", "36"}});
    pCaption->SetBkColor("bk_wnd_lightcolor");
    pRoot->AddItem(pCaption);

    auto* pSpacer = ui::Create<ui::Control>(this, {{"mouse_enabled", "false"}});
    pCaption->AddItem(pSpacer);

    auto* pFullscreenBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pFullscreenBtn->SetClass("btn_wnd_fullscreen_11");
    pFullscreenBtn->SetName("fullscreenbtn");
    pFullscreenBtn->SetToolTipText("Fullscreen, press ESC to exit fullscreen");
    pCaption->AddItem(pFullscreenBtn);

    auto* pMinBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pMinBtn->SetClass("btn_wnd_min_11");
    pMinBtn->SetName("minbtn");
    pMinBtn->SetToolTipText("Minimize");
    pCaption->AddItem(pMinBtn);

    auto* pMaxBox = ui::Create<ui::Box>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,2,0,2"}});
    pCaption->AddItem(pMaxBox);

    auto* pMaxBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "stretch"}});
    pMaxBtn->SetClass("btn_wnd_max_11");
    pMaxBtn->SetName("maxbtn");
    pMaxBtn->SetToolTipText("Maximize");
    pMaxBox->AddItem(pMaxBtn);

    auto* pRestoreBtn = ui::Create<ui::Button>(this, {{"height", "32"}, {"width", "stretch"}});
    pRestoreBtn->SetClass("btn_wnd_restore_11");
    pRestoreBtn->SetName("restorebtn");
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText("Restore");
    pMaxBox->AddItem(pRestoreBtn);

    auto* pCloseBtn = ui::Create<ui::Button>(this, {{"height", "stretch"}, {"width", "40"}, {"margin", "0,0,0,2"}});
    pCloseBtn->SetClass("btn_wnd_close_11");
    pCloseBtn->SetName("closebtn");
    pCloseBtn->SetToolTipText("Close");
    pCaption->AddItem(pCloseBtn);

    // Work area
    auto* pContent = ui::Create<ui::VBox>(this, {{"child_halign", "center"}, {"child_valign", "center"}});
    pRoot->AddItem(pContent);

    auto* pControlRow = ui::Create<ui::HBox>(this, {{"height", "40"}, {"margin", "8,0,0,0"}});
    pContent->AddItem(pControlRow);

    auto* pAnimBtn = ui::Create<ui::Button>(this, {{"width", "96"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "8,0,0,0"}, {"valign", "center"}});
    pAnimBtn->SetClass("btn_global_color_gray");
    pAnimBtn->SetName("animation_btn");
    pAnimBtn->SetText("Play/Pause");
    pControlRow->AddItem(pAnimBtn);

    auto* pFrameText = ui::Create<ui::Label>(this, {{"valign", "center"}, {"margin", "8,0,0,0"}});
    pFrameText->SetText("Current Frame/Total:");
    pControlRow->AddItem(pFrameText);

    auto* pFrameValue = ui::Create<ui::Label>(this, {{"width", "120"}, {"valign", "center"}, {"margin", "8,0,0,0"}});
    pFrameValue->SetName("animation_frame");
    pFrameValue->SetText("[000/100]");
    pControlRow->AddItem(pFrameValue);

    auto* pGotoBtn = ui::Create<ui::Button>(this, {{"width", "56"}, {"height", "30"}, {"border_round", "3,3"}, {"margin", "8,0,0,0"}, {"valign", "center"}});
    pGotoBtn->SetClass("btn_global_color_gray");
    pGotoBtn->SetName("goto_frame_btn");
    pGotoBtn->SetText("Go to");
    pControlRow->AddItem(pGotoBtn);

    auto* pFrameNoText = ui::Create<ui::Label>(this, {{"valign", "center"}, {"margin", "8,0,0,0"}});
    pFrameNoText->SetText("No.");
    pControlRow->AddItem(pFrameNoText);

    auto* pGotoNumber = ui::Create<ui::RichEdit>(this, {{"number", "true"}, {"width", "80"}, {"min_number", "0"}, {"max_number", "10000"}, {"valign", "center"}, {"text_align", "vcenter"}, {"margin", "8,0,0,0"}});
    pGotoNumber->SetClass("simple simple_border rich_edit_spin");
    pGotoNumber->SetName("goto_frame_number");
    pGotoNumber->SetBkColor("white");
    pControlRow->AddItem(pGotoNumber);

    auto* pFrameEndText = ui::Create<ui::Label>(this, {{"valign", "center"}, {"margin", "8,0,0,0"}});
    pFrameEndText->SetText("Frame");
    pControlRow->AddItem(pFrameEndText);

    auto* pAnimationTest = ui::Create<ui::Control>(this, {{"halign", "center"}, {"valign", "center"}});
    pAnimationTest->SetName("animation_test");
    pAnimationTest->SetBkImage("file='render/apng_test.png' name='bk_animation_test' auto_play='true' icon_as_animation='true' icon_frame_delay='2000' valign='center' halign='center'");
    pContent->AddItem(pAnimationTest);

    AttachBox(pRoot);
}

void AnimationForm::OnInitWindow()
{
    BuildUI();

    //Bind animation playback related events
    ui::Control* pControl = ui::Find<ui::Control>(this, "animation_test");
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
    ui::Button* pButton = ui::Find<ui::Button>(this, "animation_btn");
    if (pButton != nullptr) {
        pButton->AttachClick([this](const ui::EventArgs& arg) {
            ui::Control* pControl = ui::Find<ui::Control>(this, "animation_test");
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

    pButton = ui::Find<ui::Button>(this, "goto_frame_btn");
    if (pButton != nullptr) {
        pButton->AttachClick([this](const ui::EventArgs& arg) {
            ui::Control* pControl = ui::Find<ui::Control>(this, "animation_test");
            if (pControl != nullptr) {
                if (m_bImagePlaying) {
                    //Pause
                    pControl->StopImageAnimation();
                }
                int32_t nFrameIndex = 0;
                ui::RichEdit* pRichEdit = ui::Find<ui::RichEdit>(this, "goto_frame_number");
                if (pRichEdit != nullptr) {
                    nFrameIndex = (int32_t)pRichEdit->GetTextNumber();
                }
                pControl->SetImageAnimationFrame("bk_animation_test", nFrameIndex);
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
        ui::Button* pButton = ui::Find<ui::Button>(this, "animation_btn");
        if (pButton != nullptr) {
            pButton->SetText("Pause");
        }
        ui::RichEdit* pRichEdit = ui::Find<ui::RichEdit>(this, "goto_frame_number");
        if (pRichEdit != nullptr) {
            pRichEdit->SetMinNumber(0);
            pRichEdit->SetMaxNumber((int32_t)status.m_nFrameCount - 1);
        }
    }
    else if (eventType == ui::EventType::kEventImageAnimationStop) {
        //Playback stopped
        m_bImagePlaying = false;
        ui::Button* pButton = ui::Find<ui::Button>(this, "animation_btn");
        if (pButton != nullptr) {
            pButton->SetText("Play");
        }
    }
    else if (eventType == ui::EventType::kEventImageAnimationPlayFrame) {
        //Playing
        m_bImagePlaying = true;
        ui::Button* pButton = ui::Find<ui::Button>(this, "animation_btn");
        if (pButton != nullptr) {
            pButton->SetText("Pause");
        }

        ui::Label* pLabel = ui::Find<ui::Label>(this, "animation_frame");
        if (pLabel != nullptr) {
            DString statusText = ui::StringUtil::Printf("[%d/%d]", status.m_nFrameIndex, status.m_nFrameCount);
            pLabel->SetText(statusText);
        }
    }
}
