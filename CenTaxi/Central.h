#pragma once
#include "Core.h"
#include "BreadthFirstSearch.h"
#include "Car.h"

class Central
{
	TownMap* townMap;
	BOOL exit;
	TCHAR* textMap;
	HANDLE hMutex;

public:
	vector<Car*> cars;
	Central();
	~Central();
	int getSizeCars() const;
	void addCar(Car* car);
	void setExit(BOOL exit);
	BOOL isExit();
	BOOL carExists(TAXI* taxi);
	vector<Car*> getCars();
	TownMap* getTownMap();
	TCHAR* getMapToShare();
	TCHAR* getCleanMap();
	void updateCar(TAXI* car);
private:
	void buildMapToShare();
	void buildCleanMap();
};

