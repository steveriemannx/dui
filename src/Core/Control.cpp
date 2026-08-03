#include "duilib/Core/Control.h"
#include "duilib/Core/ControlLoading.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/Box.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/ColorManager.h"
#include "duilib/Core/StateColorMap.h"
#include "duilib/Core/StateColorMap2.h"
#include "duilib/Image/Image.h"
#include "duilib/Render/IRender.h"
#include "duilib/Render/AutoClip.h"
#include "duilib/Animation/AnimationPlayer.h"
#include "duilib/Animation/AnimationManager.h"
#include "duilib/Utils/StringConvert.h"
#include "duilib/Utils/StringUtil.h"
#include "duilib/Utils/AttributeUtil.h"
#include "duilib/Utils/PerformanceUtil.h"

#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)
    #include "duilib/Core/ControlDropTargetImpl_Windows.h"
#endif

#ifdef DUILIB_BUILD_FOR_SDL
    #include "duilib/Core/ControlDropTargetImpl_SDL.h"
#endif

namespace ui 
{
Control::Control(Window* pWindow) :
    PlaceHolder(pWindow),
    m_bContextMenuUsed(false),
    m_bMouseFocused(false),
    m_bNoFocus(false),
    m_bAllowTabstop(true),
    m_cursorType(CursorType::kCursorArrow),
    m_controlState(kControlStateNormal),
    m_nAlpha(255),
    m_nHotAlpha(0),
    m_bBoxShadowPainted(false),
    m_uUserDataID((size_t)-1),
    m_bShowFocusRect(false),
    m_nPaintOrder(0),
    m_bBordersOnTop(true),
    m_bMouseEnter(false)
{
}

Control::~Control()
{
    // Remove from the delayed paint list
    GlobalManager::Instance().Image().RemoveDelayPaintData(this);

    // Clean up animation-related resources to avoid the timer generating callbacks again and causing errors
    if (m_pAnimationData != nullptr) {
        if (m_pAnimationData->m_animationManager != nullptr) {
            m_pAnimationData->m_animationManager->Clear(this);
        }
        m_pAnimationData->m_animationManager.reset();
    }

    Window* pWindow = GetWindow();
    if (pWindow) {
        pWindow->ReapObjects(this);
    }

    m_pAnimationData.reset();
    m_pBkImage.reset();
    m_pImageMap.reset();
    m_pDragDropData.reset();
    m_pOtherData.reset();
    m_pEventMapData.reset();
    m_pImageMap.reset();
    m_pColorMap.reset();
    m_pColorData.reset();
    m_pBorderData.reset();
}

DString Control::GetType() const { return DUI_CTR_CONTROL; }

void Control::SetAttribute(const DString& strName, const DString& strValue)
{
    ASSERT(GetWindow() != nullptr);// The associated window must be set first because DPI awareness is needed
    if (strName == _T("class")) {
        SetClass(strValue);
    }
    else if (strName == _T("halign")) {
        if (strValue == _T("left")) {
            SetHorAlignType(HorAlignType::kAlignLeft);
        }
        else if (strValue == _T("center")) {
            SetHorAlignType(HorAlignType::kAlignCenter);
        }
        else if (strValue == _T("right")) {
            SetHorAlignType(HorAlignType::kAlignRight);
        }
        else {
            ASSERT(0);
        }
    }
    else if (strName == _T("valign")) {
        if (strValue == _T("top")) {
            SetVerAlignType(VerAlignType::kAlignTop);
        }
        else if (strValue == _T("center")) {
            SetVerAlignType(VerAlignType::kAlignCenter);
        }
        else if (strValue == _T("bottom")) {
            SetVerAlignType(VerAlignType::kAlignBottom);
        }
        else {
            ASSERT(0);
        }
    }
    else if (strName == _T("align")) {
        // Horizontal alignment
        if (strValue.find(_T("left")) != DString::npos) {
            SetHorAlignType(HorAlignType::kAlignLeft);
        }
        else if (strValue.find(_T("hcenter")) != DString::npos) {
            SetHorAlignType(HorAlignType::kAlignCenter);
        }
        else if (strValue.find(_T("right")) != DString::npos) {
            SetHorAlignType(HorAlignType::kAlignRight);
        }
        // Vertical alignment
        if (strValue.find(_T("top")) != DString::npos) {
            SetVerAlignType(VerAlignType::kAlignTop);
        }
        else if (strValue.find(_T("vcenter")) != DString::npos) {
            SetVerAlignType(VerAlignType::kAlignCenter);
        }
        else if (strValue.find(_T("bottom")) != DString::npos) {
            SetVerAlignType(VerAlignType::kAlignBottom);
        }
    }
    else if (strName == _T("margin")) {
        UiMargin rcMargin;
        AttributeUtil::ParseMarginValue(strValue.c_str(), rcMargin);
        SetMargin(rcMargin, true);
    }
    else if (strName == _T("padding")) {
        UiPadding rcPadding;
        AttributeUtil::ParsePaddingValue(strValue.c_str(), rcPadding);
        SetPadding(rcPadding, true);
    }
    else if (strName == _T("control_padding")) {
        SetEnableControlPadding(strValue == _T("true"));
    }
    else if (strName == _T("bkcolor")) {
        // Background color
        SetBkColor(strValue);
    }
    else if (strName == _T("bkcolor2")) {
        // Second background color (implements the gradient background color)
        SetBkColor2(strValue);
    }
    else if (strName == _T("bkcolor2_direction")) {
        // Direction of the second background color: "1": left->right, "2": top->bottom, "3": top-left->bottom-right, "4": top-right->bottom-left
        SetBkColor2Direction(strValue);
    }
    else if (strName == _T("fore_color")) {
        // Foreground color
        SetForeColor(strValue);
    }
    else if ((strName == _T("border_size")) || (strName == _T("bordersize"))) {
        // Border width
        DString nValue = strValue;
        if (nValue.find(_T(',')) == DString::npos) {
            int32_t nBorderSize = StringUtil::StringToInt32(strValue);
            if (nBorderSize < 0) {
                nBorderSize = 0;
            }
            UiRectF rcBorder((float)nBorderSize, (float)nBorderSize, (float)nBorderSize, (float)nBorderSize);
            SetBorderSize(rcBorder, true);
        }
        else {
            UiMargin rcMargin;
            AttributeUtil::ParseMarginValue(strValue.c_str(), rcMargin);
            UiRectF rcBorder((float)rcMargin.left, (float)rcMargin.top, (float)rcMargin.right, (float)rcMargin.bottom);
            SetBorderSize(rcBorder, true);
        }
    }
    else if (strName == _T("border_dash_style")) {
        // Border dash style (the dash styles of the four borders can only be the same, separate settings are not supported)
        IPen::DashStyle dashStyle = IPen::kDashStyleSolid;
        if (strValue == _T("solid")) {
            dashStyle = IPen::kDashStyleSolid;
        }
        else if (strValue == _T("dash")) {
            dashStyle = IPen::kDashStyleDash;
        }
        else if (strValue == _T("dot")) {
            dashStyle = IPen::kDashStyleDot;
        }
        else if (strValue == _T("dash_dot")) {
            dashStyle = IPen::kDashStyleDashDot;
        }
        else if (strValue == _T("dash_dot_dot")) {
            dashStyle = IPen::kDashStyleDashDotDot;
        }
        SetBorderDashStyle((int8_t)dashStyle);
    }
    else if (strName == _T("borders_on_top")) {
        // Whether the border is on top (i.e. draw the child controls first, then the border, to avoid the border being covered by child controls)
        SetBordersOnTop(strValue == _T("true"));
    }
    else if ((strName == _T("border_round")) || (strName == _T("borderround"))) {
        // Border round size
        UiSize cxyRound;
        AttributeUtil::ParseSizeValue(strValue.c_str(), cxyRound);
        SetBorderRound(cxyRound);
    }
    else if ((strName == _T("box_shadow")) || (strName == _T("boxshadow"))) {
        SetBoxShadow(strValue);
    }
    else if (strName == _T("width")) {
        if (strValue == _T("stretch")) {
            // Width is stretch: the width is allocated by the parent container
            SetFixedWidth(UiFixedInt::MakeStretch(), true, true);
        }
        else if (strValue == _T("auto")) {
            // Width is auto: the width is calculated automatically based on the control's text, image, etc.
            SetFixedWidth(UiFixedInt::MakeAuto(), true, true);
        }
        else if (!strValue.empty()) {
            if (strValue.back() == _T('%')) {
                // Width is stretch: the width is allocated by the parent container by percentage, e.g. width="30%" means the expected width of the control is 30% of the parent control's width
                int32_t iValue = StringUtil::StringToInt32(strValue);
                if ((iValue <= 0) || (iValue > 100)) {
                    iValue = 100;
                }
                SetFixedWidth(UiFixedInt::MakeStretch(iValue), true, false);
            }
            else {
                // Width is a fixed value
                ASSERT(StringUtil::StringToInt32(strValue) >= 0);
                SetFixedWidth(UiFixedInt(StringUtil::StringToInt32(strValue)), true, true);
            }
        }
        else {
            SetFixedWidth(UiFixedInt(0), true, true);
        }
    }
    else if (strName == _T("height")) {
        if (strValue == _T("stretch")) {
            // Height is stretch: the height is allocated by the parent container
            SetFixedHeight(UiFixedInt::MakeStretch(), true, true);
        }
        else if (strValue == _T("auto")) {
            // Height is auto: the height is calculated automatically based on the control's text, image, etc.
            SetFixedHeight(UiFixedInt::MakeAuto(), true, true);
        }
        else if (!strValue.empty()) {
            if (strValue.back() == _T('%')) {
                // Height is stretch: the height is allocated by the parent container by percentage, e.g. height="30%" means the expected height of the control is 30% of the parent control's height
                int32_t iValue = StringUtil::StringToInt32(strValue);
                if ((iValue <= 0) || (iValue > 100)) {
                    iValue = 100;
                }
                SetFixedHeight(UiFixedInt::MakeStretch(iValue), true, false);
            }
            else {
                // Height is a fixed value
                ASSERT(StringUtil::StringToInt32(strValue) >= 0);
                SetFixedHeight(UiFixedInt(StringUtil::StringToInt32(strValue)), true, true);
            }
        }
        else {
            SetFixedHeight(UiFixedInt(0), true, true);
        }
    }
    else if (strName == _T("state")) {
        if (strValue == _T("normal")) {
            SetState(kControlStateNormal);
        }
        else if (strValue == _T("hot")) {
            SetState(kControlStateHot);
        }
        else if (strValue == _T("pushed")) {
            SetState(kControlStatePushed);
        }
        else if (strValue == _T("disabled")) {
            SetState(kControlStateDisabled);
        }
        else {
            ASSERT(0);
        }
    }
    else if ((strName == _T("cursor_type")) || (strName == _T("cursortype"))) {
        if (strValue == _T("arrow")) {
            SetCursorType(CursorType::kCursorArrow);
        }
        else if (strValue == _T("ibeam")) {
            SetCursorType(CursorType::kCursorIBeam);
        }
        else if (strValue == _T("hand")) {
            SetCursorType(CursorType::kCursorHand);
        }
        else if (strValue == _T("wait")) {
            SetCursorType(CursorType::kCursorWait);
        }
        else if (strValue == _T("cross")) {
            SetCursorType(CursorType::kCursorCross);
        }
        else if (strValue == _T("size_we")) {
            SetCursorType(CursorType::kCursorSizeWE);
        }
        else if (strValue == _T("size_ns")) {
            SetCursorType(CursorType::kCursorSizeNS);
        }
        else if (strValue == _T("size_nwse")) {
            SetCursorType(CursorType::kCursorSizeNWSE);
        }
        else if (strValue == _T("size_nesw")) {
            SetCursorType(CursorType::kCursorSizeNESW);
        }
        else if (strValue == _T("size_all")) {
            SetCursorType(CursorType::kCursorSizeAll);
        }
        else if (strValue == _T("no")) {
            SetCursorType(CursorType::kCursorNo);
        }
        else if (strValue == _T("progress")) {
            SetCursorType(CursorType::kCursorProgress);
        }
        else {
            ASSERT(0);
        }
    }
    else if ((strName == _T("render_offset")) || (strName == _T("renderoffset"))) {
        UiPoint renderOffset;
        AttributeUtil::ParsePointValue(strValue.c_str(), renderOffset);
        SetRenderOffset(renderOffset, true);
    }
    else if ((strName == _T("normal_color")) || (strName == _T("normalcolor"))) {
        SetStateColor(kControlStateNormal, strValue);
    }
    else if ((strName == _T("hot_color")) || (strName == _T("hotcolor"))) {
        SetStateColor(kControlStateHot, strValue);
    }
    else if ((strName == _T("pushed_color")) || (strName == _T("pushedcolor"))) {
        SetStateColor(kControlStatePushed, strValue);
    }
    else if ((strName == _T("disabled_color")) || (strName == _T("disabledcolor"))) {
        SetStateColor(kControlStateDisabled, strValue);
    }
    else if (strName == _T("normal_color_margin")) {
        UiMargin rcMargin;
        AttributeUtil::ParseMarginValue(strValue.c_str(), rcMargin);
        SetStateColorMargin(kControlStateNormal, rcMargin, true);
    }
    else if (strName == _T("hot_color_margin")) {
        UiMargin rcMargin;
        AttributeUtil::ParseMarginValue(strValue.c_str(), rcMargin);
        SetStateColorMargin(kControlStateHot, rcMargin, true);
    }
    else if (strName == _T("pushed_color_margin")) {
        UiMargin rcMargin;
        AttributeUtil::ParseMarginValue(strValue.c_str(), rcMargin);
        SetStateColorMargin(kControlStatePushed, rcMargin, true);
    }
    else if (strName == _T("disabled_color_margin")) {
        UiMargin rcMargin;
        AttributeUtil::ParseMarginValue(strValue.c_str(), rcMargin);
        SetStateColorMargin(kControlStateDisabled, rcMargin, true);
    }
    else if (strName == _T("normal_color_round")) {
        UiSize szRound;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szRound);
        SetStateColorRound(kControlStateNormal, szRound, true);
    }
    else if (strName == _T("hot_color_round")) {
        UiSize szRound;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szRound);
        SetStateColorRound(kControlStateHot, szRound, true);
    }
    else if (strName == _T("pushed_color_round")) {
        UiSize szRound;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szRound);
        SetStateColorRound(kControlStatePushed, szRound, true);
    }
    else if (strName == _T("disabled_color_round")) {
        UiSize szRound;
        AttributeUtil::ParseSizeValue(strValue.c_str(), szRound);
        SetStateColorRound(kControlStateDisabled, szRound, true);
    }
    else if ((strName == _T("border_color")) || (strName == _T("bordercolor"))) {
        SetBorderColor(strValue);
    }
    else if (strName == _T("normal_border_color")) {
        SetBorderColor(kControlStateNormal, strValue);
    }
    else if (strName == _T("hot_border_color")) {
        SetBorderColor(kControlStateHot, strValue);
    }
    else if (strName == _T("pushed_border_color")) {
        SetBorderColor(kControlStatePushed, strValue);
    }
    else if (strName == _T("disabled_border_color")) {
        SetBorderColor(kControlStateDisabled, strValue);
    }
    else if (strName == _T("focus_border_color")) {
        SetFocusBorderColor(strValue);
    }
    else if ((strName == _T("left_border_size")) || (strName == _T("leftbordersize"))) {
        SetLeftBorderSize((float)StringUtil::StringToInt32(strValue), true);
    }
    else if ((strName == _T("top_border_size")) || (strName == _T("topbordersize"))) {
        SetTopBorderSize((float)StringUtil::StringToInt32(strValue), true);
    }
    else if ((strName == _T("right_border_size")) || (strName == _T("rightbordersize"))) {
        SetRightBorderSize((float)StringUtil::StringToInt32(strValue), true);
    }
    else if ((strName == _T("bottom_border_size")) || (strName == _T("bottombordersize"))) {
        SetBottomBorderSize((float)StringUtil::StringToInt32(strValue), true);
    }
    else if (strName == _T("bkimage")) {
        SetBkImage(strValue);
    }
    else if ((strName == _T("min_width")) || (strName == _T("minwidth"))) {
        SetMinWidth(StringUtil::StringToInt32(strValue), true);
    }
    else if ((strName == _T("max_width")) || (strName == _T("maxwidth"))) {
        SetMaxWidth(StringUtil::StringToInt32(strValue), true);
    }
    else if ((strName == _T("min_height")) || (strName == _T("minheight"))) {
        SetMinHeight(StringUtil::StringToInt32(strValue), true);
    }
    else if ((strName == _T("max_height")) || (strName == _T("maxheight"))) {
        SetMaxHeight(StringUtil::StringToInt32(strValue), true);
    }
    else if (strName == _T("name")) {
        SetName(strValue);
    }
    else if ((strName == _T("tooltip_text")) || (strName == _T("tooltiptext"))) {
        SetToolTipText(strValue);
    }
    else if ((strName == _T("tooltip_text_id")) || (strName == _T("tooltip_textid")) || (strName == _T("tooltiptextid"))) {
        SetToolTipTextId(strValue);
    }
    else if (strName == _T("tooltip_width")) {
        SetToolTipWidth(StringUtil::StringToInt32(strValue), true);
    }
    else if ((strName == _T("data_id")) || (strName == _T("dataid"))) {
        SetDataID(strValue);
    }
    else if ((strName == _T("user_data_id")) || (strName == _T("user_dataid"))) {
        SetUserDataID(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("enabled")) {
        SetEnabled(strValue == _T("true"));
    }
    else if ((strName == _T("mouse_enabled")) || (strName == _T("mouse"))) {
        SetMouseEnabled(strValue == _T("true"));
    }
    else if ((strName == _T("keyboard_enabled")) || (strName == _T("keyboard"))) {
        SetKeyboardEnabled(strValue == _T("true"));
    }
    else if (strName == _T("visible")) {
        SetVisible(strValue == _T("true"));
    }
    else if ((strName == _T("fade_visible")) || (strName == _T("fadevisible"))) {
        SetFadeVisible(strValue == _T("true"));
    }
    else if (strName == _T("float")) {
        SetFloat(strValue == _T("true"));
    }
    else if (strName == _T("keep_float_pos")) {
        SetKeepFloatPos(strValue == _T("true"));
    }
    else if (strName == _T("cache")) {
        // Ignore this option: the corresponding feature has been removed
    }
    else if ((strName == _T("no_focus")) || (strName == _T("nofocus"))) {
        SetNoFocus();
    }
    else if (strName == _T("alpha")) {
        SetAlpha(ui::TruncateToUInt8(StringUtil::StringToInt32(strValue)));
    }
    else if ((strName == _T("normal_image")) || (strName == _T("normalimage"))) {
        SetStateImage(kControlStateNormal, strValue);
    }
    else if ((strName == _T("hot_image")) || (strName == _T("hotimage"))) {
        SetStateImage(kControlStateHot, strValue);
    }
    else if ((strName == _T("pushed_image")) || (strName == _T("pushedimage"))) {
        SetStateImage(kControlStatePushed, strValue);
    }
    else if ((strName == _T("disabled_image")) || (strName == _T("disabledimage"))) {
        SetStateImage(kControlStateDisabled, strValue);
    }
    else if ((strName == _T("fore_normal_image")) || (strName == _T("forenormalimage"))) {
        SetForeStateImage(kControlStateNormal, strValue);
    }
    else if ((strName == _T("fore_hot_image")) || (strName == _T("forehotimage"))) {
        SetForeStateImage(kControlStateHot, strValue);
    }
    else if ((strName == _T("fore_pushed_image")) || (strName == _T("forepushedimage"))) {
        SetForeStateImage(kControlStatePushed, strValue);
    }
    else if ((strName == _T("fore_disabled_image")) || (strName == _T("foredisabledimage"))) {
        SetForeStateImage(kControlStateDisabled, strValue);
    }
    else if ((strName == _T("fade_alpha")) || (strName == _T("fadealpha"))) {
        bool bFadeVisible = strValue != _T("false");
        uint8_t nEndAlpha = GetAlpha();
        if (bFadeVisible) {
            if (strValue != _T("true")) {
                nEndAlpha = ui::TruncateToUInt8(StringUtil::StringToInt32(strValue));
            }
        }
        GetAnimationManager().SetFadeAlpha(bFadeVisible, nEndAlpha);
    }
    else if ((strName == _T("fade_hot")) || (strName == _T("fadehot"))) {
        SetFadeHot(strValue == _T("true"));
    }
    else if (strName == _T("fade_hot_frame_interval_ms")) {
        SetFadeHotFrameIntervalMillSeconds(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("fade_hot_total_ms")) {
        SetFadeHotTotalMillSeconds(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("fade_hot_easing_function")) {
        SetFadeHotEasingFunctionType(EasingFunctions::GetEasingFunctionType(strValue));
    }
    else if ((strName == _T("fade_width")) || (strName == _T("fadewidth"))) {
        GetAnimationManager().SetFadeWidth(strValue == _T("true"));
    }
    else if ((strName == _T("fade_height")) || (strName == _T("fadeheight"))) {
        GetAnimationManager().SetFadeHeight(strValue == _T("true"));
    }
    else if (strName == _T("fade_size")) {
        GetAnimationManager().SetFadeSize(strValue == _T("true"));
    }
    else if ((strName == _T("fade_in_out_x_from_left")) || (strName == _T("fadeinoutxfromleft"))) {
        GetAnimationManager().SetFadeInOutX(strValue == _T("true"), false);
    }
    else if ((strName == _T("fade_in_out_x_from_right")) || (strName == _T("fadeinoutxfromright"))) {
        GetAnimationManager().SetFadeInOutX(strValue == _T("true"), true);
    }
    else if ((strName == _T("fade_in_out_y_from_top")) || (strName == _T("fadeinoutyfromtop"))) {
        GetAnimationManager().SetFadeInOutY(strValue == _T("true"), false);
    }
    else if ((strName == _T("fade_in_out_y_from_bottom")) || (strName == _T("fadeinoutyfrombottom"))) {
        GetAnimationManager().SetFadeInOutY(strValue == _T("true"), true);
    }
    else if (strName == _T("fade_frame_interval_ms")) {
        GetAnimationManager().SetFrameIntervalMillSeconds(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("fade_total_ms")) {
        GetAnimationManager().SetTotalMillSeconds(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("fade_easing_function")) {        
        GetAnimationManager().SetEasingFunctionType(EasingFunctions::GetEasingFunctionType(strValue));
    }
    else if ((strName == _T("tab_stop")) || (strName == _T("tabstop"))) {
        SetTabStop(strValue == _T("true"));
    }
    else if (strName == _T("loading")) {
        SetLoadingAttribute(strValue);
    }
    else if (strName == _T("show_focus_rect")) {
        SetShowFocusRect(strValue == _T("true"));
    }
    else if (strName == _T("focus_rect_color")) {
        SetFocusRectColor(strValue);
    }
    else if (strName == _T("paint_order")) {
        uint8_t nPaintOrder = TruncateToUInt8(StringUtil::StringToInt32(strValue));
        SetPaintOrder(nPaintOrder);
    }
    else if ((strName == _T("start_image_animation")) || (strName == _T("start_gif_play"))) {
        ParseStartImageAnimation(strValue);
    }
    else if ((strName == _T("stop_image_animation")) || (strName == _T("stop_gif_play"))) {
        ParseStopImageAnimation(strValue);
    }
    else if (strName == _T("set_image_animation_frame")) {
        ParseSetImageAnimationFrame(strValue);
    }
    else if (strName == _T("enable_drag_drop")) {
        // Whether drag and drop operations are allowed
        SetEnableDragDrop(strValue == _T("true"));
    }
    else if (strName == _T("enable_drop_file")) {
        // Whether dropping files is allowed
        SetEnableDropFile(strValue == _T("true"));
    }
    else if (strName == _T("drop_file_types")) {
        // The list of file extensions that can be dropped
        SetDropFileTypes(strValue);
    }
    else if (strName == _T("row_span")) {
        // Set the cell spanning property (how many rows it spans); only takes effect in the GridLayout
        SetRowSpan(StringUtil::StringToInt32(strValue));
    }
    else if (strName == _T("col_span")) {
        // Set the cell spanning property (how many columns it spans); only takes effect in the GridLayout
        SetColumnSpan(StringUtil::StringToInt32(strValue));
    }
    else {
        ASSERT(!"Control::SetAttribute failed: found an unrecognized attribute");
    }
}

void Control::ParseStartImageAnimation(const DString& value)
{
    std::vector<DString> paramList;
    auto params = StringUtil::Split(value, _T(","));
    for (DString& v : params) {
        StringUtil::Trim(v);
        paramList.push_back(v);
    }
    DString imageName;
    AnimationImagePos nStartFrame = AnimationImagePos::kFrameCurrent;
    int32_t nPlayCount = 0;
    if (paramList.size() > 0) {
        imageName = paramList[0];
    }
    if (paramList.size() > 1) {
        int32_t nFrame = StringUtil::StringToInt32(paramList[1]);
        if (nFrame == 0) {
            nStartFrame = AnimationImagePos::kFrameFirst;
        }
        else if (nFrame == 2) {
            nStartFrame = AnimationImagePos::kFrameLast;
        }
        else {
            ASSERT(nFrame == 1);
        }
    }
    if (paramList.size() > 2) {
        nPlayCount = StringUtil::StringToInt32(paramList[2]);
    }
    StartImageAnimation(imageName, nStartFrame, nPlayCount);
}

void Control::ParseStopImageAnimation(const DString& value)
{
    std::vector<DString> paramList;
    auto params = StringUtil::Split(value, _T(","));
    for (DString& v : params) {
        StringUtil::Trim(v);
        paramList.push_back(v);
    }
    DString imageName;
    AnimationImagePos nStartFrame = AnimationImagePos::kFrameCurrent;
    bool bTriggerEvent = true;
    if (paramList.size() > 0) {
        imageName = paramList[0];
    }
    if (paramList.size() > 1) {
        int32_t nFrame = StringUtil::StringToInt32(paramList[1]);
        if (nFrame == 0) {
            nStartFrame = AnimationImagePos::kFrameFirst;
        }
        else if (nFrame == 2) {
            nStartFrame = AnimationImagePos::kFrameLast;
        }
        else {
            ASSERT(nFrame == 1);
        }
    }
    if (paramList.size() > 2) {
        bTriggerEvent = (paramList[2] == _T("true")) || (paramList[2] == _T("1"));
    }
    StopImageAnimation(imageName, nStartFrame, bTriggerEvent);
}

void Control::ParseSetImageAnimationFrame(const DString& value)
{
    std::vector<DString> paramList;
    auto params = StringUtil::Split(value, _T(","));
    for (DString& v : params) {
        StringUtil::Trim(v);
        paramList.push_back(v);
    }
    DString imageName;
    int32_t nFrameIndex = -1;
    if (paramList.size() > 0) {
        imageName = paramList[0];
    }
    if (paramList.size() > 1) {
        nFrameIndex = StringUtil::StringToInt32(paramList[1]);        
    }
    SetImageAnimationFrame(imageName, nFrameIndex);
}

void Control::ChangeDpiScale(uint32_t nOldDpiScale, uint32_t nNewDpiScale)
{
    if (!Dpi().CheckDisplayScaleFactor(nNewDpiScale)) {
        return;
    }
    UiMargin rcMargin = GetMargin();
    rcMargin = Dpi().GetScaleMargin(rcMargin, nOldDpiScale);
    SetMargin(rcMargin, false);

    UiPadding rcPadding = GetPadding();
    rcPadding = Dpi().GetScalePadding(rcPadding, nOldDpiScale);
    SetPadding(rcPadding, false);

    UiRectF rcBorder = GetBorderSize();
    rcBorder.left = Dpi().GetScaleFloat(rcBorder.left, nOldDpiScale);
    rcBorder.top = Dpi().GetScaleFloat(rcBorder.top, nOldDpiScale);
    rcBorder.right = Dpi().GetScaleFloat(rcBorder.right, nOldDpiScale);
    rcBorder.bottom = Dpi().GetScaleFloat(rcBorder.bottom, nOldDpiScale);
    SetBorderSize(rcBorder, false);

    UiPoint renderOffset = GetRenderOffset();
    renderOffset = Dpi().GetScalePoint(renderOffset, nOldDpiScale);
    SetRenderOffset(renderOffset, false);

    int32_t nMinWidth = GetMinWidth();
    if (nMinWidth >= 0) {
        nMinWidth = Dpi().GetScaleInt(nMinWidth, nOldDpiScale);
        SetMinWidth(nMinWidth, false);
    }

    int32_t nMaxWidth = GetMaxWidth();
    if ((nMaxWidth >= 0) && (nMaxWidth != INT32_MAX)) {
        nMaxWidth = Dpi().GetScaleInt(nMaxWidth, nOldDpiScale);
        SetMaxWidth(nMaxWidth, false);
    }

    int32_t nMinHeight = GetMinHeight();
    if (nMinHeight >= 0) {
        nMinHeight = Dpi().GetScaleInt(nMinHeight, nOldDpiScale);
        SetMinHeight(nMinHeight, false);
    }

    int32_t nMaxHeight = GetMaxHeight();
    if ((nMaxHeight >= 0) && (nMaxHeight != INT32_MAX)) {
        nMaxHeight = Dpi().GetScaleInt(nMaxHeight, nOldDpiScale);
        SetMaxHeight(nMaxHeight, false);
    }

    int32_t nToolTipWidth = GetToolTipWidth();
    if (nToolTipWidth >= 0) {
        nToolTipWidth = Dpi().GetScaleInt(nToolTipWidth, nOldDpiScale);
        SetToolTipWidth(nToolTipWidth, false);
    }

    UiMargin rcBkImageMargin = GetBkImageMargin();
    SetBkImageMargin(rcBkImageMargin, false);// This value does not need DPI scaling; just store it as the value of the current DPI

    UiFixedInt fixedWidth = GetFixedWidth();
    if (fixedWidth.IsInt32()) {
        int32_t nFixedWidth = Dpi().GetScaleInt(fixedWidth.GetInt32(), nOldDpiScale);
        SetFixedWidth(UiFixedInt(nFixedWidth), true, false);
    }
    UiFixedInt fixedHeight = GetFixedHeight();
    if (fixedHeight.IsInt32()) {
        int32_t nFixedHeight = Dpi().GetScaleInt(fixedHeight.GetInt32(), nOldDpiScale);
        SetFixedHeight(UiFixedInt(nFixedHeight), true, false);
    }

    if (m_pColorMap != nullptr) {
        for (int32_t nStateType = 0; nStateType < kControlStateCount; ++nStateType) {
            ControlStateType stateType = (ControlStateType)nStateType;

            UiMargin colorMargin = m_pColorMap->GetStateColorMargin(stateType);
            UiMargin newColorMargin = Dpi().GetScaleMargin(colorMargin, nOldDpiScale);
            if (!newColorMargin.Equals(colorMargin)) {
                m_pColorMap->SetStateColorMargin(stateType, newColorMargin);
            }

            UiSize colorRound = m_pColorMap->GetStateColorRound(stateType);
            UiSize newColorRound = Dpi().GetScaleSize(colorRound, nOldDpiScale);
            if (!newColorRound.Equals(colorRound)) {
                m_pColorMap->SetStateColorRound(stateType, newColorRound);
            }
        }
    }

    // For auto-sized controls, the size needs to be re-estimated
    SetReEstimateSize(true);
}

void Control::OnLanguageChanged()
{
    RelayoutOrRedraw();
    Invalidate();
}

void Control::SetClass(const DString& strClass)
{
    if (strClass.empty()) {
        return;
    }
    std::list<DString> splitList = StringUtil::Split(strClass, _T(" "));
    for (auto it = splitList.begin(); it != splitList.end(); it++) {
        DString pDefaultAttributes = GlobalManager::Instance().GetClassAttributes((*it));
        Window* pWindow = GetWindow();
        if (pDefaultAttributes.empty() && (pWindow != nullptr)) {
            pDefaultAttributes = pWindow->GetClassAttributes(*it);
        }

        if (!pDefaultAttributes.empty()) {
            ApplyAttributeList(pDefaultAttributes);
        }
    }
}

void Control::ApplyAttributeList(const DString& strList)
{
    // Attribute list: parse first, then apply
    if (strList.empty()) {
        return;
    }
    std::vector<std::pair<DString, DString>> attributeList;
    if (strList.find(_T('\"')) != DString::npos) {
        AttributeUtil::ParseAttributeList(strList, _T('\"'), attributeList);
    }    
    else if (strList.find(_T('\'')) != DString::npos) {
        AttributeUtil::ParseAttributeList(strList, _T('\''), attributeList);
    }
    for (const auto& attribute : attributeList) {
        SetAttribute(attribute.first, attribute.second);
    }
}

bool Control::OnApplyAttributeList(const DString& strReceiver, const DString& strList, const EventArgs& /*eventArgs*/)
{
    bool isFindSubControl = false;
    DString receiverName = strReceiver;
    if (receiverName.size() >= 2) {
        if (receiverName.substr(0, 2) == _T(".\\") || receiverName.substr(0, 2) == _T("./")) {
            receiverName = receiverName.substr(2);
            isFindSubControl = true;
        }
    }
    Control* pReceiverControl = nullptr;
    if (isFindSubControl) {
        Box* pBox = dynamic_cast<Box*>(this);
        if (pBox != nullptr) {
            pReceiverControl = pBox->FindSubControl(receiverName);
        }
    }
    else if (!receiverName.empty()) {
        pReceiverControl = GetWindow()->FindControl(receiverName);
    }
    else {
        pReceiverControl = this;
    }

    DString strValueList = strList;
    // These are hand-written attributes, using curly braces {} instead of double quotes, so escape characters are not needed when writing them;
    StringUtil::ReplaceAll(_T("{"), _T("\""), strValueList);
    StringUtil::ReplaceAll(_T("}"), _T("\""), strValueList);

    if (pReceiverControl != nullptr) {        
        pReceiverControl->ApplyAttributeList(strValueList);
        return true;
    }
    else {
        if (strReceiver == _T("#window#")) {
            // A special Receiver, representing the associated window
            if (GetWindow() != nullptr) {
                GetWindow()->ApplyAttributeList(strValueList);
                return true;
            }
        }
        ASSERT(!"Control::OnApplyAttributeList error：Receiver Control not found!");
        return false;
    }
}

void Control::SetFadeHot(bool bFadeHot)
{
    if (bFadeHot) {
        AnimationPlayer* pAnimationPlayer = new AnimationPlayer;
        pAnimationPlayer->SetAnimationType(AnimationType::kAnimationHot);
        pAnimationPlayer->SetStartValue(0);
        pAnimationPlayer->SetEndValue(255);
        ControlPtr pControl(this);

        AnimationPlayCallback playCallback = [pControl](int32_t nNewValue) {
                if (pControl != nullptr) {
                    if (nNewValue < 0) {
                        nNewValue = 0;
                    }
                    if (nNewValue > 255) {
                        nNewValue = 255;
                    }
                    pControl->SetHotAlpha(TruncateToUInt8(nNewValue));
                }
            };
        pAnimationPlayer->SetPlayCallback(playCallback);

        // After the animation completes, one repaint is needed
        AnimationCompleteCallback completeCallback = [pControl]() {
                if (pControl != nullptr) {
                    pControl->Invalidate();
                }
            };
        pAnimationPlayer->SetCompleteCallback(completeCallback);

        if (m_pHotAnimationPlayer != nullptr) {
            // Synchronize the properties
            pAnimationPlayer->SetTotalMillSeconds(m_pHotAnimationPlayer->GetTotalMillSeconds());
            pAnimationPlayer->SetFrameIntervalMillSeconds(m_pHotAnimationPlayer->GetFrameIntervalMillSeconds());
            pAnimationPlayer->SetEasingFunctionType(m_pHotAnimationPlayer->GetEasingFunctionType());
        }
        m_pHotAnimationPlayer.reset(pAnimationPlayer);
    }
    else {
        m_pHotAnimationPlayer.reset();
    }
}

AnimationPlayer* Control::GetHotAnimationPlayer() const
{
    if (!GlobalManager::Instance().IsAnimationEnabled()) {
        return nullptr;
    }
    return m_pHotAnimationPlayer.get();
}

void Control::SetFadeHotFrameIntervalMillSeconds(int32_t frameIntervalMillSeconds)
{
    if (m_pHotAnimationPlayer == nullptr) {
        SetFadeHot(true);
    }
    ASSERT(m_pHotAnimationPlayer != nullptr);
    if (m_pHotAnimationPlayer != nullptr) {
        m_pHotAnimationPlayer->SetFrameIntervalMillSeconds(frameIntervalMillSeconds);
    }
}

int32_t Control::GetFadeHotFrameIntervalMillSeconds() const
{
    if (m_pHotAnimationPlayer != nullptr) {
        return m_pHotAnimationPlayer->GetFrameIntervalMillSeconds();
    }
    return -1;
}

void Control::SetFadeHotTotalMillSeconds(int32_t totalMillSeconds)
{
    if (m_pHotAnimationPlayer == nullptr) {
        SetFadeHot(true);
    }
    ASSERT(m_pHotAnimationPlayer != nullptr);
    if (m_pHotAnimationPlayer != nullptr) {
        m_pHotAnimationPlayer->SetTotalMillSeconds(totalMillSeconds);
    }
}

int32_t Control::GetFadeHotTotalMillSeconds() const
{
    if (m_pHotAnimationPlayer != nullptr) {
        return m_pHotAnimationPlayer->GetTotalMillSeconds();
    }
    return -1;
}

void Control::SetFadeHotEasingFunctionType(EasingFunctionType easingFunctionType)
{
    if (m_pHotAnimationPlayer == nullptr) {
        SetFadeHot(true);
    }
    ASSERT(m_pHotAnimationPlayer != nullptr);
    if (m_pHotAnimationPlayer != nullptr) {
        m_pHotAnimationPlayer->SetEasingFunctionType(easingFunctionType);
    }
}

EasingFunctionType Control::GetFadeHotEasingFunctionType() const
{
    if (m_pHotAnimationPlayer != nullptr) {
        return m_pHotAnimationPlayer->GetEasingFunctionType();
    }
    return EasingFunctionType::EaseInOutCubic;
}

bool Control::HasAnimationPlayer(AnimationType animationType) const
{
    if (!GlobalManager::Instance().IsAnimationEnabled()) {
        return false;
    }
    if (animationType == AnimationType::kAnimationHot) {
        return m_pHotAnimationPlayer != nullptr;
    }
    else if (m_pAnimationData != nullptr) {
        if (m_pAnimationData->m_animationManager != nullptr) {
            return m_pAnimationData->m_animationManager->HasAnimationPlayer(animationType);
        }
    }
    return false;
}

bool Control::IsAnimationPlayerPlaying(AnimationType animationType) const
{
    if (!GlobalManager::Instance().IsAnimationEnabled()) {
        return false;
    }
    if (animationType == AnimationType::kAnimationHot) {
        return (m_pHotAnimationPlayer != nullptr) && m_pHotAnimationPlayer->IsPlaying();
    }
    else if ((m_pAnimationData != nullptr) && (m_pAnimationData->m_animationManager != nullptr)) {
        AnimationPlayer* pAnimationPlayer = m_pAnimationData->m_animationManager->GetAnimationPlayer(animationType);
        if (pAnimationPlayer != nullptr) {
            return pAnimationPlayer->IsPlaying();
        }
    }
    return false;
}

AnimationManager& Control::GetAnimationManager()
{
    if (m_pAnimationData == nullptr) {
        m_pAnimationData = std::make_unique<TAnimationData>();
    }
    if (m_pAnimationData->m_animationManager == nullptr) {
        m_pAnimationData->m_animationManager = std::make_unique<AnimationManager>(this);
    }
    return *m_pAnimationData->m_animationManager;
}

DString Control::GetBkColor() const
{
    return (m_pColorData != nullptr) ? m_pColorData->m_strBkColor.c_str() : DString();
}

void Control::SetBkColor(const DString& strColor)
{
    ASSERT(strColor.empty() || HasUiColor(strColor));
    if (m_pColorData == nullptr) {
        m_pColorData = std::make_unique<TColorData>();
    }
    if (m_pColorData->m_strBkColor == strColor) {
        return;
    }
    m_pColorData->m_strBkColor = strColor;
    Invalidate();
}

void Control::SetBkColor(const UiColor& color)
{
    if (color.IsEmpty()) {
        SetBkColor(_T(""));
    }
    else {
        SetBkColor(GetColorString(color));
    }
}

void Control::SetBkColor2(const DString& strColor)
{
    ASSERT(strColor.empty() || HasUiColor(strColor));
    if (m_pColorData == nullptr) {
        m_pColorData = std::make_unique<TColorData>();
    }
    if (m_pColorData->m_strBkColor2 == strColor) {
        return;
    }
    m_pColorData->m_strBkColor2 = strColor;
    Invalidate();
}

void Control::SetBkColor2(const UiColor& color)
{
    if (color.IsEmpty()) {
        SetBkColor2(_T(""));
    }
    else {
        SetBkColor2(GetColorString(color));
    }
}

DString Control::GetBkColor2() const
{
    return (m_pColorData != nullptr) ? m_pColorData->m_strBkColor2.c_str() : DString();
}

void Control::SetBkColor2Direction(const DString& direction)
{
    int8_t nDirection = GetColor2Direction(direction);
    if (m_pColorData == nullptr) {
        m_pColorData = std::make_unique<TColorData>();
    }
    if (m_pColorData->m_nBkColor2Direction != nDirection) {
        m_pColorData->m_nBkColor2Direction = nDirection;
        Invalidate();
    }
}

DString Control::GetBkColor2Direction() const
{
    DString strBkColor2Direction = _T("1");
    if (m_pColorData != nullptr) {
        if (m_pColorData->m_nBkColor2Direction == 2) {
            strBkColor2Direction = _T("2");
        }
        else if (m_pColorData->m_nBkColor2Direction == 3) {
            strBkColor2Direction = _T("3");
        }
        else if (m_pColorData->m_nBkColor2Direction == 4) {
            strBkColor2Direction = _T("4");
        }
    }
    return strBkColor2Direction;
}

int8_t Control::GetColor2Direction(const UiString& bkColor2Direction) const
{
    int8_t nColor2Direction = 1;
    // Gradient background color
    if (bkColor2Direction == _T("2")) {
        nColor2Direction = 2;
    }
    else if (bkColor2Direction == _T("3")) {
        nColor2Direction = 3;
    }
    else if (bkColor2Direction == _T("4")) {
        nColor2Direction = 4;
    }
    return nColor2Direction;
}

DString Control::GetForeColor() const
{
    return (m_pColorData != nullptr) ? m_pColorData->m_strForeColor.c_str() : DString();
}

void Control::SetForeColor(const DString& strColor)
{
    ASSERT(strColor.empty() || HasUiColor(strColor));
    if (m_pColorData == nullptr) {
        m_pColorData = std::make_unique<TColorData>();
    }
    if (m_pColorData->m_strForeColor == strColor) {
        return;
    }
    m_pColorData->m_strForeColor = strColor;
    Invalidate();
}

void Control::SetForeColor(const UiColor& color)
{
    if (color.IsEmpty()) {
        SetForeColor(_T(""));
    }
    else {
        SetForeColor(GetColorString(color));
    }
}

DString Control::GetStateColor(ControlStateType stateType) const
{
    if (m_pColorMap != nullptr) {
        return m_pColorMap->GetStateColor(stateType);
    }
    return DString();
}

UiMargin Control::GetStateColorMargin(ControlStateType stateType) const
{
    if (m_pColorMap != nullptr) {
        return m_pColorMap->GetStateColorMargin(stateType);
    }
    return UiMargin();
}

UiSize Control::GetStateColorRound(ControlStateType stateType) const
{
    if (m_pColorMap != nullptr) {
        return m_pColorMap->GetStateColorRound(stateType);
    }
    return UiSize();
}

void Control::SetStateColor(ControlStateType stateType, const DString& strColor)
{
    ASSERT(strColor.empty() || HasUiColor(strColor));
    if (m_pColorMap != nullptr) {
        if (m_pColorMap->GetStateColor(stateType) == strColor) {
            return;
        }
    }
    if (m_pColorMap == nullptr) {
        m_pColorMap = std::make_unique<StateColorMap2>(this);
    }
    m_pColorMap->SetStateColor(stateType, strColor);
    if (stateType == kControlStateHot) {
        SetFadeHot(true);
    }
    Invalidate();
}

void Control::SetStateColorMargin(ControlStateType stateType, UiMargin colorMargin, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScaleMargin(colorMargin);
    }
    if (m_pColorMap != nullptr) {
        if (m_pColorMap->GetStateColorMargin(stateType).Equals(colorMargin)) {
            return;
        }
    }
    if (m_pColorMap == nullptr) {
        m_pColorMap = std::make_unique<StateColorMap2>(this);
    }
    m_pColorMap->SetStateColorMargin(stateType, colorMargin);
    if (stateType == kControlStateHot) {
        SetFadeHot(true);
    }
    Invalidate();
}

void Control::SetStateColorRound(ControlStateType stateType, UiSize colorRound, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScaleSize(colorRound);
    }
    if (m_pColorMap != nullptr) {
        if (m_pColorMap->GetStateColorRound(stateType).Equals(colorRound)) {
            return;
        }
    }
    if (m_pColorMap == nullptr) {
        m_pColorMap = std::make_unique<StateColorMap2>(this);
    }
    m_pColorMap->SetStateColorRound(stateType, colorRound);
    if (stateType == kControlStateHot) {
        SetFadeHot(true);
    }
    Invalidate();
}

DString Control::GetBkImage() const
{
    if (m_pBkImage != nullptr) {
        return m_pBkImage->GetImageString();
    }
    return DString();
}

std::string Control::GetUTF8BkImage() const
{
    std::string strOut = StringConvert::TToUTF8(GetBkImage());
    return strOut;
}

void Control::SetBkImage(const DString& strImage)
{
    if (!strImage.empty()) {
        if (m_pBkImage == nullptr) {
            m_pBkImage = std::make_unique<Image>();
            m_pBkImage->SetControl(this);
        }
    }
    bool bChanged = false;
    if (m_pBkImage != nullptr) {
        if (m_pBkImage->GetImageString() != strImage) {
            bChanged = true;
            if (!strImage.empty()) {
                m_pBkImage->SetImageString(strImage, Dpi());
            }
            else {
                m_pBkImage.reset();
            }
        }
    }
    if (bChanged) {
        RelayoutOrRedraw();
    }
}

void Control::SetUTF8BkImage(const std::string& strImage)
{
    DString strOut = StringConvert::UTF8ToT(strImage);
    SetBkImage(strOut);
}

bool Control::SetLoadingAttribute(const DString& loadingAttribute)
{
    bool bRet = false;
    if (!loadingAttribute.empty()) {
        if (m_pOtherData == nullptr) {
            m_pOtherData = std::make_unique<TOtherData>();
        }
        if (m_pOtherData->m_pLoading == nullptr) {
            m_pOtherData->m_pLoading = std::make_unique<ControlLoading>(this);
        }
        else {
            if (m_pOtherData->m_pLoading->IsLoading()) {
                m_pOtherData->m_pLoading->StopLoading();
            }
        }
        bRet = m_pOtherData->m_pLoading->SetLoadingAttribute(loadingAttribute);
        if (!bRet) {
            m_pOtherData->m_pLoading.reset();
        }
    }
    else {
        bRet = true;
        if (m_pOtherData != nullptr) {
            m_pOtherData->m_pLoading.reset();
        }       
    }
    return bRet;
}

bool Control::StartLoading(int32_t nIntervalMs, int32_t nMaxCount)
{
    bool bRet = false;
    ASSERT((m_pOtherData != nullptr) && (m_pOtherData->m_pLoading != nullptr));
    if ((m_pOtherData != nullptr) && (m_pOtherData->m_pLoading != nullptr)) {
        bRet = m_pOtherData->m_pLoading->StartLoading(nIntervalMs, nMaxCount);
    }
    if (bRet) {
        SetEnabled(false);
        ASSERT(GetLoadingUiRootBox() != nullptr);
    }
    return bRet;
}

void Control::StopLoading()
{
    if ((m_pOtherData != nullptr) && (m_pOtherData->m_pLoading != nullptr)) {
        m_pOtherData->m_pLoading->StopLoading();
    }
    SetEnabled(true);
}

bool Control::IsLoading() const
{
    bool bRet = false;
    if ((m_pOtherData != nullptr) && (m_pOtherData->m_pLoading != nullptr)) {
        bRet = m_pOtherData->m_pLoading->IsLoading();
    }
    return bRet;
}

Box* Control::GetLoadingUiRootBox() const
{
    Box* pLoadingUiRootBox = nullptr;
    if ((m_pOtherData != nullptr) && (m_pOtherData->m_pLoading != nullptr)) {
        pLoadingUiRootBox = m_pOtherData->m_pLoading->GetLoadingUiRootBox();
    }
    return pLoadingUiRootBox;
}

bool Control::HasStateImages(void) const
{
    if (m_pImageMap != nullptr) {
        return m_pImageMap->HasStateImages();
    }
    return false;
}

bool Control::HasStateImage(StateImageType stateImageType) const
{
    if (m_pImageMap != nullptr) {
        return m_pImageMap->HasStateImage(stateImageType);
    }
    return false;
}

DString Control::GetStateImage(StateImageType imageType, ControlStateType stateType) const
{
    if (m_pImageMap != nullptr) {
        return m_pImageMap->GetImageString(imageType, stateType);
    }
    return DString();
}

void Control::SetStateImage(StateImageType imageType, ControlStateType stateType, const DString& strImage)
{
    if (m_pImageMap == nullptr) {
        m_pImageMap = std::make_unique<StateImageMap>();
        m_pImageMap->SetControl(this);
    }
    m_pImageMap->SetImageString(imageType, stateType, strImage, Dpi());
}

bool Control::PaintStateImage(IRender* pRender, StateImageType stateImageType, 
                              ControlStateType stateType, 
                              const DString& sImageModify,
                              UiRect* pDestRect)
{
    if (m_pImageMap != nullptr) {
        return m_pImageMap->PaintStateImage(pRender, stateImageType, stateType, sImageModify, pDestRect);
    }
    return false;
}

UiSize Control::GetStateImageSize(StateImageType imageType, ControlStateType stateType)
{
    Image* pImage = nullptr;
    if (m_pImageMap != nullptr) {
        pImage = m_pImageMap->GetStateImage(imageType, stateType);
    }
    UiSize imageSize;
    if (pImage != nullptr) {
        LoadImageInfo(*pImage);
        std::shared_ptr<ImageInfo> imageInfo = pImage->GetImageInfo();
        if (imageInfo != nullptr) {
            imageSize.cx = imageInfo->GetWidth();
            imageSize.cy = imageInfo->GetHeight();
        }
    }
    return imageSize;
}

void Control::ClearStateImages()
{
    m_pImageMap.reset();
    RelayoutOrRedraw();
}

DString Control::GetStateImage(ControlStateType stateType) const
{
    return GetStateImage(kStateImageBk, stateType);
}

void Control::SetStateImage(ControlStateType stateType, const DString& strImage)
{
    if (stateType == kControlStateHot) {
        SetFadeHot(true);
    }
    SetStateImage(kStateImageBk, stateType, strImage);
    RelayoutOrRedraw();
}

DString Control::GetForeStateImage(ControlStateType stateType) const
{
    return GetStateImage(kStateImageFore, stateType);
}

void Control::SetForeStateImage(ControlStateType stateType, const DString& strImage)
{
    if (stateType == kControlStateHot) {
        SetFadeHot(true);
    }
    SetStateImage(kStateImageFore, stateType, strImage);
    Invalidate();
}

bool Control::AdjustStateImagesMarginLeft(int32_t leftOffset, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScaleInt(leftOffset);
    }
    if (leftOffset == 0) {
        return false;
    }
    std::vector<Image*> allImages;
    if (m_pImageMap != nullptr) {
        m_pImageMap->GetAllImages(allImages);
    }
    bool bSetOk = false;
    UiMargin rcMargin;
    for (Image* pImage : allImages) {
        if (pImage == nullptr) {
            continue;
        }
        rcMargin = pImage->GetImageMargin(Dpi());
        rcMargin.left += leftOffset;
        if (rcMargin.left < 0) {
            rcMargin.left = 0;
        }
        if (!pImage->GetImageMargin(Dpi()).Equals(rcMargin)) {
            pImage->SetImageMargin(rcMargin, false, Dpi());
            bSetOk = true;
        }
    }
    if (bSetOk) {
        Invalidate();
    }
    return bSetOk;
}

UiMargin Control::GetBkImageMargin() const
{
    UiMargin rcMargin;
    if (m_pBkImage != nullptr) {
        rcMargin = m_pBkImage->GetImageMargin(Dpi());
    }
    return rcMargin;
}

bool Control::SetBkImageMargin(UiMargin rcMargin, bool bNeedDpiScale)
{
    bool bSetOk = false;
    if (m_pBkImage != nullptr) {
        if (bNeedDpiScale) {
            Dpi().ScaleMargin(rcMargin);
        }
        if (!m_pBkImage->GetImageMargin(Dpi()).Equals(rcMargin)) {
            m_pBkImage->SetImageMargin(rcMargin, false, Dpi());
            bSetOk = true;
            Invalidate();
        }        
    }
    return bSetOk;
}

bool Control::IsBkImagePaintEnabled() const
{
    if (m_pBkImage != nullptr) {
        return m_pBkImage->IsImagePaintEnabled();
    }
    return true;
}

void Control::SetBkImagePaintEnabled(bool bEnable)
{
    if (m_pBkImage != nullptr) {
        bool isChange = m_pBkImage->IsImagePaintEnabled() != bEnable;        
        if (isChange) {
            m_pBkImage->SetImagePaintEnabled(bEnable);
            Invalidate();
        }
    }
}

DString Control::GetBkImagePath() const
{
    if (m_pBkImage != nullptr) {
        return m_pBkImage->GetImagePath();
    }
    return DString();
}

UiSize Control::GetBkImageSize() const
{
    UiSize imageSize;
    if (m_pBkImage != nullptr) {
        LoadImageInfo(*m_pBkImage);
        std::shared_ptr<ImageInfo> imageInfo = m_pBkImage->GetImageInfo();
        if (imageInfo != nullptr) {
            imageSize.cx = imageInfo->GetWidth();
            imageSize.cy = imageInfo->GetHeight();
        }
    }
    return imageSize;
}

ControlStateType Control::GetState() const
{
    return static_cast<ControlStateType>(m_controlState);
}

void Control::SetState(ControlStateType controlState)
{
    if (controlState == kControlStateNormal) {
        m_nHotAlpha = 0;
    }
    else if (controlState == kControlStateHot) {
        m_nHotAlpha = 255;
    }
    PrivateSetState(controlState);
    Invalidate();
}

void Control::PrivateSetState(ControlStateType controlState)
{
    if (GetState() != controlState) {
        ControlStateType oldState = GetState();
        m_controlState = TruncateToInt8(controlState);
        SendEvent(kEventStateChanged, controlState, oldState);
        Invalidate();
    }
}

bool Control::IsHotState() const
{
    return (GetState() == kControlStateHot) ? true : false;
}

DString Control::GetBorderColor(ControlStateType stateType) const
{
    DString borderColor;
    if ((m_pBorderData != nullptr) && (m_pBorderData->m_pBorderColorMap != nullptr)) {
        borderColor = m_pBorderData->m_pBorderColorMap->GetStateColor(stateType);
    }
    return borderColor;
}

void Control::SetBorderColor(const DString& strBorderColor)
{
    SetBorderColor(kControlStateNormal, strBorderColor);
    SetBorderColor(kControlStateHot, strBorderColor);
    SetBorderColor(kControlStatePushed, strBorderColor);
    SetBorderColor(kControlStateDisabled, strBorderColor);
}

void Control::SetBorderColor(ControlStateType stateType, const DString& strBorderColor)
{
    if (m_pBorderData == nullptr) {
        m_pBorderData = std::make_unique<TBorderData>();
    }
    if (m_pBorderData->m_pBorderColorMap == nullptr) {
        m_pBorderData->m_pBorderColorMap = std::make_unique<StateColorMap>(this);
    }
    if (GetBorderColor(stateType) != strBorderColor) {
        m_pBorderData->m_pBorderColorMap->SetStateColor(stateType, strBorderColor);
        Invalidate();
    }
}

void Control::SetFocusBorderColor(const DString& strBorderColor)
{
    if (m_pBorderData == nullptr) {
        m_pBorderData = std::make_unique<TBorderData>();
    }
    if (m_pBorderData->m_focusBorderColor != strBorderColor) {
        m_pBorderData->m_focusBorderColor = strBorderColor;
        Invalidate();
    }
}

DString Control::GetFocusBorderColor() const
{
    if (m_pBorderData != nullptr) {
        return m_pBorderData->m_focusBorderColor.c_str();
    }
    return DString();
}

void Control::SetBorderSize(UiRectF rc, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        rc.left = Dpi().GetScaleFloat(rc.left);
        rc.top = Dpi().GetScaleFloat(rc.top);
        rc.right = Dpi().GetScaleFloat(rc.right);
        rc.bottom = Dpi().GetScaleFloat(rc.bottom);
    }
    rc.left = std::max(rc.left, 0.0f);
    rc.top = std::max(rc.top, 0.0f);
    rc.right = std::max(rc.right, 0.0f);
    rc.bottom = std::max(rc.bottom, 0.0f);

    if (m_pBorderData == nullptr) {
        m_pBorderData = std::make_unique<TBorderData>();
    }
    if (m_pBorderData->m_rcBorderSize != rc) {
        m_pBorderData->m_rcBorderSize = rc;
        Invalidate();
    }    
}

