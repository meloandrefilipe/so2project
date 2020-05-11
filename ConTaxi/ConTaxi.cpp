#include "ConTaxi.h"


int wmain(int argc ,TCHAR* argv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	HANDLE hTimer;
	LARGE_INTEGER liDueTime;
	DWORD ThreadIDArray[2];
	HANDLE CommandsThread, CommunicationThread;
	PARAMETERS params; 

	liDueTime.QuadPart = WAIT_ONE_SECOND;

	Car car = getCarData();
	params.car = &car;
	params.exit = false;

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

	CommandsThread = CreateThread(NULL, 0, CmdsThread, &params, 0, &ThreadIDArray[0]);
	CommunicationThread = CreateThread(NULL, 0, CommsThread, &params, 0, &ThreadIDArray[1]);

	if (CommandsThread == NULL || CommunicationThread == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel criar a Thread!\n[CODE] %d\n"), GetLastError());
		return EXIT_FAILURE;
	}
	
	WaitForSingleObject(CommandsThread, INFINITE);
	WaitForSingleObject(CommunicationThread, INFINITE);
	CloseHandle(CommandsThread);
	CloseHandle(CommunicationThread);

	if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0) {
		printf("[ERRO] Não foi possível iniciar o WaitForSingleObject!\n[CODE] %d\n", GetLastError());
		CloseHandle(hTimer);
		return EXIT_FAILURE;
	}
	CloseHandle(hTimer);
	return EXIT_SUCCESS;
}

Car getCarData() {
	int row, col;
	TCHAR plate[TAXI_PLATE_SIZE] = TEXT("XX XX XX");
	_tprintf(TEXT("Indique as coordenadas do seu veículo!\n"));

	
	while (true) {
		_tprintf(TEXT("X: "));
		wcin >> col;
		if (wcin.fail()) {
			_tprintf(TEXT("Insira um valor inteiro para a coordenada!\n"));
			wcin.clear();
			wcin.ignore(256, '\n');
			continue;
		}
		else break;
	}

	while (true) {
		_tprintf(TEXT("Y: "));
		wcin >> row;
		if (wcin.fail()) {
			_tprintf(TEXT("Insira um valor inteiro para a coordenada!\n"));
			wcin.clear();
			wcin.ignore(256, '\n');
			continue;
		}
		else break;
	}
	wcin.clear();
	wcin.ignore(256, '\n');
	_tprintf(TEXT("Matricula: "));
	if (fgetws(plate, sizeof(plate), stdin) == NULL) {
		_tprintf(TEXT("[ERRO] Ocorreu um erro a ler a matricula\n[CODE] %d\n"),GetLastError());
	}
	for (int i = 0; i < sizeof(plate) && plate[i]; i++)
	{
		if (plate[i] == '\n')
			plate[i] = '\0';
	}

	return 	Car(GetCurrentProcessId(), row, col, plate);
}

DWORD WINAPI CmdsThread(LPVOID lpParam) {
	TCHAR command[COMMAND_SIZE] = TEXT("");
	PARAMETERS* params = (PARAMETERS*)lpParam;

	while (true) {
		_tprintf(TEXT("COMMAND: "));
		if (fgetws(command, sizeof(command), stdin) == NULL) {
			_tprintf(TEXT("[ERRO] Ocorreu um erro a ler o comando inserido!\n[CODE] %d\n"), GetLastError());
		}
		for (int i = 0; i < sizeof(command) && command[i]; i++)
		{
			if (command[i] == '\n')
				command[i] = '\0';
		}
		if (_tcscmp(command, TEXT("exit")) == 0) {
			_tprintf(TEXT("Cya!\n"));
			params->exit = true;
			return EXIT_SUCCESS;
		}
	}
}

DWORD WINAPI CommsThread(LPVOID lpParam) {
	
	HANDLE hFileMapping, sCanRead, sCanWrite;
	PARAMETERS *params = (PARAMETERS*)lpParam;
	TAXI* pBuf;
	TAXI taxi;

	sCanWrite = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, SEMAPHORE_CAN_WRITE_CENCON);
	sCanRead = CreateSemaphore(NULL, 0, BUFFER_SIZE, SEMAPHORE_CAN_READ_CENCON);


	if (sCanWrite == NULL || sCanRead == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel criar o somafro!\n[CODE] %d\n"), GetLastError());
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		return EXIT_FAILURE;
	}

	hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE, SHAREDMEMORY_CEN_CON_ZONE);

	if (hFileMapping == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel criar o file mapping!\n[CODE] %d\n"), GetLastError());
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}
	pBuf = (TAXI*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(TAXI));

	if (pBuf == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel mapear o ficheiro!\n[CODE] %d\n"), GetLastError());
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}

	WaitForSingleObject(sCanWrite, INFINITE);
	taxi.col = params->car->getCol();
	taxi.row = params->car->getRow();
	taxi.pid = params->car->getId();
	_stprintf_s(taxi.matricula, TEXT("%s"), params->car->getPlate());
	CopyMemory((TAXI*)pBuf, &taxi, sizeof(TAXI));
	ReleaseSemaphore(sCanRead, 1, NULL);

	CloseHandle(sCanWrite);
	CloseHandle(sCanRead);
	UnmapViewOfFile(pBuf);
	CloseHandle(hFileMapping);
	return EXIT_SUCCESS;
}