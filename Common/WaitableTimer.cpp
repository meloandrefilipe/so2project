#include "WaitableTimer.h"


WaitableTimer::WaitableTimer(LONGLONG time)
{
    this->hDLL = NULL;
    this->hTimer = NULL;
    this->liDueTime = { 0 };
    this->liDueTime.QuadPart = time;


    this->hDLL = LoadLibrary(DLL_PATH_64);
    if (this->hDLL == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possivel carregar a DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
        return;
    }
    FuncRegister fRegister = (FuncRegister)GetProcAddress(this->hDLL, "dll_register");
    FuncLog fLog = (FuncLog)GetProcAddress(this->hDLL, "dll_log");

    this->hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (this->hTimer == NULL) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel criar o WaitableTimer!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(this->hDLL);
        CloseHandle(this->hTimer);
        return;
    }
    if (!SetWaitableTimer(this->hTimer, &this->liDueTime, 0, NULL, NULL, 0)) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel iniciar o WaitableTimer!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(this->hDLL);
        CloseHandle(this->hTimer);
        return;
    }
    if (WaitForSingleObject(this->hTimer, INFINITE) != WAIT_OBJECT_0) {
        tstringstream msg;
        msg << "[ERRO] Não foi possível iniciar o WaitForSingleObject!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(this->hDLL);
        CloseHandle(this->hTimer);
        return;
    }
}

WaitableTimer::~WaitableTimer()
{
    CloseHandle(this->hTimer);
    FreeLibrary(this->hDLL);
}
