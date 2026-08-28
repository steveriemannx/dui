#include "TestForm.h"

TestForm::TestForm():
    m_nProgressValue(0.0)
{
}

TestForm::~TestForm()
{
}

DString TestForm::GetSkinFolder()
{
    return _T("");
}

DString TestForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return _T("");
}

void TestForm::BuildUI()
{
    // Corresponds to the test.xml layout
    auto* pRoot = ui::Create<ui::VBox>(this, {});
    pRoot->SetBkColor(_T("white"));

    auto* pHeader = ui::Create<ui::HBox>(this, {{_T("height"), _T("30")}, {_T("margin"), _T("0,10,0,0")}});
    pRoot->AddItem(pHeader);

    auto* pTitle = ui::Create<ui::Label>(this, {});
    pTitle->SetText(_T("Current Progress:"));
    pHeader->AddItem(pTitle);

    auto* pProgressText = ui::Create<ui::Label>(this, {});
    pProgressText->SetName(_T("progress_text"));
    pProgressText->SetText(_T("0%"));
    pHeader->AddItem(pProgressText);

    auto* pBody = ui::Create<ui::HBox>(this, {{_T("margin"), _T("0,10,0,0")}});
    pRoot->AddItem(pBody);

    // Horizontal progress bar
    auto* pLeftCol = ui::Create<ui::VBox>(this, {{_T("valign"), _T("center")}, {_T("halign"), _T("center")}, {_T("width"), _T("50%")}, {_T("height"), _T("100%")}});
    pBody->AddItem(pLeftCol);

    auto* pProgress11 = ui::Create<ui::Progress>(this, {{_T("reverse"), _T("false")}, {_T("height"), _T("6")}, {_T("width"), _T("stretch")}, {_T("min"), _T("0")}, {_T("max"), _T("100")}, {_T("value"), _T("0")}, {_T("margin"), _T("20,20,20,20")}});
    pProgress11->SetClass(_T("progress_horizontal_blue"));
    pProgress11->SetName(_T("progress11"));
    pProgress11->SetToolTipText(_T("ui::Progress"));
    pLeftCol->AddItem(pProgress11);

    auto* pProgress12 = ui::Create<ui::Progress>(this, {{_T("progress_color"), _T("blue")}, {_T("reverse"), _T("false")}, {_T("height"), _T("6")}, {_T("width"), _T("stretch")}, {_T("min"), _T("0")}, {_T("max"), _T("100")}, {_T("value"), _T("0")}, {_T("marquee"), _T("false")}, {_T("margin"), _T("20,20,20,20")}});
    pProgress12->SetName(_T("progress12"));
    pProgress12->SetBkColor(_T("LightGray"));
    pProgress12->SetToolTipText(_T("ui::Progress"));
    pLeftCol->AddItem(pProgress12);

    auto* pProgress13 = ui::Create<ui::Slider>(this, {{_T("reverse"), _T("false")}, {_T("height"), _T("14")}, {_T("width"), _T("stretch")}, {_T("min"), _T("0")}, {_T("max"), _T("100")}, {_T("value"), _T("0")}, {_T("progress_bar_padding"), _T("0,4,0,4")}, {_T("margin"), _T("20,20,20,20")}});
    pProgress13->SetClass(_T("slider_horizontal_green"));
    pProgress13->SetName(_T("progress13"));
    pProgress13->SetToolTipText(_T("ui::Slider"));
    pLeftCol->AddItem(pProgress13);

    auto* pProgress14 = ui::Create<ui::CircleProgress>(this, {{_T("reverse"), _T("false")}, {_T("circular"), _T("true")}, {_T("height"), _T("80")}, {_T("width"), _T("80")}, {_T("circle_width"), _T("12")}, {_T("bgcolor"), _T("gray")}, {_T("fgcolor"), _T("green")}, {_T("gradient_color"), _T("red")}, {_T("clockwise"), _T("true")}, {_T("min"), _T("0")}, {_T("max"), _T("100")}, {_T("value"), _T("0")}, {_T("margin"), _T("20,20,20,20")}, {_T("text_padding"), _T("10,32,10,10")}, {_T("normal_text_color"), _T("darkcolor")}, {_T("indicator"), _T("file='public/progress/indicator.svg' width='12' height='12'")}});
    pProgress14->SetName(_T("progress14"));
    pProgress14->SetToolTipText(_T("ui::CircleProgress"));
    pLeftCol->AddItem(pProgress14);

    // Vertical progress bar
    auto* pRightCol = ui::Create<ui::HBox>(this, {{_T("valign"), _T("center")}, {_T("halign"), _T("center")}, {_T("width"), _T("50%")}, {_T("height"), _T("100%")}});
    pBody->AddItem(pRightCol);

    auto* pProgress21 = ui::Create<ui::Progress>(this, {{_T("reverse"), _T("false")}, {_T("height"), _T("stretch")}, {_T("width"), _T("6")}, {_T("min"), _T("0")}, {_T("max"), _T("100")}, {_T("value"), _T("0")}, {_T("horizontal"), _T("false")}, {_T("margin"), _T("20,20,20,20")}});
    pProgress21->SetClass(_T("progress_vertical_blue"));
    pProgress21->SetName(_T("progress21"));
    pProgress21->SetToolTipText(_T("ui::Progress"));
    pRightCol->AddItem(pProgress21);

    auto* pProgress22 = ui::Create<ui::Progress>(this, {{_T("progress_color"), _T("blue")}, {_T("reverse"), _T("false")}, {_T("height"), _T("stretch")}, {_T("width"), _T("6")}, {_T("min"), _T("0")}, {_T("max"), _T("100")}, {_T("value"), _T("0")}, {_T("horizontal"), _T("false")}, {_T("margin"), _T("20,20,20,20")}});
    pProgress22->SetName(_T("progress22"));
    pProgress22->SetBkColor(_T("LightGray"));
    pProgress22->SetToolTipText(_T("ui::Progress"));
    pRightCol->AddItem(pProgress22);

    auto* pProgress23 = ui::Create<ui::Slider>(this, {{_T("reverse"), _T("false")}, {_T("height"), _T("stretch")}, {_T("width"), _T("14")}, {_T("min"), _T("0")}, {_T("max"), _T("100")}, {_T("value"), _T("0")}, {_T("progress_bar_padding"), _T("4,0,4,0")}, {_T("margin"), _T("20,20,20,20")}, {_T("horizontal"), _T("false")}});
    pProgress23->SetClass(_T("slider_vertical_green"));
    pProgress23->SetName(_T("progress23"));
    pProgress23->SetToolTipText(_T("ui::Slider"));
    pRightCol->AddItem(pProgress23);

    auto* pProgress24 = ui::Create<ui::CircleProgress>(this, {{_T("reverse"), _T("false")}, {_T("circular"), _T("true")}, {_T("height"), _T("80")}, {_T("width"), _T("80")}, {_T("horizontal"), _T("false")}, {_T("circle_width"), _T("12")}, {_T("bgcolor"), _T("gray")}, {_T("fgcolor"), _T("green")}, {_T("gradient_color"), _T("red")}, {_T("clockwise"), _T("true")}, {_T("min"), _T("0")}, {_T("max"), _T("100")}, {_T("value"), _T("0")}, {_T("margin"), _T("20,20,20,20")}, {_T("text_padding"), _T("10,32,10,10")}, {_T("normal_text_color"), _T("darkcolor")}, {_T("indicator"), _T("file='public/progress/indicator.svg' width='12' height='12'")}});
    pProgress24->SetName(_T("progress24"));
    pProgress24->SetToolTipText(_T("ui::CircleProgress"));
    pRightCol->AddItem(pProgress24);

    AttachBox(pRoot);
}

void TestForm::OnInitWindow()
{
    BuildUI();

    //Start a timer
    ui::GlobalManager::Instance().Thread().PostRepeatedTask(ui::kThreadUI,
        ui::UiBind(this, [this]() {
                OnTimer();
            }),
        100);
}

void TestForm::OnTimer()
{
    ui::Label* pLabel = dynamic_cast<ui::Label*>(FindControl(_T("progress_text")));
    std::vector<DString> controlList = {_T("progress11"), _T("progress12"), _T("progress13"), _T("progress14"),
                                        _T("progress21"), _T("progress22"), _T("progress23"), _T("progress24") };
    for (const DString& name : controlList) {
        ui::Progress* pProgress = dynamic_cast<ui::Progress*>(FindControl(name));
        if (pProgress != nullptr) {
            if (pLabel != nullptr) {
                pLabel->SetText(ui::StringUtil::Printf(_T("%d%%"), (int32_t)m_nProgressValue));                
            }
            pProgress->SetValue(m_nProgressValue);
        }
    }

    m_nProgressValue += 0.4;
    if (m_nProgressValue > 100.0) {
        m_nProgressValue = 0.0;
    }
}

