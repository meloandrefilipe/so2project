#pragma once

#include <iostream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include <fcntl.h>
#include <vector>
#include <io.h>
#include "Central.h"


using namespace std;


// Threads
DWORD WINAPI ReadInterest(LPVOID lpParam);
DWORD WINAPI MainMenuThread(LPVOID lpParam);
DWORD WINAPI CommunicationThread(LPVOID lpParam);
DWORD WINAPI PlateValidatorThread(LPVOID lpParam);
DWORD WINAPI SendMapThread(LPVOID lpParam);
DWORD WINAPI SendMapInfoThread(LPVOID lpParam);
DWORD WINAPI ConPassThread(LPVOID lpParam);
DWORD WINAPI WaitForAnswers(LPVOID lpParam);
DWORD WINAPI WaitForConpass(LPVOID lpParam);