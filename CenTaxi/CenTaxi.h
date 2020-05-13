#pragma once

#include <iostream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include <fcntl.h>
#include <vector>
#include <io.h>
#include "BreadthFirstSearch.h"


using namespace std;


// Threads
DWORD WINAPI InitMenu(LPVOID lpParam);
DWORD WINAPI CommsThread(LPVOID lpParam);

// Structs
typedef struct THREAD_PARAMETERS {
	BOOL exit;
	vector<Car*> cars;
}PARAMETERS;
