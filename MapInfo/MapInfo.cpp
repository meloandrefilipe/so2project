#include "MapInfo.h"

int _tmain(int argc, TCHAR argv[]) {

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    DWORD idGetMapInfoThread, idCloseThread;
    HANDLE getMapInfoThread, closeThread;
    DLLProfessores* dll = new DLLProfessores();
    Cidade* cidade = new Cidade();

    _tprintf(TEXT("MapInfo\n"));
    WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND);

    getMapInfoThread = CreateThread(NULL, 0, GetMapThread, cidade, 0, &idGetMapInfoThread);
    closeThread = CreateThread(NULL, 0, CloseThread, cidade, 0, &idCloseThread);


    if (getMapInfoThread == NULL) {
        dll->log((TCHAR*)TEXT("Não foi possivel criar a Thread!"), TYPE::ERRO);
        delete dll;
        return EXIT_FAILURE;
    }

    WaitForSingleObject(getMapInfoThread, INFINITE);
    WaitForSingleObject(closeThread, INFINITE);

    CloseHandle(getMapInfoThread);
    CloseHandle(closeThread);
    delete dll;

    return EXIT_SUCCESS;
}

DWORD WINAPI GetMapThread(LPVOID lpParam) {
    HANDLE hFileMapping, sCanRead, sCanWrite;
    MAPINFO* pBuf;
    MAPINFO mapinfo;
    DLLProfessores* dll = new DLLProfessores();
    Cidade* cidade = (Cidade*)lpParam;

    sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_MAPINFO_WRITE);
    sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_MAPINFO_READ);

    if (sCanWrite == NULL || sCanRead == NULL) {
        dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead!"), TYPE::ERRO);
        delete dll;
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        return EXIT_FAILURE;
    }
    dll->regist((TCHAR*)SEMAPHORE_MAPINFO_WRITE, 3);
    dll->regist((TCHAR*)SEMAPHORE_MAPINFO_READ, 3);

    hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_ZONE_MAPINFO);

    if (hFileMapping == NULL) {
        dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping SHAREDMEMORY_ZONE_SHAREMAP!"), TYPE::ERRO);
        delete dll;
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }

    pBuf = (MAPINFO*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(MAPINFO));
    if (pBuf == NULL) {
        dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
        delete dll;
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }

    dll->regist((TCHAR*)SHAREDMEMORY_ZONE_MAPINFO, 7);
    while (!cidade->isExit()) {
        WaitForSingleObject(sCanRead, INFINITE);
        if (cidade->isExit()) {
            break;
        }
        CopyMemory(&mapinfo, pBuf, sizeof(MAPINFO));
        Clear();
        _tprintf(TEXT("MapInfo\n"));
        _tprintf(TEXT("\n%s\n"), mapinfo.map);
        ReleaseSemaphore(sCanWrite, 1, NULL);
    }
    delete dll;
    CloseHandle(sCanWrite);
    CloseHandle(sCanRead);
    UnmapViewOfFile(pBuf);
    CloseHandle(hFileMapping);
    return EXIT_SUCCESS;
}


DWORD WINAPI CloseThread(LPVOID lpParam) {

    HANDLE hEventClose;
    DLLProfessores* dll = new DLLProfessores();
    Cidade* cidade = (Cidade*)lpParam;

    hEventClose = CreateEvent(NULL, TRUE, FALSE, EVENT_CLOSE_ALL);
    if (hEventClose == NULL) {
        dll->log((TCHAR*)TEXT("Não foi possivel criar o evento!"), TYPE::ERRO);
        delete dll;
    }
    dll->regist((TCHAR*)EVENT_CLOSE_ALL, 4);

    WaitForSingleObject(hEventClose, INFINITE);
    _tprintf(TEXT("\n[WARNING] A Central fechou!\n"));
    cidade->setExit(true);
    _tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
    delete dll;
    CloseHandle(hEventClose);
    WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 5);

    exit(EXIT_SUCCESS);
}



void Clear()
{
#if defined _WIN32
    system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined (__APPLE__)
    system("clear");
#endif
}