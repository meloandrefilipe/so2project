#include "Cidade.h"

Cidade::Cidade()
{
	this->exit = false;
	this->dll = new DLLProfessores();
	this->mapSize = 0;
	this->sizeCars = 0;
	this->sizeClients = 0;
	this->map = nullptr;
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	if (this->hMutex == NULL) {
		_tprintf(TEXT("Cidade CreateMutex error: %d\n"), GetLastError());
		return;
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

void Cidade::setMap(TCHAR* map)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->map = new TCHAR[mapSize];
	_tcscpy_s(this->map, mapSize, map);
	ReleaseMutex(this->hMutex);
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
	_tprintf(TEXT("\nCars: %d Clients: %d\n"), this->sizeCars, this->sizeClients);
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
