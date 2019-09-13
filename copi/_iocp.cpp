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
#include "_iocp.hpp"

#include <string>

#include <cstdio>
#include <cstddef>
#include <cstdint>

/*
 *
 *  Note: Object-orientation is handed over to Cython.
 *
 */

extern "C" {
void PyInit__iocp(void)
{

}
}

namespace IOCP {


void Win_ErrorMsg(const std::string & function, DWORD error)
{
    char buffer[1024];

    ::FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buffer,
        1024,
        NULL
    );
    ::fprintf(stderr, "[%s] failed with: [%d] %s", function.c_str(), error, buffer);

}

bool Create(PerPortData & port)
{
    bool result = true;

    port.handle  = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)0, 1);
    if (port.handle == NULL) {
        result = false;
    }

    return result;
}


bool RegisterHandle(HANDLE port, HANDLE object, ULONG_PTR key)
{
    ::HANDLE handle;

    handle = ::CreateIoCompletionPort(object, port, key, 0);
    if (handle == NULL) {
        Win_ErrorMsg("XcpTl_RegisterIOCPHandle::CreateIoCompletionPort()", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    return (handle == port);
}

#if 0
void IOCP_PostQuitMessage(void)
{
    ::PostQueuedCompletionStatus(XcpTl_Connection.iocp, 0, (ULONG_PTR)NULL, NULL);
}
#endif
}

