#include "Cooms.h"

DWORD SendCar(Taxista* taxista) {
	HANDLE hFileMapping, sCanRead, sCanWrite;
	TAXI* pBuf;
	TAXI taxi = taxista->car->toStruct();
	DLLProfessores* dll = new DLLProfessores();

	sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_CAN_WRITE_CENCON);
	sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_CAN_READ_CENCON);

	if (sCanWrite == NULL || sCanRead == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead!"), TYPE::ERRO);
		delete dll;
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		return EXIT_FAILURE;
	}
	dll->regist((TCHAR*)SEMAPHORE_CAN_WRITE_CENCON, 3);
	dll->regist((TCHAR*)SEMAPHORE_CAN_READ_CENCON, 3);
	dll->regist((TCHAR*)SEMAPHORE_CAN_CONTAXI_CENCON, 3);
	hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_CEN_CON_ZONE);

	if (hFileMapping == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping!"), TYPE::ERRO);
		delete dll;
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}
	pBuf = (TAXI*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TAXI));
	if (pBuf == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
		delete dll;
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}
	dll->regist((TCHAR*)SHAREDMEMORY_CEN_CON_ZONE, 7);

	WaitForSingleObject(sCanWrite, INFINITE);
	CopyMemory((TAXI*)pBuf, &taxi, sizeof(TAXI));
	ReleaseSemaphore(sCanRead, 1, NULL);
	delete dll;
	CloseHandle(sCanWrite);
	CloseHandle(sCanRead);
	UnmapViewOfFile(pBuf);
	CloseHandle(hFileMapping);
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
	HANDLE hFileMapping, sCanRead, sCanWrite;
	TAXI* pBuf;
	DLLProfessores* dll = new DLLProfessores();

	sCanWrite = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_SHAREMAP_WRITE);
	sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_SHAREMAP_READ);
	if (sCanWrite == NULL || sCanRead == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel criar o semafro sCanWrite ou sCanRead!"), TYPE::ERRO);
		delete dll;
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		return EXIT_FAILURE;
	}
	dll->regist((TCHAR*)SEMAPHORE_CAN_WRITE_CENCON, 3);
	dll->regist((TCHAR*)SEMAPHORE_CAN_READ_CENCON, 3);
	dll->regist((TCHAR*)SEMAPHORE_CAN_CONTAXI_CENCON, 3);

	hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_ZONE_SHAREMAP);
	if (hFileMapping == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel criar o file mapping! SHAREDMEMORY_ZONE_SHAREMAP"), TYPE::ERRO);
		delete dll;
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}

	pBuf = (TAXI*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TAXI));
	if (pBuf == NULL) {
		dll->log((TCHAR*)TEXT("Não foi possivel mapear o ficheiro!"), TYPE::ERRO);
		delete dll;
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}
	dll->regist((TCHAR*)SHAREDMEMORY_ZONE_SHAREMAP, 7);


	ReleaseSemaphore(sCanWrite, 1, NULL);
	WaitForSingleObject(sCanRead, INFINITE);
	taxista->setMap(new TownMap(pBuf->map));
	delete dll;
	CloseHandle(sCanWrite);
	CloseHandle(sCanRead);
	UnmapViewOfFile(pBuf);
	CloseHandle(hFileMapping);
	return EXIT_SUCCESS;
}