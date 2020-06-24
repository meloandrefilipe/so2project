#include "Central.h"

Central::Central()
{
	this->townMap = new TownMap();
	this->dll = new DLLProfessores();
	this->pipesOn = false;
	this->exit = false;
	this->textCleanMap = nullptr;
	this->textFilledMap = nullptr;
	this->sizeMap = 0;
	this->takingIn = true;
	this->waitTime = DEFAULT_WAIT_TIME;
	this->bufferCircular = new BufferCircular();
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	this->hConPassNPRead = nullptr;
	this->hConPassNPWrite = nullptr;
	if (this->hMutex == NULL)
	{
		_tprintf(TEXT("Central CreateMutex error: %d\n"), GetLastError());
		return;
	}

	this->buildCleanMap(); // mutex no interior
}

Central::~Central()
{
	delete this->dll;
	delete this->townMap;
	delete this->bufferCircular;
	delete this->textCleanMap;
	delete this->textFilledMap;
	CloseHandle(this->hMutex);
	vector<Car*> cars = this->getCars();
	for (int i = 0; i < cars.size() ; i++)
	{
		DisconnectNamedPipe(cars[i]->getPipeHandle());
		CloseHandle(cars[i]->getPipeHandle());
	}
	DisconnectNamedPipe(this->hConPassNPRead);
	DisconnectNamedPipe(this->hConPassNPWrite);
	CloseHandle(this->hConPassNPRead);
	CloseHandle(this->hConPassNPWrite);
}

int Central::getSizeCars() const
{
	return (int)this->cars.size();
}

int Central::getSizeMap() const
{
	return this->sizeMap;
}

int Central::getWaitTime() const
{
	return this->waitTime;
}

BOOL Central::isPipesOn()
{
	return this->pipesOn;
}

void Central::setPipesOn(BOOL val)
{
	this->pipesOn = val;
}

DWORD Central::expulsar(Car* car) {
	WaitForSingleObject(this->hMutex, INFINITE);
	DWORD noBytesWrite;
	TRANSPORT* transport = new TRANSPORT;
	ZeroMemory(&transport->client, sizeof(PASSENGER));
	transport->status = STATUS_TRANSPORT::EXPULSO;
	BOOL writedNamedPipe = WriteFile(car->getPipeHandle(), (LPCVOID)transport, sizeof(TRANSPORT), &noBytesWrite, NULL);
	if (!writedNamedPipe) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel escrever no Named Pipe do taxi!"), TYPE::ERRO);
		ReleaseMutex(this->hMutex);
		return EXIT_FAILURE;
	}

	BOOL fluchedFileBuffer = FlushFileBuffers(car->getPipeHandle());
	if (!fluchedFileBuffer) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel submeter no Named Pipe do taxi!"), TYPE::ERRO);
		ReleaseMutex(this->hMutex);
		return EXIT_FAILURE;
	}
	ReleaseMutex(this->hMutex);
	return EXIT_SUCCESS;
}

DWORD Central::connectConpass()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	// Create named pipes
	this->hConPassNPRead = CreateNamedPipe(NAMED_PIPE_CONPASS_A, PIPE_ACCESS_INBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, sizeof(PASSENGER), sizeof(PASSENGER), 0, NULL);
	this->hConPassNPWrite = CreateNamedPipe(NAMED_PIPE_CONPASS_B, PIPE_ACCESS_OUTBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, sizeof(PASSENGER), sizeof(PASSENGER), 0, NULL);
	if (this->hConPassNPRead == INVALID_HANDLE_VALUE || this->hConPassNPWrite == INVALID_HANDLE_VALUE) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel criar o Named Pipe Conpass!"), TYPE::ERRO);
		ReleaseMutex(this->hMutex);
		return EXIT_FAILURE;
	}

	HANDLE hConnectA = CreateEvent(NULL, TRUE, FALSE, EVENT_CONPASS_A);
	HANDLE hConnectB = CreateEvent(NULL, TRUE, FALSE, EVENT_CONPASS_B);
	if (hConnectA == NULL || hConnectB == NULL) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento de espera pelo pipe conpass!"), TYPE::ERRO);
		ReleaseMutex(this->hMutex);
		return EXIT_FAILURE;
	}
	SetEvent(hConnectA);
	//connect named pipes
	BOOL connectNamedPipeRead = ConnectNamedPipe(this->hConPassNPRead, NULL);
	if (!connectNamedPipeRead) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel connectar ao Named Pipe Read Conpass!"), TYPE::ERRO);
		ReleaseMutex(this->hMutex);
		return EXIT_FAILURE;
	}
	SetEvent(hConnectB);
	BOOL connectNamedPipeWrite = ConnectNamedPipe(this->hConPassNPWrite, NULL);
	if (!connectNamedPipeWrite) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel connectar ao Named Pipe Write Conpass!"), TYPE::ERRO);
		ReleaseMutex(this->hMutex);
		return EXIT_FAILURE;
	}

	CloseHandle(hConnectA);
	CloseHandle(hConnectB);
	this->setPipesOn(true);
	ReleaseMutex(this->hMutex);
	return EXIT_SUCCESS;
}

