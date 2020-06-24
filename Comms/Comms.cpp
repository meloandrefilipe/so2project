#include "Cooms.h"

DWORD SendCar(Taxista* taxista) {
	HANDLE hFileMapping, sCanRead, sCanWrite;
	TAXI* pBuf;
	TAXI taxi = taxista->car->toStruct();

	sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_CAN_WRITE_CENCON);
	sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_CAN_READ_CENCON);

	if (sCanWrite == NULL || sCanRead == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead!"), TYPE::ERRO);

		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		return EXIT_FAILURE;
	}

	hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_CEN_CON_ZONE);
	if (hFileMapping == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping!"), TYPE::ERRO);
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}
	pBuf = (TAXI*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TAXI));
	if (pBuf == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}

	WaitForSingleObject(sCanWrite, INFINITE);
	CopyMemory((TAXI*)pBuf, &taxi, sizeof(TAXI));
	ReleaseSemaphore(sCanRead, 1, NULL);
	CloseHandle(sCanWrite);
	CloseHandle(sCanRead);
	UnmapViewOfFile(pBuf);
	CloseHandle(hFileMapping);
	return EXIT_SUCCESS;
}

DWORD SendInterest(TCHAR* matricula, TCHAR * client) {
	HANDLE sCanWrite, sCanRead, hFileMapping;
	INTEREST interest;
	_tcscpy_s(interest.car, TAXI_PLATE_SIZE, matricula);
	_tcscpy_s(interest.passenger, PASSENGER_NAME_SIZE, client);
	INTEREST* pBuf;
	DLLProfessores* dll = new DLLProfessores();

	sCanWrite = CreateSemaphore(NULL, 1, 1, SEMAPHORE_INTEREST_WRITE);
	sCanRead = CreateSemaphore(NULL, 0, 1, SEMAPHORE_INTEREST_READ);

	if (sCanWrite == NULL || sCanRead == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead Interesse!"), TYPE::ERRO);
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		return EXIT_FAILURE;
	}

	hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_INTEREST_ZONE);

	if (hFileMapping == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping interesse!"), TYPE::ERRO);
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}

	pBuf = (INTEREST*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(INTEREST));
	if (pBuf == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}

	WaitForSingleObject(sCanWrite, INFINITE);
	CopyMemory((INTEREST*)pBuf, &interest, sizeof(INTEREST));
	ReleaseSemaphore(sCanRead, 1, NULL);


	UnmapViewOfFile(pBuf);
	CloseHandle(sCanWrite);
	CloseHandle(sCanRead);
	CloseHandle(hFileMapping);
	delete dll;
	return EXIT_SUCCESS;
}

BOOL validatePlate(TCHAR* plate) {
	HANDLE hFileMapping, sCanRead, sConTaxi, sCanWrite;
	PLATE* pBuf;
	DLLProfessores* dll = new DLLProfessores();
	PLATE p;
	BOOL Exists = false;
	_tcscpy_s(p.plate, TAXI_PLATE_SIZE, plate);
	p.status = 0;


	sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_WRITE);
	sConTaxi = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_CONTAXI);
	sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_READ);
	if (sCanWrite == NULL || sCanRead == NULL || sConTaxi == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead ou sContaxi COMMS"), TYPE::ERRO);
		delete dll;
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(sConTaxi);
		return Exists;
	}
	dll->regist((TCHAR*)SEMAPHORE_CAN_WRITE_CENCON, 3);
	dll->regist((TCHAR*)SEMAPHORE_CAN_READ_CENCON, 3);
	dll->regist((TCHAR*)SEMAPHORE_PLATE_VALIDATOR_CONTAXI, 3);
	hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_PLATE_VALIDATION);

	if (hFileMapping == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping!"), TYPE::ERRO);
		delete dll;
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(sConTaxi);
		CloseHandle(hFileMapping);
		return Exists;
	}
	pBuf = (PLATE *)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(PLATE));
	if (pBuf == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
		delete dll;
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		CloseHandle(sConTaxi);
		return false;
	}
	dll->regist((TCHAR*)SHAREDMEMORY_CEN_CON_ZONE, 7);



	WaitForSingleObject(sCanWrite, INFINITE);
	CopyMemory(pBuf, &p, sizeof(PLATE));
	ReleaseSemaphore(sCanRead, 1, NULL);
	WaitForSingleObject(sConTaxi, INFINITE);
	Exists = pBuf->status;
	delete dll;
	CloseHandle(sCanWrite);
	CloseHandle(sCanRead);
	CloseHandle(sConTaxi);
	UnmapViewOfFile(pBuf);
	CloseHandle(hFileMapping);
	return Exists;
}

