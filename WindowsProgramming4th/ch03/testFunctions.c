// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hFile;
	TCHAR* szFilename = _TEXT("testFunctions.c");
	LARGE_INTEGER structLargeInt;

	hFile = CreateFile(szFilename, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Error no. %d\n", GetLastError());
		exit(EXIT_FAILURE);
	}
	else {
		GetFileSizeEx(hFile, &structLargeInt);
		//only the QuadPart member of the LARGE_INTEGER structure concerns us
		printf("Size: %d bytes.\n", structLargeInt.QuadPart);
	}

	return 0;
}


/*
#include <windows.h>
#include <tchar.h>

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hInFile;
	int iPos = 1;

	printf("argc: %d\n", argc);
	for (int i = 0; i < argc; i++) {
		printf("argv[%d] : %s\n", i, argv[i]);
	}

	// Create file
	hInFile = CreateFile(argv[iPos], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hInFile == INVALID_HANDLE_VALUE) {
		DWORD dw = GetLastError();
		printf("error-msg : %d\n", dw);
		_tprintf(TEXT("Terminal failure: Unable to open file \"%s\" for read.\n"), argv[1]);
		return;
	}

	printf("Read OK!!!\n");

	PLARGE_INTEGER lpFileSize = NULL;
	GetFileSizeEx(hInFile, &lpFileSize);
//	TCHAR szFileSize[128] = { 0 };
//	_stprintf(szFileSize, _T("Document size is %d"), lpFileSize->QuadPart);
	LONGLONG  len_ll = lpFileSize.QuadPart;
	printf("size is %lld\n", len_ll);

//	BOOL blnFlag = GetFileSizeEx(hInFile, &lpFileSize);
//	printf("size is %d\n", blnFlag);
//	if (blnFlag) {
//		printf("The file size is %d.\n", *lpFileSize);
//	}

	printf("Run END!!\n");

	return 0;
}
*/
