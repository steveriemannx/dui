#include "dui/Control/CircleProgress.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Utils/AttributeUtil.h"
#include <algorithm>
#include <cmath>

namespace ui
{

CircleProgress::CircleProgress(Window* pWindow) :
    Progress(pWindow),
    m_bCircular(true),
    m_bClockwise(true),
    m_fCircleWidth(1.0f),
    m_dwBackgroundColor(0),
    m_dwForegroundColor(0),
    m_dwGradientColor(0),
    m_pIndicatorImage(nullptr),
    m_bIndeterminate(false),
    m_fIndeterminateAngle(0.0f),
    m_fStartAngle(270.0f),
    m_fSpanAngle(360.0f),
    m_fIndeterminateArc(90.0f),
    m_fIndeterminateSpeed(6.0f),
    m_nIndeterminateInterval(16),
    m_fCenterRatio(1.0f),
    m_rcCenterPadding(),
    m_centerCallback()
{
}

CircleProgress::~CircleProgress()
{
    if (m_pIndicatorImage != nullptr) {
        delete m_pIndicatorImage;
        m_pIndicatorImage = nullptr;
    }
}

std::string CircleProgress::GetType() const { return DUI_CTR_CIRCLEPROGRESS; }

void CircleProgress::SetAttribute(const std::string& srName, const std::string& strValue)
{
    if (srName == "circular") {
        SetCircular(strValue == "true");
    }
    else if ((srName == "circle_width") || (srName == "circlewidth")) {
        int32_t iValue = StringUtil::StringToInt32(strValue);
        SetCircleWidth((float)iValue, true);
    }
    else if (srName == "indicator") {
        SetIndicator(strValue);
    }
    else if (srName == "clockwise") {
        SetClockwiseRotation(strValue == "true");
    }
    else if (srName == "bgcolor") {
        SetBackgroundColor(strValue);
    }
    else if (srName == "fgcolor") {
        SetForegroundColor(strValue);
    }
    else if ((srName == "gradient_color") || (srName == "gradientcolor")) {
        SetCircleGradientColor(strValue);
    }
    else if ((srName == "indeterminate")) {
        SetIndeterminate(strValue == "true");
    }
    else if ((srName == "start_angle") || (srName == "startangle")) {
        SetStartAngle((float)StringUtil::StringToInt32(strValue));
    }
    else if ((srName == "span_angle") || (srName == "spanangle")) {
        SetSpanAngle((float)StringUtil::StringToInt32(strValue));
    }
    else if ((srName == "indeterminate_arc") || (srName == "indeterminatearc")) {
        SetIndeterminateArc((float)StringUtil::StringToInt32(strValue));
    }
    else if ((srName == "indeterminate_speed") || (srName == "indeterminatespeed")) {
        SetIndeterminateSpeed((float)StringUtil::StringToInt32(strValue));
    }
    else if ((srName == "indeterminate_interval") || (srName == "indeterminateinterval")) {
        SetIndeterminateInterval(StringUtil::StringToInt32(strValue));
    }
    else if ((srName == "center_ratio") || (srName == "centerratio")) {
        SetCenterRatio((float)StringUtil::StringToInt32(strValue) / 100.0f);
    }
    else if ((srName == "center_padding") || (srName == "centerpadding")) {
        UiPadding padding;
        AttributeUtil::ParsePaddingValue(strValue.c_str(), padding);
        SetCenterPadding(padding, true);
    }
    else {
        Progress::SetAttribute(srName, strValue);
    }
}

void CircleProgress::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    float fValue = GetCircleWidth();
    fValue = Dpi().GetScaleFloat(fValue, nOldDpiScale);
    SetCircleWidth(fValue, false);
    BaseClass::ChangeDpiScale(nOldDpiScale, nNewDpiScale);
}

