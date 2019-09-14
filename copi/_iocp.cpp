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

#include <string>

#include <cstdio>
#include <cstddef>


/*
 *
 *
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


IOCP::IOCP(DWORD numProcessors)
{
    std::cout << "IOCP c-tor" << std::endl;
}

IOCP::~IOCP()
{
    std::cout << "IOCP d-tor" << std::endl;
}

bool IOCP::Register(PerHandleData * object)
{
    return true;
}

void IOCP::PostQuitMessage() const
{

}

bool Create(PerPortData & port, DWORD numProcessors)
{
    bool result = true;

    port.handle  = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)0, numProcessors);
    if (port.handle == NULL) {
        result = false;
    }

    return result;
}


bool RegisterHandle(PerPortData port, PerHandleData * object)
{
    HANDLE handle;

    //PerHandleData data;

    handle = ::CreateIoCompletionPort(object, object->handle, reinterpret_cast<ULONG_PTR>(object), 0);
    if (handle == NULL) {
        // TODO: raise!?
    }
    return (handle == port.handle);
}

void IOCP_PostMessage(PerPortData port)
{
    ::PostQueuedCompletionStatus(port.handle, 0, (ULONG_PTR)NULL, NULL);
}

void IOCP_PostQuitMessage(PerPortData port)
{
    ::PostQueuedCompletionStatus(port.handle, 0, (ULONG_PTR)NULL, NULL);
}

#if 0
std::vector<char> myData;
for (;;) {
    const int BufferSize = 1024;

    const size_t oldSize = myData.size();
    myData.resize(myData.size() + BufferSize);

    const unsigned bytesRead = get_network_data(&myData[oldSize], BufferSize);
    myData.resize(oldSize + bytesRead);

    if (bytesRead == 0) {
        break;
    }
}


std::vector<char> myData;
for (;;) {
    const int BufferSize = 1024;
    char rawBuffer[BufferSize];

    const unsigned bytesRead = get_network_data(rawBuffer, sizeof(rawBuffer));
    if (bytesRead <= 0) {
        break;
    }

    myData.insert(myData.end(), rawBuffer, rawBuffer + bytesRead);
}


//Finally, if you need to treat your data as a raw-array:
some_c_function(myData.data(), myData.size());
//std::vector is guaranteed to be contiguous.
//

std::vector<char> buffer;
static const size_t MaxBytesPerRecv = 1024;
size_t bytesRead;
do
{
    const size_t oldSize = buffer.size();

    buffer.resize(oldSize + MaxBytesPerRecv);
    bytesRead = receive(&buffer[oldSize], MaxBytesPerRecv); // pseudo, as is the case with winsock recv() functions, they get a buffer and maximum bytes to write to the buffer

    myData.resize(oldSize + bytesRead); // shrink the vector, this is practically no-op - it only modifies the internal size, no data is moved/freed
} while (bytesRead > 0);
#endif
}


