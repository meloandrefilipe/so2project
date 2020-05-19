#include "DLLProfessores.h"

DLLProfessores::DLLProfessores()
{
    this->hDLL = nullptr;
    this->funcRegister = nullptr;
    this->funcLog = nullptr;

    this->hDLL = LoadLibrary(DLL_PATH_64);
    if (this->hDLL == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possivel carregar a DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
        return;
    }

    this->funcRegister = (FuncRegister)GetProcAddress(this->hDLL, "dll_register");
    this->funcLog = (FuncLog)GetProcAddress(this->hDLL, "dll_log");

    if (this->funcRegister == NULL || this->funcLog == NULL) {
        FreeLibrary(hDLL);
        _tprintf(TEXT("[ERRO] Não foi possivel carregar as funções da DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
        return;
    }
}

DLLProfessores::~DLLProfessores()
{
    FreeLibrary(this->hDLL);
}

DWORD DLLProfessores::regist(TCHAR* message, int code)
{
    this->funcRegister(message, code);
    return EXIT_SUCCESS;
}

DWORD DLLProfessores::log(TCHAR* message, TYPE type)
{
    TCHAR str[MAP_SHARE_SIZE] = TEXT("");
    tstringstream msg;
    switch (type)
    {
    case TYPE::ERRO:
        msg << "[ERRO] " << message << endl;
        break;
    case TYPE::WARNING:
        msg << "[WARNING] " << message << endl;
        break;
    case TYPE::NOTIFICATION:
        msg << "[NOTIFICATION] " << message << endl;
        break;
    default:
        msg << message << endl;
        break;
    }
    
    msg << "[CODE] " << GetLastError() << endl;
    _tprintf(msg.str().c_str());
    _tcscpy_s(str, MAP_SHARE_SIZE, msg.str().c_str());
    this->funcLog(str);
    return EXIT_SUCCESS;
}
