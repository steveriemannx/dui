#include "dui/Core/Callback.h"
#include "dui/Core/DpiManager.h"
#include "dui/Core/FrameworkThread.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/TimerManager.h"
#include "dui/Core/UiMargin.h"
#include "dui/Core/UiRect.h"
#include "dui/Core/UiSize.h"
#include "dui/Core/WindowBuilder.h"
#include "dui/Core/Window.h"
#include "dui/Core/Box.h"
#include "dui/Core/SharePtr.h"
#include "dui/Image/ImageDecoder_PNG.h"
#include "dui/Image/ImageDecoder_JPEG.h"
#include "dui/Image/ImageDecoder_WEBP.h"
#include "dui/Image/ImageDecoder_SVG.h"
#include "dui/Image/ImageDecoderFactory.h"
#include "dui/Utils/AttributeUtil.h"
#include "dui/Utils/FileUtil.h"
#include "dui/Utils/FilePathUtil.h"
#include "dui/Utils/StringConvert.h"
#include "dui/Utils/StringUtil.h"

#include <cassert>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

#if defined(DUI_BUILD_FOR_MACOS)
#include "dui/Core/MessageLoop_MacOS.h"
#endif

#define DUI_TEST_SOURCE_ROOT_TEXT_IMPL(value) DUI_T(value)
#define DUI_TEST_SOURCE_ROOT_TEXT(value) DUI_TEST_SOURCE_ROOT_TEXT_IMPL(value)

namespace {

ui::ImageDecodeParam MemoryImage(const std::vector<uint8_t>& data)
{
    ui::ImageDecodeParam param;
    param.m_bAssertEnabled = false;
    param.m_pFileData = std::make_shared<std::vector<uint8_t>>(data);
    return param;
}

class Counted final : public ui::RefCount {
public:
    explicit Counted(int* destructions) : m_destructions(destructions) {}

