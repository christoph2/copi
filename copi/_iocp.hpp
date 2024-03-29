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
#if !defined(__IOCP_HPP)
#define __IOCP_HPP


#include <vector>

#if !defined(__GNUC__)
#pragma comment(lib,"ws2_32.lib") // MSVC only.
#endif

namespace COPI {

enum HandleType {
    HANDLE_SOCKET,
    HANDLE_FILE,
    HANDLE_NAMED_PIPE,
    HANDLE_USER,
};

enum IOType {
    IO_ACCEPT,
    IO_CONNECT,
    IO_READ,
    IO_WRITE
};

struct PerPortData {
    HANDLE handle;
};

class CSocket;

struct CPerHandleData {
    HandleType m_handleType;
    CSocket * m_socket;
    DWORD m_seqNoSend;
    DWORD m_seqNoRecv;

    CPerHandleData(HandleType handleType, CSocket * socket) : m_handleType(handleType),
        m_socket(socket), m_seqNoSend(0), m_seqNoRecv(0) {}

#if 0
    HANDLE getHandle() const {
        return m_socket->getHandle();
    }
#endif // 0
};

struct CPerIOData {
    OVERLAPPED m_overlapped;
    IOType m_opcode;
    WSABUF m_wsabuf;
    char * m_xferBuffer;
    size_t m_bytesToXfer;
    size_t m_bytesRemaining;

    CPerIOData(size_t bufferSize) {
        m_xferBuffer = NULL;
        m_xferBuffer = new char[bufferSize];
        m_wsabuf.buf = m_xferBuffer;
        m_wsabuf.len = bufferSize;
        m_bytesRemaining = 0;
        m_bytesToXfer = 0;
    }

    ~CPerIOData() {
        if (m_xferBuffer) {
            delete[] m_xferBuffer;
        }
    }

    void resetOverlapped() {
        SecureZeroMemory(&m_overlapped, sizeof(OVERLAPPED));
    }
};

struct ThreadType {
    HANDLE handle;
    DWORD id;
};

class IOCP {
public:
    IOCP(DWORD numProcessors = 0);
    ~IOCP();
    bool registerHandle(CPerHandleData * object);
    void postUserMessage() const;
    void postQuitMessage() const;
    HANDLE getHandle() const;
private:
    PerPortData m_port;
    DWORD m_numWorkerThreads;
    std::vector<HANDLE> m_threads;
};

}

#endif // __IOCP_HPP

