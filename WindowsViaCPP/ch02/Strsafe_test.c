#include <strsafe.h>
#include <stdio.h>
#include <tchar.h>

int main() 
{
    TCHAR buffer[100];
    HRESULT hr = StringCchCopy(buffer, 100, _T("Hello, safe world!"));
    if (SUCCEEDED(hr)) {
        // 安全複製成功
        _tprintf(_T("buffer = %s\n"), buffer);
    }
    else {
        _tprintf(_T("複製失敗，錯誤碼：0x%X\n"), hr);
    }

    return 0;
}