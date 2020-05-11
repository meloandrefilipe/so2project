#include "Car.h"

Car::Car(int id, int row, int col, TCHAR * plate){
	this->id = id;
	this->col = col;
	this->row = row;
	this->plate = new TCHAR[TAXI_PLATE_SIZE];
	_tcscpy_s(this->plate, TAXI_PLATE_SIZE,plate);
}

Car::~Car()
{
	_tprintf(TEXT("Car %s destroyed.\n"), this->getPlate());
}

int Car::getRow()
{
	return this->row;
}

int Car::getCol()
{
	return this->col;
}
int Car::getId()
{
	return this->id;
}

TCHAR* Car::getPlate()
{
	return this->plate;
}

TCHAR* Car::toString()
{
	return (TEXT("%d %d %s"), this->getRow(), this->getCol(), this->getPlate());
}
