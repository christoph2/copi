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

namespace COPI {

static DWORD WINAPI WorkerThread(LPVOID lpParameter);

IOCP::IOCP(DWORD numProcessors)
{
    m_port.handle  = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, static_cast<ULONG_PTR>(0), numProcessors);
    if (m_port.handle == NULL) {
        throw WindowsException();
    }
    CSystemInformation si = CSystemInformation();

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
    DWORD numThreads = static_cast<DWORD>(m_threads.size());
    std::ldiv_t divres = std::ldiv(numThreads, MAXIMUM_WAIT_OBJECTS);
    DWORD rounds = static_cast<DWORD>(divres.quot);
    DWORD remaining = static_cast<DWORD>(divres.rem);
    HANDLE * thrArray = NULL;
    DWORD offset = 0;

    postQuitMessage();

    for (DWORD r = 0; r < rounds; ++r) {
        thrArray = new HANDLE[MAXIMUM_WAIT_OBJECTS];
        for (DWORD idx = 0; idx < MAXIMUM_WAIT_OBJECTS; ++idx) {
            thrArray[idx] = m_threads.at(idx + offset);
        }
        WaitForMultipleObjects(MAXIMUM_WAIT_OBJECTS, thrArray, TRUE, INFINITE);
        delete[] thrArray;
        offset += MAXIMUM_WAIT_OBJECTS;
    }

    if (remaining > 0) {
        thrArray = new HANDLE[numThreads];
        for (DWORD idx = 0; idx < remaining; ++idx) {
            thrArray[idx] = m_threads.at(idx + offset);
        }
        WaitForMultipleObjects(remaining, thrArray, TRUE, INFINITE);
        delete[] thrArray;
    }
    CloseHandle(m_port.handle);
}

bool IOCP::registerHandle(PerHandleData * object)
{
    HANDLE handle;

    handle = ::CreateIoCompletionPort(object->m_handle, m_port.handle, reinterpret_cast<ULONG_PTR>(object), 0);
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
    DWORD numBytesRecv = 0;
    ULONG_PTR CompletionKey;
    CPerIOData * iod = NULL;
    OVERLAPPED * olap = NULL;
    bool exitLoop = FALSE;
    static WSABUF wsaBuffer;
    DWORD flags = (DWORD)0;
    DWORD error;


    printf("Entering thread with [%p] [%d]...\n", iocp, iocp->getHandle());
    while (!exitLoop) {
        if (::GetQueuedCompletionStatus(iocp->getHandle(), &numBytesRecv, &CompletionKey, (LPOVERLAPPED*)&olap, INFINITE)) {
            //printf("Got Event: %ld %ld\n", numBytesRecv, CompletionKey);
            if ((numBytesRecv == 0) &&  (CompletionKey == NULL)) {
                iocp->postQuitMessage();    // "Broadcast"
                exitLoop = TRUE;
            } else {

                iod = (CPerIOData*)olap;
                printf("\tOPCODE: %d bytes: %d\n", iod->m_opcode, numBytesRecv);
                switch (iod->m_opcode) {
                    case IO_WRITE:
                        iod->m_bytesRemaining -= numBytesRecv;
                        iod->m_socket->triggerRecv(1024);
                        if (iod->m_bytesRemaining == 0) {
                            delete iod;
                        } else {
                            iod->m_wsabuf.buf = iod->m_wsabuf.buf + (iod->m_bytesToXfer - iod->m_bytesRemaining);
                            iod->m_wsabuf.len = iod->m_bytesRemaining;
                            iod->resetOverlapped();
                        }
                        break;
                    case IO_READ:
                        printf("IO_READ() numBytes: %d\n", numBytesRecv);
                        break;
                    case IO_ACCEPT:
                        break;
                }
            }
        } else {
            error = GetLastError();
            if (olap == NULL) {

            } else {
                // Failed I/O operation.
                // The function stores information in the variables pointed to by lpNumberOfBytes, lpCompletionKey.
            }
            Win_ErrorMsg("IOWorkerThread::GetQueuedCompletionStatus()", error);
        }
    }
    printf("Exiting thread...\n");
    ExitThread(0);
    return 0;
}
}
