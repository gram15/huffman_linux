#ifndef TCHAR_H
#define TCHAR_H

/*
#define TCHAR char
#define _TCHAR char
#define _T(X) X
#define _tcscmp strcmp
#define _stprintf_s
*/

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <string.h>

/* In winnt.h */
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))

/* In winbase.h */
#define ZeroMemory RtlZeroMemory


//Unicode for unix
#if ((defined(UNICODE) || defined(_UNICODE)) && defined(__unix__))
    #include <wchar.h>
    #define LPCTSTR const wchar_t*
    #define LPTSTR wchar_t*
    #define TCHAR wchar_t
    #define _TCHAR wchar_t

    //Functions
    #define _T(x) Lx
    #define _tcslen	wcslen
    #define _stprintf_s swprintf
    #define _vstprintf vswprintf

    #define _tcscpy wcscpy
    #define _tcsncpy wcsncpy

    #define _tcstok_s wcstok

    #define _tcscmp wcscmp

//ASCII for unix
#elif __unix__
    #define LPCTSTR const char*
    #define LPTSTR char*
    #define TCHAR char
    #define _TCHAR char

    //Functions
    #define _T(x) x
    #define _tcslen	strlen
    #define _stprintf_s snprintf
    #define _tprintf printf
    #define _vstprintf vsprintf

    #define _tcscpy strcpy
    #define _tcsncpy strncpy

    #define _tcstok_s strtok_r

    #define _tcscmp strcmp

#endif

#endif // TCHAR_H
