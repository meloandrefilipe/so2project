#include "ConTaxi.h"


int _tmain(int argc ,TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	DWORD idCommandsThread, idCommunicationThread, idCloseThread, idGetCarDataThread, idGetMapThread, idMoveCarThread;
	HANDLE hEventCanBoot,commandsThread, communicationThread, closeThread, getCarDataThread, getMapThread, moveCarThread;

	Taxista* taxista = new Taxista();


	hEventCanBoot = CreateEvent(NULL, TRUE, FALSE, EVENT_BOOT_ALL);
	if (hEventCanBoot == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento para encerrar!"), TYPE::ERRO);
		return EXIT_FAILURE;
	}
	_tprintf(TEXT("A ESPERA...\n"));
	WaitForSingleObject(hEventCanBoot, INFINITE);
	CloseHandle(hEventCanBoot);
	_tprintf(TEXT("ConTaxi\n"));
	
	closeThread = CreateThread(NULL, 0, CloseThread, taxista, 0, &idCloseThread);
	getMapThread = CreateThread(NULL, 0, GetMapThread, taxista, 0, &idGetMapThread);
	WaitForSingleObject(getMapThread, INFINITE);

	if (!taxista->getCanRegist()) {
		WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 5);
		_tprintf(TEXT("A central está fechada, volte mais tarde!\n"));
		_tprintf(TEXT("A fechar...!\n"));
		wt->wait();
		delete wt;
		CloseHandle(closeThread);
		CloseHandle(getMapThread);
		delete taxista;
		exit(EXIT_SUCCESS);
	}
	getCarDataThread = CreateThread(NULL, 0, GetCarDataThread, taxista, 0, &idGetCarDataThread);

	WaitForSingleObject(getCarDataThread, INFINITE);

	moveCarThread = CreateThread(NULL, 0, MoveCarThread, taxista, 0, &idMoveCarThread);
	commandsThread = CreateThread(NULL, 0, CommandsThread, taxista, 0, &idCommandsThread);
	communicationThread = CreateThread(NULL, 0, CommunicationThread, taxista, 0, &idCommunicationThread);


	if (commandsThread == NULL || communicationThread == NULL || closeThread == NULL || moveCarThread == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel criar a Thread!"), TYPE::ERRO);
		return EXIT_FAILURE;
	}
	
	WaitForSingleObject(commandsThread, INFINITE);
	WaitForSingleObject(moveCarThread, INFINITE);
	WaitForSingleObject(communicationThread, INFINITE);
	WaitForSingleObject(closeThread, INFINITE);

	CloseHandle(getCarDataThread);
	CloseHandle(moveCarThread);
	CloseHandle(commandsThread);
	CloseHandle(closeThread);
	CloseHandle(communicationThread);
	delete taxista;
	return EXIT_SUCCESS;
}