UiRectF Control::GetBorderSize() const
{
    return (m_pBorderData != nullptr) ? m_pBorderData->m_rcBorderSize : UiRectF();
}

float Control::GetLeftBorderSize() const
{
    return (m_pBorderData != nullptr) ? m_pBorderData->m_rcBorderSize.left : 0.0f;
}

void Control::SetLeftBorderSize(float fSize, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        fSize = Dpi().GetScaleFloat(fSize);
    }

    if (m_pBorderData == nullptr) {
        m_pBorderData = std::make_unique<TBorderData>();
    }
    if (m_pBorderData->m_rcBorderSize.left != fSize) {
        m_pBorderData->m_rcBorderSize.left = fSize;
        Invalidate();
    }    
}

float Control::GetTopBorderSize() const
{
    return (m_pBorderData != nullptr) ? m_pBorderData->m_rcBorderSize.top : 0.0f;
}

void Control::SetTopBorderSize(float fSize, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        fSize = Dpi().GetScaleFloat(fSize);
    }

    if (m_pBorderData == nullptr) {
        m_pBorderData = std::make_unique<TBorderData>();
    }
    if (m_pBorderData->m_rcBorderSize.top != fSize) {
        m_pBorderData->m_rcBorderSize.top = fSize;
        Invalidate();
    }
}

