#include "dui/Control/DateTimeWnd_SDL.h"
#include "dui/Control/DateTime.h"
#include "dui/Control/RichEdit.h"
#include "dui/Control/Label.h"
#include "dui/Control/Button.h"
#include "dui/Box/VBox.h"
#include "dui/Core/GlobalManager.h"

#if defined(DUI_BUILD_FOR_SDL) || defined(DUI_BUILD_FOR_WAYLAND)

namespace ui
{
DateTimeWnd::DateTimeWnd(DateTime* pOwner):
    m_pOwner(pOwner),
    m_bInited(false),
    m_editFormat(-1),
    m_pSpinBox(nullptr)
{
}
DateTimeWnd::~DateTimeWnd()
{
}

bool DateTimeWnd::Init(DateTime* pOwner)
{
    ASSERT(m_pOwner == pOwner);
    if (pOwner == nullptr) {
        return false;
    }
    m_pOwner = pOwner;
    DateTime::EditFormat editFormat = pOwner->GetEditFormat();
    if (m_bInited && (m_editFormat == (int8_t)editFormat)) {
        //No need to initialize repeatedly
        return true;
    }
    Clear();

    //The separator of the year, month and day
    DString sep;
    sep = pOwner->GetDateSeparator();

    m_editList.resize(EditType::COUNT, nullptr);
    switch (editFormat)
    {
    case DateTime::EditFormat::kDateTimeUpDown:
        //Year-month-day hour:minute:second
        m_editList[EditType::YEAR] = CreateEditYear();
        pOwner->AddItem(m_editList[EditType::YEAR]);
        pOwner->AddItem(CreateLabel(sep));
        m_editList[EditType::MON] = CreateEditMon();
        pOwner->AddItem(m_editList[EditType::MON]);
        pOwner->AddItem(CreateLabel(sep));
        m_editList[EditType::MDAY] = CreateEditMDay();
        pOwner->AddItem(m_editList[EditType::MDAY]);
        pOwner->AddItem(CreateLabel(_T(" ")));
        m_editList[EditType::HOUR] = CreateEditHour();
        pOwner->AddItem(m_editList[EditType::HOUR]);
        pOwner->AddItem(CreateLabel(_T(":")));
        m_editList[EditType::MIN] = CreateEditMin();
        pOwner->AddItem(m_editList[EditType::MIN]);
        pOwner->AddItem(CreateLabel(_T(":")));
        m_editList[EditType::SEC] = CreateEditSec();
        pOwner->AddItem(m_editList[EditType::SEC]);
        break;
    case DateTime::EditFormat::kDateMinuteUpDown:
        //Year-month-day hour:minute
        m_editList[EditType::YEAR] = CreateEditYear();
        pOwner->AddItem(m_editList[EditType::YEAR]);
        pOwner->AddItem(CreateLabel(sep));
        m_editList[EditType::MON] = CreateEditMon();
        pOwner->AddItem(m_editList[EditType::MON]);
        pOwner->AddItem(CreateLabel(sep));
        m_editList[EditType::MDAY] = CreateEditMDay();
        pOwner->AddItem(m_editList[EditType::MDAY]);
        m_editList[EditType::HOUR] = CreateEditHour();
        pOwner->AddItem(m_editList[EditType::HOUR]);
        pOwner->AddItem(CreateLabel(_T(":")));
        m_editList[EditType::MIN] = CreateEditMin();
        pOwner->AddItem(m_editList[EditType::MIN]);
        break;
    case DateTime::EditFormat::kTimeUpDown:
        //Hour:minute:second
        m_editList[EditType::HOUR] = CreateEditHour();
        pOwner->AddItem(m_editList[EditType::HOUR]);
        pOwner->AddItem(CreateLabel(_T(":")));
        m_editList[EditType::MIN] = CreateEditMin();
        pOwner->AddItem(m_editList[EditType::MIN]);
        pOwner->AddItem(CreateLabel(_T(":")));
        m_editList[EditType::SEC] = CreateEditSec();
        pOwner->AddItem(m_editList[EditType::SEC]);
        break;
    case DateTime::EditFormat::kMinuteUpDown:
        //Hour:minute
        m_editList[EditType::HOUR] = CreateEditHour();
        pOwner->AddItem(m_editList[EditType::HOUR]);
        pOwner->AddItem(CreateLabel(_T(":")));
        m_editList[EditType::MIN] = CreateEditMin();
        pOwner->AddItem(m_editList[EditType::MIN]);
        break;
    default:
        //Year-month-day
        m_editList[EditType::YEAR] = CreateEditYear();
        pOwner->AddItem(m_editList[EditType::YEAR]);
        pOwner->AddItem(CreateLabel(sep));
        m_editList[EditType::MON] = CreateEditMon();
        pOwner->AddItem(m_editList[EditType::MON]);
        pOwner->AddItem(CreateLabel(sep));
        m_editList[EditType::MDAY] = CreateEditMDay();
        pOwner->AddItem(m_editList[EditType::MDAY]);
        break;
    }

    //Set the common attributes
    for (RichEdit* pRichEdit : m_editList) {
        if (pRichEdit != nullptr) {
            pRichEdit->SetAttribute(_T("text_align"), _T("vcenter,hcenter"));
            pRichEdit->SetAttribute(_T("number_only"), _T("true"));
            pRichEdit->SetAttribute(_T("height"), _T("100%"));
            pRichEdit->SetAttribute(_T("width"), _T("auto"));
            pRichEdit->SetAttribute(_T("text_padding"), _T("2,1,2,1"));
            pRichEdit->SetAttribute(_T("hide_selection"), _T("true"));
            pRichEdit->SetAttribute(_T("want_tab"), _T("false"));
            pRichEdit->SetAttribute(_T("want_return_msg"), _T("false"));
            pRichEdit->SetBkColor(_T("white"));
            pRichEdit->SetTabStop(true);
        }
    }

    //Right-side fill
    Control* pEmpty = new Control(m_pOwner->GetWindow());
    pEmpty->SetAttribute(_T("width"), _T("100%"));
    pEmpty->SetAttribute(_T("height"), _T("100%"));
    pEmpty->SetBkColor(_T("white"));
    pEmpty->SetTabStop(false);
    pEmpty->SetNoFocus();
    pEmpty->SetMouseEnabled(false);
    pOwner->AddItem(pEmpty);

    //Initialize the date value    
    if (!pOwner->IsValidDateTime()) {
        pOwner->InitLocalTime();
    }
    const struct tm& timeValue = pOwner->GetDateTime();
    if (m_editList[EditType::YEAR] != nullptr) {
        m_editList[EditType::YEAR]->SetText(StringUtil::Printf(_T("%d"), timeValue.tm_year + 1900));
    }
    if (m_editList[EditType::MON] != nullptr) {
        m_editList[EditType::MON]->SetText(StringUtil::Printf(_T("%02d"), timeValue.tm_mon + 1));
    }
    if (m_editList[EditType::MDAY] != nullptr) {
        m_editList[EditType::MDAY]->SetText(StringUtil::Printf(_T("%02d"), timeValue.tm_mday));
    }
    if (m_editList[EditType::HOUR] != nullptr) {
        m_editList[EditType::HOUR]->SetText(StringUtil::Printf(_T("%02d"), timeValue.tm_hour));
    }
    if (m_editList[EditType::MIN] != nullptr) {
        m_editList[EditType::MIN]->SetText(StringUtil::Printf(_T("%02d"), timeValue.tm_min));
    }
    if (m_editList[EditType::SEC] != nullptr) {
        m_editList[EditType::SEC]->SetText(StringUtil::Printf(_T("%02d"), timeValue.tm_sec > 59 ? 59 : timeValue.tm_sec)); //May be 60 or 61, leap second, change to display 59
    }
    m_editFormat = (int8_t)editFormat;
    m_bInited = true;

    //Attach events to control the day range (leap year, etc.)
    if ((m_editList[EditType::YEAR] != nullptr) && (m_editList[EditType::MON] != nullptr) && (m_editList[EditType::MDAY] != nullptr)) {
        m_editList[EditType::MON]->AttachTextChanged([this](const EventArgs&) {
            UpdateDayRange();
            return true;
            });
    }

    for (size_t index = 0; index < m_editList.size(); ++index) {
        RichEdit* pRichEdit = m_editList[index];
        if (pRichEdit == nullptr) {
            continue;
        }
        pRichEdit->AttachReturn([this](const EventArgs& /*args*/) {
            //After pressing Enter, end the editing
            EndEditDateTime();
            return true;
            });
    }

    //Spin feature
    DString spinClass = pOwner->GetSpinClass();
    if (!spinClass.empty()) {
        SetSpinClass(spinClass);
    }

    //Attach the focus loss event
    const size_t nItemCount = pOwner->GetItemCount();
    for (size_t nItem = 0; nItem < nItemCount; ++nItem) {
        Control* pControl = pOwner->GetItemAt(nItem);
        if (pControl == nullptr) {
            continue;
        }
        pControl->AttachKillFocus([this, pControl](const EventArgs& args) {
            OnKillFocusEvent(pControl, (Control*)args.wParam);
            return true;
            });
    }

    //Set the focus to the edit box
    for (RichEdit* pRichEdit : m_editList) {
        if (pRichEdit != nullptr) {
            pRichEdit->SetFocus();
            int32_t nTextLen = pRichEdit->GetTextLength();
            pRichEdit->SetSel(nTextLen, nTextLen);
            break;
        }
    }
    return true;
}

void DateTimeWnd::OnKillFocusEvent(Control* /*pControl*/, Control* pNewFocus)
{
    if ((pNewFocus == nullptr) || (m_pOwner == nullptr) || (m_pOwner == pNewFocus)) {
        return;
    }
    if (m_pOwner->GetItemIndex(pNewFocus) != Box::InvalidIndex) {
        return;
    }
    //The focus has switched to another control, end the editing
    EndEditDateTime();
}

void DateTimeWnd::EndEditDateTime()
{
    if (m_pOwner == nullptr) {
        return;
    }
    if (!m_pOwner->IsValidDateTime()) {
        m_pOwner->InitLocalTime();
    }
    DateTime::EditFormat editFormat = m_pOwner->GetEditFormat();
    if (m_editFormat != (int8_t)editFormat) {
        return;
    }
    //Validate the day range
    UpdateDayRange();
    bool bYear = false;
    bool bMonth = false;
    bool bMDay = false;
    bool bHour = false;
    bool bMin = false;
    bool bSec = false;

    struct tm timeValue = m_pOwner->GetDateTime();
    switch (editFormat)
    {
    case DateTime::EditFormat::kDateTimeUpDown:
        //Year-month-day hour:minute:second
        bYear = true;
        bMonth = true;
        bMDay = true;
        bHour = true;
        bMin = true;
        bSec = true;
        break;
    case DateTime::EditFormat::kDateMinuteUpDown:
        //Year-month-day hour:minute
        bYear = true;
        bMonth = true;
        bMDay = true;
        bHour = true;
        bMin = true;
        break;
    case DateTime::EditFormat::kTimeUpDown:
        //Hour:minute:second
        bHour = true;
        bMin = true;
        bSec = true;
        break;
    case DateTime::EditFormat::kMinuteUpDown:
        //Hour:minute
        bHour = true;
        bMin = true;
        break;
    default:
        //Year-month-day
        bYear = true;
        bMonth = true;
        bMDay = true;
        break;
    }

    if (bYear && (m_editList[EditType::YEAR] != nullptr)) {
        int32_t nYear = StringUtil::StringToInt32(m_editList[EditType::YEAR]->GetText());
        if (nYear >= 1900) {
            timeValue.tm_year = nYear - 1900;
        }
    }
    if (bMonth && (m_editList[EditType::MON] != nullptr)) {
        int32_t nMonth = StringUtil::StringToInt32(m_editList[EditType::MON]->GetText());
        if ((nMonth >= 1) && (nMonth <= 12)) {
            timeValue.tm_mon = nMonth - 1;
        }
    }
    if (bMDay && (m_editList[EditType::MDAY] != nullptr)) {
        int32_t nDay = StringUtil::StringToInt32(m_editList[EditType::MDAY]->GetText());
        if ((nDay >= 1) && (nDay <= 31)) {
            timeValue.tm_mday = nDay;
        }
    }
    if (bHour && (m_editList[EditType::HOUR] != nullptr)) {
        int32_t nHour = StringUtil::StringToInt32(m_editList[EditType::HOUR]->GetText());
        if ((nHour >= 0) && (nHour <= 23)) {
            timeValue.tm_hour = nHour;
        }
    }
    if (bMin && (m_editList[EditType::MIN] != nullptr)) {
        int32_t nMin = StringUtil::StringToInt32(m_editList[EditType::MIN]->GetText());
        if ((nMin >= 0) && (nMin <= 59)) {
            timeValue.tm_min = nMin;
        }
    }
    if (bSec && (m_editList[EditType::SEC] != nullptr)) {
        int32_t nSec = StringUtil::StringToInt32(m_editList[EditType::SEC]->GetText());
        if ((nSec >= 0) && (nSec <= 59)) {
            timeValue.tm_min = nSec;
        }
    }

    Clear();
    m_pOwner->SetDateTime(timeValue);
    m_pOwner->EndEditDateTime();
}

RichEdit* DateTimeWnd::CreateEditYear() const
{
    RichEdit* pRichEdit = new RichEdit(m_pOwner->GetWindow());    
    pRichEdit->SetAttribute(_T("limit_text"), _T("4"));
    pRichEdit->SetAttribute(_T("number_format"), _T("%04I64d"));
    pRichEdit->SetText(L"2024");
    pRichEdit->SetMinNumber(1900);
    return pRichEdit;
}

RichEdit* DateTimeWnd::CreateEditMon() const
{
    RichEdit* pRichEdit = new RichEdit(m_pOwner->GetWindow());
    pRichEdit->SetAttribute(_T("limit_text"), _T("2"));
    pRichEdit->SetAttribute(_T("number_format"), _T("%02I64d"));
    pRichEdit->SetText(L"01");
    pRichEdit->SetMinNumber(1);
    pRichEdit->SetMaxNumber(12);
    return pRichEdit;
}

RichEdit* DateTimeWnd::CreateEditMDay() const
{
    RichEdit* pRichEdit = new RichEdit(m_pOwner->GetWindow());
    pRichEdit->SetAttribute(_T("limit_text"), _T("2"));
    pRichEdit->SetAttribute(_T("number_format"), _T("%02I64d"));
    pRichEdit->SetText(L"01");
    pRichEdit->SetMinNumber(1);
    pRichEdit->SetMaxNumber(31);
    return pRichEdit;
}

RichEdit* DateTimeWnd::CreateEditHour() const
{
    RichEdit* pRichEdit = new RichEdit(m_pOwner->GetWindow());
    pRichEdit->SetAttribute(_T("limit_text"), _T("2"));
    pRichEdit->SetAttribute(_T("number_format"), _T("%02I64d"));
    pRichEdit->SetText(L"00");
    pRichEdit->SetMinNumber(0);
    pRichEdit->SetMaxNumber(23);
    return pRichEdit;
}

RichEdit* DateTimeWnd::CreateEditMin() const
{
    RichEdit* pRichEdit = new RichEdit(m_pOwner->GetWindow());
    pRichEdit->SetAttribute(_T("limit_text"), _T("2"));
    pRichEdit->SetAttribute(_T("number_format"), _T("%02I64d"));
    pRichEdit->SetText(L"00");
    pRichEdit->SetMinNumber(0);
    pRichEdit->SetMaxNumber(59);
    return pRichEdit;
}

RichEdit* DateTimeWnd::CreateEditSec() const
{
    RichEdit* pRichEdit = new RichEdit(m_pOwner->GetWindow());
    pRichEdit->SetAttribute(_T("limit_text"), _T("2"));
    pRichEdit->SetAttribute(_T("number_format"), _T("%02I64d"));
    pRichEdit->SetText(L"00");
    pRichEdit->SetMinNumber(0);
    pRichEdit->SetMaxNumber(59);
    return pRichEdit;
}

Control* DateTimeWnd::CreateLabel(const DString& text) const
{
    Label* pLabel = new Label(m_pOwner->GetWindow());
    pLabel->SetText(text);
    pLabel->SetAttribute(_T("text_align"), _T("vcenter,hcenter"));
    pLabel->SetAttribute(_T("width"), _T("auto"));
    pLabel->SetAttribute(_T("min_width"), _T("4"));
    pLabel->SetAttribute(_T("height"), _T("100%"));
    pLabel->SetBkColor(_T("white"));
    pLabel->SetTabStop(false);
    pLabel->SetNoFocus();
    pLabel->SetMouseEnabled(false);
    return pLabel;
}

void DateTimeWnd::UpdateDayRange()
{
    if ((m_editList[EditType::YEAR] != nullptr) && (m_editList[EditType::MON] != nullptr) && (m_editList[EditType::MDAY] != nullptr)) {
        int32_t nYear = StringUtil::StringToInt32(m_editList[EditType::YEAR]->GetText());
        int32_t nMonth = StringUtil::StringToInt32(m_editList[EditType::MON]->GetText());
        if ((nYear >= 1900) && (nMonth >= 1) && (nMonth <= 12)) {
            switch (nMonth)
            {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                m_editList[EditType::MDAY]->SetMaxNumber(31);
                break;
            case 2:
                if ((nYear % 400 == 0) || ((nYear % 4 == 0) && (nYear % 100 != 0))) {
                    //Leap year
                    m_editList[EditType::MDAY]->SetMaxNumber(29);
                }
                else {
                    m_editList[EditType::MDAY]->SetMaxNumber(28);
                }
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                m_editList[EditType::MDAY]->SetMaxNumber(31);
                break;
            default:
                break;
            }
        }

        int32_t nDay = StringUtil::StringToInt32(m_editList[EditType::MDAY]->GetText());
        if (nDay < m_editList[EditType::MDAY]->GetMinNumber()) {
            m_editList[EditType::MDAY]->SetText(StringUtil::Printf(_T("%02d"), m_editList[EditType::MDAY]->GetMinNumber()));
        }
        if (nDay > m_editList[EditType::MDAY]->GetMaxNumber()) {
            m_editList[EditType::MDAY]->SetText(StringUtil::Printf(_T("%02d"), m_editList[EditType::MDAY]->GetMaxNumber()));
        }
    }
}

void DateTimeWnd::UpdateWndPos()
{
    //No implementation needed
}

void DateTimeWnd::ShowWindow()
{
    //No implementation needed
}

void DateTimeWnd::Clear()
{
    m_flagAdjustTextNumber.Cancel();
    if (m_pOwner != nullptr) {
        m_pOwner->RemoveAllItems();
    }
    m_pSpinBox = nullptr;
    m_editList.clear();
    m_bInited = false;
    m_editFormat = -1;
}

bool DateTimeWnd::SetSpinClass(const DString& spinClass)
{
    ASSERT(m_pOwner != nullptr);
    if (m_pOwner == nullptr) {
        return false;
    }
    DString spinBoxClass;
    DString spinBtnUpClass;
    DString spinBtnDownClass;
    std::list<DString> classNames = StringUtil::Split(spinClass, _T(","));
    if (classNames.size() == 3) {
        auto iter = classNames.begin();
        spinBoxClass = *iter++;
        spinBtnUpClass = *iter++;
        spinBtnDownClass = *iter++;
    }
    if (!spinClass.empty()) {
        ASSERT(!spinBoxClass.empty() && !spinBtnUpClass.empty() && !spinBtnDownClass.empty());
    }
    if (!spinBoxClass.empty() && !spinBtnUpClass.empty() && !spinBtnDownClass.empty()) {
        Button* pUpButton = nullptr;
        Button* pDownButton = nullptr;
        if (m_pSpinBox == nullptr) {
            m_pSpinBox = new VBox(m_pOwner->GetWindow());
            m_pOwner->AddItem(m_pSpinBox);

            pUpButton = new Button(m_pOwner->GetWindow());
            m_pSpinBox->AddItem(pUpButton);

            pDownButton = new Button(m_pOwner->GetWindow());
            m_pSpinBox->AddItem(pDownButton);
        }
        else {
            pUpButton = dynamic_cast<Button*>(m_pSpinBox->GetItemAt(0));
            pDownButton = dynamic_cast<Button*>(m_pSpinBox->GetItemAt(1));
        }

        ASSERT((pUpButton != nullptr) && (pDownButton != nullptr));
        if ((pUpButton == nullptr) || (pDownButton == nullptr)) {
            m_pOwner->RemoveItem(m_pSpinBox);
            m_pSpinBox = nullptr;
            return false;
        }
        m_pSpinBox->SetClass(spinBoxClass);
        pUpButton->SetClass(spinBtnUpClass);
        pDownButton->SetClass(spinBtnDownClass);

        //Attach event handlers
        pUpButton->DetachEvent(kEventClick);
        pUpButton->AttachClick([this](const EventArgs& /*args*/) {
            AdjustTextNumber(1);
            return true;
            });

        pUpButton->DetachEvent(kEventMouseButtonDown);
        pUpButton->AttachButtonDown([this](const EventArgs& /*args*/) {
            StartAutoAdjustTextNumberTimer(1);
            return true;
            });

        pUpButton->DetachEvent(kEventMouseButtonUp);
        pUpButton->AttachButtonUp([this](const EventArgs& /*args*/) {
            StopAutoAdjustTextNumber();
            return true;
            });

        pUpButton->DetachEvent(kEventMouseLeave);
        pUpButton->AttachMouseLeave([this](const EventArgs& /*args*/) {
            StopAutoAdjustTextNumber();
            return true;
            });

        pDownButton->DetachEvent(kEventClick);
        pDownButton->AttachClick([this](const EventArgs& /*args*/) {
            StopAutoAdjustTextNumber();
            AdjustTextNumber(-1);
            return true;
            });

        pDownButton->DetachEvent(kEventMouseButtonDown);
        pDownButton->AttachButtonDown([this](const EventArgs& /*args*/) {
            StartAutoAdjustTextNumberTimer(-1);
            return true;
            });

        pDownButton->DetachEvent(kEventMouseButtonUp);
        pDownButton->AttachButtonUp([this](const EventArgs& /*args*/) {
            StopAutoAdjustTextNumber();
            return true;
            });

        pDownButton->DetachEvent(kEventMouseLeave);
        pDownButton->AttachMouseLeave([this](const EventArgs& /*args*/) {
            StopAutoAdjustTextNumber();
            return true;
            });
        return true;
    }
    else {
        if (m_pSpinBox != nullptr) {
            m_pOwner->RemoveItem(m_pSpinBox);
            m_pSpinBox = nullptr;
        }
    }
    return false;
}

void DateTimeWnd::StartAutoAdjustTextNumberTimer(int32_t nDelta)
{
    if (nDelta != 0) {
        //Start the timer
        m_flagAdjustTextNumber.Cancel();
        std::function<void()> closure = UiBind(&DateTimeWnd::StartAutoAdjustTextNumber, this, nDelta);
        GlobalManager::Instance().Timer().AddTimer(m_flagAdjustTextNumber.GetWeakFlag(), closure, 1000, 1);
    }
}

void DateTimeWnd::StartAutoAdjustTextNumber(int32_t nDelta)
{
    if (nDelta != 0) {
        //Start the timer
        m_flagAdjustTextNumber.Cancel();
        std::function<void()> closure = UiBind(&DateTimeWnd::AdjustTextNumber, this, nDelta);
        GlobalManager::Instance().Timer().AddTimer(m_flagAdjustTextNumber.GetWeakFlag(), closure, 120);
    }
}

void DateTimeWnd::StopAutoAdjustTextNumber()
{
    m_flagAdjustTextNumber.Cancel();
}

void DateTimeWnd::AdjustTextNumber(int32_t nDelta)
{
    for (size_t index = 0; index < m_editList.size(); ++index) {
        RichEdit* pRichEdit = m_editList[index];
        if ((pRichEdit == nullptr) || !pRichEdit->IsFocused()) {
            continue;
        }
        pRichEdit->AdjustTextNumber(nDelta);
        break;
    }
}

} //namespace ui

#endif // DUI_BUILD_FOR_SDL

