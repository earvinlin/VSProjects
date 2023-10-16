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

	if (argc < 2) {	// 沒有傳入參數
		ReportError(_T("Usage: RecordAccess file [nrec [prompt]]"), 1, FALSE);
	}

	/* 
	* _ttoi(), atoi() : 將CString轉換為整數(ex: "20000" --> 20000)
	* OPEN_EXISTING   : 一律開啟檔案。
	* CREATE_ALWAYS   : 一律建立新的檔案。
	*
	*/
	OpenOption = ((argc > 2 && _ttoi(argv[2]) <= 0) || argc <= 2) ? OPEN_EXISTING : CREATE_ALWAYS;
	hFile = CreateFile(argv[1],
		GENERIC_READ | GENERIC_WRITE,	//
		0,								// 如果此參數為零且 CreateFile 成功，則無法共用檔案或裝置，而且在關閉檔案或裝置的控制碼之前無法再次開啟。 
		NULL,							// 如果此參數為 Null，則應用程式可建立的任何子進程無法繼承 CreateFile 傳回的控制碼，且與傳回控制碼相關聯的檔案或裝置會取得預設的安全性描述元。
		OpenOption,						// 要對存在或不存在的檔案或裝置採取的動作；本例只可為 「OPEN_EXISTING」 or 「CREATE_ALWAYS」。
		FILE_FLAG_RANDOM_ACCESS,		// 檔案或裝置屬性和旗標；(FILE_FLAG_RANDOM_ACCESS : 存取是隨機的。系統可使用這個做為最佳化檔案快取的提示。)
		NULL							// 具有 GENERIC_READ 存取權限之範本檔案的有效控制碼。
	);
	if (hFile == INVALID_HANDLE_VALUE) {
		ReportError(_T("RecordAccess error: Cannot open existing file."), 2, TRUE);
	}

	if (argc >= 3 && _ttoi(argv[2]) > 0) { /* Write the header (all header and pre-size the new file) */
		header.numRecords = _ttoi(argv[2]);
		if (!WriteFile(	// 如果函式成功，則傳回值為非零 (TRUE) 。
			hFile,			// [in]
			&header,		// [in] 緩衝區的指標，其中包含要寫入檔案或裝置的資料。
			sizeof(header), // [in] 要寫入檔案或裝置的位元組數目。
			&nXfer,			// [out, optional] 使用同步 hFile 參數時，接收寫入位元組數目的變數指標。
			&ovZero			// [in, out, optional] 如果hFile參數是以FILE_FLAG_OVERLAPPED開啟，則需要重迭結構的指標，否則此參數可以是Null。
			)) {
			ReportError(_T("RecordAccess Error: WriteFile header."), 4, TRUE);
		}

		currentPtr.QuadPart = (LONGLONG)sizeof(RECORD) * _ttoi(argv[2]) + sizeof(HEADER);
		if (!SetFilePointerEx(hFile,	// [in] 檔案的控制碼。
			currentPtr,					// [in] 要移動檔案指標的位元組數目。 
			NULL,						// [out, optional] 要接收新檔案指標之變數的指標。如果此參數為 Null，則不會傳回新的檔案指標。 
			FILE_BEGIN					// [in] 檔案指標移動的起點。
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
	if (!ReadFile(hFile,	// [in] 裝置的控制碼 (例如，檔案、檔案資料流程、實體磁片、磁片區、主控台緩衝區、磁帶機、通訊端、通訊資源、郵件集或管道) 。
		&header,			// [out] 緩衝區的指標，接收從檔案或裝置讀取的資料。
		sizeof(HEADER),		// [in] 要讀取的位元組數目上限。
		&nXfer,				// [out, optional] 使用同步 hFile 參數時，接收讀取位元組數目之變數的指標。 ReadFile 會將此值設定為零，再進行任何工作或錯誤檢查。 如果這是非同步作業，以避免發生錯誤的結果，請使用 Null 作為此參數。
		&ovZero				// [in, out, optional] 如果hFile參數是以FILE_FLAG_OVERLAPPED開啟，則需要重迭結構的指標，否則可以是Null。
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

		currentPtr.QuadPart = (LONGLONG)recNo * sizeof(RECORD) + sizeof(HEADER);	// 儲存資料容量大小
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
