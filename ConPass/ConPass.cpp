#include "ConPass.h"

int _tmain(int argc, TCHAR argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	DWORD idCommandsThread, idCloseThread, idPipeReadThread;
	HANDLE hMutex, hCommandsThread, hCloseThread, hCanBoot, hPipeReadThread;


	hCanBoot = CreateEvent(NULL, TRUE, FALSE, EVENT_BOOT_ALL);
	if (hCanBoot == NULL) {
		_tprintf(TEXT("Não foi possivel criar o evento de espera!"));
		return EXIT_FAILURE;
	}
	_tprintf(TEXT("A ESPERA...\n"));
	Estacao* estacao = new Estacao();
	WaitForSingleObject(hCanBoot, INFINITE);
	CloseHandle(hCanBoot);
	Clear();


	// Criar um named mutex para garantir que existe apenas uma ConPass a correr no sistema
	hMutex = CreateMutex(NULL, TRUE, CONPASS_MAIN_MUTEX);
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		estacao->dll->log((TCHAR*)TEXT("Já existe um ConPass a correr!"), TYPE::ERRO);
		WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 5);
		wt->wait();
		delete wt;
		delete estacao;
		return EXIT_FAILURE;
	}

	_tprintf(TEXT("CONPASS\n"));



	hCloseThread = CreateThread(NULL, 0, CloseThread, estacao, 0, &idCloseThread);
	hPipeReadThread = CreateThread(NULL, 0, ReadNamedPipe, estacao, 0, &idPipeReadThread);
	hCommandsThread = CreateThread(NULL, 0, CommandsThread, estacao, 0, &idCommandsThread);


	if (hCloseThread == NULL || hCommandsThread == NULL || hPipeReadThread == NULL) {
		estacao->dll->log((TCHAR*)TEXT("Não foi possivel criar a Thread!"), TYPE::ERRO);
		return EXIT_FAILURE;
	}



	WaitForSingleObject(hCloseThread, INFINITE);
	_tprintf(TEXT("FECHOU...\n"));
	WaitForSingleObject(hPipeReadThread, INFINITE);
	WaitForSingleObject(hCommandsThread, INFINITE);

	CloseHandle(hCommandsThread);
	CloseHandle(hCloseThread);
	CloseHandle(hPipeReadThread);
	CloseHandle(hMutex);
	delete estacao;
	return EXIT_SUCCESS;
}

