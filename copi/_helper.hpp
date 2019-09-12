

#if !defined(__HELPER_HPP)
#define __HELPER_HPP

#define _WIN32_WINNT    0x601
#include <Windows.h>

namespace IOCP {
BOOL isWow64();

class SystemInformation {
public:
    SystemInformation();
    DWORD getPageSize() const;
    DWORD getAllocationGranularity() const;
private:
    SYSTEM_INFO m_info;
};

}

#endif // __HELPER_HPP