void CircleProgress::PaintStateImages(IRender* pRender)
{
    if (pRender == nullptr) {
        return;
    }
    Progress::PaintStateImages(pRender);
    if (!m_bCircular) {
        return;
    }

    int32_t nMax = GetMaxValue();
    int32_t nMin = GetMinValue();
    double fValue = GetDisplayValue();

    if (nMax <= nMin) {
        nMax = nMin + 1;
    }
    if (fValue > nMax) {
        fValue = nMax;
    }
    if (fValue < nMin) {
        fValue = nMin;
    }
    //Progress ratio: normalize first and then invert it for reverse (doing the absolute value flip
    //(nMax - nMin) - fValue directly would compute an out-of-range ratio when min != 0)
    double fRatio = (fValue - nMin) / static_cast<double>(nMax - nMin);
    fRatio = std::clamp(fRatio, 0.0, 1.0);
    if (IsReverse()) {
        //Reverse: the arc is full at value = min and empty at value = max
        fRatio = 1.0 - fRatio;
    }

    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory == nullptr) {
        return;
    }
    IPen* bgPen = pRenderFactory->CreatePen(m_dwBackgroundColor, m_fCircleWidth);
    std::unique_ptr<IPen> spBgPen(bgPen);
    if (bgPen == nullptr) {
        return;
    }
    IPen* fgPen = pRenderFactory->CreatePen(m_dwForegroundColor, m_fCircleWidth);
    std::unique_ptr<IPen> spFgPen(fgPen);
    if (fgPen == nullptr) {
        return;
    }

    int direction = m_bClockwise ? 1 : -1;   //Rotation direction
    int bordersize = Dpi().GetScaleInt(1); //The arc width currently uses 1 pixel

    //Start angle and sweep angle of the arc: a fixed, continuously rotating arc in indeterminate
    //mode, otherwise driven by the progress value
    const float fStartAngle = m_fStartAngle;
    float fSweepAngle;
    if (m_bIndeterminate) {
        fSweepAngle = m_fIndeterminateArc;
    }
    else {
        fSweepAngle = static_cast<float>(direction * m_fSpanAngle * fRatio);
    }

    //Center of the circle: the control center for a full circle; for a partial circle (a dashboard
    //or a gapped ring) it is the center of the arc's bounding box, so that the upper half of a
    //half-circle dial fits completely inside the control
    const UiRect& rect = GetRect();
    UiPoint center;
    int side;
    if (m_fSpanAngle >= 359.9f) {
        //Full circle: the largest square inside the control is centered
        center.x = rect.left + (rect.right - rect.left) / 2;
        center.y = rect.top + (rect.bottom - rect.top) / 2;
        side = std::min(rect.right - rect.left, rect.bottom - rect.top);
    }
    else {
        //Partial circle: use the actual bounding box of the arc. Sample the start/end angles and the
        //quadrant extremes passed through to find min/max
        const float fPi = 3.14159265f;
        float fMinX = 1.0f, fMaxX = -1.0f, fMinY = 1.0f, fMaxY = -1.0f;
        auto AccPoint = [&](float fDeg) {
            const float fRad = fDeg * fPi / 180.0f;
            const float fX = std::cos(fRad);
            const float fY = std::sin(fRad);
            fMinX = std::min(fMinX, fX); fMaxX = std::max(fMaxX, fX);
            fMinY = std::min(fMinY, fY); fMaxY = std::max(fMaxY, fY);
            };
        //Start and end angles
        AccPoint(fStartAngle);
        AccPoint(fStartAngle + m_fSpanAngle);
        //Quadrant extremes passed through (0/90/180/270); otherwise the bounding box of a half circle
        //or a gapped arc would be computed too small
        const float fStep = m_bClockwise ? 1.0f : -1.0f;
        for (int i = 0; i <= 4; ++i) {
            const float fQ = std::fmod(fStartAngle + fStep * 90.0f * i, 360.0f);
            if (fQ < 0.0f) {
                AccPoint(fQ + 360.0f);
            }
            else {
                AccPoint(fQ);
            }
        }

        const float fArcW = fMaxX - fMinX;
        const float fArcH = fMaxY - fMinY;
        const float fAvailW = (rect.right - rect.left) / 2.0f;
        const float fAvailH = (rect.bottom - rect.top) / 2.0f;
        //Take the largest radius required by any direction so that the arc fits completely inside the control
        float fRadius = 0.0f;
        if (fArcW > 0.001f) {
            fRadius = std::max(fRadius, fAvailW / fArcW);
        }
        if (fArcH > 0.001f) {
            fRadius = std::max(fRadius, fAvailH / fArcH);
        }
        if (fRadius <= 0.0f) {
            fRadius = std::min(fAvailW, fAvailH);
        }
        //Place the circle center at the center of the arc's bounding box
        center.x = rect.left + (rect.right - rect.left) / 2 -
            static_cast<int>((fMinX + fMaxX) / 2.0f * fRadius);
        center.y = rect.top + (rect.bottom - rect.top) / 2 -
            static_cast<int>((fMinY + fMaxY) / 2.0f * fRadius);
        side = static_cast<int>(fRadius * 2.0f);
    }

    UiRect rcBorder;
    rcBorder.left = center.x - side / 2;
    rcBorder.top = center.y - side / 2;
    rcBorder.right = rcBorder.left + side;
    rcBorder.bottom = rcBorder.top + side;

    //Load the image

    std::shared_ptr<ImageInfo> pIndicatorImageInfo;
    if (m_pIndicatorImage != nullptr) {
        LoadImageInfo(*m_pIndicatorImage);
        pIndicatorImageInfo = m_pIndicatorImage->GetImageInfo();
        ASSERT(pIndicatorImageInfo != nullptr);
    }

    UiRect outer = rcBorder;
    //The drawing radius of the ring is determined by the ring width; the indicator does not take part
    //in the shrink, so that changing the indicator size does not deform the ring
    outer.Inflate(-1 * (int32_t)(m_fCircleWidth / 2), -1 * (int32_t)(m_fCircleWidth / 2));
    int inflateValue = Dpi().GetScaleInt(-1);
    outer.Inflate(inflateValue, inflateValue);

    //Background arc: both the start angle and the span are configurable (span < 360 means a gapped
    //ring or a dashboard)
    pRender->DrawArc(outer, fStartAngle, m_fSpanAngle, false, bgPen);

    //Foreground arc: a rotating arc of fixed length in indeterminate mode, otherwise it sweeps the
    //span according to the progress value
    if (m_bIndeterminate) {
        //Rotating arc: the start advances with the rotation angle and the arc length is configurable
        const float fRotateStart = m_fStartAngle + m_fIndeterminateAngle;
        if (m_dwGradientColor.GetARGB() == 0) {
            pRender->DrawArc(outer, fRotateStart, m_fIndeterminateArc, false, fgPen);
        }
        else {
            pRender->DrawArc(outer, fRotateStart, m_fIndeterminateArc, false, fgPen, &m_dwGradientColor, &rcBorder);
        }
    }
    else if (m_dwGradientColor.GetARGB() == 0) {
        //No gradient color: fill directly with the foreground color
        pRender->DrawArc(outer, fStartAngle, fSweepAngle, false, fgPen);
    }
    else {
        //Use the gradient color
        pRender->DrawArc(outer, fStartAngle, fSweepAngle, false, fgPen, &m_dwGradientColor, &rcBorder);
    }

    if (pIndicatorImageInfo != nullptr) {
        std::unique_ptr<IMatrix> spMatrix(pRenderFactory->CreateMatrix());
        //The indicator follows the end of the arc: it uses the start angle plus the sweep angle of
        //the arc instead of a fixed 12 o'clock direction
        const float fAngle = m_bIndeterminate ? (m_fStartAngle + m_fIndeterminateAngle + m_fIndeterminateArc)
            : (fStartAngle + fSweepAngle);
        if (spMatrix != nullptr) {
            spMatrix->RotateAt(fAngle, (float)center.x, (float)center.y);
        }

        //Position it at 12 o'clock first (the start direction of the arc) and then let the matrix above
        //rotate it to the end angle
        UiRect imageRect;
        imageRect.left = center.x - pIndicatorImageInfo->GetWidth() / 2;
        imageRect.top = outer.top + bordersize / 2 - pIndicatorImageInfo->GetHeight() / 2;
        imageRect.right = imageRect.left + pIndicatorImageInfo->GetWidth();
        imageRect.bottom = imageRect.top + pIndicatorImageInfo->GetHeight();
        imageRect.Offset(-GetRect().left, -GetRect().top);
        std::string imageModify = StringUtil::Printf("destscale='false' dest='%d,%d,%d,%d'",
            imageRect.left, imageRect.top, imageRect.right, imageRect.bottom);
        PaintImage(pRender, m_pIndicatorImage, imageModify, -1, spMatrix.get());
    }

    //Center content area: dashboards often need custom painting (a needle, an icon, a number); the
    //default is to paint the text
    PaintCenterContent(pRender, fValue);
}

