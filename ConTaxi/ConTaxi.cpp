#include "ConTaxi.h"

int _tmain(int argc ,TCHAR* argv[]) {

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
	HMODULE hDLL;

	hDLL = LoadLibrary(DLL_PATH_64);
	if (hDLL == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel carregar a DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
		return EXIT_FAILURE;
	}

	FuncRegister fRegister = (FuncRegister)GetProcAddress(hDLL, "dll_register");
	FuncLog fLog = (FuncLog)GetProcAddress(hDLL, "dll_log");

	liDueTime.QuadPart = WAIT_ONE_SECOND;

	Car car = getCarData();
	params.car = &car;
	params.exit = false;

	hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	if (hTimer == NULL) {
		tstringstream msg;
		msg << "[ERRO] Não foi possivel criar o WaitableTimer!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
		CloseHandle(hTimer);
		FreeLibrary(hDLL);
		return EXIT_FAILURE;
	}
	if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
		tstringstream msg;
		msg << "[ERRO] Não foi possivel iniciar o WaitableTimer!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
		CloseHandle(hTimer);
		FreeLibrary(hDLL);
		return EXIT_FAILURE;
	}

	CommandsThread = CreateThread(NULL, 0, CmdsThread, &params, 0, &ThreadIDArray[0]);
	CommunicationThread = CreateThread(NULL, 0, CommsThread, &params, 0, &ThreadIDArray[1]);

	if (CommandsThread == NULL || CommunicationThread == NULL) {
		tstringstream msg;
		msg << "[ERRO] Não foi possivel criar a Thread!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
		FreeLibrary(hDLL);
		return EXIT_FAILURE;
	}
	
	WaitForSingleObject(CommandsThread, INFINITE);
	WaitForSingleObject(CommunicationThread, INFINITE);
	CloseHandle(CommandsThread);
	CloseHandle(CommunicationThread);

	if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0) {
		tstringstream msg;
		msg << "[ERRO] Não foi possível iniciar o WaitForSingleObject!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
		CloseHandle(hTimer);
		FreeLibrary(hDLL);
		return EXIT_FAILURE;
	}
	FreeLibrary(hDLL);
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
	HMODULE hDLL;

	hDLL = LoadLibrary(DLL_PATH_64);
	if (hDLL == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel carregar a DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
		return EXIT_FAILURE;
	}

	FuncRegister fRegister = (FuncRegister)GetProcAddress(hDLL, "dll_register");
	FuncLog fLog = (FuncLog)GetProcAddress(hDLL, "dll_log");
	while (true) {
		_tprintf(TEXT("COMMAND: "));
		if (fgetws(command, sizeof(command), stdin) == NULL) {
			tstringstream msg;
			msg << "[ERRO] [ERRO] Ocorreu um erro a ler o comando inserido!" << endl;
			msg << "[CODE] " << GetLastError() << endl;
			_tprintf(msg.str().c_str());
			fLog((TCHAR*)msg.str().c_str());
		}
		for (int i = 0; i < sizeof(command) && command[i]; i++)
		{
			if (command[i] == '\n')
				command[i] = '\0';
		}
		if (_tcscmp(command, TEXT("exit")) == 0) {
			_tprintf(TEXT("Cya!\n"));
			params->exit = true;
			FreeLibrary(hDLL);
			return EXIT_SUCCESS;
		}
	}
}

DWORD WINAPI CommsThread(LPVOID lpParam) {
	
	HANDLE hFileMapping, sCanRead, sCanWrite;
	PARAMETERS *params = (PARAMETERS*)lpParam;
	TAXI* pBuf;
	TAXI taxi;
	HMODULE hDLL;

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
	hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE, SHAREDMEMORY_CEN_CON_ZONE);

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
	taxi.col = params->car->getCol();
	taxi.row = params->car->getRow();
	taxi.pid = params->car->getId();
	_stprintf_s(taxi.matricula, TEXT("%s"), params->car->getPlate());
	CopyMemory((TAXI*)pBuf, &taxi, sizeof(TAXI));
	ReleaseSemaphore(sCanRead, 1, NULL);
	FreeLibrary(hDLL);
	CloseHandle(sCanWrite);
	CloseHandle(sCanRead);
	UnmapViewOfFile(pBuf);
	CloseHandle(hFileMapping);
	return EXIT_SUCCESS;
}