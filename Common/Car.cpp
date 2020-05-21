#include "Car.h"



Car::Car(int id, int row, int col, TCHAR * plate){
	this->id = id;
	this->col = col;
	this->row = row;
	this->speed = DEFAULT_SPEED;
	this->nq = NQ;
	this->autopicker = true;
	this->plate = new TCHAR[TAXI_PLATE_SIZE];
	_tcscpy_s(this->plate, TAXI_PLATE_SIZE, plate);
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	if (this->hMutex == NULL)
	{
		_tprintf(TEXT("Car %d CreateMutex error: %d\n"), id, GetLastError());
		return;
	}
}
Car::Car(TAXI* taxi){
	this->id = taxi->pid;
	this->col = taxi->col;
	this->row = taxi->row;
	this->speed = taxi->speed;
	this->nq = taxi->nq;
	this->autopicker = taxi->autopicker;
	this->plate = new TCHAR[TAXI_PLATE_SIZE];
	_tcscpy_s(this->plate, TAXI_PLATE_SIZE,taxi->plate);
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
}

Car::~Car()
{
	delete this->plate;
	_tprintf(TEXT("Car %s destroyed.\n"), this->getPlate());
	CloseHandle(this->hMutex);
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

int Car::getSpeed() const
{
	return this->speed;
}

TCHAR* Car::getPlate()
{
	return this->plate;
}

TCHAR* Car::toString()
{
	return (TEXT("%d %d %s"), this->getRow(), this->getCol(), this->getPlate());
}

TAXI Car::toStruct()
{
	TAXI taxi; 

	taxi.col = this->getCol();
	taxi.row = this->getRow();
	taxi.pid = this->getId();
	taxi.nq = this->getNq();
	taxi.speed = this->getSpeed();
	taxi.autopicker = this->getAutopicker();
	_stprintf_s(taxi.plate, TEXT("%s"), this->getPlate());
	return taxi;
}

void Car::speedUp()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	if (this->getSpeed() > 1) {
		this->setSpeed(this->getSpeed() - SPEED_SHIFTER);
		_tprintf(TEXT("A velocidade do taxi foi aumentada para %d\n"), MAX_SPEED - this->getSpeed());
	}
	else {
		this->setSpeed(1);
		_tprintf(TEXT("A velocidade do taxi est� no maximo! %d\n"), MAX_SPEED);

	}
	ReleaseMutex(this->hMutex);
}

void Car::speedDown()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	if (this->getSpeed() < MAX_SPEED) {
		this->setSpeed(this->getSpeed() +  SPEED_SHIFTER);
		_tprintf(TEXT("A velocidade do taxi foi diminuida para %d\n"), MAX_SPEED - this->getSpeed());
	}
	else {
		this->setSpeed(MAX_SPEED);
		_tprintf(TEXT("A velocidade do taxi est� no m�nimo! %d\n"), 1);
	}
	ReleaseMutex(this->hMutex);
}

void Car::setSpeed(int speed)
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

void Car::setPosition(int row, int col)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->row = row;
	this->col = col;
	ReleaseMutex(this->hMutex);
}