UiRect CircleProgress::GetCenterRect() const
{
    const UiRect& rect = GetRect();
    int nWidth = rect.right - rect.left;
    int nHeight = rect.bottom - rect.top;
    //Shrink by the configured ratio
    const float fRatio = (m_fCenterRatio > 0.0f && m_fCenterRatio <= 1.0f) ? m_fCenterRatio : 1.0f;
    nWidth = static_cast<int>(nWidth * fRatio);
    nHeight = static_cast<int>(nHeight * fRatio);
    UiRect rcCenter((rect.right - rect.left) - nWidth, (rect.bottom - rect.top) - nHeight,
                    (rect.right - rect.left), (rect.bottom - rect.top));
    //Apply the padding
    rcCenter.left += m_rcCenterPadding.left;
    rcCenter.top += m_rcCenterPadding.top;
    rcCenter.right -= m_rcCenterPadding.right;
    rcCenter.bottom -= m_rcCenterPadding.bottom;
    if (rcCenter.IsEmpty()) {
        rcCenter = rect;
    }
    return rcCenter;
}

void CircleProgress::PaintCenterContent(IRender* pRender, double fValue)
{
    if ((pRender == nullptr) || !m_centerCallback) {
        //Without a custom callback the center text is handled by the painting flow of the base class
        //(text_padding / show_percent)
        return;
    }
    m_centerCallback(pRender, GetCenterRect(), fValue);
}

