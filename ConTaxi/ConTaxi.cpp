#include "ConTaxi.h"


int _tmain(int argc ,TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	DWORD idCommandsThread, idCloseThread, idGetCarDataThread, idGetMapThread, idMoveCarThread, idBufferCircular, idRespostaInterese;
	HANDLE hEventCanBoot,commandsThread, closeThread, getCarDataThread, getMapThread, moveCarThread, bufferCircularThread, respostaIntereseThread;

	Taxista* taxista = new Taxista();


	hEventCanBoot = CreateEvent(NULL, TRUE, FALSE, EVENT_BOOT_ALL);
	if (hEventCanBoot == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento para encerrar!"), TYPE::ERRO);
		return EXIT_FAILURE;
	}
	_tprintf(TEXT("A ESPERA...\n"));
	WaitForSingleObject(hEventCanBoot, INFINITE);
	CloseHandle(hEventCanBoot);
	Clear();
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

	respostaIntereseThread = CreateThread(NULL, 0, RespostaIntereseThread, taxista, 0, &idRespostaInterese);
	moveCarThread = CreateThread(NULL, 0, MoveCarThread, taxista, 0, &idMoveCarThread);
	bufferCircularThread = CreateThread(NULL, 0, BufferCircularThread, taxista, 0, &idBufferCircular);
	commandsThread = CreateThread(NULL, 0, CommandsThread, taxista, 0, &idCommandsThread);


	if (commandsThread == NULL  || closeThread == NULL || moveCarThread == NULL || bufferCircularThread == NULL) {
		taxista->dll->log((TCHAR*)TEXT("Não foi possivel criar a Thread!"), TYPE::ERRO);
		return EXIT_FAILURE;
	}
	
	WaitForSingleObject(commandsThread, INFINITE);
	WaitForSingleObject(moveCarThread, INFINITE);
	WaitForSingleObject(closeThread, INFINITE);
	WaitForSingleObject(bufferCircularThread, INFINITE);

	CloseHandle(getCarDataThread);
	CloseHandle(moveCarThread);
	CloseHandle(commandsThread);
	CloseHandle(closeThread);
	CloseHandle(bufferCircularThread);
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
	WaitableTimer* wt = new WaitableTimer((LONGLONG)(WAIT_ONE_SECOND * car->getSpeed()));
	

	while (!taxista->isExit()) {
		if (taxista->isRandomMove()) { // Random move
			oldRow = car->getRow();
			oldCol = car->getCol();
			wt->updateTime((LONGLONG)(WAIT_ONE_SECOND * car->getSpeed()));
			wt->wait();
			car->setPosition(nextMove->getRow(), nextMove->getCol());
			car->setStatus(STATUS_TAXI::ALEATORIO);
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
		}
		else {
			carNode = city->getNodeAt(car->getRow(), car->getCol());
			nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
		}
	}
	delete wt;
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
	SendCar(taxista);
	_tprintf(TEXT("[CAR] Iniciado na posição (%d,%d)\n"), taxista->car->getRow(), taxista->car->getCol());

	tstringstream msg;
	msg << "\\\\.\\pipe\\" << "pipe_" << taxista->car->getId() << endl;

	HANDLE hNamedPipe = CreateFile(msg.str().c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hNamedPipe == INVALID_HANDLE_VALUE) {
		taxista->dll->log((TCHAR*)TEXT("Erro a aceder ao named pipe!"), TYPE::ERRO);
		return EXIT_FAILURE;
	}
	taxista->car->setPipeHandle(hNamedPipe);

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
			taxista->car->setStatus(STATUS_TAXI::SAIR);
			SendCar(taxista);
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
					taxista->clearATransportar();
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

			if (!taxista->car->getAutopicker()) {
				taxista->transportar(pch);
			}
			else {
				taxista->dll->log((TCHAR*)TEXT("Para utilizar este comando tem de desativar o autopicker! (ex: autopicker off)"), TYPE::WARNING);
			}
		}
		else {
			taxista->dll->log((TCHAR*)TEXT("O comando inserido não existe!"), TYPE::WARNING);
		}
	}
	delete wt;
	return EXIT_SUCCESS;
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

