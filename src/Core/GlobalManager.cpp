#include "duilib/Core/GlobalManager.h"
#include "duilib/Utils/StringUtil.h"
#include "duilib/Utils/FilePathUtil.h"
#include "duilib/Core/Window.h"
#include "duilib/Core/Control.h"
#include "duilib/Core/Box.h"

//The render engine
#include "duilib/RenderSkia/RenderFactory_Skia.h"

//Image decode interfaces
#include "duilib/Image/ImageDecoder_ICO.h"
#include "duilib/Image/ImageDecoder_Icon.h"
#include "duilib/Image/ImageDecoder_GIF.h"
#include "duilib/Image/ImageDecoder_PNG.h"
#include "duilib/Image/ImageDecoder_PAG.h"
#include "duilib/Image/ImageDecoder_SVG.h"
#include "duilib/Image/ImageDecoder_WEBP.h"
#include "duilib/Image/ImageDecoder_JPEG.h"
#include "duilib/Image/ImageDecoder_LOTTIE.h"
#include "duilib/Image/ImageDecoder_Common.h"

#if defined (DUILIB_BUILD_FOR_WIN)
    //Standard controls such as ToolTip/date time require initializing commctrl
    #include <commctrl.h>
    #include <Objbase.h>
#endif

#include <filesystem>

namespace ui 
{
/** The worker thread inside the library
*/
class UiWorkerThread : public ui::FrameworkThread
{
public:
    struct Param
    {
        DString name;
        int32_t nIdentifier;
    };
public:
    UiWorkerThread(const DString& threadName, int32_t nThreadIdentifier):
        FrameworkThread(threadName, nThreadIdentifier)
    { }
    virtual ~UiWorkerThread() override {}

private:
    /** Initialization before running, called before entering the message loop
    */
    virtual void OnInit() override
    {
#if defined (DUILIB_BUILD_FOR_WIN)
        HRESULT hr = ::CoInitialize(nullptr);
        ASSERT_UNUSED_VARIABLE((hr == S_OK) || (hr == S_FALSE));
#endif
    }

    /** Cleanup on exit, called after the message loop exits
    */
    virtual void OnCleanup() override
    {
#if defined (DUILIB_BUILD_FOR_WIN)
        ::CoUninitialize();
#endif
    }
};

GlobalManager::GlobalManager():
    m_platformData(nullptr),
    m_bAnimationEnabled(true)
{
}

GlobalManager::~GlobalManager()
{
}

GlobalManager& GlobalManager::Instance()
{
    static GlobalManager self;
    return self;
}

FilePath GlobalManager::GetDefaultResourcePath(bool bMacOsAppBundle)
{
    ui::FilePath resourcePath;
#ifdef DUILIB_BUILD_FOR_MACOS
    //On the MacOS platform, prefer to use the resource directory of the bundle
    if (bMacOsAppBundle) {
        resourcePath = ui::FilePathUtil::GetBundleResourcesPath();
        if (!resourcePath.IsEmpty()) {
            resourcePath.NormalizeDirectoryPath();
            resourcePath += _T("duilib/");
            if (!resourcePath.IsExistsDirectory()) {
                resourcePath.Clear();
            }
        }
    }
#else
    UNUSED_VARIABLE(bMacOsAppBundle);
#endif
    if (resourcePath.IsEmpty()) {
        resourcePath = ui::FilePathUtil::GetCurrentModuleDirectory();
        resourcePath += _T("resources/");
    }
    resourcePath.NormalizeDirectoryPath();
    return resourcePath;
}

bool GlobalManager::Startup(const ResourceParam& resParam,
                            DpiInitParam dpiInitParam,
                            const CreateControlCallback& callback)
{
    ASSERT(m_renderFactory == nullptr);
    if (m_renderFactory != nullptr) {
        return false;
    }
    //Initialize COM/OLE
#if defined (DUILIB_BUILD_FOR_WIN)
    HRESULT hr = ::CoInitialize(nullptr);
    ASSERT_UNUSED_VARIABLE((hr == S_OK) || (hr == S_FALSE));

    hr = ::OleInitialize(nullptr);
    ASSERT_UNUSED_VARIABLE((hr == S_OK) || (hr == S_FALSE));

    //Init Windows Common Controls (for the ToolTip control)
    ::InitCommonControls();
#endif

    //Record the current thread ID
    m_dwUiThreadId = std::this_thread::get_id();

    //Initialize the DPI awareness mode and the DPI value
    DpiManager& dpiManager = Dpi();
    dpiManager.InitDpiAwareness(dpiInitParam);

    //Initialize the image format decoders
    m_imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_SVG>());
    m_imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_PNG>());
    m_imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_GIF>());