void CircleProgress::ClearImageCache()
{
    BaseClass::ClearImageCache();
    if (m_pIndicatorImage != nullptr) {
        m_pIndicatorImage->ClearImageCache();
    }    
}

void CircleProgress::SetCircular(bool bCircular /*= true*/)
{
    m_bCircular = bCircular;
    Invalidate();
}

void CircleProgress::SetClockwiseRotation(bool bClockwise /*= true*/)
{
    m_bClockwise = bClockwise;
}

void CircleProgress::SetCircleWidth(float fCircleWidth, bool bNeedDpiScale)
{
    if (fCircleWidth < 0) {
        fCircleWidth = 0.0f;
    }
    if (bNeedDpiScale) {
        fCircleWidth = Dpi().GetScaleFloat(fCircleWidth);
    }
    if (m_fCircleWidth != fCircleWidth) {
        m_fCircleWidth = fCircleWidth;
        Invalidate();
    }    
}

float CircleProgress::GetCircleWidth() const
{
    return m_fCircleWidth;
}

void CircleProgress::SetBackgroundColor(const std::string& strColor)
{
    m_dwBackgroundColor = GetUiColor(strColor);
    Invalidate();
}

void CircleProgress::SetForegroundColor(const std::string& strColor)
{
    m_dwForegroundColor = GetUiColor(strColor);
    Invalidate();
}

void CircleProgress::SetIndicator(const std::string& sIndicatorImage)
{
    if (m_pIndicatorImage == nullptr) {
        m_pIndicatorImage = new Image;
    }
    if (m_pIndicatorImage->GetImageString() != sIndicatorImage) {
        m_pIndicatorImage->ClearImageCache();
        m_pIndicatorImage->SetImageString(sIndicatorImage, Dpi());
        Invalidate();    
    }
}

void CircleProgress::SetCircleGradientColor(const std::string& strColor)
{
    m_dwGradientColor = GetUiColor(strColor);
    Invalidate();
}

void CircleProgress::SetIndeterminate(bool bIndeterminate)
{
    if (m_bIndeterminate == bIndeterminate) {
        return;
    }
    m_bIndeterminate = bIndeterminate;
    m_fIndeterminateAngle = 0.0f;
    if (m_bIndeterminate) {
        auto timerCallback = UiBind(&CircleProgress::OnIndeterminateTimer, this);
        GlobalManager::Instance().Timer().AddTimer(m_indeterminateTimer.GetWeakFlag(),
            timerCallback, (uint32_t)m_nIndeterminateInterval);
    }
    else {
        m_indeterminateTimer.Cancel();
    }
    Invalidate();
}

