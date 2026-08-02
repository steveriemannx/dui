#include "duilib/Core/Window.h"
#include "duilib/Core/Control.h"
#include "duilib/Core/Box.h"
#include "duilib/Core/FullscreenBox.h"
#include "duilib/Core/Shadow.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/ToolTip.h"
#include "duilib/Core/Keyboard.h"
#include "duilib/Core/WindowMessage.h"
#include "duilib/Render/IRender.h"
#include "duilib/Render/AutoClip.h"
#include "duilib/Utils/PerformanceUtil.h"
#include "duilib/Utils/FilePathUtil.h"
#include "duilib/Utils/AttributeUtil.h"

namespace ui
{
Window::Window() :
    m_pRoot(nullptr),
    m_pFocus(nullptr),
    m_pEventHover(nullptr),
    m_pEventToolTip(nullptr),
    m_pEventClick(nullptr),
    m_pEventKey(nullptr),
    m_rcAlphaFix(0, 0, 0, 0),
    m_bFirstLayout(false),
    m_bInitLayout(false),
    m_bIsArranged(false),
    m_bPostQuitMsgWhenClosed(false),
    m_renderBackendType(RenderBackendType::kRaster_BackendType),
    m_bWindowAttributesApplied(false),
    m_bCheckSetWindowFocus(false),
    m_bControlFullscreen(false)
{
    m_toolTip = std::make_unique<ToolTip>();
}

Window::~Window()
{
    ASSERT(!IsWindow());
    ClearWindow();
}

void Window::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("shadow_type")) {
        //Sets the shadow type of the window
        Shadow::ShadowType nShadowType = Shadow::ShadowType::kShadowCount;
        if (Shadow::GetShadowType(strValue, nShadowType)) {
            SetShadowType(nShadowType);
        }
    }
    else if (strName == _T("shadow_attached")) {
        //Whether the shadow is enabled
        SetShadowAttached(strValue == _T("true"));
    }
    else if (strName == _T("drag_drop")) {
        //Whether drag-and-drop is allowed
        SetEnableDragDrop(strValue == _T("true"));
    }
    else if (strName == _T("layered_window")) {
        //Whether it is a layered window
        SetLayeredWindow(strValue == _T("true"), true);
    }
    else if (strName == _T("layered_window_alpha")) {
        //The opacity of the layered window
        SetLayeredWindowAlpha(StringUtil::StringToInt32(strValue));
    }
}

void Window::SetEnableDragDrop(bool bEnable)
{
    NativeWnd()->SetEnableDragDrop(bEnable);
}

bool Window::IsEnableDragDrop() const
{
    return NativeWnd()->IsEnableDragDrop();
}

void Window::SetClass(const DString& strClass)
{
    if (strClass.empty()) {
        return;
    }
    std::list<DString> splitList = StringUtil::Split(strClass, _T(" "));
    for (auto it = splitList.begin(); it != splitList.end(); it++) {
        DString pDefaultAttributes = GlobalManager::Instance().GetClassAttributes((*it));
        if (pDefaultAttributes.empty()) {
            pDefaultAttributes = GetClassAttributes(*it);
        }

        ASSERT(!pDefaultAttributes.empty());
        if (!pDefaultAttributes.empty()) {
            ApplyAttributeList(pDefaultAttributes);
        }
    }
}

void Window::ApplyAttributeList(const DString& strList)
{
    //The attribute list is parsed first, then applied
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

Window* Window::GetParentWindow() const
{
    WindowBase* pWindowBase = WindowBase::GetParentWindow();
    if (pWindowBase != nullptr) {
        return dynamic_cast<Window*>(pWindowBase);
    }
    else {
        return nullptr;
    }
}

bool Window::SetRenderBackendType(RenderBackendType backendType)
{
#if defined (DUILIB_BUILD_FOR_WIN) && !defined (DUILIB_BUILD_FOR_SDL)
    m_renderBackendType = backendType;
#else
    backendType = RenderBackendType::kRaster_BackendType;
    m_renderBackendType = backendType;
#endif
    ASSERT(IsWindow());
    if (!IsWindow()) {
        return false;
    }
    bool bRet = false;
    if (m_render == nullptr) {
        //Initialize on the first call
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        ASSERT(pRenderFactory != nullptr);
        if (pRenderFactory != nullptr) {
            m_render.reset(pRenderFactory->CreateRender(GetRenderDpi(), GetWindowHandle(), m_renderBackendType));
            bRet = (m_render != nullptr) ? true : false;
        }
    }
    else {
        ASSERT(m_render->GetRenderBackendType() == backendType);
        bRet = (m_render->GetRenderBackendType() == backendType) ? true : false;
    }
    ASSERT(bRet);
    return bRet;
}

RenderBackendType Window::GetRenderBackendType() const
{
    RenderBackendType backendType = m_renderBackendType;
    if (m_render != nullptr) {
        backendType = m_render->GetRenderBackendType();
    }
    return backendType;
}

bool Window::SetWindowIcon(const DString& iconFilePath)
{
    if (iconFilePath.empty()) {
        return false;
    }
    bool bRet = false;
    FilePath iconFullPath = GlobalManager::Instance().GetExistsResFullPath(GetResourcePath(), GetXmlPath(), FilePath(iconFilePath));
    ASSERT(!iconFullPath.IsEmpty());
    if (iconFullPath.IsEmpty()) {
        return false;
    }
    if (GlobalManager::Instance().Zip().IsUseZip() &&
        GlobalManager::Instance().Zip().IsZipResExist(iconFullPath)) {
        //Use the compressed package
        std::vector<uint8_t> fileData;
        GlobalManager::Instance().Zip().GetZipData(iconFullPath, fileData);
        ASSERT(!fileData.empty());
        if (!fileData.empty()) {
            bRet = WindowBase::SetWindowIcon(fileData, iconFilePath);
        }
    }
    else {
        //Use local files
        if (iconFullPath.IsExistsFile()) {
            bRet = WindowBase::SetWindowIcon(iconFullPath);
        }
        else {
            ASSERT(false);
        }
    }
    return bRet;
}

void Window::InitSkin(const DString& skinFolder, const DString& skinFile)
{
    m_skinFolder = skinFolder;
    m_skinFile = skinFile;
    m_windowBuilder.reset();
}

DString Window::GetSkinFolder()
{
    return m_skinFolder;
}

DString Window::GetSkinFile()
{
    return m_skinFile;
}

Control* Window::CreateControl(const DString& /*strClass*/)
{
    return nullptr;
}

void Window::GetCreateWindowAttributes(WindowCreateAttributes& createAttributes)
{
    //Parses the XML file associated with the window
    if (m_windowBuilder == nullptr) {
        ParseWindowXml();
    }

    //Parses the window attributes
    if (m_windowBuilder != nullptr) {
        m_windowBuilder->ParseWindowCreateAttributes(createAttributes);
    }
}

void Window::ParseWindowXml()
{
    FilePath skinFolder(GetSkinFolder());
    DString xmlFile = GetSkinFile();
    if (skinFolder.IsEmpty() && xmlFile.empty()) {
        return;
    }

    //Absolute paths are not supported for resource paths
    ASSERT(!skinFolder.IsAbsolutePath());
    if (skinFolder.IsAbsolutePath()) {
        return;
    }

    //Saves the resource path
    SetResourcePath(skinFolder);
    SetXmlPath(FilePath());

    //The path of the XML file; it should be a relative path    
    DString skinXmlFileData;
    FilePath skinXmlFilePath;
    if (!xmlFile.empty() && xmlFile.front() == _T('<')) {
        //The returned content is the XML file content, not a file path
        skinXmlFileData = std::move(xmlFile);
    }
    else {
        const FilePath xmlFilePath(xmlFile);
        ASSERT(!xmlFilePath.IsAbsolutePath());
        if (xmlFilePath.IsAbsolutePath()) {
            return;
        }

        //Saves the path of the XML file
        size_t nPos = xmlFile.find_last_of(_T("/\\"));
        if (nPos != DString::npos) {
            DString xmlPath = xmlFile.substr(0, nPos);
            if (!xmlPath.empty()) {
                SetXmlPath(FilePath(xmlPath));
            }
        }
        skinXmlFilePath = xmlFilePath;
    }
    //Parses the XML file
    bool bRet = false;
    m_windowBuilder = std::make_unique<WindowBuilder>();
    if (!skinXmlFileData.empty()) {
        bRet = m_windowBuilder->ParseXmlData(skinXmlFileData);
    }
    else {
        ASSERT(!skinXmlFilePath.IsEmpty());
        bRet = m_windowBuilder->ParseXmlFile(skinXmlFilePath, GetResourcePath());
    }
    if (!bRet) {
        m_windowBuilder.reset();
        SetResourcePath(FilePath());
        SetXmlPath(FilePath());
    }
}

void Window::PreInitWindow()
{
    if (!IsWindow()) {
        return;
    }
    //Reinitialize the shadow attachment value based on whether the window is a layered window (true for a layered window, otherwise false)
    ASSERT(m_shadow == nullptr);
    if (m_shadow != nullptr) {
        return;
    }

    //Creates the window shadow
    m_shadow = std::make_unique<Shadow>(this);
    if (m_shadow->IsUseDefaultShadowAttached()) {
        m_shadow->SetShadowAttached(IsLayeredWindow());
        m_shadow->SetUseDefaultShadowAttached(true);
    }

    //Adds to the global manager
    GlobalManager::Instance().Windows().AddWindow(this);

    //Parses the XML file associated with the window
    if (m_windowBuilder == nullptr) {
        ParseWindowXml();
    }

    Box* pRoot = nullptr;
    if (m_windowBuilder != nullptr) {
        auto callback = UiBind(&Window::CreateControl, this, std::placeholders::_1);
        Control* pControl = m_windowBuilder->CreateControls(this, callback);
        pRoot = m_windowBuilder->ToBox(pControl);
        ASSERT(pRoot != nullptr);
    }

    if (pRoot != nullptr) {
        //Associates the window shadow
        pRoot = AttachShadow(pRoot);

        if (IsUseSystemCaption()) {
            //When using the system title bar, disable the shadow
            SetShadowAttached(false);
        }

        //Associates the Root object
        AttachBox(pRoot);

        //Updates the custom-drawn title bar state
        OnUseSystemCaptionBarChanged();
    }
}

void Window::PostInitWindow()
{
    //Creates the rendering interface
    if (m_render == nullptr) {
        IRenderFactory* pRenderFactory = GlobalManager::Instance().GetRenderFactory();
        ASSERT(pRenderFactory != nullptr);
        if (pRenderFactory != nullptr) {
            m_render.reset(pRenderFactory->CreateRender(GetRenderDpi(), GetWindowHandle(), m_renderBackendType));
        }
    }
    ASSERT(m_render != nullptr);

    //Initialize the window size according to the size attribute in the XML
    if ((m_szInitSize.cx > 0) && (m_szInitSize.cy > 0)) {
        Resize(m_szInitSize.cx, m_szInitSize.cy, true, false);
    }

    //Check whether the window size needs to be set according to the auto type of the root node (e.g., menus use this)
    if (AutoResizeWindow(false)) {
        //After resizing, the window needs to be centered again
        if (NativeWnd()->NeedCenterWindowAfterCreated()) {
            CenterWindow();
        }
    }

    //After creation, the Render size stays in sync with the client area size
    ResizeRenderToClientSize();
}

void Window::PreCloseWindow()
{
    ClearStatus();
    OnPreCloseWindow();

    //Destroys the ToolTip window
    if (m_toolTip != nullptr) {
        m_toolTip->DestroyToolTip();
    }
}

void Window::PostCloseWindow()
{
    if (m_bPostQuitMsgWhenClosed) {
        this->PostQuitMsg(0);
    }
    OnCloseWindow();
}

void Window::FinalMessage()
{
    //Cleans up resources in reverse order
    ClearWindow();
    ClearWindowBase();

    //Calls back the Final interface
    OnFinalMessage();
}

void Window::OnFinalMessage()
{
    if (!IsDoModal()) {
        delete this;
    }
}

void Window::ClearWindow()
{
    //Recycles the control
    GlobalManager::Instance().Windows().RemoveWindow(this);
    ReapObjects(GetRoot());

    m_controlFinder.Clear();
    m_toolTip.reset();
    m_shadow.reset();
    m_render.reset();

    Box* pRoot = m_pRoot.get();
    m_pRoot.reset();
    if (pRoot != nullptr) {
        delete pRoot;
        pRoot = nullptr;
    }

    RemoveAllClass();
    RemoveAllOptionGroups();
}

bool Window::AttachBox(Box* pRoot)
{
    // Pure-code windows (no layout XML) build the control tree in the derived
    // class's OnInitWindow, so PreInitWindow could not associate the shadow
    // (there was no root control yet). Associate it on the first AttachBox;
    // Shadow::AttachShadow is idempotent and returns the shadow-wrapped root.
    if ((m_shadow != nullptr) && (m_shadow->GetShadowBox() == nullptr)) {
        pRoot = AttachShadow(pRoot);
    }
    ASSERT(IsWindow());
    SetFocusControl(nullptr); //Sets the state related to m_pFocus
    m_pEventKey = nullptr;
    m_pEventHover = nullptr;
    m_pEventToolTip = nullptr;
    m_pEventClick = nullptr;
    // Remove the existing control-tree. We might have gotten inside this function as
    // a result of an event fired or similar, so we cannot just delete the objects and
    // pull the internal memory of the calling code. We'll delay the cleanup.
    if ((m_pRoot != nullptr) && (pRoot != m_pRoot)) {
        Box* pOldRoot = m_pRoot.get();
        m_pRoot.reset();
        if (pOldRoot != nullptr) {
            delete pOldRoot;
            pOldRoot = nullptr;
        }
    }
    // Set the dialog root element
    m_pRoot = pRoot;
    m_controlFinder.SetRoot(pRoot);
    // Go ahead...
    m_bIsArranged = true;
    m_bFirstLayout = false;
    m_bInitLayout = false;
    // Initiate all control
    return InitControls(m_pRoot.get());
}

Box* Window::GetRoot() const
{
    return m_pRoot.get();
}

Box* Window::GetXmlRoot() const
{
    Box* pXmlRoot = nullptr;
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pXmlRoot = pShadow->GetAttachedXmlRoot();
    }
    if (pXmlRoot == nullptr) {
        pXmlRoot = m_pRoot.get();
    }
    return pXmlRoot;
}

