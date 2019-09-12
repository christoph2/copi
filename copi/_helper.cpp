
#include "_helper.hpp"

extern "C" {
void PyInit__helper(void)
{

}
};

namespace IOCP {

SystemInformation::SystemInformation()
{
    if (isWow64()) {
        ::GetNativeSystemInfo(&m_info);
    } else {
        ::GetSystemInfo(&m_info);
    }
}

DWORD SystemInformation::getPageSize() const
{
    return m_info.dwPageSize;
}

DWORD SystemInformation::getAllocationGranularity() const
{
    return m_info.dwAllocationGranularity;
}

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
GetModuleHandle("kernel32"),"IsWow64Process");

BOOL isWow64()
{
    BOOL bIsWow64 = FALSE;
 
    if (NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            // handle error
        }
    }
    return bIsWow64;
}
}
