#include "duilib/Core/ControlLoading.h"
#include "duilib/Core/Box.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Render/IRender.h"
#include "duilib/Utils/AttributeUtil.h"

namespace ui 
{
//Attributes
class LoadingAttribute
{
public:
    //The XML file name, including the relative path
    UiString m_sXmlPath;

    //The name of the animation control
    UiString m_sAnimationControlName;

    //Position offset (X direction)
    int32_t m_nOffsetX = -1;

    //Position offset (X direction)
    int32_t m_nOffsetY = -1;

    //Set the width, e.g. 300
    int32_t m_nWidth = 0;

    //Set the height, e.g. 200
    int32_t m_nHeight = 0;

    //The horizontal alignment in the target area (valid only when m_nOffsetX is not specified)
    UiString m_hAlign;

    //The vertical alignment in the target area (valid only when m_nOffsetY is not specified)
    UiString m_vAlign;

    //Transparency (0 - 255)
    uint8_t m_bFade = 255;

    //Automatically stop loading when the animation ends
    bool m_bAutoStopWhenAnimationEnd = false;
};

ControlLoading::ControlLoading(Control* pControl):
    m_bIsLoading(false),
    m_nIntervalMs(0),
    m_nCallbackCount(0),
    m_nMaxCount(-1),
    m_pControl(pControl),
    m_pAnimationControl(nullptr)
{
    ASSERT(pControl != nullptr);
    m_pLoadingAttribute = std::make_unique<LoadingAttribute>();
}

ControlLoading::~ControlLoading()
{
}

bool ControlLoading::SetLoadingAttribute(const DString& loadingAttribute)
{
    ASSERT(!m_bIsLoading);
    if (m_bIsLoading) {
        return false;
    }
    return InitAttribute(*m_pLoadingAttribute, loadingAttribute);
}

bool ControlLoading::InitAttribute(LoadingAttribute& loadingAttribute, const DString& loadingString)
{
    loadingAttribute = LoadingAttribute();
    if (loadingString.find(_T('=')) == DString::npos) {
        //No equals sign, meaning there are no attributes, return directly
        return false;
    }
    std::vector<std::pair<DString, DString>> attributeList;
    AttributeUtil::ParseAttributeList(loadingString, _T('\''), attributeList);
    for (const auto& attribute : attributeList) {
        const DString& name = attribute.first;
        const DString& value = attribute.second;
        if (name.empty() || value.empty()) {
            continue;
        }
        if (name == _T("file")) {
            //The XML resource file name, used to load the XML resource which is finally displayed in a Box container
            loadingAttribute.m_sXmlPath = value;
        }
        else if (name == _T("width")) {
            //Set the width
            loadingAttribute.m_nWidth = StringUtil::StringToInt32(value);
        }
        else if (name == _T("height")) {
            //Set the height
            loadingAttribute.m_nHeight = StringUtil::StringToInt32(value);
        }
        else if (name == _T("offset_x")) {
            //Set the X offset
            loadingAttribute.m_nOffsetX = StringUtil::StringToInt32(value);
        }
        else if (name == _T("offset_y")) {
            //Set the Y offset
            loadingAttribute.m_nOffsetY = StringUtil::StringToInt32(value);
        }
        else if (name == _T("halign")) {
            //Set the horizontal alignment in the target area
            ASSERT((value == _T("left")) || (value == _T("center")) || (value == _T("right")));
            if ((value == _T("left")) || (value == _T("center")) || (value == _T("right"))) {
                loadingAttribute.m_hAlign = value;
            }
        }
        else if (name == _T("valign")) {
            //Set the vertical alignment in the target area
            ASSERT((value == _T("top")) || (value == _T("center")) || (value == _T("bottom")));
            if ((value == _T("top")) || (value == _T("center")) || (value == _T("bottom"))) {
                loadingAttribute.m_vAlign = value;
            }
        }
        else if (name == _T("fade")) {
            //The transparency of the loading control
            loadingAttribute.m_bFade = (uint8_t)StringUtil::StringToInt32(value);
        }
        else if (name == _T("auto_stop")) {
            //After the loading animation finishes playing, automatically stop the loading status
            loadingAttribute.m_bAutoStopWhenAnimationEnd = value == _T("true");
        }
        else if (name == _T("animation_control")) {
            //The name of the animation control, used for interaction between the Loading feature and the animation control on the loading control
            loadingAttribute.m_sAnimationControlName = value;
        }
        else {
            ASSERT(!"ControlLoading::InitAttribute: fount unknown attribute!");
        }
    }
    return !loadingAttribute.m_sXmlPath.empty();
}

void ControlLoading::PaintLoading(IRender* pRender, const UiRect& rcPaint)
{
    ASSERT((pRender != nullptr) && (m_pControl != nullptr));
    if ((pRender == nullptr) || (m_pControl == nullptr)){
        return;
    }
    if (m_bIsLoading && (m_pLoadingBox != nullptr)) {
        if ((m_pLoadingBox->GetWidth() <= 0) || (m_pLoadingBox->GetHeight() <= 0)) {
            //Initialize the display position
            UpdateLoadingPos();
        }
        m_pLoadingBox->AlphaPaint(pRender, rcPaint);
    }
}

void ControlLoading::UpdateLoadingPos()
{
    if (!m_bIsLoading || (m_pLoadingBox == nullptr) || (m_pControl == nullptr)) {
        return;
    }
    //Initialize the display position
    int32_t nBoxWidth = 0;
    int32_t nBoxHeight = 0;
    if ((m_pLoadingAttribute->m_nWidth <= 0) || (m_pLoadingAttribute->m_nHeight <= 0)) {
        //Calculate the width and height
        int32_t nWidth = 0;
        int32_t nHeight = 0;
        UiFixedInt fixedHeight = m_pLoadingBox->GetFixedHeight();
        UiFixedInt fixedWidth = m_pLoadingBox->GetFixedWidth();
        if (fixedHeight.IsAuto() || fixedWidth.IsAuto()) {
            UiSize rc;
            rc.cx = m_pControl->GetRect().Width();
            rc.cy = m_pControl->GetRect().Height();
            UiEstSize size = m_pLoadingBox->EstimateSize(rc);
            if (fixedWidth.IsAuto()) {
                nWidth = size.cx.GetInt32();
            }
            if (fixedHeight.IsAuto()) {
                nHeight = size.cy.GetInt32();
            }
        }

        if ((nWidth <= 0) && fixedWidth.IsInt32()){
            nWidth = fixedWidth.GetInt32();
        }
        if ((nHeight <= 0) && fixedHeight.IsInt32()) {
            nHeight = fixedHeight.GetInt32();
        }
        if (m_pLoadingAttribute->m_nWidth <= 0) {
            nBoxWidth = nWidth;
        }
        else {
            nBoxWidth = m_pControl->Dpi().GetScaleInt(m_pLoadingAttribute->m_nWidth);
        }
        if (m_pLoadingAttribute->m_nHeight <= 0) {
            nBoxHeight = nHeight;
        }
        else {
            nBoxHeight = m_pControl->Dpi().GetScaleInt(m_pLoadingAttribute->m_nHeight);
        }
    }
    else {
        nBoxWidth = m_pControl->Dpi().GetScaleInt(m_pLoadingAttribute->m_nWidth);
        nBoxHeight = m_pControl->Dpi().GetScaleInt(m_pLoadingAttribute->m_nHeight);
    }

    if ((nBoxWidth <= 0) || (nBoxWidth > m_pControl->GetRect().Width())) {
        nBoxWidth = m_pControl->GetRect().Width();
    }
    if ((nBoxHeight <= 0) || (nBoxHeight > m_pControl->GetRect().Height())) {
        nBoxHeight = m_pControl->GetRect().Height();
    }

    const UiRect rect = m_pControl->GetRect();
    UiRect rcLoading = m_pControl->GetRect();
    const int32_t nOffsetX = m_pControl->Dpi().GetScaleInt(m_pLoadingAttribute->m_nOffsetX);
    if ((nOffsetX >= 0) && (nOffsetX < rect.Width())) {
        rcLoading.left = rect.left + nOffsetX;
    }
    else {
        if (m_pLoadingAttribute->m_hAlign == _T("left")) {
            //Align to the left
            rcLoading.left = rect.left;
        }
        else if (m_pLoadingAttribute->m_hAlign == _T("right")) {
            //Align to the right
            rcLoading.left = rect.right - nBoxWidth;
        }
        else {
            //Align to the center
            rcLoading.left = rect.left + (rect.Width() - nBoxWidth) / 2;
        }        
    }
    rcLoading.right = rcLoading.left + nBoxWidth;

    const int32_t nOffsetY = m_pControl->Dpi().GetScaleInt(m_pLoadingAttribute->m_nOffsetY);
    if ((nOffsetY >= 0) && (nOffsetY < rect.Height())) {
        rcLoading.top = rect.top + nOffsetY;
    }
    else {
        if (m_pLoadingAttribute->m_vAlign == _T("top")) {
            rcLoading.top = rect.top;
        }
        else if (m_pLoadingAttribute->m_vAlign == _T("bottom")) {
            rcLoading.top = rect.bottom - nBoxHeight;
        }
        else {
            //Align to the center
            rcLoading.top = rect.top + (rect.Height() - nBoxHeight) / 2;
        }
    }
    rcLoading.bottom = rcLoading.top + nBoxHeight;

    //Set the control position
    m_pLoadingBox->SetPos(rcLoading);
}

void ControlLoading::Loading()
{
    if (!m_bIsLoading) {
        return;
    }

    //Trigger the loading event
    std::weak_ptr<WeakFlag> weakFlag = GetWeakFlag();
    ControlPtrT<Control> pControl(m_pControl);
    ControlPtrT<Box> pLoadingUiRootBox(m_pLoadingBox.get());
    ControlPtrT<Control> pAnimationControl(m_pAnimationControl);

    ControlLoadingStatus status;
    status.m_pControl = pControl;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_startTime);
    status.m_nElapsedTimeMs = (uint64_t)microseconds.count();
    status.m_pLoadingUiRootBox = pLoadingUiRootBox;
    status.m_pAnimationControl = pAnimationControl;
    status.m_bStopLoading = false;
    m_pControl->SendEvent(kEventLoading, (WPARAM)&status);

