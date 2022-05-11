#include <windows.h>
#include "kill_process.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


void WINAPI On_初期化(HANDLE 呼び出し元ハンドル) { }

void WINAPI On_末期化() { }


class InmmSvrTermMod {
public:
    InmmSvrTermMod() { }
    ~InmmSvrTermMod() {
        KillTargetProcess(L"_inmmserv.exe");
    }
};

// オブジェクトがdll読み込みとともに生成され、破棄される際にデストラクタが実行されるため、_inmmserv.exe が強制終了される。
InmmSvrTermMod istm;