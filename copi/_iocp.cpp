
#include "_iocp.hpp"

#include <string>
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

void Win_ErrorMsg(const std::string & function, DWORD error)
{

}

HANDLE IOCP_Create(void)
{
    HANDLE handle;

    handle  = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, (ULONG_PTR)0, 1);
    if (handle == NULL) {
        Win_ErrorMsg("XcpTl_CreateIOCP::CreateIoCompletionPort()", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    return handle;
}


bool IOCP_RegisterHandle(HANDLE port, HANDLE object, ULONG_PTR key)
{
    HANDLE handle;

    handle = ::CreateIoCompletionPort(object, port, key, 0);
    if (handle == NULL) {
        Win_ErrorMsg("XcpTl_RegisterIOCPHandle::CreateIoCompletionPort()", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    return (bool)(handle == port);
}

#if 0
void IOCP_PostQuitMessage(void)
{
    ::PostQueuedCompletionStatus(XcpTl_Connection.iocp, 0, (ULONG_PTR)NULL, NULL);
}
#endif