#ifdef DUILIB_IMAGE_SUPPORT_JPEG_TURBO
    m_imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_JPEG>());
#endif

    m_imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_WEBP>());
    m_imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_ICO>());
    m_imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_Icon>());
    m_imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_LOTTIE>());

#ifdef DUILIB_IMAGE_SUPPORT_LIB_PAG
    m_imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_PAG>());
#endif

    //The common decoder, placed last
    m_imageDecoderFactory.AddImageDecoder(std::make_shared<ImageDecoder_Common>());
    

    //Initialize the timer manager
    m_timerManager.Initialize(m_platformData);

    //The Skia render engine implementation
    m_renderFactory = std::make_unique<RenderFactory_Skia>();    

    ASSERT(m_renderFactory != nullptr);
    if (m_renderFactory == nullptr) {
        return false;
    }

    //Save the callback function
    if (callback != nullptr) {
        m_pfnCreateControlCallbackList.push_back(callback);
    }

    //Initialize the thread pool
    StartInnerThread(ThreadIdentifier::kThreadWorker);
    StartInnerThread(ThreadIdentifier::kThreadImage1);
    StartInnerThread(ThreadIdentifier::kThreadImage2);

    //Load the resources
    if (!ReloadResource(resParam, false)) {
        m_renderFactory.reset();
        return false;
    }
    return true;
}

void GlobalManager::Shutdown()
{
    //Stop the thread pool
    for (std::shared_ptr<FrameworkThread> pThread: m_threadList) {
        if (pThread != nullptr) {
            pThread->Stop();
        }
    }
    m_threadList.clear();

    m_threadManager.Clear();
    m_timerManager.Clear();
    m_colorManager.Clear();    
    m_fontManager.RemoveAllFonts();
    m_fontManager.RemoveAllFontFiles();
    m_imageManager.RemoveAllImages();
    m_zipManager.CloseResZip();    
    m_langManager.ClearStringTable();
    m_windowManager.Clear();
    
    m_renderFactory.reset();
    m_renderFactory = nullptr;
    m_pfnCreateControlCallbackList.clear();
    m_globalClass.clear();    
    m_dwUiThreadId = std::thread::id();
    m_resourcePath.Clear();
    m_languagePath.Clear();
    m_fontFilePath.Clear();
    m_builderMap.clear();
    m_platformData = nullptr;

    //Execute the functions that clean up resources on exit
    for (std::function<void()> atExitFunction : m_atExitFunctions) {
        if (atExitFunction != nullptr) {
            atExitFunction();
        }
    }
    m_atExitFunctions.clear();

#if defined (DUILIB_BUILD_FOR_WIN)
    ::CoUninitialize();
    ::OleUninitialize();
#endif
}