float Control::GetRightBorderSize() const
{
    return (m_pBorderData != nullptr) ? m_pBorderData->m_rcBorderSize.right : 0.0f;
}

void Control::SetRightBorderSize(float fSize, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        fSize = Dpi().GetScaleFloat(fSize);
    }

    if (m_pBorderData == nullptr) {
        m_pBorderData = std::make_unique<TBorderData>();
    }
    if (m_pBorderData->m_rcBorderSize.right != fSize) {
        m_pBorderData->m_rcBorderSize.right = fSize;
        Invalidate();
    }    
}

float Control::GetBottomBorderSize() const
{
    return  (m_pBorderData != nullptr) ? m_pBorderData->m_rcBorderSize.bottom : 0.0f;
}

void Control::SetBottomBorderSize(float fSize, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        fSize = Dpi().GetScaleFloat(fSize);
    }

    if (m_pBorderData == nullptr) {
        m_pBorderData = std::make_unique<TBorderData>();
    }
    if (m_pBorderData->m_rcBorderSize.bottom != fSize) {
        m_pBorderData->m_rcBorderSize.bottom = fSize;
        Invalidate();
    }    
}

void Control::SetBorderDashStyle(int8_t borderDashStyle)
{
    IPen::DashStyle dashStyle = IPen::kDashStyleSolid;
    if (borderDashStyle == IPen::kDashStyleDash) {
        dashStyle = IPen::kDashStyleDash;
    }
    else if (borderDashStyle == IPen::kDashStyleDot) {
        dashStyle = IPen::kDashStyleDot;
    }
    else if (borderDashStyle == IPen::kDashStyleDashDot) {
        dashStyle = IPen::kDashStyleDashDot;
    }
    else if (borderDashStyle == IPen::kDashStyleDashDotDot) {
        dashStyle = IPen::kDashStyleDashDotDot;
    }

    if (m_pBorderData == nullptr) {
        m_pBorderData = std::make_unique<TBorderData>();
    }
    if (m_pBorderData->m_borderDashStyle != dashStyle) {
        m_pBorderData->m_borderDashStyle = (int8_t)dashStyle;
        Invalidate();
    }
}

int8_t Control::GetBorderDashStyle() const
{
    if (m_pBorderData != nullptr) {
        return m_pBorderData->m_borderDashStyle;
    }
    return 0;
}

void Control::SetBordersOnTop(bool bBordersOnTop)
{
    if (m_bBordersOnTop != bBordersOnTop) {
        m_bBordersOnTop = bBordersOnTop;
        Invalidate();
    }
}

bool Control::IsBordersOnTop() const
{
    return m_bBordersOnTop;
}

bool Control::GetBorderRound(float& fRoundWidth, float& fRoundHeight) const
{
    fRoundWidth = 0.0f;
    fRoundHeight = 0.0f;
    if (HasBorderRound() && (m_pBorderData != nullptr)) {
        fRoundWidth = Dpi().GetScaleFloat(m_pBorderData->m_borderRound.cx);
        fRoundHeight = Dpi().GetScaleFloat(m_pBorderData->m_borderRound.cy);
        return true;
    }
    return false;
}

bool Control::HasBorderRound() const
{
    return (m_pBorderData != nullptr) && (m_pBorderData->m_borderRound.cx > 0) && (m_pBorderData->m_borderRound.cy > 0);
}

void Control::SetBorderRound(UiSize borderRound)
{
    int32_t cx = borderRound.cx;
    int32_t cy = borderRound.cy;
    ASSERT(cx >= 0);
    ASSERT(cy >= 0);
    if ((cx < 0) || (cy < 0)) {
        return;
    }
    // The two parameters must both be 0 or both be greater than 0, otherwise the parameters are invalid
    ASSERT(((cx > 0) && (cy > 0)) || ((cx == 0) && (cy == 0)));
    if (cx == 0) {
        if (cy != 0) {
            return;
        }
    }
    else {
        if (cy == 0) {
            return;
        }
    }

    if (m_pBorderData == nullptr) {
        m_pBorderData = std::make_unique<TBorderData>();
    }
    UiSize16& borderRoundData = m_pBorderData->m_borderRound;
    if ((borderRoundData.cx != borderRound.cx) || (borderRoundData.cy != borderRound.cy)) {
        borderRoundData.cx = ui::TruncateToInt16(borderRound.cx);
        borderRoundData.cy = ui::TruncateToInt16(borderRound.cy);
        Invalidate();
    }
}

void Control::SetBoxShadow(const DString& strShadow)
{
    if (strShadow.empty()) {
        return;
    }
    if (m_pOtherData == nullptr) {
        m_pOtherData = std::make_unique<TOtherData>();
    }
    if (m_pOtherData->m_pBoxShadow == nullptr) {
        m_pOtherData->m_pBoxShadow = std::make_unique<BoxShadow>(this);
    }
    m_pOtherData->m_pBoxShadow->SetBoxShadowString(strShadow);
}

CursorType Control::GetCursorType() const
{
    return m_cursorType;
}

void Control::SetCursorType(CursorType cursorType)
{
    m_cursorType = cursorType;
}

DString Control::GetToolTipText() const
{
    DString strText;
    if ((m_pOtherData != nullptr) && (m_pOtherData->m_pTooltip != nullptr)) {
        strText = m_pOtherData->m_pTooltip->m_sToolTipText.c_str();
        if (strText.empty() && !m_pOtherData->m_pTooltip->m_sToolTipTextId.empty()) {
            strText = GlobalManager::Instance().Lang().GetStringViaID(m_pOtherData->m_pTooltip->m_sToolTipTextId.c_str());
        }
    }
    return strText;
}

std::string Control::GetUTF8ToolTipText() const
{
    std::string strOut = StringConvert::TToUTF8(GetToolTipText());
    return strOut;
}

void Control::SetToolTipText(const DString& strText)
{
    if (m_pOtherData == nullptr) {
        m_pOtherData = std::make_unique<TOtherData>();
    }
    if (m_pOtherData->m_pTooltip == nullptr) {
        m_pOtherData->m_pTooltip = std::make_unique<TTooltipData>();
    }
    if (strText != m_pOtherData->m_pTooltip->m_sToolTipText) {
        DString strTemp(strText);
        StringUtil::ReplaceAll(_T("<n>"), _T("\r\n"), strTemp);
        m_pOtherData->m_pTooltip->m_sToolTipText = strTemp;
        Invalidate();

        if (GetWindow() != nullptr) {
            Control* pHover = GetWindow()->GetHoverControl();
            if (pHover == this) {
                // Update the ToolTip display
                GetWindow()->UpdateToolTip();
            }
        }
    }    
}

void Control::SetUTF8ToolTipText(const std::string& strText)
{
    DString strOut = StringConvert::UTF8ToT(strText);
    SetToolTipText(strOut);
}

void Control::SetToolTipTextId(const DString& strTextId)
{
    if (m_pOtherData == nullptr) {
        m_pOtherData = std::make_unique<TOtherData>();
    }
    if (m_pOtherData->m_pTooltip == nullptr) {
        m_pOtherData->m_pTooltip = std::make_unique<TTooltipData>();
    }

    if (m_pOtherData->m_pTooltip->m_sToolTipTextId != strTextId) {
        m_pOtherData->m_pTooltip->m_sToolTipTextId = strTextId;
        Invalidate();

        if (GetWindow() != nullptr) {
            Control* pHover = GetWindow()->GetHoverControl();
            if (pHover == this) {
                // Update the ToolTip display
                GetWindow()->UpdateToolTip();
            }
        }
    }
}

void Control::SetUTF8ToolTipTextId(const std::string& strTextId)
{
    DString strOut = StringConvert::UTF8ToT(strTextId);
    SetToolTipTextId(strOut);
}

void Control::SetToolTipWidth(int32_t nWidth, bool bNeedDpiScale)
{
    if (nWidth < 0) {
        nWidth = 0;
    }
    if (bNeedDpiScale) {
        Dpi().ScaleInt(nWidth);
    }
    if (m_pOtherData == nullptr) {
        m_pOtherData = std::make_unique<TOtherData>();
    }
    if (m_pOtherData->m_pTooltip == nullptr) {
        m_pOtherData->m_pTooltip = std::make_unique<TTooltipData>();
    }
    m_pOtherData->m_pTooltip->m_nTooltipWidth = nWidth;
}

int32_t Control::GetToolTipWidth(void) const
{
    int32_t nTooltipWidth = 0;
    if ((m_pOtherData != nullptr) && (m_pOtherData->m_pTooltip != nullptr)) {
        nTooltipWidth = m_pOtherData->m_pTooltip->m_nTooltipWidth;
    }
    return nTooltipWidth;
}

void Control::SetContextMenuUsed(bool bMenuUsed)
{
    m_bContextMenuUsed = bMenuUsed;
}

DString Control::GetDataID() const
{
    return m_sUserDataID.c_str();
}

std::string Control::GetUTF8DataID() const
{
    std::string strOut = StringConvert::TToUTF8(GetDataID());
    return strOut;
}

void Control::SetDataID(const DString& strText)
{
    m_sUserDataID = strText;
}

void Control::SetUTF8DataID(const std::string& strText)
{
    m_sUserDataID = StringConvert::UTF8ToT(strText);
}

void Control::SetUserDataID(size_t dataID)
{
    m_uUserDataID = dataID;
}

size_t Control::GetUserDataID() const
{
    return m_uUserDataID;
}

void Control::SetFadeVisible(bool bVisible)
{
    // Show or hide the control in an animated way
    if (!GlobalManager::Instance().IsAnimationEnabled()) {
        // Animation feature disabled
        SetVisible(bVisible);
    }
    else {
        // Animation feature enabled
        if (bVisible) {
            GetAnimationManager().Appear();
        }
        else {
            GetAnimationManager().Disappear();
        }
    }
}

void Control::OnSetVisible(bool bChanged)
{
    BaseClass::OnSetVisible(bChanged);
    const bool bVisible = IsVisible();
    if (!bVisible) {
        // Ensure this control is no longer the focus control
        EnsureNoFocus();

        // Pause the animation playback of this control
        PauseImageAnimation();
    }
    if (bChanged) {
        // Let the parent container relayout
        ArrangeAncestor();

        // Finally, fire the visible state changed event to notify the application layer
        WPARAM wParam = bVisible ? 1 : 0;
        SendEvent(kEventVisibleChanged, wParam);
    }
}

void Control::OnSetEnabled(bool bChanged)
{
    BaseClass::OnSetEnabled(bChanged);
    if (IsEnabled()) {
        PrivateSetState(kControlStateNormal);
        m_nHotAlpha = 0;
    }
    else {
        PrivateSetState(kControlStateDisabled);
    }

    if (!IsEnabled()) {
        PauseImageAnimation();
    }
    if (bChanged) {
        Invalidate();
    }    
}

bool Control::IsFocused() const
{
    Window* pWindow = GetWindow();
    return ((pWindow != nullptr) && (pWindow->GetFocusControl() == this) );
}

void Control::SetFocus()
{
    if (m_bNoFocus) {
        return;
    }
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        pWindow->SetFocusControl(this);
    }
}

uint32_t Control::GetControlFlags() const
{
    return IsAllowTabStop() ? UIFLAG_TABSTOP : UIFLAG_DEFAULT;
}

void Control::SetNoFocus()
{
    m_bNoFocus = true;
    EnsureNoFocus();
}

void Control::SetShowFocusRect(bool bShowFocusRect)
{
    m_bShowFocusRect = bShowFocusRect;
}

bool Control::IsShowFocusRect() const
{
    return m_bShowFocusRect;
}

void Control::SetFocusRectColor(const DString& focusRectColor)
{
    if (m_pColorData == nullptr) {
        m_pColorData = std::make_unique<TColorData>();
    }
    if (m_pColorData->m_focusRectColor == focusRectColor) {
        return;
    }
    m_pColorData->m_focusRectColor = focusRectColor;
    Invalidate();
}

DString Control::GetFocusRectColor() const
{
    if (m_pColorData != nullptr) {
        return m_pColorData->m_focusRectColor.c_str();
    }
    return DString();
}

void Control::Activate(const EventArgs* /*pMsg*/)
{
}

bool Control::IsActivatable() const
{
    if (!IsVisible() || !IsEnabled()) {
        return false;
    }
    return true;
}

