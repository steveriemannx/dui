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
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("bk_wnd_darkcolor"));

    // Title bar area
    ui::HBox* pCaption = new ui::HBox(this);
    pCaption->SetAttribute(_T("name"), _T("window_caption_bar"));
    pCaption->SetAttribute(_T("width"), _T("stretch"));
    pCaption->SetAttribute(_T("height"), _T("36"));
    pCaption->SetBkColor(_T("bk_wnd_lightcolor"));
    pRoot->AddItem(pCaption);

    ui::Control* pSpacer = new ui::Control(this);
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCaption->AddItem(pSpacer);

    ui::Button* pFullscreenBtn = new ui::Button(this);
    pFullscreenBtn->SetClass(_T("btn_wnd_fullscreen_11"));
    pFullscreenBtn->SetAttribute(_T("height"), _T("32"));
    pFullscreenBtn->SetAttribute(_T("width"), _T("40"));
    pFullscreenBtn->SetName(_T("fullscreenbtn"));
    pFullscreenBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pFullscreenBtn->SetToolTipText(_T("Fullscreen, press ESC to exit fullscreen"));
    pCaption->AddItem(pFullscreenBtn);

    ui::Button* pMinBtn = new ui::Button(this);
    pMinBtn->SetClass(_T("btn_wnd_min_11"));
    pMinBtn->SetAttribute(_T("height"), _T("32"));
    pMinBtn->SetAttribute(_T("width"), _T("40"));
    pMinBtn->SetName(_T("minbtn"));
    pMinBtn->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pMinBtn->SetToolTipText(_T("Minimize"));
    pCaption->AddItem(pMinBtn);

    ui::Box* pMaxBox = new ui::Box(this);
    pMaxBox->SetAttribute(_T("height"), _T("stretch"));
    pMaxBox->SetAttribute(_T("width"), _T("40"));
    pMaxBox->SetAttribute(_T("margin"), _T("0,2,0,2"));
    pCaption->AddItem(pMaxBox);

    ui::Button* pMaxBtn = new ui::Button(this);
    pMaxBtn->SetClass(_T("btn_wnd_max_11"));
    pMaxBtn->SetAttribute(_T("height"), _T("32"));
    pMaxBtn->SetAttribute(_T("width"), _T("stretch"));
    pMaxBtn->SetName(_T("maxbtn"));
    pMaxBtn->SetToolTipText(_T("Maximize"));
    pMaxBox->AddItem(pMaxBtn);

    ui::Button* pRestoreBtn = new ui::Button(this);
    pRestoreBtn->SetClass(_T("btn_wnd_restore_11"));
    pRestoreBtn->SetAttribute(_T("height"), _T("32"));
    pRestoreBtn->SetAttribute(_T("width"), _T("stretch"));
    pRestoreBtn->SetName(_T("restorebtn"));
    pRestoreBtn->SetVisible(false);
    pRestoreBtn->SetToolTipText(_T("Restore"));
    pMaxBox->AddItem(pRestoreBtn);

    ui::Button* pCloseBtn = new ui::Button(this);
    pCloseBtn->SetClass(_T("btn_wnd_close_11"));
    pCloseBtn->SetAttribute(_T("height"), _T("stretch"));
    pCloseBtn->SetAttribute(_T("width"), _T("40"));
    pCloseBtn->SetName(_T("closebtn"));
    pCloseBtn->SetAttribute(_T("margin"), _T("0,0,0,2"));
    pCloseBtn->SetToolTipText(_T("Close"));
    pCaption->AddItem(pCloseBtn);

    // Work area
    ui::VBox* pContent = new ui::VBox(this);
    pContent->SetAttribute(_T("child_halign"), _T("center"));
    pContent->SetAttribute(_T("child_valign"), _T("center"));
    pRoot->AddItem(pContent);

    ui::HBox* pControlRow = new ui::HBox(this);
    pControlRow->SetAttribute(_T("height"), _T("40"));
    pControlRow->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pContent->AddItem(pControlRow);

    ui::Button* pAnimBtn = new ui::Button(this);
    pAnimBtn->SetClass(_T("btn_global_color_gray"));
    pAnimBtn->SetName(_T("animation_btn"));
    pAnimBtn->SetText(_T("Play/Pause"));
    pAnimBtn->SetAttribute(_T("width"), _T("96"));
    pAnimBtn->SetAttribute(_T("height"), _T("30"));
    pAnimBtn->SetAttribute(_T("border_round"), _T("3,3"));
    pAnimBtn->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pAnimBtn->SetAttribute(_T("valign"), _T("center"));
    pControlRow->AddItem(pAnimBtn);

    ui::Label* pFrameText = new ui::Label(this);
    pFrameText->SetText(_T("Current Frame/Total:"));
    pFrameText->SetAttribute(_T("valign"), _T("center"));
    pFrameText->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pControlRow->AddItem(pFrameText);

    ui::Label* pFrameValue = new ui::Label(this);
    pFrameValue->SetName(_T("animation_frame"));
    pFrameValue->SetText(_T("[000/100]"));
    pFrameValue->SetAttribute(_T("width"), _T("120"));
    pFrameValue->SetAttribute(_T("valign"), _T("center"));
    pFrameValue->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pControlRow->AddItem(pFrameValue);

    ui::Button* pGotoBtn = new ui::Button(this);
    pGotoBtn->SetClass(_T("btn_global_color_gray"));
    pGotoBtn->SetName(_T("goto_frame_btn"));
    pGotoBtn->SetText(_T("Go to"));
    pGotoBtn->SetAttribute(_T("width"), _T("56"));
    pGotoBtn->SetAttribute(_T("height"), _T("30"));
    pGotoBtn->SetAttribute(_T("border_round"), _T("3,3"));
    pGotoBtn->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pGotoBtn->SetAttribute(_T("valign"), _T("center"));
    pControlRow->AddItem(pGotoBtn);

    ui::Label* pFrameNoText = new ui::Label(this);
    pFrameNoText->SetText(_T("No."));
    pFrameNoText->SetAttribute(_T("valign"), _T("center"));
    pFrameNoText->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pControlRow->AddItem(pFrameNoText);

    ui::RichEdit* pGotoNumber = new ui::RichEdit(this);
    pGotoNumber->SetClass(_T("simple simple_border rich_edit_spin"));
    pGotoNumber->SetName(_T("goto_frame_number"));
    pGotoNumber->SetAttribute(_T("number"), _T("true"));
    pGotoNumber->SetAttribute(_T("width"), _T("80"));
    pGotoNumber->SetAttribute(_T("min_number"), _T("0"));
    pGotoNumber->SetAttribute(_T("max_number"), _T("10000"));
    pGotoNumber->SetAttribute(_T("valign"), _T("center"));
    pGotoNumber->SetAttribute(_T("text_align"), _T("vcenter"));
    pGotoNumber->SetBkColor(_T("white"));
    pGotoNumber->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pControlRow->AddItem(pGotoNumber);

    ui::Label* pFrameEndText = new ui::Label(this);
    pFrameEndText->SetText(_T("Frame"));
    pFrameEndText->SetAttribute(_T("valign"), _T("center"));
    pFrameEndText->SetAttribute(_T("margin"), _T("8,0,0,0"));
    pControlRow->AddItem(pFrameEndText);

    ui::Control* pAnimationTest = new ui::Control(this);
    pAnimationTest->SetName(_T("animation_test"));
    pAnimationTest->SetAttribute(_T("halign"), _T("center"));
    pAnimationTest->SetAttribute(_T("valign"), _T("center"));
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
