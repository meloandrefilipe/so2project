#include "CenTaxi.h"

int _tmain(int argc, TCHAR argv[]){

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    DWORD idMainMenuThread, idCommunicationThread, idPlateValidatorThread, idSendMapThread, idSendMapInfoThread;
    HANDLE hMutexHandle, communicationThread, mainMenuThread, plateValidatorThread, sendMapThread, sendMapInfoThread;
    HANDLE hEventCanBoot;
    Central* central = new Central();


    // Criar um named mutex para garantir que existe apenas uma CenTaxi a correr no sistema
    hMutexHandle = CreateMutex(NULL, TRUE, CENTAXI_MAIN_MUTEX);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        central->dll->log((TCHAR*)TEXT("Aplicação já a correr"), TYPE::ERRO);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)CENTAXI_MAIN_MUTEX, 1);


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

    hEventCanBoot = CreateEvent(NULL, TRUE, FALSE, EVENT_BOOT_ALL);
    if (hEventCanBoot == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento para encerrar!"), TYPE::ERRO);
        return EXIT_FAILURE;
    }

    _tprintf(TEXT("CenTaxi!\n"));
    sendMapThread = CreateThread(NULL, 0, SendMapThread, central, 0, &idSendMapThread);
    sendMapInfoThread = CreateThread(NULL, 0, SendMapInfoThread, central, 0, &idSendMapInfoThread);
    mainMenuThread = CreateThread(NULL, 0, MainMenuThread, central, 0, &idMainMenuThread);
    communicationThread = CreateThread(NULL, 0, CommunicationThread, central, 0, &idCommunicationThread);
    plateValidatorThread = CreateThread(NULL, 0, PlateValidatorThread, central, 0, &idPlateValidatorThread);


    if (mainMenuThread == NULL || communicationThread == NULL || sendMapThread == NULL || plateValidatorThread == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar a Thread!"), TYPE::ERRO);
        return EXIT_FAILURE;
    }

    SetEvent(hEventCanBoot);

    
    WaitForSingleObject(mainMenuThread, INFINITE);
    WaitForSingleObject(communicationThread, INFINITE);
    WaitForSingleObject(plateValidatorThread, INFINITE);
    WaitForSingleObject(sendMapThread, INFINITE);

    CloseHandle(mainMenuThread);
    CloseHandle(communicationThread);
    CloseHandle(plateValidatorThread);
    CloseHandle(sendMapThread);
    CloseHandle(hEventCanBoot);


    ReleaseMutex(hMutexHandle);
    CloseHandle(hMutexHandle);

    return EXIT_SUCCESS;
}

DWORD WINAPI MainMenuThread(LPVOID lpParam) {
    Central* central = (Central*)lpParam;
    TCHAR command[COMMAND_SIZE] = TEXT("");
    HANDLE sCanRead;
    WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 5);

    while (!central->isExit()) {
        _tprintf(TEXT("COMMAND: "));
        do {
            if (fgetws(command, sizeof(command), stdin) == NULL) {
                central->dll->log((TCHAR*)TEXT("Ocorreu um erro a ler o comando inserido!"), TYPE::ERRO);
            }
        } while (sizeof(command) < 1);
        for (int i = 0; i < sizeof(command) && command[i]; i++){
            if (command[i] == '\n')
                command[i] = '\0';
        }
        if (_tcscmp(command, TEXT("exit")) == 0) {

            HANDLE hEventClose = CreateEvent(NULL, TRUE, FALSE, EVENT_CLOSE_ALL);
            if (hEventClose == NULL) {
                central->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento para encerrar!"), TYPE::ERRO);
                return EXIT_FAILURE;
            }
            SetEvent(hEventClose);

            sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_CAN_READ_CENCON);
            if (sCanRead == NULL) {
                central->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanRead Menu!"), TYPE::ERRO);
                CloseHandle(sCanRead);
                CloseHandle(hEventClose);
                return EXIT_FAILURE;
            }
            ReleaseSemaphore(sCanRead, 1, NULL);
            CloseHandle(sCanRead);
            CloseHandle(hEventClose);
            _tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
            wt->wait();
            central->setExit(true);
            wt->updateTime(WAIT_ONE_SECOND);
            wt->wait();
            delete wt;
            delete central;
            exit(EXIT_SUCCESS);
        }
    }
    delete wt;
    return EXIT_SUCCESS;
}