Control* Control::FindControl(FINDCONTROLPROC Proc, void* pProcData,
                              uint32_t uFlags, const UiPoint& ptMouse,
                              const UiPoint& scrollPos)
{
    if (Proc == nullptr) {
        return nullptr;
    }
    if ((uFlags & UIFIND_VISIBLE) != 0 && !IsVisible()) {
        return nullptr;
    }
    if ((uFlags & UIFIND_ENABLED) != 0 && !IsEnabled()) {
        return nullptr;
    }
    UiPoint pt(ptMouse);
    pt.Offset(scrollPos);
#ifdef _DEBUG
    if (((uFlags & UIFIND_HITTEST) != 0) && ((uFlags & UIFIND_DRAG_DROP) == 0) && (pProcData != nullptr)) {
        if (!IsFloat()) {
            UiPoint ptOrg(*(UiPoint*)pProcData);
            ptOrg.Offset(GetScrollOffsetInScrollBox());
            ASSERT(ptOrg == pt);
        }
    }
#endif // _DEBUG
    if ((uFlags & UIFIND_TOOLTIP) == 0) {
        if ((uFlags & UIFIND_HITTEST) != 0 &&
            (!IsMouseEnabled() || !GetRect().ContainsPt(pt))) {
            return nullptr;
        }
    }
    else {
        if ((uFlags & UIFIND_HITTEST) != 0 && !GetRect().ContainsPt(pt)) {
            return nullptr;
        }
    }
    return Proc(this, pProcData);
}

Control* Control::FindControl(const DString& name)
{
    Window* pWindow = GetWindow();
    ASSERT(pWindow != nullptr);
    if (pWindow != nullptr) {
        return pWindow->FindSubControlByName(this, name);
    }
    return nullptr;
}

UiRect Control::GetPos() const
{
    return BaseClass::GetPos();
}

void Control::SetPos(UiRect rc)
{
    // There is a lot of similar code: SetPos(GetPos()), which means setting the position and repainting
    rc.Validate();
    SetArranged(false);
    std::weak_ptr<WeakFlag> weakFlag = GetWeakFlag();
    bool bPosChanged = (GetRect().Left() != rc.Left()) || (GetRect().Top() != rc.Top());
    bool bSizeChanged = (GetRect().Width() != rc.Width()) || (GetRect().Height() != rc.Height());

    UiRect rcOldRect = GetRect();
    if (rcOldRect.IsEmpty()) {
        rcOldRect = rc;// avoid being empty
    }
    // If there is a box-shadow, the area it expands for painting needs to be included
    rcOldRect = GetBoxShadowExpandedRect(rcOldRect);

    SetRect(rc);
    if (GetWindow() == nullptr) {
        return;
    }
    UiRect rcNewRect = GetRect();
    rcNewRect = GetBoxShadowExpandedRect(rcNewRect);

    UiRect rcInvalidateRect = rcOldRect;
    rcInvalidateRect.Union(rcNewRect);// The union of the old and new rectangle ranges; both need to be marked as dirty areas

    bool needInvalidate = true;
    UiRect rcTemp;
    UiRect rcParent;
    UiPoint offset = GetScrollOffsetInScrollBox();
    rcInvalidateRect.Offset(-offset.x, -offset.y);// Convert to client area coordinates within the window
    Control* pParent = GetParent();
    while (pParent != nullptr) {
        rcTemp = rcInvalidateRect;
        rcParent = pParent->GetPos();
        rcParent = pParent->GetBoxShadowExpandedRect(rcParent);
        UiPoint offsetParent = pParent->GetScrollOffsetInScrollBox();
        rcParent.Offset(-offsetParent.x, -offsetParent.y);// Convert to client area coordinates within the window
        if (!UiRect::Intersect(rcInvalidateRect, rcTemp, rcParent)) {
            needInvalidate = false;
            break;
        }
        pParent = pParent->GetParent();
    }
    if (needInvalidate && (GetWindow() != nullptr)) {
        GetWindow()->Invalidate(rcInvalidateRect);
    }

    if ((m_pOtherData != nullptr) && (m_pOtherData->m_pLoading != nullptr)) {
        m_pOtherData->m_pLoading->UpdateLoadingPos();
    }
    if (bPosChanged && !weakFlag.expired()) {
        SendEvent(kEventPosChanged);
    }
    if (bSizeChanged && !weakFlag.expired()) {
        SendEvent(kEventSizeChanged);
    }
}

bool Control::PreEstimateSize(UiSize& szAvailable, UiFixedSize& fixedSize, UiEstSize& returnEstSize) const
{
    fixedSize = GetFixedSize();
    if (!fixedSize.cx.IsAuto() && !fixedSize.cy.IsAuto()) {
        // If neither width nor height is an auto property, return directly
        returnEstSize = MakeEstSize(fixedSize);
        return false;
    }
    if (fixedSize.cx.IsInt32()) {
        // The width of this control is a fixed value: set it to the value used in the actual layout
        int32_t cx = std::clamp(fixedSize.cx.GetInt32(), GetMinWidth(), GetMaxWidth());
        szAvailable.cx = cx;
    }
    if (fixedSize.cy.IsInt32()) {
        // The height of this control is a fixed value: set it to the value used in the actual layout
        int32_t cy = std::clamp(fixedSize.cy.GetInt32(), GetMinHeight(), GetMaxHeight());
        szAvailable.cy = cy;
    }
    szAvailable.Validate();
    if (!IsReEstimateSize(szAvailable)) {
        // Use the estimated result from the cache
        returnEstSize = GetEstimateSize();
        return false;
    }
    return true;
}

UiEstSize Control::EstimateSize(UiSize szAvailable)
{
    UiFixedSize fixedSize;
    UiEstSize returnEstSize;
    if (!PreEstimateSize(szAvailable, fixedSize, returnEstSize)) {
        return returnEstSize;
    }

    // Set the type of the estimated image width and height to optimize performance (after some attributes are set, the image can be loaded lazily)
    EstimateImageType estImageType = EstimateImageType::kBoth;
    if (!fixedSize.cx.IsAuto() || !fixedSize.cy.IsAuto()) {
        if (fixedSize.cx.IsAuto()) {
            estImageType = EstimateImageType::kWidthOnly;
        }
        else {
            estImageType = EstimateImageType::kHeightOnly;
        }
    }
    UiSize szControlSize = EstimateControlSize(szAvailable, estImageType);

    // Take the maximum of the image and text area heights and widths
    if (fixedSize.cx.IsAuto()) {
        fixedSize.cx.SetInt32(szControlSize.cx);
    }
    if (fixedSize.cy.IsAuto()) {
        fixedSize.cy.SetInt32(szControlSize.cy);
    }
    // Keep the result in the cache to avoid re-estimating every time
    UiEstSize estSize = MakeEstSize(fixedSize);
    SetEstimateSize(estSize, szAvailable);
    SetReEstimateSize(false);
    return estSize;
}

UiSize Control::EstimateControlSize(UiSize szAvailable, EstimateImageType estImageType)
{
    // Estimate the image area size
    UiSize imageSize = EstimateImage(szAvailable, estImageType);

    // Estimate the text area size; when the function calculates, the inner padding is already included
    UiSize textSize = EstimateText(szAvailable);

    UiSize szControlSize;
    szControlSize.cx = std::max(imageSize.cx, textSize.cx);
    szControlSize.cy = std::max(imageSize.cy, textSize.cy);
    return szControlSize;
}

UiSize Control::EstimateText(UiSize /*szAvailable*/)
{
    return UiSize(0, 0);
}

UiSize Control::EstimateImage(UiSize szAvailable, EstimateImageType estImageType)
{
    UiSize imageSize;
    Image* image = GetEstimateImage();
    uint32_t nImageInfoWidth = 0;
    uint32_t nImageInfoHeight = 0;
    if (image != nullptr) {
        bool bNeedLoadImage = true;// Whether the image needs to be loaded
        ImageLoadParam loadParam = image->GetImageLoadParam();        
        loadParam.GetImageFixedSize(nImageInfoWidth, nImageInfoHeight);
        if (estImageType == EstimateImageType::kWidthOnly) {
            if (nImageInfoWidth > 0) {
                bNeedLoadImage = false;
            }
        }
        else if (estImageType == EstimateImageType::kHeightOnly) {
            if (nImageInfoHeight > 0) {
                bNeedLoadImage = false;
            }
        }
        else {
            if ((nImageInfoWidth > 0) && (nImageInfoHeight > 0)) {
                bNeedLoadImage = false;
            }
        }
        // Load the image: the width and height of the image need to be obtained
        if (bNeedLoadImage) {
            LoadImageInfo(*image);
            std::shared_ptr<ImageInfo> imageInfo = image->GetImageInfo();
            if (imageInfo != nullptr) {
                nImageInfoWidth = imageInfo->GetWidth();
                nImageInfoHeight = imageInfo->GetHeight();
            }
        }
        else {
            if (nImageInfoWidth == 0) {
                nImageInfoWidth = nImageInfoHeight;// redundant setting; the width is actually not needed
            }
            if (nImageInfoHeight == 0) {
                nImageInfoHeight = nImageInfoWidth;// redundant setting; the height is actually not needed
            }
        }
    }
    // The inner padding of the control itself
    const UiPadding rcControlPadding = GetControlPadding();

    if ((nImageInfoWidth > 0) && (nImageInfoHeight > 0) && (image != nullptr)) {
        ImageAttribute imageAttribute = image->GetImageAttribute();
        UiRect rcDest;
        bool hasDestAttr = false;
        UiRect rcImageDestRect = imageAttribute.GetImageDestRect(nImageInfoWidth, nImageInfoHeight, Dpi());
        if (ImageAttribute::HasValidImageRect(rcImageDestRect)) {
            // Use the target area specified in the configuration (DPI-adaptive already applied per the configuration): use it as the primary basis for the image size
            rcDest = rcImageDestRect;
            if (rcDest.left < 0) {
                rcDest.left = 0;
            }
            if (rcDest.top < 0) {
                rcDest.top = 0;
            }
            hasDestAttr = true;
        }
        UiRect rcSource = imageAttribute.GetImageSourceRect();
        if (imageAttribute.m_bImageDpiScaleEnabled) {
            // This image supports DPI adaptation
            Dpi().ScaleRect(rcSource);
        }        
        if (rcSource.right > (int32_t)nImageInfoWidth) {
            rcSource.right = (int32_t)nImageInfoWidth;
        }
        if (rcSource.bottom > (int32_t)nImageInfoHeight) {
            rcSource.bottom = (int32_t)nImageInfoHeight;
        }

        if (rcDest.Width() > 0) {
            // With 0 as the base point and right as the boundary
            imageSize.cx = rcDest.right;
        }
        else if (rcSource.Width() > 0) {
            imageSize.cx = rcSource.Width();
        }
        else {
            imageSize.cx = nImageInfoWidth;
        }

        if (rcDest.Height() > 0) {
            // With 0 as the base point and bottom as the boundary
            imageSize.cy = rcDest.bottom;
        }
        else if (rcSource.Height() > 0) {
            imageSize.cy = rcSource.Height();
        }
        else {
            imageSize.cy = nImageInfoHeight;
        }
        if (!hasDestAttr) {
            // If there is no rcDest attribute, the image margin needs to be added (the margin attribute of the image itself)
            UiMargin rcImageMargin = imageAttribute.GetImageMargin(Dpi());
            imageSize.cx += (rcImageMargin.left + rcImageMargin.right);
            imageSize.cy += (rcImageMargin.top + rcImageMargin.bottom);
        }
        if (imageAttribute.m_bAdaptiveDestRect) {
            // Automatically adapt to the target area (scale the image proportionally): adjust the paint area according to the image size
            const int32_t nImageWidth = rcSource.Width();
            const int32_t nImageHeight = rcSource.Height();
            UiRect rcControlDest = UiRect(0, 0,
                                          szAvailable.cx - rcControlPadding.left - rcControlPadding.right,
                                          szAvailable.cy - rcControlPadding.top - rcControlPadding.bottom);
            rcControlDest.Validate();
            if (rcControlDest.Width() > 0 && rcControlDest.Height() > 0) {
                rcControlDest = ImageAttribute::CalculateAdaptiveRect(nImageWidth, nImageHeight,
                                                                      rcControlDest,
                                                                      imageAttribute.m_hAlign.c_str(),
                                                                      imageAttribute.m_vAlign.c_str());
                imageSize.cx = rcControlDest.Width();
                imageSize.cy = rcControlDest.Height();
            }
        }
    }

    // The image size needs to include the inner padding of the control
    if (imageSize.cx > 0) {
        imageSize.cx += (rcControlPadding.left + rcControlPadding.right);
    }
    if (imageSize.cy > 0) {
        imageSize.cy += (rcControlPadding.top + rcControlPadding.bottom);
    }
    return imageSize;
}

Image* Control::GetEstimateImage()
{
    Image* estimateImage = nullptr;
    if ((m_pBkImage != nullptr) && !m_pBkImage->GetImagePath().empty()) {
        estimateImage = m_pBkImage.get();
    }
    else if(m_pImageMap != nullptr){
        estimateImage = m_pImageMap->GetEstimateImage(kStateImageBk);
        if (estimateImage == nullptr) {
            estimateImage = m_pImageMap->GetEstimateImage(kStateImageSelectedBk);
        }
    }
    return estimateImage;
}

bool Control::IsPointInWithScrollOffset(const UiPoint& point) const
{
    UiPoint scrollOffset = GetScrollOffsetInScrollBox();
    UiPoint newPoint = point;
    newPoint.Offset(scrollOffset);
    return GetRect().ContainsPt(newPoint);
}

void Control::SendEvent(EventType eventType, WPARAM wParam, LPARAM lParam)
{
    SendEvent(eventType, wParam, lParam, nullptr);
}

void Control::SendEvent(EventType eventType, WPARAM wParam, LPARAM lParam, void* pEventData)
{
    EventArgs msg;
    msg.SetSender(this);
    msg.eventType = eventType;
    msg.vkCode = VirtualKeyCode::kVK_None;
    msg.wParam = wParam;
    msg.lParam = lParam;
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        msg.ptMouse = pWindow->GetLastMousePos();
    }
    msg.modifierKey = 0;
    msg.eventData = 0;
    msg.pEventData = pEventData;

    // Dispatch the message
    SendEventMsg(msg);
}

void Control::SendEvent(EventType eventType, EventArgs msg)
{
    ASSERT((msg.eventType == eventType) || (msg.eventType == EventType::kEventNone));
    msg.eventType = eventType;
    msg.SetSender(this);
    if ((msg.ptMouse.x == 0) && (msg.ptMouse.y == 0)) {
        Window* pWindow = GetWindow();
        if (pWindow != nullptr) {
            msg.ptMouse = pWindow->GetLastMousePos();
        }
    }
    // Dispatch the message
    SendEventMsg(msg);
}

void Control::SendEventMsg(const EventArgs& msg)
{
//#ifdef _DEBUG
//    DString eventType = EventTypeToString(msg.eventType);
//    DString type = GetType();
//    DStringW::value_type buf[256] = {};
//    swprintf_s(buf, _T("Control::SendEventMsg: type=%s, eventType=%s\r\n"), type.c_str(), eventType.c_str());
//    ::OutputDebugStringW(buf);    
//#endif

    bool bRet = true;
    // The Enter and Leave messages of the mouse are handled by a special flow, and events are fired inside the handler itself
    if (!IsDisabledEvents(msg) && (msg.eventType != kEventMouseEnter) && (msg.eventType != kEventMouseLeave)) {
        bRet = FireAllEvents(msg);
    }
    if(bRet) {
        HandleEvent(msg);
    }
}

bool Control::IsDisabledEvents(const EventArgs& msg) const
{
    if ((msg.eventType > kEventMouseBegin) && (msg.eventType < kEventMouseEnd)) {
        // When the current control is forbidden to receive mouse messages, forward the mouse-related messages to the upper layer
        if (!IsEnabled() || !IsMouseEnabled()) {
            return true;
        }
    }
    else if ((msg.eventType > kEventKeyBegin) && (msg.eventType < kEventKeyEnd)) {
        // When the current control is forbidden to receive keyboard messages, forward the keyboard-related messages to the upper layer
        if (!IsEnabled() || !IsKeyboardEnabled()) {
            return true;
        }
    }
    return false;
}

void Control::HandleEvent(const EventArgs& msg)
{
    std::weak_ptr<WeakFlag> weakFlag = GetWeakFlag();
    if (IsDisabledEvents(msg)) {
        // If it is a mouse or keyboard message and the control is Disabled, forward it to the upper layer control
        Box* pParent = GetParent();
        if (pParent != nullptr) {
            pParent->SendEventMsg(msg);
        }
        return;
    }
    if( msg.eventType == kEventSetCursor ) {
        if (OnSetCursor(msg)) {
            return;
        }        
    }
    else if (msg.eventType == kEventSetFocus) {
        if (OnSetFocus(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventKillFocus) {
        if (OnKillFocus(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventWindowSetFocus) {
        if (OnWindowSetFocus(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventWindowKillFocus) {
        if (OnWindowKillFocus(msg)) {
            return;
        }
    }    
    else if (msg.eventType == kEventCaptureChanged) {
        if (OnCaptureChanged(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventImeSetContext) {
        if (OnImeSetContext(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventImeStartComposition) {
        if (OnImeStartComposition(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventImeComposition) {
        if (OnImeComposition(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventImeEndComposition) {
        if (OnImeEndComposition(msg)) {
            return;
        }
    }
    else if( msg.eventType == kEventMouseEnter ) {
        if (GetWindow() != nullptr) {
            // If the current Hover control is not a related control (the current control itself or its descendant controls), ignore this message
            if (!IsControlRelated(this, GetWindow()->GetHoverControl())) {
                return;
            }
        }
        if (MouseEnter(msg)) {
            return;
        }
    }
    else if( msg.eventType == kEventMouseLeave ) {
        if (GetWindow() != nullptr) {
            // If the current Hover control is a related control (the current control itself or its descendant controls), ignore this message
            if (IsControlRelated(this, GetWindow()->GetHoverControl())) {
                return;
            }
        }
        if (MouseLeave(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventMouseButtonDown) {
        if (ButtonDown(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventMouseButtonUp) {
        if (ButtonUp(msg)) {
            return;
        }        
    }
    else if (msg.eventType == kEventMouseDoubleClick) {
        if (ButtonDoubleClick(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventMouseRButtonDown) {
        if (RButtonDown(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventMouseRButtonUp) {
        if (RButtonUp(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventMouseRDoubleClick) {
        if (RButtonDoubleClick(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventMouseMButtonDown) {
        if (MButtonDown(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventMouseMButtonUp) {
        if (MButtonUp(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventMouseMDoubleClick) {        
        if (MButtonDoubleClick(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventMouseMove) {        
        if (MouseMove(msg)) {
            return;
        }        
    }
    else if (msg.eventType == kEventMouseHover) {        
        if (MouseHover(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventMouseWheel) {        
        if (MouseWheel(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventContextMenu) {        
        if (MouseMenu(msg)) {
            return;
        }        
    }
    else if (msg.eventType == kEventChar) {        
        if (OnChar(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventKeyDown) {        
        if (OnKeyDown(msg)) {
            return;
        }
    }
    else if (msg.eventType == kEventKeyUp) {        
        if (OnKeyUp(msg)) {
            return;
        }
    }
    if (!weakFlag.expired() && (GetParent() != nullptr)) {
        // Here in the parent control, the BubbledEvent dispatch is triggered, and the parent control handles this message
        GetParent()->SendEventMsg(msg);
    }
}

bool Control::CheckEventType(const EventArgs& msg, EventType eventType) const
{
    ASSERT(msg.eventType == eventType);
    if (msg.eventType != eventType) {
        return false;
    }
    if (msg.IsSenderExpired()) {
        return false;
    }
    return true;
}

bool Control::HasHotState()
{
    bool bState = false;
    if (m_pColorMap != nullptr) {
        bState = m_pColorMap->HasHotColor();
    }
    if (!bState && (m_pImageMap != nullptr)) {
        bState = m_pImageMap->HasHotImage();
    }
    if (!bState && (m_pBorderData != nullptr) && (m_pBorderData->m_pBorderColorMap != nullptr)) {
        bState = m_pBorderData->m_pBorderColorMap->HasHotColor();
    }
    return bState;
}

bool Control::MouseEnter(const EventArgs& msg)
{
    // MouseEnter flow: ancestor controls[MouseEnter] -> parent control[MouseEnter] -> child control[MouseEnter]
    if (!CheckEventType(msg, kEventMouseEnter)) {
        return true;
    }
    if(IsEnabled()) {
        if (GetState() == kControlStateNormal) {            
            if (HasHotState()) {
                // Hot state animation
                AnimationPlayer* pHotAnimationPlayer = GetHotAnimationPlayer();
                if (pHotAnimationPlayer != nullptr) {
                    pHotAnimationPlayer->Continue();
                }
            }
            PrivateSetState(kControlStateHot);
        }
        if (!m_bMouseEnter) {
            m_bMouseEnter = true;

            // Fire the event, so the application layer can receive the callback event of this message
            EventArgs newMsg = msg;
            newMsg.SetSender(this);
            FireNormalEvents(newMsg);
        }
    }
    else {
        // Restore the state
        m_bMouseEnter = false;
        if (GetState() == kControlStateHot) {
            PrivateSetState(kControlStateNormal);
        }
    }
    return false; // When false is returned, the parent control will also receive the MouseEnter event
}

bool Control::MouseLeave(const EventArgs& msg)
{
    // MouseLeave flow: child control[MouseLeave] -> parent control[MouseLeave] -> ancestor controls[MouseLeave]
    if (!CheckEventType(msg, kEventMouseLeave)) {
        return true;
    }
    if(IsEnabled()) {
        if (GetState() == kControlStateHot) {
            PrivateSetState(kControlStateNormal);
            if (HasHotState()) {
                // Hot state animation
                AnimationPlayer* pHotAnimationPlayer = GetHotAnimationPlayer();
                if (pHotAnimationPlayer != nullptr) {
                    pHotAnimationPlayer->ReverseContinue();
                }
            }
            Invalidate();
        }
        if (m_bMouseEnter) {
            m_bMouseEnter = false;

            // Fire the event, so the application layer can receive the callback event of this message
            EventArgs newMsg = msg;
            newMsg.SetSender(this);
            FireNormalEvents(newMsg);
        }
    }
    else {
        // Restore the state
        m_bMouseEnter = false;
        if (GetState() == kControlStateHot) {
            PrivateSetState(kControlStateNormal);
            Invalidate();
        }
    }
    return false; // When false is returned, the parent control will also receive the MouseLeave event
}

bool Control::ButtonDown(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseButtonDown)) {
        return true;
    }
    if( IsEnabled() ) {
        PrivateSetState(kControlStatePushed);
        SetMouseFocused(true);
        Invalidate();
    }
    return true;
}

bool Control::ButtonUp(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseButtonUp)) {
        return true;
    }
    if( IsMouseFocused() ) {
        SetMouseFocused(false);
        // Stop the Hot state animation
        AnimationPlayer* pHotAnimationPlayer = GetHotAnimationPlayer();
        if (pHotAnimationPlayer != nullptr) {
            pHotAnimationPlayer->Stop();
        }
        Invalidate();
        if( IsPointInWithScrollOffset(msg.ptMouse) ) {
            PrivateSetState(kControlStateHot);
            m_nHotAlpha = 255;
            Activate(&msg);
        }
        else {
            PrivateSetState(kControlStateNormal);
            m_nHotAlpha = 0;
        }
    }
    return true;
}

bool Control::ButtonDoubleClick(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseDoubleClick)) {
        return true;
    }
    return true;
}

bool Control::RButtonDown(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseRButtonDown)) {
        return true;
    }
    if (IsEnabled()) {
        SetMouseFocused(true);
    }
    return true;
}

bool Control::RButtonUp(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseRButtonUp)) {
        return true;
    }
    if (IsMouseFocused()) {
        SetMouseFocused(false);
        if (IsPointInWithScrollOffset(msg.ptMouse)) {
            EventArgs msgData;
            msgData.ptMouse = msg.ptMouse;
            msgData.modifierKey = msg.modifierKey;
            SendEvent(kEventRClick, msgData);
        }
    }
    return true;
}

bool Control::RButtonDoubleClick(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseRDoubleClick)) {
        return true;
    }
    return true;
}

bool Control::MButtonDown(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseMButtonDown)) {
        return true;
    }
    if (IsEnabled()) {
        SetMouseFocused(true);
    }
    return true;
}

bool Control::MButtonUp(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseMButtonUp)) {
        return true;
    }
    if (IsMouseFocused()) {
        SetMouseFocused(false);
    }
    return true;
}

bool Control::MButtonDoubleClick(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseMDoubleClick)) {
        return true;
    }
    return true;
}

bool Control::MouseMove(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseMove)) {
        return true;
    }
    return true;
}

bool Control::MouseHover(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseHover)) {
        return true;
    }
    return true;
}

bool Control::MouseWheel(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventMouseWheel)) {
        return true;
    }

    // Not handled by default; left to the parent control
    //int32_t wheelDelta = msg.eventData;
    return false;
}

bool Control::MouseMenu(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventContextMenu)) {
        return true;
    }

    // Shift + F10 generates the context menu by the system, or clicking the right button fires the menu: not handled by default, left to the parent control
    return false;
}

bool Control::OnChar(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventChar)) {
        return true;
    }

    // Not handled by default; left to the parent control
    return false;
}

bool Control::OnKeyDown(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventKeyDown)) {
        return true;
    }

    // Not handled by default; left to the parent control
    return false;
}

bool Control::OnKeyUp(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventKeyUp)) {
        return true;
    }

    // Not handled by default; left to the parent control
    return false;
}

bool Control::OnSetCursor(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventSetCursor)) {
        return true;
    }
    switch (m_cursorType) {
    case CursorType::kCursorHand:
        {
            if (IsEnabled()) {
                SetCursor(CursorType::kCursorHand);
            }
            else {
                SetCursor(CursorType::kCursorArrow);
            }
        }
        break;
    default:
        SetCursor(m_cursorType);
        break;
    }
    return true;
}

void Control::SetCursor(CursorType cursorType)
{
    GlobalManager::Instance().Cursor().SetCursor(cursorType);
}

bool Control::OnSetFocus(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventSetFocus)) {
        return true;
    }
#if defined (DUILIB_BUILD_FOR_WIN)
    // By default, when the control gets focus, the input method is closed
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        pWindow->NativeWnd()->SetImeOpenStatus(false);
    }
#endif

    if (GetState() == kControlStateNormal) {
        SetState(kControlStateHot);
        Invalidate();
    }
    return true;
}

bool Control::OnKillFocus(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventKillFocus)) {
        return true;
    }
    if (GetState() == kControlStateHot) {
        SetState(kControlStateNormal);
    }
    else if (GetState() == kControlStatePushed) {
        // When losing focus, repair the control state (if the window loses focus while the mouse button is pressed down, this control will not receive the mouse up event)
        SetMouseFocused(false);
        // Stop the Hot state animation
        AnimationPlayer* pHotAnimationPlayer = GetHotAnimationPlayer();
        if (pHotAnimationPlayer != nullptr) {
            pHotAnimationPlayer->Stop();
        }
        SetState(kControlStateNormal);
    }
    Invalidate();
    return true;
}

