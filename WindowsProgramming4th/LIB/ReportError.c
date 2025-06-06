#include "Everything.h"

/*
	回報系統錯誤的公用函式
	取得錯誤碼並轉換為系統錯誤訊息
	在標準錯誤設備上顯示此訊息以及使用者指定的訊息
	userMessage		  : 要顯示到訊息以及使用者指定的訊息
	exitCode		  : 0   - 返回 ; 
						> 0 - 以此碼呼叫ExitProcess
	printErrorMessage : 如果設定則顯示最後的系統錯誤訊息
	※ 編譯指令 ※
	CL /IC:\WORKSPACES\GithubProjects\VSProjects\WindowsProgramming4th\INCLUDE ReportError.c
	CL /ID:\WORKSPACES\GithubProjects\VSProjects\WindowsProgramming4th\INCLUDE ReportError.c
 */
VOID ReportError(LPCTSTR userMessage, DWORD exitCode, BOOL printErrorMessage)
{
	DWORD eMsgLen, errNum = GetLastError();
	LPTSTR lpvSysMsg;

	_ftprintf(stderr, _T("%s\n"), userMessage);
	if (printErrorMessage) {
		eMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, errNum,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpvSysMsg, 0, NULL);
		if (eMsgLen > 0) {
			_fprintf(stderr, _T("%s\n"), lpvSysMsg);
		}
		else {
			_fprintf(stderr, _T("Last Error Number; %d.\n"), errNum);
		}
		if (lpvSysMsg != NULL)
			LocalFree(lpvSysMsg);
	}
	if (exitCode > 0)
		ExitProcess(exitCode);

	return;
}