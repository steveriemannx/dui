// The Release configuration defines NDEBUG, which would compile every assert() in
// this file away; see the same note in core_tests.cpp. Undo it before any header
// processes <cassert>.
#undef NDEBUG

#include "dui/Core/GlobalManager.h"
#include "dui/Core/MessageLoop_MacOS.h"
#include "dui/Core/Window.h"

#include <cassert>

#define DUI_TEST_SOURCE_ROOT_TEXT_IMPL(value) (value)
#define DUI_TEST_SOURCE_ROOT_TEXT(value) DUI_TEST_SOURCE_ROOT_TEXT_IMPL(value)

namespace {

class TestWindow final : public ui::Window {
protected:
    std::string GetSkinFile() override
    {
        return "<Window size='160,80'><VBox><Label text='GUI test'/></VBox></Window>";
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
    const ui::FilePath root(DUI_TEST_SOURCE_ROOT_TEXT(DUI_TEST_SOURCE_ROOT));
    ui::LocalFilesResParam resources(ui::FilePathUtil::JoinFilePath(
        root, ui::FilePath("resources")));
    assert(ui::GlobalManager::Instance().Startup(resources));

    auto* window = new TestWindow();
    assert(window->CreateWnd(nullptr, ui::WindowCreateParam("dui GUI test", true)));
    ui::MessageLoop_MacOS loop;
    loop.Run(nullptr);
    ui::GlobalManager::Instance().Shutdown();
    return 0;
}
