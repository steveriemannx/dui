// Regression coverage for the framework paths the suite never reached: event
// dispatch, the weak-reference machinery, the Box control tree, the layout
// engine, and point hit-testing. production.md records these as "zero coverage".
//
// Two conventions, both copied from core_tests.cpp deliberately:
//
//   * `#undef NDEBUG` comes first. CMake defines NDEBUG for a Release build
//     (CMAKE_CXX_FLAGS_RELEASE), which would compile every assert() below away and
//     leave the suite green while testing nothing at all.
//   * The library's own ASSERT() is NDEBUG-based too, so it is live in a Debug
//     build. Control::~Control() reaches GlobalManager::AssertUIThread(), whose
//     ASSERT compares against a thread id that only Startup() ever sets, and
//     Layout::DeflatePadding / Control::FindControl assert on their preconditions
//     as well. Every case therefore runs inside a Startup/Shutdown pair, even
//     though nothing here opens a window.
//
// Each group is registered as its own CTest entry (see tests/CMakeLists.txt) so
// that one aborting assert cannot hide the results of the others.
//
// One limit worth stating: the use-after-free in EventSource::operator() that the
// dispatch group exists to pin is a *read* of the freed callback list, and it is
// invisible to assertions -- nothing observable changes if it happens. It is
// detectable only by a sanitizer, and no build configuration in this repository
// enables one (production.md, "Sanitizers: zero matches repo-wide"). What the
// dispatch cases below do assert is the behavior that surrounds it: dispatch
// stops, later callbacks are abandoned, and nothing is called once the sender has
// died. To catch the read itself, configure the library and the tests with
// -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer" plus the same in
// CMAKE_C_FLAGS and CMAKE_EXE_LINKER_FLAGS. Run that way, commit 2812c547 (the
// fix) is clean; with its operator() reverted to the body the fix replaced, ASan
// reports "heap-use-after-free ... READ of size 8" inside
// ui::EventSource::operator() from case 5 below.

#undef NDEBUG

#include "dui/Core/Box.h"
#include "dui/Core/Control.h"
#include "dui/Core/ControlFinder.h"
#include "dui/Core/ControlPtrT.h"
#include "dui/Core/EventArgs.h"
#include "dui/Core/GlobalManager.h"
#include "dui/Core/ResourceParam.h"
#include "dui/Core/UiPoint.h"
#include "dui/Core/UiRect.h"
#include "dui/Core/UiSize.h"
#include "dui/Layout/GridLayout.h"
#include "dui/Layout/HLayout.h"
#include "dui/Layout/VLayout.h"
#include "dui/Utils/FilePathUtil.h"

#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#define DUI_TEST_SOURCE_ROOT_TEXT_IMPL(value) (value)
#define DUI_TEST_SOURCE_ROOT_TEXT(value) DUI_TEST_SOURCE_ROOT_TEXT_IMPL(value)

namespace {

/// Stands in for anything that owns callbacks or controls: it can hand out a weak
/// flag, and freeing it is what expires every handle taken from it.
class LifetimeOwner final : public ui::SupportWeakCallback
{
};

/// A Control that reports its own destruction, so a test can tell "detached" apart
/// from "deleted".
class TrackedControl final : public ui::Control
{
public:
    explicit TrackedControl(int* destructions, const char* name = nullptr)
        : Control(nullptr), m_destructions(destructions)
    {
        if (name != nullptr) {
            SetName(name);
        }
    }

    ~TrackedControl() override { ++*m_destructions; }

    std::string GetType() const override { return "TrackedControl"; }

private:
    int* m_destructions;
};

/// An object that owns an EventSource, so a test can free the source from inside
/// one of its own callbacks.
struct SourceOwner
{
    ui::EventSource source;
};

/// The owner of a bound weak callback, for the UiBind case.
class BoundTarget final : public ui::SupportWeakCallback
{
public:
    int Add(int32_t value)
    {
        ++m_nCalls;
        return value + m_nOffset;
    }

