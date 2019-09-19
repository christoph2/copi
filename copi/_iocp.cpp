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

void PyInit__copi(void)
{

}
}

namespace IOCP {

static DWORD WINAPI WorkerThread(LPVOID lpParameter);

IOCP::IOCP(DWORD numProcessors)
{
    m_port.handle  = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, static_cast<ULONG_PTR>(0), numProcessors);
    if (m_port.handle == NULL) {
        throw WindowsException();
    }
    SystemInformation si = SystemInformation();

    if (numProcessors == 0) {
        numProcessors = si.getNumberOfProcessors();
    }
    m_numWorkerThreads = numProcessors * 2; // Hard-coded for now.

    //m_numWorkerThreads = 553;

    m_threads.reserve(m_numWorkerThreads);

    HANDLE hThread;

    for (DWORD idx = 0; idx < m_numWorkerThreads; ++idx) {
        hThread = ::CreateThread(NULL, 0, WorkerThread, reinterpret_cast<LPVOID>(this), 0, NULL);
        SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
        m_threads.push_back(hThread);
    }
}

IOCP::~IOCP()
{
    DWORD numThreads = m_threads.size();
    std::ldiv_t divres = std::ldiv(numThreads, MAXIMUM_WAIT_OBJECTS);
    HANDLE * thrArray = NULL;

    DWORD offset = 0;

    postQuitMessage();

    for (DWORD r = 0; r < divres.quot; ++r) {
        thrArray = new HANDLE[MAXIMUM_WAIT_OBJECTS];
        for (DWORD idx = 0; idx < MAXIMUM_WAIT_OBJECTS; ++idx) {
            thrArray[idx] = m_threads.at(idx + offset);
        }
        WaitForMultipleObjects(MAXIMUM_WAIT_OBJECTS, thrArray, TRUE, INFINITE);
        delete[] thrArray;
        offset += MAXIMUM_WAIT_OBJECTS;
    }

    if (divres.rem > 0) {
        thrArray = new HANDLE[numThreads];
        for (DWORD idx = 0; idx < divres.rem; ++idx) {
            thrArray[idx] = m_threads.at(idx + offset);
        }
        WaitForMultipleObjects(divres.rem, thrArray, TRUE, INFINITE);
        delete[] thrArray;
    }
    CloseHandle(m_port.handle);
}

bool IOCP::registerHandle(PerHandleData * object)
{
    HANDLE handle;

    handle = ::CreateIoCompletionPort(object->handle, m_port.handle, reinterpret_cast<ULONG_PTR>(object), 0);
    printf("Registered Handle: %p\n", handle);
    if (handle == NULL) {
        throw WindowsException();
    }
    return (handle == m_port.handle);
}

void IOCP::postQuitMessage() const
{
    if (!::PostQueuedCompletionStatus(m_port.handle, 0, static_cast<ULONG_PTR>(NULL), NULL)) {
        throw WindowsException();
    }
}

HANDLE IOCP::getHandle() const
{
    return m_port.handle;
}

void IOCP::postUserMessage() const
{
    ::PostQueuedCompletionStatus(m_port.handle, 0, static_cast<ULONG_PTR>(NULL), NULL);
}


static DWORD WINAPI WorkerThread(LPVOID lpParameter)
{
    IOCP const * const iocp = reinterpret_cast<IOCP const * const>(lpParameter);
    DWORD NumBytesRecv = 0;
    ULONG CompletionKey = (ULONG_PTR)0;
    PerIOData * iod = NULL;
    OVERLAPPED * ov = NULL;
    bool exitLoop = FALSE;
//    char receiveBuffer[XCP_COMM_BUFLEN];
    static WSABUF wsaBuffer;
    DWORD flags = (DWORD)0;
    DWORD numReceived;
//    uint16_t dlc;


//    wsaBuffer.buf = receiveBuffer;
//    wsaBuffer.len = XCP_COMM_BUFLEN;

    printf("Entering thread with [%p] [%d]...\n", iocp, iocp->getHandle());
    while (!exitLoop) {
        if (::GetQueuedCompletionStatus(iocp->getHandle(), &NumBytesRecv, &CompletionKey, (LPOVERLAPPED*)&ov, INFINITE)) {
            printf("Got Event: %ld %ld", NumBytesRecv, CompletionKey);
            if ((NumBytesRecv == 0) &&  (CompletionKey == 0)) {
                iocp->postQuitMessage();    // "Broadcast"
                exitLoop = TRUE;
            } else {
#if 0
                iod = (PerIoData*)ov;
                //printf("\tOPCODE: %d\n", iod->opcode);
                switch (iod->opcode) {
                    case IoRead:
                        WSARecv(XcpTl_Connection.connectedSocket,
                                &wsaBuffer,
                                1,
                                &numReceived,
                                &flags,
                                (LPWSAOVERLAPPED)NULL,
                                (LPWSAOVERLAPPED_COMPLETION_ROUTINE)NULL
                        );
                        if (numReceived == (DWORD)0) {
                            DBG_PRINT1("Client closed connection\n");
                            //if (!CancelIo((HANDLE)XcpTl_Connection.connectedSocket)) {
                            //    printf("Cancelation failed.\n");
                            //}
                            XcpTl_Connection.socketConnected = XCP_FALSE;
                            closesocket(XcpTl_Connection.connectedSocket);
                            Xcp_Disconnect();
                        }
                        XcpTl_Receive(0);
                        //XcpUtl_Hexdump(buf, numReceived);
                        if (numReceived > 0) {
#if XCP_TRANSPORT_LAYER_LENGTH_SIZE == 1
                            dlc = (uint16_t)buf[0];
#elif XCP_TRANSPORT_LAYER_LENGTH_SIZE == 2
                            dlc = MAKEWORD(buf[0], buf[1]);
                            //dlc = (uint16_t)*(buf + 0);
#endif // XCP_TRANSPORT_LAYER_LENGTH_SIZE
                            if (!XcpTl_Connection.xcpConnected || (XcpTl_VerifyConnection())) {
                                Xcp_PduIn.len = dlc;
                                Xcp_PduIn.data = buf + XCP_TRANSPORT_LAYER_BUFFER_OFFSET;
                                Xcp_DispatchCommand(&Xcp_PduIn);
                            }
                            if (numReceived < 5) {
                                DBG_PRINT2("Error: frame to short: %d\n", numReceived);
                            } else {

                            }
                            fflush(stdout);
                        }
                        break;
                    case IoAccept:
                        break;
                    case IoWrite:
                        break;
                }
#endif // 0
            }
        } else {
            //Win_ErrorMsg("IOWorkerThread::GetQueuedCompletionStatus()", GetLastError());
            exitLoop = TRUE;

        }
    }
    printf("Exiting thread...\n");
    ExitThread(0);
    return 0;
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
