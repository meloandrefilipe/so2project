#pragma once
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <fcntl.h>
#include <io.h>
#include "Cooms.h"


using namespace std;

// Vars
TCHAR memCenCon[] = TEXT("Local\\MyFileMappingObject");


// Threads
DWORD WINAPI CommandsThread(LPVOID lpParam);
DWORD WINAPI CommunicationThread(LPVOID lpParam);
DWORD WINAPI CloseThread(LPVOID lpParam);
DWORD WINAPI GetCarDataThread(LPVOID lpParam);
DWORD WINAPI GetMapThread(LPVOID lpParam);
DWORD WINAPI MoveCarThread(LPVOID lpParam);
DWORD WINAPI RespostaIntereseThread(LPVOID lpParam);
DWORD WINAPI BufferCircularThread(LPVOID lpParam);

// Functions
void Clear();
DWORD TransportClient(Taxista* taxista, TRANSPORT* transport);