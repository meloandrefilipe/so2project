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
	

public:
	Car* car;

	Taxista();
	~Taxista();


	TCHAR* getMapText();
	void setMap(TownMap* m);

	BOOL isExit() const;
	void setExit(BOOL exit);
	Node* getRandomRoad();
	TownMap* getMap();

private:
	void buildMapText();

};