DWORD Central::closeConpass()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->setPipesOn(false);
	DisconnectNamedPipe(this->hConPassNPRead);
	DisconnectNamedPipe(this->hConPassNPWrite);
	CloseHandle(this->hConPassNPRead);
	CloseHandle(this->hConPassNPWrite);
	this->hConPassNPRead = nullptr;
	this->hConPassNPWrite = nullptr;
	ReleaseMutex(this->hMutex);
	return EXIT_SUCCESS;
}

DWORD Central::sendAnswer(Car * car, Passageiro* client)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	DWORD noBytesWrite;
	TRANSPORT* transport = new TRANSPORT;
	transport->client = client->getStruct();
	vector<Car*> cars = client->getInterested();
	for (int i = 0; i < cars.size(); i++){
		if (_tcscmp(cars[i]->getPlate(), car->getPlate()) == 0) {
			transport->status = STATUS_TRANSPORT::ACEITE;
		}
		else {
			transport->status = STATUS_TRANSPORT::REJEITADO;
		}
		BOOL writedNamedPipe = WriteFile(cars[i]->getPipeHandle(), (LPCVOID)transport, sizeof(TRANSPORT), &noBytesWrite, NULL);
		if (!writedNamedPipe) {
			this->dll->log((TCHAR*)TEXT("Não foi possivel escrever no Named Pipe do taxi!"), TYPE::ERRO);
			ReleaseMutex(this->hMutex);
			return EXIT_FAILURE;
		}

		BOOL fluchedFileBuffer = FlushFileBuffers(cars[i]->getPipeHandle());
		if (!fluchedFileBuffer) {
			this->dll->log((TCHAR*)TEXT("Não foi possivel submeter no Named Pipe do taxi!"), TYPE::ERRO);
			ReleaseMutex(this->hMutex);
			return EXIT_FAILURE;
		}
	}
	ReleaseMutex(this->hMutex);
	return EXIT_SUCCESS;
}

void Central::setWaitTime(int time)
{
	this->waitTime = time;
}

BOOL Central::addCar(Car* car)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	// Create named pipe
	tstringstream msg;
	msg << "\\\\.\\pipe\\"  << "pipe_" << car->getId() << endl;


	HANDLE hNamedPipe = CreateNamedPipe(msg.str().c_str(), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1, sizeof(TRANSPORT), sizeof(TRANSPORT), 0, NULL);

	if (hNamedPipe == INVALID_HANDLE_VALUE) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel criar o Named Pipe do carro!"), TYPE::ERRO);
		ReleaseMutex(this->hMutex);
		return FALSE;
	}
	BOOL connectNamedPipe = ConnectNamedPipe(hNamedPipe, NULL);
	if (!connectNamedPipe) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel connectar ao Named Pipe do carro!"), TYPE::ERRO);
		ReleaseMutex(this->hMutex);
		return FALSE;
	}
	car->setPipeHandle(hNamedPipe);
	this->cars.push_back(car);
	ReleaseMutex(this->hMutex);
	return TRUE;
}

void Central::setExit(BOOL exit)
{
	this->exit = exit;
}

void Central::setTakingIn(BOOL val)
{
	this->takingIn = val;
}

BOOL Central::isExit()
{
	return this->exit;
}

BOOL Central::isTakingIn()
{
	return this->takingIn;
}

BOOL Central::carExists(TAXI* taxi)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	BOOL exists = false;
	for (int i = 0; i < cars.size(); i++)
	{
		if (cars[i]->isSamePlate(taxi->plate)) {
			exists = true;
			break;
		}
	}
	ReleaseMutex(this->hMutex);
	return exists;
}

vector<Car*> Central::getCars()
{
	return this->cars;
}

Car* Central::getCar(TCHAR* matricula)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	for (int i = 0; i < cars.size(); i++)
	{
		if (_tcscmp(cars[i]->getPlate(), matricula) == 0) {
			ReleaseMutex(this->hMutex);
			return cars[i];
		}
	}
	ReleaseMutex(this->hMutex);
	return nullptr;
}

