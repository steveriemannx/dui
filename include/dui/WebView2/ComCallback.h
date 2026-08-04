#ifndef UI_WEBVIEW2_COM_CALLBACK_H_
#define UI_WEBVIEW2_COM_CALLBACK_H_

#include "dui/dui_defs.h"

#if defined (DUI_BUILD_FOR_WIN) && defined (DUI_BUILD_FOR_WEBVIEW2)

#include "ComPtr.h"

#include <unknwn.h>
#include <Shlwapi.h>
#include <utility>
#include <memory>
#include <functional>
#include <type_traits>
#include <tuple>

namespace ui
{
    // Forward declaration of the smart pointer class (to be used with the previously implemented ComPtr)
    template<typename T> class ComPtr;

    // ------------------------------
    // Extracts the parameter types of the Invoke method in the interface (supports the STDMETHODCALLTYPE calling convention)
    // ------------------------------

    // Base template: no Invoke method by default
    template <typename TInterface, typename = void>
    struct InvokeTraits {
        static constexpr bool HasInvoke = false;
    };

    // Specialized template: matches an Invoke method with the STDMETHODCALLTYPE (__stdcall) calling convention
    template <typename TInterface>
    struct InvokeTraits<TInterface, std::void_t<decltype(&TInterface::Invoke)>> {
        static constexpr bool HasInvoke = true;

        // Member function supporting the STDMETHODCALLTYPE (__stdcall) calling convention
        template <typename R, typename... Args>
        static std::tuple<Args...> GetArgs(R(STDMETHODCALLTYPE TInterface::*)(Args...));

        // Extracts the parameter types of the Invoke method
        using ArgsTuple = decltype(GetArgs(&TInterface::Invoke));

        // Argument count
        static constexpr size_t ArgCount = std::tuple_size_v<ArgsTuple>;
    };

    // ------------------------------
    // Callback implementation class: specialized by argument count (supports 0-6 arguments)
    // ------------------------------

    // Base template (disabled, must use the specialized version)
    template <typename TInterface, const IID& IID_TInterface, typename TCallback, size_t ArgCount>
    class ComCallbackImpl;

    // Specialization: Invoke method with 0 arguments
    template <typename TInterface, const IID& IID_TInterface, typename TCallback>
    class ComCallbackImpl<TInterface, IID_TInterface, TCallback, 0> : public TInterface {
    public:
        explicit ComCallbackImpl(TCallback callback)
            : m_callback(std::move(callback)), m_refCount(1) {
        }
        virtual ~ComCallbackImpl() = default;

        // IUnknown implementation
        STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
            if (riid == IID_IUnknown || riid == IID_TInterface) {
                *ppv = static_cast<TInterface*>(this);
                AddRef();
                return S_OK;
            }
            *ppv = nullptr;
            return E_NOINTERFACE;
        }

        STDMETHOD_(ULONG, AddRef)() override {
            return InterlockedIncrement(&m_refCount);
        }

        STDMETHOD_(ULONG, Release)() override {
            ULONG ref = InterlockedDecrement(&m_refCount);
            if (ref == 0) {
                delete this;
            }
            return ref;
        }

        // Implements the 0-argument Invoke
        STDMETHOD(Invoke)() override {
            return m_callback();
        }

