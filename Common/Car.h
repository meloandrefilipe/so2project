#pragma once
#include <iostream>
#include <sstream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include "Core.h"

#define SPEED_SHIFTER 0.5
#define MAX_SPEED 3
#define NQ 10

class Car {
	int row;
	int col;
	int id;
	int nq;
	float speed;
	BOOL autopicker;
	TCHAR * plate;

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
	float getSpeed() const;
	TCHAR * getPlate();
	TCHAR * toString();
	TAXI toStruct();
	void speedUp();
	void speedDown();
	void setSpeed(float speed);
	void setNq(int nq);
	void setAutopicker(BOOL val);

};

