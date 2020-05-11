#pragma once
#include <iostream>
#include <sstream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>

using namespace std;

class Car {
	int row;
	int col;
	TCHAR * plate;

public:
	Car(int row, int col, TCHAR * plate);
	~Car();
	int getRow();
	int getCol();
	TCHAR * getPlate();
	TCHAR * toString();
};

