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
    return DUI_T("");
}

DString TestForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return DUI_T("");
}

void TestForm::BuildUI()
{
    // Corresponds to the test.xml layout
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(DUI_T("white"));

    auto* pHeader = ui::Create<ui::HBox>(this, {{DUI_T("height"), DUI_T("30")}, {DUI_T("margin"), DUI_T("0,10,0,0")}});
    ui::Attach(pRoot, pHeader);

    auto* pTitle = ui::Create<ui::Label>(this, {{DUI_T("text"), DUI_T("Current Progress:")}});
    ui::Attach(pHeader, pTitle);

    auto* pProgressText = ui::Create<ui::Label>(this, {{DUI_T("name"), DUI_T("progress_text")}, {DUI_T("text"), DUI_T("0%")}});
    ui::Attach(pHeader, pProgressText);

    auto* pBody = ui::Create<ui::HBox>(this, {{DUI_T("margin"), DUI_T("0,10,0,0")}});
    ui::Attach(pRoot, pBody);

    // Horizontal progress bar
    auto* pLeftCol = ui::Create<ui::VBox>(this, {{DUI_T("valign"), DUI_T("center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("width"), DUI_T("50%")}, {DUI_T("height"), DUI_T("100%")}});
    ui::Attach(pBody, pLeftCol);

    auto* pProgress11 = ui::Create<ui::Progress>(this, {{DUI_T("class"), DUI_T("progress_horizontal_blue")}, {DUI_T("name"), DUI_T("progress11")}, {DUI_T("reverse"), DUI_T("false")}, {DUI_T("height"), DUI_T("6")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("min"), DUI_T("0")}, {DUI_T("max"), DUI_T("100")}, {DUI_T("value"), DUI_T("0")}, {DUI_T("margin"), DUI_T("20,20,20,20")}});
    pProgress11->SetToolTipText(DUI_T("ui::Progress"));
    ui::Attach(pLeftCol, pProgress11);

    auto* pProgress12 = ui::Create<ui::Progress>(this, {{DUI_T("progress_color"), DUI_T("blue")}, {DUI_T("name"), DUI_T("progress12")}, {DUI_T("reverse"), DUI_T("false")}, {DUI_T("height"), DUI_T("6")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("min"), DUI_T("0")}, {DUI_T("max"), DUI_T("100")}, {DUI_T("value"), DUI_T("0")}, {DUI_T("marquee"), DUI_T("false")}, {DUI_T("margin"), DUI_T("20,20,20,20")}, {DUI_T("bkcolor"), DUI_T("LightGray")}});
    pProgress12->SetToolTipText(DUI_T("ui::Progress"));
    ui::Attach(pLeftCol, pProgress12);

    auto* pProgress13 = ui::Create<ui::Slider>(this, {{DUI_T("class"), DUI_T("slider_horizontal_green")}, {DUI_T("name"), DUI_T("progress13")}, {DUI_T("reverse"), DUI_T("false")}, {DUI_T("height"), DUI_T("14")}, {DUI_T("width"), DUI_T("stretch")}, {DUI_T("min"), DUI_T("0")}, {DUI_T("max"), DUI_T("100")}, {DUI_T("value"), DUI_T("0")}, {DUI_T("progress_bar_padding"), DUI_T("0,4,0,4")}, {DUI_T("margin"), DUI_T("20,20,20,20")}});
    pProgress13->SetToolTipText(DUI_T("ui::Slider"));
    ui::Attach(pLeftCol, pProgress13);

    auto* pProgress14 = ui::Create<ui::CircleProgress>(this, {{DUI_T("name"), DUI_T("progress14")}, {DUI_T("reverse"), DUI_T("false")}, {DUI_T("circular"), DUI_T("true")}, {DUI_T("height"), DUI_T("80")}, {DUI_T("width"), DUI_T("80")}, {DUI_T("circle_width"), DUI_T("12")}, {DUI_T("bgcolor"), DUI_T("gray")}, {DUI_T("fgcolor"), DUI_T("green")}, {DUI_T("gradient_color"), DUI_T("red")}, {DUI_T("clockwise"), DUI_T("true")}, {DUI_T("min"), DUI_T("0")}, {DUI_T("max"), DUI_T("100")}, {DUI_T("value"), DUI_T("0")}, {DUI_T("margin"), DUI_T("20,20,20,20")}, {DUI_T("text_padding"), DUI_T("10,32,10,10")}, {DUI_T("normal_text_color"), DUI_T("darkcolor")}, {DUI_T("indicator"), DUI_T("file='public/progress/indicator.svg' width='12' height='12'")}});
    pProgress14->SetToolTipText(DUI_T("ui::CircleProgress"));
    ui::Attach(pLeftCol, pProgress14);

    // Vertical progress bar
    auto* pRightCol = ui::Create<ui::HBox>(this, {{DUI_T("valign"), DUI_T("center")}, {DUI_T("halign"), DUI_T("center")}, {DUI_T("width"), DUI_T("50%")}, {DUI_T("height"), DUI_T("100%")}});
    ui::Attach(pBody, pRightCol);

    auto* pProgress21 = ui::Create<ui::Progress>(this, {{DUI_T("class"), DUI_T("progress_vertical_blue")}, {DUI_T("name"), DUI_T("progress21")}, {DUI_T("reverse"), DUI_T("false")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("6")}, {DUI_T("min"), DUI_T("0")}, {DUI_T("max"), DUI_T("100")}, {DUI_T("value"), DUI_T("0")}, {DUI_T("horizontal"), DUI_T("false")}, {DUI_T("margin"), DUI_T("20,20,20,20")}});
    pProgress21->SetToolTipText(DUI_T("ui::Progress"));
    ui::Attach(pRightCol, pProgress21);

    auto* pProgress22 = ui::Create<ui::Progress>(this, {{DUI_T("progress_color"), DUI_T("blue")}, {DUI_T("name"), DUI_T("progress22")}, {DUI_T("reverse"), DUI_T("false")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("6")}, {DUI_T("min"), DUI_T("0")}, {DUI_T("max"), DUI_T("100")}, {DUI_T("value"), DUI_T("0")}, {DUI_T("horizontal"), DUI_T("false")}, {DUI_T("margin"), DUI_T("20,20,20,20")}, {DUI_T("bkcolor"), DUI_T("LightGray")}});
    pProgress22->SetToolTipText(DUI_T("ui::Progress"));
    ui::Attach(pRightCol, pProgress22);

    auto* pProgress23 = ui::Create<ui::Slider>(this, {{DUI_T("class"), DUI_T("slider_vertical_green")}, {DUI_T("name"), DUI_T("progress23")}, {DUI_T("reverse"), DUI_T("false")}, {DUI_T("height"), DUI_T("stretch")}, {DUI_T("width"), DUI_T("14")}, {DUI_T("min"), DUI_T("0")}, {DUI_T("max"), DUI_T("100")}, {DUI_T("value"), DUI_T("0")}, {DUI_T("progress_bar_padding"), DUI_T("4,0,4,0")}, {DUI_T("margin"), DUI_T("20,20,20,20")}, {DUI_T("horizontal"), DUI_T("false")}});
    pProgress23->SetToolTipText(DUI_T("ui::Slider"));
    ui::Attach(pRightCol, pProgress23);

    auto* pProgress24 = ui::Create<ui::CircleProgress>(this, {{DUI_T("name"), DUI_T("progress24")}, {DUI_T("reverse"), DUI_T("false")}, {DUI_T("circular"), DUI_T("true")}, {DUI_T("height"), DUI_T("80")}, {DUI_T("width"), DUI_T("80")}, {DUI_T("horizontal"), DUI_T("false")}, {DUI_T("circle_width"), DUI_T("12")}, {DUI_T("bgcolor"), DUI_T("gray")}, {DUI_T("fgcolor"), DUI_T("green")}, {DUI_T("gradient_color"), DUI_T("red")}, {DUI_T("clockwise"), DUI_T("true")}, {DUI_T("min"), DUI_T("0")}, {DUI_T("max"), DUI_T("100")}, {DUI_T("value"), DUI_T("0")}, {DUI_T("margin"), DUI_T("20,20,20,20")}, {DUI_T("text_padding"), DUI_T("10,32,10,10")}, {DUI_T("normal_text_color"), DUI_T("darkcolor")}, {DUI_T("indicator"), DUI_T("file='public/progress/indicator.svg' width='12' height='12'")}});
    pProgress24->SetToolTipText(DUI_T("ui::CircleProgress"));
    ui::Attach(pRightCol, pProgress24);

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
    ui::Label* pLabel = ui::Find<ui::Label>(this, DUI_T("progress_text"));
    std::vector<DString> controlList = {DUI_T("progress11"), DUI_T("progress12"), DUI_T("progress13"), DUI_T("progress14"),
                                        DUI_T("progress21"), DUI_T("progress22"), DUI_T("progress23"), DUI_T("progress24") };
    for (const DString& name : controlList) {
        ui::Progress* pProgress = ui::Find<ui::Progress>(this, name);
        if (pProgress != nullptr) {
            if (pLabel != nullptr) {
                pLabel->SetText(ui::StringUtil::Printf(DUI_T("%d%%"), (int32_t)m_nProgressValue));                
            }
            pProgress->SetValue(m_nProgressValue);
        }
    }

    m_nProgressValue += 0.4;
    if (m_nProgressValue > 100.0) {
        m_nProgressValue = 0.0;
    }
}