bool GlobalManager::StopInnerThread(int32_t nThreadIdentifier)
{
    AssertUIThread();
    ASSERT((nThreadIdentifier == ui::kThreadWorker)  ||
           (nThreadIdentifier == ui::kThreadNetwork) ||
           (nThreadIdentifier == ui::kThreadImage1)  ||
           (nThreadIdentifier == ui::kThreadImage2));
    if ((nThreadIdentifier != ui::kThreadWorker)  &&
        (nThreadIdentifier != ui::kThreadNetwork) &&
        (nThreadIdentifier != ui::kThreadImage1)  &&
        (nThreadIdentifier != ui::kThreadImage2)) {
        return false;
    }
    bool bRet = false;
    for (auto iter = m_threadList.begin(); iter != m_threadList.end(); ++iter) {
        auto pThread = *iter;
        if ((pThread != nullptr) && (nThreadIdentifier == pThread->GetThreadIdentifier())) {
            m_threadList.erase(iter);
            pThread->Stop();
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool GlobalManager::StartInnerThread(int32_t nThreadIdentifier)
{
    AssertUIThread();
    ASSERT((nThreadIdentifier == ui::kThreadWorker)  ||
           (nThreadIdentifier == ui::kThreadNetwork) ||
           (nThreadIdentifier == ui::kThreadImage1)  ||
           (nThreadIdentifier == ui::kThreadImage2));
    if ((nThreadIdentifier != ui::kThreadWorker)  &&
        (nThreadIdentifier != ui::kThreadNetwork) &&
        (nThreadIdentifier != ui::kThreadImage1)  &&
        (nThreadIdentifier != ui::kThreadImage2)) {
        return false;
    }
    bool bRet = false;
    for (auto iter = m_threadList.begin(); iter != m_threadList.end(); ++iter) {
        auto pThread = *iter;
        if ((pThread != nullptr) && (nThreadIdentifier == pThread->GetThreadIdentifier())) {
            bRet = true;
            break;
        }
    }
    if (!bRet) {
        //Initialize the thread pool
        std::vector<UiWorkerThread::Param> threadParams = { {_T("Worker"), ThreadIdentifier::kThreadWorker},
                                                            {_T("Network"), ThreadIdentifier::kThreadNetwork},
                                                            {_T("Image1"), ThreadIdentifier::kThreadImage1},
                                                            {_T("Image2"), ThreadIdentifier::kThreadImage2} };
        for (const UiWorkerThread::Param& param : threadParams) {
            if (param.nIdentifier != nThreadIdentifier) {
                continue;
            }
            auto pThread = std::make_shared<UiWorkerThread>(param.name, param.nIdentifier);
            m_threadList.push_back(pThread);
            pThread->Start();
            bRet = true;
            break;
        }
    }
    return bRet;
}

const FilePath& GlobalManager::GetResourcePath() const
{
    return m_resourcePath;
}

void GlobalManager::SetResourcePath(const FilePath& strPath)
{
    m_resourcePath = strPath;
    m_resourcePath.NormalizeDirectoryPath();
}

void GlobalManager::SetPlatformData(void* pPlatformData)
{
    m_platformData = pPlatformData;
}

void* GlobalManager::GetPlatformData() const
{
    return m_platformData;
}

void GlobalManager::SetFontFilePath(const FilePath& strPath)
{
     m_fontFilePath = strPath;
     m_fontFilePath.NormalizeDirectoryPath();
}

const FilePath& GlobalManager::GetFontFilePath() const
{
    return m_fontFilePath;
}

void GlobalManager::SetLanguagePath(const FilePath& strPath)
{
    m_languagePath = strPath;
    m_languagePath.NormalizeDirectoryPath();
}

const FilePath& GlobalManager::GetLanguagePath() const
{
    return m_languagePath;
}

const DString& GlobalManager::GetLanguageFileName() const
{
    return m_languageFileName;
}

bool GlobalManager::ReloadResource(const ResourceParam& resParam, bool bInvalidate)
{
    AssertUIThread();
    //Validate the input parameters
    FilePath strResourcePath = resParam.resourcePath;
    if (resParam.GetResType() == ResourceType::kLocalFiles) {
        //In the form of local files, all resources exist as local files
        //const LocalFilesResParam& param = static_cast<const LocalFilesResParam&>(resParam);
        ASSERT(!strResourcePath.IsEmpty());
        if (strResourcePath.IsEmpty()) {
            return false;
        }
    }
    else if (resParam.GetResType() == ResourceType::kZipFile) {
        //The resource files are packaged into a zip archive and exist as local files
        const ZipFileResParam& param = static_cast<const ZipFileResParam&>(resParam);
        bool bZipOpenOk = Zip().OpenZipFile(param.zipFilePath, param.zipPassword);
        if (!bZipOpenOk) {
            ASSERT(!"OpenZipFile failed!");
            return false;
        }
    }
    else if (resParam.GetResType() == ResourceType::kMemoryRes) {
        //The resources are packaged into a custom binary archive embedded in the executable
        //(Qt qrc style, no zip container); accessed directly from memory
        const MemoryResParam& param = static_cast<const MemoryResParam&>(resParam);
        bool bResOpenOk = Zip().OpenMemoryArchive(param.pData, param.nSize);
        if (!bResOpenOk) {
            ASSERT(!"OpenMemoryArchive failed!");
            return false;
        }
    }
#ifdef DUILIB_BUILD_FOR_WIN
    else if (resParam.GetResType() == ResourceType::kResZipFile) {
        //The resource files are packaged into a zip archive and placed in the resource file of the exe/dll
        const ResZipFileResParam& param = static_cast<const ResZipFileResParam&>(resParam);
        bool bZipOpenOk = Zip().OpenResZip(param.hResModule, param.resourceName, param.resourceType, param.zipPassword);
        if (!bZipOpenOk) {
            ASSERT(!"OpenResZip failed!");
            return false;
        }
    }
#endif
    else {
        ASSERT(false);
        return false;
    }

    //Clear the original resource data (fonts, colors, Class definitions, image resources, etc.)
    m_fontManager.RemoveAllFonts();
    m_fontManager.RemoveAllFontFiles();
    m_colorManager.RemoveAllColors();
    RemoveAllImages();
    RemoveAllClasss();

    //Save the resource path
    SetResourcePath(FilePathUtil::JoinFilePath(strResourcePath, resParam.themePath));

    //Save the path of the font files
    SetFontFilePath(FilePathUtil::JoinFilePath(strResourcePath, resParam.fontFilePath));

    //Parse the global resource info (by default the "global.xml" file)
    ASSERT(!resParam.globalXmlFileName.empty());
    if (!resParam.globalXmlFileName.empty()) {
        WindowBuilder dialog_builder;
        Window paint_manager;
        if (dialog_builder.ParseXmlFile(FilePath(resParam.globalXmlFileName))) {
            dialog_builder.CreateControls(&paint_manager);
        }        
    }

    //Load the multi-language file (optional)
    if (!resParam.languagePath.IsEmpty() && !resParam.languageFileName.empty()) {
        FilePath languagePath = FilePathUtil::JoinFilePath(strResourcePath, resParam.languagePath);
        ReloadLanguage(languagePath, resParam.languageFileName, false);
    }
    else if (!resParam.languagePath.IsEmpty()) {
        SetLanguagePath(FilePathUtil::JoinFilePath(strResourcePath, resParam.languagePath));
    }

    //Update the states of all child controls in the windows
    if (bInvalidate) {
        std::vector<WindowPtr> windowList = Windows().GetAllWindowList();
        for (const WindowPtr& pWindow : windowList) {
            if (pWindow != nullptr) {
                Box* pBox = pWindow->GetRoot();
                if (pBox != nullptr) {
                    pBox->Invalidate();
                }
            }            
        }
    }
    return true;
}

bool GlobalManager::ReloadLanguage(const FilePath& languagePath,
                                   const DString& languageFileName,
                                   bool bInvalidate)
{
    AssertUIThread();
    ASSERT(!languageFileName.empty());
    if (languageFileName.empty()) {
        return false;
    }

    FilePath newLanguagePath = GetLanguagePath();
    if (!languagePath.IsEmpty()) {
        newLanguagePath = languagePath;
        newLanguagePath.NormalizeDirectoryPath();
    }

    //Load the multi-language file; if a resource zip package is used, load the language file from memory
    bool bReadOk = false;
    if ( (newLanguagePath.IsEmpty() || !newLanguagePath.IsAbsolutePath()) &&
         m_zipManager.IsUseZip() ) {
        std::vector<unsigned char> fileData;
        FilePath filePath = FilePathUtil::JoinFilePath(newLanguagePath, FilePath(languageFileName));
        if (m_zipManager.GetZipData(filePath, fileData)) {
            bReadOk = m_langManager.LoadStringTable(fileData);
        }
        else {
            ASSERT(!"GetZipData failed!");
        }
    }
    else {
        FilePath filePath = FilePathUtil::JoinFilePath(newLanguagePath, FilePath(languageFileName));
        bReadOk = m_langManager.LoadStringTable(filePath);
    }

    if (bReadOk) {
        //Save the language file path
        if (!newLanguagePath.IsEmpty() && (newLanguagePath != GetLanguagePath())) {
            SetLanguagePath(newLanguagePath);
        }
        //Save the language file name
        m_languageFileName = languageFileName;
    }

    ASSERT(bReadOk && "ReloadLanguage");
    if (bReadOk && bInvalidate) {
        //Refresh the UI display
        std::vector<WindowPtr> windowList = Windows().GetAllWindowList();
        for (const WindowPtr& pWindow : windowList) {
            if (pWindow == nullptr) {
                continue;
            }
            Box* pBox = pWindow->GetRoot();
            //When multi-language is supported, TextId takes priority
            if (!pWindow->GetTextId().empty()) {
                //Update the window title bar text
                pWindow->SetTextId(pWindow->GetTextId());
            }
            if (pBox != nullptr) {
                pBox->OnLanguageChanged();
                pBox->SetPos(pBox->GetPos());
            }
        }
    }
    return bReadOk;
}

bool GlobalManager::GetLanguageList(std::vector<std::pair<DString, DString>>& languageList,
                                    const DString& languageNameID) const
{
    FilePath languagePath = GetLanguagePath();
    ASSERT(!languagePath.IsEmpty());
    if (languagePath.IsEmpty()) {
        return false;
    }

    languageList.clear();
#ifdef DUILIB_BUILD_FOR_WIN
    //Windows: the path string uses DStringW::value_type, UTF16
    const std::filesystem::path path{ languagePath.ToStringW()};
#else
    //Windows: the path string uses char, UTF8
    const std::filesystem::path path{ languagePath.ToStringA() };
#endif
    if (path.is_absolute()) {
        //An absolute path, the language files are on the local disk
        for (auto const& dir_entry : std::filesystem::directory_iterator{ path }) {
            if (dir_entry.is_regular_file()) {
                languageList.push_back({ FilePath(dir_entry.path().filename()).ToString(), _T("")});
            }
        }
        if (!languageNameID.empty()) {
            for (auto& lang : languageList) {
                const DString& fileName = lang.first;
                DString& displayName = lang.second;

                FilePath filePath = FilePathUtil::JoinFilePath(languagePath, FilePath(fileName));
                ui::LangManager langManager;
                if (langManager.LoadStringTable(filePath)) {
                    displayName = langManager.GetStringViaID(languageNameID);
                }
            }
        }
    }
    else if(m_zipManager.IsUseZip()){
        //A relative path, the language files should all be inside the zip package
        std::vector<DString> fileList;
        m_zipManager.GetZipFileList(languagePath, fileList);
        for (auto const& file : fileList) {
            languageList.push_back({ file, _T("") });
        }

        if (!languageNameID.empty()) {
            for (auto& lang : languageList) {
                const DString& fileName = lang.first;
                DString& displayName = lang.second;

                FilePath filePath = FilePathUtil::JoinFilePath(languagePath, FilePath(fileName));
                std::vector<unsigned char> fileData;
                if (m_zipManager.GetZipData(filePath, fileData)) {
                    ui::LangManager langManager;
                    if (langManager.LoadStringTable(fileData)) {
                        displayName = langManager.GetStringViaID(languageNameID);
                    }
                }
            }
        }
    }
    else {
        ASSERT(false);
        return false;
    }
    return true;
}

void GlobalManager::CheckImagePath(FilePath& imageFullPath, bool& bLocalPath)
{
    imageFullPath.NormalizeFilePath();
    if (m_zipManager.IsZipResExist(imageFullPath)) {
        bLocalPath = false;
    }
    else if (imageFullPath.IsExistsFile()) {
        bLocalPath = true;
    }
    else {
        //If the file does not exist, return empty
        imageFullPath.Clear();
    }
}

bool GlobalManager::IsResInPublicPath(const FilePath& resPath) const
{
    DString resPathString = resPath.ToString();
    StringUtil::ReplaceAll(_T("\\"), _T("/"), resPathString);
    if ((resPathString.find(_T("public/")) == 0) || ((resPathString.find(_T("/public/")) == 0))) {
        return true;
    }
    return false;
}

FilePath GlobalManager::GetExistsResFullPath(const FilePath& windowResPath, const FilePath& windowXmlPath, const FilePath& resPath)
{
    bool bLocalPath = false;
    bool bResPath = false;
    return GetExistsResFullPath(windowResPath, windowXmlPath, resPath, nullptr, bLocalPath, bResPath);
}

FilePath GlobalManager::GetExistsResFullPath(const FilePath& windowResPath,
                                             const FilePath& windowXmlPath,
                                             const FilePath& resPath,
                                             const Control* pControl,
                                             bool& bLocalPath,
                                             bool& bResPath)
{
    FilePath imageFullPath = FindExistsResFullPath(windowResPath, windowXmlPath, resPath, bLocalPath, bResPath);
    if (imageFullPath.IsEmpty()) {
        //The image resource failed to load; give one chance to fix it through the callback function
        std::vector<ResNotFoundCallbackData> resNotFoundCallbacks = m_resNotFoundCallbacks;
        for (const ResNotFoundCallbackData& callbackData : resNotFoundCallbacks) {
            FilePath newWindowResPath = windowResPath;
            FilePath newWindowXmlPath = windowXmlPath;
            if (callbackData.m_callback(pControl, resPath, newWindowResPath, newWindowXmlPath)) {
                if ((newWindowResPath != windowResPath) || (newWindowXmlPath != windowXmlPath)) {
                    imageFullPath = FindExistsResFullPath(newWindowResPath, newWindowXmlPath, resPath, bLocalPath, bResPath);
                    if (!imageFullPath.IsEmpty()) {
                        //The resource was found successfully, stop the attempts
                        break;
                    }
                }
            }
        }
    }
    ASSERT(!imageFullPath.IsEmpty() && !resPath.IsEmpty() && "Image File Not Found!");
    return imageFullPath;
}

FilePath GlobalManager::FindExistsResFullPath(const FilePath& windowResPath,
                                              const FilePath& windowXmlPath,
                                              const FilePath& resPath,
                                              bool& bLocalPath,
                                              bool& bResPath)
{
    bLocalPath = true;
    bResPath = true;
    ASSERT(!resPath.IsEmpty());
    if (resPath.IsEmpty()) {
        return resPath;
    }
    FilePath imageFullPath;
#ifdef DUILIB_BUILD_FOR_WIN
    const bool bOSWindows = true;
#else
    const bool bOSWindows = false;
#endif

    bool bWindows = bOSWindows;//Avoid compilation warnings
    if (bWindows && resPath.IsAbsolutePath()) {
        //An absolute path on the Windows platform: an external file
        imageFullPath = resPath;
        imageFullPath.NormalizeFilePath();
        if (imageFullPath.IsExistsFile()) {
            bLocalPath = true;
            bResPath = false;
        }
        else {
            //If the file does not exist, return empty
            imageFullPath.Clear();
        }
    }
    else {
        //A relative path: first search the resource directory of the window (high hit rate)
        const FilePath windowResFullPath = FilePathUtil::JoinFilePath(GlobalManager::GetResourcePath(), windowResPath);        
        if (IsResInPublicPath(resPath)) {
            //Match from the public directory first
            imageFullPath = FilePathUtil::JoinFilePath(GlobalManager::GetResourcePath(), resPath);
            CheckImagePath(imageFullPath, bLocalPath);
        }
        if (imageFullPath.IsEmpty()) {
            //Search the directory specified by the window
            imageFullPath = FilePathUtil::JoinFilePath(windowResFullPath, resPath);
            CheckImagePath(imageFullPath, bLocalPath);
        }
        if (imageFullPath.IsEmpty()) {
            //Then search the public directory (high hit rate)
            imageFullPath = FilePathUtil::JoinFilePath(GlobalManager::GetResourcePath(), resPath);
            CheckImagePath(imageFullPath, bLocalPath);
        }
        if (imageFullPath.IsEmpty() && !windowXmlPath.IsEmpty()) {
            //Finally search the directory where the XML file is located
            const FilePath windowXmlFullPath = FilePathUtil::JoinFilePath(windowResFullPath, windowXmlPath);
            imageFullPath = FilePathUtil::JoinFilePath(windowXmlFullPath, resPath);
            CheckImagePath(imageFullPath, bLocalPath);

            if (imageFullPath.IsEmpty()) {
                const FilePath xmlFullPath = FilePathUtil::JoinFilePath(GlobalManager::GetResourcePath(), windowXmlPath);
                imageFullPath = FilePathUtil::JoinFilePath(xmlFullPath, resPath);
                CheckImagePath(imageFullPath, bLocalPath);
            }
        }
        if (!bWindows && imageFullPath.IsEmpty() && resPath.IsAbsolutePath()) {
            //Note: non-Windows absolute paths have the same form as relative paths, both starting with '/', so this check is placed last
            imageFullPath = resPath;
            imageFullPath.NormalizeFilePath();
            if (imageFullPath.IsExistsFile()) {
                bLocalPath = true;
                bResPath = false;
            }
            else {
                //If the file does not exist, return empty
                imageFullPath.Clear();
            }
        }
    }
    return imageFullPath;
}

void GlobalManager::AddResNotFoundCallback(ResNotFoundCallback callback, size_t callbackId)
{
    if (callback != nullptr) {
        ResNotFoundCallbackData data;
        data.m_callback = callback;
        data.m_callbackId = callbackId;
        m_resNotFoundCallbacks.push_back(data);
    }
}

void GlobalManager::RemoveResNotFoundCallback(size_t callbackId)
{
    auto iter = m_resNotFoundCallbacks.begin();
    while (iter != m_resNotFoundCallbacks.end()) {
        if (iter->m_callbackId == callbackId) {
            iter = m_resNotFoundCallbacks.erase(iter);
        }
        else {
            ++iter;
        }
    }
}

void GlobalManager::RemoveAllImages()
{
    AssertUIThread();
    std::vector<WindowPtr> windowList = Windows().GetAllWindowList();
    for (const WindowPtr& pWindow : windowList) {
        if (pWindow != nullptr) {
            Box* pBox = pWindow->GetRoot();
            if (pBox != nullptr) {
                pBox->ClearImageCache();
            }
        }
    }
    m_imageManager.RemoveAllImages();
}

IRenderFactory* GlobalManager::GetRenderFactory()
{
    return m_renderFactory.get();
}

void GlobalManager::AddClass(const DString& strClassName, const DString& strControlAttrList)
{
    AssertUIThread();
    ASSERT(!strClassName.empty() && !strControlAttrList.empty());
    if (!strClassName.empty() && !strControlAttrList.empty()) {
        m_globalClass[strClassName] = strControlAttrList;
    }    
}

DString GlobalManager::GetClassAttributes(const DString& strClassName) const
{
    AssertUIThread();
    auto it = m_globalClass.find(strClassName);
    if (it != m_globalClass.end()) {
        return it->second;
    }
    return DString();
}

void GlobalManager::RemoveAllClasss()
{
    AssertUIThread();
    m_globalClass.clear();
}

ColorManager& GlobalManager::Color()
{
    return m_colorManager;
}

FontManager& GlobalManager::Font()
{
    return m_fontManager;
}

ImageManager& GlobalManager::Image()
{
    return m_imageManager;
}

ImageDecoderFactory& GlobalManager::ImageDecoders()
{
    return m_imageDecoderFactory;
}

IconManager& GlobalManager::Icon()
{
    return m_iconManager;
}

ZipManager& GlobalManager::Zip()
{
    return m_zipManager;
}

DpiManager& GlobalManager::Dpi()
{
    return m_dpiManager;
}

TimerManager& GlobalManager::Timer()
{
    return m_timerManager;
}

ThreadManager& GlobalManager::Thread()
{
    return m_threadManager;
}

LangManager& GlobalManager::Lang()
{
    return m_langManager;
}

CursorManager& GlobalManager::Cursor()
{
    return m_cursorManager;
}

WindowManager& GlobalManager::Windows()
{
    return m_windowManager;
}

Box* GlobalManager::CreateBox(Window* pWindow, const FilePath& strXmlPath, CreateControlCallback callback)
{
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return nullptr;
    }
    Box* pBox = nullptr;
    WindowBuilder builder;
    if (builder.ParseXmlFile(strXmlPath, pWindow->GetResourcePath())) {
        Control* pControl = builder.CreateControls(pWindow, callback);
        ASSERT(pControl != nullptr);
        if (pControl != nullptr) {
            pBox = builder.ToBox(pControl);
            ASSERT(pBox != nullptr);
            if (pBox == nullptr) {
                delete pControl;
                pControl = nullptr;
            }
        }
    }
    return pBox;
}

Box* GlobalManager::CreateBoxWithCache(Window* pWindow, const FilePath& strXmlPath, CreateControlCallback callback)
{
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return nullptr;
    }
    Box* pBox = nullptr;
    auto it = m_builderMap.find(strXmlPath);
    if (it == m_builderMap.end()) {
        WindowBuilder* builder = new WindowBuilder();
        if (builder->ParseXmlFile(strXmlPath, pWindow->GetResourcePath())) {
            Control* pControl = builder->CreateControls(pWindow, callback);
            ASSERT(pControl != nullptr);
            if (pControl != nullptr) {
                pBox = builder->ToBox(pControl);
                ASSERT(pBox != nullptr);
                if (pBox == nullptr) {
                    delete pControl;
                    pControl = nullptr;
                }
            }            
        }        
        if (pBox != nullptr) {
            m_builderMap[strXmlPath].reset(builder);
        }
        else {
            delete builder;
            builder = nullptr;
        }
    }
    else {
        Control* pControl = it->second->CreateControls(pWindow, callback);
        ASSERT(pControl != nullptr);
        if (pControl != nullptr) {
            pBox = it->second->ToBox(pControl);
            ASSERT(pBox != nullptr);
            if (pBox == nullptr) {
                delete pControl;
                pControl = nullptr;
            }
        }
    }
    ASSERT(pBox != nullptr);
    return pBox;
}

bool GlobalManager::FillBox(Box* pUserDefinedBox, const FilePath& strXmlPath, CreateControlCallback callback)
{
    bool bRet = false;
    ASSERT(pUserDefinedBox != nullptr);
    if (pUserDefinedBox != nullptr) {
        Window* pWindow = pUserDefinedBox->GetWindow();
        ASSERT(pWindow != nullptr);
        if (pWindow == nullptr) {
            return false;
        }
        WindowBuilder winBuilder;
        if (winBuilder.ParseXmlFile(strXmlPath, pWindow->GetResourcePath())) {
            Control* pControl = winBuilder.CreateControls(pWindow, callback, nullptr, pUserDefinedBox);
            Box* box = winBuilder.ToBox(pControl);
            bRet = box != nullptr;
        }
    }
    return bRet;
}

bool GlobalManager::FillBoxWithCache(Box* pUserDefinedBox, const FilePath& strXmlPath, CreateControlCallback callback)
{
    ASSERT(pUserDefinedBox != nullptr);
    if (pUserDefinedBox == nullptr) {
        return false;
    }
    Window* pWindow = pUserDefinedBox->GetWindow();
    ASSERT(pWindow != nullptr); //The DPI awareness feature requires the window to be associated first
    if (pWindow == nullptr) {
        return false;
    }    
    Box* pBox = nullptr;
    auto it = m_builderMap.find(strXmlPath);
    if (it == m_builderMap.end()) {
        WindowBuilder* winBuilder = new WindowBuilder();
        if (winBuilder->ParseXmlFile(strXmlPath, pWindow->GetResourcePath())) {
            Control* pControl = winBuilder->CreateControls(pWindow, callback, nullptr, pUserDefinedBox);
            ASSERT(pControl != nullptr);
            if (pControl != nullptr) {
                pBox = winBuilder->ToBox(pControl);
                if (pBox == nullptr) {
                    delete pControl;
                    pControl = nullptr;
                }
            }
        }        
        if (pBox != nullptr) {
            m_builderMap[strXmlPath].reset(winBuilder);
        }
        else {
            delete winBuilder;
            winBuilder = nullptr;
        }
    }
    else {
        Control* pControl = it->second->CreateControls(pWindow, callback, nullptr, pUserDefinedBox);
        ASSERT(pControl != nullptr);
        if (pControl != nullptr) {
            pBox = it->second->ToBox(pControl);
            ASSERT(pBox != nullptr);
            if (pBox == nullptr) {
                delete pControl;
                pControl = nullptr;
            }
        }        
    }
    ASSERT(pUserDefinedBox == pBox);
    return (pBox != nullptr);
}

Control* GlobalManager::CreateControl(const DString& strControlName)
{
    Control* pControl = nullptr;
    for (CreateControlCallback pfnCreateControlCallback : m_pfnCreateControlCallbackList) {
        if (pfnCreateControlCallback != nullptr) {
            pControl = pfnCreateControlCallback(strControlName);
            if (pControl != nullptr) {
                break;
            }
        }
    }
    return pControl;
}

void GlobalManager::AddCreateControlCallback(const CreateControlCallback& pfnCreateControlCallback)
{
    if (pfnCreateControlCallback != nullptr) {
        m_pfnCreateControlCallbackList.push_back(pfnCreateControlCallback);
    }
}

Box* GlobalManager::CreateBoxForXmlPreview(Window* pWindow, const FilePath& xmlFilePath, XmlPreviewAttributes& xmlPreviewAttributes)
{
    const std::vector<unsigned char> xmlFileData;
    return CreateBoxForXmlPreview(pWindow, xmlFileData, xmlPreviewAttributes, xmlFilePath);
}

Box* GlobalManager::CreateBoxForXmlPreview(Window* pWindow,
                                           const std::vector<unsigned char>& xmlFileData,
                                           XmlPreviewAttributes& xmlPreviewAttributes,
                                           const FilePath& xmlFilePath)
{
    ASSERT(pWindow != nullptr);
    if (pWindow == nullptr) {
        return nullptr;
    }
    bool bParseXmlResult = false;
    Box* pBox = nullptr;
    WindowBuilder builder;
    if (!xmlFileData.empty()) {
        if (builder.ParseXmlData(xmlFileData, xmlFilePath)) {
            bParseXmlResult = true;
        }
    }
    else if (!xmlFilePath.IsEmpty()) {
        if (builder.ParseXmlFile(xmlFilePath, pWindow->GetResourcePath())) {
            bParseXmlResult = true;
        }
    }
    if (bParseXmlResult) {
        Control* pControl = builder.CreateControls(pWindow, nullptr);
        ASSERT(pControl != nullptr);
        if (pControl != nullptr) {
            pBox = builder.ToBox(pControl);
            ASSERT(pBox != nullptr);
            if (pBox == nullptr) {
                delete pControl;
                pControl = nullptr;
            }
        }
        if (pBox != nullptr) {
            xmlPreviewAttributes.m_windowAttributes.clear();
            builder.ParseWindowAttributes(xmlPreviewAttributes.m_windowAttributes);
            xmlPreviewAttributes.m_windowClassList = builder.GetWindowClassList();
            xmlPreviewAttributes.m_windowTextColorList = builder.GetWindowTextColorList();
            xmlPreviewAttributes.m_globalFontIdList = builder.GetGlobalFontIdList();
        }
    }
    return pBox;
}

bool GlobalManager::IsInUIThread() const
{
    return (m_dwUiThreadId == std::this_thread::get_id()) ? true : false;
}

void GlobalManager::AssertUIThread() const
{
    ASSERT(m_dwUiThreadId == std::this_thread::get_id());
}

void GlobalManager::AddAtExitFunction(std::function<void()> atExitFunction)
{
    ASSERT(atExitFunction != nullptr);
    if (atExitFunction != nullptr) {
        m_atExitFunctions.push_back(atExitFunction);
    }
}

void GlobalManager::SetAnimationEnabled(bool bEnable)
{
    m_bAnimationEnabled = bEnable;
}

bool GlobalManager::IsAnimationEnabled() const
{
    return m_bAnimationEnabled;
}

} // namespace ui