bool Window::InitControls(Control* pControl)
{
    ASSERT(pControl != nullptr);
    if (pControl == nullptr) {
        return false;
    }
    m_controlFinder.AddControl(pControl);
    pControl->SetWindow(this);
    return true;
}

void Window::ReapObjects(Control* pControl)
{
    if (pControl == nullptr) {
        return;
    }
    if (pControl == m_pEventKey) {
        m_pEventKey = nullptr;
    }
    if (pControl == m_pEventHover) {
        m_pEventHover = nullptr;
    }
    if (pControl == m_pEventToolTip) {
        m_pEventToolTip = nullptr;
    }    
    if (pControl == m_pEventClick) {
        m_pEventClick = nullptr;
    }
    bool bFocusChanged = false;
    if (pControl == m_pFocus) {
        bFocusChanged = (m_pFocus != nullptr) ? true : false;
        m_pFocus = nullptr;        
    }
    if (!IsClosingWnd()) {
        m_controlFinder.RemoveControl(pControl);
        if (bFocusChanged) {
            OnFocusControlChanged();
        }
    }    
}

void Window::SetResourcePath(const FilePath& strPath)
{
    m_resourcePath = strPath;
    m_resourcePath.NormalizeDirectoryPath();
}

const FilePath& Window::GetResourcePath() const
{
    return m_resourcePath;
}

void Window::SetXmlPath(const FilePath& xmlPath)
{
    m_xmlPath = xmlPath;
    m_xmlPath.NormalizeDirectoryPath();
}

const FilePath& Window::GetXmlPath() const
{
    return m_xmlPath;
}

void Window::AddClass(const DString& strClassName, const DString& strControlAttrList)
{
    ASSERT(!strClassName.empty());
    ASSERT(!strControlAttrList.empty());
#ifdef _DEBUG
    //Check: avoid accidental modification
    auto iter = m_defaultAttrHash.find(strClassName);
    if (iter != m_defaultAttrHash.end()) {
        ASSERT(iter->second == strControlAttrList);
    }
#endif
    m_defaultAttrHash[strClassName] = strControlAttrList;
}

DString Window::GetClassAttributes(const DString& strClassName) const
{
    auto it = m_defaultAttrHash.find(strClassName);
    if (it != m_defaultAttrHash.end()) {
        return it->second;
    }
    return _T("");
}

bool Window::RemoveClass(const DString& strClassName)
{
    auto it = m_defaultAttrHash.find(strClassName);
    if (it != m_defaultAttrHash.end()) {
        m_defaultAttrHash.erase(it);
        return true;
    }
    return false;
}

void Window::RemoveAllClass()
{
    m_defaultAttrHash.clear();
}

void Window::AddTextColor(const DString& strName, const DString& strValue)
{
    m_colorMap.AddColor(strName, strValue);
}

void Window::AddTextColor(const DString& strName, UiColor argb)
{
    m_colorMap.AddColor(strName, argb);
}

UiColor Window::GetTextColor(const DString& strName) const
{
    return m_colorMap.GetColor(strName);
}

void Window::RemoveTextColor(const DString& strName)
{
    m_colorMap.RemoveColor(strName);
}

bool Window::AddOptionGroup(const DString& strGroupName, Control* pControl)
{
    ASSERT(!strGroupName.empty());
    ASSERT(pControl != nullptr);
    if ((pControl == nullptr) || strGroupName.empty()) {
        return false;
    }
    auto it = m_mOptionGroup.find(strGroupName);
    if (it != m_mOptionGroup.end()) {
        auto it2 = std::find(it->second.begin(), it->second.end(), pControl);
        if (it2 != it->second.end()) {
            return false;
        }
        it->second.push_back(pControl);
    }
    else {
        m_mOptionGroup[strGroupName].push_back(pControl);
    }
    return true;
}

std::vector<Control*>* Window::GetOptionGroup(const DString& strGroupName)
{
    auto it = m_mOptionGroup.find(strGroupName);
    if (it != m_mOptionGroup.end()) {
        return &(it->second);
    }
    return nullptr;
}

void Window::RemoveOptionGroup(const DString& strGroupName, Control* pControl)
{
    ASSERT(!strGroupName.empty());
    ASSERT(pControl != nullptr);
    auto it = m_mOptionGroup.find(strGroupName);
    if (it != m_mOptionGroup.end()) {
        auto it2 = std::find(it->second.begin(), it->second.end(), pControl);
        if (it2 != it->second.end()) {
            it->second.erase(it2);
        }

        if (it->second.empty()) {
            m_mOptionGroup.erase(it);
        }
    }
}

void Window::RemoveAllOptionGroups()
{
    m_mOptionGroup.clear();
}

bool Window::IsKeyDown(const EventArgs& msg, ModifierKey modifierKey) const
{
    switch (msg.eventType) {
    case kEventChar:
        if (modifierKey == ModifierKey::kFirstPress) {
            return msg.modifierKey & ModifierKey::kFirstPress;
        }
        else if (modifierKey == ModifierKey::kAlt) {
            return msg.modifierKey & ModifierKey::kAlt;
        }
        else if (modifierKey == ModifierKey::kIsSystemKey) {
            return msg.modifierKey & ModifierKey::kIsSystemKey;
        }
        break;

    case kEventKeyDown:
        if (modifierKey == ModifierKey::kFirstPress) {
            return msg.modifierKey & ModifierKey::kFirstPress;
        }
        else if (modifierKey == ModifierKey::kAlt) {
            return msg.modifierKey & ModifierKey::kAlt;
        }
        else if (modifierKey == ModifierKey::kIsSystemKey) {
            return msg.modifierKey & ModifierKey::kIsSystemKey;
        }
        break;

    case kEventKeyUp:
        if (modifierKey == ModifierKey::kAlt) {
            return msg.modifierKey & ModifierKey::kAlt;
        }
        else if (modifierKey == ModifierKey::kIsSystemKey) {
            return msg.modifierKey & ModifierKey::kIsSystemKey;
        }
        break;

    case kEventMouseWheel:
    {
        if (modifierKey == ModifierKey::kControl) {
            return msg.modifierKey & ModifierKey::kControl;
        }
        else if (modifierKey == ModifierKey::kShift) {
            return msg.modifierKey & ModifierKey::kShift;
        }
        break;
    }
    break;
    case kEventMouseHover:
    case kEventMouseMove:
    case kEventMouseButtonDown:
    case kEventMouseButtonUp:
    case kEventMouseDoubleClick:
    case kEventMouseRButtonDown:
    case kEventMouseRButtonUp:
    case kEventMouseRDoubleClick:
    case kEventMouseMButtonDown:
    case kEventMouseMButtonUp:
    case kEventMouseMDoubleClick:
        if (modifierKey == ModifierKey::kControl) {
            return msg.modifierKey & ModifierKey::kControl;
        }
        else if (modifierKey == ModifierKey::kShift) {
            return msg.modifierKey & ModifierKey::kShift;
        }
        break;
    default:
        break;
    }
    //By default, obtained from the keyboard state
    if (modifierKey == ModifierKey::kControl) {
        return Keyboard::IsKeyDown(kVK_CONTROL);
    }
    else if (modifierKey == ModifierKey::kShift) {
        return Keyboard::IsKeyDown(kVK_SHIFT);
    }
    else if (modifierKey == ModifierKey::kAlt) {
        return Keyboard::IsKeyDown(kVK_MENU);
    }
    else if (modifierKey == ModifierKey::kWin) {
        return Keyboard::IsKeyDown(kVK_LWIN) || Keyboard::IsKeyDown(kVK_RWIN);
    }
    return false;
}