DWORD WINAPI RespostaIntereseThread(LPVOID lpParam) {
	Taxista* taxista = (Taxista*)lpParam;
	BOOL readedNamedPipe;
	DWORD noBytesRead;
	TRANSPORT* transport = new TRANSPORT;
	ZeroMemory(transport, sizeof(TRANSPORT));
	WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 3);
	while (!taxista->isExit()) {
		readedNamedPipe = ReadFile(taxista->car->getPipeHandle(), (LPVOID)transport, sizeof(TRANSPORT), &noBytesRead, NULL);
		if (!readedNamedPipe) {
			taxista->dll->log((TCHAR*)TEXT("Não foi possivel ler do Named Pipe!"), TYPE::ERRO);
			return EXIT_FAILURE;
		}
		switch (transport->status){
		case STATUS_TRANSPORT::ACEITE:
			_tprintf(TEXT("\nFui ACEITE para o transporte do passageiro %s!\nCOMMAND: "), transport->client.id);
			taxista->car->setClient(transport->client.id);
			TransportClient(taxista, transport);
			taxista->setCanInterest(true);
			break;
		case STATUS_TRANSPORT::REJEITADO:
			_tprintf(TEXT("\nFui REJEITADO para o transporte do passageiro %s!\nCOMMAND: "), transport->client.id);
			taxista->setCanInterest(true);
			break;
		case STATUS_TRANSPORT::EXPULSO:
			_tprintf(TEXT("\nFui expulso do sistema :'C!\nA sair..."));
			taxista->setExit(true);
			wt->wait();
			delete wt;
			exit(EXIT_SUCCESS);
			break;
		default:
			break;
		}
	}
	delete wt;
	return EXIT_SUCCESS;
}

DWORD TransportClient(Taxista* taxista, TRANSPORT* transport) {
	taxista->disableRandomMove();
	srand((unsigned int)time(NULL));
	Car* car = taxista->car;
	TownMap* city = taxista->getMap();
	vector<Node*> nodes = city->getNodes();
	int oldRow = car->getRow();
	int oldCol = car->getCol();
	Node* carNode = city->getNodeAt(car->getRow(), car->getCol());
	Node* nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
	WaitableTimer* wt = new WaitableTimer((LONGLONG)(WAIT_ONE_SECOND * car->getSpeed()));
	_tprintf(TEXT("\nA viajar para o passageiro %s em <%d,%d>!\nCOMMAND: "), transport->client.id, transport->client.row, transport->client.col);
	int row = transport->client.row;
	int col = transport->client.col;
	for (int r = 0; r < 2; r++){
		if (r != 0) {
			_tprintf(TEXT("\nA viajar para o destino <%d,%d>!\nCOMMAND: "), transport->client.dest_row, transport->client.dest_col);
			row = transport->client.dest_row;
			col = transport->client.dest_col;
		}
		Node* src = taxista->getNodeAt(taxista->car->getRow(), taxista->car->getCol());
		Node* dest = taxista->getNodeAt(row, col);
		BreadthFirstSearch* bfs = new BreadthFirstSearch(taxista->getMap());
		_tprintf(TEXT("\nA configurar o gps!"));
		_tprintf(TEXT("\nCOMMAND: "));
		BESTPATH path = bfs->getBestPath(src, dest);
		INT i = 0;
		DOUBLE speed = car->getSpeed();
		car->setTimeToDestiny(((path.path.size() - 1 - i) * car->getSpeed()));
		do {
			i++;
			if (speed != car->getSpeed() || i == 1) {
				car->setTimeToDestiny(((path.path.size() - 1 - i) * car->getSpeed()));
				_tprintf(TEXT("\nTempo para o destino: %0.6g segundos\nCOMMAND:"), car->getTimeToDestiny());
			}
			speed = car->getSpeed();
			wt->updateTime((LONGLONG)(WAIT_ONE_SECOND * car->getSpeed()));
			wt->wait();
			car->setPosition(path.path[i]->getRow(), path.path[i]->getCol());
			if (i == 1 && r== 0) {
				taxista->car->setStatus(STATUS_TAXI::IRPASSAGEIRO);
			}
			else if (i == 1 && r == 1) {
				taxista->car->setStatus(STATUS_TAXI::IRDESTINO);
			}
			else {
				taxista->car->setStatus(STATUS_TAXI::NOCAMINHO);
			}
			SendCar(taxista);

		} while ((i < path.path.size() - 1) && (!taxista->getSmartPath()));
		wt->updateTime(WAIT_ONE_SECOND);
		wt->wait();
		_tprintf(TEXT("\nCheguei a posição desejada! <%d,%d>"), row, col);
		_tprintf(TEXT("\nCOMMAND: "));
		car->setPosition(dest->getRow(), dest->getCol());
	}
	
	taxista->car->setStatus(STATUS_TAXI::ENTREGUE);
	SendCar(taxista);
	taxista->car->clearClient();
	taxista->enableRandomMove();
	return EXIT_SUCCESS;
}

DWORD WINAPI BufferCircularThread(LPVOID lpParam) {
	Taxista* taxista = (Taxista*)lpParam;
	return bufferCircular(taxista);
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