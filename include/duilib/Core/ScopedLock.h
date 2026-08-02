#ifndef UI_CORE_SCOPED_LOCK_H_
#define UI_CORE_SCOPED_LOCK_H_

#include <mutex>

namespace ui 
{
/** Wrapper class for automatic unlocking
*/
class DUILIB_API ScopedLock
{
public:
    // The constructor acquires the lock
    explicit ScopedLock(std::mutex& mutex)
        : m_mutex(mutex), m_locked(true)
    {
        m_mutex.lock();
    }

    // The destructor releases the lock automatically
    ~ScopedLock()
    {
        Unlock();
    }

    // Manually unlock method
    void Unlock()
    {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

    // Copying is prohibited
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;

private:
    std::mutex& m_mutex;
    bool m_locked;
};

} // namespace ui

#endif // UI_CORE_SCOPED_LOCK_H_