void Window::ClearImageCache()
{
    Control* pRoot = nullptr;
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pRoot = pShadow->GetShadowBox();
    }
    if (pRoot) {
        pRoot->ClearImageCache();
    }
    else if (m_pRoot != nullptr) {
        m_pRoot->ClearImageCache();
    }
}

void Window::OnUseSystemCaptionBarChanged()
{
    if (IsUseSystemCaption()) {
        //Disables the shadow
        SetShadowAttached(false);
    }
}

void Window::OnLayeredWindowChanged()
{
    //Reinitialize the shadow attachment value based on whether the window is a layered window (true for a layered window, otherwise false)
    Shadow* pShadow = GetShadow();
    if ((pShadow != nullptr) && pShadow->IsUseDefaultShadowAttached()) {
        pShadow->SetShadowAttached(IsLayeredWindow());
        pShadow->SetUseDefaultShadowAttached(true);
    }
    InvalidateAll();
}

void Window::InvalidateAll()
{
    UiRect rcClient;
    GetClientRect(rcClient);
    Invalidate(rcClient);
}

void Window::OnWindowAlphaChanged()
{
    InvalidateAll();
}

void Window::OnWindowEnterFullscreen()
{
}

void Window::OnWindowExitFullscreen()
{
}

void Window::OnWindowDisplayScaleChanged(uint32_t /*nOldScaleFactor*/, uint32_t /*nNewScaleFactor*/)
{
}

void Window::GetShadowCorner(UiPadding& rcShadow) const
{
    rcShadow.Clear();
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        rcShadow = pShadow->GetShadowCorner();
    }
}

void Window::GetCurrentShadowCorner(UiPadding& rcShadow) const
{
    rcShadow.Clear();
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        rcShadow = pShadow->GetCurrentShadowCorner();
    }
}

bool Window::IsPtInCaptionBarControl(const UiPoint& pt) const
{
    Control* pControl = FindControl(pt);
    if (pControl) {
        if (pControl->CanPlaceCaptionBar()) {
            return true;
        }
    }
    return false;
}

bool Window::HasMinMaxBox(bool& /*bMinimizeBox*/, bool& /*bMaximizeBox*/) const
{
    return false;
}

bool Window::IsPtInMaximizeRestoreButton(const UiPoint& /*pt*/) const
{
    return false;
}

const UiRect& Window::GetAlphaFixCorner() const
{
    return m_rcAlphaFix;
}

void Window::SetAlphaFixCorner(const UiRect& rc, bool bNeedDpiScale)
{
    ASSERT((rc.left >= 0) && (rc.top >= 0) && (rc.right >= 0) && (rc.bottom >= 0));
    if ((rc.left >= 0) && (rc.top >= 0) && (rc.right >= 0) && (rc.bottom >= 0)) {
        m_rcAlphaFix = rc;
        if (bNeedDpiScale) {
            Dpi().ScaleRect(m_rcAlphaFix);
        }
    }
}

Box* Window::AttachShadow(Box* pRoot)
{
    //Attaches the shadow to the window
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        return pShadow->AttachShadow(pRoot);
    }
    else {
        return pRoot;
    }
}

void Window::SetShadowAttached(bool bShadowAttached)
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->SetShadowAttached(bShadowAttached);
        OnWindowShadowTypeChanged();
    }
}

void Window::SetShadowType(Shadow::ShadowType nShadowType)
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->SetShadowType(nShadowType);
        //Repaint the window, otherwise drawing glitches occur
        InvalidateAll();
        OnWindowShadowTypeChanged();
    }
}

Shadow::ShadowType Window::GetShadowType() const
{
    Shadow::ShadowType nShadowType = Shadow::ShadowType::kShadowDefault;
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        nShadowType = pShadow->GetShadowType();
    }
    return nShadowType;
}

DString Window::GetShadowImage() const
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        return pShadow->GetShadowImage();
    }
    else {
        return DString();
    }
}

void Window::SetShadowImage(const DString& shadowImage)
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->SetShadowImage(shadowImage);
    }
}

void Window::SetShadowBorderSize(int32_t nShadowBorderSize)
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->SetShadowBorderSize(nShadowBorderSize);
    }
}

int32_t Window::GetShadowBorderSize() const
{
    Shadow* pShadow = GetShadow();
    int32_t nShadowBorderSize = 0;    
    if (pShadow != nullptr) {
        nShadowBorderSize = pShadow->GetShadowBorderSize();
    }
    return nShadowBorderSize;
}

void Window::SetShadowBorderColor(const DString& shadowBorderColor)
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->SetShadowBorderColor(shadowBorderColor);
    }
}

DString Window::GetShadowBorderColor() const
{
    Shadow* pShadow = GetShadow();
    DString shadowBorderColor;
    if (pShadow != nullptr) {
        shadowBorderColor = pShadow->GetShadowBorderColor();
    }
    return shadowBorderColor;
}

UiPadding Window::GetCurrentShadowCorner() const
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        return pShadow->GetCurrentShadowCorner();
    }
    else {
        return UiPadding();
    }
}

bool Window::IsShadowAttached() const
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        return pShadow->IsShadowAttached();
    }
    else {
        return false;
    }
}

bool Window::IsUseDefaultShadowAttached() const
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        return pShadow->IsUseDefaultShadowAttached();
    } 
    else {
        return false;
    }    
}

void Window::SetUseDefaultShadowAttached(bool bDefault)
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->SetUseDefaultShadowAttached(bDefault);
    }
}

UiPadding Window::GetShadowCorner() const
{
    UiPadding rcShadowCorner;
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        rcShadowCorner = pShadow->GetShadowCorner();
    }
    return rcShadowCorner;
}

void Window::SetShadowCorner(const UiPadding& rcShadowCorner)
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->SetShadowCorner(rcShadowCorner);
    }
}

void Window::SetShadowBorderRound(UiSize szBorderRound)
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->SetShadowBorderRound(szBorderRound);
    }
}

void Window::SetEnableShadowSnap(bool bEnable)
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->SetEnableShadowSnap(bEnable);
    }
}

bool Window::IsEnableShadowSnap() const
{
    bool bRet = false;
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        bRet = pShadow->IsEnableShadowSnap();
    }
    return bRet;
}

UiSize Window::GetShadowBorderRound() const
{
    UiSize szBorderRound;
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        szBorderRound = pShadow->GetShadowBorderRound();
    }
    return szBorderRound;
}

void Window::SetInitSize(int cx, int cy)
{
    ASSERT(IsWindow());
    if (GetRoot() == nullptr) {
        m_szInitSize.cx = cx;
        m_szInitSize.cy = cy;
    }
    else {
        Resize(cx, cy, true, false);
    }
}

void Window::OnDisplayScaleChanged(uint32_t nOldScaleFactor, uint32_t nNewScaleFactor)
{
    if ((nOldScaleFactor == nNewScaleFactor) || (nNewScaleFactor == 0)) {
        return;
    }
    if (!Dpi().CheckDisplayScaleFactor(nNewScaleFactor)) {
        return;
    }
    WindowBase::OnDisplayScaleChanged(nOldScaleFactor, nNewScaleFactor);

    //Window shadow
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->ChangeDpiScale(Dpi(), nOldScaleFactor, nNewScaleFactor);
    }

    //Updates the DPI-related attributes of the window itself
    m_rcAlphaFix = Dpi().GetScaleRect(m_rcAlphaFix, nOldScaleFactor);
    m_renderOffset = Dpi().GetScalePoint(m_renderOffset, nOldScaleFactor);

    //Updates the layout and the DPI-related attributes of controls
    SetArrange(true);

    Box* pRoot = GetRoot();
    if (pRoot != nullptr) {
        pRoot->ChangeDpiScale(nOldScaleFactor, nNewScaleFactor);
        pRoot->Arrange();
        Invalidate(pRoot->GetPos());
    }
}

LRESULT Window::OnWindowMessage(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, bool& bHandled)
{
    bHandled = false;
    return 0;
}

LRESULT Window::OnWindowPosChangedMsg(const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    if (m_pFocus != nullptr) {
        m_pFocus->SendEvent(kEventWindowPosChanged);
        if (windowFlag.expired()) {
            return 0;
        }
    }
    return 0;
}

