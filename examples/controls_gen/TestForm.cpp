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
    ui::VBox* pRoot = new ui::VBox(this);
    pRoot->SetBkColor(_T("white"));

    ui::HBox* pHeader = new ui::HBox(this);
    pHeader->SetAttribute(_T("height"), _T("30"));
    pHeader->SetAttribute(_T("margin"), _T("0,10,0,0"));
    pRoot->AddItem(pHeader);

    ui::Label* pTitle = new ui::Label(this);
    pTitle->SetText(_T("Current Progress:"));
    pHeader->AddItem(pTitle);

    ui::Label* pProgressText = new ui::Label(this);
    pProgressText->SetName(_T("progress_text"));
    pProgressText->SetText(_T("0%"));
    pHeader->AddItem(pProgressText);

    ui::HBox* pBody = new ui::HBox(this);
    pBody->SetAttribute(_T("margin"), _T("0,10,0,0"));
    pRoot->AddItem(pBody);

    // Horizontal progress bar
    ui::VBox* pLeftCol = new ui::VBox(this);
    pLeftCol->SetAttribute(_T("valign"), _T("center"));
    pLeftCol->SetAttribute(_T("halign"), _T("center"));
    pLeftCol->SetAttribute(_T("width"), _T("50%"));
    pLeftCol->SetAttribute(_T("height"), _T("100%"));
    pBody->AddItem(pLeftCol);

    ui::Progress* pProgress11 = new ui::Progress(this);
    pProgress11->SetClass(_T("progress_horizontal_blue"));
    pProgress11->SetName(_T("progress11"));
    pProgress11->SetAttribute(_T("reverse"), _T("false"));
    pProgress11->SetAttribute(_T("height"), _T("6"));
    pProgress11->SetAttribute(_T("width"), _T("stretch"));
    pProgress11->SetAttribute(_T("min"), _T("0"));
    pProgress11->SetAttribute(_T("max"), _T("100"));
    pProgress11->SetAttribute(_T("value"), _T("0"));
    pProgress11->SetAttribute(_T("margin"), _T("20,20,20,20"));
    pProgress11->SetToolTipText(_T("ui::Progress"));
    pLeftCol->AddItem(pProgress11);

    ui::Progress* pProgress12 = new ui::Progress(this);
    pProgress12->SetAttribute(_T("progress_color"), _T("blue"));
    pProgress12->SetName(_T("progress12"));
    pProgress12->SetAttribute(_T("reverse"), _T("false"));
    pProgress12->SetAttribute(_T("height"), _T("6"));
    pProgress12->SetAttribute(_T("width"), _T("stretch"));
    pProgress12->SetAttribute(_T("min"), _T("0"));
    pProgress12->SetAttribute(_T("max"), _T("100"));
    pProgress12->SetAttribute(_T("value"), _T("0"));
    pProgress12->SetAttribute(_T("marquee"), _T("false"));
    pProgress12->SetAttribute(_T("margin"), _T("20,20,20,20"));
    pProgress12->SetBkColor(_T("LightGray"));
    pProgress12->SetToolTipText(_T("ui::Progress"));
    pLeftCol->AddItem(pProgress12);

    ui::Slider* pProgress13 = new ui::Slider(this);
    pProgress13->SetClass(_T("slider_horizontal_green"));
    pProgress13->SetName(_T("progress13"));
    pProgress13->SetAttribute(_T("reverse"), _T("false"));
    pProgress13->SetAttribute(_T("height"), _T("14"));
    pProgress13->SetAttribute(_T("width"), _T("stretch"));
    pProgress13->SetAttribute(_T("min"), _T("0"));
    pProgress13->SetAttribute(_T("max"), _T("100"));
    pProgress13->SetAttribute(_T("value"), _T("0"));
    pProgress13->SetAttribute(_T("progress_bar_padding"), _T("0,4,0,4"));
    pProgress13->SetAttribute(_T("margin"), _T("20,20,20,20"));
    pProgress13->SetToolTipText(_T("ui::Slider"));
    pLeftCol->AddItem(pProgress13);

    ui::CircleProgress* pProgress14 = new ui::CircleProgress(this);
    pProgress14->SetName(_T("progress14"));
    pProgress14->SetAttribute(_T("reverse"), _T("false"));
    pProgress14->SetAttribute(_T("circular"), _T("true"));
    pProgress14->SetAttribute(_T("height"), _T("80"));
    pProgress14->SetAttribute(_T("width"), _T("80"));
    pProgress14->SetAttribute(_T("circle_width"), _T("12"));
    pProgress14->SetAttribute(_T("bgcolor"), _T("gray"));
    pProgress14->SetAttribute(_T("fgcolor"), _T("green"));
    pProgress14->SetAttribute(_T("gradient_color"), _T("red"));
    pProgress14->SetAttribute(_T("clockwise"), _T("true"));
    pProgress14->SetAttribute(_T("min"), _T("0"));
    pProgress14->SetAttribute(_T("max"), _T("100"));
    pProgress14->SetAttribute(_T("value"), _T("0"));
    pProgress14->SetAttribute(_T("margin"), _T("20,20,20,20"));
    pProgress14->SetAttribute(_T("text_padding"), _T("10,32,10,10"));
    pProgress14->SetAttribute(_T("normal_text_color"), _T("darkcolor"));
    pProgress14->SetAttribute(_T("indicator"), _T("file='public/progress/indicator.svg' width='12' height='12'"));
    pProgress14->SetToolTipText(_T("ui::CircleProgress"));
    pLeftCol->AddItem(pProgress14);

    // Vertical progress bar
    ui::HBox* pRightCol = new ui::HBox(this);
    pRightCol->SetAttribute(_T("valign"), _T("center"));
    pRightCol->SetAttribute(_T("halign"), _T("center"));
    pRightCol->SetAttribute(_T("width"), _T("50%"));
    pRightCol->SetAttribute(_T("height"), _T("100%"));
    pBody->AddItem(pRightCol);

    ui::Progress* pProgress21 = new ui::Progress(this);
    pProgress21->SetClass(_T("progress_vertical_blue"));
    pProgress21->SetName(_T("progress21"));
    pProgress21->SetAttribute(_T("reverse"), _T("false"));
    pProgress21->SetAttribute(_T("height"), _T("stretch"));
    pProgress21->SetAttribute(_T("width"), _T("6"));
    pProgress21->SetAttribute(_T("min"), _T("0"));
    pProgress21->SetAttribute(_T("max"), _T("100"));
    pProgress21->SetAttribute(_T("value"), _T("0"));
    pProgress21->SetAttribute(_T("horizontal"), _T("false"));
    pProgress21->SetAttribute(_T("margin"), _T("20,20,20,20"));
    pProgress21->SetToolTipText(_T("ui::Progress"));
    pRightCol->AddItem(pProgress21);

    ui::Progress* pProgress22 = new ui::Progress(this);
    pProgress22->SetAttribute(_T("progress_color"), _T("blue"));
    pProgress22->SetName(_T("progress22"));
    pProgress22->SetAttribute(_T("reverse"), _T("false"));
    pProgress22->SetAttribute(_T("height"), _T("stretch"));
    pProgress22->SetAttribute(_T("width"), _T("6"));
    pProgress22->SetAttribute(_T("min"), _T("0"));
    pProgress22->SetAttribute(_T("max"), _T("100"));
    pProgress22->SetAttribute(_T("value"), _T("0"));
    pProgress22->SetAttribute(_T("horizontal"), _T("false"));
    pProgress22->SetAttribute(_T("margin"), _T("20,20,20,20"));
    pProgress22->SetBkColor(_T("LightGray"));
    pProgress22->SetToolTipText(_T("ui::Progress"));
    pRightCol->AddItem(pProgress22);

    ui::Slider* pProgress23 = new ui::Slider(this);
    pProgress23->SetClass(_T("slider_vertical_green"));
    pProgress23->SetName(_T("progress23"));
    pProgress23->SetAttribute(_T("reverse"), _T("false"));
    pProgress23->SetAttribute(_T("height"), _T("stretch"));
    pProgress23->SetAttribute(_T("width"), _T("14"));
    pProgress23->SetAttribute(_T("min"), _T("0"));
    pProgress23->SetAttribute(_T("max"), _T("100"));
    pProgress23->SetAttribute(_T("value"), _T("0"));
    pProgress23->SetAttribute(_T("progress_bar_padding"), _T("4,0,4,0"));
    pProgress23->SetAttribute(_T("margin"), _T("20,20,20,20"));
    pProgress23->SetAttribute(_T("horizontal"), _T("false"));
    pProgress23->SetToolTipText(_T("ui::Slider"));
    pRightCol->AddItem(pProgress23);

    ui::CircleProgress* pProgress24 = new ui::CircleProgress(this);
    pProgress24->SetName(_T("progress24"));
    pProgress24->SetAttribute(_T("reverse"), _T("false"));
    pProgress24->SetAttribute(_T("circular"), _T("true"));
    pProgress24->SetAttribute(_T("height"), _T("80"));
    pProgress24->SetAttribute(_T("width"), _T("80"));
    pProgress24->SetAttribute(_T("horizontal"), _T("false"));
    pProgress24->SetAttribute(_T("circle_width"), _T("12"));
    pProgress24->SetAttribute(_T("bgcolor"), _T("gray"));
    pProgress24->SetAttribute(_T("fgcolor"), _T("green"));
    pProgress24->SetAttribute(_T("gradient_color"), _T("red"));
    pProgress24->SetAttribute(_T("clockwise"), _T("true"));
    pProgress24->SetAttribute(_T("min"), _T("0"));
    pProgress24->SetAttribute(_T("max"), _T("100"));
    pProgress24->SetAttribute(_T("value"), _T("0"));
    pProgress24->SetAttribute(_T("margin"), _T("20,20,20,20"));
    pProgress24->SetAttribute(_T("text_padding"), _T("10,32,10,10"));
    pProgress24->SetAttribute(_T("normal_text_color"), _T("darkcolor"));
    pProgress24->SetAttribute(_T("indicator"), _T("file='public/progress/indicator.svg' width='12' height='12'"));
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

