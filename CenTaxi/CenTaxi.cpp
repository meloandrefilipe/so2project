#include "CenTaxi.h"

int _tmain(int argc, TCHAR argv[]){

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    DWORD idMainMenuThread, idCommunicationThread, idPlateValidatorThread, idSendMapThread, idSendMapInfoThread, idNewPassengersThread, idInterestThread;
    HANDLE hMutexHandle, communicationThread, mainMenuThread, plateValidatorThread, sendMapThread, sendMapInfoThread, newPassengersThread, hInterestThread;
    HANDLE hEventCanBoot;

    Central* central = new Central();




    // Criar um named mutex para garantir que existe apenas uma CenTaxi a correr no sistema
    hMutexHandle = CreateMutex(NULL, TRUE, CENTAXI_MAIN_MUTEX);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {  
        central->dll->log((TCHAR*)TEXT("Já Existe uma CenTaxi a funcionar!"), TYPE::ERRO);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)CENTAXI_MAIN_MUTEX, 1);

    hEventCanBoot = CreateEvent(NULL, TRUE, FALSE, EVENT_BOOT_ALL);
    if (hEventCanBoot == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento para encerrar!"), TYPE::ERRO);
        return EXIT_FAILURE;
    }




    _tprintf(TEXT("CenTaxi!\n"));
    sendMapThread = CreateThread(NULL, 0, SendMapThread, central, 0, &idSendMapThread);
    hInterestThread = CreateThread(NULL, 0, ReadInterest, central, 0, &idInterestThread);
    newPassengersThread = CreateThread(NULL, 0, ConPassThread, central, 0, &idNewPassengersThread);
    sendMapInfoThread = CreateThread(NULL, 0, SendMapInfoThread, central, 0, &idSendMapInfoThread);
    mainMenuThread = CreateThread(NULL, 0, MainMenuThread, central, 0, &idMainMenuThread);
    communicationThread = CreateThread(NULL, 0, CommunicationThread, central, 0, &idCommunicationThread);
    plateValidatorThread = CreateThread(NULL, 0, PlateValidatorThread, central, 0, &idPlateValidatorThread);


    if (mainMenuThread == NULL || communicationThread == NULL || sendMapThread == NULL || plateValidatorThread == NULL || newPassengersThread == NULL || hInterestThread == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar a Thread!"), TYPE::ERRO);
        return EXIT_FAILURE;
    }
    SetEvent(hEventCanBoot);



    
    WaitForSingleObject(mainMenuThread, INFINITE);
    WaitForSingleObject(communicationThread, INFINITE);
    WaitForSingleObject(plateValidatorThread, INFINITE);
    WaitForSingleObject(sendMapThread, INFINITE);
    WaitForSingleObject(newPassengersThread, INFINITE);
    WaitForSingleObject(hInterestThread, INFINITE);
    WaitForSingleObject(sendMapInfoThread, INFINITE);


    CloseHandle(newPassengersThread);
    CloseHandle(mainMenuThread);
    CloseHandle(communicationThread);
    CloseHandle(plateValidatorThread);
    CloseHandle(sendMapThread);
    CloseHandle(hEventCanBoot);
    CloseHandle(sendMapInfoThread);
    CloseHandle(hInterestThread);


    ReleaseMutex(hMutexHandle);
    CloseHandle(hMutexHandle);

    return EXIT_SUCCESS;
}