bool Control::OnWindowSetFocus(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventWindowSetFocus)) {
        return true;
    }
    // Not handled by default; left to the parent control
    return false;
}

bool Control::OnWindowKillFocus(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventWindowKillFocus)) {
        return true;
    }
    // Not handled by default; left to the parent control
    return false;
}

bool Control::OnCaptureChanged(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventCaptureChanged)) {
        return true;
    }
    // Not handled by default; left to the parent control
    return false;
}

bool Control::OnImeSetContext(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventImeSetContext)) {
        return true;
    }
    // Not handled by default; left to the parent control
    return false;
}

bool Control::OnImeStartComposition(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventImeStartComposition)) {
        return true;
    }
    // Not handled by default; left to the parent control
    return false;
}

bool Control::OnImeComposition(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventImeComposition)) {
        return true;
    }
    // Not handled by default; left to the parent control
    return false;
}

bool Control::OnImeEndComposition(const EventArgs& msg)
{
    if (!CheckEventType(msg, kEventImeEndComposition)) {
        return true;
    }
    // Not handled by default; left to the parent control
    return false;
}

bool Control::PaintImage(IRender* pRender,
                         Image* pImage,
                         const DString& strModify, int32_t nFade, 
                         IMatrix* pMatrix,
                         const UiRect* pDestRect,
                         UiRect* pPaintedRect) const
{
    PerformanceStat statPerformance(_T("Control::PaintImage"));
    // Note: the strModify parameter, currently what is passed in externally is mainly: "destscale='false' dest='%d,%d,%d,%d'"
    //                   There is also a class that passes in: _T(" corner='%d,%d,%d,%d'").
    if (pImage == nullptr) {
        // This may be empty; no assertion is needed, just return directly if empty
        return false;
    }
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return false;
    }

    Image& duiImage = *pImage;
    if (duiImage.HasImageError()) {
        // The image has a decode error; do not paint it
        if (!duiImage.IsDecodeEventFired()) {
            // When reusing the original image, this event needs to be fired additionally
            FireImageEvent(pImage, pImage->GetImagePath(), false, false, true);
        }        
        return false;
    }

    if (duiImage.GetImagePath().empty()) {
        return false;
    }

    if (!duiImage.GetImageAttribute().m_bPaintEnabled) {
        // This image is forbidden to be painted; return
        return false;
    }

    LoadImageInfo(duiImage, true);
    std::shared_ptr<ImageInfo> imageInfo = duiImage.GetImageInfo();
    if (duiImage.GetImageAttribute().IsAssertEnabled()) {
        ASSERT(imageInfo != nullptr);
    }
    if (imageInfo == nullptr) {
        // The image failed to load
        duiImage.SetImageError(true);
        return false;
    }
    ASSERT((imageInfo->GetWidth() > 0) && (imageInfo->GetHeight() > 0));
    if ((imageInfo->GetWidth() <= 0) || (imageInfo->GetHeight() <= 0)) {
        duiImage.SetImageError(true);
        return false;
    }

//#ifdef _DEBUG
//    if (this->GetBkImagePtr() == &duiImage) {
//        DString log = StringUtil::Printf(_T("BkImage: Width=%d, Height=%d, LoadScale=%d, fScale=%.02f"),
//            imageInfo->GetWidth(), imageInfo->GetHeight(),
//            imageInfo->GetLoadDpiScale(), imageInfo->GetImageSizeScale());
//        const_cast<Control*>(this)->SetToolTipText(log);
//    }
//#endif

    ImageAttribute newImageAttribute = duiImage.GetImageAttribute();
    if (!strModify.empty()) {
        newImageAttribute.ModifyAttribute(strModify, Dpi());
    }
    UiRect rcDest = GetRect();
    rcDest.Deflate(GetControlPadding());// Remove the inner padding
    if (pDestRect != nullptr) {
        // Paint the image in the rectangle area passed in externally
        rcDest = *pDestRect;
    }
    UiRect rcImageDestRect = newImageAttribute.GetImageDestRect(imageInfo->GetWidth(), imageInfo->GetHeight(), Dpi());
    if (ImageAttribute::HasValidImageRect(rcImageDestRect)) {
        // Use the target area specified in the configuration (DPI-adaptive already applied per the configuration)
        rcDest = rcImageDestRect;
        rcDest.Offset(GetRect().left, GetRect().top);
    }

    UiRect rcDestCorners;
    UiRect rcSource = newImageAttribute.GetImageSourceRect();
    UiRect rcSourceCorners = newImageAttribute.GetImageCorner();
    imageInfo->ScaleImageSourceRect(Dpi(), rcDestCorners, rcSource, rcSourceCorners);
    
    // Apply the three image attributes: rcMargin, hAlign and vAlign
    rcDest.Deflate(newImageAttribute.GetImageMargin(Dpi()));
    rcDest.Validate();
    rcSource.Validate();
    const int32_t nImageWidth = rcSource.Width();
    const int32_t nImageHeight = rcSource.Height();

    bool bAdaptiveDestRect = newImageAttribute.m_bAdaptiveDestRect; // Automatically adapt to the target area (after proportional scaling, paint according to the specified alignment)
    if (!bAdaptiveDestRect && (!newImageAttribute.m_hAlign.empty() || !newImageAttribute.m_vAlign.empty())) {
        if (!newImageAttribute.m_hAlign.empty() && (nImageWidth > rcDest.Width())) {
            bAdaptiveDestRect = true;
        }
        else if (!newImageAttribute.m_vAlign.empty() && (nImageHeight > rcDest.Height())) {
            bAdaptiveDestRect = true;
        }
    }
    if (bAdaptiveDestRect) {
        // Automatically adapt to the target area (scale the image proportionally): adjust the paint area according to the image size
        rcDest = ImageAttribute::CalculateAdaptiveRect(nImageWidth, nImageHeight,
                                                       rcDest,
                                                       newImageAttribute.m_hAlign.c_str(),
                                                       newImageAttribute.m_vAlign.c_str());
    }
    else {
        // After applying the alignment, the image will no longer be stretched, but displayed at its original size
        if (!newImageAttribute.m_hAlign.empty()) {
            if (newImageAttribute.m_hAlign == _T("left")) {
                rcDest.right = rcDest.left + nImageWidth;
            }
            else if (newImageAttribute.m_hAlign == _T("center")) {
                rcDest.left = rcDest.CenterX() - nImageWidth / 2;
                rcDest.right = rcDest.left + nImageWidth;
            }
            else if (newImageAttribute.m_hAlign == _T("right")) {
                rcDest.left = rcDest.right - nImageWidth;
            }
            else {
                rcDest.right = rcDest.left + nImageWidth;
            }

            if (newImageAttribute.m_vAlign.empty()) {
                rcDest.bottom = rcDest.top + nImageHeight;
            }
        }
        if (!newImageAttribute.m_vAlign.empty()) {
            if (newImageAttribute.m_vAlign == _T("top")) {
                rcDest.bottom = rcDest.top + nImageHeight;
            }
            else if (newImageAttribute.m_vAlign == _T("center")) {
                rcDest.top = rcDest.CenterY() - nImageHeight / 2;
                rcDest.bottom = rcDest.top + nImageHeight;
            }
            else if (newImageAttribute.m_vAlign == _T("bottom")) {
                rcDest.top = rcDest.bottom - nImageHeight;
            }
            else {
                rcDest.bottom = rcDest.top + nImageHeight;
            }

            if (newImageAttribute.m_hAlign.empty()) {
                rcDest.right = rcDest.left + nImageWidth;
            }
        }
    }

    // Backup of the calculated rcDest; in the multi-frame case, rcDest will be modified
    const UiRect rcImageDect = rcDest;
    if (pPaintedRect) {
        // Return the painted target area
        *pPaintedRect = rcImageDect;
    }
    // Set the area of the animated image
    duiImage.SetDrawDestRect(rcImageDect);

    // Get the bitmap image to paint
    std::shared_ptr<IBitmap> pBitmap;

    // Whether the image data is being decoded lazily (multi-threaded image data decoding)
    bool bDataPending = false;

    // Whether an image decode error has occurred
    bool bDecodeError = false;

    if (duiImage.IsMultiFrameImage()) {
        // Multi-frame image
        AnimationFramePtr pAnimationFrame = duiImage.GetCurrentFrame(rcImageDect, rcSource, rcSourceCorners);
        ASSERT(pAnimationFrame != nullptr);
        if (pAnimationFrame == nullptr) {
            return false;
        }
        if (pAnimationFrame->m_pBitmap != nullptr) {
            pBitmap = pAnimationFrame->m_pBitmap;

            // Apply some parameters (rcDest needs to be scaled down proportionally)
            const int32_t nDestWidth = rcDest.Width();
            const int32_t nDestHeight = rcDest.Height();
            if (pAnimationFrame->m_nOffsetX != 0) {
                float fImageScaleX = static_cast<float>(pAnimationFrame->m_pBitmap->GetWidth()) / imageInfo->GetWidth();
                float fRectScaleX = static_cast<float>(nDestWidth) / imageInfo->GetWidth();
                rcDest.left += ImageUtil::GetScaledImageOffset(pAnimationFrame->m_nOffsetX, fRectScaleX);
                rcDest.right = rcDest.left + (int32_t)ImageUtil::GetScaledImageSize((uint32_t)nDestWidth, fImageScaleX);
            }
            if (pAnimationFrame->m_nOffsetY != 0) {
                float fImageScaleY = static_cast<float>(pAnimationFrame->m_pBitmap->GetHeight()) / imageInfo->GetHeight();
                float fRectScaleY = static_cast<float>(nDestHeight) / imageInfo->GetHeight();
                rcDest.top += ImageUtil::GetScaledImageOffset(pAnimationFrame->m_nOffsetY, fRectScaleY);
                rcDest.bottom = rcDest.top + (int32_t)ImageUtil::GetScaledImageSize((uint32_t)nDestHeight, fImageScaleY);
            }
        }
        else if (pAnimationFrame->m_bDataPending) {
            // The data is not ready yet; it can be ignored
            ASSERT(pAnimationFrame->m_pBitmap == nullptr);
            if (duiImage.GetImageAttribute().m_bAsyncLoad) {
                bDataPending = true;
            }
            else {
                ASSERT(!"pAnimationFrame->m_bDataPending is invalid!");
            }
        }
        else if (pAnimationFrame->m_bDataError) {
            // An image decode error was encountered
            bDecodeError = true;
        }
        else {
            // Other unknown cases; there is an error in the flow
            ASSERT(!"pAnimationFrame->m_pBitmap is invalid!");
        }
    }
    else {
        // Single-frame image
        bool bImageStretch = true;// Whether the image will be stretched when painted
        if (newImageAttribute.IsTiledDraw()) {
            // When tiled drawing is set, the image does not need to be stretched
            bImageStretch = false;
        }
        else if (newImageAttribute.m_bWindowShadowMode) {
            // Shadow mode: no stretching, to avoid deformation of the four corners
            bImageStretch = false;
        }        
        pBitmap = duiImage.GetCurrentBitmap(bImageStretch, rcImageDect, rcSource, rcSourceCorners, &bDecodeError);
        if (pBitmap == nullptr) {
            if (!bDecodeError && duiImage.GetImageAttribute().m_bAsyncLoad) {
                bDataPending = true;
            }
        }
    }

    bool bPainted = false;
    if (pBitmap != nullptr) {
        bPainted = true;
        // Validate rcSource (in the multi-frame case, the actual image may not match the total width and height, so further validation is needed)
        if ((rcSource.left < 0) || (rcSource.left >= (int32_t)pBitmap->GetWidth())) {
            rcSource.left = 0;
        }
        if ((rcSource.top < 0) || (rcSource.top >= (int32_t)pBitmap->GetHeight())) {
            rcSource.top = 0;
        }
        if ((rcSource.right < 0) || (rcSource.right > (int32_t)pBitmap->GetWidth())) {
            rcSource.right = (int32_t)pBitmap->GetWidth();
        }
        if ((rcSource.bottom < 0) || (rcSource.bottom > (int32_t)pBitmap->GetHeight())) {
            rcSource.bottom = (int32_t)pBitmap->GetHeight();
        }

        // Image alpha property
        uint8_t iFade = (nFade == DUI_NOSET_VALUE) ? newImageAttribute.m_bFade : static_cast<uint8_t>(nFade);
        if (pMatrix != nullptr) {
            // Matrix painting: add assertions for unsupported properties to avoid errors
            ASSERT(newImageAttribute.GetImageCorner().IsEmpty());
            ASSERT(!newImageAttribute.IsTiledDraw());
            pRender->DrawImageRect(m_rcPaint, pBitmap.get(), rcDest, rcSource, iFade, pMatrix);
        }
        else {
            TiledDrawParam tiledDrawParam;
            if (newImageAttribute.m_pTiledDrawParam != nullptr) {
                tiledDrawParam = newImageAttribute.GetTiledDrawParam(Dpi());
            }
            pRender->DrawImage(m_rcPaint, pBitmap.get(), rcDest, rcDestCorners, rcSource, rcSourceCorners,
                               iFade,
                               newImageAttribute.IsTiledDraw() ? &tiledDrawParam : nullptr,
                               newImageAttribute.m_bWindowShadowMode);
        }

        // After painting successfully, remove from the delayed paint list
        GlobalManager::Instance().Image().RemoveDelayPaintData(pImage);
    }
    else if (bDataPending) {
        // Currently the image is loaded asynchronously; add it to the delayed paint list
        Control* pControl = const_cast<Control*>(this);
        DString imageKey = imageInfo->GetImageKey();
        GlobalManager::Instance().Image().AddDelayPaintData(pControl, pImage, imageKey);
    }
    else if (bDecodeError) {
        // An image decode error was encountered
        duiImage.SetImageError(true);
    }
    // Start the animation as needed
    if (!duiImage.HasImageError()) {
        if (duiImage.IsMultiFrameImage()) {
            duiImage.CheckStartImageAnimation();
        }
    }    
    return bPainted;
}

std::unique_ptr<AutoClip> Control::CreateRectClip(IRender* pRender, const UiRect& rc, bool bClip) const
{
    if (!bClip) {
        return nullptr;
    }
    return std::make_unique<AutoClip>(pRender, rc, bClip);
}

std::unique_ptr<AutoClip> Control::CreateRoundClip(IRender* pRender, const UiRect& rc, bool bRoundClip) const
{
    float fRoundWidth = 0;
    float fRoundHeight = 0;
    if (!bRoundClip || !GetBorderRound(fRoundWidth, fRoundHeight)) {
        return nullptr;
    }
    return std::make_unique<AutoClip>(pRender, rc, fRoundWidth, fRoundHeight, bRoundClip);
}

void Control::SetPaintRect(const UiRect& rect)
{
    m_rcPaint = rect;
}

std::unique_ptr<IRender> Control::CreateTempRender() const
{
    std::unique_ptr<IRender> spTempRender;
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    ASSERT(pRenderFactory != nullptr);
    if (pRenderFactory != nullptr) {
        ASSERT(GetWindow() != nullptr);
        IRenderDpiPtr spRenderDpi;
        if (GetWindow() != nullptr) {
            spRenderDpi = GetWindow()->GetRenderDpi();
        }
        spTempRender.reset(pRenderFactory->CreateRender(spRenderDpi));
    }
    return spTempRender;
}

void Control::AlphaPaint(IRender* pRender, const UiRect& rcPaint)
{
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }
    if (GetRect().IsEmpty()) {
        return;
    }    
    if (m_nAlpha == 0) {
        // The control is completely transparent; do not paint it
        return;
    }

    UiRect rcTemp; // The dirty area within this control, the area that needs to be painted this time
    if (!UiRect::Intersect(rcTemp, rcPaint, GetBoxShadowExpandedRect(GetRect()))) {// If the area including the box-shadow is a dirty area, painting needs to be performed
        return;
    }
    UiRect::Intersect(m_rcPaint, rcPaint, GetRect()); // Set the value of m_rcPaint

    // Whether the rectangular area is set as the clip area
    const bool bRectClip = IsClip();

    // Whether the rounded rectangle area is set as the clip area
    const bool bRoundClip = IsClip() && ShouldBeRoundRectFill();

    // Whether the current control has an alpha value set (the alpha value is not 255)
    const bool bAlpha = IsAlpha();

    // Whether the current control has a box-shadow set (control shadow effect)
    const bool bPaintBoxShadow = HasBoxShadow();

    // The paint position offset of the control (used for the animation effects of the control)
    const UiPoint renderOffset = GetRenderOffset();

    if (bAlpha) {
        // When an alpha value is set, the control (including child controls if it is a container) needs to be painted completely
        UiRect rcPaintRect = GetRect();
        SetPaintRect(rcPaintRect);
        if (m_pTempRender == nullptr) {
            m_pTempRender = CreateTempRender();
        }
        IRender* pTempRender = m_pTempRender.get();
        ASSERT(pTempRender != nullptr);
        if (pTempRender == nullptr) {
            return;
        }
        if ((pTempRender->GetWidth() != GetRect().Width()) || (pTempRender->GetHeight() != GetRect().Height())) {
            if (!pTempRender->Resize(GetRect().Width(), GetRect().Height())) {
                // An error occurred; painting failed
                ASSERT(!"pTempRender->Resize failed!");
                return;
            }
        }
        
        if ((pTempRender->GetWidth() > 0) && (pTempRender->GetHeight() > 0)) {
            // Paint the control (including child controls if it is a container) completely into the cached new render
            // Before painting, first clear the original content
            pTempRender->Clear(UiColor());

            const UiPoint ptOffset(GetRect().left, GetRect().top);
            const UiPoint ptOldOrg = pTempRender->OffsetWindowOrg(ptOffset);

            std::unique_ptr<AutoClip> rectCacheClip = CreateRectClip(pTempRender, GetRect(), bRectClip);
            std::unique_ptr<AutoClip> roundCacheClip = CreateRoundClip(pTempRender, GetRect(), bRoundClip);

            // First paint itself
            Paint(pTempRender, rcPaintRect);

            // When an alpha value is set, paint the child controls onto pTempRender, then AlphaBlend the whole to pRender
            PaintChild(pTempRender, rcPaintRect);
            if (IsBordersOnTop()) {
                PaintBorder(pTempRender);  // Paint the border
            }
            PaintLoading(pTempRender, rcPaintRect); // Paint the Loading image, stateless, needs to be painted after the child controls
            PaintForeColor(pTempRender); // Paint the foreground color

            pTempRender->SetWindowOrg(ptOldOrg);
        }

        // If a box-shadow is configured, paint it first, because the box-shadow will be painted beyond the rect boundary (if a clip area is used, it will not be fully shown)
        if (bPaintBoxShadow) {
            m_bBoxShadowPainted = false;
            PaintShadow(pRender);
            m_bBoxShadowPainted = true;
        }
        UiPoint ptOldOrg = pRender->OffsetWindowOrg(renderOffset);// The position offset of the control, shown as the animation effect
        std::unique_ptr<AutoClip> rectClip = CreateRectClip(pRender, GetRect(), bRectClip);
        std::unique_ptr<AutoClip> roundClip = CreateRoundClip(pRender, GetRect(), bRoundClip);

        int32_t xOffset = std::max(rcPaintRect.left - GetRect().left, 0);
        int32_t yOffset = std::max(rcPaintRect.top - GetRect().top, 0);
        pRender->AlphaBlend(rcPaintRect.left,
                            rcPaintRect.top,
                            rcPaintRect.Width() - xOffset,
                            rcPaintRect.Height() - yOffset,
                            pTempRender,
                            xOffset,
                            yOffset,
                            rcPaintRect.Width() - xOffset,
                            rcPaintRect.Height() - yOffset,
                            static_cast<uint8_t>(m_nAlpha));
        if (bPaintBoxShadow) {
            // After Paint, immediately reset the flag to avoid affecting other paint logic
            m_bBoxShadowPainted = false;
        }
        pRender->SetWindowOrg(ptOldOrg);// Restore the view origin
        UiRect::Intersect(m_rcPaint, rcPaint, GetRect()); // Set the value of m_rcPaint
    }
    else {
        // Clear the temporary Render (when the Hot animation is used, Alpha may be used; after it finishes, it should be cleared and the memory released)
        m_pTempRender.reset();

        // The current control has no alpha set; no cached painting is used, and the control is painted directly on the target render (including child controls if it is a container)
        UiPoint ptOldOrg = pRender->OffsetWindowOrg(renderOffset);// The position offset of the control, shown as the animation effect

        // If a box-shadow is configured, paint it first, because the box-shadow will be painted beyond the rect boundary (if a clip area is used, it will not be fully shown)
        if (bPaintBoxShadow) {
            m_bBoxShadowPainted = false;
            PaintShadow(pRender);
            m_bBoxShadowPainted = true;
        }

        std::unique_ptr<AutoClip> rectClip = CreateRectClip(pRender, GetRect(), bRectClip);
        std::unique_ptr<AutoClip> roundClip = CreateRoundClip(pRender, GetRect(), bRoundClip);
        Paint(pRender, rcPaint);        // Paint the control itself
        if (bPaintBoxShadow) {
            // After Paint, immediately reset the flag to avoid affecting other paint logic
            m_bBoxShadowPainted = false;
        }
        PaintChild(pRender, rcPaint);   // Paint the child controls
        if (IsBordersOnTop()) {
            PaintBorder(pRender);       // Paint the border
        }
        PaintLoading(pRender, rcPaint); // Paint the Loading state, stateless, needs to be painted after the child controls
        PaintForeColor(pRender);        // Paint the foreground color

        pRender->SetWindowOrg(ptOldOrg);// Restore the view origin
    }
}

