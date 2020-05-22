#pragma once
#include "Core.h"
#include "BreadthFirstSearch.h"
#include "Car.h"
#include "WaitableTimer.h"

#define DEFAULT_WAIT_TIME 10

class Central
{
	TownMap* townMap;
	BOOL exit;
	TCHAR* textCleanMap;
	TCHAR* textFilledMap;
	HANDLE hMutex;
	int sizeMap;
	int waitTime;
	BOOL takingIn;
public:
	DLLProfessores* dll;
	vector<Car*> cars;
	Central();
	~Central();
	int getSizeCars() const;
	int getSizeMap() const;
	int getWaitTime() const;
	void setWaitTime(int time);
	void addCar(Car* car);
	void setExit(BOOL exit);
	void setTakingIn(BOOL val);
	BOOL isExit();
	BOOL isTakingIn();
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