LRESULT Window::OnSizeMsg(WindowSizeType sizeType, const UiSize& /*newWindowSize*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();

    //Resize the Render to match the client area size
    ResizeRenderToClientSize();

    Box* pRoot = GetRoot();
    if (pRoot != nullptr) {
        pRoot->Arrange();
    }
    if (sizeType == WindowSizeType::kSIZE_MAXIMIZED) {
        //Maximize
        ProcessWindowMaximized();        
    }
    else if (sizeType == WindowSizeType::kSIZE_RESTORED) {
        //Restore
        ProcessWindowRestored();
    }
    if (m_pFocus != nullptr) {        
        EventArgs msgData;
        msgData.eventData = (int32_t)sizeType;
        m_pFocus->SendEvent(kEventWindowSize, msgData);
        if (windowFlag.expired()) {
            return 0;
        }
    }
    return 0;
}

LRESULT Window::OnMoveMsg(const UiPoint& ptTopLeft, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    if (m_pFocus != nullptr) {        
        EventArgs msgData;
        msgData.ptMouse = ptTopLeft;
        m_pFocus->SendEvent(kEventWindowMove, msgData);
        if (windowFlag.expired()) {
            return 0;
        }
    }
    //If the window overflows the screen, the entire window must be drawn to prevent the content outside the window from failing to draw when it moves into the screen
    UiRect rcWindow;
    GetWindowRect(rcWindow);
    UiRect rcMonitor;
    GetMonitorWorkRect(rcMonitor);
    if ((rcWindow.left < rcMonitor.left) ||
        (rcWindow.top < rcMonitor.top) ||
        (rcWindow.right > rcMonitor.right) ||
        (rcWindow.bottom > rcMonitor.bottom)) {
        InvalidateAll();
    }
    return 0;
}

LRESULT Window::OnShowWindowMsg(bool bShow, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    OnShowWindow(bShow);
    return 0;
}

bool Window::ResizeRenderToClientSize() const
{
    bool bRet = false;
    UiRect rcClient;
    GetClientRect(rcClient);
    ASSERT(m_render != nullptr);
    if ((m_render != nullptr) && !rcClient.IsEmpty()) {
        if ((m_render->GetWidth() != rcClient.Width()) || (m_render->GetHeight() != rcClient.Height())) {
            bRet = m_render->Resize(rcClient.Width(), rcClient.Height());
            ASSERT(bRet && "Window::ResizeRenderToClientSize failed!");
        }
        else {
            bRet = true;
        }
    } 
    return bRet;
}

bool Window::OnPreparePaint()
{
    GlobalManager::Instance().AssertUIThread();
    if (!IsWindow()) {
        return false;
    }
    if (m_render == nullptr) {
        return false;
    }
    if (IsWindowMinimized() || (GetRoot() == nullptr)) {
        return false;
    }
    //Updates the state and creates the Render, etc.
    if (!PreparePaint(true)) {
        return false;
    }

    //Callback for completing the interface layout initialization (called before the first draw)
    if (!m_bInitLayout) {
        m_bInitLayout = true;
        OnInitLayout();

        //Further check whether the layout needs to be updated
        PreparePaint(true);
    }
    return true;
}

LRESULT Window::OnPaintMsg(const UiRect& rcPaint, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    PerformanceStat statPerformance(_T("PaintWindow, Window::OnPaintMsg"));
    bHandled = false;
    if (!IsWindowFirstShown()) {
        //On the first draw, draw the full area (to avoid the incomplete display when the initial window is partially off-screen and then dragged to the center of the screen)
        UiRect rc;
        GetClientRect(rc);
        bHandled = Paint(rc);
    }
    else {
        //On non-first draws, only paint the dirty region
        bHandled = Paint(rcPaint);
    }
    return 0;
}

bool Window::Paint(const UiRect& rcPaint)
{
    GlobalManager::Instance().AssertUIThread();
    IRender* pRender = GetRender();
    ASSERT(pRender != nullptr);
    if (pRender == nullptr) {
        return false;
    }

    //Before drawing, remove the alpha channel
    if (IsLayeredWindow()) {
        PerformanceStat statPerformance(_T("PaintWindow, Window::Paint ClearAlpha"));
        pRender->ClearAlpha(rcPaint);
    }

    // Draw
    Box* pRoot = GetRoot();
    if (pRoot == nullptr) {
        return false;
    }
    if (pRoot->IsVisible()) {
        PerformanceStat statPerformance(_T("PaintWindow, Window::Paint Paint/PaintChild"));
        AutoClip rectClip(pRender, rcPaint, true);
        UiPoint ptOldWindOrg = pRender->OffsetWindowOrg(m_renderOffset);
        pRoot->AlphaPaint(pRender, rcPaint);
        pRender->SetWindowOrg(ptOldWindOrg);
    }
    else {
        UiColor bkColor = UiColor(UiColors::LightGray);
        if (!pRoot->GetBkColor().empty()) {
            bkColor = pRoot->GetUiColor(pRoot->GetBkColor());
        }
        pRender->FillRect(UiRectF::MakeFromRect(rcPaint), bkColor);
    }

#if defined (DUILIB_BUILD_FOR_WIN) && !defined(DUILIB_RICH_EDIT_DRAW_OPT)
    //Before drawing, repair the alpha channel
    if (IsLayeredWindow()) {
        PerformanceStat statPerformance(_T("PaintWindow, Window::Paint RestoreAlpha"));
        Shadow* pShadow = GetShadow();
        if ((pShadow != nullptr) && pShadow->IsShadowAttached() &&
            (m_renderOffset.x == 0) && (m_renderOffset.y == 0)) {
            //Remedies the alpha channel being 0 caused by GDI drawing
            UiRect rcNewPaint = rcPaint;
            rcNewPaint.Intersect(pRoot->GetPosWithoutPadding());
            UiPadding rcRootPadding = pRoot->GetPadding();

            //Consider the corner radius
            rcRootPadding.left += 1;
            rcRootPadding.top += 1;
            rcRootPadding.right += 1;
            rcRootPadding.bottom += 1;
            pRender->RestoreAlpha(rcNewPaint, rcRootPadding);//Currently only the RichEdit drawing on Windows makes the window transparent, so this repair is needed
        }
        else {
            UiRect rcAlphaFixCorner = GetAlphaFixCorner();
            if ((rcAlphaFixCorner.left > 0) || (rcAlphaFixCorner.top > 0) ||
                (rcAlphaFixCorner.right > 0) || (rcAlphaFixCorner.bottom > 0)) {
                UiRect rcNewPaint = rcPaint;
                UiRect rcRootPaddingPos = pRoot->GetPosWithoutPadding();
                rcRootPaddingPos.Deflate(rcAlphaFixCorner.left, rcAlphaFixCorner.top,
                                         rcAlphaFixCorner.right, rcAlphaFixCorner.bottom);
                rcNewPaint.Intersect(rcRootPaddingPos);
                UiPadding rcRootPadding;
                pRender->RestoreAlpha(rcNewPaint, rcRootPadding);//Currently only the RichEdit drawing on Windows makes the window transparent, so this repair is needed
            }
        }
    }
#endif
    return true;
}

LRESULT Window::OnSetFocusMsg(WindowBase* /*pLostFocusWindow*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    //When gaining focus, if there is no focus control, disable the input method
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    ControlPtr pFocus = m_pFocus;
    if (pFocus != nullptr) {
        pFocus->SendEvent(kEventWindowSetFocus);

        //Re-activate the control focus (but do not restore the Hot state, to avoid abnormal display states of controls such as buttons)
        if (!windowFlag.expired() && (pFocus == m_pFocus)) {
            pFocus->SendEvent(kEventSetFocus);
        }
        if (!windowFlag.expired() && (pFocus == m_pFocus)) {
            UiPoint pt;
            GetCursorPos(pt);
            ScreenToClient(pt);
            if (pFocus->IsPointInWithScrollOffset(pt)) {
                //The mouse is still within the control range, keep the hot state
                pFocus->SetState(kControlStateHot);
            }
            else {
                //The mouse is no longer within the control range, restore the Normal state
                pFocus->SetState(kControlStateNormal);
            }
        }
    }
    else {
        NativeWnd()->SetImeOpenStatus(false);
    }
    return 0;
}

LRESULT Window::OnKillFocusMsg(WindowBase* /*pSetFocusWindow*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    ControlPtr pEventClick = m_pEventClick;
    if (!Keyboard::IsKeyDown(VirtualKeyCode::kVK_LBUTTON) &&
        !Keyboard::IsKeyDown(VirtualKeyCode::kVK_RBUTTON) &&
        !Keyboard::IsKeyDown(VirtualKeyCode::kVK_MBUTTON)) {
        //Clear only when no mouse button is pressed; otherwise it should be cleared when the mouse button is released, to avoid affecting the click function in the non-focus state
        m_pEventClick = nullptr;
    }

    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    if (pEventClick != nullptr) {
        pEventClick->SendEvent(kEventWindowKillFocus);
        if (windowFlag.expired()) {
            return 0;
        }
    }
    ControlPtr pFocus = m_pFocus;
    if ((pFocus != nullptr) && (pFocus != pEventClick)) {
        pFocus->SendEvent(kEventWindowKillFocus);
        if (windowFlag.expired()) {
            return 0;
        }
    }
    return 0;
}

LRESULT Window::OnImeSetContextMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    bHandled = false;
    if (m_pFocus != nullptr) {
        m_pFocus->SendEvent(kEventImeSetContext, nativeMsg.wParam, nativeMsg.lParam);
        if ((m_pFocus != nullptr) && m_pFocus->IsCefOsrImeMode()) {
            bHandled = true;
        }
    }
    return 0;
}

LRESULT Window::OnImeStartCompositionMsg(const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    if (m_pFocus != nullptr) {
        m_pFocus->SendEvent(kEventImeStartComposition);
        if ((m_pFocus != nullptr) && m_pFocus->IsCefOsrImeMode()) {
            bHandled = true;
        }
    }
    return 0;
}

LRESULT Window::OnImeCompositionMsg(const NativeMsg& nativeMsg, bool& bHandled)
{
    bHandled = false;
    if (m_pFocus != nullptr) {
        m_pFocus->SendEvent(kEventImeComposition, nativeMsg.wParam, nativeMsg.lParam);
        if ((m_pFocus != nullptr) && m_pFocus->IsCefOsrImeMode()) {
            bHandled = true;
        }
    }
    return 0;
}

LRESULT Window::OnImeEndCompositionMsg(const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    if (m_pFocus != nullptr) {
        m_pFocus->SendEvent(kEventImeEndComposition);
        if ((m_pFocus != nullptr) && m_pFocus->IsCefOsrImeMode()) {
            bHandled = true;
        }
    }
    return 0;
}

LRESULT Window::OnSetCursorMsg(const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    if (m_pEventClick != nullptr) {
        bHandled = true;
        return 0;
    }

    UiPoint pt;
    GetCursorPos(pt);
    ScreenToClient(pt);
    SetLastMousePos(pt);
    Control* pControl = FindControl(pt);
    if (pControl != nullptr) {
        //Return value to be confirmed: if the application handles this message, it should return TRUE to stop further processing or FALSE to continue.
        std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
        EventArgs msgData;
        msgData.ptMouse = pt;
        pControl->SendEvent(kEventSetCursor, msgData);
        bHandled = true;
        if (windowFlag.expired()) {
            return 0;
        }
        else if (pControl->IsCefOSR()) {
            //In offscreen rendering mode, the system must handle cursor messages, otherwise the cursor misbehaves
            bHandled = false;
        }
    }
    return 0;
}

LRESULT Window::OnContextMenuMsg(const UiPoint& pt, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    ReleaseCapture();

    if ((pt.x != -1) && (pt.y != -1)) {
        SetLastMousePos(pt);
        Control* pControl = FindContextMenuControl(&pt);
        if (pControl != nullptr) {
            Control* ptControl = FindControl(pt);//the control at the current click point
            std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
            EventArgs msgData;
            msgData.ptMouse = pt;
            msgData.lParam = (LPARAM)ptControl;
            pControl->SendEvent(kEventContextMenu, msgData);
            if (windowFlag.expired()) {
                return 0;
            }
        }
    }
    else {
        //If the user pressed SHIFT+F10, the context menu position is -1, -1, 
        //The application should display the context menu at the location of the current selection (not at (xPos, yPos)).
        Control* pControl = FindContextMenuControl(nullptr);
        if (pControl != nullptr) {
            std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
            EventArgs msgData;
            msgData.ptMouse = pt;
            msgData.lParam = 0;
            pControl->SendEvent(kEventContextMenu, msgData);
            if (windowFlag.expired()) {
                return 0;
            }
        }
    }
    return 0;
}

LRESULT Window::OnKeyDownMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kAlt) {
        if (vkCode != kVK_MENU) {
            ASSERT(Keyboard::IsKeyDown(kVK_MENU));
        }        
    }
#endif
    bHandled = false;
    LRESULT lResult = 0;
    if (modifierKey & ModifierKey::kAlt) {
        //Contains the modifier key: Alt
        m_pEventKey = m_pFocus;
        if (m_pEventKey != nullptr) {
            std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
            EventArgs msgData;
            msgData.vkCode = vkCode;
            msgData.modifierKey = modifierKey;
            msgData.wParam = nativeMsg.wParam;
            msgData.lParam = nativeMsg.lParam;
            m_pEventKey->SendEvent(kEventKeyDown, msgData);
            if (windowFlag.expired()) {
                return lResult;
            }
        }
        return lResult;
    }

    if ((vkCode == kVK_ESCAPE) && IsWindowFullscreen()) {
        //Exit fullscreen when ESC is pressed
        if (GetFullscreenControl() != nullptr) {
            ExitControlFullscreen();
        }
        else {
            ExitFullscreen();
        }
        return lResult;
    }
    if (m_pFocus != nullptr) {
        bool bMsgHandled = false;
        if (vkCode == kVK_TAB) {
            if (m_pFocus->IsVisible() &&
                m_pFocus->IsEnabled() &&
                m_pFocus->IsWantTab()) {
                bMsgHandled = false;
            }
            else {
                //Switches the focus control via the TAB key
                SetNextTabControl(!Keyboard::IsKeyDown(kVK_SHIFT));
                bMsgHandled = true;
            }
        }
        if(!bMsgHandled) {
            m_pEventKey = m_pFocus;
            std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
            EventArgs msgData;
            msgData.vkCode = vkCode;
            msgData.modifierKey = modifierKey;
            msgData.wParam = nativeMsg.wParam;
            msgData.lParam = nativeMsg.lParam;
            m_pEventKey->SendEvent(kEventKeyDown, msgData);
            if (windowFlag.expired()) {
                return lResult;
            }
        }
    }
    else {
        m_pEventKey = nullptr;
    }
    if ((vkCode == kVK_ESCAPE) && (m_pEventClick != nullptr)) {
        std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
        m_pEventClick->SendEvent(kEventMouseClickEsc);
        if (windowFlag.expired()) {
            return lResult;
        }
    }
    return lResult;
}

LRESULT Window::OnKeyUpMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kAlt) {
        if (vkCode != kVK_MENU) {
            ASSERT(Keyboard::IsKeyDown(kVK_MENU));
        }
    }
#endif

    bHandled = false;
    LRESULT lResult = 0;
    if (m_pEventKey != nullptr) {
        std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
        EventArgs msgData;
        msgData.vkCode = vkCode;
        msgData.modifierKey = modifierKey;
        msgData.wParam = nativeMsg.wParam;
        msgData.lParam = nativeMsg.lParam;
        m_pEventKey->SendEvent(kEventKeyUp, msgData);
        if (windowFlag.expired()) {
            return lResult;
        }
        m_pEventKey = nullptr;
    }
    return lResult;
}

LRESULT Window::OnCharMsg(VirtualKeyCode vkCode, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kAlt) {
        ASSERT(Keyboard::IsKeyDown(kVK_MENU));
    }
#endif

    bHandled = false;
    LRESULT lResult = 0;
    ControlPtr pFocusControl = (m_pEventKey != nullptr) ? m_pEventKey : m_pFocus;
    if (pFocusControl != nullptr) {
        EventArgs msgData;
        msgData.vkCode = vkCode;
        msgData.modifierKey = modifierKey;
        msgData.wParam = nativeMsg.wParam;
        msgData.lParam = nativeMsg.lParam;
        msgData.eventData = nativeMsg.uMsg;
        pFocusControl->SendEvent(kEventChar, msgData);
    }
    return lResult;
}

LRESULT Window::OnHotKeyMsg(int32_t /*hotkeyId*/, VirtualKeyCode /*vkCode*/, uint32_t modifierKey, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kAlt) {
        ASSERT(Keyboard::IsKeyDown(kVK_MENU));
    }
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
    if (modifierKey & ModifierKey::kWin) {
        ASSERT(Keyboard::IsKeyDown(kVK_LWIN) || Keyboard::IsKeyDown(kVK_RWIN));
    }
