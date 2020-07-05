#include "Cidade.h"

Cidade::Cidade()
{
	this->exit = false;
	this->dll = new DLLProfessores();
	this->mapSize = 0;
	this->sizeCars = 0;
	this->sizeClients = 0;
	this->cols = 0;
	this->rows = 0;
	this->map = nullptr;
	this->town = nullptr;
	this->hMutexMap = nullptr;
	this->color_building = CreateSolidBrush(RGB(19, 15, 64));
	this->color_street = CreateSolidBrush(RGB(48, 51, 107));
	this->color_car = CreateSolidBrush(RGB(241, 196, 15));
	this->color_person = CreateSolidBrush(RGB(46, 204, 113));
	this->border_car_ocup = CreateSolidBrush(RGB(255, 0, 0));
	this->border_car_free = CreateSolidBrush(RGB(255, 255, 255));
	this->border_client_car = CreateSolidBrush(RGB(255, 255, 255));
	this->msg = MSG();
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	if (this->hMutex == NULL) {
		_tprintf(TEXT("Cidade CreateMutex error: %d\n"), GetLastError());
		return;
	}
	this->hMutexMap = CreateEvent(NULL, TRUE, FALSE, TEXT("event.map.load"));
	if (this->hMutexMap == NULL) {
		this->dll->log((TCHAR*)TEXT("Não foi possivel criar o evento!"), TYPE::ERRO);
		return ;
	}
}

Cidade::~Cidade()
{
	delete this->dll;
	for (int i = 0; i < clients.size(); i++) {
		delete (clients[i]);
	}
	for (int c = 0; c < cars.size(); c++) {
		delete (cars[c]);
	}
	this->clients.clear();
	this->cars.clear();
	CloseHandle(this->hMutex);
}

TCHAR* Cidade::getMap()
{
	return this->map;
}

HANDLE Cidade::getHMutexMap()
{
	return this->hMutexMap;
}

void Cidade::setMap(TCHAR* map)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->map = new TCHAR[mapSize];
	_tcscpy_s(this->map, mapSize, map);
	this->town = new TownMap(this->map);
	ReleaseMutex(this->hMutex);
}

vector<Car*> Cidade::getCars()
{
	return this->cars;
}

vector<Passageiro*> Cidade::getClients()
{
	return this->clients;
}

Car* Cidade::getCarAt(int row, int col)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	for (int i = 0; i < cars.size(); i++)
	{
		if (cars[i]->getCol() == col && cars[i]->getRow() == row) {
			ReleaseMutex(this->hMutex);
			return cars[i];
		}
	}
	ReleaseMutex(this->hMutex);
	return nullptr;
}

Passageiro* Cidade::getPassageiroAt(int row, int col)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	for (int i = 0; i < clients.size(); i++){
		if (clients[i]->getCol() == col && clients[i]->getRow() == row) {
			ReleaseMutex(this->hMutex);
			return clients[i];
		}
	}
	ReleaseMutex(this->hMutex);
	return nullptr;
}

int Cidade::getRows()
{
	return this->rows;
}

int Cidade::getCols()
{
	return this->cols;
}

int Cidade::getMapSize()
{
	return this->mapSize;
}

void Cidade::setMapSize(int size)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->mapSize = size;
	ReleaseMutex(this->hMutex);
}

void Cidade::update(MAPINFODATA * data)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->mapSize = data->size;
	this->sizeCars = data->sizeCars;
	this->sizeClients = data->sizeClients;
	this->cols = data->cols;
	this->rows = data->rows;
	for (int i = 0; i < clients.size(); i++){
		delete (clients[i]);
	}
	for (int c = 0; c < cars.size(); c++) {
		delete (cars[c]);
	}
	this->clients.clear();
	this->cars.clear();

	for (int i = 0; i < data->sizeClients; i++)
	{
		this->clients.push_back(new Passageiro(&data->clients[i]));
	}
	for (int c = 0; c < data->sizeCars; c++)
	{
		this->cars.push_back(new Car(&data->cars[c]));
	}
	ReleaseMutex(this->hMutex);
}

BOOL Cidade::isExit() const
{
	return this->exit;
}

void Cidade::setExit(BOOL val)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->exit = val;
	ReleaseMutex(this->hMutex);
}
