#pragma once
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <fcntl.h>
#include <io.h>
#include <Car.h>
#include "BreadthFirstSearch.h"
#include "WaitableTimer.h"

using namespace std;

// Vars
TCHAR memCenCon[] = TEXT("Local\\MyFileMappingObject");

// Structs 
typedef struct THREAD_PARAMETERS {
	Car* car;
	BOOL exit;
}PARAMETERS;

// Threads
DWORD WINAPI CommandsThread(LPVOID lpParam);
DWORD WINAPI CommunicationThread(LPVOID lpParam);
DWORD WINAPI CloseThread(LPVOID lpParam);
DWORD WINAPI GetCarDataThread(LPVOID lpParam);

// Functions