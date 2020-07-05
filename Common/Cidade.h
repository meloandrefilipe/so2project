#pragma once
#include <iostream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include <fcntl.h>
#include <vector>
#include <io.h>
#include "WaitableTimer.h"
#include "Passageiro.h"
#include "TownMap.h"

class Cidade
{
	BOOL exit;
	TCHAR* map;
	int mapSize;
	int rows;
	int cols;
	int sizeCars;
	int sizeClients;
	vector<Passageiro*> clients;
	vector<Car*> cars;
	HANDLE hMutex;    
	HANDLE hMutexMap;


public:
	HBRUSH color_building, color_street, color_car, color_person, border_car_ocup, border_car_free, border_client_car;
	TownMap* town;
	MSG msg;
	DLLProfessores* dll;
	Cidade();
	~Cidade();
	HANDLE getHMutexMap();
	TCHAR* getMap();

	void setMap(TCHAR* map);
	vector<Car*> getCars();
	vector<Passageiro*> getClients();
	Car* getCarAt(int row, int col);
	Passageiro* getPassageiroAt(int row, int col);
	int getRows();
	int getCols();
	int getMapSize();
	void setMapSize(int size);

	void update(MAPINFODATA* data);

	BOOL isExit() const;
	void setExit(BOOL val);
};

