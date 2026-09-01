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
    return "";
}

DString TestForm::GetSkinFile()
{
    // Pure code mode: no layout XML is loaded
    return "";
}

void TestForm::BuildUI()
{
    // Corresponds to the test.xml layout
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor("white");

    auto* pHeader = ui::Create<ui::HBox>(this, {{"height", "30"}, {"margin", "0,10,0,0"}});
    ui::Attach(pRoot, pHeader);

    auto* pTitle = ui::Create<ui::Label>(this, {{"text", "Current Progress:"}});
    ui::Attach(pHeader, pTitle);

    auto* pProgressText = ui::Create<ui::Label>(this, {{"name", "progress_text"}, {"text", "0%"}});
    ui::Attach(pHeader, pProgressText);

    auto* pBody = ui::Create<ui::HBox>(this, {{"margin", "0,10,0,0"}});
    ui::Attach(pRoot, pBody);

    // Horizontal progress bar
    auto* pLeftCol = ui::Create<ui::VBox>(this, {{"valign", "center"}, {"halign", "center"}, {"width", "50%"}, {"height", "100%"}});
    ui::Attach(pBody, pLeftCol);

    auto* pProgress11 = ui::Create<ui::Progress>(this, {{"class", "progress_horizontal_blue"}, {"name", "progress11"}, {"reverse", "false"}, {"height", "6"}, {"width", "stretch"}, {"min", "0"}, {"max", "100"}, {"value", "0"}, {"margin", "20,20,20,20"}});
    pProgress11->SetToolTipText("ui::Progress");
    ui::Attach(pLeftCol, pProgress11);

    auto* pProgress12 = ui::Create<ui::Progress>(this, {{"progress_color", "blue"}, {"name", "progress12"}, {"reverse", "false"}, {"height", "6"}, {"width", "stretch"}, {"min", "0"}, {"max", "100"}, {"value", "0"}, {"marquee", "false"}, {"margin", "20,20,20,20"}, {"bkcolor", "LightGray"}});
    pProgress12->SetToolTipText("ui::Progress");
    ui::Attach(pLeftCol, pProgress12);

    auto* pProgress13 = ui::Create<ui::Slider>(this, {{"class", "slider_horizontal_green"}, {"name", "progress13"}, {"reverse", "false"}, {"height", "14"}, {"width", "stretch"}, {"min", "0"}, {"max", "100"}, {"value", "0"}, {"progress_bar_padding", "0,4,0,4"}, {"margin", "20,20,20,20"}});
    pProgress13->SetToolTipText("ui::Slider");
    ui::Attach(pLeftCol, pProgress13);

    auto* pProgress14 = ui::Create<ui::CircleProgress>(this, {{"name", "progress14"}, {"reverse", "false"}, {"circular", "true"}, {"height", "80"}, {"width", "80"}, {"circle_width", "12"}, {"bgcolor", "gray"}, {"fgcolor", "green"}, {"gradient_color", "red"}, {"clockwise", "true"}, {"min", "0"}, {"max", "100"}, {"value", "0"}, {"margin", "20,20,20,20"}, {"text_padding", "10,32,10,10"}, {"normal_text_color", "darkcolor"}, {"indicator", "file='public/progress/indicator.svg' width='12' height='12'"}});
    pProgress14->SetToolTipText("ui::CircleProgress");
    ui::Attach(pLeftCol, pProgress14);

    // Vertical progress bar
    auto* pRightCol = ui::Create<ui::HBox>(this, {{"valign", "center"}, {"halign", "center"}, {"width", "50%"}, {"height", "100%"}});
    ui::Attach(pBody, pRightCol);

    auto* pProgress21 = ui::Create<ui::Progress>(this, {{"class", "progress_vertical_blue"}, {"name", "progress21"}, {"reverse", "false"}, {"height", "stretch"}, {"width", "6"}, {"min", "0"}, {"max", "100"}, {"value", "0"}, {"horizontal", "false"}, {"margin", "20,20,20,20"}});
    pProgress21->SetToolTipText("ui::Progress");
    ui::Attach(pRightCol, pProgress21);

    auto* pProgress22 = ui::Create<ui::Progress>(this, {{"progress_color", "blue"}, {"name", "progress22"}, {"reverse", "false"}, {"height", "stretch"}, {"width", "6"}, {"min", "0"}, {"max", "100"}, {"value", "0"}, {"horizontal", "false"}, {"margin", "20,20,20,20"}, {"bkcolor", "LightGray"}});
    pProgress22->SetToolTipText("ui::Progress");
    ui::Attach(pRightCol, pProgress22);

    auto* pProgress23 = ui::Create<ui::Slider>(this, {{"class", "slider_vertical_green"}, {"name", "progress23"}, {"reverse", "false"}, {"height", "stretch"}, {"width", "14"}, {"min", "0"}, {"max", "100"}, {"value", "0"}, {"progress_bar_padding", "4,0,4,0"}, {"margin", "20,20,20,20"}, {"horizontal", "false"}});
    pProgress23->SetToolTipText("ui::Slider");
    ui::Attach(pRightCol, pProgress23);

    auto* pProgress24 = ui::Create<ui::CircleProgress>(this, {{"name", "progress24"}, {"reverse", "false"}, {"circular", "true"}, {"height", "80"}, {"width", "80"}, {"horizontal", "false"}, {"circle_width", "12"}, {"bgcolor", "gray"}, {"fgcolor", "green"}, {"gradient_color", "red"}, {"clockwise", "true"}, {"min", "0"}, {"max", "100"}, {"value", "0"}, {"margin", "20,20,20,20"}, {"text_padding", "10,32,10,10"}, {"normal_text_color", "darkcolor"}, {"indicator", "file='public/progress/indicator.svg' width='12' height='12'"}});
    pProgress24->SetToolTipText("ui::CircleProgress");
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
    ui::Label* pLabel = ui::Find<ui::Label>(this, "progress_text");
    std::vector<DString> controlList = {"progress11", "progress12", "progress13", "progress14",
                                        "progress21", "progress22", "progress23", "progress24" };
    for (const DString& name : controlList) {
        ui::Progress* pProgress = ui::Find<ui::Progress>(this, name);
        if (pProgress != nullptr) {
            if (pLabel != nullptr) {
                pLabel->SetText(ui::StringUtil::Printf("%d%%", (int32_t)m_nProgressValue));                
            }
            pProgress->SetValue(m_nProgressValue);
        }
    }

    m_nProgressValue += 0.4;
    if (m_nProgressValue > 100.0) {
        m_nProgressValue = 0.0;
    }
}

