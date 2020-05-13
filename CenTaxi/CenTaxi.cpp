#include "CenTaxi.h"



int wmain(int argc, TCHAR argv[]){
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    DWORD threadIDArray[2];
    HANDLE hMutexHandle, tInitMenu, tCommunication;
    PARAMETERS params;
    HANDLE hTimer;
    LARGE_INTEGER liDueTime;
    TownMap* townMap = new TownMap();

    Node* dest = NULL;
    Node* src = NULL;
    vector<Node*> nodes = townMap->getNodes();

    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i]->getRow() == 0 && nodes[i]->getCol() == 0) {
            src = nodes[i];
            if (!nodes[i]->isRoad()) {
                _tprintf(TEXT("[ERRO] O ponto (%d,%d) nao é um pedaço de estrada!\n"),nodes[i]->getRow(), nodes[i]->getCol());
            }
        }
        if (nodes[i]->getRow() == 6 && nodes[i]->getCol() == 5) {
            if (!nodes[i]->isRoad()) {
                _tprintf(TEXT("[ERRO] O ponto (%d,%d) nao é um pedaço de estrada!\n"), nodes[i]->getRow(), nodes[i]->getCol());
            }
            dest = nodes[i];
        }
    }
    if (src != NULL && dest != NULL) {
        _tprintf(TEXT("[SRC] ID: %s \n"), src->getID());
        _tprintf(TEXT("[DEST] ID: %s \n"), dest->getID());

        BreadthFirstSearch bfs = BreadthFirstSearch(townMap);
        BESTPATH bp = bfs.getBestPath(src, dest);


        _tprintf(TEXT("[MAP] cost: %d\n"), bp.cost);
        for (int i = 0; i < bp.path.size(); i++)
        {
            _tprintf(TEXT("[PATH] %s\n"), bp.path[i]->getID());
        }
    }
    liDueTime.QuadPart = WAIT_ONE_SECOND;
    params.exit = false;

    // Criar um named mutex para garantir que existe apenas uma CenTaxi a correr no sistema
    hMutexHandle = CreateMutex(NULL, TRUE, mainMutexName);

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        _tprintf(TEXT("[ERRO] Aplicação já a correr!\n[CODE] %d\n"), GetLastError());
        return EXIT_FAILURE;
    }

    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (hTimer == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possivel criar o WaitableTimer!\n[CODE] %d\n"), GetLastError());
        CloseHandle(hTimer);
        return EXIT_FAILURE;
    }
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
        _tprintf(TEXT("[ERRO] Não foi possivel iniciar o WaitableTimer!\n[CODE] %d\n"), GetLastError());
        CloseHandle(hTimer);
        return EXIT_FAILURE;
    }

    _tprintf(TEXT("CenTaxi!\n"));

    tInitMenu = CreateThread(NULL, 0, InitMenu, &params, 0, &threadIDArray[0]);
    tCommunication = CreateThread(NULL, 0, CommsThread, &params, 0, &threadIDArray[1]);


    if (tInitMenu == NULL || tCommunication == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possivel criar a Thread!\n[CODE] %d\n"), GetLastError());
        return EXIT_FAILURE;
    }

    WaitForSingleObject(tInitMenu, INFINITE);
    WaitForSingleObject(tCommunication, INFINITE);

    CloseHandle(tInitMenu);
    CloseHandle(tCommunication);

    ReleaseMutex(hMutexHandle);
    CloseHandle(hMutexHandle);
    

    if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0) {
        printf("[ERRO] Não foi possível iniciar o WaitForSingleObject!\n[CODE] %d\n", GetLastError());
        CloseHandle(hTimer);
        return EXIT_FAILURE;
    }
    CloseHandle(hTimer);
    return EXIT_SUCCESS;
}

DWORD WINAPI InitMenu(LPVOID lpParam) {
    PARAMETERS* params = (PARAMETERS*)lpParam;
    TCHAR command[COMMAND_SIZE] = TEXT("");
    HANDLE sCanRead;


    while (true) {
        _tprintf(TEXT("COMMAND: "));
        if (fgetws(command, sizeof(command), stdin) == NULL) {
            _tprintf(TEXT("[ERRO] Ocorreu um erro a ler o comando inserido!\n[CODE] %d\n"), GetLastError());
        }
        for (int i = 0; i < sizeof(command) && command[i]; i++){
            if (command[i] == '\n')
                command[i] = '\0';
        }
        if (_tcscmp(command, TEXT("exit")) == 0) {
            _tprintf(TEXT("Cya!\n"));
            params->exit = true;

            sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_CAN_READ_CENCON);
            if (sCanRead == NULL) {
                _tprintf(TEXT("[ERRO] Não foi possivel criar o somafro!\n[CODE] %d\n"), GetLastError());
                CloseHandle(sCanRead);
                return EXIT_FAILURE;
            }
            ReleaseSemaphore(sCanRead, 1, NULL);
            CloseHandle(sCanRead);

            return EXIT_SUCCESS;
        }
    }
}

DWORD WINAPI CommsThread(LPVOID lpParam) {
    TAXI * pBuf;
    HANDLE hFileMapping, hFile, sCanRead, sCanWrite;
    PARAMETERS* params = (PARAMETERS*)lpParam;

    hFile = CreateFile(SHAREDMEMORY_CEN_CON, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possível criar o ficheiro %s!\n[CODE] %d\n"), SHAREDMEMORY_CEN_CON ,GetLastError());
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }

    sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_CAN_WRITE_CENCON);
    sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_CAN_READ_CENCON);


    if (sCanWrite == NULL || sCanRead == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possivel criar o somafro!\n[CODE] %d\n"), GetLastError());
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }


    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, BUFFER_SIZE, SHAREDMEMORY_CEN_CON_ZONE);

    if (hFileMapping == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possivel criar o file mapping!\n[CODE] %d\n"), GetLastError());
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }
    pBuf = (TAXI*) MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TAXI));
    if (pBuf == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possivel mapear o ficheiro!\n[CODE] %d\n"), GetLastError());
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    } 

    while(!params->exit){
        WaitForSingleObject(sCanRead, INFINITE);
        if (!params->exit) {
            Car *c = new Car(pBuf->pid, pBuf->row, pBuf->col, pBuf->matricula);
            params->cars.push_back(c);
            _tprintf(TEXT("\n[NEW CAR] Entrou um novo taxi! Matricula: %s\nCOMMAND: "), c->getPlate());
            ReleaseSemaphore(sCanWrite, 1, NULL);
        }
    }
    UnmapViewOfFile(pBuf);
    CloseHandle(sCanWrite);
    CloseHandle(sCanRead);
    CloseHandle(hFile);
    CloseHandle(hFileMapping);
    return EXIT_SUCCESS;
}