#pragma once
#include <iostream>
#include <sstream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include "Core.h"

#define SPEED_SHIFTER 0.25 // são 250ms para mudar a posição
#define MAX_SPEED 0.25 // a velocidade mais rapida para mudar posição é 250ms
#define MIN_SPEED 2 // a velocidade mais lenta para mudar posição é 2s
#define DEFAULT_SPEED 1// a velocidade inicial é 1s
#define NQ 10

class Car {
	int row;
	int col;
	int id;
	int nq;
	DOUBLE speed;
	BOOL autopicker;
	TCHAR * plate;
	HANDLE hMutex;
	HANDLE hNamedPipe;
	STATUS_TAXI status;
	TCHAR * client;
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
	DOUBLE getSpeed() const;
	TCHAR * getPlate();
	TAXI toStruct();
	HANDLE getPipeHandle();
	STATUS_TAXI getStatus();
	TCHAR* getClient();

	void clearClient();
	void setClient(TCHAR* client);
	void update(TAXI* car);
	void setStatus(STATUS_TAXI status);
	void setPipeHandle(HANDLE pipeHandle);
	void speedUp();
	void speedDown();
	void setSpeed(DOUBLE speed);
	void setNq(int nq);
	void setAutopicker(BOOL val);
	void setPosition(int row, int col);
};