DWORD getMap(Taxista* taxista) {
	HANDLE hFileMapping, hFileMappingMap, sCanRead, sCanWrite, sCanSize, sCanMap;
	MAPINFO* pBuf;
	LPCTSTR pMap;

	sCanWrite = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_SHAREMAP_WRITE);
	sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_SHAREMAP_READ);
	sCanSize = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_SHAREMAP_SIZE);
	sCanMap = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_SHAREMAP_WANT);

	if (sCanWrite == NULL || sCanRead == NULL || sCanMap == NULL || sCanSize == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead!"), TYPE::ERRO);
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(sCanMap);
		CloseHandle(sCanSize);
		return EXIT_FAILURE;
	}
	taxista->dll->regist((TCHAR*)SEMAPHORE_SHAREMAP_WRITE, 3);
	taxista->dll->regist((TCHAR*)SEMAPHORE_SHAREMAP_READ, 3);
	taxista->dll->regist((TCHAR*)SEMAPHORE_SHAREMAP_SIZE, 3);
	taxista->dll->regist((TCHAR*)SEMAPHORE_SHAREMAP_WANT, 3);

	hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_CONTAXI_MAP_SIZE);
	hFileMappingMap = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_CONTAXI_MAP);

	if (hFileMapping == NULL || hFileMappingMap == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping! SHAREDMEMORY_ZONE_SHAREMAP"), TYPE::ERRO);
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(sCanMap);
		CloseHandle(hFileMapping);
		CloseHandle(hFileMappingMap);
		CloseHandle(sCanSize);
		return EXIT_FAILURE;
	}

	pBuf = (MAPINFO*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(MAPINFO));
	if (pBuf == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro! PBUF"), TYPE::ERRO);
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(sCanMap);
		CloseHandle(sCanSize);
		CloseHandle(hFileMappingMap);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}
	ReleaseSemaphore(sCanMap, 1, NULL);
	WaitForSingleObject(sCanSize, INFINITE);
	taxista->setMapSize(pBuf->size);
	taxista->setCanRegist(pBuf->canRegist);
	pMap = (LPTSTR)MapViewOfFile(hFileMappingMap, FILE_MAP_ALL_ACCESS, 0, 0, pBuf->size);
	if (pMap == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro! PMAP"), TYPE::ERRO);
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(sCanMap);
		CloseHandle(sCanSize);
		CloseHandle(hFileMappingMap);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}

	taxista->dll->regist((TCHAR*)SHAREDMEMORY_CONTAXI_MAP_SIZE, 7);
	taxista->dll->regist((TCHAR*)SHAREDMEMORY_CONTAXI_MAP, 7);

	ReleaseSemaphore(sCanWrite, 1, NULL);
	WaitForSingleObject(sCanRead, INFINITE);
	taxista->setMap(new TownMap((TCHAR*)pMap));
	CloseHandle(sCanWrite);
	CloseHandle(sCanRead);
	CloseHandle(sCanMap);
	CloseHandle(sCanSize);
	CloseHandle(hFileMappingMap);
	UnmapViewOfFile(pBuf);
	UnmapViewOfFile(pMap);
	CloseHandle(hFileMapping);
	return EXIT_SUCCESS;
}

DWORD bufferCircular(Taxista* taxista) {
	BUFFERCIRCULAR* bc = new BUFFERCIRCULAR;

	HANDLE hEvents[BUFFER_CIRCULAR_SIZE];
	HANDLE hFile, hFileMapping;

	ZeroMemory(bc, sizeof(BUFFERCIRCULAR));
	ZeroMemory(hEvents, BUFFER_CIRCULAR_SIZE * sizeof(HANDLE));

	for (int i = 0; i < BUFFER_CIRCULAR_SIZE; i++) {
		tstringstream msg;
		msg << "buffer.circular.event." << i << endl;
		hEvents[i] = CreateEvent(NULL, TRUE, FALSE, msg.str().c_str());
		if (hEvents[i] == NULL) {
			_tprintf(TEXT("Não foi possivel criar eventos do Buffer Circular!\n"));
			return EXIT_FAILURE;
		}
	}

	hFile = CreateFile(SHAREDMEMORY_CEN_CON, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possível criar o ficheiro de memoria para Buffer-Circular!"), TYPE::ERRO);
		CloseHandle(hFile);
		return EXIT_FAILURE;
	}
	hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(BUFFERCIRCULAR), SHAREDMEMORY_BUFFER_CIRCULAR);
	if (hFileMapping == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping Buffer-circular!"), TYPE::ERRO);
		CloseHandle(hFile);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}
	bc = (BUFFERCIRCULAR*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(BUFFERCIRCULAR));
	if (bc == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
		CloseHandle(hFile);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}
	while (!taxista->isExit()) {
		for (int i = 0; i <= bc->bufferPos; i++){
			if (taxista->isInNq(bc->dataArray[bc->bufferPos]) && taxista->getCanInterest() && taxista->car->getAutopicker()) {
			/*if (taxista->getCanInterest() && taxista->car->getAutopicker()) {*/
				DWORD event = WaitForSingleObject(hEvents[i], 30);
				if (WAIT_OBJECT_0 == event) {
					SendInterest(taxista->car->getPlate(), bc->dataArray[i].id);
					taxista->setCanInterest(FALSE);
					_tprintf(TEXT("\nMostrei interesse no %s\nCOMMAND:"), bc->dataArray[i].id);
				}
			}
			//else if (!taxista->car->getAutopicker() && taxista->getATransportar()[0] != '\0' && taxista->isInNq(bc->dataArray[bc->bufferPos])) {
			else if (!taxista->car->getAutopicker() && taxista->getATransportar()[0] != '\0') {
				DWORD event = WaitForSingleObject(hEvents[i], 30);
				if (WAIT_OBJECT_0 == event) {
					if (_tcscmp(bc->dataArray[i].id, taxista->getATransportar()) == 0) {
						SendInterest(taxista->car->getPlate(), bc->dataArray[i].id);
						taxista->setCanInterest(FALSE);
						taxista->clearATransportar();
						_tprintf(TEXT("\nMostrei interesse no %s\nCOMMAND:"), bc->dataArray[i].id);
					}
				}
			}
		}
	}

	for (int i = 0; i < BUFFER_CIRCULAR_SIZE; i++) {
		CloseHandle(hEvents[i]);
	}
	CloseHandle(hFile);
	CloseHandle(hFileMapping);
	return EXIT_SUCCESS;
}