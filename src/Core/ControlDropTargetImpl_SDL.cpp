#include "dui/Core/ControlDropTargetImpl_SDL.h"

#ifdef DUI_BUILD_FOR_SDL

#include "dui/Core/Control.h"
#include "dui/Core/ControlDropTargetUtils.h"

namespace ui 
{
ControlDropTargetImpl_SDL::ControlDropTargetImpl_SDL(Control* pControl) :
    m_pControl(pControl)
{
}

ControlDropTargetImpl_SDL::~ControlDropTargetImpl_SDL()
{
}

int32_t ControlDropTargetImpl_SDL::OnDropBegin(const UiPoint& pt)
{
    if (m_pControl != nullptr) {
        ControlDropData_SDL data;
        data.m_bHandled = false;
        data.m_ptClientX = pt.x;
        data.m_ptClientY = pt.y;
        data.m_bTextData = false;

        EventArgs msg;
        msg.SetSender(m_pControl.get());
        msg.eventType = EventType::kEventDropEnter;
        msg.vkCode = VirtualKeyCode::kVK_None;
        msg.wParam = kControlDropTypeSDL;
        msg.lParam = (LPARAM)&data;
        msg.ptMouse = pt;
        msg.modifierKey = 0;
        msg.eventData = 0;

        m_pControl->SendEventMsg(msg);
    }
    return S_OK;
}

void ControlDropTargetImpl_SDL::OnDropPosition(const UiPoint& pt)
{
    if (m_pControl != nullptr) {
        ControlDropData_SDL data;
        data.m_bHandled = false;
        data.m_ptClientX = pt.x;
        data.m_ptClientY = pt.y;
        data.m_bTextData = false;

        EventArgs msg;
        msg.SetSender(m_pControl.get());
        msg.eventType = EventType::kEventDropOver;
        msg.vkCode = VirtualKeyCode::kVK_None;
        msg.wParam = kControlDropTypeSDL;
        msg.lParam = (LPARAM)&data;
        msg.ptMouse = pt;
        msg.modifierKey = 0;
        msg.eventData = 0;

        m_pControl->SendEventMsg(msg);
    }
}

void ControlDropTargetImpl_SDL::OnDropTexts(const std::vector<DString>& textList, const UiPoint& pt)
{
    if (m_pControl != nullptr) {
        ControlDropData_SDL data;
        data.m_bHandled = false;
        data.m_ptClientX = pt.x;
        data.m_ptClientY = pt.y;
        data.m_bTextData = true;
        data.m_textList = textList;

        EventArgs msg;
        msg.SetSender(m_pControl.get());
        msg.eventType = EventType::kEventDropData;
        msg.vkCode = VirtualKeyCode::kVK_None;
        msg.wParam = kControlDropTypeSDL;
        msg.lParam = (LPARAM)&data;
        msg.ptMouse = pt;
        msg.modifierKey = 0;
        msg.eventData = 0;

        m_pControl->SendEventMsg(msg);
    }
}

void ControlDropTargetImpl_SDL::OnDropFiles(const DString& source, const std::vector<DString>& fileList, const UiPoint& pt)
{
    if (m_pControl != nullptr) {
        if (!fileList.empty()) {
            // Currently executing a file drag and drop operation
            if (!m_pControl->IsEnableDropFile()) {
                // File drag and drop is not supported
                return;
            }
            // File drag and drop is supported; check whether the filter conditions are met
            DString fileTypes = m_pControl->GetDropFileTypes();
            if (!ControlDropTargetUtils::IsFilteredFileTypes(fileTypes, fileList)) {
                // The file types do not meet the filter conditions
                return;
            }
        }

        ControlDropData_SDL data;
        data.m_bHandled = false;
        data.m_ptClientX = pt.x;
        data.m_ptClientY = pt.y;
        data.m_bTextData = false;
        data.m_source = source;
        data.m_fileList = fileList;

        EventArgs msg;
        msg.SetSender(m_pControl.get());
        msg.eventType = EventType::kEventDropData;
        msg.vkCode = VirtualKeyCode::kVK_None;
        msg.wParam = kControlDropTypeSDL;
        msg.lParam = (LPARAM)&data;
        msg.ptMouse = pt;
        msg.modifierKey = 0;
        msg.eventData = 0;

        m_pControl->SendEventMsg(msg);
    }
}

void ControlDropTargetImpl_SDL::OnDropLeave()
{
    if (m_pControl != nullptr) {
        m_pControl->SendEvent(EventType::kEventDropLeave);
    }
}

} // namespace ui

#endif //DUI_BUILD_FOR_SDL
