#pragma once

#include <iostream>
#include <sstream>
#include <tchar.h>
#include <Windows.h>

#define SHAREDMEMORY_CEN_CON TEXT("C:\\tmp\\cencon.txt")
#define SHAREDMEMORY_CEN_MAP TEXT("C:\\tmp\\cenmap.txt")
#define SHAREDMEMORY_CEN_CON_ZONE TEXT("SharedMemoryCenTaxiConTaxi")
#define SHAREDMEMORY_PLATE_VALIDATION TEXT("SharedMemoryPlateValidationConTaxi")


// memoria partilhada, mapa para o contaxi
#define SHAREDMEMORY_SHAREMAP TEXT("C:\\tmp\\sharemap.txt")
#define SHAREDMEMORY_ZONE_SHAREMAP TEXT("SharedMemoryMapZone")
#define SEMAPHORE_SHAREMAP_READ TEXT("ShareMapRead")
#define SEMAPHORE_SHAREMAP_WRITE TEXT("ShareMapWrite")


// memoria partilhada, mapa para o mapinfo
#define SHAREDMEMORY_MAPINFO TEXT("C:\\tmp\\mapinfo.txt")
#define SHAREDMEMORY_ZONE_MAPINFO TEXT("SharedMemoryMapInfo")
#define SEMAPHORE_MAPINFO_READ TEXT("MapInfoRead")
#define SEMAPHORE_MAPINFO_WRITE TEXT("MapInfoWrite")

#define SEMAPHORE_CAN_READ_CENCON TEXT("CenConCanRead")
#define SEMAPHORE_CAN_WRITE_CENCON TEXT("CenConCanWrite")
#define SEMAPHORE_CAN_CONTAXI_CENCON TEXT("CenConContaxi")
#define SEMAPHORE_PLATE_VALIDATOR_READ TEXT("PlateValidatorCanRead")
#define SEMAPHORE_PLATE_VALIDATOR_WRITE TEXT("PlateValidatorCanWrite")
#define SEMAPHORE_PLATE_VALIDATOR_CONTAXI TEXT("PlateValidatorContaxi")
#define EVENT_CLOSE_ALL TEXT("CloseApps")
#define SEMAPHORE_COUNT 10
#define WAIT_ONE_SECOND -10000000LL
#define WAIT_TEN_SECONDS -100000000LL

#define TAXI_PLATE_SIZE 100
#define BUFFER_SIZE 2048
#define MAP_SHARE_SIZE 1000
#define SHAREDMEMORY_SIZE 3072
#define COMMAND_SIZE 100

#define MAP_NAME "..\\Maps\\map10_10.txt"
#define DLL_PATH_32 TEXT("..\\Dlls\\SO2_TP_DLL_32.dll")
#define DLL_PATH_64 TEXT("..\\Dlls\\SO2_TP_DLL_64.dll")
#define CENTAXI_MAIN_MUTEX TEXT("centaxi.main.mutex")
#define MUTEX_CENTAXI_MAPINFO TEXT("centaxi.mapinfo.mutex")

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
	int nq;
	float speed;
	BOOL autopicker;
	TCHAR plate[TAXI_PLATE_SIZE];
	TCHAR map[MAP_SHARE_SIZE];
}TAXI;

typedef struct SHAREDMEMORY_PLATE {
	int status;
	TCHAR plate[TAXI_PLATE_SIZE];
}PLATE;

typedef struct SHAREDMEMORY_MAPINFO_STRUCT {
	TCHAR map[MAP_SHARE_SIZE];
}MAPINFO;