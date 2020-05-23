#include "MapInfo.h"

int _tmain(int argc, TCHAR argv[]) {

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    DWORD idGetMapInfoThread, idCloseThread;
    HANDLE getMapInfoThread, closeThread, hEventCanBoot;

    Cidade* cidade = new Cidade();

    hEventCanBoot = CreateEvent(NULL, TRUE, FALSE, EVENT_BOOT_ALL);
    if (hEventCanBoot == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento para encerrar!"), TYPE::ERRO);
        return EXIT_FAILURE;
    }
    _tprintf(TEXT("A ESPERA...\n"));
    WaitForSingleObject(hEventCanBoot, INFINITE);
    CloseHandle(hEventCanBoot);
    _tprintf(TEXT("MapInfo\n"));

    getMapInfoThread = CreateThread(NULL, 0, GetMapThread, cidade, 0, &idGetMapInfoThread);
    closeThread = CreateThread(NULL, 0, CloseThread, cidade, 0, &idCloseThread);


    if (getMapInfoThread == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel criar a Thread!"), TYPE::ERRO);
        return EXIT_FAILURE;
    }

    WaitForSingleObject(getMapInfoThread, INFINITE);
    WaitForSingleObject(closeThread, INFINITE);

    CloseHandle(getMapInfoThread);
    CloseHandle(hEventCanBoot);
    CloseHandle(closeThread);
    delete  cidade;

    return EXIT_SUCCESS;
}

DWORD WINAPI GetMapThread(LPVOID lpParam) {
    HANDLE hFileMapping, hFileMappingMap, sCanRead, sCanSize;
    MAPINFO* pBuf;
    LPCTSTR pMap;
    Cidade* cidade = (Cidade*)lpParam;

    sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_MAPINFO_READ);
    sCanSize = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_MAPINFO_SIZE);

    if (sCanSize == NULL || sCanRead == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead!"), TYPE::ERRO);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        return EXIT_FAILURE;
    }
    cidade->dll->regist((TCHAR*)SEMAPHORE_MAPINFO_WRITE, 3);
    cidade->dll->regist((TCHAR*)SEMAPHORE_MAPINFO_READ, 3);

    hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_ZONE_MAPSIZE);
    hFileMappingMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_ZONE_MAPINFO);

    if (hFileMapping == NULL || hFileMappingMap == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping SHAREDMEMORY_ZONE_MAPINFO!"), TYPE::ERRO);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        CloseHandle(hFileMapping);
        CloseHandle(hFileMappingMap);
        return EXIT_FAILURE;
    }

    pBuf = (MAPINFO*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(MAPINFO));
    if (pBuf == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        CloseHandle(hFileMapping);
        CloseHandle(hFileMappingMap);
        return EXIT_FAILURE;
    }


    WaitForSingleObject(sCanSize, INFINITE);

    pMap = (LPTSTR)MapViewOfFile(hFileMappingMap, FILE_MAP_ALL_ACCESS, 0, 0, pBuf->size);
    if (pMap == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        CloseHandle(hFileMapping);
        CloseHandle(hFileMappingMap);
        return EXIT_FAILURE;
    }
    cidade->dll->regist((TCHAR*)SHAREDMEMORY_ZONE_MAPINFO, 7);

    while (!cidade->isExit()) {
        WaitForSingleObject(sCanRead, INFINITE);
        if (cidade->isExit()) {
            break;
        }
        Clear();
        _tprintf(TEXT("MapInfo\n"));
        _tprintf(TEXT("\n%s\n"), pMap);
    }
    CloseHandle(sCanRead);
    CloseHandle(sCanSize);
    UnmapViewOfFile(pBuf);
    UnmapViewOfFile(pMap);
    CloseHandle(hFileMapping);
    CloseHandle(hFileMappingMap);
    return EXIT_SUCCESS;
}


DWORD WINAPI CloseThread(LPVOID lpParam) {

    HANDLE hEventClose;
    Cidade* cidade = (Cidade*)lpParam;
    WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 5);
    hEventClose = CreateEvent(NULL, TRUE, FALSE, EVENT_CLOSE_ALL);
    if (hEventClose == NULL) {
        cidade->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento!"), TYPE::ERRO);
    }
    WaitForSingleObject(hEventClose, INFINITE);

    cidade->dll->regist((TCHAR*)EVENT_CLOSE_ALL, 4);

    _tprintf(TEXT("\n[WARNING] A Central fechou!\n"));
    cidade->setExit(true);
    _tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
    CloseHandle(hEventClose);
    wt->wait();
    delete wt;
    delete cidade;
    exit(EXIT_SUCCESS);
}



void Clear(){
    #if defined _WIN32
        system("cls");
    #elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
        system("clear");
    #elif defined (__APPLE__)
        system("clear");
    #endif
}