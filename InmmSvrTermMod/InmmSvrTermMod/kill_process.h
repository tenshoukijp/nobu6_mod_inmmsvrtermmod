#include <iostream>
#include <string>
#include <tchar.h>
#include <process.h>
#include <windows.h>
#include <tlhelp32.h>


//  Forward declarations:
BOOL KillTargetProcess(wchar_t* pwszTargetProcessName);
BOOL TerminateTheProcess(DWORD dwProcessId, UINT uExitCode);
