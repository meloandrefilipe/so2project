#include "ConTaxi.h"
#include "Cooms.h"

int _tmain(int argc ,TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	HANDLE hTimer;
	LARGE_INTEGER liDueTime;
	DWORD idCommandsThread, idCommunicationThread, idCloseThread, idGetCarDataThread;
	HANDLE commandsThread, communicationThread, closeThread, getCarDataThread;
	PARAMETERS params; 
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
		return EXIT_FAILURE;
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

	getCarDataThread = CreateThread(NULL, 0, GetCarDataThread, &params, 0, &idGetCarDataThread);
	closeThread = CreateThread(NULL, 0, CloseThread, &params, 0, &idCloseThread);
	WaitForSingleObject(getCarDataThread, INFINITE);
	commandsThread = CreateThread(NULL, 0, CommandsThread, &params, 0, &idCommandsThread);
	communicationThread = CreateThread(NULL, 0, CommunicationThread, &params, 0, &idCommunicationThread);


	if (commandsThread == NULL || communicationThread == NULL || closeThread == NULL) {
		tstringstream msg;
		msg << "[ERRO] Não foi possivel criar a Thread!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
		FreeLibrary(hDLL);
		return EXIT_FAILURE;
	}
	
	WaitForSingleObject(commandsThread, INFINITE);
	WaitForSingleObject(communicationThread, INFINITE);
	WaitForSingleObject(closeThread, INFINITE);

	CloseHandle(getCarDataThread);
	CloseHandle(commandsThread);
	CloseHandle(closeThread);
	CloseHandle(communicationThread);
	_tprintf(TEXT("Cya!\n"));

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
DWORD WINAPI GetCarDataThread(LPVOID lpParam) {
	PARAMETERS* params = (PARAMETERS*)lpParam;
	int row = 0; 
	int col = 0;
	TCHAR plate[TAXI_PLATE_SIZE] = TEXT("XX XX XX");

	_tprintf(TEXT("Indique a matrícula do seu veículo : "));
	int validate = 0;
	do {
		if (validate == 1) {
			_tprintf(TEXT("Esta matricula já se encontra no nosso sistema, coloque outra: "));
		}
		if (fgetws(plate, sizeof(plate), stdin) == NULL) {
			_tprintf(TEXT("[ERRO] Ocorreu um erro a ler a matrícula\n[CODE] %d\n"), GetLastError());
		}
		for (int i = 0; i < sizeof(plate) && plate[i]; i++)
		{
			if (plate[i] == '\n')
				plate[i] = '\0';
		}
		validate = validatePlate(plate);
	} while (validate != 0);
	
	
	while (!params->exit) {
		_tprintf(TEXT("Indique a coordenada X: "));
		wcin >> col;
		if (wcin.fail()) {
			_tprintf(TEXT("Insira um valor inteiro para a coordenada!\n"));
			wcin.clear();
			wcin.ignore(256, '\n');
			continue;
		}
		else break;
	}

	while (!params->exit) {
		_tprintf(TEXT("Indique a coordenada Y: "));
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
	params->car = new Car(GetCurrentProcessId(), row, col, plate);
	return EXIT_SUCCESS;
}

DWORD WINAPI CommandsThread(LPVOID lpParam) {
	TCHAR command[COMMAND_SIZE] = TEXT("");
	TCHAR* pch;
	TCHAR* something;
	PARAMETERS* params = (PARAMETERS*)lpParam;
	HMODULE hDLL;

	hDLL = LoadLibrary(DLL_PATH_64);
	if (hDLL == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel carregar a DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
		return EXIT_FAILURE;
	}

	FuncRegister fRegister = (FuncRegister)GetProcAddress(hDLL, "dll_register");
	FuncLog fLog = (FuncLog)GetProcAddress(hDLL, "dll_log");
	while (!params->exit) {
		_tprintf(TEXT("COMMAND: "));
		if (fgetws(command, sizeof(command), stdin) == NULL) {
			tstringstream msg;
			msg << "[ERRO] Ocorreu um erro a ler o comando inserido!" << endl;
			msg << "[CODE] " << GetLastError() << endl;
			_tprintf(msg.str().c_str());
			fLog((TCHAR*)msg.str().c_str());
		}
		for (int i = 0; i < sizeof(command) && command[i]; i++)
		{
			if (command[i] == '\n')
				command[i] = '\0';
		}
		pch = _tcstok_s(command, TEXT(" "), &something);
		if (_tcscmp(pch, TEXT("exit")) == 0) {
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
			FreeLibrary(hDLL);
			CloseHandle(hEventClose);
			return EXIT_SUCCESS;
		}
		else if (_tcscmp(pch, TEXT("acelerar")) == 0) {
			params->car->speedUp();
		}
		else if (_tcscmp(pch, TEXT("desacelerar")) == 0) {
			params->car->speedDown();
		}
		else if (_tcscmp(pch, TEXT("nq")) == 0) {

			pch = _tcstok_s(something, TEXT(" "), &something);

			int val = NQ;

			try {
				val = stoi(pch);
			}
			catch (invalid_argument) {
				tstringstream msg;
				msg << "[ERRO] Valor inválido!" << endl;
				msg << "[CODE] " << GetLastError() << endl;
				_tprintf(msg.str().c_str());
				fLog((TCHAR*)msg.str().c_str());
				continue;
			}
			catch (out_of_range) {
				tstringstream msg;
				msg << "[ERRO] Este valor não é muito grande?" << endl;
				msg << "[CODE] " << GetLastError() << endl;
				_tprintf(msg.str().c_str());
				fLog((TCHAR*)msg.str().c_str());
				continue;
			}
			params->car->setNq(val);
		}
		else if (_tcscmp(pch, TEXT("autopicker")) == 0) {

			pch = _tcstok_s(something, TEXT(" "), &something);

			if (_tcscmp(pch, TEXT("on")) == 0) {
				params->car->setAutopicker(true);
				tstringstream msg;
				msg << "[AUTOPICKER] Ativado!" << endl;
				_tprintf(msg.str().c_str());
				fLog((TCHAR*)msg.str().c_str());

			}
			else if (_tcscmp(pch, TEXT("off")) == 0) {
				params->car->setAutopicker(false);
				tstringstream msg;
				msg << "[AUTOPICKER] Desativado!" << endl;
				_tprintf(msg.str().c_str());
				fLog((TCHAR*)msg.str().c_str());
			}
			else {
				tstringstream msg;
				msg << "[WARNING] A Opção inserida não existe!" << endl;
				_tprintf(msg.str().c_str());
				fLog((TCHAR*)msg.str().c_str());
				continue;
			}
		}
		else if (_tcscmp(pch, TEXT("transportar")) == 0) {

			pch = _tcstok_s(something, TEXT(" "), &something);

			int traveler = 0;

			try {
				traveler = stoi(pch);
			}
			catch (invalid_argument) {
				tstringstream msg;
				msg << "[ERRO] Valor inválido!" << endl;
				msg << "[CODE] " << GetLastError() << endl;
				_tprintf(msg.str().c_str());
				fLog((TCHAR*)msg.str().c_str());
				continue;
			}
			catch (out_of_range) {
				tstringstream msg;
				msg << "[ERRO] Este valor não é muito grande?" << endl;
				msg << "[CODE] " << GetLastError() << endl;
				_tprintf(msg.str().c_str());
				fLog((TCHAR*)msg.str().c_str());
				continue;
			}
			tstringstream msg;
			msg << "[PASSENGER] Vou até ao passageiro #%d" << traveler << endl;
			msg << "[CODE] " << GetLastError() << endl;
			_tprintf(msg.str().c_str());
			fLog((TCHAR*)msg.str().c_str());


			// IMPLEMENTAR CODIGO DE TRANSPORTE DO PASSAGEIRO
		}
		else {
			tstringstream msg;
			msg << "[WARNING] O comando inserido não existe!" << endl;
			_tprintf(msg.str().c_str());
			fLog((TCHAR*)msg.str().c_str());
		}
	}
	return EXIT_SUCCESS;
}

DWORD WINAPI CommunicationThread(LPVOID lpParam) {
	
	PARAMETERS *params = (PARAMETERS*)lpParam;

	return SendCar(params->car);
}

DWORD WINAPI CloseThread(LPVOID lpParam) {

	PARAMETERS* params = (PARAMETERS*)lpParam;
	HMODULE hDLL;
	HANDLE hEventClose;

	hDLL = LoadLibrary(DLL_PATH_64);
	if (hDLL == NULL) {
		_tprintf(TEXT("[ERRO] Não foi possivel carregar a DLL 'dos professores'!\n[CODE] %d\n"), GetLastError());
		return EXIT_FAILURE;
	}

	FuncRegister fRegister = (FuncRegister)GetProcAddress(hDLL, "dll_register");
	FuncLog fLog = (FuncLog)GetProcAddress(hDLL, "dll_log");

	hEventClose = CreateEvent(NULL, FALSE, FALSE, EVENT_CLOSE_ALL);
	if (hEventClose == NULL) {
		tstringstream msg;
		msg << "[ERRO] Não foi possivel criar o evento!" << endl;
		msg << "[CODE] " << GetLastError() << endl;
		_tprintf(msg.str().c_str());
		fLog((TCHAR*)msg.str().c_str());
	}

	WaitForSingleObject(hEventClose, INFINITE);
	_tprintf(TEXT("[WARNING] A Central fechou!\nA fechar a aplicação..."));

	params->exit = true;
	_tprintf(TEXT("Cya!\n"));

	FreeLibrary(hDLL);
	CloseHandle(hEventClose);
	exit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}