DWORD WINAPI MoveCarThread(LPVOID lpParam) {
	srand((unsigned int)time(NULL));
	Taxista* taxista = (Taxista*)lpParam;
	Car* car = taxista->car;
	TownMap* city = taxista->getMap();
	vector<Node*> nodes = city->getNodes();
	int oldRow = car->getRow();
	int oldCol = car->getCol();
	Node* carNode = city->getNodeAt(car->getRow(), car->getCol());
	Node* nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
	WaitableTimer* wt = new WaitableTimer(car->getSpeed());
	HANDLE hMutex;

	hMutex = CreateMutex(NULL, FALSE, NULL);
	if (hMutex == NULL){
		CloseHandle(hMutex);
		taxista->dll->log(((TCHAR*)TEXT("Contaxi Move CreateMutex")), TYPE::ERRO);
		return EXIT_FAILURE;
	}

	while (!taxista->isExit()) {
		if (taxista->isRandomMove()) { // Random move
			WaitForSingleObject(hMutex, INFINITE);
			oldRow = car->getRow();
			oldCol = car->getCol();
			wt->updateTime(car->getSpeed());
			wt->wait();
			car->setPosition(nextMove->getRow(), nextMove->getCol());
			SendCar(taxista);
			carNode = city->getNodeAt(car->getRow(), car->getCol());
			if (carNode->getNeighbours().size() > 2) {
				do {
					nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
				} while (nextMove->getCol() == oldCol && nextMove->getRow() == oldRow);
			}
			else {
				if (car->getCol() == oldCol && car->getRow() > oldRow) { //Moveu para a casa de baixo
					do {
						if (car->getRow() + 1 < taxista->getMap()->getRows()) { // verifico se é posição do mapa
							nextMove = city->getNodeAt(car->getRow() + 1, car->getCol()); // apanho a posição
							if (!nextMove->isRoad()) { // verifico se é estrada
								nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()]; // seleciono uma proxima posição
								if (carNode->getNeighbours().size() == 1) { // verifico se a posição nao é a unica (no caso de ser um beco sem saida) para poder sair do while
									break;
								}
							}
						}
						else {
							nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()]; //obtenho nova posição caso a posição anterior exceda o mapa
							if (carNode->getNeighbours().size() == 1) { // sai do while se for um beco sem saida
								break;
							}
						}
					} while (nextMove->getCol() == carNode->getCol() && nextMove->getRow() == carNode->getRow()); // precorro o codigo enquando a nova posição nao for diferente da velha
				}
				else if (car->getCol() == oldCol && car->getRow() < oldRow) { // moveu para a casa de cima
					do {
						if (car->getRow() - 1 > 0) {
							nextMove = city->getNodeAt(car->getRow() - 1, car->getCol());
							if (!nextMove->isRoad()) {
								nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
								if (carNode->getNeighbours().size() == 1) {
									break;
								}
							}
						}
						else {
							nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
							if (carNode->getNeighbours().size() == 1) {
								break;
							}
						}
					} while (nextMove->getCol() == carNode->getCol() && nextMove->getRow() == carNode->getRow());
				}
				else if (car->getCol() > oldCol && car->getRow() == oldRow) { // moveu para a direita
					do {
						if (car->getCol() + 1 < taxista->getMap()->getCols()) {
							nextMove = city->getNodeAt(car->getRow(), car->getCol() + 1);
							if (!nextMove->isRoad()) {
								nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
								if (carNode->getNeighbours().size() == 1) {
									break;
								}
							}
						}
						else {
							nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
							if (carNode->getNeighbours().size() == 1) {
								break;
							}
						}
					} while (nextMove->getCol() == carNode->getCol() && nextMove->getRow() == carNode->getRow());
				}
				else  if (car->getCol() < oldCol && car->getRow() == oldRow) { //moveu para a esquerda
					do {
						if (car->getCol() - 1 > 0) {
							nextMove = city->getNodeAt(car->getRow(), car->getCol() - 1);
							if (!nextMove->isRoad()) {
								nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
								if (carNode->getNeighbours().size() == 1) {
									break;
								}
							}
						}
						else {
							nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
							if (carNode->getNeighbours().size() == 1) {
								break;
							}
						}
					} while (nextMove->getCol() == carNode->getCol() && nextMove->getRow() == carNode->getRow());
				}
				else { // esta parado na mesma posição que antes
					nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
				}
			}
			ReleaseMutex(hMutex);
		}
		else { // SMART MOVE
			WaitForSingleObject(hMutex, INFINITE);
			do {
				if (taxista->getSmartPath()) {
					taxista->resetSmartPath();
				}
				int row = taxista->move.dest_row;
				int col = taxista->move.dest_col;
				Node* src = taxista->getNodeAt(taxista->car->getRow(), taxista->car->getCol());
				Node* dest = taxista->getNodeAt(row, col);
				BreadthFirstSearch* bfs = new BreadthFirstSearch(taxista->getMap());
				_tprintf(TEXT("\nA calcular caminho até <%d,%d>..."), row, col);
				_tprintf(TEXT("\nCOMMAND: "));
				BESTPATH path = bfs->getBestPath(src, dest);
				INT i = 0;
				LONGLONG speed = car->getSpeed();
				_tprintf(TEXT("\nA ir até <%d,%d>..."), row, col);
				_tprintf(TEXT("\nCOMMAND: "));
				do {
					i++;
					if (speed != car->getSpeed() || i == 1) {
						//_tprintf(TEXT("Tempo para o destino: %f\n"), (FLOAT)((path.path.size() - 1 - i) * (LONG)car->getSpeed()) / 10000000.f);
					}
					speed = car->getSpeed();
					wt->updateTime(speed);
					wt->wait();
					car->setPosition(path.path[i]->getRow(), path.path[i]->getCol());
					SendCar(taxista);

				} while ((i < path.path.size() -1) && (!taxista->getSmartPath()));
				wt->updateTime(WAIT_ONE_SECOND);
				wt->wait();
				_tprintf(TEXT("\nCheguei a posição desejada! <%d,%d>"), row, col);
				_tprintf(TEXT("\nCOMMAND: "));
				taxista->enableRandomMove();
				carNode = dest;
				nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
			} while (taxista->getSmartPath());
			ReleaseMutex(hMutex);
		}
	}
	delete wt;
	CloseHandle(hMutex);
	return EXIT_SUCCESS;
}

