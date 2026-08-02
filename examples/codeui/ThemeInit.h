#ifndef EXAMPLES_CODEUI_THEME_INIT_H_
#define EXAMPLES_CODEUI_THEME_INIT_H_

#include "duilib/duilib.h"

/** Compile-time theme initialization - no XML parsing at runtime.
 *  Registers the same colors, fonts, and classes as global.xml.
 */
inline void InitThemeCompileTime()
{
    using namespace ui;

    // === Fonts ===
    auto& fm = GlobalManager::Instance().Font();
    fm.SetDefaultFontFamilyNames(
        _T("Noto Sans CJK SC,PingFang SC,SimSun,Microsoft YaHei"));

    // Register system fonts that global.xml would normally provide
    UiFont f;
    f.m_fontName = _T("system"); f.m_fontSize = 12;
    fm.AddFont(_T("system_12"), f, false);
    f.m_fontSize = 14; fm.AddFont(_T("system_14"), f, true);  // default
    f.m_fontSize = 16; fm.AddFont(_T("system_16"), f, false);
    f.m_fontSize = 18; fm.AddFont(_T("system_18"), f, false);
    f.m_fontSize = 20; fm.AddFont(_T("system_20"), f, false);
    f.m_fontSize = 22; fm.AddFont(_T("system_22"), f, false);

    // Bold variants
    f.m_bBold = true;
    f.m_fontSize = 12; fm.AddFont(_T("system_bold_12"), f, false);
    f.m_fontSize = 14; fm.AddFont(_T("system_bold_14"), f, false);
    f.m_fontSize = 16; fm.AddFont(_T("system_bold_16"), f, false);
    f.m_fontSize = 18; fm.AddFont(_T("system_bold_18"), f, false);
    f.m_fontSize = 20; fm.AddFont(_T("system_bold_20"), f, false);
    f.m_fontSize = 22; fm.AddFont(_T("system_bold_22"), f, false);
    f.m_bBold = false;

    // === Text Colors ===
    auto& cm = GlobalManager::Instance().Color();
    cm.AddColor(_T("white"),             UiColor(0xFFFFFFFF));
    cm.AddColor(_T("black"),             UiColor(0xFF000000));
    cm.AddColor(_T("darkcolor"),         UiColor(0xFF333333));
    cm.AddColor(_T("lightcolor"),        UiColor(0xFF888888));
    cm.AddColor(_T("gray"),              UiColor(0xFF8E99A6));
    cm.AddColor(_T("light_gray"),        UiColor(0xFFA8A8A8));
    cm.AddColor(_T("dark_gray"),         UiColor(0xFF72797F));
    cm.AddColor(_T("green"),             UiColor(0xFF00BB96));
    cm.AddColor(_T("light_green"),       UiColor(0xFF21C7A6));
    cm.AddColor(_T("blue"),              UiColor(0xFF006DD9));
    cm.AddColor(_T("red"),               UiColor(0xFFC63535));
    cm.AddColor(_T("default_font_color"),UiColor(0xFF000000));
    cm.AddColor(_T("disabled_font_color"),UiColor(0xFFA1AEBC));
    cm.AddColor(_T("default_link_font_color"),  UiColor(0xFF0000FF));
    cm.AddColor(_T("hover_link_font_color"),    UiColor(0xFFD3215F));
    cm.AddColor(_T("mouse_down_link_font_color"),UiColor(0xFFFF0000));
    cm.AddColor(_T("bk_wnd_darkcolor"),   UiColor(0xFFF0F2F5));
    cm.AddColor(_T("bk_wnd_lightcolor"),  UiColor(0xFFFFFFFF));
    cm.AddColor(_T("bk_menuitem_hovered"),UiColor(0xFFE1E6EB));
    cm.AddColor(_T("bk_menuitem_selected"),UiColor(0xFFCED4DB));
    cm.AddColor(_T("bk_listitem_hovered"),UiColor(0xFFF0F2F5));
    cm.AddColor(_T("bk_listitem_selected"),UiColor(0xFFE4E7EB));
    cm.AddColor(_T("bk_progress_darkcolor"), UiColor(0xFFE1E6EB));
    cm.AddColor(_T("bk_progress_lightcolor"), UiColor(0xFF2ECC71));
    cm.AddColor(_T("bk_combo_hovered"),   UiColor(0xFFDAE1E8));
    cm.AddColor(_T("bk_combo_selected"),  UiColor(0xFFC5CDD5));
    cm.AddColor(_T("bk_scrollbar_color"), UiColor(0xFFD3D9E0));
    cm.SetDefaultTextColor(_T("default_font_color"));

    // Note: Button classes (close, min, max, restore) are defined
    // inline in CodeForm::OnInitWindow via Window::AddClass() using
    // text symbols - no SVG files needed.
}

#endif
