#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define BUF_SIZE 256

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hIn, hOut;
	DWORD nIn, nOut;
	CHAR buffer[BUF_SIZE];

	if (argc != 2) {
		printf("Input create file name.\n");
		return 1;
	}

	hIn = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
		printf("Cannot open input file. Error: %x\n", GetLastError());
		return 2;
	}

/*
	// �յۦC�L�X�ɮפ��e�Abuffer���|�۰ʲM�šA�ҥH�A�̫�@���i��|�����D�C
	while (ReadFile(hIn, buffer, BUF_SIZE, &nIn, NULL) && (nIn != 0)) {
		printf("%d\n", nIn);
		printf("%s\n", buffer);
	}
*/

	while (ReadFile(hInFile, buffer, BUF_SIZE, &nIn, NULL)
		&& (nIn != 0)
		&& WriteFile(hOutFile, buffer, nIn, &nOut, NULL));


	CloseHandle(hIn);

	return 0;
}