#include "Cooms.h"

DWORD SendCar(Car* car) {
	HANDLE hFileMapping, sCanRead, sCanWrite;
	TAXI* pBuf;
	HMODULE hDLL;
	TAXI taxi = car->toStruct();

	hDLL = LoadLibrary(DLL_PATH_64);
	if (hDLL == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel carregar a DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
		return EXIT_FAILURE;
	}

	FuncRegister fRegister = (FuncRegister)GetProcAddress(hDLL, "dll_register");
	FuncLog fLog = (FuncLog)GetProcAddress(hDLL, "dll_log");

	sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_CAN_WRITE_CENCON);
	sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_CAN_READ_CENCON);
	if (sCanWrite == NULL || sCanRead == NULL) {
		tstringstream msg;
		msg << "[ERRO] Não foi possivel criar o somafro!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		FreeLibrary(hDLL);
		return EXIT_FAILURE;
	}
	fRegister((TCHAR*)SEMAPHORE_CAN_WRITE_CENCON, 3);
	fRegister((TCHAR*)SEMAPHORE_CAN_READ_CENCON, 3);
	hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_CEN_CON_ZONE);

	if (hFileMapping == NULL) {
		tstringstream msg;
		msg << "[ERRO] Não foi possivel criar o file mapping!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		FreeLibrary(hDLL);
		return EXIT_FAILURE;
	}
	pBuf = (TAXI*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TAXI));
	if (pBuf == NULL) {
		tstringstream msg;
		msg << "[ERRO] Não foi possivel mapear o ficheiro!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		FreeLibrary(hDLL);
		return EXIT_FAILURE;
	}
	fRegister((TCHAR*)SHAREDMEMORY_CEN_CON_ZONE, 7);

	WaitForSingleObject(sCanWrite, INFINITE);
	CopyMemory((TAXI*)pBuf, &taxi, sizeof(TAXI));
	ReleaseSemaphore(sCanRead, 1, NULL);
	FreeLibrary(hDLL);
	CloseHandle(sCanWrite);
	CloseHandle(sCanRead);
	UnmapViewOfFile(pBuf);
	CloseHandle(hFileMapping);
	return EXIT_SUCCESS;
}
BOOL validatePlate(TCHAR* plate) {
	HANDLE hFileMapping, sCanRead, sConTaxi, sCanWrite;
	PLATE* pBuf;
	HMODULE hDLL;
	PLATE p;
	BOOL Exists = false;
	_tcscpy_s(p.plate, TAXI_PLATE_SIZE, plate);
	p.status = 0;
	hDLL = LoadLibrary(DLL_PATH_64);
	if (hDLL == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel carregar a DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
		return Exists;
	}

	FuncRegister fRegister = (FuncRegister)GetProcAddress(hDLL, "dll_register");
	FuncLog fLog = (FuncLog)GetProcAddress(hDLL, "dll_log");

	sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_WRITE);
	sConTaxi = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_CONTAXI);
	sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_PLATE_VALIDATOR_READ);
	if (sCanWrite == NULL || sCanRead == NULL || sConTaxi == NULL) {
		tstringstream msg;
		msg << "[ERRO] Não foi possivel criar o somafro!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(sConTaxi);
		FreeLibrary(hDLL);
		return Exists;
	}
	fRegister((TCHAR*)SEMAPHORE_CAN_WRITE_CENCON, 3);
	fRegister((TCHAR*)SEMAPHORE_CAN_READ_CENCON, 3);
	fRegister((TCHAR*)SEMAPHORE_PLATE_VALIDATOR_CONTAXI, 3);
	hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHAREDMEMORY_PLATE_VALIDATION);

	if (hFileMapping == NULL) {
		tstringstream msg;
		msg << "[ERRO] Não foi possivel criar o file mapping!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(sConTaxi);
		CloseHandle(hFileMapping);
		FreeLibrary(hDLL);
		return Exists;
	}
	pBuf = (PLATE *)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(PLATE));
	if (pBuf == NULL) {
		tstringstream msg;
		msg << "[ERRO] Não foi possivel mapear o ficheiro!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		CloseHandle(sConTaxi);
		FreeLibrary(hDLL);
		return false;
	}
	fRegister((TCHAR*)SHAREDMEMORY_CEN_CON_ZONE, 7);



	WaitForSingleObject(sCanWrite, INFINITE);
	CopyMemory(pBuf, &p, sizeof(PLATE));
	ReleaseSemaphore(sCanRead, 1, NULL);
	WaitForSingleObject(sConTaxi, INFINITE);
	Exists = pBuf->status;
	FreeLibrary(hDLL);
	CloseHandle(sCanWrite);
	CloseHandle(sCanRead);
	CloseHandle(sConTaxi);
	UnmapViewOfFile(pBuf);
	CloseHandle(hFileMapping);
	return Exists;
}
