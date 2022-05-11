#include "kill_process.h"

#pragma comment( lib, "shell32.lib" )

using namespace std;


BOOL DeleteNotifyIcon(HWND hWnd) {
    NOTIFYICONDATA ni;
    memset(&ni, 0, sizeof(NOTIFYICONDATA));

    ni.cbSize = sizeof(NOTIFYICONDATA);
    ni.hWnd = hWnd;

    const int ID_TRAYICON = 1; // _inmmserv.exe のリソースのiconの番号より
    ni.uID = ID_TRAYICON;

//    ni.uFlags = NIF_ICON;
//    ni.uCallbackMessage = WM_APP + 1;
    // タスクトレイにアイコンを追加したら自分のウィンドウを隠す。
    Shell_NotifyIcon(NIM_DELETE, &ni);
    return TRUE;
}

HWND GetWindowHandle(const DWORD TargetProcessID)
{
    HWND hWnd = GetTopWindow(NULL);
    do {
        if (GetWindowLong(hWnd, GWL_HWNDPARENT) != 0) {
            continue;
        }
        DWORD ProcessID;
        GetWindowThreadProcessId(hWnd, &ProcessID);
        if (TargetProcessID == ProcessID) {
            return hWnd;
        }
    } while ((hWnd = GetNextWindow(hWnd, GW_HWNDNEXT)) != NULL);

    return NULL;
}

BOOL KillTargetProcess(wchar_t* pwszTargetProcessName)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // システム内のすべてのプロセスのスナップショットを作成。
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    // 使用する前に構造のサイズを設定
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // 最初のプロセスに関する情報を取得し、失敗した場合は終了
    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);  // スナップショットオブジェクトをクリーンアップ
        return FALSE;
    }

    // プロセスのスナップショットを巡査
    do
    {
        wstring str(pe32.szExeFile);

        if (str == pwszTargetProcessName) // 強制終了したいプロセスの名前と比較
        {
            // プロセスからウィンドウを得て...
            HWND hWnd = GetWindowHandle(pe32.th32ProcessID);
            // 存在すればトレイアイコンを消去
            if (hWnd) {
                DeleteNotifyIcon(hWnd);
            }

            // プロセスを削除
            TerminateTheProcess(pe32.th32ProcessID, 1);
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return TRUE;
}

BOOL TerminateTheProcess(DWORD dwProcessId, UINT uExitCode)
{
    DWORD dwDesiredAccess = PROCESS_TERMINATE;
    BOOL  bInheritHandle = FALSE;
    HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
    if (hProcess == NULL) {
        return FALSE;
    }

    BOOL result = TerminateProcess(hProcess, uExitCode);

    CloseHandle(hProcess);

    return result;
}