#endif
    ASSERT_UNUSED_VARIABLE(modifierKey);

    //To be added (needs confirmation; it should probably be added to the window)
    bHandled = false;
    return 0;
}

LRESULT Window::OnMouseWheelMsg(int32_t wheelDelta, const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    bHandled = false;
    LRESULT lResult = 0;
    SetLastMousePos(pt);
    Control* pControl = FindControl(pt);
    if (pControl != nullptr) {
        EventArgs msgData;
        msgData.eventData = wheelDelta;
        msgData.modifierKey = modifierKey;
        msgData.ptMouse = pt;
        msgData.wParam = nativeMsg.wParam;
        msgData.lParam = nativeMsg.lParam;
        pControl->SendEvent(kEventMouseWheel, msgData);
    }
    return lResult;
}

LRESULT Window::OnMouseMoveMsg(const UiPoint& pt, uint32_t modifierKey, bool bFromNC, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    //Whether ToolTip needs to be handled (not for NC messages, because handling them triggers WM_MOUSEMOVE, which affects the flow)
    bool bProcessToolTip = !bFromNC;

    bHandled = false;
    LRESULT lResult = 0;
    if (bProcessToolTip) {
        m_toolTip->SetMouseTracking(this, true);
    }
    SetLastMousePos(pt);

    // Do not move the focus to the new control when the mouse is pressed
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    if (!IsCaptured()) {
        if (!HandleMouseEnterLeave(pt, modifierKey, bProcessToolTip)) {
            return lResult;
        }
    }
    if (windowFlag.expired()) {
        return lResult;
    }

    //Dynamic display of the fullscreen button
    if (m_bControlFullscreen) {
        ProcessFullscreenButtonMouseMove(pt);
    }

    EventArgs msgData;
    msgData.modifierKey = modifierKey;
    msgData.ptMouse = pt;
    msgData.wParam = nativeMsg.wParam;
    msgData.lParam = nativeMsg.lParam;
    if (m_pEventClick != nullptr) {        
        m_pEventClick->SendEvent(kEventMouseMove, msgData);
    }
    else if (m_pEventHover != nullptr) {
        m_pEventHover->SendEvent(kEventMouseMove, msgData);
    }    
    return lResult;
}


bool Window::HandleMouseEnterLeave(const UiPoint& pt, uint32_t modifierKey, bool bHideToolTip)
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    ControlPtr pNewHover = ControlPtr(FindControl(pt));
    //Set the new Hover control (the m_pEventHover value must be set first, otherwise the Enter/Leave mouse message handling logic in Control::HandleEvent conflicts)
    ControlPtr pOldHover = m_pEventHover;
    m_pEventHover = pNewHover;

    ControlPtr pNewToolTip = ControlPtr(FindToolTipControl(pt));
    //Sets the new ToolTip control
    ControlPtr pOldToolTip = m_pEventToolTip;
    m_pEventToolTip = pNewToolTip;
    if ((pNewToolTip != pOldToolTip) && (pOldToolTip != nullptr)) {
        if (bHideToolTip) {
            m_toolTip->HideToolTip();
        }
    }

    if ((pNewHover != pOldHover) && (pOldHover != nullptr)) {
        //The control in the Hover state changed; the Tooltip of the previous Hover control should disappear
        EventArgs msgData;
        msgData.modifierKey = modifierKey;
        msgData.ptMouse = pt;
        pOldHover->SendEvent(kEventMouseLeave, msgData);
        if (windowFlag.expired()) {
            return false;
        }
    }
    ASSERT(pNewHover == m_pEventHover);
    if (pNewHover != m_pEventHover) {
        return false;
    }

    if ((pNewHover != pOldHover) && (pNewHover != nullptr)) {
        EventArgs msgData;
        msgData.modifierKey = modifierKey;
        msgData.ptMouse = pt;
        pNewHover->SendEvent(kEventMouseEnter, msgData);
        if (windowFlag.expired()) {
            return false;
        }
    }
    return true;
}

LRESULT Window::OnMouseHoverMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    bHandled = false;
    LRESULT lResult = 0;
    m_toolTip->SetMouseTracking(this, false);
    if (IsCaptured()) {
        //If in Capture state, do not show the ToolTip
        return lResult;
    }
    Control* pNewHover = FindControl(pt);
    if (pNewHover == nullptr) {
        return lResult;
    }
    std::weak_ptr<WeakFlag> hoverFlag = pNewHover->GetWeakFlag();
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    EventArgs msgData;
    msgData.modifierKey = modifierKey;
    msgData.ptMouse = pt;
    msgData.wParam = nativeMsg.wParam;
    msgData.lParam = nativeMsg.lParam;
    pNewHover->SendEvent(kEventMouseHover, msgData);
    if (hoverFlag.expired() || windowFlag.expired()) {
        return lResult;
    }

    //The control showing the ToolTip
    ControlPtr pOldToolTip = m_pEventToolTip;
    Control* pNewToolTip = FindToolTipControl(pt);
    if ((pNewToolTip != nullptr) && (pOldToolTip == pNewToolTip)) {
        //Check and show the ToolTip info on demand
        UiRect rect = pNewToolTip->GetPos();
        uint32_t maxWidth = pNewToolTip->GetToolTipWidth();
        DString toolTipText = pNewToolTip->GetToolTipText();
        m_toolTip->ShowToolTip(this, rect, maxWidth, pt, toolTipText);
    }
    return lResult;
}

LRESULT Window::OnMouseLeaveMsg(const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    m_toolTip->HideToolTip();
    m_toolTip->ClearMouseTracking();
    return 0;
}

LRESULT Window::OnMouseLButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    bHandled = false;
    OnButtonDown(kEventMouseButtonDown, pt, nativeMsg, modifierKey);
    return 0;
}

LRESULT Window::OnMouseLButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    bHandled = false;
    OnButtonUp(kEventMouseButtonUp, pt, nativeMsg, modifierKey);
    return 0;
}

LRESULT Window::OnMouseLButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    bHandled = false;
    OnButtonDown(kEventMouseDoubleClick, pt, nativeMsg, modifierKey);
    return 0;
}

LRESULT Window::OnMouseRButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    bHandled = false;
    OnButtonDown(kEventMouseRButtonDown, pt, nativeMsg, modifierKey);
    return 0;
}

LRESULT Window::OnMouseRButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    bHandled = false;
    OnButtonUp(kEventMouseRButtonUp, pt, nativeMsg, modifierKey);
    return 0;
}

