#include "ConTaxi.h"


int wmain(int argc ,TCHAR* argv[]) {
#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	DWORD ThreadIDArray[2];
	HANDLE CommandsThread, CommunicationThread;

	PARAMETERS params; 

	Car car = getCarData();

	params.car = car.toString();
	params.pid = GetCurrentProcessId();
	params.exit = false;

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

	return EXIT_SUCCESS;
}

Car getCarData() {
	int row, col;
	TCHAR plate[20] = TEXT("XX XX XX");
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
	else {
		_tprintf(TEXT("Carro: %d %d %s"), col, row, plate);
	}
	for (int i = 0; i < sizeof(plate) && plate[i]; i++)
	{
		if (plate[i] == '\n')
			plate[i] = '\0';
	}

	return 	Car(row, col, plate);
}

DWORD WINAPI CmdsThread(LPVOID lpParam) {
	TCHAR command[100] = TEXT("");
	HANDLE hTimer;
	LARGE_INTEGER liDueTime;
	PARAMETERS* params = (PARAMETERS*)lpParam;
	liDueTime.QuadPart = WAIT_ONE_SECOND;


	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (hTimer == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel criar o WaitableTimer!\n[CODE] %d\n"), GetLastError());
		CloseHandle(hTimer);
		return EXIT_FAILURE;
	}

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
			if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
				_tprintf(TEXT("[ERRO] Não foi possivel iniciar o WaitableTimer!\n[CODE] %d\n"), GetLastError());
				CloseHandle(hTimer);
				return EXIT_FAILURE;
			}
			if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0) {
				printf("[ERRO] Não foi possível iniciar o WaitForSingleObject!\n[CODE] %d\n", GetLastError());
				CloseHandle(hTimer);
				return EXIT_FAILURE;
			}
			params->exit = true;
			CloseHandle(hTimer);
			return EXIT_SUCCESS;
		}
	}
}

DWORD WINAPI CommsThread(LPVOID lpParam) {
	
	HANDLE hFileMapping, sCanRead, sCanWrite;
	PARAMETERS *params = (PARAMETERS*)lpParam;
	LPCTSTR pBuf;

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
	pBuf = (LPTSTR)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(BUFFER_SIZE));

	if (pBuf == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel mapear o ficheiro!\n[CODE] %d\n"), GetLastError());
		CloseHandle(sCanWrite);
		CloseHandle(sCanRead);
		CloseHandle(hFileMapping);
		return EXIT_FAILURE;
	}

	WaitForSingleObject(sCanWrite, INFINITE);
	TCHAR buf[BUFFER_SIZE];
	_stprintf_s(buf, TEXT("%s %d"), params->car,params->pid);

	CopyMemory((PVOID)pBuf, buf, ( _tcslen(buf) * sizeof(TCHAR)));
	ReleaseSemaphore(sCanRead, 1, NULL);

	CloseHandle(sCanWrite);
	CloseHandle(sCanRead);
	UnmapViewOfFile(pBuf);
	CloseHandle(hFileMapping);
	return EXIT_SUCCESS;
}