DWORD WINAPI CommandsThread(LPVOID lpParam) {
	Estacao* estacao = (Estacao*)lpParam;
	TCHAR command[COMMAND_SIZE] = TEXT("");
	TCHAR* pch = new TCHAR[COMMAND_SIZE];
	TCHAR* something = new TCHAR[COMMAND_SIZE];
	WaitableTimer * wt = new WaitableTimer(WAIT_ONE_SECOND * 5);

	while (!estacao->isExit())
	{
		_tprintf(TEXT("COMMAND: "));
		do {
			if (fgetws(command, sizeof(command), stdin) == NULL) {
				estacao->dll->log((TCHAR*)TEXT("Ocorreu um erro a ler o comando inserido!"), TYPE::ERRO);
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
			estacao->setExit(true);
			_tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
			wt->wait();
			delete wt;
			delete estacao;
			exit(EXIT_SUCCESS);
		}
		else if (_tcscmp(pch, TEXT("novo")) == 0) {
			int row, col;
			TCHAR* id = new TCHAR[PASSENGER_NAME_SIZE];

			pch = _tcstok_s(something, TEXT(" "), &something);
			if (pch != NULL) {
				_tcscpy_s(id, PASSENGER_NAME_SIZE, pch);
				pch = _tcstok_s(something, TEXT(" "), &something);
				if (pch != NULL) {
					try {
						row = stoi(pch);
					}
					catch (invalid_argument) {
						estacao->dll->log((TCHAR*)TEXT("Valor inválido!"), TYPE::WARNING);
						continue;
					}
					catch (out_of_range) {
						estacao->dll->log((TCHAR*)TEXT("Este valor não é muito grande?"), TYPE::WARNING);
						continue;
					}
					pch = _tcstok_s(something, TEXT(" "), &something);
					if (pch != NULL) {
						try {
							col = stoi(pch);
						}
						catch (invalid_argument) {
							estacao->dll->log((TCHAR*)TEXT("Valor inválido!"), TYPE::WARNING);
							continue;
						}
						catch (out_of_range) {
							estacao->dll->log((TCHAR*)TEXT("Este valor não é muito grande?"), TYPE::WARNING);
							continue;
						}
						Passageiro* client = new Passageiro(id, row, col);
						ADDPASSENGER* ADD = new ADDPASSENGER;
						ADD->client = client;
						ADD->estacao = estacao;
						DWORD idAddPassengerThread;
						HANDLE hThread = CreateThread(NULL, 0, SendPassengerThread, ADD, 0, &idAddPassengerThread);
						if (hThread == NULL) {
							estacao->dll->log((TCHAR*)TEXT("Não foi possivel criar a Thread!"), TYPE::ERRO);
							return EXIT_FAILURE;
						}
						estacao->addHandle(&hThread);
					}
					else {
						estacao->dll->log((TCHAR*)TEXT("O comando inserido não existe!\n ex: novo jose 1 2"), TYPE::WARNING);
					}
				}
				else {
					estacao->dll->log((TCHAR*)TEXT("O comando inserido não existe!\n ex: novo jose 1 2"), TYPE::WARNING);
				}
			}
			else {
				estacao->dll->log((TCHAR*)TEXT("O comando inserido não existe!"), TYPE::WARNING);
			}
		}
		else if (_tcscmp(pch, TEXT("transportar")) == 0) {
			int row, col;
			TCHAR* id = new TCHAR[PASSENGER_NAME_SIZE];
			pch = _tcstok_s(something, TEXT(" "), &something);
			if (pch != NULL) {
				_tcscpy_s(id, PASSENGER_NAME_SIZE, pch);
				pch = _tcstok_s(something, TEXT(" "), &something);
				if (pch != NULL) {
					try {
						row = stoi(pch);
					}
					catch (invalid_argument) {
						estacao->dll->log((TCHAR*)TEXT("Valor inválido!"), TYPE::WARNING);
						continue;
					}
					catch (out_of_range) {
						estacao->dll->log((TCHAR*)TEXT("Este valor não é muito grande?"), TYPE::WARNING);
						continue;
					}
					pch = _tcstok_s(something, TEXT(" "), &something);
					if (pch != NULL) {
						try {
							col = stoi(pch);
						}
						catch (invalid_argument) {
							estacao->dll->log((TCHAR*)TEXT("Valor inválido!"), TYPE::WARNING);
							continue;
						}
						catch (out_of_range) {
							estacao->dll->log((TCHAR*)TEXT("Este valor não é muito grande?"), TYPE::WARNING);
							continue;
						}
						Passageiro* client = estacao->addMove(id, row, col); // altera o status para transportar
						if (client != nullptr) {
							if (client->getStatus() == STATUS::TRANSPORTAR) {
								ADDPASSENGER* ADD = new ADDPASSENGER;
								ADD->client = client;
								ADD->estacao = estacao;
								DWORD idAddPassengerThread;
								HANDLE hThread = CreateThread(NULL, 0, SendPassengerThread, ADD, 0, &idAddPassengerThread);
								if (hThread == NULL) {
									estacao->dll->log((TCHAR*)TEXT("Não foi possivel criar a Thread!"), TYPE::ERRO);
									return EXIT_FAILURE;
								}
								estacao->addHandle(&hThread);
							}
							else {
								estacao->dll->log((TCHAR*)TEXT("Este passageiro não pode ser transportado!"), TYPE::NOTIFICATION);
							}
						}
						else {
							estacao->dll->log((TCHAR*)TEXT("Este passageiro não existe!"), TYPE::NOTIFICATION);
						}
					}
					else {
						estacao->dll->log((TCHAR*)TEXT("O comando inserido não existe!\n ex: transportar jose 1 2"), TYPE::WARNING);
					}
				}
				else {
					estacao->dll->log((TCHAR*)TEXT("O comando inserido não existe!\n ex: transportar jose 1 2"), TYPE::WARNING);
				}
			}
			else {
				estacao->dll->log((TCHAR*)TEXT("O comando inserido não existe!"), TYPE::WARNING);
			}
		}
		else {
			estacao->dll->log((TCHAR*)TEXT("O comando inserido não existe!"), TYPE::WARNING);
		}
	}
	return EXIT_SUCCESS;
}

DWORD WINAPI CloseThread(LPVOID lpParam){
	Estacao* estacao = (Estacao*)lpParam;
	HANDLE hEventClose;
	WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND);
	hEventClose = CreateEvent(NULL, TRUE, FALSE, EVENT_CLOSE_ALL);
	if (hEventClose == NULL) {
		estacao->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento!"), TYPE::ERRO);
	}
	estacao->dll->regist((TCHAR*)EVENT_CLOSE_ALL, 4);

	WaitForSingleObject(hEventClose, INFINITE);
	_tprintf(TEXT("\n[WARNING] A Central fechou!\n"));
	estacao->setExit(true);
	_tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
	wt->wait();
	
	CloseHandle(hEventClose);
	delete wt;
	_tprintf(TEXT("Vou fechar...\n"));
	exit(EXIT_SUCCESS);
}

DWORD WINAPI SendPassengerThread(LPVOID lpParam) {
	ADDPASSENGER * ADD = (ADDPASSENGER*)lpParam;
	Estacao* estacao = ADD->estacao;
	Passageiro* client = ADD->client;
	estacao->writeNamedPipe(client);
	return EXIT_SUCCESS;
}

DWORD WINAPI ReadNamedPipe(LPVOID lpParam) {
	Estacao* estacao = (Estacao*)lpParam;
	while (!estacao->isExit()) {
		PASSENGER p = estacao->readNamedPipe();
		Passageiro* client = estacao->updateClient(p);
		estacao->execStatus(client);
	}
	return EXIT_SUCCESS;
}

void Clear() {
#if defined _WIN32
	system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
	system("clear");
#elif defined (__APPLE__)
	system("clear");
#endif
}