#pragma once
#include <iostream>
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
#include <fcntl.h>
#include <vector>
#include <io.h>
#include "Estacao.h"


using namespace std;

typedef struct ADD_PASSENGER_STRUCT {
	Estacao* estacao;
	Passageiro* client;
}ADDPASSENGER;


// Threads
DWORD WINAPI CommandsThread(LPVOID lpParam);
DWORD WINAPI CloseThread(LPVOID lpParam);
DWORD WINAPI GetMapThread(LPVOID lpParam);
DWORD WINAPI SendPassengerThread(LPVOID lpParam);
DWORD WINAPI ReadNamedPipe(LPVOID lpParam);


// Functions
void Clear();