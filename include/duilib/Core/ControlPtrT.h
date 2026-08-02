#ifndef UI_CORE_CONTROL_PTR_T_H_
#define UI_CORE_CONTROL_PTR_T_H_

#include "duilib/Core/Callback.h"

namespace ui 
{
/** Control pointer validity protection object, used in the same way as a raw pointer, but can avoid the problem of dangling pointers
*/
template<typename T>
class ControlPtrT
{
public:
    explicit ControlPtrT(std::nullptr_t):
        m_pControl(nullptr)
    {
    }
    explicit ControlPtrT(T* pControl) :
        m_pControl(pControl)
    {
        if (pControl != nullptr) {
            m_weak_flag = pControl->GetWeakFlag();
        }
    }
    ControlPtrT():
        m_pControl(nullptr)
    {
    }
    
    ControlPtrT(const ControlPtrT& r):
        m_pControl(r.get())
    {
        if (m_pControl != nullptr) {
            m_weak_flag = m_pControl->GetWeakFlag();
        }
    }

    ControlPtrT& operator=(const ControlPtrT& r)
    {
        m_pControl = r.get();
        if (m_pControl != nullptr) {
            m_weak_flag = m_pControl->GetWeakFlag();
        }
        else {
            m_weak_flag.reset();
        }
        return *this;
    }

    ~ControlPtrT() = default;

    /** Assignment operator
    */
    ControlPtrT& operator = (std::nullptr_t)
    {
        m_pControl = nullptr;
        m_weak_flag.reset();
        return *this;
    }

    ControlPtrT& operator = (T* pControl)
    {
        m_pControl = pControl;
        if (pControl != nullptr) {
            m_weak_flag = pControl->GetWeakFlag();
        }
        else {
            m_weak_flag.reset();
        }
        return *this;
    }
    
public:
    /** Check whether the raw pointer of the control has expired
    @return Returns true if it has expired, false if it has not expired
    */
    bool expired() const
    {
        return m_weak_flag.expired();
    }
    
    /** Get the raw pointer
    */
    T* get() const
    {
        if (m_weak_flag.expired()) {
            return nullptr;
        }
        return m_pControl;
    }
    
    /** Use the raw pointer
    */
    T* operator->() const
    {
        if (m_weak_flag.expired()) {
            return nullptr;
        }
        return m_pControl;
    }

    /** Clear
    */
    void reset()
    {
        m_weak_flag.reset();
        m_pControl = nullptr;
    }

    /** Comparison operator
    */
    bool operator == (std::nullptr_t) const
    {
        return get() == nullptr;
    }

    bool operator == (T* pControl) const
    {
        return get() == pControl;
    }

    /** Comparison operator
    */
    bool operator == (const ControlPtrT& r) const
    {
        return get() == r.get();
    }

    /** Comparison operator
    */
    bool operator != (std::nullptr_t) const
    {
        return get() != nullptr;
    }

    bool operator != (T* pControl) const
    {
        return get() != pControl;
    }

    /** Comparison operator
    */
    bool operator != (const ControlPtrT& r) const
    {
        return get() != r.get();
    }

    /** Comparison operator (global)
    */
    friend inline bool operator == (std::nullptr_t, const ControlPtrT<T>& r) {
        return r.get() == nullptr;
    }

    friend inline bool operator == (T* pControl, const ControlPtrT<T>& r) {
        return r.get() == pControl;
    }

    /** Comparison operator (global)
    */
    friend inline bool operator != (std::nullptr_t, const ControlPtrT<T>& r) {
        return r.get() != nullptr;
    }

    friend inline bool operator != (T* pControl, const ControlPtrT<T>& r) {
        return r.get() != pControl;
    }
    
private:
    /** The raw pointer of the control
    */
    T* m_pControl;
    
    /** Lifecycle protection
    */
    std::weak_ptr<WeakFlag> m_weak_flag;
};

/** The smart pointer of the control
*/
class Control;
typedef ControlPtrT<Control> ControlPtr;

/** The smart pointer of the container
*/
class Box;
typedef ControlPtrT<Box> BoxPtr;

/** The smart pointer of the window
*/
class Window;
typedef ControlPtrT<Window> WindowPtr;

} // namespace ui

#endif // UI_CORE_CONTROL_PTR_T_H_
