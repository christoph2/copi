
#if !defined(__IOCP_HPP)
#define __IOCP_HPP

#define _WIN32_WINNT    0x601

#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Mstcpip.h>


#include <vector>

#if !defined(__GNUC__)
#pragma comment(lib,"ws2_32.lib") // MSVC only.
#endif

namespace IOCP {

typedef std::vector<char> BufferType;

enum HandleType {
    HANDLE_SOCKET,
    HANDLE_FILE
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

struct PerHandleData {
    HandleType handleType;
};


struct IOCP_PerIOData {
    IOType ioType;
    BufferType xferBuffer;
    size_t bytesToXfer;
    size_t bytesRemaining;
};

bool Create(PerPortData & port);
bool RegisterHandle(::HANDLE port, ::HANDLE object, ::ULONG_PTR key);
void PostQuitMessage(void);
}

#endif // __IOCP_HPP

