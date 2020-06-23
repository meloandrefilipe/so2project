#include "Passageiro.h"

Passageiro::Passageiro(TCHAR* id, int row, int col)
{
	this->id = new TCHAR[PASSENGER_NAME_SIZE];
	this->plate = new TCHAR[TAXI_PLATE_SIZE];
	ZeroMemory(this->plate, sizeof(TCHAR) * TAXI_PLATE_SIZE);
	_tcscpy_s(this->id, PASSENGER_NAME_SIZE, id);
	this->row = row;
	this->col = col;
	this->dest_col = -1;
	this->dest_row = -1;
	this->status = STATUS::NOVO;
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	if (this->hMutex == NULL)
	{
		_tprintf(TEXT("Central CreateMutex error: %d\n"), GetLastError());
		return;
	}
}

Passageiro::Passageiro(PASSENGER* p)
{
	this->id = new TCHAR[PASSENGER_NAME_SIZE];
	this->plate = new TCHAR[TAXI_PLATE_SIZE];
	ZeroMemory(this->plate, sizeof(TCHAR) * TAXI_PLATE_SIZE);
	_tcscpy_s(this->id, PASSENGER_NAME_SIZE, p->id);
	this->row = p->row;
	this->col = p->col;
	this->dest_col = p->dest_col;
	this->dest_row = p->dest_row;
	this->status = p->status;
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	this->setPlate(p->plate);
	if (this->hMutex == NULL)
	{
		_tprintf(TEXT("Central CreateMutex error: %d\n"), GetLastError());
		return;
	}
}

Passageiro::~Passageiro()
{
	CloseHandle(this->hMutex);
	_tprintf(TEXT("Passageiro %s saiu da aplicação!\n"), this->getId());
}

TCHAR* Passageiro::getId() const
{
	return this->id;
}

int Passageiro::getRow() const
{
	return this->row;
}

int Passageiro::getCol() const
{
	return this->col;
}

int Passageiro::getDestRow() const
{
	return this->dest_row;
}

int Passageiro::getDestCol() const
{

	return this->dest_col;
}

STATUS Passageiro::getStatus() const
{
	return this->status;
}

PASSENGER Passageiro::getStruct() const
{
	WaitForSingleObject(this->hMutex, INFINITE);
	PASSENGER passageiro;
	_tcscpy_s(passageiro.id,PASSENGER_NAME_SIZE, this->getId());
	_tcscpy_s(passageiro.plate, TAXI_PLATE_SIZE, this->getPlate());
	passageiro.col = this->getCol();
	passageiro.row = this->getRow();
	passageiro.dest_col = this->getDestCol();
	passageiro.dest_row = this->getDestRow();
	passageiro.status = this->getStatus();

	ReleaseMutex(this->hMutex);
	return passageiro;
}
TCHAR* Passageiro::getPlate() const
{
	return this->plate;
}

vector<Car*> Passageiro::getInterested()
{
	return this->interested;
}

Car* Passageiro::getRandomInterested(){
	WaitForSingleObject(this->hMutex, INFINITE);
	srand((unsigned int)time(NULL));
	vector<Car*> interested = this->getInterested();
	if (interested.size() > 0) {
		 ReleaseMutex(this->hMutex);
		 return interested[rand() % interested.size()];
	}
	ReleaseMutex(this->hMutex);
	return nullptr;
}

void Passageiro::setPlate(TCHAR* plate)
{
	_tcscpy_s(this->plate, TAXI_PLATE_SIZE, plate);
}

void Passageiro::addInterested(Car * car)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->interested.push_back(car);
	ReleaseMutex(this->hMutex);
}

void Passageiro::setRow(int row)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->row = row;
	ReleaseMutex(this->hMutex);
}

void Passageiro::setCol(int col)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->col = col;
	ReleaseMutex(this->hMutex);
}

void Passageiro::setDestRow(int row)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->dest_row = row;
	ReleaseMutex(this->hMutex);
}

void Passageiro::setDestCol(int col)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->dest_col = col;
	ReleaseMutex(this->hMutex);
}

void Passageiro::setStatus(STATUS status)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->status = status;
	ReleaseMutex(this->hMutex);
}

void Passageiro::update(PASSENGER p)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->row = p.row;
	this->col = p.col;
	this->dest_col = p.dest_col;
	this->dest_row = p.dest_row;
	this->status = p.status;
	this->setPlate(p.plate);
	ReleaseMutex(this->hMutex);
}


