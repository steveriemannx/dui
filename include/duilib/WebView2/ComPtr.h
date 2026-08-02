#ifndef UI_WEBVIEW2_COM_PTR_H_
#define UI_WEBVIEW2_COM_PTR_H_

#include "duilib/duilib_defs.h"

#if defined (DUILIB_BUILD_FOR_WIN) && defined (DUILIB_BUILD_FOR_WEBVIEW2)

#include <cstddef>
#include <utility>

namespace ui
{
/** Lightweight COM smart pointer that automatically manages the lifetime of COM objects
 *  Automatically manages the reference counting of COM objects to avoid memory leaks
 */
template<typename T>
class ComPtr
{
public:
    typedef T Interface;

    /** Default constructor, initializes to a null pointer
     */
    ComPtr(): m_pInterface(nullptr) {}
    
    /** Null pointer constructor
     */
    ComPtr(std::nullptr_t) : m_pInterface(nullptr) {}

    /** Constructs from a raw COM pointer
     * @param pInterface Raw COM interface pointer; increments the reference count
     */
    explicit ComPtr(T* pInterface) : m_pInterface(pInterface)
    {
        if (m_pInterface) {
            m_pInterface->AddRef(); // increment the reference count when taking ownership
        }
    }

    /** Copy constructor
     * @param other Another ComPtr instance sharing the same COM object
     */
    ComPtr(const ComPtr& other) : m_pInterface(other.m_pInterface)
    {
        if (m_pInterface) {
            m_pInterface->AddRef(); // increment the reference count on copy
        }
    }

    /** Template copy constructor, supports construction from a compatible interface type
     * @tparam U Source interface type, must be convertible to T
     * @param other Source ComPtr instance
     */
    template<typename U>
    ComPtr(const ComPtr<U>& other) : m_pInterface(other.Get())
    {
        if (m_pInterface) {
            m_pInterface->AddRef(); // increment the reference count when copying across interface types
        }
    }

    /** Move constructor
     * @param other The ComPtr instance to move from; becomes empty after the move
     */
    ComPtr(ComPtr&& other) noexcept : m_pInterface(other.m_pInterface)
    {
        other.m_pInterface = nullptr; // transfer ownership, set the original pointer to null
    }

    /** Template move constructor, supports moving from a compatible interface type
     * @tparam U Source interface type, must be convertible to T
     * @param other The ComPtr instance to move from
     */
    template<typename U>
    ComPtr(ComPtr<U>&& other) noexcept : m_pInterface(other.Detach()) {}

    /** Destructor, automatically releases the held COM object
     */
    ~ComPtr()
    {
        if (m_pInterface) {
            m_pInterface->Release(); // decrement the reference count when releasing ownership
        }
    }

    /** Null pointer assignment operator
     */
    ComPtr& operator=(std::nullptr_t)
    {
        Reset();
        return *this;
    }

    /** Raw pointer assignment operator
     * @param pInterface Raw COM interface pointer
     */
    ComPtr& operator=(T* pInterface)
    {
        if (m_pInterface != pInterface) {
            ComPtr temp(pInterface);
            Swap(temp); // use a temporary object for exception-safe assignment
        }
        return *this;
    }

    /** Copy assignment operator
     * @param other Another ComPtr instance
     */
    ComPtr& operator=(const ComPtr& other)
    {
        if (this != &other) {
            ComPtr temp(other);
            Swap(temp); // use the copy-and-swap technique for exception safety
        }
        return *this;
    }

    /** Template copy assignment operator
     * @tparam U Source interface type
     * @param other Source ComPtr instance
     */
    template<typename U>
    ComPtr& operator=(const ComPtr<U>& other)
    {
        ComPtr temp(other);
        Swap(temp);
        return *this;
    }

    /** Move assignment operator
     * @param other The ComPtr instance to move from
     */
    ComPtr& operator=(ComPtr&& other) noexcept
    {
        ComPtr temp(std::move(other));
        Swap(temp);
        return *this;
    }

    /** Template move assignment operator
     * @tparam U Source interface type
     * @param other The ComPtr instance to move from
     */
    template<typename U>
    ComPtr& operator=(ComPtr<U>&& other) noexcept
    {
        ComPtr temp(std::move(other));
        Swap(temp);
        return *this;
    }

    /** Gets the pointer address, used to receive the output of a COM object creation function
     * @note Calling this method resets the current pointer
     */
    T** GetAddressOf()
    {
        Reset();
        return &m_pInterface;
    }

    /** Gets the const pointer address, for const objects
     */
    T* const* GetAddressOf() const
    {
        return &m_pInterface;
    }