DWORD WINAPI GetMapThread(LPVOID lpParam) {
	Taxista* taxista = (Taxista*)lpParam;
	return getMap(taxista);
}

DWORD WINAPI GetCarDataThread(LPVOID lpParam) {
	Taxista* taxista = (Taxista*)lpParam;
	BOOL exists = false;
	int row = 0; 
	int col = 0;
	TCHAR plate[TAXI_PLATE_SIZE] = TEXT("XX XX XX");

	_tprintf(TEXT("\nIndique a matrícula do seu veículo: "));
	
	do {
		if (exists) {
			_tprintf(TEXT("Esta matricula já se encontra no nosso sistema!\n Coloque outra ex:(XX-XX-XX): "));
		}
		if (fgetws(plate, sizeof(plate), stdin) == NULL) {
			_tprintf(TEXT("[ERRO] Ocorreu um erro a ler a matrícula\n[CODE] %d\n"), GetLastError());
		}
		for (int i = 0; i < sizeof(plate) && plate[i]; i++)
		{
			if (plate[i] == '\n')
				plate[i] = '\0';
		}
		exists = validatePlate(plate);
	} while (exists);
	Node* position = taxista->getRandomRoad();
	taxista->car = new Car(GetCurrentProcessId(), position->getRow(), position->getCol(), plate);
	_tprintf(TEXT("[CAR] Iniciado na posição (%d,%d)\n"), taxista->car->getRow(), taxista->car->getCol());
	return EXIT_SUCCESS;
}

