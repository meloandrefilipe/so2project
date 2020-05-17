#include "Car.h"



Car::Car(int id, int row, int col, TCHAR * plate){
	this->id = id;
	this->col = col;
	this->row = row;
	this->speed = 1;
	this->nq = NQ;
	this->autopicker = true;
	this->plate = new TCHAR[TAXI_PLATE_SIZE];
	_tcscpy_s(this->plate, TAXI_PLATE_SIZE, plate);
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
}

Car::~Car()
{
	_tprintf(TEXT("Car %s destroyed.\n"), this->getPlate());
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

float Car::getSpeed() const
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
	if (this->getSpeed() < MAX_SPEED) {
		this->setSpeed(this->getSpeed() + (float) SPEED_SHIFTER);
		_tprintf(TEXT("A velocidade do taxi foi aumentada para %f\n"), this->getSpeed());
	}
	else {
		this->setSpeed(MAX_SPEED);
		_tprintf(TEXT("A velocidade do taxi está no maximo! %f\n"), this->getSpeed());

	}
}

void Car::speedDown()
{
	if (this->getSpeed() > 1) {
		this->setSpeed(this->getSpeed() - (float) SPEED_SHIFTER);
		_tprintf(TEXT("A velocidade do taxi foi diminuida para %f\n"), this->getSpeed());
	}
	else {
		this->setSpeed(1);
		_tprintf(TEXT("A velocidade do taxi está no mínimo! %f\n"), this->getSpeed());
	}
}

void Car::setSpeed(float speed)
{
	this->speed = speed;
}

void Car::setNq(int nq)
{
	this->nq = nq;
}

void Car::setAutopicker(BOOL val)
{
	this->autopicker = val;
}