void Control::Paint(IRender* pRender, const UiRect& rcPaint)
{
    if (GetRect().IsEmpty()) {
        return;
    }
    UiRect rcTemp; // The dirty area within this control, the area that needs to be painted this time
    if (!UiRect::Intersect(rcTemp, rcPaint, GetBoxShadowExpandedRect(GetRect()))) {// If the area including the box-shadow is a dirty area, painting needs to be performed
        return;
    }
    UiRect::Intersect(m_rcPaint, rcPaint, GetRect()); // Set the value of m_rcPaint

    if (!m_bBoxShadowPainted) {
        // Paint the box-shadow; it may be painted beyond the rect boundary (if clipping is used, it may not be fully shown)
        PaintShadow(pRender);
    }    

    // Paint other content
    PaintBkColor(pRender);        // Background color (covers the whole rectangle)
    PaintStateColors(pRender);    // Colors of the specified control state: normal state, focus state, pressed state, disabled state (cover the whole rectangle)
    PaintBkImage(pRender);        // Background image, stateless
    PaintStateImages(pRender);    // Paint the background image first, then the foreground image; each image has a specified state: normal state, focus state, pressed state, disabled state
    PaintText(pRender);           // Paint the text
    if (!IsBordersOnTop()) {
        PaintBorder(pRender);     // Paint the border
    }
    PaintFocusRect(pRender);      // Paint the focus state
}

void Control::PaintShadow(IRender* pRender)
{
    if (!HasBoxShadow()) {
        return;
    }
    BoxShadow boxShadow(this);
    if ((m_pOtherData != nullptr) && (m_pOtherData->m_pBoxShadow != nullptr)) {
        boxShadow = *m_pOtherData->m_pBoxShadow;
    }

    ASSERT(pRender != nullptr);
    if (pRender != nullptr) {
        UiSize borderRound;
        float fRoundWidth = 0;
        float fRoundHeight = 0;
        if (GetBorderRound(fRoundWidth, fRoundHeight)) {
            borderRound.cx = (int32_t)(fRoundWidth + 0.5f);
            borderRound.cy = (int32_t)(fRoundHeight + 0.5f);
        }
        pRender->DrawBoxShadow(GetRect(),
                               borderRound,
                               boxShadow.m_cpOffset,
                               boxShadow.m_nBlurRadius,
                               boxShadow.m_nSpreadRadius,
                               GlobalManager::Instance().Color().GetColor(boxShadow.m_strColor),
                               m_nAlpha);// The alpha of the control shadow follows the alpha of the control
    }    
}

void Control::PaintBkColor(IRender* pRender)
{
    if ((m_pColorData == nullptr) || m_pColorData->m_strBkColor.empty()) {
        return;
    }
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }

    UiColor dwBackColor = GetUiColor(m_pColorData->m_strBkColor.c_str());
    if(dwBackColor.GetARGB() != 0) {
        int32_t nBorderSize = 0;
        if ((m_pBorderData != nullptr) && (m_pBorderData->m_rcBorderSize.left > 0.001f) &&
            IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.right) &&
            IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.top)   &&
            IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.bottom)) {
            // All four borders exist and have the same size
            nBorderSize = static_cast<int32_t>(m_pBorderData->m_rcBorderSize.left);// no rounding
        }
        nBorderSize /= 2;

        // The rectangle range for the background fill
        UiRect fillRect = GetRect();
        if (nBorderSize > 0) { 
            // If a border exists, do not fill the border position when filling, to avoid jagged edges of the background color
            UiRect borderRect(nBorderSize, nBorderSize, nBorderSize, nBorderSize);
            fillRect.Deflate(borderRect.left, borderRect.top, borderRect.right, borderRect.bottom);
        }
        if (ShouldBeRoundRectFill()) {
            // A rounded rectangle needs to be painted; the fill also needs to fill the rounded rectangle
            float fRoundWidth = 0;
            float fRoundHeight = 0;
            GetBorderRound(fRoundWidth, fRoundHeight);
            FillRoundRect(pRender, fillRect, fRoundWidth, fRoundHeight, dwBackColor);
        }
        else {            
            UiColor dwBackColor2;
            if ((m_pColorData != nullptr) && !m_pColorData->m_strBkColor2.empty()) {
                dwBackColor2 = GetUiColor(m_pColorData->m_strBkColor2.c_str());
            }
            if (!dwBackColor2.IsEmpty()) {
                // Gradient background color
                int8_t nColor2Direction = 1;
                if (m_pColorData != nullptr) {
                    nColor2Direction = m_pColorData->m_nBkColor2Direction;
                }
                pRender->FillRect(UiRectF::MakeFromRect(fillRect), dwBackColor, dwBackColor2, nColor2Direction);
            }
            else {
                pRender->FillRect(UiRectF::MakeFromRect(fillRect), dwBackColor);
            }            
        }
    }
}

void Control::PaintForeColor(IRender* pRender)
{
    if ((m_pColorData == nullptr) || m_pColorData->m_strForeColor.empty()) {
        return;
    }
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }

    UiColor dwForeColor = GetUiColor(m_pColorData->m_strForeColor.c_str());
    if (dwForeColor.GetARGB() != 0) {
        int32_t nBorderSize = 0;
        if ((m_pBorderData != nullptr) && (m_pBorderData->m_rcBorderSize.left > 0.001f) &&
            IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.right) &&
            IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.top) &&
            IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.bottom)) {
            // All four borders exist and have the same size
            nBorderSize = static_cast<int32_t>(m_pBorderData->m_rcBorderSize.left);// no rounding
        }
        nBorderSize /= 2;

        // The rectangle range for the background fill
        UiRect fillRect = GetRect();
        if (nBorderSize > 0) {
            // If a border exists, do not fill the border position when filling, to avoid jagged edges of the background color
            UiRect borderRect(nBorderSize, nBorderSize, nBorderSize, nBorderSize);
            fillRect.Deflate(borderRect.left, borderRect.top, borderRect.right, borderRect.bottom);
        }
        if (ShouldBeRoundRectFill()) {
            // A rounded rectangle needs to be painted; the fill also needs to fill the rounded rectangle
            float fRoundWidth = 0;
            float fRoundHeight = 0;
            GetBorderRound(fRoundWidth, fRoundHeight);
            FillRoundRect(pRender, fillRect, fRoundWidth, fRoundHeight, dwForeColor);
        }
        else {
            pRender->FillRect(UiRectF::MakeFromRect(fillRect), dwForeColor);
        }
    }
}

void Control::PaintBorder(IRender* pRender)
{
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }
    UiColor dwBorderColor;
    DString borderColor;
    if (IsFocused()) {
        if (borderColor.empty()) {
            borderColor = GetFocusBorderColor();
        }
    }
    if (borderColor.empty()) {
        borderColor = GetBorderColor(GetState());
    }
    if (!borderColor.empty()) {
        dwBorderColor = GetUiColor(borderColor);
    }
    if (dwBorderColor.GetARGB() == 0) {
        return;
    }
    bool bPainted = false;
    if ((m_pBorderData != nullptr) && (m_pBorderData->m_rcBorderSize.left > 0.001f) &&
        IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.right) &&
        IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.top)   &&
        IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.bottom)) {
        // All four borders exist and have the same size, so paint the rectangle directly; rounded rectangle is supported
        if (ShouldBeRoundRectBorders()) {
            // For rounded rectangles only, use this function to paint the border lines
            PaintBorders(pRender, GetRect(), m_pBorderData->m_rcBorderSize.left, dwBorderColor, GetBorderDashStyle());
            bPainted = true;
        }
    }

    if(!bPainted) {
        // For non-rounded rectangles, paint the border lines for the four borders separately according to the settings
        const float epsilon = 0.001f;
        if ((m_pBorderData != nullptr) && (m_pBorderData->m_rcBorderSize.left > epsilon)) {
            // Left border line
            UiRect rcBorder = GetRect();
            if (std::fabs(m_pBorderData->m_rcBorderSize.left - 1.0f) < epsilon) {
                rcBorder.bottom -= 1;
            }
            const float fWidth = (float)m_pBorderData->m_rcBorderSize.left;
            UiPointF pt1((float)rcBorder.left + fWidth / 2, (float)rcBorder.top);
            UiPointF pt2((float)rcBorder.left + fWidth / 2, (float)rcBorder.bottom);
            DrawBorderLine(pRender, pt1, pt2, m_pBorderData->m_rcBorderSize.left, dwBorderColor, GetBorderDashStyle());
        }
        if ((m_pBorderData != nullptr) && (m_pBorderData->m_rcBorderSize.top > epsilon)) {
            // Top border line
            UiRect rcBorder = GetRect();
            if (std::fabs(m_pBorderData->m_rcBorderSize.top - 1.0f) < epsilon) {
                rcBorder.right -= 1;
            }
            const float fWidth = (float)m_pBorderData->m_rcBorderSize.top;
            UiPointF pt1((float)rcBorder.left, (float)rcBorder.top + fWidth / 2);
            UiPointF pt2((float)rcBorder.right, (float)rcBorder.top + fWidth / 2);
            DrawBorderLine(pRender, pt1, pt2, m_pBorderData->m_rcBorderSize.top, dwBorderColor, GetBorderDashStyle());
        }
        if ((m_pBorderData != nullptr) && (m_pBorderData->m_rcBorderSize.right > epsilon)) {
            // Right border line
            UiRect rcBorder = GetRect();
            if (std::fabs(m_pBorderData->m_rcBorderSize.right - 1.0f) < epsilon) {
                rcBorder.bottom -= 1;
            }
            const float fWidth = (float)m_pBorderData->m_rcBorderSize.right;
            UiPointF pt1((float)rcBorder.right - fWidth / 2, (float)rcBorder.top);
            UiPointF pt2((float)rcBorder.right - fWidth / 2, (float)rcBorder.bottom);
            DrawBorderLine(pRender, pt1, pt2, m_pBorderData->m_rcBorderSize.right, dwBorderColor, GetBorderDashStyle());
        }
        if ((m_pBorderData != nullptr) && (m_pBorderData->m_rcBorderSize.bottom > epsilon)) {
            // Bottom border line
            UiRect rcBorder = GetRect();
            if (std::fabs(m_pBorderData->m_rcBorderSize.bottom - 1.0f) < epsilon) {
                rcBorder.right -= 1;
            }
            const float fWidth = (float)m_pBorderData->m_rcBorderSize.bottom;
            UiPointF pt1((float)rcBorder.left, (float)rcBorder.bottom - fWidth / 2);
            UiPointF pt2((float)rcBorder.right, (float)rcBorder.bottom - fWidth / 2);
            DrawBorderLine(pRender, pt1, pt2, m_pBorderData->m_rcBorderSize.bottom, dwBorderColor, GetBorderDashStyle());
        }
    }
}

void Control::DrawBorderLine(IRender* pRender, const UiPointF& pt1, const UiPointF& pt2,
                             float fBorderSize, UiColor dwBorderColor, int8_t borderDashStyle)
{
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }
    IPen::DashStyle dashStyle = (IPen::DashStyle)borderDashStyle;
    if (dashStyle == IPen::DashStyle::kDashStyleSolid) {
        // Normal solid line
        pRender->DrawLine(pt1, pt2, dwBorderColor, fBorderSize);
    }
    else {
        // Other line styles
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        if (pRenderFactory != nullptr) {
            std::unique_ptr<IPen> pPen(pRenderFactory->CreatePen(dwBorderColor, fBorderSize));
            pPen->SetDashStyle(dashStyle);
            pRender->DrawLine(pt1, pt2, pPen.get());
        }
        else {
            pRender->DrawLine(pt1, pt2, dwBorderColor, fBorderSize);
        }
    }
}

void Control::PaintBorders(IRender* pRender, UiRect rcDraw,
                           float fBorderSize, UiColor dwBorderColor, int8_t borderDashStyle) const
{
    if ((pRender == nullptr) || rcDraw.IsEmpty() || (fBorderSize < 0.1) || (dwBorderColor.GetARGB() == 0)) {
        return;
    }

    // Paint the border lines
    UiRectF rcDrawF((float)rcDraw.left, (float)rcDraw.top, (float)rcDraw.right, (float)rcDraw.bottom);

    const bool bRoundRectBorders = ShouldBeRoundRectBorders();
    const bool bRootBoxRoundCorner = IsRootBox() && IsWindowRoundRect(); // The window is rounded, and this control is the root container
    if (bRoundRectBorders && bRootBoxRoundCorner) {
        // In a rounded window, the root container is also rounded; the corner radius of the root container must match the window's corner radius to avoid black edges at the corners
        fBorderSize *= 2;// Enlarge by 2 times; when painting with the window edge as the center line, the actually displayed line is exactly the same as the setting
    }
    else {
        // Ensure the border lines are within the rectangle
        float fHalfBorderSize = fBorderSize / 2;
        rcDrawF.left += fHalfBorderSize;
        rcDrawF.top += fHalfBorderSize;
        rcDrawF.right -= fHalfBorderSize;
        rcDrawF.bottom -= fHalfBorderSize;
    }

    if (bRoundRectBorders) {
        float fRoundWidth = 0;
        float fRoundHeight = 0;
        GetBorderRound(fRoundWidth, fRoundHeight);        
        DrawRoundRect(pRender, rcDrawF, fRoundWidth, fRoundHeight, dwBorderColor, fBorderSize, borderDashStyle);
    }
    else {
        if (borderDashStyle == IPen::DashStyle::kDashStyleSolid) {
            // Normal solid line
            pRender->DrawRect(rcDrawF, dwBorderColor, fBorderSize, false);
        }
        else {
            // Other line styles
            IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
            if (pRenderFactory != nullptr) {
                std::unique_ptr<IPen> pPen(pRenderFactory->CreatePen(dwBorderColor, fBorderSize));
                pPen->SetDashStyle((IPen::DashStyle)borderDashStyle);
                pRender->DrawRect(rcDrawF, pPen.get(), false);
            }
            else {
                pRender->DrawRect(rcDrawF, dwBorderColor, fBorderSize, false);
            }
        }
    }
}

bool Control::ShouldBeRoundRectFill() const
{
    bool isRoundRect = false;
    if ((m_pBorderData != nullptr) &&
        (IsFloatEqual(m_pBorderData->m_rcBorderSize.left, 0.0f) || (m_pBorderData->m_rcBorderSize.left > 0.001f)) &&
        IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.right)      &&
        IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.top)        &&
        IsFloatEqual(m_pBorderData->m_rcBorderSize.left, m_pBorderData->m_rcBorderSize.bottom)) {
        // All four borders have the same size (whether zero or greater than zero); rounded rectangles are supported
        if (HasBorderRound()) {
            isRoundRect = true;
        }
    }
    else {
        if (HasBorderRound()) {
            isRoundRect = true;
        }
    }
    if (isRoundRect) {
        // When the window is maximized, do not paint the rounded border lines for the Root Box control
        if (IsRootBox() && (GetWindow() != nullptr)) {
            if (GetWindow()->IsWindowMaximized()) {
                isRoundRect = false;
            }
        }
    }    
    return isRoundRect;
}

bool Control::ShouldBeRoundRectBorders() const
{
    bool isRoundRect = ShouldBeRoundRectFill();
    return isRoundRect && (m_pBorderData != nullptr) && (m_pBorderData->m_rcBorderSize.left > 0.001f);
}

void Control::PaintFocusRect(IRender* pRender)
{
    if ((pRender != nullptr) && IsShowFocusRect() && IsFocused()) {
        DoPaintFocusRect(pRender);    // Paint the focus state
    }
}

void Control::DoPaintFocusRect(IRender* pRender)
{
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }
    IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
    if (pRenderFactory == nullptr) {
        return;
    }
    float fWidth =  Dpi().GetScaleFloat(1.0f); // Brush width
    UiColor dwBorderColor;// Brush color
    DString focusRectColor = GetFocusRectColor();
    if (!focusRectColor.empty()) {
        dwBorderColor = GetUiColor(focusRectColor);
    }
    if(dwBorderColor.IsEmpty()) {
        dwBorderColor = UiColor(UiColors::Gray);
    }
    UiRect rcBorderSize(1, 1, 1, 1);
    UiRect rcFocusRect = GetRect();
    int32_t nFocusWidth = Dpi().GetScaleInt(2); // Rectangle gap
    rcFocusRect.Deflate(nFocusWidth, nFocusWidth);
    if (rcFocusRect.IsEmpty()) {
        return;
    }
    std::unique_ptr<IPen> pPen(pRenderFactory->CreatePen(dwBorderColor, fWidth));
    pPen->SetDashStyle(IPen::DashStyle::kDashStyleDot);

    if (rcBorderSize.left > 0) {
        // Left border line
        UiPointF pt1(rcFocusRect.left, rcFocusRect.top);
        UiPointF pt2(rcFocusRect.left, rcFocusRect.bottom);
        pRender->DrawLine(pt1, pt2, pPen.get());
    }
    if (rcBorderSize.top > 0) {
        // Top border line
        UiPointF pt1(rcFocusRect.left, rcFocusRect.top);
        UiPointF pt2(rcFocusRect.right, rcFocusRect.top);
        pRender->DrawLine(pt1, pt2, pPen.get());
    }
    if (rcBorderSize.right > 0) {
        // Right border line
        UiPointF pt1(rcFocusRect.right, rcFocusRect.top);
        UiPointF pt2(rcFocusRect.right, rcFocusRect.bottom);
        pRender->DrawLine(pt1, pt2, pPen.get());
    }
    if (rcBorderSize.bottom > 0) {
        // Bottom border line
        UiPointF pt1(rcFocusRect.left, rcFocusRect.bottom);
        UiPointF pt2(rcFocusRect.right, rcFocusRect.bottom);
        pRender->DrawLine(pt1, pt2, pPen.get());
    }
}

bool Control::IsRootBox() const
{
    bool isRootBox = false;
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        if ((Control*)pWindow->GetRoot() == this) {
            isRootBox = true;
        }
        else if ((Control*)pWindow->GetXmlRoot() == this) {
            isRootBox = true;
        }
    }
    return isRootBox;
}

bool Control::IsWindowRoundRect() const
{
    bool isWindowRoundRect = false;
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        UiSize roundCorner = pWindow->GetRoundCorner();
        if ((roundCorner.cx > 0) && (roundCorner.cy > 0)) {
            isWindowRoundRect = true;
        }
    }
    return isWindowRoundRect;
}

void Control::DrawRoundRect(IRender* pRender, const UiRect& rc, float rx, float ry,
                            UiColor dwBorderColor, float fBorderSize,
                            int8_t borderDashStyle) const
{
    UiRectF rcF((float)rc.left, (float)rc.top, (float)rc.right, (float)rc.bottom);
    DrawRoundRect(pRender, rcF, rx, ry, dwBorderColor, fBorderSize, borderDashStyle);
}

void Control::DrawRoundRect(IRender* pRender, const UiRectF& rc, float rx, float ry,
                            UiColor dwBorderColor, float fBorderSize,
                            int8_t borderDashStyle) const
{
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }
    if (borderDashStyle == IPen::DashStyle::kDashStyleSolid) {
        // Normal solid line
        pRender->DrawRoundRect(rc, rx, ry, dwBorderColor, fBorderSize);
    }
    else {
        // Other line styles
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        if (pRenderFactory != nullptr) {
            std::unique_ptr<IPen> pen(pRenderFactory->CreatePen(dwBorderColor, fBorderSize));
            pen->SetDashStyle((IPen::DashStyle)borderDashStyle);
            pRender->DrawRoundRect(rc, rx, ry, pen.get());
        }
        else {
            pRender->DrawRoundRect(rc, rx, ry, dwBorderColor, fBorderSize);
        }
    }
}

void Control::FillRoundRect(IRender* pRender, const UiRect& rc, float rx, float ry, UiColor dwColor) const
{
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return;
    }
    UiColor dwBackColor2;
    if ((m_pColorData != nullptr) && !m_pColorData->m_strBkColor2.empty()) {
        dwBackColor2 = GetUiColor(m_pColorData->m_strBkColor2.c_str());
    }
    if (!dwBackColor2.IsEmpty()) {
        // Gradient background color
        int8_t nColor2Direction = 1;
        if (m_pColorData != nullptr) {
            nColor2Direction = m_pColorData->m_nBkColor2Direction;
        }
        pRender->FillRoundRect(UiRectF::MakeFromRect(rc), rx, ry, dwColor, dwBackColor2, nColor2Direction);
    }
    else {
        pRender->FillRoundRect(UiRectF::MakeFromRect(rc), rx, ry, dwColor);
    }  
}

void Control::PaintBkImage(IRender* pRender)
{
    if (m_pBkImage != nullptr) {
        PaintImage(pRender, m_pBkImage.get());
    }    
}

Image* Control::GetBkImagePtr() const
{
    return m_pBkImage.get();
}

void Control::PaintStateColor(IRender* pRender, ControlStateType stateType) const
{
    if (m_pColorMap != nullptr) {
        m_pColorMap->PaintStateColor(pRender, GetRect(), stateType);
    }
}

void Control::PaintStateColors(IRender* pRender)
{
    PaintStateColor(pRender, GetState());
}

void Control::PaintStateImages(IRender* pRender)
{
    if (m_pImageMap != nullptr) {
        m_pImageMap->PaintStateImage(pRender, kStateImageBk, GetState());
        m_pImageMap->PaintStateImage(pRender, kStateImageFore, GetState());
    }    
}

void Control::PaintText(IRender* /*pRender*/)
{
    return;
}

void Control::PaintLoading(IRender* pRender, const UiRect& rcPaint)
{
    if ((m_pOtherData != nullptr) && (m_pOtherData->m_pLoading != nullptr)) {
        m_pOtherData->m_pLoading->PaintLoading(pRender, rcPaint);
    }
}

void Control::SetAlpha(uint8_t nAlpha)
{
    if (m_nAlpha != nAlpha) {
        m_nAlpha = nAlpha;
        Invalidate();
    }
}

void Control::SetHotAlpha(uint8_t nHotAlpha)
{
    if (m_nHotAlpha != nHotAlpha) {
        m_nHotAlpha = nHotAlpha;
        Invalidate();
    }
}

void Control::SetTabStop(bool enable)
{
    m_bAllowTabstop = enable;
}

UiPoint Control::GetRenderOffset() const
{
    if (m_pAnimationData != nullptr) {
        return m_pAnimationData->m_renderOffset;
    }
    return UiPoint();
}

void Control::SetRenderOffset(UiPoint renderOffset, bool bNeedDpiScale)
{
    if (bNeedDpiScale) {
        Dpi().ScalePoint(renderOffset);
    }
    if (m_pAnimationData == nullptr) {
        m_pAnimationData = std::make_unique<TAnimationData>();
    }
    if (m_pAnimationData->m_renderOffset != renderOffset) {
        m_pAnimationData->m_renderOffset = renderOffset;
        Invalidate();
    }    
}

