#pragma once

#include <iostream>
#include <sstream>
#include <tchar.h>
#include <Windows.h>



//memoria partilhada comunicação contaxi->centaxi;
#define SHAREDMEMORY_CEN_CON TEXT("C:\\tmp\\cencon.txt")
#define SHAREDMEMORY_CEN_CON_ZONE TEXT("SharedMemoryCenTaxiConTaxi")
#define SEMAPHORE_CAN_READ_CENCON TEXT("CenConCanRead")
#define SEMAPHORE_CAN_WRITE_CENCON TEXT("CenConCanWrite")

//Memoria partilhada para validação da matricula
#define SHAREDMEMORY_PLATE_VALIDATION TEXT("SharedMemoryPlateValidationConTaxi")
#define SEMAPHORE_PLATE_VALIDATOR_READ TEXT("PlateValidatorCanRead")
#define SEMAPHORE_PLATE_VALIDATOR_WRITE TEXT("PlateValidatorCanWrite")
#define SEMAPHORE_PLATE_VALIDATOR_CONTAXI TEXT("PlateValidatorContaxi")

// memoria partilhada, mapa para o contaxi
#define SHAREDMEMORY_SHAREMAP TEXT("C:\\tmp\\sharemap.txt")
#define SHAREDMEMORY_CONTAXI_MAP TEXT("ConTaxiMap")
#define SHAREDMEMORY_CONTAXI_MAP_SIZE TEXT("ConTaxiMapSize")
#define SEMAPHORE_SHAREMAP_READ TEXT("ShareMapRead")
#define SEMAPHORE_SHAREMAP_WRITE TEXT("ShareMapWrite")
#define SEMAPHORE_SHAREMAP_SIZE TEXT("ShareMapSize")
#define SEMAPHORE_SHAREMAP_WANT TEXT("ShareMapWant")

// memoria partilhada, mapa para o mapinfo
#define SHAREDMEMORY_MAPINFO TEXT("C:\\tmp\\mapinfo.txt")
#define SHAREDMEMORY_ZONE_MAPSIZE TEXT("SharedMemoryMapSize")
#define SHAREDMEMORY_ZONE_MAPINFO TEXT("SharedMemoryMapInfo")
#define SEMAPHORE_MAPINFO_READ TEXT("MapInfoRead")
#define SEMAPHORE_MAPINFO_WRITE TEXT("MapInfoWrite")
#define SEMAPHORE_MAPINFO_SIZE TEXT("MapInfoSize")

//eventos
#define EVENT_CLOSE_ALL TEXT("CloseApps")
#define EVENT_BOOT_ALL TEXT("CanBoot")

//Mutexes
#define CENTAXI_MAIN_MUTEX TEXT("centaxi.main.mutex")
#define MUTEX_CENTAXI_MAPINFO TEXT("centaxi.mapinfo.mutex")
#define MUTEX_CENTAXI_TAKINGIN TEXT("centaxi.stop.mutex")

// Timmers
#define WAIT_ONE_SECOND -10000000LL
#define WAIT_TEN_SECONDS -100000000LL

#define TAXI_PLATE_SIZE 100
#define BUFFER_SIZE 1024
#define COMMAND_SIZE 100

//Ficheiros externos
#define MAP_NAME "..\\Maps\\map2.txt"
#define DLL_PATH_32 TEXT("..\\Dlls\\SO2_TP_DLL_32.dll")
#define DLL_PATH_64 TEXT("..\\Dlls\\SO2_TP_DLL_64.dll")


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
	LONGLONG speed;
	BOOL autopicker;
	TCHAR plate[TAXI_PLATE_SIZE];
}TAXI;

typedef struct SHAREDMEMORY_PLATE {
	int status;
	TCHAR plate[TAXI_PLATE_SIZE];
}PLATE;

typedef struct SHAREDMEMORY_MAPINFO_STRUCT {
	int size;
	BOOL canRegist;
}MAPINFO;