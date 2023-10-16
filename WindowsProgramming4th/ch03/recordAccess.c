/*	Chapter 3. RecordAccess. */
//
// Ref : (Cht) https://learn.microsoft.com/zh-tw/windows/win32/api/fileapi/nf-fileapi-writefile
//       (Eng) https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
//       (CL-cmd) https://learn.microsoft.com/zh-tw/cpp/build/reference/errorreport-report-internal-compiler-errors?view=msvc-170
//
/*	Demonstrates simple record management in a file.
	The file is specified as a collection of an arbitrary
	number of records (The number is specified on the command
	line). Each record has a character string (limited length),
	creation time and last update time, and an update count.
	The file has an 8-byte HEADER containing the total number
	of record slots and the total number of non-empty records.
	The user can interactively create, update, and modify
	records.

	Usage: RecordAccessMM FileName[nrec[prompt]]
	If nrec is omitted, FileName must already exist.
	If nrec is supplied and > 0, FileName is recreated (destroying any existing file)
	   and the program exits, having created an empty file.
	All user interaction is suppressed if prompt is specified - useful if input
	   commands are redirected from a file, as in the performance tests
	  [as in RecordAccessTIME.bat]

	If the number of records is large, a sparse file is recommended.
	The techniques here could also be used if a hash function
	determined the record location (this would be a simple
	prorgam enhancment). */

	/* This program illustrates:
		1. Random file access.
		2. LARGE_INTEGER arithmetic and using the 64-bit file positions.
		3. record update in place.
		4. File initialization to 0 (this will not work under Windows 9x
			or with a FAT file system.
	*/

#include "..\INCLUDE\Everything.h"