    private:
        TCallback m_callback;
        LONG m_refCount;
    };

    // Specialization: Invoke method with 1 argument
    template <typename TInterface, const IID& IID_TInterface, typename TCallback>
    class ComCallbackImpl<TInterface, IID_TInterface, TCallback, 1> : public TInterface {
    public:
        explicit ComCallbackImpl(TCallback callback)
            : m_callback(std::move(callback)), m_refCount(1) {
        }
        virtual ~ComCallbackImpl() = default;

        STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
            if (riid == IID_IUnknown || riid == IID_TInterface) {
                *ppv = static_cast<TInterface*>(this);
                AddRef();
                return S_OK;
            }
            return E_NOINTERFACE;
        }

        STDMETHOD_(ULONG, AddRef)() override { return InterlockedIncrement(&m_refCount); }
        STDMETHOD_(ULONG, Release)() override {
            ULONG ref = InterlockedDecrement(&m_refCount);
            if (ref == 0) {
                delete this;
            }
            return ref;
        }

        // Implements the 1-argument Invoke
        STDMETHOD(Invoke)(
            typename std::tuple_element_t<0, typename InvokeTraits<TInterface>::ArgsTuple> arg1
            ) override {
            return m_callback(arg1);
        }

    private:
        TCallback m_callback;
        LONG m_refCount;
    };

    // Specialization: Invoke method with 2 arguments (commonly used by WebView2)
    template <typename TInterface, const IID& IID_TInterface, typename TCallback>
    class ComCallbackImpl<TInterface, IID_TInterface, TCallback, 2> : public TInterface {
    public:
        explicit ComCallbackImpl(TCallback callback)
            : m_callback(std::move(callback)), m_refCount(1) {
        }
        virtual ~ComCallbackImpl() = default;

        STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
            if (riid == IID_IUnknown || riid == IID_TInterface) {
                *ppv = static_cast<TInterface*>(this);
                AddRef();
                return S_OK;
            }
            return E_NOINTERFACE;
        }

        STDMETHOD_(ULONG, AddRef)() override { return InterlockedIncrement(&m_refCount); }
        STDMETHOD_(ULONG, Release)() override {
            ULONG ref = InterlockedDecrement(&m_refCount);
            if (ref == 0) {
                delete this;
            }
            return ref;
        }

        // Implements the 2-argument Invoke
        STDMETHOD(Invoke)(
            typename std::tuple_element_t<0, typename InvokeTraits<TInterface>::ArgsTuple> arg1,
            typename std::tuple_element_t<1, typename InvokeTraits<TInterface>::ArgsTuple> arg2
            ) override {
            return m_callback(arg1, arg2);
        }

    private:
        TCallback m_callback;
        LONG m_refCount;
    };

    // Specialization: Invoke method with 3 arguments
    template <typename TInterface, const IID& IID_TInterface, typename TCallback>
    class ComCallbackImpl<TInterface, IID_TInterface, TCallback, 3> : public TInterface {
    public:
        explicit ComCallbackImpl(TCallback callback)
            : m_callback(std::move(callback)), m_refCount(1) {
        }
        virtual ~ComCallbackImpl() = default;

        STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
            if (riid == IID_IUnknown || riid == IID_TInterface) {
                *ppv = static_cast<TInterface*>(this);
                AddRef();
                return S_OK;
            }
            return E_NOINTERFACE;
        }

        STDMETHOD_(ULONG, AddRef)() override { return InterlockedIncrement(&m_refCount); }
        STDMETHOD_(ULONG, Release)() override {
            ULONG ref = InterlockedDecrement(&m_refCount);
            if (ref == 0) {
                delete this;
            }
            return ref;
        }

        // Implements the 3-argument Invoke
        STDMETHOD(Invoke)(
            typename std::tuple_element_t<0, typename InvokeTraits<TInterface>::ArgsTuple> arg1,
            typename std::tuple_element_t<1, typename InvokeTraits<TInterface>::ArgsTuple> arg2,
            typename std::tuple_element_t<2, typename InvokeTraits<TInterface>::ArgsTuple> arg3
            ) override {
            return m_callback(arg1, arg2, arg3);
        }

    private:
        TCallback m_callback;
        LONG m_refCount;
    };

    // Specialization: Invoke method with 4 arguments
    template <typename TInterface, const IID& IID_TInterface, typename TCallback>
    class ComCallbackImpl<TInterface, IID_TInterface, TCallback, 4> : public TInterface {
    public:
        explicit ComCallbackImpl(TCallback callback)
            : m_callback(std::move(callback)), m_refCount(1) {
        }
        virtual ~ComCallbackImpl() = default;

        STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
            if (riid == IID_IUnknown || riid == IID_TInterface) {
                *ppv = static_cast<TInterface*>(this);
                AddRef();
                return S_OK;
            }
            return E_NOINTERFACE;
        }

        STDMETHOD_(ULONG, AddRef)() override { return InterlockedIncrement(&m_refCount); }
        STDMETHOD_(ULONG, Release)() override {
            ULONG ref = InterlockedDecrement(&m_refCount);
            if (ref == 0) {
                delete this;
            }
            return ref;
        }

        // Implements the 4-argument Invoke
        STDMETHOD(Invoke)(
            typename std::tuple_element_t<0, typename InvokeTraits<TInterface>::ArgsTuple> arg1,
            typename std::tuple_element_t<1, typename InvokeTraits<TInterface>::ArgsTuple> arg2,
            typename std::tuple_element_t<2, typename InvokeTraits<TInterface>::ArgsTuple> arg3,
            typename std::tuple_element_t<3, typename InvokeTraits<TInterface>::ArgsTuple> arg4
            ) override {
            return m_callback(arg1, arg2, arg3, arg4);
        }

    private:
        TCallback m_callback;
        LONG m_refCount;
    };

    // Specialization: Invoke method with 5 arguments
    template <typename TInterface, const IID& IID_TInterface, typename TCallback>
    class ComCallbackImpl<TInterface, IID_TInterface, TCallback, 5> : public TInterface {
    public:
        explicit ComCallbackImpl(TCallback callback)
            : m_callback(std::move(callback)), m_refCount(1) {
        }
        virtual ~ComCallbackImpl() = default;

        STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
            if (riid == IID_IUnknown || riid == IID_TInterface) {
                *ppv = static_cast<TInterface*>(this);
                AddRef();
                return S_OK;
            }
            return E_NOINTERFACE;
        }

        STDMETHOD_(ULONG, AddRef)() override { return InterlockedIncrement(&m_refCount); }
        STDMETHOD_(ULONG, Release)() override {
            ULONG ref = InterlockedDecrement(&m_refCount);
            if (ref == 0) {
                delete this;
            }
            return ref;
        }

        // Implements the 5-argument Invoke
        STDMETHOD(Invoke)(
            typename std::tuple_element_t<0, typename InvokeTraits<TInterface>::ArgsTuple> arg1,
            typename std::tuple_element_t<1, typename InvokeTraits<TInterface>::ArgsTuple> arg2,
            typename std::tuple_element_t<2, typename InvokeTraits<TInterface>::ArgsTuple> arg3,
            typename std::tuple_element_t<3, typename InvokeTraits<TInterface>::ArgsTuple> arg4,
            typename std::tuple_element_t<4, typename InvokeTraits<TInterface>::ArgsTuple> arg5
            ) override {
            return m_callback(arg1, arg2, arg3, arg4, arg5);
        }

    private:
        TCallback m_callback;
        LONG m_refCount;
    };

    // Specialization: Invoke method with 6 arguments
    template <typename TInterface, const IID& IID_TInterface, typename TCallback>
    class ComCallbackImpl<TInterface, IID_TInterface, TCallback, 6> : public TInterface {
    public:
        explicit ComCallbackImpl(TCallback callback)
            : m_callback(std::move(callback)), m_refCount(1) {
        }
        virtual ~ComCallbackImpl() = default;

        STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
            if (riid == IID_IUnknown || riid == IID_TInterface) {
                *ppv = static_cast<TInterface*>(this);
                AddRef();
                return S_OK;
            }
            return E_NOINTERFACE;
        }

        STDMETHOD_(ULONG, AddRef)() override { return InterlockedIncrement(&m_refCount); }
        STDMETHOD_(ULONG, Release)() override {
            ULONG ref = InterlockedDecrement(&m_refCount);
            if (ref == 0) {
                delete this;
            }
            return ref;
        }

        // Implements the 6-argument Invoke
        STDMETHOD(Invoke)(
            typename std::tuple_element_t<0, typename InvokeTraits<TInterface>::ArgsTuple> arg1,
            typename std::tuple_element_t<1, typename InvokeTraits<TInterface>::ArgsTuple> arg2,
            typename std::tuple_element_t<2, typename InvokeTraits<TInterface>::ArgsTuple> arg3,
            typename std::tuple_element_t<3, typename InvokeTraits<TInterface>::ArgsTuple> arg4,
            typename std::tuple_element_t<4, typename InvokeTraits<TInterface>::ArgsTuple> arg5,
            typename std::tuple_element_t<5, typename InvokeTraits<TInterface>::ArgsTuple> arg6
            ) override {
            return m_callback(arg1, arg2, arg3, arg4, arg5, arg6);
        }

    private:
        TCallback m_callback;
        LONG m_refCount;
    };

    // ------------------------------
    // Factory function: creates a callback instance
    // ------------------------------

    template <typename TInterface, const IID& IID_TInterface, typename TCallback>
    ComPtr<TInterface> ComCallback(TCallback&& callback) {
        // Verify that the interface must contain an Invoke method
        static_assert(InvokeTraits<TInterface>::HasInvoke,
            "接口必须定义 STDMETHODCALLTYPE 调用约定的 Invoke 方法");

        // Verify that the argument count does not exceed 6
        static_assert(InvokeTraits<TInterface>::ArgCount <= 6,
            "Invoke 方法最多支持 6 个参数");

        // Instantiate the implementation class for the corresponding argument count
        using Impl = ComCallbackImpl<
            TInterface,
            IID_TInterface,
            std::decay_t<TCallback>,
            InvokeTraits<TInterface>::ArgCount
        >;

        return ComPtr<TInterface>(new Impl(std::forward<TCallback>(callback)));
    }

    // Version supporting member functions (matches the STDMETHODCALLTYPE calling convention)
    template <typename TInterface, const IID& IID_TInterface, typename T, typename... Args>
    ComPtr<TInterface> ComCallback(T* obj, HRESULT(STDMETHODCALLTYPE T::* method)(Args...)) {
        return ComCallback<TInterface, IID_TInterface>(
            [obj, method](Args... args) -> HRESULT {
                return (obj->*method)(args...);
            }
        );
    }

} //namespace ui

#endif //DUI_BUILD_FOR_WEBVIEW2

#endif //UI_WEBVIEW2_COM_CALLBACK_H_
