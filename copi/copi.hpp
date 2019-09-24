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

#if !defined(__COPI_HPP)
#define __COPI_HPP


#define _WIN32_WINNT    0x601
//#include <Windows.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Mstcpip.h>

#if _MSC_VER > 1500
    #include <intrin.h>
    #define ATOMIC_INCR(v)          ::_InterlockedIncrement((v))
    #define ATOMIC_DECR(v)          ::_InterlockedDecrement((v))
    #define ATOMIC_CAS(v, a, b)     ::_InterlockedCompareExchange((v), (a), (b))
#else
    #define ATOMIC_INCR(v)          ::InterlockedIncrement((v))
    #define ATOMIC_DECR(v)          ::InterlockedDecrement((v))
    #define ATOMIC_CAS(v, a, b)     ::InterlockedCompareExchange((v), (a), (b))
#endif

#include "_exceptions.hpp"
#include "_iocp.hpp"
#include "_helper.hpp"
#include "_locks.hpp"
#include "_condition_variable.hpp"
#include "_queue.hpp"
#include "_file.hpp"
#include "_mmap.hpp"
#include "_socket.hpp"
#include "_wsock.hpp"

#endif // __COPI_HPP