DWORD WINAPI CommunicationThread(LPVOID lpParam) {
    TAXI * pBuf;
    HANDLE hFileMapping, hFile, sCanRead, sCanWrite;
    Central* central = (Central*)lpParam;


    hFile = CreateFile(SHAREDMEMORY_CEN_CON, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possível criar o ficheiro de memoria Centaxi-Contaxi!"), TYPE::ERRO);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SHAREDMEMORY_CEN_CON, 6);

    sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_CAN_WRITE_CENCON);
    sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_CAN_READ_CENCON);

    if (sCanWrite == NULL || sCanRead == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead Comunicação!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SEMAPHORE_CAN_WRITE_CENCON, 3);
    central->dll->regist((TCHAR*)SEMAPHORE_CAN_READ_CENCON, 3);

    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(TAXI), SHAREDMEMORY_CEN_CON_ZONE);
    

    if (hFileMapping == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }
    pBuf = (TAXI*) MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TAXI));
    if (pBuf == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    } 
    central->dll->regist((TCHAR*)SHAREDMEMORY_CEN_CON_ZONE, 7);

    while(!central->isExit()){
        WaitForSingleObject(sCanRead, INFINITE);
        if (!central->isExit()) {
            if (!central->carExists(pBuf)) {
                Car* c = new Car(pBuf);
                central->addCar(c);
                tstringstream msg;
                msg << "\n[NEW CAR] Entrou um novo taxi! Matricula: " << c->getPlate() << endl;
                _tprintf(msg.str().c_str());
                _tprintf(TEXT("\nCOMMAND: "));
            }
            else {
                central->updateCar(pBuf);
            }
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

DWORD WINAPI PlateValidatorThread(LPVOID lpParam) {
    PLATE * pBuf;
    PLATE p;
    HANDLE hFileMapping, hFile, sCanRead, sCanWrite, sConTaxi;
    Central* central = (Central*)lpParam;

    hFile = CreateFile(SHAREDMEMORY_CEN_CON, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == NULL) {
        central->dll->log((TCHAR*)TEXT(" Não foi possível criar o ficheiro de memoria!"), TYPE::ERRO);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SHAREDMEMORY_CEN_CON, 6);
    sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_WRITE);
    sConTaxi = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_CONTAXI);
    sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_READ);
    if (sCanWrite == NULL || sCanRead == NULL || sConTaxi == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead ou sConTaxi Plate!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sConTaxi);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }

    central->dll->regist((TCHAR*)SEMAPHORE_PLATE_VALIDATOR_READ, 3);
    central->dll->regist((TCHAR*)SEMAPHORE_PLATE_VALIDATOR_WRITE, 3);
    central->dll->regist((TCHAR*)SEMAPHORE_PLATE_VALIDATOR_CONTAXI, 3);

    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(PLATE), SHAREDMEMORY_PLATE_VALIDATION);
    if (hFileMapping == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sConTaxi);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }

    pBuf = (PLATE *)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(PLATE));
    if (pBuf == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sConTaxi);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }

    central->dll->regist((TCHAR*)SHAREDMEMORY_PLATE_VALIDATION, 7);

    while (!central->isExit()) {
        WaitForSingleObject(sCanRead, INFINITE);
        _tcscpy_s(p.plate, TAXI_PLATE_SIZE, pBuf->plate);
        if (!central->isExit()) {
            p.status = 0;
            for (int i = 0; i < central->cars.size(); i++)
            {
                if (central->cars[i]->isSamePlate(p.plate)) {
                    p.status = 1;
                    break;
                }
            }
            CopyMemory(pBuf, &p, sizeof(PLATE));
            ReleaseSemaphore(sConTaxi, 1, NULL);
            ReleaseSemaphore(sCanWrite, 1, NULL);
        }
    }
    UnmapViewOfFile(pBuf);
    CloseHandle(sCanWrite);
    CloseHandle(sCanRead);
    CloseHandle(sConTaxi);
    CloseHandle(hFile);
    CloseHandle(hFileMapping);
    return EXIT_SUCCESS;
}