LRESULT Window::OnMouseRButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    bHandled = false;
    OnButtonDown(kEventMouseRDoubleClick, pt, nativeMsg, modifierKey);
    return 0;
}

LRESULT Window::OnMouseMButtonDownMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    bHandled = false;
    OnButtonDown(kEventMouseMButtonDown, pt, nativeMsg, modifierKey);
    return 0;
}

LRESULT Window::OnMouseMButtonUpMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    bHandled = false;
    OnButtonUp(kEventMouseMButtonUp, pt, nativeMsg, modifierKey);
    return 0;
}

LRESULT Window::OnMouseMButtonDbClickMsg(const UiPoint& pt, uint32_t modifierKey, const NativeMsg& nativeMsg, bool& bHandled)
{
#ifdef _DEBUG
    if (modifierKey & ModifierKey::kControl) {
        ASSERT(Keyboard::IsKeyDown(kVK_CONTROL));
    }
    if (modifierKey & ModifierKey::kShift) {
        ASSERT(Keyboard::IsKeyDown(kVK_SHIFT));
    }
#endif

    bHandled = false;
    OnButtonDown(kEventMouseMDoubleClick, pt, nativeMsg, modifierKey);
    return 0;
}

LRESULT Window::OnCaptureChangedMsg(const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    if (m_pFocus != nullptr) {
        m_pFocus->SendEvent(kEventCaptureChanged);
    }
    return 0;
}

LRESULT Window::OnWindowCloseMsg(uint32_t /*wParam*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
    return 0;
}

void Window::OnWindowCreateMsg(bool /*bDoModal*/, const NativeMsg& /*nativeMsg*/, bool& bHandled)
{
    bHandled = false;
}

void Window::OnWindowPosSnapped(bool bLeftSnap, bool bRightSnap, bool bTopSnap, bool bBottomSnap)
{
    if (IsWindowMaximized() || IsWindowMinimized()) {
        //Do nothing when the window is maximized or minimized
        return;
    }
    UiRect rcSizeBox = GetSizeBox();

    //No window border; do not handle
    if (rcSizeBox.left <= 0) {
        bLeftSnap = false;
    }
    if (rcSizeBox.top <= 0) {
        bTopSnap = false;
    }
    if (rcSizeBox.right <= 0) {
        bRightSnap = false;
    }
    if (rcSizeBox.bottom <= 0) {
        bBottomSnap = false;
    }
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->SetWindowPosSnap(bLeftSnap, bRightSnap, bTopSnap, bBottomSnap);
    }
}

void Window::OnButtonDown(EventType eventType, const UiPoint& pt, const NativeMsg& nativeMsg, uint32_t modifierKey)
{
    ASSERT(eventType == kEventMouseButtonDown ||
           eventType == kEventMouseRButtonDown ||
           eventType == kEventMouseMButtonDown ||
           eventType == kEventMouseDoubleClick ||
           eventType == kEventMouseRDoubleClick ||
           eventType == kEventMouseMDoubleClick);

    const bool bWindowFocused = IsWindowFocused();
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();    
    if ((eventType == kEventMouseButtonDown) || (eventType == kEventMouseMButtonDown) || (eventType == kEventMouseRButtonDown)) {
        SetCapture();
        if (windowFlag.expired()) {
            return;
        }
    }
    Shadow* pShadow = GetShadow();
    SetLastMousePos(pt);
    Control* pControl = FindControl(pt);
    if (pControl != nullptr) {
        std::weak_ptr<WeakFlag> controlFlag = pControl->GetWeakFlag();
        std::weak_ptr<WeakFlag> clickFlag;
        if (m_pEventClick != nullptr) {
            clickFlag = m_pEventClick->GetWeakFlag();
        }
        ControlPtr pOldEventClick = m_pEventClick;
        m_pEventClick = pControl;
        bool bOldCheckSetWindowFocus = IsCheckSetWindowFocus();
        SetCheckSetWindowFocus(false);
        pControl->SetFocus();
        if (windowFlag.expired()) {
            return;
        }
        SetCheckSetWindowFocus(bOldCheckSetWindowFocus);
        if (controlFlag.expired()) {
            if (m_pEventClick.get() == pControl) {
                m_pEventClick = nullptr;
            }
            return;
        }
        EventArgs msgData;
        msgData.modifierKey = modifierKey;
        msgData.ptMouse = pt;
        msgData.wParam = nativeMsg.wParam;
        msgData.lParam = nativeMsg.lParam;
        pControl->SendEvent(eventType, msgData);
        if (windowFlag.expired()) {
            return;
        }
        if ((pOldEventClick != nullptr) && (pOldEventClick != pControl) && !clickFlag.expired()) {
            pOldEventClick->SendEvent(kEventMouseClickChanged);
            if (windowFlag.expired()) {
                return;
            }
        }
    }
    else if (!IsUseSystemCaption() && (pShadow != nullptr) && IsShadowAttached()) {
        //Check whether the click is on the window shadow area (allows mouse clicks on the shadow to pass through to the window behind)
        pShadow->CheckMouseClickOnShadow(eventType, pt);
    }
    if (!bWindowFocused && !windowFlag.expired()) {
        //Ensure the clicked window has input focus (solves the problem in CEF window mode where the input focus cannot switch from the page to the address bar)
        CheckSetWindowFocus();
    }
}

void Window::OnButtonUp(EventType eventType, const UiPoint& pt, const NativeMsg& nativeMsg, uint32_t modifierKey)
{
    ASSERT(eventType == kEventMouseButtonUp || eventType == kEventMouseRButtonUp || eventType == kEventMouseMButtonUp);

    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    if ((eventType == kEventMouseButtonUp) || (eventType == kEventMouseRButtonUp) || (eventType == kEventMouseMButtonUp)) {
        ReleaseCapture();
    }
    if (windowFlag.expired()) {
        return;
    }
    SetLastMousePos(pt);
    if (m_pEventClick != nullptr) {
        EventArgs msgData;
        msgData.modifierKey = modifierKey;
        msgData.ptMouse = pt;
        msgData.wParam = nativeMsg.wParam;
        msgData.lParam = nativeMsg.lParam;
        m_pEventClick->SendEvent(eventType, msgData);
        if (windowFlag.expired()) {
            return;
        }
        m_pEventClick = nullptr;
    }
}

void Window::ClearStatus()
{
    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    m_pEventToolTip = nullptr;
    if (m_pEventHover != nullptr) {
        m_pEventHover->SendEvent(kEventMouseLeave);
        if (windowFlag.expired()) {
            return;
        }
        m_pEventHover = nullptr;
    }    
    if (m_pEventClick != nullptr) {
        m_pEventClick->SendEvent(kEventMouseLeave);
        if (windowFlag.expired()) {
            return;
        }
        m_pEventClick = nullptr;
    }
    if (m_pEventKey != nullptr) {
        m_pEventKey->SendEvent(kEventMouseLeave);
        if (windowFlag.expired()) {
            return;
        }
        m_pEventKey = nullptr;
    }
    KillFocusControl();
}

Control* Window::GetFocusControl() const
{
    return m_pFocus.get();
}

Control* Window::GetEventClick() const
{
    return m_pEventClick.get();
}

void Window::SetFocusControl(Control* pControl)
{
    if (pControl == m_pFocus) {
        return;
    }

    std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
    std::weak_ptr<WeakFlag> controlFlag;
    if (pControl != nullptr) {
        controlFlag = pControl->GetWeakFlag();
    }    
    ControlPtr pOldFocus = m_pFocus;
    if (pOldFocus != nullptr) {
        m_pFocus = nullptr;
        //WPARAM is the interface of the new focus control
        pOldFocus->SendEvent(kEventKillFocus, (WPARAM)pControl);
        if (windowFlag.expired()) {
            return;
        }        
        if ((pControl != nullptr) && controlFlag.expired()){
            //The control has been destroyed
            OnFocusControlChanged();
            return;
        }
    }
    if ((pControl != nullptr) && pControl->IsVisible() && pControl->IsEnabled()) {
        ASSERT(pControl->GetWindow() == this);

        //Sets the control as the focus control
        m_pFocus = pControl;
        m_pFocus->SendEvent(kEventSetFocus);
        if (windowFlag.expired()) {
            return;
        }        
    }
    if (!windowFlag.expired() && (pOldFocus != m_pFocus)) {
        OnFocusControlChanged();
    }

    if (!windowFlag.expired() && (pControl != nullptr) && !controlFlag.expired()) {
        //Ensure the window has focus (but controls in CEF child window mode do not compete with the child window for focus)
        if (IsCheckSetWindowFocus() && !pControl->IsCefNative()) {
            CheckSetWindowFocus();
            if (windowFlag.expired()) {
                return;
            }
        }
    }
}

void Window::SetCheckSetWindowFocus(bool bCheckSetWindowFocus)
{
    m_bCheckSetWindowFocus = bCheckSetWindowFocus;
}

bool Window::IsCheckSetWindowFocus() const
{
    return m_bCheckSetWindowFocus;
}

void Window::KillFocusControl()
{
    if (m_pFocus != nullptr) {
        std::weak_ptr<WeakFlag> windowFlag = GetWeakFlag();
        m_pFocus->SendEvent(kEventKillFocus);
        if (!windowFlag.expired()) {
            m_pFocus = nullptr;
            OnFocusControlChanged();
        }        
    }
}

void Window::OnFocusControlChanged()
{
    if (IsWindowFocused() && (m_pFocus == nullptr)) {
        //When there is no focus control, disable the input method
        NativeWnd()->SetImeOpenStatus(false);
    }
}

Window* Window::WindowFromPoint(const UiPoint& pt, bool bIgnoreChildWindow)
{
    WindowBase* pWindow = WindowBaseFromPoint(pt, bIgnoreChildWindow);
    if (!GlobalManager::Instance().Windows().HasWindowBase(pWindow)) {
        //Do not use windows that do not belong to this process, to avoid crashes with cross-process windows
        pWindow = nullptr;
    }
    if (pWindow != nullptr) {
        return dynamic_cast<Window*>(pWindow);
    }
    return nullptr;
}

void Window::UpdateToolTip()
{
    //Hide the existing one; it will be updated when shown again
    m_toolTip->HideToolTip();
    m_toolTip->ClearMouseTracking();
}

Control* Window::GetHoverControl() const
{
    return m_pEventHover.get();
}

