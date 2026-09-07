#include "dui/Utils/ProcessSingleton.h"
#include "dui/Utils/ProcessSingletonData.h"
#include "dui/Utils/StringConvert.h"
#include <chrono>

namespace ui
{

// SDL-only stub: no cross-process singleton enforcement (always single instance check returns false)
class ProcessSingletonImpl : public ProcessSingleton
{
public:
    explicit ProcessSingletonImpl(const std::string& strAppName) : ProcessSingleton(strAppName) {}
    virtual ~ProcessSingletonImpl() { m_bRunning = false; if (m_thListener.joinable()) m_thListener.join(); }
protected:
    void InitializePlatformComponents() override {}
    void CleanupPlatformComponents() override {}
    bool PlatformCheckInstance() override { return false; }
    bool PlatformSendData(const std::string&) override { return false; }
    void PlatformListen() override {
        InitializePlatformComponents();
        while (m_bRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        CleanupPlatformComponents();
    }
};

ProcessSingleton::ProcessSingleton(const std::string& strAppName):
    m_strAppName(strAppName), 
    m_bRunning(false) 
{
}

ProcessSingleton::~ProcessSingleton()
{
    m_bRunning = false;
}

bool ProcessSingleton::IsAnotherInstanceRunning()
{
    return PlatformCheckInstance();
}

bool ProcessSingleton::SendArgumentsToExistingInstance(const std::vector<std::string>& vecArgs)
{
    std::string strData;
    try {
        strData = ProcessSingletonData::SerializeArguments(vecArgs);
    }
    catch (const std::exception& ex) {
        LogError("Argument serialization failed: " + std::string(ex.what()));
        return false;
    }
    return PlatformSendData(strData);
}

void ProcessSingleton::StartListener(OnAlreadyRunningAppRelaunchEvent fnCallback)
{
    ASSERT(!m_bRunning);
    if (m_bRunning) {
        return;
    }
    m_fnCallback = fnCallback;
    m_bRunning = true;
    m_thListener = std::thread(&ProcessSingleton::PlatformListen, this);
}

void ProcessSingleton::LogError(const std::string& /*strMessage*/)
{
}

void ProcessSingleton::OnAlreadyRunningAppRelaunch(const std::vector<std::string>& args)
{
    if (m_fnCallback != nullptr) {
        DString line;
        std::vector<DString> argumentList;
        for (const std::string& v : args) {
            line = StringConvert::UTF8ToT(v);
            if (!line.empty()) {
                argumentList.push_back(line);
            }
        }
        m_fnCallback(argumentList);
    }
}

std::unique_ptr<ProcessSingleton> ProcessSingleton::Create(const DString& strAppName)
{
    ASSERT(!strAppName.empty());
    if (strAppName.empty()) {
        return nullptr;
    }
    return std::make_unique<ProcessSingletonImpl>(StringConvert::TToUTF8(strAppName));
}

} //namespace ui
