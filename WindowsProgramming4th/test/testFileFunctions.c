#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>


#define BUF_SIZE 256

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hIn, hOut;
	DWORD nIn, nOut;
	TCHAR buffer[BUF_SIZE] = { 0 };;

	if (argc != 2) {
		printf("Input create file name.\n");
		return 1;
	}


	hIn = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
		printf("Cannot open input file. Error: %x\n", GetLastError());
		return 2;
	}


	// 試著列印出檔案內容，buffer不會自動清空，所以，最後一次可能會有問題。
	while (ReadFile(hIn, buffer, BUF_SIZE, &nIn, NULL) && (nIn != 0)) {
//		printf("%d : %d\n", i, nIn);
//		printf("%s\n", buffer);

//		if (nIn > 0 && nIn <= BUF_SIZE - 1)
		if (nIn > 0 && nIn <= BUF_SIZE) {
			if (nIn < BUF_SIZE) {
				buffer[nIn] = '\0'; // NULL character
			}
//			_tprintf(TEXT("Data read from %s (%d bytes): \n"), argv[1], nIn);
			printf("%s\n", buffer);
		}
	}

/*
	while (ReadFile(hInFile, buffer, BUF_SIZE, &nIn, NULL)
		&& (nIn != 0)
		&& WriteFile(hOutFile, buffer, nIn, &nOut, NULL));
*/

	CloseHandle(hIn);

	return 0;
}