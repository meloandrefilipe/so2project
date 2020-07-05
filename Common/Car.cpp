#include "Car.h"

Car::Car(int id, int row, int col, TCHAR * plate){
	this->id = id;
	this->oldCol = -1;
	this->oldRow = -1;
	this->row = row;
	this->col = col;
	this->row = row;
	this->speed = DEFAULT_SPEED;
	this->nq = NQ;
	this->autopicker = true;
	this->plate = new TCHAR[TAXI_PLATE_SIZE];
	this->client = new TCHAR[PASSENGER_NAME_SIZE];
	ZeroMemory(this->client, sizeof(TCHAR) * PASSENGER_NAME_SIZE);
	ZeroMemory(this->plate, sizeof(TCHAR) * TAXI_PLATE_SIZE);
	_tcscpy_s(this->plate, TAXI_PLATE_SIZE, plate);
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	srand((unsigned int)time(NULL));
	this->color.red = rand() % 256;
	this->color.green = rand() % 256;
	this->color.blue = rand() % 256;
	this->status = STATUS_TAXI::ALEATORIO;
	if (this->hMutex == NULL)
	{
		_tprintf(TEXT("Car %d CreateMutex error: %d\n"), id, GetLastError());
		return;
	}
	this->hNamedPipe = NULL;
	this->timeToDestiny = 0;
}
Car::Car(TAXI* taxi){
	this->oldCol = taxi->oldCol;
	this->oldRow = taxi->oldRow;
	this->id = taxi->pid;
	this->color.red = taxi->color.red;
	this->color.green = taxi->color.green;
	this->color.blue = taxi->color.blue;
	this->col = taxi->col;
	this->row = taxi->row;
	this->speed = taxi->speed;
	this->timeToDestiny = taxi->timeToDestiny;
	this->status = taxi->status;
	this->nq = taxi->nq;
	this->autopicker = taxi->autopicker;
	this->plate = new TCHAR[TAXI_PLATE_SIZE];
	this->client = new TCHAR[PASSENGER_NAME_SIZE];
	ZeroMemory(this->client, sizeof(TCHAR) * PASSENGER_NAME_SIZE);
	ZeroMemory(this->plate, sizeof(TCHAR) * TAXI_PLATE_SIZE);
	_tcscpy_s(this->plate, TAXI_PLATE_SIZE,taxi->plate);
	_tcscpy_s(this->client, PASSENGER_NAME_SIZE, taxi->client);
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	this->hNamedPipe = NULL;
}

Car::~Car()
{
	delete this->plate;
	delete this->client;
	if (this->hNamedPipe != NULL) {
		DisconnectNamedPipe(this->hNamedPipe);
		CloseHandle(this->hNamedPipe);
	}
	CloseHandle(this->hMutex);
}

DOUBLE Car::getTimeToDestiny()
{
	return this->timeToDestiny;
}

void Car::setTimeToDestiny(DOUBLE t)
{
	this->timeToDestiny = t;
}

COLOR Car::getColor()
{
	return this->color;
}

int Car::getOldRow() const
{
	return this->oldRow;
}

int Car::getOldCol() const
{
	return this->oldCol;
}

int Car::getRow() const
{
	return this->row;
}

int Car::getCol() const
{
	return this->col;
}
int Car::getId() const
{
	return this->id;
}

int Car::getNq() const
{
	return this->nq;
}

BOOL Car::getAutopicker() const
{
	return this->autopicker;
}

BOOL Car::isSamePlate(TCHAR* plate)
{
	if (_tcscmp(this->getPlate(), plate) == 0) {
		return true;
	}
	return false;
}

DOUBLE Car::getSpeed() const
{
	return this->speed;
}