void CircleProgress::OnIndeterminateTimer()
{
    if (!m_bIndeterminate || !IsVisible()) {
        return;
    }
    //Rotate at the configured speed (6 degrees every 16ms by default, i.e. about 0.96s per revolution)
    m_fIndeterminateAngle += m_fIndeterminateSpeed;
    if (m_fIndeterminateAngle >= 360.0f) {
        m_fIndeterminateAngle -= 360.0f;
    }
    Invalidate();
}

void CircleProgress::SetStartAngle(float fAngle)
{
    //Normalize into [0, 360)
    fAngle = std::fmod(fAngle, 360.0f);
    if (fAngle < 0.0f) {
        fAngle += 360.0f;
    }
    if (m_fStartAngle != fAngle) {
        m_fStartAngle = fAngle;
        Invalidate();
    }
}

void CircleProgress::SetSpanAngle(float fSpan)
{
    //Limit the span to (0, 360]
    if (fSpan <= 0.0f || fSpan > 360.0f) {
        fSpan = 360.0f;
    }
    if (m_fSpanAngle != fSpan) {
        m_fSpanAngle = fSpan;
        Invalidate();
    }
}

void CircleProgress::SetIndeterminateArc(float fArc)
{
    if (fArc <= 0.0f || fArc > 360.0f) {
        fArc = 360.0f;
    }
    if (m_fIndeterminateArc != fArc) {
        m_fIndeterminateArc = fArc;
        Invalidate();
    }
}

void CircleProgress::SetIndeterminateSpeed(float fSpeed)
{
    if (fSpeed <= 0.0f) {
        fSpeed = 0.1f;
    }
    if (fSpeed > 360.0f) {
        fSpeed = 360.0f;
    }
    if (m_fIndeterminateSpeed != fSpeed) {
        m_fIndeterminateSpeed = fSpeed;
        //The speed changed while the arc is rotating, so the timer must be restarted with the new interval
        if (m_bIndeterminate) {
            m_indeterminateTimer.Cancel();
            m_fIndeterminateAngle = 0.0f;
            auto timerCallback = UiBind(&CircleProgress::OnIndeterminateTimer, this);
            GlobalManager::Instance().Timer().AddTimer(m_indeterminateTimer.GetWeakFlag(),
                timerCallback, (uint32_t)m_nIndeterminateInterval);
        }
    }
}

void CircleProgress::SetIndeterminateInterval(int32_t nInterval)
{
    if (nInterval < 1) {
        nInterval = 1;
    }
    if (m_nIndeterminateInterval != nInterval) {
        m_nIndeterminateInterval = nInterval;
        if (m_bIndeterminate) {
            m_indeterminateTimer.Cancel();
            m_fIndeterminateAngle = 0.0f;
            auto timerCallback = UiBind(&CircleProgress::OnIndeterminateTimer, this);
            GlobalManager::Instance().Timer().AddTimer(m_indeterminateTimer.GetWeakFlag(),
                timerCallback, (uint32_t)nInterval);
        }
    }
}

void CircleProgress::SetCenterRatio(float fRatio)
{
    if (fRatio <= 0.0f || fRatio > 1.0f) {
        fRatio = 1.0f;
    }
    if (m_fCenterRatio != fRatio) {
        m_fCenterRatio = fRatio;
        Invalidate();
    }
}

void CircleProgress::SetCenterPadding(const UiPadding& padding, bool bNeedDpiScale)
{
    UiPadding newPadding = padding;
    if (bNeedDpiScale) {
        Dpi().ScalePadding(newPadding);
    }
    if (!m_rcCenterPadding.Equals(newPadding)) {
        m_rcCenterPadding = newPadding;
        Invalidate();
    }
}

void CircleProgress::SetCenterCallback(const CircleCenterCallback& callback)
{
    m_centerCallback = callback;
    Invalidate();
}

}
