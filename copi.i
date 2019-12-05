
%module copi

%{
#include "copi.hpp"

using namespace COPI;
%}

%include "copi.hpp"


//namespace COPI {


class CSystemInformation {
public:
    CSystemInformation();
    DWORD getPageSize() const;
    DWORD getAllocationGranularity() const;
    DWORD getNumberOfProcessors() const;
    bool isWow64() const;
private:
    SYSTEM_INFO m_info;
};

//void Win_ErrorMsg(const std::string & function, DWORD error);

//}