bool Window::SetNextTabControl(bool bForward)
{
    Box* pRoot = GetRoot();
    if (pRoot == nullptr) {
        return false;
    }
    // Find next/previous tabbable control
    FINDTABINFO info1 = { 0 };
    info1.pFocus = m_pFocus.get();
    info1.bForward = bForward;
    Control* pControl = pRoot->FindControl(ControlFinder::FindControlFromTab, &info1, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
    if (pControl == nullptr) {
        if (bForward) {
            // Wrap around
            FINDTABINFO info2 = { 0 };
            info2.pFocus = bForward ? nullptr : info1.pLast;
            info2.bForward = bForward;
            pControl = pRoot->FindControl(ControlFinder::FindControlFromTab, &info2, UIFIND_VISIBLE | UIFIND_ENABLED | UIFIND_ME_FIRST);
        }
        else {
            pControl = info1.pLast;
        }
    }
    if (pControl != nullptr) {
        SetFocusControl(pControl);
    }
    return true;
}

void Window::SetArrange(bool bArrange)
{
    m_bIsArranged = bArrange;
}

void Window::PostQuitMsgWhenClosed(bool bPostQuitMsg)
{
    m_bPostQuitMsgWhenClosed = bPostQuitMsg;
}

ui::IRender* Window::GetRender() const
{
    //Assertion here: when the business flow reaches this point, the render must have been created, otherwise there are logic problems (e.g., estimating the control size and automatically setting the ToolTip width will be abnormal)
    ASSERT(m_render != nullptr);
    ResizeRenderToClientSize();
    return m_render.get();
}

class RenderWindowDpi: public IRenderDpi
{
public:
    explicit RenderWindowDpi(Window* pWindow): m_pWindow(pWindow)
    {
        m_windowFlag = pWindow->GetWeakFlag();
    }
    virtual ~RenderWindowDpi() override {}

    /** Scales an integer according to the UI scale
    * @param[in] iValue An integer
    * @return int The scaled value
    */
    virtual int32_t GetScaleInt(int32_t iValue) const override
    {
        const DpiManager& dpi = ((m_pWindow != nullptr) && !m_windowFlag.expired()) ? m_pWindow->Dpi() : GlobalManager::Instance().Dpi();
        return dpi.GetScaleInt(iValue);
    }

    /** Scales an integer according to the UI scale
    * @param [in] fValue A floating point number
    * @return The scaled value
    */
    virtual float GetScaleFloat(float fValue) const override
    {
        const DpiManager& dpi = ((m_pWindow != nullptr) && !m_windowFlag.expired()) ? m_pWindow->Dpi() : GlobalManager::Instance().Dpi();
        return dpi.GetScaleFloat(fValue);
    }

private:
    Window* m_pWindow;
    std::weak_ptr<WeakFlag> m_windowFlag;
};

std::shared_ptr<IRenderDpi> Window::GetRenderDpi()
{
    IRenderDpiPtr spRenderDpi = std::make_shared<RenderWindowDpi>(this);
    return spRenderDpi;
}

void Window::SetWindowAttributesApplied(bool bApplied)
{
    m_bWindowAttributesApplied = bApplied;
}

bool Window::IsWindowAttributesApplied() const
{
    return m_bWindowAttributesApplied;
}

void Window::OnShowWindow(bool bShow)
{
    if (bShow && !m_bFirstLayout && (GetRoot() != nullptr)) {
        //First display
        PreparePaint(false);
    }
}

bool Window::PreparePaint(bool bArrange)
{
    //When estimating the control size, the Render needs width and height data, so it must be initialized with Resize
    bool bRet = ResizeRenderToClientSize();
    Box* pRoot = GetRoot();
    if (pRoot == nullptr) {
        return false;
    }

    bool bUpdated = false;
    if (m_bIsArranged && pRoot->IsArranged()) {
        //If the width and height configured on the root are auto type, automatically adjust the window size
        AutoResizeWindow(true);
        bUpdated = true;
    }

    //Lays out the control
    if (bArrange) {
        ArrangeRoot();
        bUpdated = true;
    }

    if (bUpdated) {
        //Operations such as resizing the window may occur during this period; they need to be synchronized
        bRet = ResizeRenderToClientSize();
    }
    return bRet;
}

bool Window::AutoResizeWindow(bool bRepaint)
{
    Box* pRoot = GetRoot();
    if (pRoot == nullptr) {
        return false;
    }
    bool bResized = false;
    if ((pRoot != nullptr) && (!pRoot->GetFixedWidth().IsStretch() || !pRoot->GetFixedHeight().IsStretch())) {
        //Container-related attributes: if either the width or the height is not a stretch type, automatically adjust the window size according to the container size
        UiSize maxSize(999999, 999999);
        const UiEstSize estSize = pRoot->EstimateSize(maxSize);
        if (!estSize.cx.IsStretch() || !estSize.cy.IsStretch()) {
            UiSize newSize(estSize.cx.GetInt32(), estSize.cy.GetInt32());
            newSize.cx = std::clamp(newSize.cx, pRoot->GetMinWidth(), pRoot->GetMaxWidth());
            newSize.cy = std::clamp(newSize.cy, pRoot->GetMinHeight(), pRoot->GetMaxHeight());

            if (Dpi().HasPixelDensity()) {
                //Converts to the window size
                newSize.cx = (int32_t)std::round(newSize.cx / Dpi().GetPixelDensity());
                newSize.cy = (int32_t)std::round(newSize.cy / Dpi().GetPixelDensity());
            }

            UiRect rcWindow;
            GetWindowRect(rcWindow);
            if (estSize.cx.IsStretch()) {
                newSize.cx = rcWindow.Width();
            }
            if (estSize.cy.IsStretch()) {
                newSize.cy = rcWindow.Height();
            }
            //The window height and width must not be set to 0 (note: not supported internally by SDL)
            newSize.cx = std::max(newSize.cx, 1);
            newSize.cy = std::max(newSize.cy, 1);
            if ((rcWindow.Width() != newSize.cx) || (rcWindow.Height() != newSize.cy)) {
                Resize(newSize.cx, newSize.cy, true, false);
                bResized = true;
                if (bRepaint) {
                    InvalidateAll();
                }
            }
        }
    }
    return bResized;
}

void Window::ArrangeRoot()
{
    UiRect rcClient;
    GetClientRect(rcClient);
    if (rcClient.IsEmpty()) {
        return;
    }
    Box* pRoot = GetRoot();
    if (pRoot == nullptr) {
        return;
    }
    if (m_bIsArranged) {
        m_bIsArranged = false;
        if (pRoot->IsArranged() || (pRoot->GetPos() != rcClient)) {
            //Re-arrange the layout of all controls
            pRoot->SetPos(rcClient);
        }
        else {
            //Only re-arrange the layout of the controls that have updates
            Control* pControl = pRoot->FindControl(ControlFinder::FindControlFromUpdate, nullptr, UIFIND_VISIBLE | UIFIND_ME_FIRST);
            while (pControl != nullptr) {
                pControl->SetPos(pControl->GetPos());
                //ASSERT(!pControl->IsArranged());
                pControl = pRoot->FindControl(ControlFinder::FindControlFromUpdate, nullptr, UIFIND_VISIBLE | UIFIND_ME_FIRST);
            }
        }
        if (!m_bFirstLayout) {
            m_bFirstLayout = true;
            OnFirstLayout();
        }
    }
    else if (pRoot->GetPos() != rcClient) {
        //Re-arrange the layout of all controls
        pRoot->SetPos(rcClient);
    }
}

void Window::SetRenderOffset(UiPoint renderOffset)
{
    m_renderOffset = renderOffset;
    InvalidateAll();
}

void Window::SetRenderOffsetX(int renderOffsetX)
{
    m_renderOffset.x = renderOffsetX;
    InvalidateAll();
}

void Window::SetRenderOffsetY(int renderOffsetY)
{
    m_renderOffset.y = renderOffsetY;
    InvalidateAll();
}

void Window::OnFirstLayout()
{
    Box* pRoot = GetRoot();
    if ((pRoot != nullptr) && pRoot->IsVisible()) {
        pRoot->SetFadeVisible(true);
    }
}

Control* Window::OnFindControl(const UiPoint& pt) const
{
    return FindControl(pt);
}

Control* Window::FindControl(const UiPoint& pt) const
{
    if (GetRoot() == nullptr) {
        return nullptr;
    }
    Control* pControl = m_controlFinder.FindControl(pt);
    if ((pControl != nullptr) && (pControl->GetWindow() != this)) {
        ASSERT(0);
        pControl = nullptr;
    }
    return pControl;
}

Control* Window::FindToolTipControl(const UiPoint& pt) const
{
    if (GetRoot() == nullptr) {
        return nullptr;
    }
    Control* pControl = m_controlFinder.FindToolTipControl(pt);    
    if (pControl != nullptr) {
        Window* pWindow = pControl->GetWindow();
        if (pWindow != this) {
            //ASSERT(0); for menus, this situation occurs when a submenu pops up
            pControl = nullptr;
        }
    }
    return pControl;
}

Control* Window::FindContextMenuControl(const UiPoint* pt) const
{
    Control* pControl = m_controlFinder.FindContextMenuControl(pt);
    if ((pControl != nullptr) && (pControl->GetWindow() != this)) {
        ASSERT(0);
        pControl = nullptr;
    }
    return pControl;
}

Box* Window::FindDroppableBox(const UiPoint& pt, uint8_t nDropInId) const
{
    Box* pControl = m_controlFinder.FindDroppableBox(pt, nDropInId);
    if ((pControl != nullptr) && (pControl->GetWindow() != this)) {
        ASSERT(0);
        pControl = nullptr;
    }
    return pControl;
}

Control* Window::FindControl(const DString& strName) const
{
    return m_controlFinder.FindSubControlByName(GetRoot(), strName);
}

Control* Window::FindSubControlByPoint(Control* pParent, const UiPoint& pt) const
{
    return m_controlFinder.FindSubControlByPoint(pParent, pt);
}

Control* Window::FindSubControlByName(Control* pParent, const DString& strName) const
{
    return m_controlFinder.FindSubControlByName(pParent, strName);
}

Shadow* Window::GetShadow() const
{
    ASSERT(m_shadow != nullptr);
    if (m_bControlFullscreen) {
        //When a control is fullscreen, shadow-related operations are disabled
        FullscreenBox* pFullscreenBox = dynamic_cast<FullscreenBox*>(m_pRoot.get());
        if (pFullscreenBox != nullptr) {
            return nullptr;
        }
    }
    return m_shadow.get();
}

void Window::NotifyWindowEnterFullscreen()
{
    //The window entered fullscreen
    ProcessWindowEnterFullscreen();
}

void Window::NotifyWindowExitFullscreen()
{
    //The window exited fullscreen
    ProcessWindowExitFullscreen();
}

void Window::ProcessWindowMaximized()
{
    if (!IsUseSystemCaption() && !IsWindowFullscreen()) {
        //When maximized, save and set the container margin in the fullscreen state
        SetWindowMaximizedMargin();        
    }
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->MaximizedOrRestored(true);
    }
}

void Window::ProcessWindowRestored()
{
    Shadow* pShadow = GetShadow();
    if (pShadow != nullptr) {
        pShadow->MaximizedOrRestored(false);
    }
    //Restore the outer margin when restoring
    RestoreWindowMaximizedMargin();
}