void Control::SetRenderOffsetX(int32_t renderOffsetX)
{
    int32_t x = renderOffsetX;
    if (m_pAnimationData == nullptr) {
        m_pAnimationData = std::make_unique<TAnimationData>();
    }
    if (m_pAnimationData->m_renderOffset.x != x) {
        int32_t nOldOffsetX = m_pAnimationData->m_renderOffset.x;
        m_pAnimationData->m_renderOffset.x = x;
        Invalidate();

        // The parent control also needs to be repainted (the covered part)
        if ((nOldOffsetX != 0) && (GetParent() != nullptr)) {
            UiRect rcInvalid = GetRect();
            rcInvalid.Offset(-nOldOffsetX, 0);
            GetParent()->InvalidateRect(rcInvalid);
        }
    }
}

void Control::SetRenderOffsetY(int32_t renderOffsetY)
{
    int32_t y = renderOffsetY;
    if (m_pAnimationData == nullptr) {
        m_pAnimationData = std::make_unique<TAnimationData>();
    }
    if (m_pAnimationData->m_renderOffset.y != y) {
        int32_t nOldOffsetY = m_pAnimationData->m_renderOffset.y;
        m_pAnimationData->m_renderOffset.y = y;
        Invalidate();

        // The parent control also needs to be repainted (the covered part)
        if ((nOldOffsetY != 0) && (GetParent() != nullptr)) {
            UiRect rcInvalid = GetRect();
            rcInvalid.Offset(0, -nOldOffsetY);
            GetParent()->InvalidateRect(rcInvalid);
        }
    }
}

void Control::PauseImageAnimation()
{
    // Stop all animations in this control
    if (m_pBkImage != nullptr) {
        m_pBkImage->PauseImageAnimation();
    }
    if (m_pImageMap != nullptr) {
        m_pImageMap->PauseImageAnimation();
    }
}

Image* Control::FindImageByName(const DString& imageName) const
{
    Image* pImage = nullptr;
    if (imageName.empty()) {
        // If empty, use the background image
        pImage = m_pBkImage.get();
    }
    else if ((m_pBkImage != nullptr) && (m_pBkImage->GetImageAttribute().m_sImageName == imageName)) {
        // Background image
        pImage = m_pBkImage.get();
    }
    else if (m_pImageMap != nullptr) {
        // State image
        pImage = m_pImageMap->FindImageByName(imageName);
    }
    return pImage;
}

bool Control::StartImageAnimation(const DString& imageName,
                                  AnimationImagePos nStartFrame,
                                  int32_t nPlayCount)
{
    GlobalManager::Instance().AssertUIThread();
    Image* pImage = FindImageByName(imageName);
    if (pImage == nullptr) {
        return false;
    }
    if (!LoadImageInfo(*pImage)) {
        return false;
    }
    return pImage->StartImageAnimation(nStartFrame, nPlayCount);
}

bool Control::StopImageAnimation(const DString& imageName,
                                 AnimationImagePos nStopFrame,
                                 bool bTriggerEvent)
{
    GlobalManager::Instance().AssertUIThread();
    Image* pImage = FindImageByName(imageName);
    if (pImage == nullptr) {
        return false;
    }
    if (pImage != nullptr) {
        pImage->StopImageAnimation(nStopFrame, bTriggerEvent);
        return true;
    }
    return false;
}

bool Control::SetImageAnimationFrame(int32_t nFrameIndex)
{
    return SetImageAnimationFrame(DString(), nFrameIndex);
}

bool Control::SetImageAnimationFrame(const DString& imageName, int32_t nFrameIndex)
{
    GlobalManager::Instance().AssertUIThread();
    ASSERT(nFrameIndex >= 0);
    if (nFrameIndex < 0) {
        return false;
    }
    Image* pImage = FindImageByName(imageName);
    if (pImage == nullptr) {
        return false;
    }
    if (pImage != nullptr) {
        pImage->SetCurrentFrameIndex((uint32_t)nFrameIndex);
        // Repaint
        Invalidate();
        return true;
    }
    return false;
}

uint32_t Control::GetImageAnimationFrameIndex() const
{
    return GetImageAnimationFrameIndex(DString());
}

uint32_t Control::GetImageAnimationFrameIndex(const DString& imageName) const
{
    GlobalManager::Instance().AssertUIThread();
    Image* pImage = FindImageByName(imageName);
    if (pImage == nullptr) {
        return 0;
    }
    return pImage->GetCurrentFrameIndex();
}

uint32_t Control::GetImageAnimationFrameCount()
{
    return GetImageAnimationFrameCount(DString());
}

uint32_t Control::GetImageAnimationFrameCount(const DString& imageName)
{
    GlobalManager::Instance().AssertUIThread();
    Image* pImage = FindImageByName(imageName);
    if (pImage == nullptr) {
        return 0;
    }
    if (!LoadImageInfo(*pImage)) {
        return 0;
    }
    return pImage->GetFrameCount();
}

bool  Control::IsImageAnimationLoaded() const
{
    return IsImageAnimationLoaded(DString());
}

bool  Control::IsImageAnimationLoaded(const DString& imageName) const
{
    GlobalManager::Instance().AssertUIThread();
    Image* pImage = FindImageByName(imageName);
    if (pImage == nullptr) {
        return false;
    }
    return pImage->GetImageInfo() != nullptr;
}

/** The data structure for multi-threaded decoding (asynchronous, decoded in the child thread)
*/
struct Control::TAsyncImageDecode
{
    ControlPtr m_pControl;                // The associated control interface
    ControlPtrT<Image> m_pImage;          // The associated image interface
    DString m_imagePath;                  // The path of the image to load

    std::shared_ptr<IImage> m_pImageData; // The image data interface
    DString m_imageKey;                   // The KEY of the image data, used to update the UI display
    size_t m_nTaskId = 0;                 // The task ID in the child thread

    uint32_t m_nFrameCount = 0;           // How many frames this image has in total
    uint32_t m_nDecodeCount = 0;          // How many asynchronous decodes are executed in total

    bool m_bDecodeExecuted = false;       // Whether an image decode operation has been executed
    bool m_bDecodeResult = false;         // Whether the asynchronous decode succeeded
    bool m_bDecodeError = false;          // Whether the asynchronous decode encountered an error
};

/** The implementation function of the multi-threaded decode (the parameter uses the TAsyncImageDecode smart pointer to avoid affecting the reference count of std::shared_ptr<IImage>)
*/
void Control::AsyncDecodeImageData(std::shared_ptr<TAsyncImageDecode> pAsyncDecoder)
{
    // Must be executed in the UI thread
    GlobalManager::Instance().AssertUIThread();
    if ((pAsyncDecoder == nullptr) || (pAsyncDecoder->m_pImageData == nullptr)) {
        return;
    }
    std::shared_ptr<IImage>& pImageData = pAsyncDecoder->m_pImageData;
    if (!pImageData->IsAsyncDecodeEnabled() || pImageData->IsAsyncDecodeFinished()) {
        // No need to decode in a thread, or the decode is already finished
        return;
    }

    if (pAsyncDecoder->m_nTaskId == 0) {
        if (pImageData->GetAsyncDecodeTaskId() != 0) {
            // Tasks cannot be executed in parallel; another thread is already decoding
            return;
        }
    }

    // Decode in a child thread
    ThreadManager& threadManager = GlobalManager::Instance().Thread();
    int32_t nThreadIdentifier = ui::kThreadUI;
    std::vector<int32_t> threadIdentifiers;
    if (pImageData->GetImageType() == ImageType::kImageAnimation) {
        // Multi-frame image
        threadIdentifiers.push_back(ui::kThreadImage2);
        threadIdentifiers.push_back(ui::kThreadImage1);
        threadIdentifiers.push_back(ui::kThreadWorker);
    }
    else {
        // Single-frame image
        threadIdentifiers.push_back(ui::kThreadImage1);
        threadIdentifiers.push_back(ui::kThreadImage2);
        threadIdentifiers.push_back(ui::kThreadWorker);
    }
    for (int32_t nThread : threadIdentifiers) {
        if (threadManager.HasThread(nThread)) {
            nThreadIdentifier = nThread;
            break;
        }
    }
    // The notification function of the asynchronous decode completion, executed in the main thread
    auto AsyncDecodeImageFinishNotify = [pAsyncDecoder]() {
            // Must be executed in the UI thread
            GlobalManager::Instance().AssertUIThread();
            if (pAsyncDecoder == nullptr) {
                return;
            }
            if (!pAsyncDecoder->m_bDecodeExecuted) {
                // No image decode operation was executed; no further processing is needed
                return;
            }
            int32_t nUseCount = pAsyncDecoder->m_pImageData.use_count(); // Resource reference count
            if (nUseCount == 1) {
                // The resource has been released; no further processing is needed
                return;
            }

            // Decode count
            pAsyncDecoder->m_nDecodeCount++;

            // Merge the data
            pAsyncDecoder->m_pImageData->MergeAsyncDecodeData();

            // Notify the related controls to repaint the UI
            GlobalManager::Instance().Image().DelayPaintImage(pAsyncDecoder->m_imageKey);

            bool bDecodeFinished = pAsyncDecoder->m_pImageData->IsAsyncDecodeFinished();
            bool bDecodeEnabled = pAsyncDecoder->m_pImageData->IsAsyncDecodeEnabled();
            ASSERT(pAsyncDecoder->m_nDecodeCount <= pAsyncDecoder->m_nFrameCount);
            if (pAsyncDecoder->m_nDecodeCount == pAsyncDecoder->m_nFrameCount) {
                ASSERT(bDecodeFinished);
            }

            if (!bDecodeFinished && bDecodeEnabled &&
                pAsyncDecoder->m_bDecodeResult &&
                !pAsyncDecoder->m_bDecodeError &&
                (pAsyncDecoder->m_nDecodeCount <= pAsyncDecoder->m_nFrameCount)) {
                // If not finished, continue decoding the next frame
                pAsyncDecoder->m_bDecodeExecuted = false;
                AsyncDecodeImageData(pAsyncDecoder);
            }
            else {
                // Clear the task ID (only cleared when finished)
                pAsyncDecoder->m_pImageData->SetAsyncDecodeTaskId(0);

                if ((pAsyncDecoder->m_pControl != nullptr) && (pAsyncDecoder->m_pImage != nullptr)) {
                    bool bDecodeError = true; // The default is a decode error
                    if (!pAsyncDecoder->m_bDecodeError && bDecodeFinished) {
                        // Decode finished
                        bDecodeError = false;
                    }
                    if (pAsyncDecoder->m_bDecodeError) {
                        pAsyncDecoder->m_pImage->SetImageError(true);
                    }
                    pAsyncDecoder->m_pControl->FireImageEvent(pAsyncDecoder->m_pImage.get(), pAsyncDecoder->m_imagePath, false, false, bDecodeError);
                }
            }
        };

    // Confirm the frame index to decode
    uint32_t nCurFrameIndex = 0;
    if (pImageData->GetImageType() == ImageType::kImageAnimation) {
        // Multi-frame
        std::shared_ptr<IAnimationImage> pAnimationImage = pImageData->GetImageAnimation();
        if (pAnimationImage != nullptr) {
            const int32_t nFrameCount = pAnimationImage->GetFrameCount();
            if (nFrameCount > 1) {
                nCurFrameIndex = pAnimationImage->GetDecodedFrameIndex() + 1;
                if (nCurFrameIndex >= (uint32_t)nFrameCount) {
                    nCurFrameIndex = nFrameCount - 1;
                }
            }
        }
    }
    else {
        // Single-frame
        nCurFrameIndex = 0;
    }

    // The asynchronous decode function, executed in the child thread
    auto AsyncDecodeImageFunction = [pAsyncDecoder, nCurFrameIndex, AsyncDecodeImageFinishNotify]() {
            int32_t nUseCount = pAsyncDecoder->m_pImageData.use_count(); // Resource reference count (when the count is 1, the resource has been released and no more decoding is needed)
            if ((nUseCount > 1) &&
                !pAsyncDecoder->m_pImageData->IsAsyncDecodeFinished() &&
                pAsyncDecoder->m_pImageData->IsAsyncDecodeEnabled()) {

                // The function to determine whether to cancel the operation
                auto IsAborted = [pAsyncDecoder]() {
                        if (pAsyncDecoder->m_pImageData.use_count() == 1) {
                            // Already released: details to be improved
                            return true;
                        }
                        return false;
                    };
                // Asynchronously decode the image data
                pAsyncDecoder->m_bDecodeExecuted = true;
                pAsyncDecoder->m_bDecodeResult = pAsyncDecoder->m_pImageData->AsyncDecode(nCurFrameIndex, IsAborted, &pAsyncDecoder->m_bDecodeError);
            }

            // Notify the UI (whether or not an image decode operation was executed, the UI must be notified,
            // the main purpose is to let the smart pointer object pAsyncDecoder->m_pImageData be released in the UI thread,
            // to avoid releasing it in the child thread causing resource conflicts and program crashes)
            size_t nTaskId = GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, AsyncDecodeImageFinishNotify);
            ASSERT_UNUSED_VARIABLE(nTaskId > 0);
        };

    // Put it into the child thread and start decoding
    size_t nTaskId = threadManager.PostTask(nThreadIdentifier, AsyncDecodeImageFunction);
    pAsyncDecoder->m_nTaskId = nTaskId;
    pAsyncDecoder->m_pImageData->SetAsyncDecodeTaskId(nTaskId);
}

bool Control::LoadImageInfo(Image& duiImage, bool bPaintImage) const
{
    GlobalManager::Instance().AssertUIThread();
    // DPI scale percentage
    const uint32_t nLoadDpiScale = Dpi().GetDisplayScaleFactor();
    if (duiImage.GetImageInfo() != nullptr) {
        // If the image cache exists and the DPI scale percentage is unchanged, do not load it again (when the image changes, this cache will be cleared)
        if (duiImage.GetImageInfo()->GetLoadDpiScale() == nLoadDpiScale) {
            return true;
        }        
    }
    Window* pWindow = GetWindow();
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return false;
    }

    if (duiImage.HasImageError()) {
        // If the image failed to load, do not reload the image
        return false;
    }

    DString sImagePath = duiImage.GetImagePath();
    if (duiImage.GetImageAttribute().IsAssertEnabled()) {
        ASSERT(!sImagePath.empty());
    }
    if (sImagePath.empty()) {
        // The image resource path is empty; mark the load as failed
        duiImage.SetImageError(true);
        FireImageEvent(&duiImage, sImagePath, true, true, false);
        return false;
    }
    ImageLoadPath imageLoadPath; // The image load path information
    imageLoadPath.m_pathType = ImageLoadPathType::kUnknownPath;    
    IconManager& iconManager = GlobalManager::Instance().Icon();
    if (iconManager.IsIconString(sImagePath)) {
        uint32_t nIconID = iconManager.GetIconID(sImagePath);
        if (iconManager.IsImageString(nIconID)) {
            // The resource image path (updated once; after the update, iconManager.IsIconString becomes false)
            DString iconImageString = iconManager.GetImageString(nIconID);
            ASSERT(!iconImageString.empty());
            DString oldImageString = duiImage.GetImageString();
            duiImage.SetImageString(iconImageString, pWindow->Dpi());
            duiImage.UpdateImageAttribute(oldImageString, pWindow->Dpi());
            sImagePath = duiImage.GetImagePath();// Update the image path to the path specified by the resource
            ASSERT(!sImagePath.empty());
            if (sImagePath.empty()) {
                // The image resource path is empty; mark the load as failed
                duiImage.SetImageError(true);
                FireImageEvent(&duiImage, sImagePath, true, true, false);
                return false;
            }
        }
        else {
            // ICON icon data, virtual path
            imageLoadPath.m_pathType = ImageLoadPathType::kVirtualPath;
        }
    }
    if (imageLoadPath.m_pathType == ImageLoadPathType::kVirtualPath) {
        // ICON icon data, virtual path
        imageLoadPath.m_imageFullPath = sImagePath;
    }
    else {
        // Non-icon data: get the full path of the image resource (an absolute disk path or a relative path inside the zip archive)
        FilePath resPath(sImagePath);
        bool bLocalPath = true;
        bool bResPath = true;
        const FilePath windowResPath = pWindow->GetResourcePath();
        const FilePath windowXmlPath = pWindow->GetXmlPath();
        FilePath imageFullPath = GlobalManager::Instance().GetExistsResFullPath(windowResPath, windowXmlPath, resPath, this, bLocalPath, bResPath);
        if (!imageFullPath.IsEmpty()) {
            imageLoadPath.m_imageFullPath = imageFullPath.NativePath();
            if (bLocalPath) {
                if (bResPath) {
                    imageLoadPath.m_pathType = ImageLoadPathType::kLocalResPath;
                }
                else {
                    imageLoadPath.m_pathType = ImageLoadPathType::kLocalPath;
                }
            }
            else {
                imageLoadPath.m_pathType = ImageLoadPathType::kZipResPath;
            }
        }
    }
    if (duiImage.GetImageAttribute().IsAssertEnabled()) {
        ASSERT(!imageLoadPath.m_imageFullPath.IsEmpty());
    }
    if (imageLoadPath.m_imageFullPath.IsEmpty()) {
        // The image resource file does not exist; mark the load as failed
        duiImage.SetImageError(true);
        FireImageEvent(&duiImage, imageLoadPath.m_imageFullPath.NativePath(), true, true, false);
        return false;
    }

    ImageLoadParam imageLoadParam = duiImage.GetImageLoadParam();
    imageLoadParam.SetLoadDpiScale(nLoadDpiScale);  // Set the DPI percentage for loading
    imageLoadParam.SetImageLoadPath(imageLoadPath); // Set the path of the image resource
    std::shared_ptr<ImageInfo> imageInfo = duiImage.GetImageInfo();
    if ((imageInfo == nullptr) ||
        (imageInfo->GetLoadKey() != imageLoadParam.GetLoadKey(nLoadDpiScale))) {
        // Case 1: if the image has not been loaded, load the image;
        // Case 2: if the image has changed, reload the image

        // Whether to enable the image load optimization (load the image at the smallest scale, occupying the least memory and painting the fastest); the enabling conditions are summarized as:
        // 1. This optimization can be enabled only when the image is loaded at paint time, because for an image loaded at that time, changing the load scale only affects the display effect of the image, not the layout of the control and the image
        // 2. If the image is specified as not supporting DPI adaptation (dpi_scale="false"), this optimization is disabled
        // 3. If the paint attribute specifies tiled drawing (xtiled="true" or ytiled="true"), this optimization is disabled
        // 4. If the paint attribute specifies shadow mode (window_shadow_mode="true"), this optimization is disabled
        // 5. If the paint attribute specifies adaptive mode (adaptive_dest_rect="true"), this optimization is disabled
        // 6. If the paint attribute specifies nine-slice drawing (corner="left,top,right,bottom"), this optimization is disabled
        bool bEnableImageLoadSizeOpt = bPaintImage;
        if (duiImage.GetImageAttribute().IsTiledDraw() ||
            duiImage.GetImageAttribute().m_bWindowShadowMode ||
           !duiImage.GetImageAttribute().m_bImageDpiScaleEnabled ||
            duiImage.GetImageAttribute().m_bAdaptiveDestRect ||
            duiImage.GetImageAttribute().HasImageCorner()) {
            bEnableImageLoadSizeOpt = false;
        }
        
        uint32_t nImageSetWidth = 0;
        uint32_t nImageSetHeight = 0;
        if (imageLoadParam.GetImageFixedSize(nImageSetWidth, nImageSetHeight)) {
            // If the image specifies a width or height (e.g. width="100" or height="100"), the most suitable scale ratio can be calculated at load time to improve efficiency, without side effects
            imageLoadParam.SetMaxDestRectSize(UiSize((int32_t)nImageSetWidth, (int32_t)nImageSetHeight));
        }
        else if (bEnableImageLoadSizeOpt) {
            // Images loaded at the paint stage do not need the image width and height to determine the target area, so load optimization is possible (for large images, a smaller image can be loaded, improving the paint speed while ensuring paint quality, and reducing memory usage)
            imageLoadParam.SetMaxDestRectSize(UiSize(GetRect().Width(), GetRect().Height()));
        }

        bool bImageDataFromCache = false;
        imageInfo = GlobalManager::Instance().Image().GetImage(imageLoadParam, bImageDataFromCache);
        duiImage.SetImageInfo(imageInfo);
        if (imageInfo != nullptr) {
            // Check and start the multi-threaded decode; decode the image data in a child thread
            std::shared_ptr<IImage> pImageData = imageInfo->GetImageData();
            if (pImageData != nullptr) {
                std::shared_ptr<TAsyncImageDecode> pAsyncDecoder = std::make_shared<TAsyncImageDecode>();                
                pAsyncDecoder->m_nFrameCount = imageInfo->GetFrameCount();
                pAsyncDecoder->m_nDecodeCount = 0;
                pAsyncDecoder->m_nTaskId = 0;
                pAsyncDecoder->m_pImageData = std::move(pImageData);
                pAsyncDecoder->m_imageKey = imageInfo->GetImageKey();
                pAsyncDecoder->m_pControl = const_cast<Control*>(this);
                pAsyncDecoder->m_pImage = &duiImage;
                pAsyncDecoder->m_imagePath = imageLoadPath.m_imageFullPath.NativePath();

                if (!bImageDataFromCache) {
                    // The image was reloaded
                    AsyncDecodeImageData(pAsyncDecoder);
                }
                else if (pAsyncDecoder->m_pImageData->IsAsyncDecodeEnabled() &&
                         !pAsyncDecoder->m_pImageData->IsAsyncDecodeFinished()) {
                    // The image obtained from the cache, but not yet loaded
                    AsyncDecodeImageData(pAsyncDecoder);
                }
            }
        }
    }
    if (imageInfo == nullptr) {
        // Mark the load as failed
        duiImage.SetImageError(true);
    }

    // The callback event of the image load result (asynchronous)
    bool bLoadError = (imageInfo == nullptr);
    FireImageEvent(&duiImage, imageLoadPath.m_imageFullPath.NativePath(), true, bLoadError, false);
    return imageInfo ? true : false;
}

void Control::FireImageEvent(Image* pImagePtr, const DString& imageFilePath, bool bLoadImage, bool bLoadError, bool bDecodeError) const
{
    if (pImagePtr == nullptr) {
        return;
    }
    if (!bLoadImage) {
        // Mark that the decode completion event has been notified
        pImagePtr->SetDecodeEventFired(true);
    }
    ControlPtr pControl(const_cast<Control*>(this));        // The control associated with the image
    ControlPtrT<Image> pImage(pImagePtr);                   // The image resource interface

    ImageDecodeResult decodeResult;
    decodeResult.m_pControl = pControl.get();               // The control associated with the image
    decodeResult.m_pImage = pImage.get();                   // The image resource interface
    decodeResult.m_imageFilePath = imageFilePath;           // The image path
    decodeResult.m_imageName = pImage->GetImageName();      // The image name, a unique ID
    decodeResult.m_bBkImage = (GetBkImagePtr() == pImagePtr);   // Whether this image is the background image
    decodeResult.m_bLoadError = bLoadError;                     // Whether this image has a load error
    decodeResult.m_bDecodeError = bDecodeError;                 // Whether this image has a data decode error

    auto LoadImageCallback = [pControl, pImage, bLoadImage, decodeResult]() {
            if ((pControl != nullptr) && (pImage != nullptr)) {
                if (bLoadImage) {
                    pControl->SendEvent(kEventImageLoad, (WPARAM)&decodeResult);
                }
                else {
                    pControl->SendEvent(kEventImageDecode, (WPARAM)&decodeResult);
                }
            }
        };
    GlobalManager::Instance().Thread().PostTask(ui::kThreadUI, LoadImageCallback);
}

