#include "WaitableTimer.h"


WaitableTimer::WaitableTimer(LONGLONG time)
{
    this->hTimer = NULL;
    this->liDueTime = { 0 };
    this->liDueTime.QuadPart = time;
    this->name = new TCHAR[20];
    swprintf_s(this->name, 20, TEXT("WT_%d"), WT++);
    this->dll = new DLLProfessores();

    this->hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (this->hTimer == NULL) {
        this->dll->log((TCHAR*)TEXT("Não foi possivel criar o WaitableTimer!"), TYPE::ERRO);
        delete this->dll;
        CloseHandle(this->hTimer);
        return;
    }
    
    this->dll->regist(this->name, 6);
}

WaitableTimer::~WaitableTimer()
{
    CloseHandle(this->hTimer);
    delete this->dll;
}

DWORD WaitableTimer::wait()
{
    if (!SetWaitableTimer(this->hTimer, &this->liDueTime, 0, NULL, NULL, 0)) {
        this->dll->log((TCHAR*)TEXT("Não foi possivel iniciar o WaitableTimer!"), TYPE::ERRO);
        delete this->dll;
        CloseHandle(this->hTimer);
        return EXIT_FAILURE;
    }
    if (WaitForSingleObject(this->hTimer, INFINITE) != WAIT_OBJECT_0) {
        this->dll->log((TCHAR*)TEXT("Não foi possível iniciar o WaitForSingleObject!"), TYPE::ERRO);
        delete this->dll;
        CloseHandle(this->hTimer);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void WaitableTimer::updateTime(LONGLONG time)
{
    this->liDueTime.QuadPart = time;
}
