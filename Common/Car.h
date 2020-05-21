#pragma once
#include <iostream>
#include <sstream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include "Core.h"

#define SPEED_SHIFTER 1
#define MAX_SPEED 10
#define DEFAULT_SPEED 5
#define NQ 10

class Car {
	int row;
	int col;
	int id;
	int nq;
	int speed;
	BOOL autopicker;
	TCHAR * plate;
	HANDLE hMutex;

public:
	Car(int id, int row, int col, TCHAR * plate);
	Car(TAXI* taxi);
	~Car();
	int getRow() const;
	int getCol() const;
	int getId() const;
	int getNq() const;
	BOOL getAutopicker() const;
	BOOL isSamePlate(TCHAR* plate);
	int getSpeed() const;
	TCHAR * getPlate();
	TCHAR * toString();
	TAXI toStruct();
	void speedUp();
	void speedDown();
	void setSpeed(int speed);
	void setNq(int nq);
	void setAutopicker(BOOL val);
	void setPosition(int row, int col);
};