DWORD WINAPI MainMenuThread(LPVOID lpParam) {
    Central* central = (Central*)lpParam;
    TCHAR command[COMMAND_SIZE] = TEXT("");
    TCHAR* pch = new TCHAR[COMMAND_SIZE];
    TCHAR* something  = new TCHAR[COMMAND_SIZE];
    HANDLE sCanRead;
    WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 5);

    while (!central->isExit()) {
        _tprintf(TEXT("COMMAND: "));
        do {
            if (fgetws(command, sizeof(command), stdin) == NULL) {
                central->dll->log((TCHAR*)TEXT("Ocorreu um erro a ler o comando inserido!"), TYPE::ERRO);
            }
        } while (sizeof(command) < 1);
        int spaces = 0;
        for (int i = 0; i < sizeof(command) && command[i]; i++)
        {
            if (command[i] == '\n')
                command[i] = '\0';
            if (command[i] == ' ')
                spaces++;
        }
        if (spaces > 0) {
            pch = _tcstok_s(command, TEXT(" "), &something);
        }
        else {
            pch = command;
        }

        if (_tcscmp(pch, TEXT("exit")) == 0) {

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

            _tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
            central->setExit(true);
            ReleaseSemaphore(sCanRead, 1, NULL);
            wt->wait();
            delete wt;
            delete central;
            CloseHandle(sCanRead);
            CloseHandle(hEventClose);
            exit(EXIT_SUCCESS);
        }
        else if (_tcscmp(pch, TEXT("listarcarros")) == 0) {
            if (central->cars.size() > 0) {
                for (int i = 0; i < central->cars.size(); i++)
                {
                    _tprintf(TEXT("Carro #%d está na posição <%d, %d> com a matricula %s.\n"), central->cars[i]->getId(), central->cars[i]->getRow(), central->cars[i]->getCol(), central->cars[i]->getPlate());
                }
            }else{
                _tprintf(TEXT("Não existem taxis associados a nossa central neste momento!\n"));
            }
        }
        else if (_tcscmp(pch, TEXT("expulsar")) == 0) {
            pch = _tcstok_s(something, TEXT(" "), &something);
            Car* car = central->getCar(pch);
            if (car != nullptr) {
                if (car->getClient()[0] == '\0') {
                    _tprintf(TEXT("O carro %s vai ser expulso!\n"), car->getPlate());
                    central->expulsar(car);
                    central->deleteCar(car);
                }
                else {
                    central->dll->log((TCHAR*)TEXT("Não é possivel expulsar um carro que está a transportar um cliente!"), TYPE::WARNING);
                }
            }
            else {
                central->dll->log((TCHAR*)TEXT("Não é possivel expulsar um carro que nao está no sistema!"), TYPE::WARNING);
            }
        }
        else if (_tcscmp(pch, TEXT("listarpassageiros")) == 0) {
            // Colocar codigo na segunda meta;
            _tprintf(TEXT("Esta funcionalidade ainda não está implementada!\n"));
        }
        else if (_tcscmp(pch, TEXT("tempo")) == 0) {
            pch = _tcstok_s(something, TEXT(" "), &something);

            int val = DEFAULT_WAIT_TIME;
            if (pch != NULL) {
                try {
                    val = stoi(pch);
                }
                catch (invalid_argument) {
                    central->dll->log((TCHAR*)TEXT("Valor inválido!"), TYPE::WARNING);
                    continue;
                }
                catch (out_of_range) {
                    central->dll->log((TCHAR*)TEXT("Este valor não é muito grande?"), TYPE::WARNING);
                    continue;
                }
                if (central->bufferCircular->getTotalPassengers() == 0) {
                    central->setWaitTime(val);
                    central->dll->log((TCHAR*)TEXT("Não é possivel alterar o tempo de espera foi alterado!"), TYPE::WARNING);
                }
                else {
                    central->dll->log((TCHAR*)TEXT("Não é possivel alterar o tempo enquanto existem passageiros a espera!"), TYPE::WARNING);
                }
            }
        }
        else if (_tcscmp(pch, TEXT("pausar")) == 0) {
            central->setTakingIn(false);
            _tprintf(TEXT("A central não aceita novos taxis!\n"));
        }
        else if (_tcscmp(pch, TEXT("recomeçar")) == 0) {
            central->setTakingIn(true);
            _tprintf(TEXT("A central voltou a aceitar novos taxis!\n"));
        }
        else {
            central->dll->log((TCHAR*)TEXT("O comando inserido não existe!"), TYPE::WARNING);
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
        if (central->isTakingIn()) {
            WaitForSingleObject(sCanRead, INFINITE);
            if (!central->isExit()) {
                if (!central->carExists(pBuf)) {
                    Car* c = new Car(pBuf);
                    central->addCar(c);
                    tstringstream msg;
                    msg << "\n[NEW CAR] Entrou um novo taxi! Matricula: " << c->getPlate() << endl;
                    _tprintf(msg.str().c_str());
                    _tprintf(TEXT("COMMAND: "));
                }
                else {
                    central->updateCar(pBuf);
                }
                ReleaseSemaphore(sCanWrite, 1, NULL);
            }
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
    HANDLE hFile, hFileMapping, hFileMappingMap, sCanRead, sCanWrite, sCanSize, sCanMap;
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
    sCanMap = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_SHAREMAP_WANT);

    if (sCanWrite == NULL || sCanRead == NULL || sCanSize == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(sCanMap);
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
        CloseHandle(sCanMap);
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
        CloseHandle(sCanMap);
        CloseHandle(sCanSize);
        CloseHandle(hFileMappingMap);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SHAREDMEMORY_CONTAXI_MAP_SIZE, 7);
    central->dll->regist((TCHAR*)SHAREDMEMORY_CONTAXI_MAP, 7);

    while (!central->isExit()) {
        WaitForSingleObject(sCanMap, INFINITE);
        mapInfo.size = central->getSizeMap();
        mapInfo.canRegist = central->isTakingIn();
        CopyMemory(pBuf, &mapInfo, sizeof(MAPINFO));
        ReleaseSemaphore(sCanSize, 1, NULL);
        WaitForSingleObject(sCanWrite, INFINITE);
        CopyMemory((PVOID)pMap, central->getCleanMap(), central->getSizeMap());
        ReleaseSemaphore(sCanRead, 1, NULL);
    }
    CloseHandle(sCanMap);
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
    HANDLE hFile, hFileMapping, sCanRead, sCanSize, hFileMappingMap;
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

    sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_MAPINFO_READ);
    sCanSize = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_MAPINFO_SIZE);

    if (sCanRead == NULL || sCanSize == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead!"), TYPE::ERRO);
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
    CloseHandle(sCanRead);
    CloseHandle(sCanSize);
    UnmapViewOfFile(pBuf);
    UnmapViewOfFile(pMap);
    CloseHandle(hFileMapping);
    CloseHandle(hFileMappingMap);
    return EXIT_SUCCESS;
}

