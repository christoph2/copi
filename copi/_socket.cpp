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

namespace IOCP {

//LPFN_CONNECTEX Socket::ConnectEx = NULL;

Socket::Socket(IOCP * iocp, int family, int socktype, int protocol, int options)
{
    m_iocp = iocp;
    loadFunctions();
    m_socket = WSASocket(family, socktype, protocol, NULL, 0, WSA_FLAG_OVERLAPPED | options);
}

Socket::~Socket()
{
    closesocket(m_socket);
}

void Socket::loadFunctions()
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
    closesocket(dummy_socket);
}

HANDLE Socket::getHandle() const
{
    return reinterpret_cast<HANDLE>(m_socket);
}

void Socket::setOption(int option, const char * optval, int optlen)
{
    setsockopt(m_socket, SOL_SOCKET, option, optval, optlen);
}

void Socket::getOption(int option, char * optval, int * optlen)
{
    getsockopt(m_socket, SOL_SOCKET, option, optval, optlen);
}

int Socket::connect(const char *hostname, int port, int family)
{
    SOCKET sock;
    int err;
    ADDRINFO hints = {}, *addr;
    char port_str[16] = {};
    DWORD dwBytes;
    char horst[NI_MAXHOST];
    char servInfo[NI_MAXSERV];
    int res;

    SecureZeroMemory(&hints, sizeof(hints));
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    //hints.ai_flags = AI_NUMERICHOST | AI_PASSIVE;
    hints.ai_flags = 0; //AI_PASSIVE;

    sprintf(port_str, "%d", port);

    sock = socket(family, SOCK_STREAM, 0);

    err = getaddrinfo(hostname, port_str, &hints, &addr);
    // Grap the first address or except.
    if (err != 0)
    {
        //throw WindowsException();
    }

    if (::connect(sock, addr->ai_addr, addr->ai_addrlen) == 0) {
        printf("Connected to: %x [%d]\n", addr->ai_addr, addr->ai_family);
        //break;
    } else {
        printf("Connect failed: %d\n", WSAGetLastError());
        Win_ErrorMsg("connect", WSAGetLastError());
    }
    return true;
}

bool Socket::resolve(const char *hostname, int port, int family, AddressListType & addresses)
{
    int err;
    ADDRINFO hints = {}, *addrs;
    char port_str[16] = {};

    SecureZeroMemory(&hints, sizeof(hints));
    hints.ai_family = family;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    //hints.ai_flags = AI_NUMERICHOST | AI_PASSIVE;
    hints.ai_flags = 0; //AI_PASSIVE;



    sprintf(port_str, "%d", port);

    err = getaddrinfo(hostname, port_str, &hints, &addrs);
    if (err != 0)
    {
        //throw WindowsException();
    }

    for(struct addrinfo *addr = addrs; addr != NULL; addr = addr->ai_next)
    {
        PerHandleData ph;

        //inet_ntoa();
        //InetNtop();
#if 0
        SecureZeroMemory(horst, NI_MAXHOST);
        SecureZeroMemory(servInfo, NI_MAXSERV);

        res = getnameinfo(addr->ai_addr,
                    sizeof(sockaddr),
                    (char*)&horst[0],
                    NI_MAXHOST,
                    (char*)&servInfo[0],
                    NI_MAXSERV,
                    NI_NUMERICSERV);
        if (res == 0) {
            printf("HOST: %s SERV: %s ADDR: ", horst, servInfo);
            for (int j = 0; j < 14; ++j) {
                printf("%02x ",addr->ai_addr->sa_data[j]);
            }
            printf("\n");
        } else {
            printf("getnameinfo error: %d [%d]\n", res, WSAGetLastError());
        }
#endif // 0

    }

    freeaddrinfo(addrs);

    return true;
}


}
