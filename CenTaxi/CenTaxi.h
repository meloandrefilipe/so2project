#pragma once

#include <iostream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include <fcntl.h>
#include <vector>
#include <io.h>
#include "WaitableTimer.h"
#include "DLLProfessores.h"
#include "Central.h"


using namespace std;


// Threads
DWORD WINAPI MainMenuThread(LPVOID lpParam);
DWORD WINAPI CommunicationThread(LPVOID lpParam);
DWORD WINAPI PlateValidatorThread(LPVOID lpParam);
DWORD WINAPI SendMapThread(LPVOID lpParam);
DWORD WINAPI SendMapInfoThread(LPVOID lpParam);