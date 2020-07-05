#pragma once
#include "Core.h"
#include "Car.h"
#include <vector>
#include <time.h>

class Passageiro
{
	TCHAR* id;
	TCHAR* plate;
	int row;
	int col;
	int dest_row;
	int dest_col;
	STATUS status;
	vector<Car*> interested;
	HANDLE hMutex;
	DOUBLE timeToArrive;

public:
	Passageiro(TCHAR* id, int row, int col);
	Passageiro(PASSENGER *p);
	Passageiro();
	~Passageiro();
	TCHAR* getId() const;
	int getRow() const;
	int getCol() const;
	int getDestRow() const;
	int getDestCol() const;
	STATUS getStatus() const;
	PASSENGER getStruct() const;
	vector<Car*> getInterested();
	Car* getRandomInterested();
	TCHAR* getPlate() const;
	DOUBLE getTimeToArrive();
	void setTimeToArrive(DOUBLE t);
	
	void setPlate(TCHAR* plate);
	void addInterested(Car * car);
	void setRow(int row);
	void setCol(int col);
	void setDestRow(int row);
	void setDestCol(int col);
	void setStatus(STATUS status);
	void update(PASSENGER p);
};

