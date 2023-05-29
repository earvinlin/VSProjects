#include "INCLUDE\Everything.h"
//#include <windows.h>
//#include <tchar.h>
//#include <stdio.h>

int _tmain(int argc, LPTSTR argv[])
{
    HANDLE hInFile;
    BOOL DashS;
    int iArg, iFirstFile;

    printf("Hllo world!");
    iFirstFile = Options(argc, argv, _T("s"), &DashS, NULL);
    hInFile = CreateFile(argv[iArg], GENERIC_READ, 0, NULL, OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL, NULL);
    
    return 0;
}