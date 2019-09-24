
# distutils: language=c++

from cpython.bool cimport bool
cimport libc.stdint as types

ctypedef types.uint32_t DWORD

cdef extern from "copi.hpp" namespace "IOCP":

    cdef cppclass CSystemInformation:
        CSystemInformation()
        DWORD getPageSize()
        DWORD getAllocationGranularity()
        DWORD getNumberOfProcessors()
        bool isWow64()

cdef class SystemInformation:

    cdef CSystemInformation *_thisptr

    def __cinit__(self):
        self._thisptr = new CSystemInformation()
        if self._thisptr == NULL:
            raise MemoryError()

    def __dealloc__(self):
        if self._thisptr != NULL:
            del self._thisptr

    cpdef DWORD getPageSize(self):
        return self._thisptr.getPageSize()

    cpdef DWORD getAllocationGranularity(self):
        return self._thisptr.getAllocationGranularity()

    cpdef DWORD getNumberOfProcessors(self):
        return self._thisptr.getNumberOfProcessors()

    cpdef bint isWow64(self):
        return self._thisptr.isWow64()