    int m_nCalls = 0;
    int m_nOffset = 1;
};

/// A child control with an explicit size. Both values are set, because a fresh
/// PlaceHolder is stretch in both directions.
ui::Control* MakeFixedControl(int32_t cx, int32_t cy)
{
    ui::Control* pControl = new ui::Control(nullptr);
    pControl->SetFixedWidth(ui::UiFixedInt::MakeInt(cx), false, false);
    pControl->SetFixedHeight(ui::UiFixedInt::MakeInt(cy), false, false);
    return pControl;
}

/// An EventArgs whose sender is alive but is not a control. EventSource::operator()
/// refuses to dispatch at all once the sender flag is expired, so every dispatch
/// test needs one of these.
ui::EventArgs MakeArgs(LifetimeOwner& owner)
{
    ui::EventArgs args;
    args.eventType = ui::kEventClick;
    args.SetSenderWeakFlag(owner.GetWeakFlag());
    return args;
}

// ---------------------------------------------------------------------------
// Event dispatch
// ---------------------------------------------------------------------------

void TestEventSourceBasics()
{
    LifetimeOwner owner;
    ui::EventArgs args = MakeArgs(owner);

    ui::EventSource source;
    assert(source.IsEmpty());
    assert(!source.HasEventCallbackByID(1));

    std::vector<int> order;
    const ui::EventCallbackID first = 1;
    const ui::EventCallbackID second = 2;

    source.AddEventCallback([&order](const ui::EventArgs&) { order.push_back(1); return true; }, first);
    source.AddEventCallback([&order](const ui::EventArgs&) { order.push_back(2); return true; }, second);
    assert(!source.IsEmpty());
    assert(source.HasEventCallbackByID(first));
    assert(source.HasEventCallbackByID(second));
    assert(!source.HasEventCallbackByID(3));

    assert(source(args));
    assert(order.size() == 2);
    assert(order[0] == 1 && order[1] == 2);   // callbacks run in registration order

    // One ID may be registered several times, and RemoveEventCallbackByID removes
    // every entry carrying it.
    source.AddEventCallback([&order](const ui::EventArgs&) { order.push_back(1); return true; }, first);
    assert(source.RemoveEventCallbackByID(first));
    assert(!source.HasEventCallbackByID(first));
    assert(!source.RemoveEventCallbackByID(first));    // already gone
    // ID 0 is not testable: RemoveEventCallbackByID asserts callbackID > 0 before
    // it returns false, and that assert is live in a Debug build.

    order.clear();
    assert(source(args));
    assert(order.size() == 1 && order[0] == 2);
}

void TestEventSourceReturnValue()
{
    LifetimeOwner owner;
    ui::EventArgs args = MakeArgs(owner);

    // true means "handled"; false stops the dispatch and is reported back.
    {
        ui::EventSource source;
        int calls = 0;
        source.AddEventCallback([&calls](const ui::EventArgs&) { ++calls; return true; }, 1);
        assert(source(args));
        assert(calls == 1);

        source.RemoveEventCallbackByID(1);
        source.AddEventCallback([&calls](const ui::EventArgs&) { ++calls; return false; }, 1);
        assert(!source(args));
        assert(calls == 2);
    }
    {
        ui::EventSource source;
        std::vector<int> order;
        source.AddEventCallback([&order](const ui::EventArgs&) { order.push_back(1); return false; }, 1);
        source.AddEventCallback([&order](const ui::EventArgs&) { order.push_back(2); return true; }, 2);
        assert(!source(args));
        assert(order.size() == 1 && order[0] == 1);   // the later one never ran
    }
}

void TestEventArgsSender()
{
    LifetimeOwner owner;
    ui::EventArgs args = MakeArgs(owner);
    assert(!args.IsSenderExpired());
    assert(args.GetSender() == nullptr);   // only the lifetime flag was set

    // A default-constructed EventArgs has an empty weak_ptr, which is an expired
    // weak_ptr: no sender means no dispatch.
    ui::EventArgs unset;
    assert(unset.IsSenderExpired());
    assert(unset.GetSender() == nullptr);

    ui::EventSource source;
    int calls = 0;
    source.AddEventCallback([&calls](const ui::EventArgs&) { ++calls; return true; }, 1);
    assert(!source(unset));
    assert(calls == 0);

    // SetSender takes the control's own lifetime flag, and GetSender() checks it
    // before handing the raw pointer back.
    ui::Control* pSender = new ui::Control(nullptr);
    ui::EventArgs controlArgs;
    controlArgs.eventType = ui::kEventClick;
    controlArgs.SetSender(pSender);
    assert(controlArgs.GetSender() == pSender);
    assert(!controlArgs.IsSenderExpired());
    assert(source(controlArgs));
    assert(calls == 1);

    delete pSender;
    assert(controlArgs.IsSenderExpired());
    assert(controlArgs.GetSender() == nullptr);   // no dangling pointer escapes
    assert(!source(controlArgs));
    assert(calls == 1);

    // Clearing the sender expires it by the same rule.
    ui::Control* pOther = new ui::Control(nullptr);
    ui::EventArgs cleared;
    cleared.SetSender(pOther);
    assert(!cleared.IsSenderExpired());
    cleared.SetSender(nullptr);
    assert(cleared.IsSenderExpired());
    assert(cleared.GetSender() == nullptr);
    delete pOther;
}

/// The contract the use-after-free fix in src/Core/EventArgs.cpp created: the size
/// of the callback list is snapshotted before dispatch, and nothing touches the
/// container after a callback has run unless the sender is still alive.
void TestEventSourceMutationDuringDispatch()
{
    LifetimeOwner owner;
    ui::EventArgs args = MakeArgs(owner);

    // 1. The only callback removes itself.
    {
        ui::EventSource source;
        int calls = 0;
        constexpr ui::EventCallbackID only = 1;
        source.AddEventCallback([&source, &calls](const ui::EventArgs&) {
            ++calls;
            assert(source.RemoveEventCallbackByID(only));
            return true;
        }, only);

        assert(source(args));
        assert(calls == 1);
        assert(source.IsEmpty());
        assert(source(args));      // nothing left to call, and that is not an error
        assert(calls == 1);
    }

    // 4. A callback adds a callback: the running dispatch uses the snapshotted
    //    count, so the new one waits for the next dispatch.
    {
        ui::EventSource source;
        std::vector<int> order;
        source.AddEventCallback([&source, &order](const ui::EventArgs&) {
            order.push_back(1);
            source.AddEventCallback([&order](const ui::EventArgs&) { order.push_back(2); return true; }, 2);
            return true;
        }, 1);

        assert(source(args));
        assert(order.size() == 1 && order[0] == 1);

        order.clear();
        assert(source(args));
        assert(order.size() == 2 && order[0] == 1 && order[1] == 2);
    }

    // 5. A callback destroys the sender, and the EventSource goes with it -- the
    //    shape the use-after-free was reported in, where the source is a member of
    //    the control whose handler destroyed it. Nothing may touch the container
    //    after the callback returns; the pre-fix code read m_callbackList.size()
    //    here, which is a read of freed memory (see the ASan note in the file
    //    header: the read is only observable to a sanitizer, the assertions below
    //    pin the behavior that goes with it).
    {
        SourceOwner* pOwner = new SourceOwner();
        ui::Control* pSender = new ui::Control(nullptr);
        ui::EventArgs liveArgs;
        liveArgs.eventType = ui::kEventClick;
        liveArgs.SetSender(pSender);

        int firstCalls = 0;
        int secondCalls = 0;
        pOwner->source.AddEventCallback([&firstCalls, pOwner, pSender](const ui::EventArgs&) {
            ++firstCalls;
            delete pSender;
            delete pOwner;      // the source outlives this call no longer
            return true;
        }, 1);
        pOwner->source.AddEventCallback([&secondCalls](const ui::EventArgs&) { ++secondCalls; return true; }, 2);

        assert(!pOwner->source(liveArgs));
        assert(firstCalls == 1);
        assert(secondCalls == 0);   // it was registered, and it must not be reached
        assert(liveArgs.IsSenderExpired());
        assert(liveArgs.GetSender() == nullptr);
    }

    // 6. Removing an unrelated callback from inside a callback is a no-op for the
    //    running dispatch: the later callback still runs.
    {
        ui::EventSource source;
        std::vector<int> order;
        source.AddEventCallback([&source, &order](const ui::EventArgs&) {
            order.push_back(1);
            assert(!source.RemoveEventCallbackByID(99));
            return true;
        }, 1);
        source.AddEventCallback([&order](const ui::EventArgs&) { order.push_back(2); return true; }, 2);

        assert(source(args));
        assert(order.size() == 2 && order[0] == 1 && order[1] == 2);
    }
}

/// Removing a callback that has not run yet -- itself, while a later callback is
/// still registered, or that later callback -- is the case the comment on the
/// removal guard in EventSource::operator() promises to support. It does not: the
/// loop keeps the pre-dispatch index into a container that just got shorter, so
/// the next iteration indexes past the end. This is a separate group so that the
/// rest of the dispatch contract is still reported independently.
void TestEventSourceRemovesPendingCallback()
{
    LifetimeOwner owner;
    ui::EventArgs args = MakeArgs(owner);

    // A callback removes itself while a later callback is still registered.
    {
        ui::EventSource source;
        int firstCalls = 0;
        int secondCalls = 0;
        constexpr ui::EventCallbackID first = 1;
        constexpr ui::EventCallbackID second = 2;
        source.AddEventCallback([&source, &firstCalls](const ui::EventArgs&) {
            ++firstCalls;
            assert(source.RemoveEventCallbackByID(first));
            return true;
        }, first);
        source.AddEventCallback([&secondCalls](const ui::EventArgs&) { ++secondCalls; return true; }, second);

        bool bThrew = false;
        bool bHandled = false;
        try {
            bHandled = source(args);
        }
        catch (...) {
            bThrew = true;      // indexing the callback list past its end
        }
        assert(!bThrew);            // removal during dispatch must not run off the list
        assert(bHandled);
        assert(firstCalls == 1);
        assert(secondCalls == 1);   // it was registered when the dispatch began
        assert(!source.HasEventCallbackByID(first));
        assert(source.HasEventCallbackByID(second));
    }

    // A callback removes a callback that has not run yet.
    {
        ui::EventSource source;
        std::vector<int> order;
        constexpr ui::EventCallbackID second = 2;
        source.AddEventCallback([&source, &order](const ui::EventArgs&) {
            order.push_back(1);
            assert(source.RemoveEventCallbackByID(second));
            return true;
        }, 1);
        source.AddEventCallback([&order](const ui::EventArgs&) { order.push_back(2); return true; }, second);

        bool bThrew = false;
        try {
            source(args);
        }
        catch (...) {
            bThrew = true;
        }
        assert(!bThrew);
        assert(order.size() == 1 && order[0] == 1);   // the removed callback never ran
    }
}

void TestEventMapHelpers()
{
    ui::EventMap eventMap;
    eventMap[ui::kEventClick].AddEventCallback([](const ui::EventArgs&) { return true; }, 7);

    assert(ui::EventUtils::HasEventCallbackByID(eventMap, 7));
    assert(ui::EventUtils::HasEventCallbackByID(eventMap, ui::kEventClick, 7));
    assert(!ui::EventUtils::HasEventCallbackByID(eventMap, ui::kEventKeyDown, 7));
    assert(!ui::EventUtils::HasEventCallbackByID(eventMap, 8));

    assert(!ui::EventUtils::RemoveEventCallbackByID(eventMap, ui::kEventKeyDown, 7));
    assert(eventMap.size() == 1);
    assert(!ui::EventUtils::RemoveEventCallbackByID(eventMap, 8));

    assert(ui::EventUtils::RemoveEventCallbackByID(eventMap, ui::kEventClick, 7));
    // Once the last callback goes, the now-empty event-type entry goes with it.
    assert(eventMap.empty());
    assert(!ui::EventUtils::HasEventCallbackByID(eventMap, 7));

    // The XML names and the display name all resolve to the same event type.
    assert(ui::EventUtils::StringToEventType("click") == ui::kEventClick);
    assert(ui::EventUtils::StringToEventType("Click") == ui::kEventClick);
    assert(ui::EventUtils::EventTypeToString(ui::kEventClick) == "kEventClick");
}

// ---------------------------------------------------------------------------
// Weak references
// ---------------------------------------------------------------------------

/// A handle that goes null when its target dies is the library's whole answer to
/// dangling pointers (modern.md 2.2); this is that guarantee.
void TestControlPtrExpiry()
{
    ui::Control* pControl = new ui::Control(nullptr);
    ui::ControlPtr handle(pControl);
    assert(!handle.expired());
    assert(handle.get() == pControl);
    assert(handle == pControl);
    assert(handle != nullptr);

    ui::ControlPtr copy(handle);      // copies share the expiry check
    assert(copy == pControl);
    assert(!copy.expired());

    // A default-constructed handle is expired, not "invalid": an empty weak_ptr is
    // an expired weak_ptr, the same convention WeakCallback uses.
    ui::ControlPtr empty;
    assert(empty.expired());
    assert(empty.get() == nullptr);
    assert(empty == nullptr);

    delete pControl;
    assert(handle.expired());
    assert(handle.get() == nullptr);
    assert(handle == nullptr);
    assert(copy.expired());
    assert(copy.get() == nullptr);

    // Assigning and clearing stay safe once expired.
    handle = nullptr;
    assert(handle.expired() && handle.get() == nullptr);
    handle = copy;
    assert(handle.expired() && handle.get() == nullptr);
}

void TestBoxPtrExpiry()
{
    ui::Box* pBox = new ui::Box(nullptr, new ui::VLayout());
    ui::BoxPtr boxHandle(pBox);
    assert(!boxHandle.expired());
    assert(boxHandle.get() == pBox);

    // The handle is not tied to the box's children: adding one changes nothing.
    boxHandle->AddItem(MakeFixedControl(10, 10));
    assert(!boxHandle.expired());
    assert(boxHandle->GetItemCount() == 1);

    delete pBox;
    assert(boxHandle.expired());
    assert(boxHandle.get() == nullptr);
    assert(boxHandle == nullptr);
}

/// The canonical usage pattern from modern.md 2.2 (AnimationManager.cpp:45,81,114):
/// capture the weak handle by value and ask it before every use.
void TestHandleCapturedByCallback()
{
    ui::Control* pControl = new ui::Control(nullptr);
    ui::ControlPtr handle(pControl);

    int32_t observed = -1;
    auto callback = [handle, &observed](int32_t value) {
        observed = (handle != nullptr) ? value : -1;
    };

    callback(7);
    assert(observed == 7);

    delete pControl;
    callback(9);
    assert(observed == -1);   // the handle went null instead of dangling
}

void TestWeakCallbackFlag()
{
    ui::WeakCallbackFlag flag;
    assert(!flag.HasUsed());

    auto weak = flag.ToWeakCallback(std::function<int32_t(int32_t)>(
        [](int32_t value) { return value + 1; }));
    assert(!weak.Expired());
    assert(flag.HasUsed());
    assert(weak(4) == 5);

    flag.Cancel();               // the owner says "stop calling me"
    assert(!flag.HasUsed());
    assert(weak.Expired());
    assert(weak(4) == 0);        // an expired weak callback yields a default value
}

void TestUiBindCarriesTheOwnerLifetime()
{
    std::function<int(int32_t)> bound;
    {
        BoundTarget target;
        // The placeholder is not decoration: this is how every UiBind call site in
        // the library is written, and with libc++ a std::bind of a member function
        // pointer with no placeholder neither converts nor invokes.
        bound = ui::UiBind(&BoundTarget::Add, &target, std::placeholders::_1);
        assert(bound(4) == 5);
        assert(target.m_nCalls == 1);
    }
    // The owner is gone; calling the bound callback must not call into it.
    assert(bound(4) == 0);
}

/// A box that owns a control destroys it on removal, and the weak handle notices.
void TestHandleFollowsBoxRemoval()
{
    int destructions = 0;
    ui::Box* pBox = new ui::Box(nullptr, new ui::VLayout());
    TrackedControl* pChild = new TrackedControl(&destructions);
    ui::ControlPtr handle(pChild);
    assert(pBox->AddItem(pChild));
    assert(!handle.expired());

    assert(pBox->IsAutoDestroyChild());
    assert(pBox->RemoveItem(pChild));
    assert(destructions == 1);
    assert(handle.expired());      // the box deleted it, and the handle knows
    assert(handle.get() == nullptr);

    delete pBox;
}

// ---------------------------------------------------------------------------
// Box / control tree
// ---------------------------------------------------------------------------

void TestBoxItemOrdering()
{
    // Auto-destroy off, so the controls survive being removed and the ordering
    // assertions can keep looking at them.
    ui::Box* pBox = new ui::Box(nullptr, new ui::VLayout());
    pBox->SetAutoDestroyChild(false);
    assert(pBox->GetItemCount() == 0);
    assert(pBox->GetItemAt(0) == nullptr);          // out of range reads are null
    assert(pBox->GetItemIndex(nullptr) == ui::Box::InvalidIndex);
    assert(!ui::Box::IsValidItemIndex(ui::Box::InvalidIndex));
    assert(ui::Box::IsValidItemIndex(0));

    ui::Control* first = MakeFixedControl(10, 10);
    ui::Control* second = MakeFixedControl(10, 10);
    ui::Control* third = MakeFixedControl(10, 10);
    ui::Control* outsider = MakeFixedControl(10, 10);

    assert(pBox->AddItem(first));
    assert(pBox->AddItem(second));
    assert(pBox->GetItemCount() == 2);
    assert(pBox->GetItemAt(0) == first);
    assert(pBox->GetItemAt(1) == second);
    assert(pBox->GetItemAt(2) == nullptr);
    assert(pBox->GetItemIndex(first) == 0);
    assert(pBox->GetItemIndex(second) == 1);
    assert(pBox->GetItemIndex(third) == ui::Box::InvalidIndex);
    assert(!pBox->AddItem(nullptr));

    // AddItem appends; AddItemAt inserts, and the indices after it shift up.
    assert(pBox->AddItemAt(third, 0));
    assert(pBox->GetItemCount() == 3);
    assert(pBox->GetItemAt(0) == third);
    assert(pBox->GetItemAt(1) == first);
    assert(pBox->GetItemAt(2) == second);

    // Reordering.
    assert(pBox->SetItemIndex(third, 2));
    assert(pBox->GetItemAt(0) == first);
    assert(pBox->GetItemAt(1) == second);
    assert(pBox->GetItemAt(2) == third);
    assert(!pBox->SetItemIndex(third, 3));       // index == size is out of range
    assert(!pBox->SetItemIndex(outsider, 0));    // not a member of this box
    assert(!pBox->SetItemIndex(nullptr, 0));
    assert(pBox->GetItemCount() == 3);           // no rejected call changed anything

    // Removal.
    assert(pBox->RemoveItemAt(0));
    assert(pBox->GetItemCount() == 2);
    assert(pBox->GetItemAt(0) == second);
    assert(!pBox->RemoveItemAt(5));
    assert(!pBox->RemoveItem(outsider));
    assert(!pBox->RemoveItem(nullptr));
    assert(pBox->RemoveItem(third));
    assert(pBox->GetItemCount() == 1);

    pBox->RemoveAllItems();
    assert(pBox->GetItemCount() == 0);
    assert(pBox->GetItemAt(0) == nullptr);
    pBox->RemoveAllItems();                      // idempotent

    delete first;
    delete second;
    delete third;
    delete outsider;
    delete pBox;
}

/// What SetAutoDestroyChild does and does not do.
void TestAutoDestroyChild()
{
    // The default: the box owns its children and deletes the one that leaves it.
    {
        int destructions = 0;
        ui::Box* pBox = new ui::Box(nullptr, new ui::VLayout());
        assert(pBox->IsAutoDestroyChild());

        TrackedControl* pChild = new TrackedControl(&destructions);
        assert(pBox->AddItem(pChild));
        assert(destructions == 0);
        assert(pBox->RemoveItem(pChild));
        assert(destructions == 1);               // RemoveItem destroyed it
        assert(pBox->GetItemCount() == 0);
        delete pBox;
        assert(destructions == 1);
    }

    // Destroying the box destroys whatever is still inside it.
    {
        int destructions = 0;
        ui::Box* pBox = new ui::Box(nullptr, new ui::VLayout());
        pBox->AddItem(new TrackedControl(&destructions));
        pBox->AddItem(new TrackedControl(&destructions));
        delete pBox;
        assert(destructions == 2);
    }

    // Auto-destroy off: the box releases the children but does not own them, and
    // destroying the box destroys nothing.
    {
        int destructions = 0;
        ui::Box* pBox = new ui::Box(nullptr, new ui::VLayout());
        pBox->SetAutoDestroyChild(false);
        assert(!pBox->IsAutoDestroyChild());

        TrackedControl* pChild = new TrackedControl(&destructions);
        assert(pBox->AddItem(pChild));
        delete pBox;
        assert(destructions == 0);               // the box left the child alone
        delete pChild;
        assert(destructions == 1);
    }

    // RemoveAllItems follows the same rule.
    {
        int destructions = 0;
        ui::Box* pBox = new ui::Box(nullptr, new ui::VLayout());
        pBox->SetAutoDestroyChild(false);
        TrackedControl* pFirst = new TrackedControl(&destructions);
        TrackedControl* pSecond = new TrackedControl(&destructions);
        pBox->AddItem(pFirst);
        pBox->AddItem(pSecond);
        pBox->RemoveAllItems();
        assert(pBox->GetItemCount() == 0);
        assert(destructions == 0);
        delete pFirst;
        delete pSecond;
        assert(destructions == 2);
        delete pBox;
    }

    // AddItem parents the control to the box it lands in.
    {
        int destructions = 0;
        ui::Box* pBox = new ui::Box(nullptr, new ui::VLayout());
        pBox->SetAutoDestroyChild(false);
        TrackedControl* pChild = new TrackedControl(&destructions);
        assert(pBox->AddItem(pChild));
        assert(pChild->GetParent() == pBox);

        delete pChild;
        delete pBox;
    }
}

/// Detaching releases the control without destroying it, so the box must stop
/// presenting itself as its parent: the back-pointer outlives the box, and
/// PlaceHolder::SetRect() (float controls), GetScrollOffsetInScrollBox() and
/// PlaceHolder::IsControlRelated() all walk GetParent() and dereference it. This
/// is the one part of the detach contract the current implementation does not
/// provide, so it has its own group.
void TestDetachedChildHasNoParent()
{
    int destructions = 0;
    ui::Box* pBox = new ui::Box(nullptr, new ui::VLayout());
    pBox->SetAutoDestroyChild(false);
    TrackedControl* pChild = new TrackedControl(&destructions);
    assert(pBox->AddItem(pChild));
    assert(pChild->GetParent() == pBox);

    assert(pBox->RemoveItem(pChild));
    assert(destructions == 0);                 // released, not destroyed
    assert(pBox->GetItemCount() == 0);
    assert(pChild->GetParent() == nullptr);    // and no longer parented to the box

    // The same for the bulk removal.
    assert(pBox->AddItem(pChild));
    pBox->RemoveAllItems();
    assert(pChild->GetParent() == nullptr);

    delete pChild;
    delete pBox;
}

// ---------------------------------------------------------------------------
// Layout engine
// ---------------------------------------------------------------------------

void TestHLayout()
{
    ui::HLayout* pLayout = new ui::HLayout();
    ui::Box* pBox = new ui::Box(nullptr, pLayout);   // the box owns the layout
    pBox->SetPadding(ui::UiPadding(5, 6, 7, 8), false);
    assert(pBox->GetLayout() == pLayout);

    ui::Control* first = MakeFixedControl(100, 20);
    ui::Control* second = MakeFixedControl(50, 30);
    assert(pBox->AddItem(first));
    assert(pBox->AddItem(second));

    const std::vector<ui::Control*> items = { first, second };

    // The content rect is the box minus its padding: (5,6)-(293,192). Children go
    // left to right, top-aligned, with no spacing by default.
    const ui::UiSize64 arranged = pLayout->ArrangeChildren(items, ui::UiRect(0, 0, 300, 200));
    assert(first->GetRect() == ui::UiRect(5, 6, 105, 26));
    assert(second->GetRect() == ui::UiRect(105, 6, 155, 36));

    // The returned size is what the content needs plus the container's padding: the
    // last child's right edge (155) plus the 7px right padding, and the tallest
    // child (30) plus the vertical padding.
    assert(arranged == ui::UiSize64(162, 44));

    // Estimate and arrange agree while every child is fixed-size. The available
    // size passed in is the box rect; the layout removes the padding itself.
    const ui::UiSize64 estimated = pLayout->EstimateLayoutSize(items, ui::UiSize(300, 200));
    assert(estimated == arranged);

    // Driving it through the box produces the same rects.
    first->SetPos(ui::UiRect(0, 0, 0, 0));
    second->SetPos(ui::UiRect(0, 0, 0, 0));
    pBox->SetPos(ui::UiRect(0, 0, 300, 200));
    assert(first->GetRect() == ui::UiRect(5, 6, 105, 26));
    assert(second->GetRect() == ui::UiRect(105, 6, 155, 36));

    delete pBox;
}

void TestHLayoutStretchAndSpacing()
{
    // Inter-child spacing is added between controls, never around them.
    {
        ui::HLayout* pLayout = new ui::HLayout();
        ui::Box* pBox = new ui::Box(nullptr, pLayout);
        pLayout->SetChildMargin(10);
        assert(pLayout->GetChildMarginX() == 10 && pLayout->GetChildMarginY() == 10);

        ui::Control* first = MakeFixedControl(100, 20);
        ui::Control* second = MakeFixedControl(50, 20);
        pBox->AddItem(first);
        pBox->AddItem(second);

        const std::vector<ui::Control*> items = { first, second };
        const ui::UiSize64 arranged = pLayout->ArrangeChildren(items, ui::UiRect(0, 0, 300, 200));
        assert(first->GetRect() == ui::UiRect(0, 0, 100, 20));
        assert(second->GetRect() == ui::UiRect(110, 0, 160, 20));   // 100 + the 10px spacing
        assert(arranged == ui::UiSize64(160, 20));

        delete pBox;
    }

    // A stretch child takes the width the fixed ones leave.
    {
        ui::HLayout* pLayout = new ui::HLayout();
        ui::Box* pBox = new ui::Box(nullptr, pLayout);

        ui::Control* fixed = MakeFixedControl(100, 20);
        ui::Control* stretch = new ui::Control(nullptr);   // the width stays stretch
        stretch->SetFixedHeight(ui::UiFixedInt::MakeInt(20), false, false);
        pBox->AddItem(fixed);
        pBox->AddItem(stretch);

        const std::vector<ui::Control*> items = { fixed, stretch };
        const ui::UiSize64 arranged = pLayout->ArrangeChildren(items, ui::UiRect(0, 0, 300, 200));
        assert(fixed->GetRect() == ui::UiRect(0, 0, 100, 20));
        assert(stretch->GetRect() == ui::UiRect(100, 0, 300, 20));   // 100% of the 200 left over
        assert(arranged == ui::UiSize64(300, 20));

        delete pBox;
    }

    // An invisible child takes no space; a margin offsets the child inside its slot.
    {
        ui::HLayout* pLayout = new ui::HLayout();
        ui::Box* pBox = new ui::Box(nullptr, pLayout);

        ui::Control* hidden = MakeFixedControl(100, 20);
        hidden->SetVisible(false);
        ui::Control* margined = MakeFixedControl(50, 20);
        margined->SetMargin(ui::UiMargin(3, 4, 5, 6), false);
        pBox->AddItem(hidden);
        pBox->AddItem(margined);

        const std::vector<ui::Control*> items = { hidden, margined };
        const ui::UiSize64 arranged = pLayout->ArrangeChildren(items, ui::UiRect(0, 0, 300, 200));
        assert(hidden->GetRect() == ui::UiRect(0, 0, 0, 0));         // never laid out
        assert(margined->GetRect() == ui::UiRect(3, 4, 53, 24));     // offset by its margins
        // The reported extent covers the child's margins: 53 + 5 and 24 + 6.
        assert(arranged == ui::UiSize64(58, 30));

        delete pBox;
    }
}

void TestVLayout()
{
    ui::VLayout* pLayout = new ui::VLayout();
    ui::Box* pBox = new ui::Box(nullptr, pLayout);
    pBox->SetPadding(ui::UiPadding(5, 6, 7, 8), false);

    ui::Control* first = MakeFixedControl(80, 20);
    ui::Control* second = MakeFixedControl(80, 30);
    assert(pBox->AddItem(first));
    assert(pBox->AddItem(second));

    const std::vector<ui::Control*> items = { first, second };

    // Children stack top to bottom inside the padded content rect, each at its own
    // size and at the left edge.
    const ui::UiSize64 arranged = pLayout->ArrangeChildren(items, ui::UiRect(0, 0, 300, 200));
    assert(first->GetRect() == ui::UiRect(5, 6, 85, 26));
    assert(second->GetRect() == ui::UiRect(5, 26, 85, 56));

    // Width is the widest child, not the sum; height is the sum of the heights.
    assert(arranged == ui::UiSize64(92, 64));

    const ui::UiSize64 estimated = pLayout->EstimateLayoutSize(items, ui::UiSize(300, 200));
    assert(estimated == arranged);

    delete pBox;
}

void TestVLayoutStretchAndAlignment()
{
    // A stretch child takes the height the fixed ones leave.
    {
        ui::VLayout* pLayout = new ui::VLayout();
        ui::Box* pBox = new ui::Box(nullptr, pLayout);

        ui::Control* fixed = MakeFixedControl(80, 50);
        ui::Control* stretch = new ui::Control(nullptr);   // the height stays stretch
        stretch->SetFixedWidth(ui::UiFixedInt::MakeInt(80), false, false);
        pBox->AddItem(fixed);
        pBox->AddItem(stretch);

        const std::vector<ui::Control*> items = { fixed, stretch };
        const ui::UiSize64 arranged = pLayout->ArrangeChildren(items, ui::UiRect(0, 0, 300, 200));
        assert(fixed->GetRect() == ui::UiRect(0, 0, 80, 50));
        assert(stretch->GetRect() == ui::UiRect(0, 50, 80, 200));   // 100% of the 150 left over
        assert(arranged == ui::UiSize64(80, 200));

        delete pBox;
    }

    // A child whose width was never set stretches to the content width, and its own
    // horizontal alignment positions it inside that slot.
    {
        ui::VLayout* pLayout = new ui::VLayout();
        ui::Box* pBox = new ui::Box(nullptr, pLayout);
        ui::Control* child = new ui::Control(nullptr);   // width and height both stretch
        child->SetFixedHeight(ui::UiFixedInt::MakeInt(30), false, false);
        child->SetHorAlignType(ui::HorAlignType::kAlignRight);
        pBox->AddItem(child);

        const std::vector<ui::Control*> items = { child };
        const ui::UiSize64 arranged = pLayout->ArrangeChildren(items, ui::UiRect(0, 0, 300, 200));
        assert(child->GetRect() == ui::UiRect(0, 0, 300, 30));
        assert(arranged == ui::UiSize64(300, 30));

        delete pBox;
    }

    // Horizontally centered children sit in the middle of the content rect.
    {
        ui::VLayout* pLayout = new ui::VLayout();
        ui::Box* pBox = new ui::Box(nullptr, pLayout);
        ui::Control* child = MakeFixedControl(50, 20);
        child->SetHorAlignType(ui::HorAlignType::kAlignCenter);
        pBox->AddItem(child);

        const std::vector<ui::Control*> items = { child };
        pLayout->ArrangeChildren(items, ui::UiRect(0, 0, 300, 100));
        assert(child->GetRect() == ui::UiRect(125, 0, 175, 20));

        delete pBox;
    }
}

void TestGridLayout()
{
    ui::GridLayout* pLayout = new ui::GridLayout();
    ui::Box* pBox = new ui::Box(nullptr, pLayout);
    pLayout->SetRows(2);
    pLayout->SetColumns(2);
    assert(pLayout->GetRows() == 2);
    assert(pLayout->GetColumns() == 2);
    assert(!pLayout->IsScaleDown());   // default: a child is resized to its cell

    ui::Control* cells[4] = { MakeFixedControl(10, 10), MakeFixedControl(10, 10),
                              MakeFixedControl(10, 10), MakeFixedControl(10, 10) };
    std::vector<ui::Control*> items;
    for (ui::Control* pCell : cells) {
        assert(pBox->AddItem(pCell));
        items.push_back(pCell);
    }

    // A 2x2 grid over a 200x100 box is four 100x50 cells, filled row by row.
    const ui::UiSize64 arranged = pLayout->ArrangeChildren(items, ui::UiRect(0, 0, 200, 100));
    assert(cells[0]->GetRect() == ui::UiRect(0, 0, 100, 50));
    assert(cells[1]->GetRect() == ui::UiRect(100, 0, 200, 50));
    assert(cells[2]->GetRect() == ui::UiRect(0, 50, 100, 100));
    assert(cells[3]->GetRect() == ui::UiRect(100, 50, 200, 100));
    assert(arranged == ui::UiSize64(200, 100));

    const ui::UiSize64 estimated = pLayout->EstimateLayoutSize(items, ui::UiSize(200, 100));
    assert(estimated == arranged);

    // The cells tile the content area: no overlap, no gap.
    assert(cells[0]->GetRect().Right() == cells[1]->GetRect().Left());
    assert(cells[0]->GetRect().Bottom() == cells[2]->GetRect().Top());

    // One stated column instead of two: the extra children wrap onto the next row.
    pLayout->SetColumns(1);
    assert(pLayout->GetColumns() == 1);
    const std::vector<ui::Control*> twoItems = { cells[0], cells[1] };
    pLayout->ArrangeChildren(twoItems, ui::UiRect(0, 0, 200, 100));
    assert(cells[0]->GetRect() == ui::UiRect(0, 0, 200, 50));
    assert(cells[1]->GetRect() == ui::UiRect(0, 50, 200, 100));

    delete pBox;
}

// ---------------------------------------------------------------------------
// Hit-testing
// ---------------------------------------------------------------------------

void TestHitTesting()
{
    // Box's default layout is the base float layout, which stacks every child at
    // the top-left of the box. The two children therefore overlap on purpose: that
    // is the case UIFIND_TOP_FIRST exists to decide.
    ui::Box* pRoot = new ui::Box(nullptr);
    pRoot->SetName("root");

    ui::Control* under = MakeFixedControl(100, 100);
    under->SetName("under");
    ui::Control* over = MakeFixedControl(100, 100);
    over->SetName("over");
    assert(pRoot->AddItem(under));
    assert(pRoot->AddItem(over));

    pRoot->SetPos(ui::UiRect(0, 0, 200, 100));
    assert(under->GetRect() == ui::UiRect(0, 0, 100, 100));
    assert(over->GetRect() == ui::UiRect(0, 0, 100, 100));

    ui::ControlFinder finder;
    assert(finder.GetRoot() == nullptr);
    finder.SetRoot(pRoot);
    assert(finder.GetRoot() == pRoot);

    // FindControl walks the children last-to-first, so the topmost one wins.
    assert(finder.FindControl(ui::UiPoint(50, 50)) == over);
    // Right of both children but still inside the root: the walk ends by offering
    // the point to the container itself, so the container is the answer.
    assert(finder.FindControl(ui::UiPoint(150, 50)) == pRoot);
    // Outside the root's own rect there is nothing to hit at all.
    assert(finder.FindControl(ui::UiPoint(500, 500)) == nullptr);

    // The same walk in registration order returns the one underneath, which is what
    // makes the result above about z-order rather than luck.
    ui::UiPoint pt(50, 50);
    assert(pRoot->FindControl(ui::ControlFinder::FindControlFromPoint, &pt,
                              UIFIND_VISIBLE | UIFIND_HITTEST, pt) == under);

    // A hidden child is skipped.
    under->SetVisible(false);
    assert(pRoot->FindControl(ui::ControlFinder::FindControlFromPoint, &pt,
                              UIFIND_VISIBLE | UIFIND_HITTEST, pt) == over);
    under->SetVisible(true);

    // By name, in tree order.
    assert(finder.FindSubControlByName(nullptr, "under") == under);
    assert(finder.FindSubControlByName(nullptr, "over") == over);
    assert(finder.FindSubControlByName(nullptr, "missing") == nullptr);
    assert(finder.FindSubControlByName(pRoot, "over") == over);

    // The by-name cache is a weak index: a registered control that dies drops out of
    // it instead of leaving a dangling pointer to hand back.
    ui::Control* pTemporary = MakeFixedControl(10, 10);
    pTemporary->SetName("cached");
    finder.AddControl(pTemporary);
    assert(finder.FindControlInCache(nullptr, "cached") == pTemporary);
    finder.AddControl(nullptr);                  // ignored, no crash
    delete pTemporary;
    assert(finder.FindControlInCache(nullptr, "cached") == nullptr);

    // Two controls can share a name; the first registered is the one found.
    ui::Control* pFirstNamed = MakeFixedControl(10, 10);
    pFirstNamed->SetName("duplicate");
    ui::Control* pSecondNamed = MakeFixedControl(10, 10);
    pSecondNamed->SetName("duplicate");
    finder.AddControl(pFirstNamed);
    finder.AddControl(pSecondNamed);
    assert(finder.FindControlInCache(nullptr, "duplicate") == pFirstNamed);
    finder.RemoveControl(pFirstNamed);
    assert(finder.FindControlInCache(nullptr, "duplicate") == pSecondNamed);
    finder.RemoveControl(pSecondNamed);
    assert(finder.FindControlInCache(nullptr, "duplicate") == nullptr);
    finder.RemoveControl(nullptr);               // ignored, no crash

    // Clearing drops the root along with the index.
    finder.Clear();
    assert(finder.GetRoot() == nullptr);

    delete pFirstNamed;
    delete pSecondNamed;
    delete pRoot;
}

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

using TestFunction = void (*)();

void RunGroup(const TestFunction* pCases, size_t nCount)
{
    for (size_t i = 0; i < nCount; ++i) {
        pCases[i]();
    }
}

const TestFunction g_eventDispatchCases[] = {
    TestEventSourceBasics,
    TestEventSourceReturnValue,
    TestEventArgsSender,
    TestEventSourceMutationDuringDispatch,
    TestEventMapHelpers,
};

const TestFunction g_eventDispatchRemovalCases[] = {
    TestEventSourceRemovesPendingCallback,
};

const TestFunction g_weakReferenceCases[] = {
    TestControlPtrExpiry,
    TestBoxPtrExpiry,
    TestHandleCapturedByCallback,
    TestWeakCallbackFlag,
    TestUiBindCarriesTheOwnerLifetime,
    TestHandleFollowsBoxRemoval,
};

const TestFunction g_controlTreeCases[] = {
    TestBoxItemOrdering,
    TestAutoDestroyChild,
};

const TestFunction g_controlTreeDetachCases[] = {
    TestDetachedChildHasNoParent,
};

const TestFunction g_layoutCases[] = {
    TestHLayout,
    TestHLayoutStretchAndSpacing,
    TestVLayout,
    TestVLayoutStretchAndAlignment,
    TestGridLayout,
};

const TestFunction g_hitTestCases[] = {
    TestHitTesting,
};

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        return 2;
    }
    const std::string group(argv[1]);

    const TestFunction* pCases = nullptr;
    size_t nCount = 0;
    if (group == "--event-dispatch") {
        pCases = g_eventDispatchCases;
        nCount = sizeof(g_eventDispatchCases) / sizeof(g_eventDispatchCases[0]);
    }
    else if (group == "--event-dispatch-removal") {
        pCases = g_eventDispatchRemovalCases;
        nCount = sizeof(g_eventDispatchRemovalCases) / sizeof(g_eventDispatchRemovalCases[0]);
    }
    else if (group == "--weak-references") {
        pCases = g_weakReferenceCases;
        nCount = sizeof(g_weakReferenceCases) / sizeof(g_weakReferenceCases[0]);
    }
    else if (group == "--control-tree") {
        pCases = g_controlTreeCases;
        nCount = sizeof(g_controlTreeCases) / sizeof(g_controlTreeCases[0]);
    }
    else if (group == "--control-tree-detach") {
        pCases = g_controlTreeDetachCases;
        nCount = sizeof(g_controlTreeDetachCases) / sizeof(g_controlTreeDetachCases[0]);
    }
    else if (group == "--layout") {
        pCases = g_layoutCases;
        nCount = sizeof(g_layoutCases) / sizeof(g_layoutCases[0]);
    }
    else if (group == "--hit-test") {
        pCases = g_hitTestCases;
        nCount = sizeof(g_hitTestCases) / sizeof(g_hitTestCases[0]);
    }
    else {
        return 2;
    }

    // Control construction and destruction reach GlobalManager -- its ASSERT in
    // ImageManager::RemoveDelayPaintData checks the UI thread id, which only Startup
    // records -- so the whole group runs inside one Startup/Shutdown pair. No window
    // is created and nothing is rendered.
    const ui::FilePath sourceRoot(DUI_TEST_SOURCE_ROOT_TEXT(DUI_TEST_SOURCE_ROOT));
    ui::LocalFilesResParam resources(ui::FilePathUtil::JoinFilePath(
        sourceRoot, ui::FilePath("resources")));
    assert(ui::GlobalManager::Instance().Startup(resources));

    RunGroup(pCases, nCount);

    ui::GlobalManager::Instance().Shutdown();
    return 0;
}
