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

class Cidade
{
	BOOL exit;
	TCHAR* map;
	int mapSize;
	int sizeCars;
	int sizeClients;
	vector<Passageiro*> clients;
	vector<Car*> cars;
	HANDLE hMutex;

public:
	DLLProfessores* dll;
	Cidade();
	~Cidade();

	TCHAR* getMap();
	void setMap(TCHAR* map);

	int getMapSize();
	void setMapSize(int size);

	void update(MAPINFODATA* data);

	BOOL isExit() const;
	void setExit(BOOL val);
};

