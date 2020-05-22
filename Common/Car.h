#pragma once
#include <iostream>
#include <sstream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include "Core.h"

#define SPEED_SHIFTER -2500000LL // são 250ms para mudar a posição
#define MAX_SPEED -2500000LL // a velocidade mais rapida para mudar posição é 250ms
#define MIN_SPEED -20000000LL // a velocidade mais lenta para mudar posição é 2s
#define DEFAULT_SPEED -10000000LL// a velocidade inicial é 1s
#define NQ 10

class Car {
	int row;
	int col;
	int id;
	int nq;
	LONGLONG speed;
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
	LONGLONG getSpeed() const;
	TCHAR * getPlate();
	TAXI toStruct();
	void speedUp();
	void speedDown();
	void setSpeed(LONGLONG speed);
	void setNq(int nq);
	void setAutopicker(BOOL val);
	void setPosition(int row, int col);
};