TCHAR* Car::getPlate()
{
	return this->plate;
}
TAXI Car::toStruct()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	TAXI taxi; 
	taxi.oldCol = this->getOldCol();
	taxi.oldRow = this->getOldRow();
	taxi.col = this->getCol();
	taxi.row = this->getRow();
	taxi.pid = this->getId();
	taxi.nq = this->getNq();
	taxi.speed = this->getSpeed();
	taxi.autopicker = this->getAutopicker();
	taxi.timeToDestiny = this->timeToDestiny;
	taxi.status = this->getStatus();
	taxi.color.red = this->color.red;
	taxi.color.green = this->color.green;
	taxi.color.blue = this->color.blue;
	_tcscpy_s(taxi.client, PASSENGER_NAME_SIZE, this->getClient());
	_tcscpy_s(taxi.plate, TAXI_PLATE_SIZE, this->getPlate());
	ReleaseMutex(this->hMutex);
	return taxi;
}

HANDLE Car::getPipeHandle()
{
	return this->hNamedPipe;
}

STATUS_TAXI Car::getStatus()
{
	return this->status;
}

TCHAR* Car::getClient()
{
	return this->client;
}

void Car::clearClient()
{
	this->client = new TCHAR[PASSENGER_NAME_SIZE];
	ZeroMemory(this->client, sizeof(TCHAR) * PASSENGER_NAME_SIZE);
}

void Car::setClient(TCHAR* client)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	_tcscpy_s(this->client, PASSENGER_NAME_SIZE, client);
	ReleaseMutex(this->hMutex);
}

void Car::update(TAXI* taxi)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->id = taxi->pid;
	this->oldCol = taxi->oldCol;
	this->oldRow = taxi->oldRow;
	this->col = taxi->col;
	this->row = taxi->row;
	this->speed = taxi->speed;
	this->status = taxi->status;
	this->timeToDestiny = taxi->timeToDestiny;
	this->nq = taxi->nq;
	this->autopicker = taxi->autopicker;
	this->plate = new TCHAR[TAXI_PLATE_SIZE];
	this->client = new TCHAR[PASSENGER_NAME_SIZE];
	_tcscpy_s(this->plate, TAXI_PLATE_SIZE, taxi->plate);
	_tcscpy_s(this->client, PASSENGER_NAME_SIZE, taxi->client);
	ReleaseMutex(this->hMutex);
}

void Car::setStatus(STATUS_TAXI status)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->status = status;
	ReleaseMutex(this->hMutex);
}

void Car::setPipeHandle(HANDLE pipeHandle)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->hNamedPipe = pipeHandle;
	ReleaseMutex(this->hMutex);
}

void Car::speedDown(){
	WaitForSingleObject(this->hMutex, INFINITE);
	if (this->getSpeed() < MAX_SPEED) {
		this->setSpeed(this->getSpeed() + SPEED);
		_tprintf(TEXT("A velocidade do taxi foi diminuida\n"));
	}
	else {
		this->setSpeed(MAX_SPEED);
		_tprintf(TEXT("A velocidade do taxi est� no minimo!\n"));

	}
	ReleaseMutex(this->hMutex);
}

void Car::speedUp()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	if (this->getSpeed() > MIN_SPEED) {
		this->setSpeed(this->getSpeed() - SPEED);
		_tprintf(TEXT("A velocidade do taxi foi aumentada\n"));
	}
	else {
		this->setSpeed(MIN_SPEED);
		_tprintf(TEXT("A velocidade do taxi est� no maximo!\n"));

	}
	ReleaseMutex(this->hMutex);
}

void Car::setSpeed(DOUBLE speed)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->speed = speed;
	ReleaseMutex(this->hMutex);
}

void Car::setNq(int nq)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->nq = nq;
	ReleaseMutex(this->hMutex);
}

void Car::setAutopicker(BOOL val)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->autopicker = val;
	ReleaseMutex(this->hMutex);
}

void Car::setPosition(int row, int col){
	WaitForSingleObject(this->hMutex, INFINITE);
	this->oldCol = this->row;
	this->oldRow = this->col;
	this->row = row;
	this->col = col;
	ReleaseMutex(this->hMutex);
}
