#pragma once
#include "Cidade.h"


// Threads 
DWORD WINAPI GetMapThread(LPVOID lpParam);
DWORD WINAPI CloseThread(LPVOID lpParam);

// Functions
void Clear();