#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>

LPWSTR GetFormattedMessage(LPWSTR pMessage, ...);

// use gcc compiler must return value (cannot use void)
int main(void)
{
    LPWSTR pBuffer = NULL;
    LPWSTR pMessage = L"%1!*.*s! %3 %4!*s!";

    // The variable length arguments correspond directly to the format
    // strings in pMessage.
    pBuffer = GetFormattedMessage(pMessage, 4, 2, L"Bill", L"Bob", 6, L"Bill");
    if (pBuffer)
    {
        // Buffer contains "  Bi Bob   Bill".
        wprintf(L"Formatted message: %s\n", pBuffer);
        LocalFree(pBuffer);
    }
    else
    {
        wprintf(L"Format message failed with 0x%x\n", GetLastError());
    }
    return 0;`
}

// Formats a message string using the specified message and variable
// list of arguments.
LPWSTR GetFormattedMessage(LPWSTR pMessage, ...)
{
    LPWSTR pBuffer = NULL;

    va_list args = NULL;
    va_start(args, pMessage);

    FormatMessage(FORMAT_MESSAGE_FROM_STRING |
                  FORMAT_MESSAGE_ALLOCATE_BUFFER,
                  pMessage, 
                  0,
                  0,
                  (LPWSTR)&pBuffer, 
                  0, 
                  &args);

    va_end(args);

    return pBuffer;
}