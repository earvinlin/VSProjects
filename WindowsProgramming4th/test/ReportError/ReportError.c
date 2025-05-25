#include "Everything.h"

/*
	�^���t�ο��~�����Ψ禡
	���o���~�X���ഫ���t�ο��~�T��
	�b�зǿ��~�]�ƤW��ܦ��T���H�ΨϥΪ̫��w���T��
	userMessage		  : �n��ܨ�T���H�ΨϥΪ̫��w���T��
	exitCode		  : 0   - ��^ ; 
						> 0 - �H���X�I�sExitProcess
	printErrorMessage : �p�G�]�w�h��̫᪺ܳ�t�ο��~�T��
	�� �sĶ���O ��
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