Passageiro* Central::getClient(TCHAR* id)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	for ( int i = 0; i < clients.size(); i++)
	{
		if (_tcscmp(clients[i]->getId(), id) == 0) {
			ReleaseMutex(this->hMutex);
			return clients[i];
		}
	}
	ReleaseMutex(this->hMutex);
	return nullptr;
}

void Central::buildFilledMap()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	int cols = this->townMap->getCols();
	vector<Node*> nodes = this->townMap->getNodes();
	vector<Car*> cars = this->getCars();
	tstringstream map;

	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodes[i]->isRoad()) {
			BOOL hasCar = false;
			for (int c = 0; c < cars.size(); c++)
			{
				if (cars[c]->getCol() == nodes[i]->getCol() && cars[c]->getRow() == nodes[i]->getRow()) {
					hasCar = true;
					break;
				}
			}
			BOOL hasPerson = false;
			for (int c = 0; c < clients.size(); c++)
			{
				if (clients[c]->getCol() == nodes[i]->getCol() && clients[c]->getRow() == nodes[i]->getRow()) {
					hasPerson = true;
					break;
				}
			}

			if (hasCar) {
				map << "C";
			}
			else if (hasPerson) {
				map << "P";
			}
			else {
				map << "_";
			}
		}
		else {
			map << "X";
		}
		if (nodes[i]->getCol() == cols - 1) {
			map << endl;
		}
	}

	this->textFilledMap = new TCHAR[this->sizeMap];
	_tcscpy_s(this->textFilledMap, this->sizeMap, map.str().c_str());
	ReleaseMutex(this->hMutex);
}

void Central::buildCleanMap()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	int cols = this->townMap->getCols();
	vector<Node*> nodes = this->townMap->getNodes();
	tstringstream map;

	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodes[i]->isRoad()) {
			map << "_";
		}
		else {
			map << "X";
		}
		if (nodes[i]->getCol() == cols - 1) {
			map << endl;
		}
	}

	int size = (int)(_tcslen(map.str().c_str()) * sizeof(TCHAR));
	this->textCleanMap = new TCHAR[size];
	_tcscpy_s(this->textCleanMap, size, map.str().c_str());
	this->sizeMap = size;
	ReleaseMutex(this->hMutex);
}

TownMap* Central::getTownMap()
{
	return this->townMap;
}

TCHAR* Central::getFilledMap()
{
	this->buildFilledMap(); // ja tem mutex no interior
	return this->textFilledMap;
}

TCHAR* Central::getCleanMap()
{
	return this->textCleanMap;

}

vector<Passageiro*> Central::getClients()
{
	return this->clients;
}

Passageiro* Central::getPassageiro(PASSENGER* p)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	int pos = -1;
	for (int i = 0; i < this->clients.size(); i++) {
		if (_tcscmp(this->clients[i]->getId(), p->id) == 0) {
			pos = i;
		}
	}
	ReleaseMutex(this->hMutex);
	if (pos >= 0) {
		return this->clients[pos];
	}
	else {
		return nullptr;
	}

}

PASSENGER* Central::readConpassNP()
{
	PASSENGER* passenger = new PASSENGER;
	ZeroMemory(passenger, sizeof(PASSENGER));
	DWORD noBytesRead;
	BOOL readedNamedPipe = ReadFile(this->hConPassNPRead, (LPVOID)passenger, sizeof(PASSENGER), &noBytesRead, NULL);
	if (!readedNamedPipe) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel ler do Named Pipe Conpass!"), TYPE::ERRO);
		return nullptr;
	}
	return passenger;
}

void Central::writeConpassNP(PASSENGER* p)
{
	DWORD noBytesWrite;
	BOOL writedNamedPipe = WriteFile(this->hConPassNPWrite, (LPCVOID)p, sizeof(PASSENGER), &noBytesWrite, NULL);
	if (!writedNamedPipe) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel escrever no Named Pipe Conpass!"), TYPE::ERRO);
		return;
	}

	BOOL fluchedFileBuffer = FlushFileBuffers(this->hConPassNPWrite);
	if (!fluchedFileBuffer) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel submeter no Named Pipe Conpass!"), TYPE::ERRO);
		return;
	}
}

