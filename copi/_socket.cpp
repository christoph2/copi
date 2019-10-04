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

#include <MSWSock.h>

namespace COPI {


CSocket::CSocket(IOCP * iocp, int family, int socktype, int protocol, int options)
{
    m_family = family;
    m_socktype = socktype;
    m_protocol = protocol;
    m_connected = false;
    m_iocp = iocp;
    m_addr = NULL;
    loadFunctions();
    m_socket = ::WSASocket(family, socktype, protocol, NULL, 0, WSA_FLAG_OVERLAPPED | options);
    SecureZeroMemory(&m_peerAddress, sizeof(SOCKADDR_STORAGE));
}

CSocket::~CSocket()
{
    ::closesocket(m_socket);
}

void CSocket::loadFunctions()
{
    int res;
    DWORD dwBytes;
    GUID guid = WSAID_CONNECTEX;
    SOCKET dummy_socket;

    dummy_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    res = ::WSAIoctl(
        dummy_socket,
        SIO_GET_EXTENSION_FUNCTION_POINTER,
        &guid, sizeof(guid),
        &connectEx, sizeof(connectEx),
        &dwBytes, NULL, NULL
    );
    ::closesocket(dummy_socket);
}

HANDLE CSocket::getHandle() const
{
    return reinterpret_cast<HANDLE>(m_socket);
}

void CSocket::setOption(int option, const char * optval, int optlen)
{
    ::setsockopt(m_socket, SOL_SOCKET, option, optval, optlen);
}

void CSocket::getOption(int option, char * optval, int * optlen)
{
    ::getsockopt(m_socket, SOL_SOCKET, option, optval, optlen);
}

bool CSocket::bind(CAddress & address)
{

    if (::bind(m_socket, &address.address, address.length) == SOCKET_ERROR) {
        Win_ErrorMsg("Socket::bind()", WSAGetLastError());
        return false;
    }
    return true;
}

bool CSocket::connect(CAddress & address)
{
    if (::connect(m_socket, &address.address, address.length) == SOCKET_ERROR) {
        Win_ErrorMsg("Socket::connect()", WSAGetLastError());
        return false;
    }
    CPerHandleData handleData(HANDLE_SOCKET, this);
    m_iocp->registerHandle(&handleData);
    m_connected = true;
    return true;
}

bool CSocket::disconnect()
{
    //::disconnect();
    return true;
}

bool CSocket::listen(int backlog)
{
    if (::listen(m_socket, backlog) == SOCKET_ERROR) {
        Win_ErrorMsg("Socket::listen()", WSAGetLastError());
        return false;
    }
    return true;
}

bool CSocket::accept(CAddress & peerAddress)
{
    SOCKET sock;

    peerAddress.length = sizeof peerAddress.address;
    sock = ::accept(m_socket, (sockaddr *)&peerAddress.address, &peerAddress.length);

    if (sock  == SOCKET_ERROR) {
        Win_ErrorMsg("Socket::accept()", WSAGetLastError());
        return false;
    }
    return true;
}

bool CSocket::getaddrinfo(int family, int socktype, int protocol, const char * hostname, int port, CAddress & address, int flags)
{
    int err;
    ADDRINFO hints;
    ADDRINFO * t_addr;
    char port_str[16] = {0};

    ::SecureZeroMemory(&hints, sizeof(hints));
    hints.ai_family = family;
    hints.ai_socktype = socktype;
    hints.ai_protocol = protocol;
    hints.ai_flags = flags;

    ::sprintf(port_str, "%d", port);
    err = ::getaddrinfo(hostname, port_str, &hints, &t_addr);
    if (err != 0) {
        //gai_strerror(err);
        ::freeaddrinfo(t_addr);
        return false;
    }

    address.length = t_addr->ai_addrlen;
    ::CopyMemory(&address.address, t_addr->ai_addr, sizeof(struct sockaddr));

    ::freeaddrinfo(t_addr);
    return true;
}

void CSocket::write(char * buf, unsigned int len)
{
    DWORD bytesWritten;
    int addrLen;
    CPerIOData * iod = new CPerIOData(128);

    iod->m_wsabuf.buf = buf;
    iod->m_wsabuf.len = len;
    iod->m_opcode = IO_WRITE;
    iod->m_bytesRemaining = iod->m_bytesToXfer = len;
    iod->resetOverlapped();

    if (m_socktype == SOCK_DGRAM) {
        addrLen = sizeof(SOCKADDR_STORAGE);
        if (::WSASendTo(m_socket,
            &iod->m_wsabuf,
            1,
            &bytesWritten,
            0,
            (LPSOCKADDR)&m_peerAddress,
            addrLen,
            (LPWSAOVERLAPPED)&iod->m_overlapped,
            NULL
        ) == SOCKET_ERROR) {
            Win_ErrorMsg("Socket:WSASendTo()", WSAGetLastError());
        }
    } else if (m_socktype == SOCK_STREAM) {
        if (::WSASend(
            m_socket,
            &iod->m_wsabuf,
            1,
            &bytesWritten,
            0,
            (LPWSAOVERLAPPED)&iod->m_overlapped,
            NULL) == SOCKET_ERROR) {
            Win_ErrorMsg("Socket:WSASend()", WSAGetLastError());
            closesocket(m_socket);
        }
    }
}

void CSocket::triggerRead(unsigned int len)
{
    DWORD numReceived = (DWORD)0;
    DWORD flags = (DWORD)0;
    DWORD err = 0;
    int addrLen;
    static char buf[1024];

    CPerIOData * iod = new CPerIOData(128);

    iod->m_wsabuf.buf = buf;
    iod->m_wsabuf.len = len;
    iod->m_opcode = IO_READ;
    iod->m_bytesRemaining = iod->m_bytesToXfer = len;
    iod->resetOverlapped();


//    SecureZeroMemory(&recvOlap.overlapped, sizeof(OVERLAPPED));

    if (m_socktype == SOCK_STREAM) {
//        if (XcpTl_Connection.socketConnected == XCP_FALSE) {
//            return;
//        }
        if (WSARecv(m_socket,
                    &iod->m_wsabuf,
                    1,
                    &numReceived,
                    &flags,
                    (LPWSAOVERLAPPED)&iod->m_overlapped,
                    (LPWSAOVERLAPPED_COMPLETION_ROUTINE)NULL)  == SOCKET_ERROR) {
            err = WSAGetLastError();
            if (err != WSA_IO_PENDING) {
                Win_ErrorMsg("CSocket::WSARecv()", err);
            }
        }
    } else if (m_socktype == SOCK_DGRAM) {
        addrLen = sizeof(SOCKADDR_STORAGE);
        if (WSARecvFrom(m_socket,
                    &iod->m_wsabuf,
                    1,
                    &numReceived,
                    &flags,
                    (LPSOCKADDR)&numReceived,
                    &addrLen,
                    (LPWSAOVERLAPPED)&iod->m_overlapped,
                    (LPWSAOVERLAPPED_COMPLETION_ROUTINE)NULL)) {
            err = WSAGetLastError();
            if (err != WSA_IO_PENDING) {
                Win_ErrorMsg("CSocket::WSARecvFrom()", WSAGetLastError());
            }
        }
    }
}

}
