/*
BSD License

Copyright (c) 2019, Christoph Schueler
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#if !defined(__LOCKS_HPP)
#define __LOCKS_HPP

#include "copi.hpp"
#include <MSWSock.h>

namespace COPI {

/** @brief Interface for locks.
 *
 *
 */
class ILock {
public:

    /** @brief Virtual destructor.
     *
     */
    virtual ~ILock() {}

    /** @brief Acquire lock.
     *
     */
    virtual void acquire()= 0;

    /** @brief Release lock.
     *
     */
    virtual void release()= 0;

    /** @brief Try to acquire lock.
     *
     * @return bool
     *      @li true - succeeded
            @li false - failed
     */
    virtual bool try_acquire()= 0;
};

/** @brief Very simplistic scoped lock.
 *
 *  Lock gets acquired in ctor and released in dtor, i.e. when ScopedLock goes out of scope.
 *
 */
class ScopedLock {
public:

    /** @brief Construct ScopedLock.
     *
     * @param lock Points to an instance implementing ILock interface.
     *
     */
    ScopedLock(ILock * lock) {
        m_lock = lock;
        m_lock->acquire();
    }

    ~ScopedLock() {
        m_lock->release();
    }
private:
    ILock * m_lock;
};

namespace win {

/** @brief A mutex based on Windows critical sections.
 *
 *  Implements ILock.
 */
class CriticalSection : public ILock {
public:
    CriticalSection(DWORD spincount = 0) {
        ::InitializeCriticalSectionAndSpinCount(&m_crit_section, spincount);
        m_is_locked = false;
    }

    ~CriticalSection() {
        if (m_is_locked) {
            release();
        }
        ::DeleteCriticalSection(&m_crit_section);
    }

    void acquire() {
        ::EnterCriticalSection(&m_crit_section);
        m_is_locked = true;
    }

    void release() {
        if (!m_is_locked) {
            return;
        }
        ::LeaveCriticalSection(&m_crit_section);
        m_is_locked = false;
    }

    bool try_acquire() {
        m_is_locked = ::TryEnterCriticalSection(&m_crit_section) == TRUE;
        return m_is_locked;
    }

private:
    CRITICAL_SECTION m_crit_section;
    bool m_is_locked;
};

#if 0
class Mutex : public ILock {
    Mutex();
    ~Mutex();
};

class Spinlock : public ILock {
    Spinlock();
    ~Spinlock();
};
#endif // 0

}

}

#endif  // __LOCKS_HPP