    ~Counted() override { ++*m_destructions; }

private:
    int* m_destructions;
};

void TestSharePtr()
{
    int destructions = 0;
    {
        ui::SharePtr<Counted> first(new Counted(&destructions));
        assert(first);
        assert(first->Unique());

        ui::SharePtr<Counted> second(first);
        assert(!first->Unique());
        second.reset(second.get()); // Self-reset must not release the object.
        first.reset();
        assert(second);
        second.reset();
    }
    assert(destructions == 1);
}

class CallbackOwner final : public ui::SupportWeakCallback {
};

void TestWeakCallback()
{
    int calls = 0;
    ui::WeakCallback<std::function<int(int)>> callback(
        std::weak_ptr<ui::WeakFlag>(),
        [](int value) { return value + 1; });
    assert(callback.Expired());
    assert(callback(4) == 0);

    {
        CallbackOwner owner;
        callback = owner.ToWeakCallback(std::function<int(int)>(
            [&calls](int value) {
                ++calls;
                return value + 1;
            }));
        assert(!callback.Expired());
        assert(callback(4) == 5);
    }
    assert(callback.Expired());
    assert(callback(4) == 0);
    assert(calls == 1);
}

class TestDecoder final : public ui::IImageDecoder {
public:
    DString GetFormatName() const override { return DUI_T("test"); }
    bool CanDecode(const DString&) const override { return false; }
    bool CanDecode(const uint8_t*, size_t) const override { return false; }
    std::unique_ptr<ui::IImage> LoadImageData(
        const ui::ImageDecodeParam&) override { return nullptr; }
};

void TestImageDecoderFactory()
{
    ui::ImageDecoderFactory factory;
    auto decoder = std::make_shared<TestDecoder>();
    assert(factory.AddImageDecoder(decoder));
    assert(!factory.AddImageDecoder(decoder));
    assert(factory.RemoveImageDecoder(decoder));
    assert(!factory.RemoveImageDecoder(decoder));

    factory.AddImageDecoder(std::make_shared<ui::ImageDecoder_SVG>());
    ui::ImageDecodeParam svgParam;
    svgParam.m_bAssertEnabled = false;
    svgParam.m_pFileData = std::make_shared<std::vector<uint8_t>>(
        std::vector<uint8_t>{'<', 's', 'v', 'g', ' ', 'w', 'i', 'd', 't', 'h', '=', '"', '3', '"',
                             ' ', 'h', 'e', 'i', 'g', 'h', 't', '=', '"', '2', '"', '/', '>'});
    auto svg = factory.LoadImageData(svgParam);
    assert(svg);
    assert(svg->GetImageType() == ui::ImageType::kImageSvg);
    assert(svg->GetWidth() == 3 && svg->GetHeight() == 2);

    ui::ImageDecodeParam invalid;
    invalid.m_bAssertEnabled = false;
    invalid.m_pFileData = std::make_shared<std::vector<uint8_t>>(
        std::vector<uint8_t>{'n', 'o', 't', '-', 'a', 'n', '-', 'i', 'm', 'a', 'g', 'e'});
    assert(factory.LoadImageData(invalid) == nullptr);
}

void TestRealImageDecoding()
{
    ui::ImageDecoderFactory factory;
    assert(factory.AddImageDecoder(std::make_shared<ui::ImageDecoder_PNG>()));
    assert(factory.AddImageDecoder(std::make_shared<ui::ImageDecoder_WEBP>()));
#ifdef DUI_IMAGE_SUPPORT_JPEG_TURBO
    assert(factory.AddImageDecoder(std::make_shared<ui::ImageDecoder_JPEG>()));
#endif

    for (const DString& relativePath : {
        DUI_T("resources/themes/default/render/autumn.png"),
        DUI_T("resources/themes/default/render/webp_test2.webp")
#ifdef DUI_IMAGE_SUPPORT_JPEG_TURBO
        , DUI_T("resources/themes/default/render/jpg_test.jpg")
#endif
    }) {
        const ui::FilePath path = ui::FilePathUtil::JoinFilePath(
            ui::FilePath(DUI_TEST_SOURCE_ROOT_TEXT(DUI_TEST_SOURCE_ROOT)), ui::FilePath(relativePath));
        std::vector<uint8_t> fileData;
        assert(ui::FileUtil::ReadFileData(path, fileData));
        ui::ImageDecodeParam param = MemoryImage(fileData);
        auto image = factory.LoadImageData(param);
        assert(image && image->GetWidth() > 0 && image->GetHeight() > 0);
    }
}

void TestStringAndAttributes()
{
    // Trim has two overloads with different semantics: Trim(const char*) returns a
    // copy, Trim(std::string&) trims in place (it returns std::string&). Passing an
    // lvalue std::string picks the in-place one, so both are checked here.
    const std::string strPadded = "  AbC AbC  ";
    assert(ui::StringUtil::Trim("  AbC AbC  ") == "AbC AbC");   // by value
    std::string text = strPadded;
    assert(ui::StringUtil::Trim(text) == "AbC AbC");
    assert(text == "AbC AbC");                                   // in place
    assert(ui::StringUtil::ReplaceAll(std::string("AbC"), std::string("x"), text) == 2);
    assert(text == "x x");
    assert(ui::StringUtil::MakeLowerString("HeLLo") == "hello");
    assert(ui::StringUtil::IsEqualNoCase("abc", "ABC"));
    assert(ui::StringUtil::StringToInt32("42") == 42);
    assert(ui::StringUtil::StringToDouble("2.5") == 2.5);

    // C++20 made u8"" a char8_t array, which no longer converts to std::string.
    // This source file is UTF-8, so a plain literal carries the same bytes.
    const std::string utf8 = "hello 世界";
    auto wide = ui::StringConvert::UTF8ToWString(utf8);
    assert(ui::StringConvert::WStringToUTF8(wide) == utf8);
    assert(ui::StringConvert::UTF32ToUTF8(ui::StringConvert::UTF8ToUTF32(utf8)) == utf8);

    ui::UiSize size;
    ui::AttributeUtil::ParseSizeValue("800,500", size);
    assert(size == ui::UiSize(800, 500));
    ui::UiRect rect;
    ui::AttributeUtil::ParseRectValue("1,2,30,40", rect);
    assert(rect == ui::UiRect(1, 2, 30, 40));
    ui::UiMargin margin;
    ui::AttributeUtil::ParseMarginValue("1,2,3,4", margin);
    assert(margin == ui::UiMargin(1, 2, 3, 4));
    std::vector<std::pair<DString, DString>> attributes;
    ui::AttributeUtil::ParseAttributeList(DUI_T("text='hello world' width='20'"), DUI_T('\''), attributes);
    assert(attributes.size() == 2);
    assert(attributes[0].first == DUI_T("text") && attributes[0].second == DUI_T("hello world"));
}

void TestGeometryAndDpi()
{
    ui::UiRect rect(0, 0, 10, 20);
    rect.Offset(2, 3);
    rect.Inflate(ui::UiMargin(1, 2, 3, 4));
    assert(rect == ui::UiRect(1, 1, 15, 27));
    assert(rect.ContainsPt(1, 1) && !rect.ContainsPt(15, 27));
    assert(rect.Width() == 14 && rect.Height() == 26);
    ui::UiRect intersection;
    assert(ui::UiRect::Intersect(intersection, rect, ui::UiRect(5, 5, 20, 30)));
    assert(intersection == ui::UiRect(5, 5, 15, 27));

    ui::UiSize invalid(-1, 2);
    assert(!invalid.IsValid());
    invalid.Validate();
    assert(invalid == ui::UiSize(0, 2));
    ui::UiMargin negative(-1, 2, -3, 4);
    negative.Validate();
    assert(negative == ui::UiMargin(0, 2, 0, 4));

    ui::DpiManager dpi;
    dpi.SetDisplayScale(1.5f, 1.0f);
    assert(dpi.GetDisplayScaleFactor() == 150);
    assert(dpi.GetScaleInt(10) == 15);
    assert(dpi.GetUnscaleInt(15) == 10);
    assert(dpi.GetScaleSize(ui::UiSize(10, 20)) == ui::UiSize(15, 30));
    assert(dpi.GetScaleRect(ui::UiRect(2, 4, 12, 14), 100) == ui::UiRect(3, 6, 18, 21));
    assert(ui::DpiManager::MulDiv(10, 3, 2) == 15);
    assert(ui::DpiManager::MulDiv(10, 3, 0) == -1);
}

void TestFilePathAndXml()
{
    const ui::FilePath root(DUI_TEST_SOURCE_ROOT_TEXT(DUI_TEST_SOURCE_ROOT));
    assert(root.IsAbsolutePath() && root.IsExistsDirectory());
    ui::FilePath global = ui::FilePathUtil::JoinFilePath(
        root, ui::FilePath(DUI_T("resources/themes/default/global.xml")));
    // FilePath::GetFileExtension() keeps the dot -- std::filesystem semantics, and
    // NativeWindow_Windows.cpp compares its result against ".ico". The static
    // FilePathUtil::GetFileExtension() strips it, and the image decoders rely on
    // that. The two are correct as-is but are not interchangeable.
    assert(global.IsExistsFile() && global.GetFileExtension() == DUI_T(".xml"));
    assert(global.GetFileName() == DUI_T("global.xml"));
    assert(global.GetParentPath().IsExistsDirectory());
    assert(ui::FilePathUtil::NormalizeFilePath(DUI_T("a/./b/../c")) == DUI_T("a/c"));
    assert(ui::FilePathUtil::GetFileExtension(DUI_T("icon.PNG")) == DUI_T("PNG"));

    ui::WindowBuilder builder;
    assert(builder.ParseXmlData(DUI_T("<Window caption='Title' width='100' height='50'><Label text='Hello'/></Window>")));
    std::map<DString, DString> windowAttributes;
    assert(builder.ParseWindowAttributes(windowAttributes));
    assert(windowAttributes[DUI_T("caption")] == DUI_T("Title"));
    assert(windowAttributes[DUI_T("width")] == DUI_T("100"));
    assert(!builder.ParseXmlData(DUI_T("<Window>")));
}

void TestResourceFileDecode()
{
    const ui::FilePath root(DUI_TEST_SOURCE_ROOT_TEXT(DUI_TEST_SOURCE_ROOT));
    ui::ImageDecoderFactory factory;
    factory.AddImageDecoder(std::make_shared<ui::ImageDecoder_PNG>());
    factory.AddImageDecoder(std::make_shared<ui::ImageDecoder_WEBP>());
#ifdef DUI_IMAGE_SUPPORT_JPEG_TURBO
    factory.AddImageDecoder(std::make_shared<ui::ImageDecoder_JPEG>());
#endif

    for (const DString& relativePath : {
        DUI_T("resources/themes/default/render/autumn.png"),
        DUI_T("resources/themes/default/render/webp_test2.webp")
#ifdef DUI_IMAGE_SUPPORT_JPEG_TURBO
        , DUI_T("resources/themes/default/render/jpg_test.jpg")
#endif
    }) {
        const ui::FilePath imagePath = ui::FilePathUtil::JoinFilePath(root, ui::FilePath(relativePath));
        std::vector<uint8_t> fileData;
        assert(ui::FileUtil::ReadFileData(imagePath, fileData));
        assert(fileData.size() > 16);
        ui::ImageDecodeParam param = MemoryImage(fileData);
        auto image = factory.LoadImageData(param);
        assert(image && image->GetWidth() > 0 && image->GetHeight() > 0);
    }
}

class TestThread final : public ui::FrameworkThread {
public:
    TestThread() : FrameworkThread(DUI_T("core-test"), ui::kThreadNone) {}
};

void TestFrameworkThread()
{
    ui::GlobalManager::Instance().Timer().Initialize(nullptr);
    TestThread thread;
    assert(!thread.IsRunning());
    int calls = 0;
    const size_t canceled = thread.PostTask([&calls]() { ++calls; });
    assert(canceled > 0 && thread.CancelTask(canceled));
    assert(!thread.CancelTask(canceled));
    assert(thread.Start() && thread.IsRunning());
    assert(!thread.Start());
    const size_t task = thread.PostTask([&calls]() { ++calls; });
    assert(task > canceled);
    for (int i = 0; i < 100 && calls == 0; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    assert(calls == 1);
    assert(thread.Stop() && !thread.IsRunning());
    assert(thread.Stop());
    const size_t repeated = thread.PostRepeatedTask([]() {}, 100, 2);
    assert(repeated > 0 && thread.CancelTask(repeated));
    ui::GlobalManager::Instance().Timer().Clear();
}

class TimerOwner final : public ui::SupportWeakCallback {};

void TestTimerManager()
{
    ui::TimerManager timers;
    timers.Initialize(nullptr);
    TimerOwner owner;
    int calls = 0;
    auto weak = owner.GetWeakFlag();
    const size_t one = timers.AddTimer(weak, [&calls]() { ++calls; }, 10, 1);
    const size_t repeated = timers.AddTimer(weak, [&calls]() { ++calls; }, 10, 3);
    assert(one > 0 && repeated > one);
    timers.RemoveTimer(one);
    timers.RemoveTimer(one);
    timers.Clear();
    assert(calls == 0);
    assert(timers.AddTimer(weak, [&calls]() { ++calls; }, 10, 2) > 0);
    timers.Clear();
    owner.GetWeakFlag().reset();
}

void TestGlobalManager(bool failure)
{
    ui::GlobalManager& global = ui::GlobalManager::Instance();
    if (!failure) {
        const ui::FilePath sourceRoot(DUI_TEST_SOURCE_ROOT_TEXT(DUI_TEST_SOURCE_ROOT));
        ui::LocalFilesResParam resources(ui::FilePathUtil::JoinFilePath(
            sourceRoot, ui::FilePath(DUI_T("resources"))));
        assert(global.Startup(resources));
        assert(global.GetRenderFactory() != nullptr);
        assert(global.GetResourcePath().IsExistsDirectory());

        ui::Window window;
        window.SetResourcePath(global.GetResourcePath());
        const ui::FilePath layout = ui::FilePathUtil::JoinFilePath(
            sourceRoot, ui::FilePath(DUI_T("resources/themes/default/move_control/app_item.xml")));
        ui::Box* box = global.CreateBox(&window, layout);
        assert(box != nullptr && box->GetItemCount() == 2);
        delete box;

        assert(!global.Startup(resources));
        global.Shutdown();
        assert(global.Startup(resources));
    }
    else {
        uint8_t badData[16] = {};
        ui::MemoryResParam bad(badData, sizeof(badData));
        assert(!global.Startup(bad));
        assert(global.GetRenderFactory() == nullptr);
    }
    global.Shutdown();
    assert(global.GetRenderFactory() == nullptr);
    assert(global.GetResourcePath().IsEmpty());
}

void TestTimerMessageLoop()
{
#if defined(DUI_BUILD_FOR_MACOS)
    ui::TimerManager timers;
    timers.Initialize(nullptr);
    TimerOwner owner;
    int calls = 0;
    bool terminate = false;
    timers.AddTimer(owner.GetWeakFlag(), [&]() {
        ++calls;
        terminate = true;
        ui::MessageLoop_MacOS::PostNoneEvent();
    }, 5, 1);
    ui::MessageLoop_MacOS loop;
    loop.RunUserLoop(terminate);
    assert(calls == 1);
    timers.Clear();
#endif
}

} // namespace

int main(int argc, char** argv)
{
    if (argc == 2 && std::string(argv[1]) == "--global-startup") {
        TestGlobalManager(false);
        return 0;
    }
    if (argc == 2 && std::string(argv[1]) == "--global-failure") {
        TestGlobalManager(true);
        return 0;
    }
    if (argc == 2 && std::string(argv[1]) == "--timer-loop") {
        TestTimerMessageLoop();
        return 0;
    }
    TestSharePtr();
    TestWeakCallback();
    TestImageDecoderFactory();
    TestRealImageDecoding();
    TestStringAndAttributes();
    TestGeometryAndDpi();
    TestFilePathAndXml();
    TestResourceFileDecode();
    TestFrameworkThread();
    TestTimerManager();
    return 0;
}