    if (!weakFlag.expired() && m_bIsLoading) {
        ++m_nCallbackCount;
        bool bStopLoading = status.m_bStopLoading;
        if ((m_nMaxCount >= 0) && (m_nCallbackCount > m_nMaxCount)) {
            //The stop condition is reached
            bStopLoading = true;
        }
        if (bStopLoading) {
            if (m_pControl != nullptr) {
                m_pControl->StopLoading();
            }
        }
    }
}

bool ControlLoading::StartLoading(int32_t nIntervalMs, int32_t nMaxCount)
{
    if (m_bIsLoading) {
        StopLoading();
    }
    ASSERT(m_pControl != nullptr);
    if (m_pControl == nullptr) {
        return false;
    }

    m_pAnimationControl = nullptr;
    m_nCallbackCount = 0;
    if (m_pLoadingBox == nullptr) {        
        ASSERT(!m_pLoadingAttribute->m_sXmlPath.empty());
        if (m_pLoadingAttribute->m_sXmlPath.empty()) {
            return false;
        }
        Box* pLoadingBox = GlobalManager::Instance().CreateBox(m_pControl->GetWindow(), FilePath(m_pLoadingAttribute->m_sXmlPath.c_str()));
        ASSERT(pLoadingBox != nullptr);
        if (pLoadingBox == nullptr) {
            return false;
        }
        m_pLoadingBox.reset(pLoadingBox);
        if (m_pLoadingAttribute->m_bFade != 255) {
            m_pLoadingBox->SetAlpha(m_pLoadingAttribute->m_bFade);
        }
        if (!m_pLoadingAttribute->m_sAnimationControlName.empty()) {
            m_pAnimationControl = m_pControl->GetWindow()->FindSubControlByName(m_pLoadingBox.get(), m_pLoadingAttribute->m_sAnimationControlName.c_str());
        }
    }
    m_startTime = std::chrono::steady_clock::now();
    m_bIsLoading = true;
    m_nIntervalMs = nIntervalMs;
    if (m_nIntervalMs <= 0) {
        //Set the default value
        m_nIntervalMs = 50;
    }
    else if (m_nIntervalMs < 10) {
        //Limit the minimum value
        m_nIntervalMs = 10;
    }
    m_nMaxCount = nMaxCount;
    GlobalManager::Instance().Timer().AddTimer(m_loadingFlag.GetWeakFlag(),
                                               UiBind(&ControlLoading::Loading, this),
                                               m_nIntervalMs);

    //Automatically stop loading after the animation ends
    if ((m_pLoadingAttribute->m_bAutoStopWhenAnimationEnd) && (m_pAnimationControl != nullptr)) {
        ControlPtrT<Control> pControl(m_pControl);
        m_pAnimationControl->AttachImageAnimationStop([pControl](const EventArgs&) {
            if (pControl != nullptr) {
                pControl->StopLoading();
            }
            return true;
            });
    }

    //Trigger the start event
    ControlPtrT<Control> pControl(m_pControl);
    ControlPtrT<Box> pLoadingUiRootBox(m_pLoadingBox.get());
    ControlPtrT<Control> pAnimationControl(m_pAnimationControl);

    ControlLoadingStatus status;
    status.m_pControl = pControl;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_startTime);
    status.m_nElapsedTimeMs = (uint64_t)microseconds.count();
    status.m_pLoadingUiRootBox = pLoadingUiRootBox;
    status.m_pAnimationControl = pAnimationControl;
    status.m_bStopLoading = false;
    m_pControl->SendEvent(kEventLoadingStart, (WPARAM)&status);
    return true;
}

void ControlLoading::StopLoading()
{
    if (!m_bIsLoading) {
        return;
    }
    m_bIsLoading = false;
    m_loadingFlag.Cancel();
    m_pLoadingBox.reset();
    m_pAnimationControl = nullptr;
    m_nCallbackCount = 0;

    //Trigger the stop event
    ControlPtrT<Control> pControl(m_pControl);
    ControlLoadingStatus status;
    status.m_pControl = pControl;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_startTime);
    status.m_nElapsedTimeMs = (uint64_t)microseconds.count();
    status.m_pLoadingUiRootBox = nullptr;
    status.m_pAnimationControl = nullptr;
    status.m_bStopLoading = false;
    m_pControl->SendEvent(kEventLoadingStop, (WPARAM)&status);
}

bool ControlLoading::IsLoading() const
{
    return m_bIsLoading;
}

Box* ControlLoading::GetLoadingUiRootBox() const
{
    return m_pLoadingBox.get();
}

}
