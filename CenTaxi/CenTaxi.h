#pragma once

#include <iostream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include <fcntl.h>
#include <vector>
#include <io.h>
#include <Car.h>
#include "BreadthFirstSearch.h"
#include "WaitableTimer.h"


using namespace std;


// Threads
DWORD WINAPI MainMenuThread(LPVOID lpParam);
DWORD WINAPI CommunicationThread(LPVOID lpParam);
DWORD WINAPI PlateValidatorThread(LPVOID lpParam);

// Structs
typedef struct THREAD_PARAMETERS {
	BOOL exit;
	vector<Car*> cars;
}PARAMETERS;
