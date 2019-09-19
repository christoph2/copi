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

namespace IOCP {

class Lock {
public:
    Lock() {}
    virtual ~Lock() {}
    virtual void acquire() {}
    virtual void release() {}
    virtual bool try_acquire() { return true; }
};


class ScopedLock {
public:
    ScopedLock(Lock const &lock);
    ~ScopedLock();
private:
    Lock m_lock;
};

namespace win {

class CriticalSection : public Lock {
public:
    CriticalSection();
    ~CriticalSection();
    void acquire();
    void release();
    bool try_acquire();
private:
    CRITICAL_SECTION m_crit_section;
};

#if 0
class Mutex : public Lock {
    Mutex();
    ~Mutex();
};

class Spinlock : public Lock {
    Spinlock();
    ~Spinlock();
};
#endif // 0

}

}

#endif  // __LOCKS_HPP

