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
#if !defined(__CONDITION_VARIABLE_HPP)
#define __CONDITION_VARIABLE_HPP

/**
 *
 *  Based on: "Strategies for Implementing POSIX Condition Variables on Win32 /
    Douglas C. Schmidt and Irfan Pyarali".
 */


#include "copi.hpp"

#include <iostream>

namespace IOCP {

namespace detail {
    const unsigned int CV_EVENT_COUNT   = 2;

    const unsigned int CV_SIGNAL        = 0;
    const unsigned int CV_BROADCAST     = 1;
}

class CConditionVariable {
public:
    CConditionVariable(DWORD spincount = 0) : m_waiters_count(0), m_lock(spincount) {
        m_events[detail::CV_SIGNAL]     = ::CreateEvent (NULL, FALSE, FALSE, NULL);   // Auto-reset.
        m_events[detail::CV_BROADCAST]  = ::CreateEvent (NULL, TRUE, FALSE, NULL);    // Manual-reset
    }

    ~CConditionVariable() {
        for (int idx = 0; idx < detail::CV_EVENT_COUNT; ++idx) {
            ::CloseHandle(m_events[idx]);
        }
    }

    void notify_one() {
        notify_(detail::CV_SIGNAL);
    }

    void notify_all() {
        notify_(detail::CV_BROADCAST);
    }

    bool wait(Lock & externalLock, DWORD millis = INFINITE) {
        DWORD res;
        bool last_waiter;

        incrWaitersCount();
        externalLock.release();

        res = ::WaitForMultipleObjects(detail::CV_EVENT_COUNT, m_events, FALSE, millis);
        if (res == WAIT_TIMEOUT) {
            externalLock.acquire();
            return false;
        }

        m_lock.acquire();
        decrWaitersCount();
        last_waiter = (res == WAIT_OBJECT_0 + detail::CV_BROADCAST) && (m_waiters_count) == 0;
        m_lock.release();
        if (last_waiter) {
            ResetEvent(m_events[detail::CV_BROADCAST]);
        }
        externalLock.acquire();
        return true;
    }

protected:
    inline void incrWaitersCount() {
        ::InterlockedIncrement(&m_waiters_count);
    }

    inline void decrWaitersCount() {
        ::InterlockedDecrement(&m_waiters_count);
    }

    inline void notify_(unsigned int event) {
        bool have_waiters;

        m_lock.acquire();
        have_waiters = (m_waiters_count > 0);
        m_lock.release();
        if (have_waiters) {
            SetEvent (m_events[event]);
        }
    }

private:
    volatile LONG m_waiters_count;
    win::CriticalSection m_lock;
    HANDLE m_events[detail::CV_EVENT_COUNT];
};

}

#endif // __CONDITION_VARIABLE_HPP