DWORD WINAPI CommandsThread(LPVOID lpParam) {
	TCHAR command[COMMAND_SIZE] = TEXT("");
	TCHAR* pch = new TCHAR[COMMAND_SIZE];
	TCHAR* something = new TCHAR[COMMAND_SIZE];
	Taxista* taxista = (Taxista*)lpParam;
	WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 5);
	while (!taxista->isExit()) {
		_tprintf(TEXT("COMMAND: "));
		do {
			if (fgetws(command, sizeof(command), stdin) == NULL) {
				taxista->dll->log((TCHAR*)TEXT("Ocorreu um erro a ler o comando inserido!"), TYPE::ERRO);
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
			taxista->setExit(true);
			_tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
			wt->wait();
			delete wt;
			delete taxista;
			exit(EXIT_SUCCESS);
		}
		else if (_tcscmp(pch, TEXT("acelerar")) == 0) {
			taxista->car->speedUp();
		}
		else if (_tcscmp(pch, TEXT("desacelerar")) == 0) {
			taxista->car->speedDown();
		}
		else if (_tcscmp(pch, TEXT("nq")) == 0) {

			pch = _tcstok_s(something, TEXT(" "), &something);

			int val = NQ;
			if (pch != NULL) {
				try {
					val = stoi(pch);
				}
				catch (invalid_argument) {
					taxista->dll->log((TCHAR*)TEXT("Valor inválido!"), TYPE::WARNING);
					continue;
				}
				catch (out_of_range) {
					taxista->dll->log((TCHAR*)TEXT("Este valor não é muito grande?"), TYPE::WARNING);
					continue;
				}
				taxista->car->setNq(val);
			}
			else {
				taxista->dll->log((TCHAR*)TEXT("O comando inserido não existe!"), TYPE::WARNING);
			}
		}
		else if (_tcscmp(pch, TEXT("autopicker")) == 0) {

			pch = _tcstok_s(something, TEXT(" "), &something);
			if (pch != NULL) {
				if (_tcscmp(pch, TEXT("on")) == 0) {
					taxista->dll->log((TCHAR*)TEXT("[AUTOPICKER] Ativado!"), TYPE::NOTIFICATION);
					taxista->car->setAutopicker(true);

				}
				else if (_tcscmp(pch, TEXT("off")) == 0) {
					taxista->dll->log((TCHAR*)TEXT("[AUTOPICKER] Desativado!"), TYPE::NOTIFICATION);
					taxista->car->setAutopicker(false);
				}
				else {
					taxista->dll->log((TCHAR*)TEXT("A Opção inserida não existe!"), TYPE::WARNING);
					continue;
				}
			}
			else {
				taxista->dll->log((TCHAR*)TEXT("O comando inserido não existe!"), TYPE::WARNING);
			}
		}
		else if (_tcscmp(pch, TEXT("transportar")) == 0) {

			pch = _tcstok_s(something, TEXT(" "), &something);

			int traveler = 0;
			if (pch != NULL) {
				try {
					traveler = stoi(pch);
				}
				catch (invalid_argument) {
					taxista->dll->log((TCHAR*)TEXT("Valor inválido!"), TYPE::WARNING);
					break;
				}
				catch (out_of_range) {
					taxista->dll->log((TCHAR*)TEXT("Este valor não é muito grande?"), TYPE::WARNING);
					break;
				}
				taxista->dll->log((TCHAR*)(TEXT("Vou até ao passageiro #%s"), pch), TYPE::NOTIFICATION);
			}
			else {
				taxista->dll->log((TCHAR*)TEXT("O comando inserido não existe!"), TYPE::WARNING);
			}

			// IMPLEMENTAR CODIGO DE TRANSPORTE DO PASSAGEIRO
		}
		else if (_tcscmp(pch, TEXT("goto")) == 0) {
			pch = _tcstok_s(something, TEXT(" "), &something);
			int row;
			int col;
			if (pch != NULL) {
				try {
					row = stoi(pch);
				}
				catch (invalid_argument) {
					taxista->dll->log((TCHAR*)TEXT("Valor inválido!"), TYPE::WARNING);
					continue;
				}
				catch (out_of_range) {
					taxista->dll->log((TCHAR*)TEXT("Este valor não é muito grande?"), TYPE::WARNING);
					continue;
				}
				pch = _tcstok_s(something, TEXT(" "), &something);
				if (pch != NULL) {
					try {
						col = stoi(pch);
					}
					catch (invalid_argument) {
						taxista->dll->log((TCHAR*)TEXT("Valor inválido!"), TYPE::WARNING);
						continue;
					}
					catch (out_of_range) {
						taxista->dll->log((TCHAR*)TEXT("Este valor não é muito grande?"), TYPE::WARNING);
						continue;
					}
					if (row > taxista->getMap()->getRows() || row < 0 || col > taxista->getMap()->getCols() || col < 0) {
						taxista->dll->log((TCHAR*)TEXT("As coordenadas inseridas não pertencem ao mapa!"), TYPE::WARNING);
					}
					else if (!taxista->getNodeAt(row, col)->isRoad()) {
						taxista->dll->log((TCHAR*)TEXT("As coordenadas inseridas não são uma estrada, isto é um taxi não um 4x4!"), TYPE::WARNING);
					}
					else {
						taxista->move.dest_row = row;
						taxista->move.dest_col = col;
						if (taxista->isRandomMove()) {
							taxista->disableRandomMove();
						}
						else {
							taxista->resetSmartPath();
						}
					}
				}
			}
			if (pch == NULL) {
				taxista->dll->log((TCHAR*)TEXT("O comando inserido não existe!"), TYPE::WARNING);
			}
		}
		else {
			taxista->dll->log((TCHAR*)TEXT("O comando inserido não existe!"), TYPE::WARNING);
		}
	}
	delete wt;
	return EXIT_SUCCESS;
}

DWORD WINAPI CommunicationThread(LPVOID lpParam) {
	Taxista* taxista = (Taxista*)lpParam;
	return SendCar(taxista);
}

DWORD WINAPI CloseThread(LPVOID lpParam) {

	Taxista* taxista = (Taxista*)lpParam;
	HANDLE hEventClose;
	WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 5);
	hEventClose = CreateEvent(NULL, TRUE, FALSE, EVENT_CLOSE_ALL);
	if (hEventClose == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento!"), TYPE::ERRO);
	}
	taxista->dll->regist((TCHAR*)EVENT_CLOSE_ALL, 4);

	WaitForSingleObject(hEventClose, INFINITE);
	_tprintf(TEXT("\n[WARNING] A Central fechou!\n"));
	taxista->setExit(true);
	_tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
	CloseHandle(hEventClose);
	wt->wait();
	delete wt;
	delete taxista;
	exit(EXIT_SUCCESS);
}