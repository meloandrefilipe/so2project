#pragma once
#include "Core.h"
#include "BreadthFirstSearch.h"
#include "Car.h"
#include "WaitableTimer.h"

class Central
{
	TownMap* townMap;
	BOOL exit;
	TCHAR* textCleanMap;
	TCHAR* textFilledMap;
	HANDLE hMutex;
	int sizeMap;
public:
	DLLProfessores* dll;
	vector<Car*> cars;
	Central();
	~Central();
	int getSizeCars() const;
	int getSizeMap() const;
	void addCar(Car* car);
	void setExit(BOOL exit);
	BOOL isExit();
	BOOL carExists(TAXI* taxi);
	vector<Car*> getCars();
	TownMap* getTownMap();
	TCHAR* getFilledMap();
	TCHAR* getCleanMap();
	void updateCar(TAXI* car);
private:
	void buildFilledMap();
	void buildCleanMap();
};

