#include <strsafe.h>
#include <stdio.h>
#include <tchar.h>

int main() 
{
    TCHAR buffer[100];
    HRESULT hr = StringCchCopy(buffer, 100, _T("Hello, safe world!"));
    if (SUCCEEDED(hr)) {
        // �w���ƻs���\
        _tprintf(_T("buffer = %s\n"), buffer);
    }
    else {
        _tprintf(_T("�ƻs���ѡA���~�X�G0x%X\n"), hr);
    }

    return 0;
}