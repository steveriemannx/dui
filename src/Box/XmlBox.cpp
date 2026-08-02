#include "duilib/Box/XmlBox.h"
#include "duilib/Core/GlobalManager.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/Shadow.h"
#include "duilib/Utils/FilePathUtil.h"
#include "duilib/Utils/FileUtil.h"
#include <set>

namespace ui
{
XmlBox::XmlBox(Window* pWindow):
    Box(pWindow),
    m_pSubBox(nullptr)
{
    m_pXmlPreviewAttributes = std::make_unique<XmlPreviewAttributes>();
}

XmlBox::~XmlBox()
{
    size_t callbackId = (size_t)this;
    GlobalManager::Instance().RemoveResNotFoundCallback(callbackId);
    m_pShadow.reset();
}

DString XmlBox::GetType() const { return DUI_CTR_XMLBOX; }

void XmlBox::SetAttribute(const DString& strName, const DString& strValue)
{
    if (strName == _T("xml_file_path")) {
        SetXmlFilePath(FilePath(strValue));
    }
    else if (strName == _T("res_path")) {
        SetResPath(FilePath(strValue));
    }
    else {
        BaseClass::SetAttribute(strName, strValue);
    }
}

bool XmlBox::SetXmlFilePath(const FilePath& xmlPath)
{
    if (IsInited()) {
        bool bRet = LoadXmlData(xmlPath);
        if (bRet) {
            m_xmlPath = xmlPath;
        }
        FilePath xmlFileFullPath = bRet ? m_xmlFileFullPath : xmlPath;
        OnXmlDataLoaded(xmlFileFullPath, bRet);
        return bRet;
    }
    else {
        m_xmlPath = xmlPath;
        return true;
    }
}

const FilePath& XmlBox::GetXmlFilePath() const
{
    return m_xmlPath;
}

void XmlBox::SetResPath(const FilePath& resPath)
{
    m_resPath = resPath;
}

const FilePath& XmlBox::GetResPath() const
{
    return m_resPath;
}

void XmlBox::ClearXmlBox()
{
    XmlPreviewAttributes xmlPreviewAttributes;
    ClearLoadedXmlData(xmlPreviewAttributes);
    m_xmlPath.Clear();
}

void XmlBox::AddLoadXmlCallback(LoadXmlCallback callback, size_t callbackId)
{
    if (callback != nullptr) {
        LoadXmlCallbackData data;
        data.m_callback = callback;
        data.m_callbackId = callbackId;
        m_loadXmlCallbacks.push_back(data);
    }
}

void XmlBox::RemoveLoadXmlCallback(size_t callbackId)
{
    auto iter = m_loadXmlCallbacks.begin();
    while (iter != m_loadXmlCallbacks.end()) {
        if (iter->m_callbackId == callbackId) {
            iter = m_loadXmlCallbacks.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

void XmlBox::OnInit()
{
    if (IsInited()) {
        return;
    }
    BaseClass::OnInit();

    //Callback function for resource load failure
    size_t callbackId = (size_t)this;
    GlobalManager::Instance().AddResNotFoundCallback([this](const Control* pControl,
                                                            const FilePath& resPath,
                                                            FilePath& windowResPath,
                                                            FilePath& windowXmlPath) {
            if (!PlaceHolder::IsControlRelated(this, pControl)) {
                //The control is not a child control of this container, so it is not related
                return false;
            }
            (void)resPath;//You can inspect its value when debugging
            windowResPath = !m_resPath.IsEmpty() ? m_resPath : m_xmlResPath;
            FilePath xmlPath(m_xmlPath);
            windowXmlPath = xmlPath.GetParentPath();
            return true;
        }, callbackId);

    bool bRet = LoadXmlData(m_xmlPath);
    FilePath xmlFileFullPath = bRet ? m_xmlFileFullPath : m_xmlPath;
    OnXmlDataLoaded(xmlFileFullPath, bRet);
}

bool XmlBox::LoadXmlData(const FilePath& xmlPath)
{
    if (xmlPath.IsEmpty()) {
        //Clear the data and return success
        XmlPreviewAttributes xmlPreviewAttributes;
        ClearLoadedXmlData(xmlPreviewAttributes);
        return true;
    }
    bool bRet = false;
    Window* pWindow = GetWindow();
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return bRet;
    }

    //Find the path where the XML file is located
    std::vector<unsigned char> xmlFileData;
    FilePath xmlOutputPath;
    FilePath xmlResPath;
    if (!ReadXmlFileData(xmlPath, pWindow->GetResourcePath(), xmlFileData, xmlOutputPath, xmlResPath)) {
        //Failed to read the XML file data
        return bRet;
    }
    FilePath oldXmlResPath = m_xmlResPath;
    m_xmlResPath = xmlResPath;
    XmlPreviewAttributes xmlPreviewAttributes;
    Box* pSubBox = ui::GlobalManager::Instance().CreateBoxForXmlPreview(GetWindow(), xmlFileData, xmlPreviewAttributes, xmlOutputPath);
    if (pSubBox != nullptr) {
        ClearLoadedXmlData(xmlPreviewAttributes);

        //Check whether a shadow is present
        DString shadowTypeString;
        const std::map<DString, DString>& windowAttributesMap = xmlPreviewAttributes.m_windowAttributes;
        auto iter = windowAttributesMap.find(_T("shadow_type"));
        if (iter != windowAttributesMap.end()) {
            shadowTypeString = iter->second;
        }
        bool bShadowAttached = true;
        iter = windowAttributesMap.find(_T("shadow_attached"));
        if (iter != windowAttributesMap.end()) {
            bShadowAttached = (iter->second == _T("true")) ? true : false;
        }

        Shadow::ShadowType nShadowType = Shadow::ShadowType::kShadowDefault;
        if (!shadowTypeString.empty() && !Shadow::GetShadowType(shadowTypeString, nShadowType)) {
            bShadowAttached = false;
        }
        else if (nShadowType == Shadow::ShadowType::kShadowCustom) {
            bShadowAttached = false;
        }
        if (bShadowAttached) {
            m_pShadow = std::make_unique<Shadow>(GetWindow());
            m_pShadow->SetEnableShadowSnap(false);
            m_pShadow->SetEnableClickThroughWindow(false);
            m_pShadow->SetShadowType(nShadowType);
            pSubBox = m_pShadow->AttachShadow(pSubBox);
        }

        AddItem(pSubBox);
        m_pSubBox = pSubBox;
        *m_pXmlPreviewAttributes = xmlPreviewAttributes;
        m_xmlFileFullPath = xmlOutputPath;
        bRet = true;
    }
    else {
        m_xmlResPath.Swap(oldXmlResPath);
    }
    return bRet;
}

bool XmlBox::ReadXmlFileData(const FilePath& xmlInputPath, const FilePath& windowResPath,
                             std::vector<unsigned char>& xmlFileData, FilePath& xmlOutputPath, FilePath& xmlResPath) const
{
    xmlFileData.clear();
    xmlOutputPath.Clear();
    xmlResPath.Clear();
    const FilePath xmlFilePath(xmlInputPath);
    if (xmlFilePath.IsRelativePath() && GlobalManager::Instance().Zip().IsUseZip()) {
        bool bFoundXmlFile = false;
        FilePath sFile;
        if (!windowResPath.IsEmpty()) {
            //Search in the window directory
            sFile = FilePathUtil::JoinFilePath(GlobalManager::Instance().GetResourcePath(), windowResPath);
            sFile = FilePathUtil::JoinFilePath(sFile, xmlFilePath);
            if (GlobalManager::Instance().Zip().IsZipResExist(sFile)) {
                //Successfully found in the window resource directory
                bFoundXmlFile = true;
                xmlResPath = windowResPath;
            }
        }
        if (!bFoundXmlFile && !m_resPath.IsEmpty()) {
            //Search in the configured resource path
            sFile = FilePathUtil::JoinFilePath(GlobalManager::Instance().GetResourcePath(), m_resPath);
            sFile = FilePathUtil::JoinFilePath(sFile, xmlFilePath);
            if (GlobalManager::Instance().Zip().IsZipResExist(sFile)) {
                //Successfully found in the window resource directory
                bFoundXmlFile = true;
                xmlResPath = m_resPath;
            }
        }
        if (!bFoundXmlFile) {
            sFile = FilePathUtil::JoinFilePath(GlobalManager::Instance().GetResourcePath(), xmlFilePath);
            if (GlobalManager::Instance().Zip().IsZipResExist(sFile)) {
                //Successfully found in the resource root directory
                bFoundXmlFile = true;
                xmlResPath = GetFirstDirectory(xmlFilePath);
            }
        }        
        if (bFoundXmlFile && GlobalManager::Instance().Zip().GetZipData(sFile, xmlFileData) && !xmlFileData.empty()) {
            //Load it as XML data
            xmlOutputPath = sFile;
            return true;
        }
    }

    //Search from the local disk
    if (xmlFilePath.IsRelativePath()) {
        //Relative path
        bool bFoundXmlFile = false;
        FilePath xmlFileFullPath;
        if (!windowResPath.IsEmpty()) {
            //Search in the window directory
            xmlFileFullPath = FilePathUtil::JoinFilePath(GlobalManager::Instance().GetResourcePath(), windowResPath);
            xmlFileFullPath = FilePathUtil::JoinFilePath(xmlFileFullPath, xmlFilePath);
            if (xmlFileFullPath.IsExistsFile()) {
                //Successfully found in the window resource directory
                bFoundXmlFile = true;
                xmlResPath = windowResPath;
            }
        }        
        if (!bFoundXmlFile && !m_resPath.IsEmpty()) {
            //Search in the configured resource path
            xmlFileFullPath = FilePathUtil::JoinFilePath(GlobalManager::Instance().GetResourcePath(), m_resPath);
            xmlFileFullPath = FilePathUtil::JoinFilePath(xmlFileFullPath, xmlFilePath);
            if (xmlFileFullPath.IsExistsFile()) {
                //Successfully found in the window resource directory
                bFoundXmlFile = true;
                xmlResPath = m_resPath;
            }
        }
        if (!bFoundXmlFile) {
            xmlFileFullPath = FilePathUtil::JoinFilePath(GlobalManager::Instance().GetResourcePath(), xmlFilePath);
            if (xmlFileFullPath.IsExistsFile()) {
                //Successfully found in the resource root directory
                bFoundXmlFile = true;
                xmlResPath = GetFirstDirectory(xmlFilePath);
            }
        }
        if (bFoundXmlFile) {
            if (FileUtil::ReadFileData(xmlFileFullPath, xmlFileData) && !xmlFileData.empty()) {
                xmlOutputPath = xmlFileFullPath;
                return true;
            }
        }
    }
    else {
        //Absolute path
        if (FileUtil::ReadFileData(xmlFilePath, xmlFileData) && !xmlFileData.empty()) {
            xmlOutputPath = xmlFilePath;
            xmlResPath = GetResDirectory(xmlFilePath, windowResPath);
            return true;
        }
    }
    return false;
}

FilePath XmlBox::GetFirstDirectory(const FilePath& resPath) const
{
    FilePath firstDir;
    if (!resPath.IsEmpty() && resPath.IsRelativePath()) {
        DStringW resPathW = resPath.ToStringW();
        DStringW::size_type pos = resPathW.find_first_of(L"/\\");
        if (pos != DStringW::npos) {
            firstDir = FilePath(resPathW.substr(0, pos));
        }
    }
    return firstDir;
}

FilePath XmlBox::GetResDirectory(FilePath xmlFilePath, const FilePath& windowResPath) const
{
    FilePath resPath;
    if (!xmlFilePath.IsEmpty() && xmlFilePath.IsAbsolutePath()) {
        xmlFilePath.NormalizeFilePath();
        const DString xmlFilePathString = xmlFilePath.ToString();
        if (!windowResPath.IsEmpty()) {
            FilePath globalResPath = GlobalManager::Instance().GetResourcePath();
            globalResPath.NormalizeDirectoryPath();
            FilePath windowResPathFull = globalResPath;
            windowResPathFull.JoinFilePath(windowResPath);
            const  DString windowResPathFullString = windowResPathFull.ToString();
            if (xmlFilePathString.find(windowResPathFullString) != DString::npos) {
                //In the resource directory of the current window
                resPath = windowResPath;
            }
        }

        if (resPath.IsEmpty()) {
            FilePath globalResPath = GlobalManager::Instance().GetResourcePath();
            globalResPath.NormalizeDirectoryPath();
            const DString globalResPathString = globalResPath.ToString();
            DString::size_type pos = xmlFilePathString.find(globalResPathString);
            if (pos != DString::npos) {
                //In the global resource directory
                DString::size_type pos2 = pos + globalResPathString.size();
                if (pos2 < xmlFilePathString.size()) {
                    DString resSubPath = xmlFilePathString.substr(pos2);
                    resPath = GetFirstDirectory(FilePath(resSubPath));
                }
            }
        }
    }
    return resPath;
}

void XmlBox::ClearLoadedXmlData(const XmlPreviewAttributes& xmlPreviewAttributesNew)
{
    //Remove the UI child controls corresponding to the loaded XML data
    if (m_pSubBox != nullptr) {
        RemoveItem(m_pSubBox);
        m_pSubBox = nullptr;
    }
    m_pShadow.reset();
    m_xmlFileFullPath.Clear();
    m_xmlResPath.Clear();
    
    //Remove the common attributes previously loaded on the window, to avoid mutual interference
    Window* pWindow = GetWindow();
    if (pWindow != nullptr) {
        std::vector<DString> oldWindowClassList;
        oldWindowClassList.swap(m_pXmlPreviewAttributes->m_windowClassList);
        RemoveValuesInNewList(oldWindowClassList, xmlPreviewAttributesNew.m_windowClassList);
        for (const DString& className : oldWindowClassList) {
            pWindow->RemoveClass(className);
        }

        std::vector<DString> oldWindowTextColorList;
        oldWindowTextColorList.swap(m_pXmlPreviewAttributes->m_windowTextColorList);
        RemoveValuesInNewList(oldWindowTextColorList, xmlPreviewAttributesNew.m_windowTextColorList);
        for (const DString& textColor : oldWindowTextColorList) {
            pWindow->RemoveTextColor(textColor);
        }
    }

    std::vector<DString> oldGlobalFontIdList;
    oldGlobalFontIdList.swap(m_pXmlPreviewAttributes->m_globalFontIdList);
    RemoveValuesInNewList(oldGlobalFontIdList, xmlPreviewAttributesNew.m_globalFontIdList);
    for (const DString& fontId : oldGlobalFontIdList) {
        GlobalManager::Instance().Font().RemoveFontId(fontId);
    }
}

void XmlBox::RemoveValuesInNewList(std::vector<DString>& oldList, const std::vector<DString>& newList) const
{
    if (oldList.empty() || newList.empty()) {
        return;
    }
    std::set<DString> newValueSet;
    for (const DString& name : newList) {
        newValueSet.insert(name);
    }
    auto iter = oldList.begin();
    while (iter != oldList.end()) {
        if (newValueSet.find(*iter) != newValueSet.end()) {
            iter = oldList.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

void XmlBox::OnXmlDataLoaded(const FilePath& xmlPath, bool bSuccess)
{
    std::vector<LoadXmlCallbackData> loadXmlCallbacks = m_loadXmlCallbacks;
    for (const LoadXmlCallbackData& callbackData : loadXmlCallbacks) {
        callbackData.m_callback(xmlPath, bSuccess);
    }
}

const FilePath& XmlBox::GetXmlFileFullPath() const
{
    return m_xmlFileFullPath;
}

} //namespace ui
