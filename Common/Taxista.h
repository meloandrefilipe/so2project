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
public:
	SMARTMOVE move;
	DLLProfessores* dll;
	Car* car;

	Taxista();
	~Taxista();

	Node* getNodeAt(int row, int col);
	TCHAR* getMapText();
	void setMap(TownMap* m);
	void setMapSize(int size);
	BOOL isRandomMove() const;
	void enableRandomMove();
	void disableRandomMove();
	void resetSmartPath();
	BOOL getCanRegist() const;
	void setCanRegist(BOOL val);
	BOOL getSmartPath() const;
	BOOL isExit() const;
	void setExit(BOOL exit);
	Node* getRandomRoad();
	TownMap* getMap();

private:
	void buildMapText();

};

