#pragma once
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <fcntl.h>
#include <io.h>
#include "BreadthFirstSearch.h"



using namespace std;

// Vars
TCHAR memCenCon[] = TEXT("Local\\MyFileMappingObject");

// Structs 
typedef struct THREAD_PARAMETERS {
	Car* car;
	BOOL exit;
}PARAMETERS;

// Threads
DWORD WINAPI CmdsThread(LPVOID lpParam);
DWORD WINAPI CommsThread(LPVOID lpParam);

// Functions
Car getCarData();