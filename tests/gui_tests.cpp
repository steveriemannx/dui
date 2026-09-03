#include "dui/Core/GlobalManager.h"
#include "dui/Core/MessageLoop_MacOS.h"
#include "dui/Core/Window.h"

#include <cassert>

namespace {

class TestWindow final : public ui::Window {
protected:
    DString GetSkinFile() override
    {
        return DUI_T("<Window size='160,80'><VBox><Label text='GUI test'/></VBox></Window>");
    }

    void OnInitWindow() override
    {
        PostQuitMsgWhenClosed(true);
        ShowWindow(ui::kSW_SHOW_NORMAL);
        Close();
    }
};

} // namespace

int main()
{
    const ui::FilePath root(DUI_T(DUI_TEST_SOURCE_ROOT));
    ui::LocalFilesResParam resources(ui::FilePathUtil::JoinFilePath(
        root, ui::FilePath(DUI_T("resources"))));
    assert(ui::GlobalManager::Instance().Startup(resources));

    auto* window = new TestWindow();
    assert(window->CreateWnd(nullptr, ui::WindowCreateParam(DUI_T("dui GUI test"), true)));
    ui::MessageLoop_MacOS loop;
    loop.Run(nullptr);
    ui::GlobalManager::Instance().Shutdown();
    return 0;
}