void Window::SetWindowMaximizedMargin()
{
    FullscreenBox* pFullscreenBox = dynamic_cast<FullscreenBox*>(m_pRoot.get());
    if (pFullscreenBox != nullptr) {
        return;
    }
    UiRect rcWindow;
    GetWindowRect(rcWindow);
    UiRect rcClientRect;
    GetClientRect(rcClientRect);
    int32_t cxClient = rcClientRect.Width();
    int32_t cyClient = rcClientRect.Height();
    if (Dpi().HasPixelDensity()) {
        Dpi().UnscaleInt(cxClient);
        Dpi().UnscaleInt(cyClient);
        Dpi().ScaleWindowSize(cxClient);
        Dpi().ScaleWindowSize(cyClient);
    }
    if ((cxClient == rcWindow.Width()) && (cyClient == rcWindow.Height())) {
        //In fullscreen, set the outer margin to prevent client area content from overflowing the screen
        UiRect rcWork;
        GetMonitorWorkRect(rcWork);

        UiMargin rcFullscreenMargin;
        if (rcWindow.left < rcWork.left) {
            rcFullscreenMargin.left = rcWork.left - rcWindow.left;
        }
        if (rcWindow.top < rcWork.top) {
            rcFullscreenMargin.top = rcWork.top - rcWindow.top;
        }
        if (rcWindow.right > rcWork.right) {
            rcFullscreenMargin.right = rcWindow.right - rcWork.right;
        }
        if (rcWindow.bottom > rcWork.bottom) {
            rcFullscreenMargin.bottom = rcWindow.bottom - rcWork.bottom;
        }
        if (Dpi().HasPixelDensity()) {
            rcFullscreenMargin.left = (int32_t)std::round(rcFullscreenMargin.left * Dpi().GetPixelDensity());
            rcFullscreenMargin.top = (int32_t)std::round(rcFullscreenMargin.top * Dpi().GetPixelDensity());
            rcFullscreenMargin.right = (int32_t)std::round(rcFullscreenMargin.right * Dpi().GetPixelDensity());
            rcFullscreenMargin.bottom = (int32_t)std::round(rcFullscreenMargin.bottom * Dpi().GetPixelDensity());
        }
        bool bHasShadowBox = false;
        Box* pXmlRoot = GetXmlRoot();
        Shadow* pShadow = GetShadow();
        if ((pShadow != nullptr) && pShadow->HasShadowBox()) {
            bHasShadowBox = true;
        }
        if (pXmlRoot != nullptr) {
            if (bHasShadowBox) {
                //The shadowed Box
                UiMargin rcMargin = pXmlRoot->GetMargin();
                rcMargin.left += (rcFullscreenMargin.left - m_rcWindowMaximizedMargin.left);
                rcMargin.top += (rcFullscreenMargin.top - m_rcWindowMaximizedMargin.top);
                rcMargin.right += (rcFullscreenMargin.right - m_rcWindowMaximizedMargin.right);
                rcMargin.bottom += (rcFullscreenMargin.bottom - m_rcWindowMaximizedMargin.bottom);
                m_rcWindowMaximizedMargin = rcFullscreenMargin;
                pXmlRoot->SetMargin(rcMargin, false);
            }
            else {
                //The non-shadowed Box
                UiPadding rcPadding = pXmlRoot->GetPadding();
                rcPadding.left += (rcFullscreenMargin.left - m_rcWindowMaximizedMargin.left);
                rcPadding.top += (rcFullscreenMargin.top - m_rcWindowMaximizedMargin.top);
                rcPadding.right += (rcFullscreenMargin.right - m_rcWindowMaximizedMargin.right);
                rcPadding.bottom += (rcFullscreenMargin.bottom - m_rcWindowMaximizedMargin.bottom);
                m_rcWindowMaximizedMargin = rcFullscreenMargin;
                pXmlRoot->SetPadding(rcPadding, false);
            }
        }
    }
}

void Window::RestoreWindowMaximizedMargin()
{
    FullscreenBox* pFullscreenBox = dynamic_cast<FullscreenBox*>(m_pRoot.get());
    if (pFullscreenBox != nullptr) {
        return;
    }
    if (!m_rcWindowMaximizedMargin.IsEmpty()) {
        bool bHasShadowBox = false;
        Box* pXmlRoot = GetXmlRoot();
        Shadow* pShadow = GetShadow();
        if ((pShadow != nullptr) && pShadow->HasShadowBox()) {
            bHasShadowBox = true;
        }
        if (pXmlRoot != nullptr) {
            if (bHasShadowBox) {
                //The shadowed Box
                UiMargin rcMargin = pXmlRoot->GetMargin();
                rcMargin.left -= m_rcWindowMaximizedMargin.left;
                rcMargin.top -= m_rcWindowMaximizedMargin.top;
                rcMargin.right -= m_rcWindowMaximizedMargin.right;
                rcMargin.bottom -= m_rcWindowMaximizedMargin.right;
                pXmlRoot->SetMargin(rcMargin, false);
            }
            else {
                //The non-shadowed Box
                UiPadding rcPadding = pXmlRoot->GetPadding();
                rcPadding.left -= m_rcWindowMaximizedMargin.left;
                rcPadding.top -= m_rcWindowMaximizedMargin.top;
                rcPadding.right -= m_rcWindowMaximizedMargin.right;
                rcPadding.bottom -= m_rcWindowMaximizedMargin.right;
                pXmlRoot->SetPadding(rcPadding, false);
            }
        }
        m_rcWindowMaximizedMargin.Clear();
    }
}

void Window::ProcessWindowEnterFullscreen()
{
    //When fullscreen, the margin set during maximization needs to be restored
    RestoreWindowMaximizedMargin();
}

void Window::ProcessWindowExitFullscreen()
{
    FullscreenBox* pFullscreenBox = dynamic_cast<FullscreenBox*>(m_pRoot.get());
    if (pFullscreenBox != nullptr) {
        //Exit the control fullscreen state
        m_pRoot = pFullscreenBox->GetOldRoot();
        ASSERT(m_pRoot != nullptr);
        m_controlFinder.SetRoot(m_pRoot.get());
        pFullscreenBox->ExitControlFullscreen();
        if (m_pRoot != nullptr) {
            m_pRoot->SetVisible(true);
        }
        delete pFullscreenBox;
        pFullscreenBox = nullptr;
    }
    m_bControlFullscreen = false;
}

void Window::ProcessFullscreenButtonMouseMove(const UiPoint& pt)
{
    FullscreenBox* pFullscreenBox = dynamic_cast<FullscreenBox*>(m_pRoot.get());
    if (pFullscreenBox != nullptr) {
        pFullscreenBox->ProcessFullscreenButtonMouseMove(pt);
    }
}

bool Window::SetFullscreenControl(Control* pFullscreenControl, const DString& exitButtonClass)
{
    ASSERT(pFullscreenControl != nullptr);
    if (pFullscreenControl == nullptr) {
        return false;
    }
    ASSERT(m_pRoot != nullptr);
    if (m_pRoot == nullptr) {
        return false;
    }
    ASSERT(m_pRoot.get() != pFullscreenControl);
    if (m_pRoot.get() == pFullscreenControl) {
        return false;
    }

    bool bRet = false;
    FullscreenBox* pFullscreenBox = dynamic_cast<FullscreenBox*>(m_pRoot.get());
    if (pFullscreenBox != nullptr) {
        //Already in the control fullscreen state
        if (pFullscreenBox->GetFullscreenControl() == pFullscreenControl) {
            return true;
        }
        ASSERT(m_bControlFullscreen);
        ASSERT(IsWindowFullscreen());
        ASSERT(m_pRoot == pFullscreenBox);
        ASSERT(m_controlFinder.GetRoot() == pFullscreenBox);
        if (m_bControlFullscreen && IsWindowFullscreen() &&
            (m_pRoot == pFullscreenBox) && (m_controlFinder.GetRoot() == pFullscreenBox)) {
            //Only switch the fullscreen control without changing the fullscreen state
            if (pFullscreenBox->UpdateControlFullscreen(pFullscreenControl, exitButtonClass)) {
                //Resets the control state
                ClearStatus();

                //Sets the focus
                pFullscreenControl->SetFocus();
                bRet = true;
            }
        }
    }
    else {
        //Was not in the control fullscreen state
        pFullscreenBox = new FullscreenBox(this);
        if (pFullscreenBox->EnterControlFullscreen(m_pRoot.get(), pFullscreenControl, exitButtonClass)) {
            //Successfully entered the control fullscreen state
            m_controlFinder.SetRoot(pFullscreenBox);
            m_pRoot = pFullscreenBox;
            m_bControlFullscreen = true;

            //The window enters the fullscreen state
            this->EnterFullscreen();

            //Resets the control state
            ClearStatus();

            //Sets the focus
            pFullscreenControl->SetFocus();
            bRet = true;
        }
        else {
            delete pFullscreenBox;
            pFullscreenBox = nullptr;
        }
    }    
    return bRet;
}

void Window::ExitControlFullscreen()
{
    if (m_bControlFullscreen) {
        FullscreenBox* pFullscreenBox = dynamic_cast<FullscreenBox*>(m_pRoot.get());
        if (pFullscreenBox != nullptr) {
            //Exits the control fullscreen
            bool bWindowOldFullscreen = pFullscreenBox->IsWindowOldFullscreen();
            ProcessWindowExitFullscreen();
            if (bWindowOldFullscreen) {
                //The window was already fullscreen, no need to exit the window fullscreen state
                return;
            }
        }
    }

    //Exits the window fullscreen
    ExitFullscreen();
}

Control* Window::GetFullscreenControl() const
{
    if (m_bControlFullscreen) {
        FullscreenBox* pFullscreenBox = dynamic_cast<FullscreenBox*>(m_pRoot.get());
        if (pFullscreenBox != nullptr) {
            return pFullscreenBox->GetFullscreenControl();
        }
    }
    return nullptr;
}

void Window::OnDropEnterMsg(ControlDropType /*dropType*/, void* /*pDropData*/)
{
}

void Window::OnDropOverMsg(ControlDropType /*dropType*/, void* /*pDropData*/)
{
}

void Window::OnDropMsg(ControlDropType /*dropType*/, void* /*pDropData*/)
{
}

void Window::OnDropLeaveMsg()
{
}

void Window::OnDisplayResolutionChangedMsg(int32_t /*nColorDepth*/, int32_t /*nScreenWidth*/, int32_t /*nScreenHeight*/)
{
}

void Window::OnDisplayScaleChangedMsg(float /*fNewDisplayScale*/, float /*fNewPixelDensity*/)
{
}

} // namespace ui
