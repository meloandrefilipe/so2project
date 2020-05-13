#pragma once

#include <iostream>
#include <sstream>
#include <tchar.h>
#include <Windows.h>
#include "Car.h"

#define SHAREDMEMORY_CEN_CON TEXT("C:\\temp\\cencon.txt")
#define SHAREDMEMORY_CEN_CON_ZONE TEXT("SharedMemoryCenTaxiConTaxi")
#define SEMAPHORE_CAN_READ_CENCON TEXT("CenConCanRead")
#define SEMAPHORE_CAN_WRITE_CENCON TEXT("CenConCanWrite")
#define SEMAPHORE_COUNT 10
#define WAIT_ONE_SECOND -10000000LL
#define WAIT_TEN_SECONDS -100000000LL
#define BUFFER_SIZE 1024
#define TAXI_PLATE_SIZE 100
#define COMMAND_SIZE 100
#define MAP_NAME "..\\Maps\\map10_10.txt"
#define DLL_PATH_32 TEXT("..\\Dlls\\SO2_TP_DLL_32.dll")
#define DLL_PATH_64 TEXT("..\\Dlls\\SO2_TP_DLL_64.dll")
#define CENTAXI_MAIN_MUTEX TEXT("centaxi.main.mutex")

//Permitir que o mesmo código possa funcionar para ASCII ou UNICODE
#ifdef UNICODE
#define tcout wcout
#define tcin wcin
#define tstring wstring
#define tstringstream wstringstream 
#else
#define tcout cout
#define tcin cin
#define tstring string
#define tstringstream ostringstream 
#endif
typedef int(__cdecl* FuncRegister)(TCHAR* name, int type);
typedef int(__cdecl* FuncLog)(TCHAR* msg);

using namespace std;

typedef struct SHAREDMEMORY_TAXI {
	int row;
	int col;
	int pid;
	TCHAR matricula[TAXI_PLATE_SIZE];
}TAXI;