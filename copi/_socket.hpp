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
#if !defined(__SOCKET_HPP)
#define __SOCKET_HPP

#include "copi.hpp"
#include <MSWSock.h>

namespace COPI {

struct CAddress {
    int length;
    struct sockaddr address;
};

class CSocket : public CHasHandle {
public:
    //Socket() {};
    CSocket(IOCP * iocp, int family = PF_INET, int socktype = SOCK_STREAM, int protocol = IPPROTO_TCP, int options = 0);
    ~CSocket();

    void getOption(int option, char * optval, int * optlen);
    void setOption(int option, const char * optval, int optlen);
    bool getaddrinfo(int family, int socktype, int protocol, const char * hostname, int port, CAddress & address, int flags = AI_PASSIVE);
    bool connect(CAddress & address);
    bool disconnect();
    bool bind(CAddress & address);
    bool listen(int backlog = 10);
    bool accept(CAddress & peerAddress);
    void write(char * buf, unsigned int len);
    void triggerRead(unsigned int len);
    HANDLE getHandle() const;
    LPFN_CONNECTEX connectEx;
protected:
    void loadFunctions();
private:
    int m_family;
    int m_socktype;
    int m_protocol;
    bool m_connected;
    IOCP * m_iocp;
    ADDRINFO * m_addr;
    SOCKET m_socket;
    SOCKADDR_STORAGE m_peerAddress;

    CPerIOData * m_acceptOlap;
    CPerIOData * m_recvOlap;
    CPerIOData * m_sendOlap;
};


}

#endif  // __SOCKET_HPP
