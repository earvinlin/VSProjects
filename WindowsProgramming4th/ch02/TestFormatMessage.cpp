#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>

// use gcc compiler must return value (cannot use void)
//int main(void)
void main(void)
{
    
     pMessage = L"%1!*.*s! %4 %5!*s!";
    DWORD_PTR pArgs[] = { (DWORD_PTR)4, (DWORD_PTR)2, (DWORD_PTR)L"Bill",  // %1!*.*s! refers back to the first insertion string in pMessage
         (DWORD_PTR)L"Bob",                                                // %4 refers back to the second insertion string in pMessage
         (DWORD_PTR)6, (DWORD_PTR)L"Bill" };                               // %5!*s! refers back to the third insertion string in pMessage
    const DWORD size = 100+1;
    WCHAR buffer[size];


    if (!FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                       pMessage, 
                       0,
                       0,
                       buffer, 
                       size, 
                       (va_list*)pArgs))
    {
        wprintf(L"Format message failed with 0x%x\n", GetLastError());
//        return 0;
        return;
    }

    // Buffer contains "  Bi Bob   Bill".
    wprintf(L"Formatted message: %s\n", buffer);
}