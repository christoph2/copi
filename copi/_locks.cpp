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
#include "copi.hpp"

#include <iostream>


namespace COPI {


ScopedLock::ScopedLock(Lock const &lock)
{
    m_lock = lock;
    m_lock.acquire();
}

ScopedLock::~ScopedLock()
{
    m_lock.release();
}

namespace win {

CriticalSection::CriticalSection(DWORD spincount)
{
    InitializeCriticalSectionAndSpinCount(&m_crit_section, spincount);
    m_is_locked = false;
}

CriticalSection::~CriticalSection()
{
    if (m_is_locked) {
//        std::cout << "\tdtor() release locked" << &m_crit_section << std::endl;
        release();
    }
    DeleteCriticalSection(&m_crit_section);
}

void CriticalSection::acquire()
{
    EnterCriticalSection(&m_crit_section);
    m_is_locked = true;
//    std::cout << "acquire() " << &m_crit_section << std::endl;
}

void CriticalSection::release()
{
//    std::cout << "release() " << &m_crit_section << std::endl;
    if (!m_is_locked) {
//        std::cout << "\tnot locked" << &m_crit_section << std::endl;
        return;
    }
    LeaveCriticalSection(&m_crit_section);
    m_is_locked = false;
}

bool CriticalSection::try_acquire()
{
    m_is_locked = (TryEnterCriticalSection(&m_crit_section) == TRUE);
    return m_is_locked;
}

class CBenaphore
{
public:
    CBenaphore()
    {
        m_counter = 0;
        m_semaphore = CreateSemaphore(NULL, 0, 1, NULL);
    }

    ~CBenaphore()
    {
        CloseHandle(m_semaphore);
    }

    void acquire()
    {
        if (ATOMIC_INCR(&m_counter) > 1)
        {
            WaitForSingleObject(m_semaphore, INFINITE);
        }
    }

    void release()
    {
        if (ATOMIC_DECR(&m_counter) > 0)
        {
            ReleaseSemaphore(m_semaphore, 1, NULL);
        }
    }

    bool try_acquire()
    {
        LONG result = ATOMIC_CAS(&m_counter, 1, 0);
        return (result == 0);
    }

private:
    volatile LONG m_counter;
    HANDLE m_semaphore;
};


}

}
