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
#include "_helper.hpp"
#include <string>

extern "C" {
void PyInit__helper(void)
{

}
};

namespace IOCP {

static bool _isWow64();

SystemInformation::SystemInformation()
{
    if (isWow64()) {
        ::GetNativeSystemInfo(&m_info);
    } else {
        ::GetSystemInfo(&m_info);
    }
}

bool SystemInformation::isWow64() const
{
    return _isWow64();
}

DWORD SystemInformation::getPageSize() const
{
    return m_info.dwPageSize;
}

DWORD SystemInformation::getAllocationGranularity() const
{
    return m_info.dwAllocationGranularity;
}

DWORD SystemInformation::getNumberOfProcessors() const
{
    return m_info.dwNumberOfProcessors;
}

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

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
GetModuleHandle("kernel32"),"IsWow64Process");

static bool _isWow64()
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
