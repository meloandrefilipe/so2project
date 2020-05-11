#include "Car.h"

Car::Car(int row, int col, TCHAR * plate){
	this->col = col;
	this->row = row;
	this->plate = new TCHAR[20];
	_tcscpy_s(this->plate, 20 ,plate);
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

TCHAR* Car::getPlate()
{
	return this->plate;
}

TCHAR* Car::toString()
{
	return (TEXT("%d %d %s"), this->getRow(), this->getCol(), this->getPlate());
}