DWORD WINAPI SendMapThread(LPVOID lpParam) {
    HANDLE hFile, hFileMapping, hFileMappingMap, sCanRead, sCanWrite, sCanSize;
    MAPINFO* pBuf;
    MAPINFO mapInfo;
    LPCTSTR pMap;
    Central* central = (Central*)lpParam;

    hFile = CreateFile(SHAREDMEMORY_SHAREMAP, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possível criar o ficheiro de memoria SHAREDMEMORY_SHAREMAP!"), TYPE::ERRO);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SHAREDMEMORY_SHAREMAP, 6);
    sCanWrite = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_SHAREMAP_WRITE);
    sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_SHAREMAP_READ);
    sCanSize = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_SHAREMAP_SIZE);

    if (sCanWrite == NULL || sCanRead == NULL || sCanSize == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SEMAPHORE_SHAREMAP_WRITE, 3);
    central->dll->regist((TCHAR*)SEMAPHORE_SHAREMAP_READ, 3);
    central->dll->regist((TCHAR*)SEMAPHORE_SHAREMAP_SIZE, 3);

    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(MAPINFO), SHAREDMEMORY_CONTAXI_MAP_SIZE);
    hFileMappingMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, central->getSizeMap(), SHAREDMEMORY_CONTAXI_MAP);

    if (hFileMapping == NULL || hFileMappingMap == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping SHAREDMEMORY_ZONE_SHAREMAP!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        CloseHandle(hFileMappingMap);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }

    pBuf = (MAPINFO*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(MAPINFO));
    pMap = (LPTSTR)MapViewOfFile(hFileMappingMap, FILE_MAP_ALL_ACCESS, 0, 0, central->getSizeMap());
    if (pBuf == NULL || pMap == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        CloseHandle(hFileMappingMap);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SHAREDMEMORY_CONTAXI_MAP_SIZE, 7);
    central->dll->regist((TCHAR*)SHAREDMEMORY_CONTAXI_MAP, 7);

    mapInfo.size = central->getSizeMap();
    CopyMemory(pBuf, &mapInfo, sizeof(MAPINFO));
    
    
    while (!central->isExit()) {
        ReleaseSemaphore(sCanSize, 1, NULL);
        WaitForSingleObject(sCanWrite, INFINITE);
        CopyMemory((PVOID)pMap, central->getCleanMap(), central->getSizeMap());
        ReleaseSemaphore(sCanRead, 1, NULL);
    }
    CloseHandle(sCanWrite);
    CloseHandle(sCanRead);
    CloseHandle(sCanSize);
    UnmapViewOfFile(pBuf);
    UnmapViewOfFile(pMap);
    CloseHandle(hFileMapping);
    CloseHandle(hFileMappingMap);
    return EXIT_SUCCESS;
}

DWORD WINAPI SendMapInfoThread(LPVOID lpParam) {
    HANDLE hFile, hFileMapping, sCanRead, sCanWrite, sCanSize, hFileMappingMap;
    MAPINFO* pBuf;
    MAPINFO mapInfo;
    LPCTSTR pMap;
    TCHAR* map;
    Central* central = (Central*)lpParam;
    WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND);

    hFile = CreateFile(SHAREDMEMORY_MAPINFO, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possível criar o ficheiro de memoria SHAREDMEMORY_SHAREMAP!"), TYPE::ERRO);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SHAREDMEMORY_MAPINFO, 6);

    sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_MAPINFO_WRITE);
    sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_MAPINFO_READ);
    sCanSize = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_MAPINFO_SIZE);

    if (sCanWrite == NULL || sCanRead == NULL || sCanSize == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SEMAPHORE_MAPINFO_WRITE, 3);
    central->dll->regist((TCHAR*)SEMAPHORE_MAPINFO_READ, 3);
    central->dll->regist((TCHAR*)SEMAPHORE_MAPINFO_SIZE, 3);

    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(MAPINFO), SHAREDMEMORY_ZONE_MAPSIZE);
    hFileMappingMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, central->getSizeMap(), SHAREDMEMORY_ZONE_MAPINFO);

    if (hFileMapping == NULL || hFileMappingMap == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping SHAREDMEMORY_ZONE_SHAREMAP!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sCanSize);
        CloseHandle(hFileMapping);
        CloseHandle(hFileMappingMap);
        return EXIT_FAILURE;
    }
    pBuf = (MAPINFO*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(MAPINFO));
    pMap = (LPTSTR)MapViewOfFile(hFileMappingMap, FILE_MAP_ALL_ACCESS, 0, 0, central->getSizeMap());

    if (pBuf == NULL || pMap == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFileMapping);
        CloseHandle(hFileMappingMap);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SHAREDMEMORY_ZONE_MAPSIZE, 7);
    central->dll->regist((TCHAR*)SHAREDMEMORY_ZONE_MAPINFO, 7);

    mapInfo.size = central->getSizeMap();
    map = new TCHAR[central->getSizeMap()];
    CopyMemory(pBuf, &mapInfo, sizeof(MAPINFO));



    while (!central->isExit()) {
        ReleaseSemaphore(sCanSize, 1, NULL);
        _tcscpy_s(map,  central->getSizeMap(), central->getFilledMap());
        wt->wait();
        CopyMemory((PVOID)pMap, map, central->getSizeMap());
        ReleaseSemaphore(sCanRead, 1, NULL);
    }
    delete wt;
    CloseHandle(sCanWrite);
    CloseHandle(sCanRead);
    CloseHandle(sCanSize);
    UnmapViewOfFile(pBuf);
    UnmapViewOfFile(pMap);
    CloseHandle(hFileMapping);
    CloseHandle(hFileMappingMap);
    return EXIT_SUCCESS;
}