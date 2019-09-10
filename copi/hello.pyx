
import sys

cdef extern from "Windows.h":
    ctypedef Py_UNICODE WCHAR
    ctypedef const WCHAR* LPCWSTR
    ctypedef void* HWND

    int __stdcall MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, int uType)

def say_hello_to(name):
    print("Hello %s!" % name)

def msg():
    title = u"Windows Interop Demo - Python %d.%d.%d" % sys.version_info[:3]
    MessageBoxW(NULL, u"Hello Cython \u263a", title, 0)