void Control::ClearImageCache()
{
    if (m_pImageMap != nullptr) {
        m_pImageMap->ClearImageCache();
    }
    if (m_pBkImage != nullptr) {
        m_pBkImage->ClearImageCache();
    }
}

void Control::AttachEvent(EventType eventType, const EventCallback& callback, EventCallbackID callbackID)
{
    EventMap& attachEventMap = GetAttachEventMap();
    attachEventMap[eventType].AddEventCallback(callback, callbackID);
    if ((eventType == kEventContextMenu) || (eventType == kEventAll)) {
        SetContextMenuUsed(true);
    }
}

void Control::DetachEvent(EventType eventType)
{
    if (!HasAttachEventMap()) {
        return;
    }
    EventMap& attachEventMap = GetAttachEventMap();
    auto event = attachEventMap.find(eventType);
    if (event != attachEventMap.end()) {
        attachEventMap.erase(event);
    }
    if ((eventType == kEventContextMenu) || (eventType == kEventAll)) {
        if ((attachEventMap.find(kEventAll) == attachEventMap.end()) &&
            (attachEventMap.find(kEventContextMenu) == attachEventMap.end())) {
            SetContextMenuUsed(false);
        }
    }
}

void Control::DetachEventByID(EventCallbackID callbackID)
{
    if (!HasAttachEventMap()) {
        return;
    }
    EventMap& attachEventMap = GetAttachEventMap();
    EventUtils::RemoveEventCallbackByID(attachEventMap, callbackID);
}

void Control::DetachEventByID(EventType eventType, EventCallbackID callbackID)
{
    if (!HasAttachEventMap()) {
        return;
    }
    EventMap& attachEventMap = GetAttachEventMap();
    EventUtils::RemoveEventCallbackByID(attachEventMap, eventType, callbackID);
}

bool Control::HasEvent(EventType eventType) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    const EventMap& eventMap = m_pEventMapData->m_attachEvent;
    return eventMap.find(eventType) != eventMap.end();
}

bool Control::HasEventByID(EventCallbackID callbackID) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    const EventMap& eventMap = m_pEventMapData->m_attachEvent;
    return EventUtils::HasEventCallbackByID(eventMap, callbackID);
}

bool Control::HasEventByID(EventType eventType, EventCallbackID callbackID) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    const EventMap& eventMap = m_pEventMapData->m_attachEvent;
    return EventUtils::HasEventCallbackByID(eventMap, eventType, callbackID);
}

void Control::AttachXmlEvent(EventType eventType, const EventCallback& callback, EventCallbackID callbackID)
{
    EventMap& xmlEventMap = GetXmlEventMap();
    xmlEventMap[eventType].AddEventCallback(callback, callbackID);
}

void Control::DetachXmlEvent(EventType eventType)
{
    if (!HasXmlEventMap()) {
        return;
    }
    EventMap& xmlEventMap = GetXmlEventMap();
    auto event = xmlEventMap.find(eventType);
    if (event != xmlEventMap.end()) {
        xmlEventMap.erase(event);
    }
}

void Control::DetachXmlEventByID(EventCallbackID callbackID)
{
    if (!HasXmlEventMap()) {
        return;
    }
    EventMap& xmlEventMap = GetXmlEventMap();
    EventUtils::RemoveEventCallbackByID(xmlEventMap, callbackID);
}

void Control::DetachXmlEventByID(EventType eventType, EventCallbackID callbackID)
{
    if (!HasXmlEventMap()) {
        return;
    }
    EventMap& xmlEventMap = GetXmlEventMap();
    EventUtils::RemoveEventCallbackByID(xmlEventMap, eventType, callbackID);
}

bool Control::HasXmlEvent(EventType eventType) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    if (m_pEventMapData->m_pXmlEvent == nullptr) {
        return false;
    }
    const EventMap& eventMap = *m_pEventMapData->m_pXmlEvent;
    return eventMap.find(eventType) != eventMap.end();
}

bool Control::HasXmlEventByID(EventCallbackID callbackID) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    if (m_pEventMapData->m_pXmlEvent == nullptr) {
        return false;
    }
    const EventMap& eventMap = *m_pEventMapData->m_pXmlEvent;
    return EventUtils::HasEventCallbackByID(eventMap, callbackID);
}

bool Control::HasXmlEventByID(EventType eventType, EventCallbackID callbackID) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    if (m_pEventMapData->m_pXmlEvent == nullptr) {
        return false;
    }
    const EventMap& eventMap = *m_pEventMapData->m_pXmlEvent;
    return EventUtils::HasEventCallbackByID(eventMap, eventType, callbackID);
}

void Control::AttachBubbledEvent(EventType eventType, const EventCallback& callback, EventCallbackID callbackID)
{
    EventMap& bubbledEventMap = GetBubbledEventMap();
    bubbledEventMap[eventType].AddEventCallback(callback, callbackID);
}

void Control::DetachBubbledEvent(EventType eventType)
{
    if (!HasBubbledEventMap()) {
        return;
    }
    EventMap& bubbledEventMap = GetBubbledEventMap();
    auto event = bubbledEventMap.find(eventType);
    if (event != bubbledEventMap.end()) {
        bubbledEventMap.erase(eventType);
    }
}

void Control::DetachBubbledEventByID(EventCallbackID callbackID)
{
    if (!HasBubbledEventMap()) {
        return;
    }
    EventMap& bubbledEventMap = GetBubbledEventMap();
    EventUtils::RemoveEventCallbackByID(bubbledEventMap, callbackID);
}

void Control::DetachBubbledEventByID(EventType eventType, EventCallbackID callbackID)
{
    if (!HasBubbledEventMap()) {
        return;
    }
    EventMap& bubbledEventMap = GetBubbledEventMap();
    EventUtils::RemoveEventCallbackByID(bubbledEventMap, eventType, callbackID);
}

bool Control::HasBubbledEvent(EventType eventType) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    if (m_pEventMapData->m_pBubbledEvent == nullptr) {
        return false;
    }
    const EventMap& eventMap = *m_pEventMapData->m_pBubbledEvent;
    return eventMap.find(eventType) != eventMap.end();
}

bool Control::HasBubbledEventByID(EventCallbackID callbackID) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    if (m_pEventMapData->m_pBubbledEvent == nullptr) {
        return false;
    }
    const EventMap& eventMap = *m_pEventMapData->m_pBubbledEvent;
    return EventUtils::HasEventCallbackByID(eventMap, callbackID);
}

bool Control::HasBubbledEventByID(EventType eventType, EventCallbackID callbackID) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    if (m_pEventMapData->m_pBubbledEvent == nullptr) {
        return false;
    }
    const EventMap& eventMap = *m_pEventMapData->m_pBubbledEvent;
    return EventUtils::HasEventCallbackByID(eventMap, eventType, callbackID);
}

void Control::AttachXmlBubbledEvent(EventType eventType, const EventCallback& callback, EventCallbackID callbackID)
{
    EventMap& xmlBubbledEventMap = GetXmlBubbledEventMap();
    xmlBubbledEventMap[eventType].AddEventCallback(callback, callbackID);
}

void Control::DetachXmlBubbledEvent(EventType eventType)
{
    if (!HasXmlBubbledEventMap()) {
        return;
    }
    EventMap& xmlBubbledEventMap = GetXmlBubbledEventMap();
    auto event = xmlBubbledEventMap.find(eventType);
    if (event != xmlBubbledEventMap.end())    {
        xmlBubbledEventMap.erase(eventType);
    }
}

void Control::DetachXmlBubbledEventByID(EventCallbackID callbackID)
{
    if (!HasXmlBubbledEventMap()) {
        return;
    }
    EventMap& xmlBubbledEventMap = GetXmlBubbledEventMap();
    EventUtils::RemoveEventCallbackByID(xmlBubbledEventMap, callbackID);
}

void Control::DetachXmlBubbledEventByID(EventType eventType, EventCallbackID callbackID)
{
    if (!HasXmlBubbledEventMap()) {
        return;
    }
    EventMap& xmlBubbledEventMap = GetXmlBubbledEventMap();
    EventUtils::RemoveEventCallbackByID(xmlBubbledEventMap, eventType, callbackID);
}

bool Control::HasXmlBubbledEvent(EventType eventType) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    if (m_pEventMapData->m_pXmlBubbledEvent == nullptr) {
        return false;
    }
    const EventMap& eventMap = *m_pEventMapData->m_pXmlBubbledEvent;
    return eventMap.find(eventType) != eventMap.end();
}

bool Control::HasXmlBubbledEventByID(EventCallbackID callbackID) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    if (m_pEventMapData->m_pXmlBubbledEvent == nullptr) {
        return false;
    }
    const EventMap& eventMap = *m_pEventMapData->m_pXmlBubbledEvent;
    return EventUtils::HasEventCallbackByID(eventMap, callbackID);
}

bool Control::HasXmlBubbledEventByID(EventType eventType, EventCallbackID callbackID) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    if (m_pEventMapData->m_pXmlBubbledEvent == nullptr) {
        return false;
    }
    const EventMap& eventMap = *m_pEventMapData->m_pXmlBubbledEvent;
    return EventUtils::HasEventCallbackByID(eventMap, eventType, callbackID);
}

bool Control::FireAllEvents(const EventArgs& msg)
{
    if (msg.IsSenderExpired()) {
        return false;
    }
    bool bRet = FireNormalEvents(msg);
    if (bRet) {
        bRet = FireBubbledEvents(msg);
    }
    return bRet;
}

bool Control::FireNormalEvents(const EventArgs & msg)
{
    if (msg.IsSenderExpired()) {
        return false;
    }
    // Note: the callbacks in EventMap and XmlEventMap need to verify whether the sender of the message is the control itself
    if (msg.GetSender() != this) {
        return true;
    }
    std::weak_ptr<WeakFlag> weakflag = GetWeakFlag();
    bool bRet = true;// When the value is false, the callback functions and the handler are no longer called
    if (bRet && HasAttachEventMap() && !GetAttachEventMap().empty()) {
        const EventMap& attachEventMap = GetAttachEventMap();
        auto callback = attachEventMap.find(msg.eventType);
        if (callback != attachEventMap.end()) {
            bRet = callback->second(msg);
        }
        if (weakflag.expired() || msg.IsSenderExpired()) {
            return false;
        }

        callback = attachEventMap.find(kEventAll);
        if (callback != attachEventMap.end()) {
            bRet = callback->second(msg);
        }
        if (weakflag.expired() || msg.IsSenderExpired()) {
            return false;
        }
    }

    if (bRet && HasXmlEventMap() && !GetXmlEventMap().empty()) {
        const EventMap& xmlEventMap = GetXmlEventMap();
        auto callback = xmlEventMap.find(msg.eventType);
        if (callback != xmlEventMap.end()) {
            bRet = callback->second(msg);
        }
        if (weakflag.expired() || msg.IsSenderExpired()) {
            return false;
        }

        callback = xmlEventMap.find(kEventAll);
        if (callback != xmlEventMap.end()) {
            bRet = callback->second(msg);
        }
        if (weakflag.expired() || msg.IsSenderExpired()) {
            return false;
        }
    }
    return bRet && !weakflag.expired();
}

bool Control::FireBubbledEvents(const EventArgs& msg)
{
    if (msg.IsSenderExpired()) {
        return false;
    }
    // Note: the callbacks in BubbledEventMap and XmlBubbledEventMap do not need to verify whether the sender of the message is the control itself
    std::weak_ptr<WeakFlag> weakflag = GetWeakFlag();
    bool bRet = true;// When the value is false, the callback functions and the handler are no longer called
    if (bRet && HasBubbledEventMap() && !GetBubbledEventMap().empty()) {
        const EventMap& bubbledEventMap = GetBubbledEventMap();
        auto callback = bubbledEventMap.find(msg.eventType);
        if (callback != bubbledEventMap.end()) {
            bRet = callback->second(msg);
        }
        if (weakflag.expired() || msg.IsSenderExpired()) {
            return false;
        }

        callback = bubbledEventMap.find(kEventAll);
        if (callback != bubbledEventMap.end()) {
            bRet = callback->second(msg);
        }
        if (weakflag.expired() || msg.IsSenderExpired()) {
            return false;
        }
    }

    if (bRet && HasXmlBubbledEventMap() && !GetXmlBubbledEventMap().empty()) {
        const EventMap& xmlBubbledEventMap = GetXmlBubbledEventMap();
        auto callback = xmlBubbledEventMap.find(msg.eventType);
        if (callback != xmlBubbledEventMap.end()) {
            bRet = callback->second(msg);
        }
        if (weakflag.expired() || msg.IsSenderExpired()) {
            return false;
        }

        callback = xmlBubbledEventMap.find(kEventAll);
        if (callback != xmlBubbledEventMap.end()) {
            bRet = callback->second(msg);
        }
        if (weakflag.expired() || msg.IsSenderExpired()) {
            return false;
        }
    }
    return bRet && !weakflag.expired();
}

bool Control::HasEventCallback(EventType eventType) const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    if (!m_pEventMapData->m_attachEvent.empty()) {
        const EventMap& eventMap = m_pEventMapData->m_attachEvent;
        if (eventMap.find(eventType) != eventMap.end()) {
            return true;
        }
    }
    if (m_pEventMapData->m_pXmlEvent != nullptr){
        const EventMap& eventMap = *m_pEventMapData->m_pXmlEvent;
        if (!eventMap.empty() && eventMap.find(eventType) != eventMap.end()) {
            return true;
        }
    }
    if (m_pEventMapData->m_pBubbledEvent != nullptr) {
        const EventMap& eventMap = *m_pEventMapData->m_pBubbledEvent;
        if (!eventMap.empty() && eventMap.find(eventType) != eventMap.end()) {
            return true;
        }
    }
    if (m_pEventMapData->m_pXmlBubbledEvent != nullptr) {
        const EventMap& eventMap = *m_pEventMapData->m_pXmlBubbledEvent;
        if (!eventMap.empty() && eventMap.find(eventType) != eventMap.end()) {
            return true;
        }
    }
    return false;
}

bool Control::HasUiColor(const DString& colorName) const
{
    if (colorName.empty()) {
        return false;
    }
    UiColor color = GetUiColorByName(colorName);
    return color.GetARGB() != 0;
}

UiColor Control::GetUiColor(const DString& colorName) const
{
    if (colorName.empty()) {
        return UiColor();
    }
    UiColor color = GetUiColorByName(colorName);
    ASSERT(!color.IsEmpty());
    return color;
}

UiColor Control::GetUiColorByName(const DString& colorName) const
{
    UiColor color;
    if (colorName.empty()) {
        return color;
    }
    if (colorName.at(0) == _T('#')) {
        // Priority 1: starts with the '#' character, directly specifying the color value, e.g. #FFFFFFFF
        color = ColorManager::ConvertToUiColor(colorName);
    }
    if (color.GetARGB() == 0) {
        Window* pWindow = GetWindow();
        if (pWindow != nullptr) {
            // Priority 2: get the color specified by the child node <TextColor> defined in the <Window> node of the configuration XML
            color = pWindow->GetTextColor(colorName);
        }
    }
    if (color.GetARGB() == 0) {
        // Priority 3: get the color specified by the child node <TextColor> defined in the <Global> node of global.xml
        color = GlobalManager::Instance().Color().GetColor(colorName);
    }
    if (color.GetARGB() == 0) {
        // Priority 4: directly specify a predefined color alias
        color = GlobalManager::Instance().Color().GetStandardColor(colorName);
    }
    ASSERT(color.GetARGB() != 0);
    return color;
}

DString Control::GetColorString(const UiColor& color) const
{
    if (color.IsEmpty()) {
        return DString();
    }
    else {
        return StringUtil::Printf(_T("#%02X%02X%02X%02X"), color.GetA(), color.GetR(), color.GetG(), color.GetB());
    }
}

bool Control::HasBoxShadow() const
{
    if ((m_pOtherData != nullptr) && (m_pOtherData->m_pBoxShadow != nullptr)) {
        return m_pOtherData->m_pBoxShadow->HasShadow();
    }
    return false;
}

UiRect Control::GetBoxShadowExpandedRect(const UiRect& rc) const
{
    if ((m_pOtherData != nullptr) &&
        (m_pOtherData->m_pBoxShadow != nullptr) &&
         m_pOtherData->m_pBoxShadow->HasShadow()) {
        return m_pOtherData->m_pBoxShadow->GetExpandedRect(rc);
    }
    return rc;
}

bool Control::IsSelectableType() const
{
    return false;
}

bool Control::IsWantTab() const
{
    return false;
}

bool Control::CanPlaceCaptionBar() const
{
    return false;
}

bool Control::CheckVisibleAncestor(void) const
{
    bool isVisible = IsVisible();
    if (isVisible) {
        Control* parent = GetParent();
        while (parent != nullptr)
        {
            if (!parent->IsVisible()) {
                isVisible = false;
                break;
            }
            parent = parent->GetParent();
        }
    }
    return isVisible;
}

bool Control::IsKeyDown(const EventArgs& msg, ModifierKey modifierKey) const
{
    Window* pWindow = GetWindow();
    ASSERT(pWindow != nullptr);
    if (pWindow != nullptr) {
        return pWindow->IsKeyDown(msg, modifierKey);
    }
    return false;
}

void Control::EnsureNoFocus()
{
    Window* pWindow = GetWindow();
    if ((pWindow != nullptr) && pWindow->GetFocusControl() != nullptr) {
        if (pWindow->GetFocusControl() == this) {
            pWindow->SetFocusControl(nullptr);
        }
    }
}

bool Control::MousePosToLayoutPos(const UiPoint& ptMouse, UiPoint& ptLayoutPos)
{
    ptLayoutPos.x = 0;
    ptLayoutPos.y = 0;
    UiPoint pt(ptMouse);
    pt.Offset(GetScrollOffsetInScrollBox()); // Convert the client area coordinates of the mouse to the control coordinates
    UiRect rcRect = GetRect();
    if (rcRect.ContainsPt(pt)) {
        // Indicates that the mouse is within the control range
        ptLayoutPos.x = pt.x - rcRect.left;
        ptLayoutPos.y = pt.y - rcRect.top;
        return true;
    }
    return false;
}

bool Control::ScreenToClient(UiPoint& pt)
{
    Window* pWindow = GetWindow();
    if ((pWindow == nullptr) || !pWindow->IsWindow()) {
        return false;
    }
    pWindow->ScreenToClient(pt);
    return true;
}

bool Control::ClientToScreen(UiPoint& pt)
{
    Window* pWindow = GetWindow();
    if ((pWindow == nullptr) || !pWindow->IsWindow()) {
        return false;
    }
    pWindow->ClientToScreen(pt);
    return true;
}

void Control::SetPaintOrder(uint8_t nPaintOrder)
{
    m_nPaintOrder = nPaintOrder;
}

uint8_t Control::GetPaintOrder() const
{
    return m_nPaintOrder;
}

IFont* Control::GetIFontById(const DString& strFontId) const
{
    return GlobalManager::Instance().Font().GetIFont(strFontId, this->Dpi());
}

bool Control::HasDestroyEventCallback() const
{
    if (m_pEventMapData == nullptr) {
        return false;
    }
    const EventMap& eventMap = m_pEventMapData->m_attachEvent;
    return eventMap.find(kEventDestroy) != eventMap.end();
}

EventMap& Control::GetAttachEventMap()
{
    if (m_pEventMapData == nullptr) {
        m_pEventMapData = std::make_unique<TEventMapData>();
    }
    return m_pEventMapData->m_attachEvent;
}

bool Control::HasAttachEventMap() const
{
    return m_pEventMapData != nullptr;
}

EventMap& Control::GetXmlEventMap()
{
    if (m_pEventMapData == nullptr) {
        m_pEventMapData = std::make_unique<TEventMapData>();
    }
    if (m_pEventMapData->m_pXmlEvent == nullptr) {
        m_pEventMapData->m_pXmlEvent = new EventMap;
    }
    return *m_pEventMapData->m_pXmlEvent;
}

bool Control::HasXmlEventMap() const
{
    return (m_pEventMapData != nullptr) && (m_pEventMapData->m_pXmlEvent != nullptr);
}

EventMap& Control::GetBubbledEventMap()
{
    if (m_pEventMapData == nullptr) {
        m_pEventMapData = std::make_unique<TEventMapData>();
    }
    if (m_pEventMapData->m_pBubbledEvent == nullptr) {
        m_pEventMapData->m_pBubbledEvent = new EventMap;
    }
    return *m_pEventMapData->m_pBubbledEvent;
}

bool Control::HasBubbledEventMap() const
{
    return (m_pEventMapData != nullptr) && (m_pEventMapData->m_pBubbledEvent != nullptr);
}

EventMap& Control::GetXmlBubbledEventMap()
{
    if (m_pEventMapData == nullptr) {
        m_pEventMapData = std::make_unique<TEventMapData>();
    }
    if (m_pEventMapData->m_pXmlBubbledEvent == nullptr) {
        m_pEventMapData->m_pXmlBubbledEvent = new EventMap;
    }
    return *m_pEventMapData->m_pXmlBubbledEvent;
}

bool Control::HasXmlBubbledEventMap() const
{
    return (m_pEventMapData != nullptr) && (m_pEventMapData->m_pXmlBubbledEvent != nullptr);
}

void Control::SetEnableDragDrop(bool bEnable)
{
    if (m_pDragDropData == nullptr) {
        m_pDragDropData = std::make_unique<TDragDropData>();
    }
    m_pDragDropData->m_bDragDropEnabled = bEnable;
}

bool Control::IsEnableDragDrop() const
{
    return (m_pDragDropData != nullptr) && m_pDragDropData->m_bDragDropEnabled;
}

void Control::SetEnableDropFile(bool bEnable)
{
    if (m_pDragDropData == nullptr) {
        m_pDragDropData = std::make_unique<TDragDropData>();
    }
    m_pDragDropData->m_bDropFileEnabled = bEnable;
    m_pDragDropData->m_bDropFileEnabledDefined = true;
}

bool Control::IsEnableDropFile() const
{
    if (m_pDragDropData != nullptr) {
        if (m_pDragDropData->m_bDropFileEnabledDefined) {
            return m_pDragDropData->m_bDropFileEnabled;
        }
        else {
            return m_pDragDropData->m_bDragDropEnabled;
        }
    }
    return false;
}

void Control::SetDropFileTypes(const DString& fileTypes)
{
    if (m_pDragDropData == nullptr) {
        m_pDragDropData = std::make_unique<TDragDropData>();
    }
    m_pDragDropData->m_dropFileTypes = fileTypes;
}

DString Control::GetDropFileTypes() const
{
    DString fileTypes;
    if (m_pDragDropData != nullptr) {
        fileTypes = m_pDragDropData->m_dropFileTypes.c_str();
    }
    return fileTypes;
}

ControlDropTarget_Windows* Control::GetControlDropTarget()
{
#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)
    if (IsEnableDragDrop() && IsEnabled()) {
        if (m_pDragDropData == nullptr) {
            m_pDragDropData = std::make_unique<TDragDropData>();
            m_pDragDropData->m_bDragDropEnabled = true;
        }
        m_pDragDropData->m_pDropTargetWindows = std::make_shared<ControlDropTargetImpl_Windows>(this);
        return m_pDragDropData->m_pDropTargetWindows.get();
    }
#endif
    return nullptr;
}

ControlDropTarget_SDL* Control::GetControlDropTarget_SDL()
{
#ifdef DUILIB_BUILD_FOR_SDL
    if (IsEnableDragDrop() && IsEnabled()) {
        if (m_pDragDropData == nullptr) {
            m_pDragDropData = std::make_unique<TDragDropData>();
            m_pDragDropData->m_bDragDropEnabled = true;
        }
        m_pDragDropData->m_pDropTargetSDL = std::make_shared<ControlDropTargetImpl_SDL>(this);
        return m_pDragDropData->m_pDropTargetSDL.get();
    }
#endif
    return nullptr;
}

} // namespace ui
