#pragma once
#include <time.h>
#include "Core.h"
#include "Car.h"
#include "WaitableTimer.h"
#include "TownMap.h"


class Taxista
{
	TownMap* townMap;
	BOOL exit;
	TCHAR* textmap;
	int mapSize;
	HANDLE hMutex;
public:
	DLLProfessores* dll;
	Car* car;

	Taxista();
	~Taxista();


	TCHAR* getMapText();
	void setMap(TownMap* m);
	void setMapSize(int size);
	BOOL isExit() const;
	void setExit(BOOL exit);
	Node* getRandomRoad();
	TownMap* getMap();

private:
	void buildMapText();

};

