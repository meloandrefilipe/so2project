#include "CenTaxi.h"

int _tmain(int argc, TCHAR argv[]){
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    DWORD threadIDArray[2];
    HANDLE hMutexHandle, tInitMenu, tCommunication, hTimer;

    PARAMETERS params;
    LARGE_INTEGER liDueTime;

    TownMap* townMap = new TownMap();

    Node* dest = NULL;
    Node* src = NULL;
    vector<Node*> nodes = townMap->getNodes();

    HMODULE hDLL;
    hDLL = LoadLibrary(DLL_PATH_64);
    if (hDLL == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possivel carregar a DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
        return EXIT_FAILURE;
    }

    FuncRegister fRegister = (FuncRegister)GetProcAddress(hDLL, "dll_register");
    FuncLog fLog = (FuncLog)GetProcAddress(hDLL, "dll_log");

    if (fRegister == NULL || fLog == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possivel carregar as funções da DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
        FreeLibrary(hDLL);
        return EXIT_FAILURE;
    }



    // Criar um named mutex para garantir que existe apenas uma CenTaxi a correr no sistema
    hMutexHandle = CreateMutex(NULL, TRUE, CENTAXI_MAIN_MUTEX);

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        tstringstream msg;
        msg << "[ERRO] Aplicação já a correr!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR *)msg.str().c_str());
        FreeLibrary(hDLL);
        return EXIT_FAILURE;
    }
    fRegister((TCHAR*)CENTAXI_MAIN_MUTEX, 1);


    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i]->getRow() == 1 && nodes[i]->getCol() == 1) {
            src = nodes[i];
            if (!nodes[i]->isRoad()) {
                tstringstream msg;
                msg << "[ERRO] O ponto (" << nodes[i]->getRow() << ", " << nodes[i]->getCol() << ") nao é um pedaço de estrada!" << endl;
                _tprintf(msg.str().c_str());
                fLog((TCHAR*)msg.str().c_str());
            }
        }
        if (nodes[i]->getRow() == 6 && nodes[i]->getCol() == 5) {
            if (!nodes[i]->isRoad()) {
                tstringstream msg;
                msg << "[ERRO] O ponto (" << nodes[i]->getRow() << ", " << nodes[i]->getCol() << ") nao é um pedaço de estrada!" << endl;
                _tprintf(msg.str().c_str());
                fLog((TCHAR*)msg.str().c_str());
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

 

    hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (hTimer == NULL) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel criar o WaitableTimer!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        CloseHandle(hTimer);
        return EXIT_FAILURE;
    }
    if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel iniciar o WaitableTimer!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        CloseHandle(hTimer);
        return EXIT_FAILURE;
    }

    _tprintf(TEXT("CenTaxi!\n"));

    tInitMenu = CreateThread(NULL, 0, InitMenu, &params, 0, &threadIDArray[0]);
    tCommunication = CreateThread(NULL, 0, CommsThread, &params, 0, &threadIDArray[1]);


    if (tInitMenu == NULL || tCommunication == NULL) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel criar a Thread!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        return EXIT_FAILURE;
    }

    WaitForSingleObject(tInitMenu, INFINITE);
    WaitForSingleObject(tCommunication, INFINITE);

    CloseHandle(tInitMenu);
    CloseHandle(tCommunication);

    ReleaseMutex(hMutexHandle);
    CloseHandle(hMutexHandle);
    

    if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0) {
        tstringstream msg;
        msg << "[ERRO] Não foi possível iniciar o WaitForSingleObject!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        CloseHandle(hTimer);
        return EXIT_FAILURE;
    }
    CloseHandle(hTimer);
    FreeLibrary(hDLL);
    return EXIT_SUCCESS;
}

