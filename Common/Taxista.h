#pragma once
#include <time.h>
#include "Core.h"
#include "Car.h"
#include "WaitableTimer.h"
#include "BreadthFirstSearch.h"


typedef struct TAXI_SMARTMOVE {
	int dest_row;
	int dest_col;
}SMARTMOVE;


class Taxista
{
	TownMap* townMap;
	BOOL exit;
	TCHAR* textmap;
	int mapSize;
	HANDLE hMutex;
	BOOL randomMove;
	BOOL smartPath;
	BOOL canRegist;
	BOOL canInterest;
	TCHAR* atransportar;
public:
	SMARTMOVE move;
	DLLProfessores* dll;
	Car* car;

	Taxista();
	~Taxista();

	Node* getNodeAt(int row, int col);
	TCHAR* getMapText();
	TCHAR* getATransportar();
	void setMap(TownMap* m);
	void setMapSize(int size);
	void transportar(TCHAR* id);
	void clearATransportar();
	BOOL isRandomMove() const;
	void enableRandomMove();
	void disableRandomMove();
	void resetSmartPath();
	BOOL getCanRegist() const;
	void setCanRegist(BOOL val);
	BOOL getSmartPath() const;
	BOOL isExit() const;
	BOOL isInNq(PASSENGER p) const;
	BOOL getCanInterest() const;
	void setCanInterest(BOOL val);
	void setExit(BOOL exit);
	Node* getRandomRoad();
	TownMap* getMap();

private:
	void buildMapText();

};

