#pragma once
#include <iostream>
#include <sstream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include "Core.h"

using namespace std;

class Car {
	int row;
	int col;
	int id;
	TCHAR * plate;

public:
	Car(int id, int row, int col, TCHAR * plate);
	~Car();
	int getRow();
	int getCol();
	int getId();
	TCHAR * getPlate();
	TCHAR * toString();
};