#define STRING_SIZE 256
typedef struct _RECORD { /* File record structure */
	DWORD			referenceCount;  /* 0 meands an empty record */
	SYSTEMTIME		recordCreationTime;
	SYSTEMTIME		recordLastRefernceTime;
	SYSTEMTIME		recordUpdateTime;
	TCHAR			dataString[STRING_SIZE];
} RECORD;
typedef struct _HEADER { /* File header descriptor */
	DWORD			numRecords;
	DWORD			numNonEmptyRecords;
} HEADER;

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hFile;
	LARGE_INTEGER currentPtr;
	DWORD OpenOption, nXfer, recNo;
	RECORD record;
	TCHAR string[STRING_SIZE], command, extra;
	OVERLAPPED ov = { 0, 0, 0, 0, NULL }, ovZero = { 0, 0, 0, 0, NULL };
	HEADER header = { 0, 0 };
	SYSTEMTIME currentTime;
	BOOLEAN headerChange, recordChange;		// BOOLEAN : BYTE(8bit)
	int prompt = (argc <= 3) ? 1 : 0;

	if (argc < 2) {	// �S���ǤJ�Ѽ�
		ReportError(_T("Usage: RecordAccess file [nrec [prompt]]"), 1, FALSE);
	}

	/* 
	* _ttoi(), atoi() : �NCString�ഫ�����(ex: "20000" --> 20000)
	* OPEN_EXISTING   : �@�߶}���ɮסC
	* CREATE_ALWAYS   : �@�߫إ߷s���ɮסC
	*
	*/
	OpenOption = ((argc > 2 && _ttoi(argv[2]) <= 0) || argc <= 2) ? OPEN_EXISTING : CREATE_ALWAYS;
	hFile = CreateFile(argv[1],
		GENERIC_READ | GENERIC_WRITE,	//
		0,								// �p�G���ѼƬ��s�B CreateFile ���\�A�h�L�k�@���ɮשθ˸m�A�ӥB�b�����ɮשθ˸m������X���e�L�k�A���}�ҡC 
		NULL,							// �p�G���ѼƬ� Null�A�h���ε{���i�إߪ�����l�i�{�L�k�~�� CreateFile �Ǧ^������X�A�B�P�Ǧ^����X�����p���ɮשθ˸m�|���o�w�]���w���ʴy�z���C
		OpenOption,						// �n��s�b�Τ��s�b���ɮשθ˸m�Ĩ����ʧ@�F���ҥu�i�� �uOPEN_EXISTING�v or �uCREATE_ALWAYS�v�C
		FILE_FLAG_RANDOM_ACCESS,		// �ɮשθ˸m�ݩʩM�X�СF(FILE_FLAG_RANDOM_ACCESS : �s���O�H�����C�t�Υi�ϥγo�Ӱ����̨Τ��ɮק֨������ܡC)
		NULL							// �㦳 GENERIC_READ �s���v�����d���ɮת����ı���X�C
	);
	if (hFile == INVALID_HANDLE_VALUE) {
		ReportError(_T("RecordAccess error: Cannot open existing file."), 2, TRUE);
	}

	if (argc >= 3 && _ttoi(argv[2]) > 0) { /* Write the header (all header and pre-size the new file) */
		header.numRecords = _ttoi(argv[2]);
		if (!WriteFile(	// �p�G�禡���\�A�h�Ǧ^�Ȭ��D�s (TRUE) �C
			hFile,			// [in]
			&header,		// [in] �w�İϪ����СA�䤤�]�t�n�g�J�ɮשθ˸m����ơC
			sizeof(header), // [in] �n�g�J�ɮשθ˸m���줸�ռƥءC
			&nXfer,			// [out, optional] �ϥΦP�B hFile �ѼƮɡA�����g�J�줸�ռƥت��ܼƫ��СC
			&ovZero			// [in, out, optional] �p�GhFile�ѼƬO�HFILE_FLAG_OVERLAPPED�}�ҡA�h�ݭn�������c�����СA�_�h���Ѽƥi�H�ONull�C
			)) {
			ReportError(_T("RecordAccess Error: WriteFile header."), 4, TRUE);
		}

		currentPtr.QuadPart = (LONGLONG)sizeof(RECORD) * _ttoi(argv[2]) + sizeof(HEADER);
		if (!SetFilePointerEx(hFile,	// [in] �ɮת�����X�C
			currentPtr,					// [in] �n�����ɮ׫��Ъ��줸�ռƥءC 
			NULL,						// [out, optional] �n�����s�ɮ׫��Ф��ܼƪ����СC�p�G���ѼƬ� Null�A�h���|�Ǧ^�s���ɮ׫��СC 
			FILE_BEGIN					// [in] �ɮ׫��в��ʪ��_�I�C
			)) {
			ReportError(_T("RecordAccess Error: Set Pointer."), 4, TRUE);
		}
		if (!SetEndOfFile(hFile)) {
			ReportError(_T("RecordAccess Error: Set End of File."), 5, TRUE);
		}
		if (prompt) {
			_tprintf(_T("Empty file with %d records created.\n"), header.numRecords);
		}

		return 0;
	}

	/* Read the file header to find the number of records and non-empty records */
	if (!ReadFile(hFile,	// [in] �˸m������X (�Ҧp�A�ɮסB�ɮ׸�Ƭy�{�B����Ϥ��B�Ϥ��ϡB�D���x�w�İϡB�ϱa���B�q�T�ݡB�q�T�귽�B�l�󶰩κ޹D) �C
		&header,			// [out] �w�İϪ����СA�����q�ɮשθ˸mŪ������ơC
		sizeof(HEADER),		// [in] �nŪ�����줸�ռƥؤW���C
		&nXfer,				// [out, optional] �ϥΦP�B hFile �ѼƮɡA����Ū���줸�ռƥؤ��ܼƪ����СC ReadFile �|�N���ȳ]�w���s�A�A�i�����u�@�ο��~�ˬd�C �p�G�o�O�D�P�B�@�~�A�H�קK�o�Ϳ��~�����G�A�Шϥ� Null �@�����ѼơC
		&ovZero				// [in, out, optional] �p�GhFile�ѼƬO�HFILE_FLAG_OVERLAPPED�}�ҡA�h�ݭn�������c�����СA�_�h�i�H�ONull�C
		)) {
		ReportError(_T("RecordAccess Error: ReadFile header."), 6, TRUE);
	}

	if (prompt) {
		_tprintf(_T("File %s contains %d non-empty records of size %d.\n Total capacity: %d\n"),
			argv[1], header.numNonEmptyRecords, sizeof(RECORD), header.numRecords);
	}

	/* Prompt the user to read or write a numbered record */
	while (TRUE) {
		headerChange = FALSE; recordChange = FALSE;
		if (prompt) {
			_tprintf(_T("Enter r(ead)/w(rite)/d(elete)/qu(it) record#\n"));
		}

		_tscanf(_T("%c%u%c"), &command, &recNo, &extra);
		if (command == _T('q')) {
			break;
		}
		if (recNo >= header.numRecords) {
			if (prompt) {
				_tprintf(_T("record Number is too large. Try again.\n"));
			}
			continue;
		}

		currentPtr.QuadPart = (LONGLONG)recNo * sizeof(RECORD) + sizeof(HEADER);	// �x�s��Ʈe�q�j�p
		ov.Offset = currentPtr.LowPart;
		ov.OffsetHigh = currentPtr.HighPart;

		if (!ReadFile(hFile, &record, sizeof(RECORD), &nXfer, &ov)) {
			ReportError(_T("RecordAccess: ReadFile failure."), 7, FALSE);
		}
		GetSystemTime(&currentTime); /* Use to update record time fields */
		record.recordLastRefernceTime = currentTime;
		if (command == _T('r') || command == _T('d')) { /* Report record contents, if any */
			if (record.referenceCount == 0) {
				if (prompt) _tprintf(_T("record Number %d is empty.\n"), recNo);
				continue;
			}
			else {
				if (prompt) {
					_tprintf(_T("record Number %d. Reference Count: %d \n"),
						recNo, record.referenceCount);
				}
				if (prompt) {
					_tprintf(_T("Data: %s\n"), record.dataString);
				}
				/* Exercise: Display times. See ls.c for an example */
			}
			if (command == _T('d')) { /* Delete the record */
				record.referenceCount = 0;
				header.numNonEmptyRecords--;
				headerChange = TRUE;
				recordChange = TRUE;
			}
		}
		else if (command == _T('w')) { /* Write the record, even if for the first time */
			if (prompt) {
				_tprintf(_T("Enter new data string for the record.\n"));
			}
			_fgetts(string, sizeof(string), stdin); // Don't use _getts (potential buffer overflow)
			string[_tcslen(string) - 1] = _T('\0'); // remove the newline character
			if (record.referenceCount == 0) {
				record.recordCreationTime = currentTime;
				header.numNonEmptyRecords++;
				headerChange = TRUE;
			}
			record.recordUpdateTime = currentTime;
			record.referenceCount++;
			_tcsncpy(record.dataString, string, STRING_SIZE - 1);
			recordChange = TRUE;
		}
		else {
			if (prompt) {
				_tprintf(_T("command must be r, w, or d. Try again.\n"));
			}
		}
		/* Update the record in place if any record contents have changed. */
		if (recordChange && !WriteFile(hFile, &record, sizeof(RECORD), &nXfer, &ov)) {
			ReportError(_T("RecordAccess: WriteFile update failure."), 8, FALSE);
		}
		/* Update the number of non-empty records if required */
		if (headerChange) {
			if (!WriteFile(hFile, &header, sizeof(header), &nXfer, &ovZero)) {
				ReportError(_T("RecordAccess: WriteFile update failure."), 9, FALSE);
			}
		}
	}

	if (prompt) {
		_tprintf(_T("Computed number of non-empty records is: %d\n"), header.numNonEmptyRecords);
	}
	if (!ReadFile(hFile, &header, sizeof(HEADER), &nXfer, &ovZero)) {
		ReportError(_T("RecordAccess Error: ReadFile header."), 10, TRUE);
	}
	if (prompt) {
		_tprintf(_T("File %s NOW contains %d non-empty records.\nTotal capacity is: %d\n"),
			argv[1], header.numNonEmptyRecords, header.numRecords);
	}
	CloseHandle(hFile);

	return 0;
}
