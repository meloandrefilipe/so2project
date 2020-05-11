#pragma once
#include <iostream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include "Core.h"
#include "Car.h"
#include <fcntl.h>
#include <io.h>

#define BUFFER_SIZE 1024

using namespace std;
using namespace Core;

// Vars
TCHAR memCenCon[] = TEXT("Local\\MyFileMappingObject");

// Structs 

// Threads
DWORD WINAPI CmdsThread(LPVOID lpParam);
DWORD WINAPI CommsThread(LPVOID lpParam);

// Functions
Car getCarData();