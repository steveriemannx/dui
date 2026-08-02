#include "Item.h"
#include "icon_png.h"
#include <chrono>

#include <sys/mman.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>

// Local base64 decoder (same as generated_ui.inc, but avoids multiple-definition)
static const signed char kDecLocal[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
    52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
    -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
    15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
    -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
    41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};
inline DString DecodeIcon() {
    int fd = memfd_create("icon_png", MFD_CLOEXEC);
    if (fd < 0) return _T("");
    const unsigned char* s = (const unsigned char*)kIconPngB64;
    unsigned char buf[1024]; size_t di = 0; int val = 0, vb = -8;
    while (*s && di < sizeof(buf)) {
        int c = kDecLocal[*s++]; if (c == -1) continue;
        val = (val << 6) | c; vb += 6;
        if (vb >= 0) { buf[di++] = (unsigned char)((val >> vb) & 0xFF); vb -= 8; }
    }
    if (write(fd, buf, di) != (ssize_t)di) { close(fd); return _T(""); }
    char tmp[32]; snprintf(tmp, sizeof(tmp), "%d", fd);
    DString path = _T("/proc/self/fd/");
    for (char* p = tmp; *p; p++) path += (DString::value_type)(unsigned char)*p;
    return path;
}

Item::Item(ui::Window* pWindow):
    ui::ListBoxItem(pWindow),
    m_pImageControl(nullptr),
    m_pTitleLabel(nullptr),
    m_pProgressControl(nullptr),
    m_pDelBtn(nullptr),
    m_nDataIndex(0)
{
    SetClass(_T("listitem"));

    // Build item layout in pure C++ (zero XML/file dependencies)
    ui::HBox* pRow = new ui::HBox(pWindow);
    pRow->SetAttribute(_T("mouse_enabled"), _T("false"));
    pRow->SetAttribute(_T("padding"), _T("10,5,10,5"));
    AddItem(pRow);

    m_pImageControl = new ui::Control(pWindow);
    m_pImageControl->SetAttribute(_T("name"), _T("control_img"));
    m_pImageControl->SetAttribute(_T("margin"), _T("0,0,10,0"));
    m_pImageControl->SetAttribute(_T("mouse_enabled"), _T("false"));
    pRow->AddItem(m_pImageControl);

    ui::VBox* pCol = new ui::VBox(pWindow);
    pCol->SetAttribute(_T("margin"), _T("0,3,0,5"));
    pCol->SetAttribute(_T("mouse_enabled"), _T("false"));

    ui::HBox* pTitleRow = new ui::HBox(pWindow);
    pTitleRow->SetAttribute(_T("height"), _T("auto"));
    pTitleRow->SetAttribute(_T("mouse_enabled"), _T("false"));

    m_pTitleLabel = new ui::Label(pWindow);
    m_pTitleLabel->SetAttribute(_T("name"), _T("label_title"));
    m_pTitleLabel->SetAttribute(_T("width"), _T("stretch"));
    m_pTitleLabel->SetAttribute(_T("font"), _T("system_bold_14"));
    m_pTitleLabel->SetAttribute(_T("mouse_enabled"), _T("false"));
    pTitleRow->AddItem(m_pTitleLabel);

    m_pDelBtn = new ui::Button(pWindow);
    m_pDelBtn->SetAttribute(_T("name"), _T("btn_del"));
    m_pDelBtn->SetAttribute(_T("text"), _T("X"));
    m_pDelBtn->SetAttribute(_T("font"), _T("system_bold_12"));
    m_pDelBtn->SetAttribute(_T("text_align"), _T("hcenter,vcenter"));
    m_pDelBtn->SetAttribute(_T("textcolor"), _T("#FFD94A4A"));
    m_pDelBtn->SetAttribute(_T("hottextcolor"), _T("#FFFF0000"));
    m_pDelBtn->SetAttribute(_T("width"), _T("24"));
    m_pDelBtn->SetAttribute(_T("height"), _T("24"));
    m_pDelBtn->SetAttribute(_T("tooltip_text"), _T("delete"));
    pTitleRow->AddItem(m_pDelBtn);

    pCol->AddItem(pTitleRow);

    ui::Control* pSpacer = new ui::Control(pWindow);
    pSpacer->SetAttribute(_T("height"), _T("stretch"));
    pSpacer->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCol->AddItem(pSpacer);

    m_pProgressControl = new ui::Progress(pWindow);
    m_pProgressControl->SetAttribute(_T("name"), _T("progress"));
    m_pProgressControl->SetAttribute(_T("class"), _T("progress_horizontal_blue"));
    m_pProgressControl->SetAttribute(_T("mouse_enabled"), _T("false"));
    pCol->AddItem(m_pProgressControl);

    pRow->AddItem(pCol);
}

Item::~Item() = default;

void Item::InitSubControls(const DString& img, const DString& title, int nDataIndex)
{
    m_nDataIndex = nDataIndex;

    // Image
    m_pImageControl->SetFixedWidth(ui::UiFixedInt(48), true, true);
    m_pImageControl->SetFixedHeight(ui::UiFixedInt(48), true, true);
    static DString iconPath = DecodeIcon();
    if (!iconPath.empty()) {
        m_pImageControl->SetBkImage(_T("file='") + iconPath + _T("'"));
    }

    // Progress
    int64_t t = std::chrono::steady_clock::now().time_since_epoch().count() / 1000;
    m_pProgressControl->SetValue((double)(t % 100));

    // Title
    m_pTitleLabel->SetText(ui::StringUtil::Printf(_T("%s %d%%"), title.c_str(), (int)(t % 100)));

    // Delete button
    m_pDelBtn->AttachClick(ui::UiBind(&Item::OnRemove, this, std::placeholders::_1));
}

bool Item::OnRemove(const ui::EventArgs& /*args*/)
{
    SetVisible(false);
    if (m_pTitleLabel) m_pTitleLabel->SetText(_T("[deleted]"));
    return true;
}