DWORD WINAPI ConPassThread(LPVOID lpParam) {
    Central* central = (Central*)lpParam;
    PASSENGER* passenger = new PASSENGER;
    ZeroMemory(passenger, sizeof(PASSENGER));
    while (!central->isExit()) {
        vector<Passageiro*> passengers = central->getClients();
        passenger = central->readConpassNP();
        if (passenger != nullptr) {
            passenger->status = central->validateClient(passenger);

            if (passenger->status == STATUS::EMESPERA) {
                DWORD idThread;
                HANDLE hThread = CreateThread(NULL, 0, WaitForAnswers, central, 0, &idThread);
                if (hThread == NULL) {
                    central->dll->log((TCHAR*)TEXT("Não foi possivel criar a Thread para um cliente!"), TYPE::ERRO);
                    return EXIT_FAILURE;
                }
                central->addHandle(&hThread);
            }

            central->writeConpassNP(passenger);
        }
    }
    delete passenger;
    return EXIT_SUCCESS;
}

DWORD WINAPI ReadInterest(LPVOID lpParam) {
    Central* central = (Central*)lpParam;
    HANDLE hFile, sCanWrite, sCanRead, hFileMapping;
    INTEREST interested;
    INTEREST* pBuf;

    hFile = CreateFile(SHAREDMEMORY_CEN_CON, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possível criar o ficheiro de memoria para interesses!"), TYPE::ERRO);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SHAREDMEMORY_CEN_CON, 6);

    sCanWrite = CreateSemaphore(NULL, 1, 1, SEMAPHORE_INTEREST_WRITE);
    sCanRead = CreateSemaphore(NULL, 0, 1, SEMAPHORE_INTEREST_READ);

    if (sCanWrite == NULL || sCanRead == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead Interesse!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SEMAPHORE_INTEREST_WRITE, 3);
    central->dll->regist((TCHAR*)SEMAPHORE_INTEREST_READ, 3);

    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(INTEREST), SHAREDMEMORY_INTEREST_ZONE);


    if (hFileMapping == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping interesse!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }

    pBuf = (INTEREST*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(INTEREST));
    if (pBuf == NULL) {
        central->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
        CloseHandle(sCanWrite);
        CloseHandle(sCanRead);
        CloseHandle(hFile);
        CloseHandle(hFileMapping);
        return EXIT_FAILURE;
    }
    central->dll->regist((TCHAR*)SHAREDMEMORY_INTEREST_ZONE, 7);


    while (!central->isExit()) {
        WaitForSingleObject(sCanRead, INFINITE);
        CopyMemory(&interested, pBuf, sizeof(INTEREST));
        Car* car = central->getCar(interested.car);
        Passageiro* client = central->getClient(interested.passenger);
        if (!central->isExit()) {
            central->addInterest(car, client);
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

DWORD WINAPI WaitForAnswers(LPVOID lpParam) {
    Central* central = (Central*)lpParam;
    WaitableTimer* wt = new WaitableTimer(central->getWaitTime() * WAIT_ONE_SECOND);
    wt->wait();
    Passageiro* client = central->getPassageiro(&central->bufferCircular->readPassenger());
    _tprintf(TEXT("\nO passageiro %s recebeu %d interesses!\nCOMMAND:"), client->getId(), (int)client->getInterested().size());

    Car * car = client->getRandomInterested();
    if (car != nullptr) {
        car->setClient(client->getId());
        client->setStatus(STATUS::ACAMINHO);
        central->sendAnswer(car, client);
        _tprintf(TEXT("\nO carro %s vai transportar o passageiro %s\nCOMMAND:"), car->getPlate(), client->getId());
        PASSENGER* p = &client->getStruct();
        _tcscpy_s(p->plate, TAXI_PLATE_SIZE, car->getPlate());
        central->writeConpassNP(p);
    }
    else {
        client->setStatus(STATUS::SEMINTERESSE);
        central->writeConpassNP(&client->getStruct());
    }
    
    delete wt;
    return EXIT_SUCCESS;
}