DWORD WINAPI InitMenu(LPVOID lpParam) {
    PARAMETERS* params = (PARAMETERS*)lpParam;
    TCHAR command[COMMAND_SIZE] = TEXT("");
    HANDLE sCanRead;
    HMODULE hDLL;
    hDLL = LoadLibrary(DLL_PATH_64);
    if (hDLL == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possivel carregar a DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
        return EXIT_FAILURE;
    }

    FuncRegister fRegister = (FuncRegister)GetProcAddress(hDLL, "dll_register");
    FuncLog fLog = (FuncLog)GetProcAddress(hDLL, "dll_log");

    if (fRegister == NULL || fLog == NULL) {
        FreeLibrary(hDLL);
        _tprintf(TEXT("[ERRO] Não foi possivel carregar as funções da DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
        return EXIT_FAILURE;
    }

    while (true) {
        _tprintf(TEXT("COMMAND: "));
        if (fgetws(command, sizeof(command), stdin) == NULL) {
            tstringstream msg;
            msg << "[ERRO] Ocorreu um erro a ler o comando inserido!" << endl;
            msg << "[CODE] " << GetLastError() << endl;
            _tprintf(msg.str().c_str());
            fLog((TCHAR*)msg.str().c_str());
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
                tstringstream msg;
                msg << "[ERRO] Não foi possivel criar o somafro!" << endl;
                msg << "[CODE] " << GetLastError() << endl;
                _tprintf(msg.str().c_str());
                fLog((TCHAR*)msg.str().c_str());
                CloseHandle(sCanRead);
                return EXIT_FAILURE;
            }
            ReleaseSemaphore(sCanRead, 1, NULL);
            CloseHandle(sCanRead);
            FreeLibrary(hDLL);
            return EXIT_SUCCESS;
        }
    }
}

DWORD WINAPI CommsThread(LPVOID lpParam) {
    TAXI * pBuf;
    HANDLE hFileMapping, hFile, sCanRead, sCanWrite;
    PARAMETERS* params = (PARAMETERS*)lpParam;
    HMODULE hDLL;
    hDLL = LoadLibrary(DLL_PATH_64);
    if (hDLL == NULL) {
        _tprintf(TEXT("[ERRO] Não foi possivel carregar a DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
        return EXIT_FAILURE;
    }

    FuncRegister fRegister = (FuncRegister)GetProcAddress(hDLL, "dll_register");
    FuncLog fLog = (FuncLog)GetProcAddress(hDLL, "dll_log");

    if (fRegister == NULL || fLog == NULL) {
        FreeLibrary(hDLL);
        _tprintf(TEXT("[ERRO] Não foi possivel carregar as funções da DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
        return EXIT_FAILURE;
    }
    hFile = CreateFile(SHAREDMEMORY_CEN_CON, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == NULL) {
        tstringstream msg;
        msg << "[ERRO] Não foi possível criar o ficheiro " << SHAREDMEMORY_CEN_CON <<"!"<< endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }

    sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_CAN_WRITE_CENCON);
    sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_CAN_READ_CENCON);
    if (sCanWrite == NULL || sCanRead == NULL) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel criar o somafro!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }
    fRegister((TCHAR*)SEMAPHORE_CAN_WRITE_CENCON, 3);
    fRegister((TCHAR*)SEMAPHORE_CAN_READ_CENCON, 3);

    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, BUFFER_SIZE, SHAREDMEMORY_CEN_CON_ZONE);
    

    if (hFileMapping == NULL) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel criar o file mapping!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }
    pBuf = (TAXI*) MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TAXI));
    if (pBuf == NULL) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel mapear o ficheiro!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    } 
    fRegister((TCHAR*)SHAREDMEMORY_CEN_CON_ZONE, 7);

    while(!params->exit){
        WaitForSingleObject(sCanRead, INFINITE);
        if (!params->exit) {
            Car *c = new Car(pBuf->pid, pBuf->row, pBuf->col, pBuf->matricula);
            params->cars.push_back(c);
            tstringstream msg;
            msg << "[NEW CAR] Entrou um novo taxi! Matricula: " << c->getPlate() << endl;
            _tprintf(msg.str().c_str());
            fLog((TCHAR*)msg.str().c_str());
            _tprintf(TEXT("COMMAND: "));
            ReleaseSemaphore(sCanWrite, 1, NULL);
        }
    }
    FreeLibrary(hDLL);
    UnmapViewOfFile(pBuf);
    CloseHandle(sCanWrite);
    CloseHandle(sCanRead);
    CloseHandle(hFile);
    CloseHandle(hFileMapping);
    return EXIT_SUCCESS;
}