    /** Releases the current object and gets the pointer address
     * @note Calling this method resets the current pointer
     */
    T** ReleaseAndGetAddressOf()
    {
        Reset();
        return &m_pInterface;
    }

    /** Detaches the current pointer, relinquishes ownership without releasing the object
     * @return The raw COM interface pointer
     */
    T* Detach()
    {
        T* pInterface = m_pInterface;
        m_pInterface = nullptr;
        return pInterface;
    }

    /** Resets the pointer to null and releases the held COM object
     */
    void Reset()
    {
        ComPtr().Swap(*this); // use a temporary object swap for safe release
    }

    /** Resets the pointer to a new value, releasing the original object
     * @param pInterface New COM interface pointer
     */
    void Reset(T* pInterface)
    {
        ComPtr(pInterface).Swap(*this);
    }

    /** Swaps the contents of two ComPtr instances
     * @param other The other ComPtr instance to swap with
     */
    void Swap(ComPtr& other) noexcept
    {
        T* temp = m_pInterface;
        m_pInterface = other.m_pInterface;
        other.m_pInterface = temp;
    }

    /** Gets the raw COM interface pointer
     * @return Raw COM interface pointer, may be nullptr
     */
    T* Get() const
    {
        return m_pInterface;
    }

    /** Gets the pointer to the raw COM interface pointer
    */
    T** operator&() noexcept {
        if (m_pInterface) {
            m_pInterface->Release();
            m_pInterface = nullptr;
        }
        return &m_pInterface;
    }

    /** Overloaded arrow operator, provides direct access to COM interface methods
     */
    T* operator->() const
    {
        return m_pInterface;
    }

    /** Boolean type conversion, for conditional checks
     */
    explicit operator bool() const
    {
        return m_pInterface != nullptr;
    }

    /** Equality comparison operator
     * @tparam U Interface type to compare
     */
    template<typename U>
    bool operator==(const ComPtr<U>& other) const
    {
        return m_pInterface == other.Get();
    }

    /** Compares with a null pointer
     */
    bool operator==(std::nullptr_t) const
    {
        return m_pInterface == nullptr;
    }

    /** Inequality comparison operator
     * @tparam U Interface type to compare
     */
    template<typename U>
    bool operator!=(const ComPtr<U>& other) const
    {
        return !(*this == other);
    }

    /** Compares with a null pointer
     */
    bool operator!=(std::nullptr_t) const
    {
        return !(*this == nullptr);
    }

    /** Queries the interface to obtain another COM interface
     * @tparam U Target interface type
     * @param p Output parameter, receives the target interface pointer
     * @return HRESULT indicating the operation result
     */
    template<typename U>
    HRESULT As(ComPtr<U>* p) const
    {
        if (!p) {
            return E_POINTER;
        }
        return QueryInterface(IID_PPV_ARGS(p));
    }

    /** Queries the interface to obtain another COM interface
     * @tparam U Target interface type
     * @return ComPtr instance containing the target interface
     */
    template<typename U>
    ComPtr<U> As() const
    {
        ComPtr<U> result;
        As(&result);
        return result;
    }

    /** Queries the interface by IID
     * @param riid IID of the target interface
     * @param ppvObject Output parameter, receives the target interface pointer
     * @return HRESULT indicating the operation result
     */
    template<typename U>
    HRESULT AsIID(REFIID riid, void** ppvObject) const
    {
        if (!ppvObject)
        {
            return E_POINTER;
        }
        *ppvObject = nullptr;
        return m_pInterface ? m_pInterface->QueryInterface(riid, ppvObject) : E_NOINTERFACE;
    }

private:
    template<typename U> friend class ComPtr;
    T* m_pInterface; // managed COM interface pointer
};

// Helper functions

/** Creates a ComPtr instance from a raw pointer
 * @tparam T Target interface type
 * @tparam U Source interface type
 * @param p Raw COM interface pointer
 * @return A ComPtr instance
 */
template<typename T, typename U>
ComPtr<T> MakeComPtr(U* p)
{
    return ComPtr<T>(p);
}

/** Creates a ComPtr instance
*/
template<typename T>
ComPtr<T> MakeComPtr()
{
    return ComPtr<T>(new T);
}

/**
 * @brief Helper function for comparing a raw pointer with a ComPtr
 */
template<typename T, typename U>
bool operator==(U* lhs, const ComPtr<T>& rhs)
{
    return lhs == rhs.Get();
}

/** Helper function for comparing a raw pointer with a ComPtr
 */
template<typename T, typename U>
bool operator!=(U* lhs, const ComPtr<T>& rhs)
{
    return !(lhs == rhs);
}

} //namespace ui

#endif //DUILIB_BUILD_FOR_WEBVIEW2

#endif //UI_WEBVIEW2_COM_PTR_H_
