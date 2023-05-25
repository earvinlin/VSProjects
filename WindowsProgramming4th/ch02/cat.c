#include "INCLUDE\Everything.h"
#define BUF_SIZE 0x200

static VOID CatFile(HANDLE, HANDLE);

int _tmain(int argc, LPTSTR argv[])
{
    HANDLE hInFile, hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    BOOL DashS;
    int iArg, iFirstFile;

    iFirstFile = Options(argc, argv, _T("s"), %DashS, NULL);
    if (iFirstFile == argc) {
        CatFile(hStdIn, hStdOut);
        return 0;
    }

    for (iArg = iFirstFile; iArg < argc; iArg++) {
        hInFile = CreateFile(argv [iArg],
            GENERIC_READ,
            0,
            NULL,
            OPEN_EXISTING, 
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if (hInFile == INVALID_HANDLE_VALUE && !DashS)
            ReportError(_T("Cat file open Error"), 1, TRUE);
        CatFile(hInFile, hStdOut);
        CloseHandle(hInFile);
    }
    return 0;

    static VOID CatFile(HANDLE hInFile, HANDLE hOutFile)
    {
        DWORD nIn, nOut;
        BYTE buffer[BUF_SIZE];
        while (ReadFile(hInFile, buffer, BUF_SIZE, &nIn, NULL) 
            && (nIn != 0)
            && WriteFile(hOutFile, buffer, nIn, &nOut, NULL));
        return;
    }
}