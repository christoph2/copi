
#if !defined(__IOCP_HPP)
#define __IOCP_HPP

#define _WIN32_WINNT    0x601

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Mstcpip.h>



#if !defined(__GNUC__)
#pragma comment(lib,"ws2_32.lib") // MSVC only.
#endif

namespace IOCP {
::HANDLE Create(void);
bool RegisterHandle(::HANDLE port, ::HANDLE object, ::ULONG_PTR key);
void PostQuitMessage(void);
}

#endif // __IOCP_HPP