void Central::updateCar(TAXI* car)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	if (car->status == STATUS_TAXI::SAIR) {
		_tprintf(TEXT("\nO Taxi %s saiu de serviço!\nCOMMAND:"), car->plate);
		ReleaseMutex(this->hMutex);
		this->deleteCar(car);
		
		return;
	}
	if (car->status == STATUS_TAXI::ENTREGUE) {
		_tprintf(TEXT("\nO Taxi %s entregou o passageiro %s!\nCOMMAND:"), car->plate, car->client);
		Passageiro* p = this->getClient(car->client);
		p->setCol(p->getDestCol());
		p->setRow(p->getDestRow());
		p->setStatus(STATUS::ENTREGUE);
		this->writeConpassNP(&p->getStruct());
		this->deleteClient(&p->getStruct());
	}
	if (car->status == STATUS_TAXI::IRDESTINO) {
		Passageiro* p = this->getClient(car->client);
		p->setStatus(STATUS::NOCARRO);
		this->writeConpassNP(&p->getStruct());
		p->setCol(-1);
		p->setRow(-1);
	}
	for (int i = 0; i < cars.size(); i++)
	{
		if (cars[i]->isSamePlate(car->plate)) {
			cars[i]->update(car);
		}
	}
	ReleaseMutex(this->hMutex);
}

void Central::updateClient(PASSENGER* p)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	vector<Passageiro*> clients = this->getClients();
	for (int i = 0; i < clients.size(); i++){
		if (_tcscmp(clients[i]->getId(), p->id) == 0) {
			clients[i]->setRow(p->row);
			clients[i]->setCol(p->col);
			clients[i]->setDestCol(p->dest_col);
			clients[i]->setDestRow(p->dest_row);
			clients[i]->setStatus(p->status);
		}
	}
	ReleaseMutex(this->hMutex);
}

STATUS Central::validateClient(PASSENGER* p)
{
	if (p == nullptr) {
		return STATUS::ERRO;
	}
	WaitForSingleObject(this->hMutex, INFINITE);
	vector<Passageiro*> clients = this->getClients();
	BOOL exists = false;
	for (int i = 0; i < clients.size(); i++) {
		if (_tcscmp(clients[i]->getId(), p->id) == 0) {
			Passageiro* client = clients[i];
			exists = true;
			this->updateClient(p);
			if (client->getStatus() == STATUS::TRANSPORTAR) {
				if (this->isStreet(client->getDestRow(), client->getDestCol())) {
					if (this->bufferCircular->writePassenger(client->getStruct())) {
						ReleaseMutex(this->hMutex);
						return STATUS::EMESPERA;
					}else{
						ReleaseMutex(this->hMutex);
						return STATUS::BUFFERCHEIO;
					}
				}
				else {
					ReleaseMutex(this->hMutex);
					return STATUS::NOSTREET;
				}
			}
		}
	}
	if (!exists && p->status == STATUS::NOVO) {
		if (this->isStreet(p->row, p->col)) {
			this->addClient(new Passageiro(p));
			ReleaseMutex(this->hMutex);
			return STATUS::ACEITE;
		}
		else {
			ReleaseMutex(this->hMutex);
			return STATUS::NOSTREET;
		}
	}
	ReleaseMutex(this->hMutex);
	return STATUS::REJEITADO;

}

void Central::deleteClient(PASSENGER* p)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	Passageiro* passageiro = this->getPassageiro(p);
	for (auto iter = clients.begin(); iter != clients.end();) {
		if (_tcscmp((*iter)->getId(), passageiro->getId()) == 0) {
			delete* iter;
			iter = clients.erase(iter);
			break;
		}
		else {
			++iter;
		}
	}
	ReleaseMutex(this->hMutex);
}

void Central::deleteCar(TAXI* car)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	Car* pCar = this->getCar(car->plate);
	for (auto iter = cars.begin(); iter != cars.end();) {
		if (_tcscmp((*iter)->getPlate(), pCar->getPlate()) == 0) {
			delete* iter;
			iter = cars.erase(iter);
			break;
		}
		else {
			++iter;
		}
	}
	ReleaseMutex(this->hMutex);
}

void Central::deleteCar(Car* car)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	for (auto iter = cars.begin(); iter != cars.end();) {
		if (_tcscmp((*iter)->getPlate(), car->getPlate()) == 0) {
			delete* iter;
			iter = cars.erase(iter);
			break;
		}
		else {
			++iter;
		}
	}
	ReleaseMutex(this->hMutex);
}

void Central::addClient(Passageiro* p)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->clients.push_back(p);
	ReleaseMutex(this->hMutex);
}

vector<HANDLE*> Central::getHandles()
{
	return this->handles;
}

void Central::addHandle(HANDLE * h)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->handles.push_back(h);
	ReleaseMutex(this->hMutex);
}

void Central::addInterest(Car * car, Passageiro* client)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	client->addInterested(car);
	ReleaseMutex(this->hMutex);
}

BOOL Central::isStreet(int row, int col) const
{
	return this->townMap->isStreet(row,col);
}
