#include "CenTaxi.h"

int _tmain(int argc, TCHAR argv[]){
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    DWORD idMainMenuThread, idCommunicationThread, idPlateValidatorThread;
    HANDLE hMutexHandle, communicationThread, mainMenuThread, plateValidatorThread;

    PARAMETERS params;

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


  /*  for (int i = 0; i < nodes.size(); i++)
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
    }*/
    //if (src != NULL && dest != NULL) {
    //    _tprintf(TEXT("[SRC] ID: %s \n"), src->getID());
    //    _tprintf(TEXT("[DEST] ID: %s \n"), dest->getID());
    //    BreadthFirstSearch bfs = BreadthFirstSearch(townMap);
    //    BESTPATH bp = bfs.getBestPath(src, dest);
    //    _tprintf(TEXT("[MAP] cost: %d\n"), bp.cost);
    //    for (int i = 0; i < bp.path.size(); i++)
    //    {
    //        _tprintf(TEXT("[PATH] %s\n"), bp.path[i]->getID());
    //    }
    //}


    params.exit = false;

    _tprintf(TEXT("CenTaxi!\n"));

    mainMenuThread = CreateThread(NULL, 0, MainMenuThread, &params, 0, &idMainMenuThread);
    communicationThread = CreateThread(NULL, 0, CommunicationThread, &params, 0, &idCommunicationThread);
    plateValidatorThread = CreateThread(NULL, 0, PlateValidatorThread, &params, 0, &idPlateValidatorThread);


    if (mainMenuThread == NULL || communicationThread == NULL) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel criar a Thread!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        return EXIT_FAILURE;
    }

    WaitForSingleObject(mainMenuThread, INFINITE);
    WaitForSingleObject(communicationThread, INFINITE);
    WaitForSingleObject(plateValidatorThread, INFINITE);

    CloseHandle(mainMenuThread);
    CloseHandle(communicationThread);
    CloseHandle(plateValidatorThread);

    ReleaseMutex(hMutexHandle);
    CloseHandle(hMutexHandle);

    FreeLibrary(hDLL);
    return EXIT_SUCCESS;
}

DWORD WINAPI MainMenuThread(LPVOID lpParam) {
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

    while (!params->exit) {
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

            HANDLE hEventClose = CreateEvent(NULL, FALSE, FALSE, EVENT_CLOSE_ALL);

            if (hEventClose == NULL) {
                tstringstream msg;
                msg << "[ERRO] Não foi possivel criar o evento!" << endl;
                msg << "[CODE] " << GetLastError() << endl;
                _tprintf(msg.str().c_str());
                fLog((TCHAR*)msg.str().c_str());
            }

            SetEvent(hEventClose);
            params->exit = true;

            sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_CAN_READ_CENCON);
            if (sCanRead == NULL) {
                tstringstream msg;
                msg << "[ERRO] Não foi possivel criar o somafro!" << endl;
                msg << "[CODE] " << GetLastError() << endl;
                _tprintf(msg.str().c_str());
                fLog((TCHAR*)msg.str().c_str());
                CloseHandle(sCanRead);
                CloseHandle(hEventClose);
                return EXIT_FAILURE;
            }
            ReleaseSemaphore(sCanRead, 1, NULL);
            CloseHandle(sCanRead);
            CloseHandle(hEventClose);
            FreeLibrary(hDLL);
            _tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
            WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND *  5);
            exit(EXIT_SUCCESS);
            return EXIT_SUCCESS;
        }
    }
    return EXIT_SUCCESS;
}

DWORD WINAPI CommunicationThread(LPVOID lpParam) {
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
            Car *c = new Car(pBuf);
            params->cars.push_back(c);
            tstringstream msg;
            msg << "[NEW CAR] Entrou um novo taxi! Matricula: " << c->getPlate() << endl;
            _tprintf(TEXT("\n"));
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

DWORD WINAPI PlateValidatorThread(LPVOID lpParam) {
    PLATE * pBuf;
    PLATE p;
    HANDLE hFileMapping, hFile, sCanRead, sCanWrite, sConTaxi;
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
        msg << "[ERRO] Não foi possível criar o ficheiro " << SHAREDMEMORY_CEN_CON << "!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }

    sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_WRITE);
    sConTaxi = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_CONTAXI);
    sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_READ);
    if (sCanWrite == NULL || sCanRead == NULL || sConTaxi == NULL) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel criar o somafro!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sConTaxi);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }

    fRegister((TCHAR*)SEMAPHORE_PLATE_VALIDATOR_READ, 3);
    fRegister((TCHAR*)SEMAPHORE_PLATE_VALIDATOR_WRITE, 3);
    fRegister((TCHAR*)SEMAPHORE_PLATE_VALIDATOR_CONTAXI, 3);

    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, BUFFER_SIZE, SHAREDMEMORY_PLATE_VALIDATION);
    if (hFileMapping == NULL) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel criar o file mapping!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sConTaxi);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }

    pBuf = (PLATE *)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(PLATE));
    if (pBuf == NULL) {
        tstringstream msg;
        msg << "[ERRO] Não foi possivel mapear o ficheiro!" << endl;
        msg << "[CODE] " << GetLastError() << endl;
        _tprintf(msg.str().c_str());
        fLog((TCHAR*)msg.str().c_str());
        FreeLibrary(hDLL);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sConTaxi);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }

    fRegister((TCHAR*)SHAREDMEMORY_PLATE_VALIDATION, 7);

    while (!params->exit) {
        WaitForSingleObject(sCanRead, INFINITE);
        _tcscpy_s(p.plate, TAXI_PLATE_SIZE, pBuf->plate);
        if (!params->exit) {
            p.status = 0;
            for (int i = 0; i < params->cars.size(); i++)
            {
                if (params->cars[i]->isSamePlate(p.plate)) {
                    p.status = 1;
                    break;
                }
            }
            CopyMemory(pBuf, &p, sizeof(PLATE));
            ReleaseSemaphore(sConTaxi, 1, NULL);
            ReleaseSemaphore(sCanWrite, 1, NULL);
        }
    }
    FreeLibrary(hDLL);
    UnmapViewOfFile(pBuf);
    CloseHandle(sCanWrite);
    CloseHandle(sCanRead);
    CloseHandle(sConTaxi);
    CloseHandle(hFile);
    CloseHandle(hFileMapping);
    return EXIT_SUCCESS;
}