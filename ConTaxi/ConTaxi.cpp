#include "ConTaxi.h"


int _tmain(int argc ,TCHAR* argv[]) {

#ifdef UNICODE
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif
	DWORD idCommandsThread, idCommunicationThread, idCloseThread, idGetCarDataThread, idGetMapThread, idMoveCarThread;
	HANDLE commandsThread, communicationThread, closeThread, getCarDataThread, getMapThread, moveCarThread;

	Taxista* taxista = new Taxista();
	DLLProfessores* dll = new DLLProfessores();

	WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND);
	closeThread = CreateThread(NULL, 0, CloseThread, taxista, 0, &idCloseThread);
	getMapThread = CreateThread(NULL, 0, GetMapThread, taxista, 0, &idGetMapThread);
	WaitForSingleObject(getMapThread, INFINITE);
	delete wt;
	getCarDataThread = CreateThread(NULL, 0, GetCarDataThread, taxista, 0, &idGetCarDataThread);
	WaitForSingleObject(getCarDataThread, INFINITE);




	moveCarThread = CreateThread(NULL, 0, MoveCarThread, taxista, 0, &idMoveCarThread);
	commandsThread = CreateThread(NULL, 0, CommandsThread, taxista, 0, &idCommandsThread);
	communicationThread = CreateThread(NULL, 0, CommunicationThread, taxista, 0, &idCommunicationThread);


	if (commandsThread == NULL || communicationThread == NULL || closeThread == NULL || moveCarThread == NULL) {
		dll->log((TCHAR*)TEXT("N�o foi possivel criar a Thread!"), TYPE::ERRO);
		delete dll;
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

	delete dll;
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

	while (!taxista->isExit()) {
		WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * (LONGLONG)car->getSpeed());
		car->setPosition(nextMove->getRow(), nextMove->getCol());
		SendCar(taxista);
		carNode = city->getNodeAt(car->getRow(), car->getCol());
		if (carNode->getNeighbours().size() > 2) {
			nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
		}
		else {
			if (car->getCol() == oldCol && car->getRow() > oldRow) { //Moveu para a casa de baixo
				do {
					if (car->getRow() + 1 < taxista->getMap()->getRows()) { // verifico se � posi��o do mapa
						nextMove = city->getNodeAt(car->getRow() + 1, car->getCol()); // apanho a posi��o
						if (!nextMove->isRoad()) { // verifico se � estrada
							nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()]; // seleciono uma proxima posi��o
							if (carNode->getNeighbours().size() == 1) { // verifico se a posi��o nao � a unica (no caso de ser um beco sem saida) para poder sair do while
								break;
							}
						}
					}
					else {
						nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()]; //obtenho nova posi��o caso a posi��o anterior exceda o mapa
						if (carNode->getNeighbours().size() == 1) { // sai do while se for um beco sem saida
							break;
						}
					}
				} while (nextMove->getCol() == oldCol && nextMove->getRow() == oldRow); // precorro o codigo enquando a nova posi��o nao for diferente da velha
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
				} while (nextMove->getCol() == oldCol && nextMove->getRow() == oldRow);
			}
			else if (car->getCol() > oldCol && car->getRow() == oldRow) { // moveu para a direita
				do {
					if(car->getCol() + 1 < taxista->getMap()->getCols()){
						nextMove = city->getNodeAt(car->getRow(), car->getCol() + 1 );
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
				} while (nextMove->getCol() == oldCol && nextMove->getRow() == oldRow);
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
				} while (nextMove->getCol() == oldCol && nextMove->getRow() == oldRow);
			}
			else { // esta parado na mesma posi��o que antes
				nextMove = carNode->getNeighbours()[(int)rand() % carNode->getNeighbours().size()];
			}
		}
		oldRow = car->getRow();
		oldCol = car->getCol();
	}	
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

	_tprintf(TEXT("\nIndique a matr�cula do seu ve�culo: "));
	
	do {
		if (exists) {
			_tprintf(TEXT("Esta matricula j� se encontra no nosso sistema!\n Coloque outra ex:(XX-XX-XX): "));
		}
		if (fgetws(plate, sizeof(plate), stdin) == NULL) {
			_tprintf(TEXT("[ERRO] Ocorreu um erro a ler a matr�cula\n[CODE] %d\n"), GetLastError());
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
	_tprintf(TEXT("[CAR] Iniciado na posi��o (%d,%d)\n"), taxista->car->getRow(), taxista->car->getCol());
	return EXIT_SUCCESS;
}

DWORD WINAPI CommandsThread(LPVOID lpParam) {
	TCHAR command[COMMAND_SIZE] = TEXT("");
	TCHAR* pch;
	TCHAR* something;
	Taxista* taxista = (Taxista*)lpParam;
	DLLProfessores* dll = new DLLProfessores();

	while (!taxista->isExit()) {
		_tprintf(TEXT("COMMAND: "));
		if (fgetws(command, sizeof(command), stdin) == NULL) {
			dll->log((TCHAR*)TEXT("Ocorreu um erro a ler o comando inserido!"), TYPE::ERRO);
		}
		for (int i = 0; i < sizeof(command) && command[i]; i++)
		{
			if (command[i] == '\n')
				command[i] = '\0';
		}
		pch = _tcstok_s(command, TEXT(" "), &something);
		if (_tcscmp(pch, TEXT("exit")) == 0) {
			delete dll;
			taxista->setExit(true);
			_tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
			WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 5);
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

			try {
				val = stoi(pch);
			}
			catch (invalid_argument) {
				dll->log((TCHAR*)TEXT("Valor inv�lido!"), TYPE::WARNING);
				continue;
			}
			catch (out_of_range) {
				dll->log((TCHAR*)TEXT("Este valor n�o � muito grande?"), TYPE::WARNING);
				continue;
			}
			taxista->car->setNq(val);
		}
		else if (_tcscmp(pch, TEXT("autopicker")) == 0) {

			pch = _tcstok_s(something, TEXT(" "), &something);

			if (_tcscmp(pch, TEXT("on")) == 0) {
				dll->log((TCHAR*)TEXT("[AUTOPICKER] Ativado!"), TYPE::WARNING);
				taxista->car->setAutopicker(true);

			}
			else if (_tcscmp(pch, TEXT("off")) == 0) {
				dll->log((TCHAR*)TEXT("[AUTOPICKER] Desativado!"), TYPE::WARNING);
				taxista->car->setAutopicker(false);
			}
			else {
				dll->log((TCHAR*)TEXT("A Op��o inserida n�o existe!"), TYPE::WARNING);
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
				dll->log((TCHAR*)TEXT("Valor inv�lido!"), TYPE::WARNING);
				break;
			}
			catch (out_of_range) {
				dll->log((TCHAR*)TEXT("Este valor n�o � muito grande?"), TYPE::WARNING);
				break;
			}
			dll->log((TCHAR*)(TEXT("Vou at� ao passageiro #%s"), pch), TYPE::NOTIFICATION);

			// IMPLEMENTAR CODIGO DE TRANSPORTE DO PASSAGEIRO
		}
		else {
			dll->log((TCHAR*)TEXT("O comando inserido n�o existe!"), TYPE::WARNING);
		}
	}
	delete dll;
	return EXIT_SUCCESS;
}

DWORD WINAPI CommunicationThread(LPVOID lpParam) {
	Taxista* taxista = (Taxista*)lpParam;
	return SendCar(taxista);
}

DWORD WINAPI CloseThread(LPVOID lpParam) {

	Taxista* taxista = (Taxista*)lpParam;
	HANDLE hEventClose;
	DLLProfessores* dll = new DLLProfessores();

	hEventClose = CreateEvent(NULL, TRUE, FALSE, EVENT_CLOSE_ALL);
	if (hEventClose == NULL) {
		dll->log((TCHAR*)TEXT("N�o foi possivel criar o evento!"), TYPE::ERRO);
		delete dll;
	}
	dll->regist((TCHAR*)EVENT_CLOSE_ALL, 4);

	WaitForSingleObject(hEventClose, INFINITE);
	_tprintf(TEXT("\n[WARNING] A Central fechou!\n"));
	taxista->setExit(true);
	_tprintf(TEXT("[SHUTDOWN] A Sair...\n"));
	delete dll;
	CloseHandle(hEventClose);
	WaitableTimer* wt = new WaitableTimer(WAIT_ONE_SECOND * 5);



	exit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}