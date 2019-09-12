
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

