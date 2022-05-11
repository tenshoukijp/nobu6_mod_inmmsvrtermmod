#include "kill_process.h"

#pragma comment( lib, "shell32.lib" )

using namespace std;


BOOL DeleteNotifyIcon(HWND hWnd) {
    NOTIFYICONDATA ni;
    memset(&ni, 0, sizeof(NOTIFYICONDATA));

    ni.cbSize = sizeof(NOTIFYICONDATA);
    ni.hWnd = hWnd;

    const int ID_TRAYICON = 1; // _inmmserv.exe �̃��\�[�X��icon�̔ԍ����
    ni.uID = ID_TRAYICON;

//    ni.uFlags = NIF_ICON;
//    ni.uCallbackMessage = WM_APP + 1;
    // �^�X�N�g���C�ɃA�C�R����ǉ������玩���̃E�B���h�E���B���B
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

    // �V�X�e�����̂��ׂẴv���Z�X�̃X�i�b�v�V���b�g���쐬�B
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    // �g�p����O�ɍ\���̃T�C�Y��ݒ�
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // �ŏ��̃v���Z�X�Ɋւ�������擾���A���s�����ꍇ�͏I��
    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);  // �X�i�b�v�V���b�g�I�u�W�F�N�g���N���[���A�b�v
        return FALSE;
    }

    // �v���Z�X�̃X�i�b�v�V���b�g������
    do
    {
        wstring str(pe32.szExeFile);

        if (str == pwszTargetProcessName) // �����I���������v���Z�X�̖��O�Ɣ�r
        {
            // �v���Z�X����E�B���h�E�𓾂�...
            HWND hWnd = GetWindowHandle(pe32.th32ProcessID);
            // ���݂���΃g���C�A�C�R��������
            if (hWnd) {
                DeleteNotifyIcon(hWnd);
            }

            // �v���Z�X���폜
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