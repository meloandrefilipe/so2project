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

#define SHAREDMEMORY_INTEREST_ZONE TEXT("SharedMemoryInterest")
#define SEMAPHORE_INTEREST_READ TEXT("SharedMemoryInterestRead")
#define SEMAPHORE_INTEREST_WRITE TEXT("SharedMemoryInterestWrite")

// ZONA Memoria partilhada buffer circular

#define SHAREDMEMORY_BUFFER_CIRCULAR TEXT("SharedMemoryBufferCiruclar")

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
#define EVENT_CONPASS_A TEXT("conpass.connect.A")
#define EVENT_CONPASS_B TEXT("conpass.connect.B")
#define EVENT_CONPASS_STARTED TEXT("conpass.connect.started")

//Mutexes
#define CENTAXI_MAIN_MUTEX TEXT("centaxi.main.mutex")
#define CONPASS_MAIN_MUTEX TEXT("conpass.main.mutex")
#define MUTEX_CENTAXI_MAPINFO TEXT("centaxi.mapinfo.mutex")
#define MUTEX_CENTAXI_TAKINGIN TEXT("centaxi.stop.mutex")


// Timmers
#define WAIT_ONE_SECOND -10000000LL
#define WAIT_TEN_SECONDS -100000000LL

#define TAXI_PLATE_SIZE 100
#define PASSENGER_NAME_SIZE 24
#define BUFFER_SIZE 1024
#define COMMAND_SIZE 100

//Ficheiros externos
#define MAP_NAME "..\\Maps\\map1.txt"
#define DLL_PATH_32 TEXT("..\\Dlls\\SO2_TP_DLL_32.dll")
#define DLL_PATH_64 TEXT("..\\Dlls\\SO2_TP_DLL_64.dll")

//Named Pipes
#define NAMED_PIPE_CONPASS_A TEXT("\\\\.\\pipe\\conpass_A") // cen le, con ecreve
#define NAMED_PIPE_CONPASS_B TEXT("\\\\.\\pipe\\conpass_B") // cen escreve, con le

// Buffer circular

#define BUFFER_CIRCULAR_SIZE 5


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

enum class STATUS { SAIR, EMESPERA, NOCARRO, ENTREGUE, NOVO, ACEITE, REJEITADO, TRANSPORTAR, NOSTREET, BUFFERCHEIO, ERRO, ACAMINHO, SEMINTERESSE
};
enum class STATUS_TRANSPORT { ACEITE, REJEITADO, EXPULSO };
enum class STATUS_TAXI { SAIR, IRPASSAGEIRO, IRDESTINO, ALEATORIO, NOCAMINHO, ENTREGUE};

typedef struct SHAREDMEMORY_TAXI {
	int row;
	int col;
	int pid;
	int nq;
	DOUBLE speed;
	BOOL autopicker;
	TCHAR plate[TAXI_PLATE_SIZE];
	TCHAR client[PASSENGER_NAME_SIZE];
	STATUS_TAXI status;
}TAXI;

typedef struct SHAREDMEMORY_PLATE {
	int status;
	TCHAR plate[TAXI_PLATE_SIZE];
}PLATE;

typedef struct SHAREDMEMORY_MAPINFO_STRUCT {
	int size;
	BOOL canRegist;
}MAPINFO;

typedef struct NAMEDPIPE_CONPASS_PASSENGER_STRUCT {
	TCHAR id[PASSENGER_NAME_SIZE];
	int row;
	int col;
	int dest_row;
	int dest_col;
	STATUS status;
	TCHAR plate[TAXI_PLATE_SIZE];
}PASSENGER;

typedef struct NAMEDPIPE_CONTAXI {
	PASSENGER client;
	STATUS_TRANSPORT status;
}TRANSPORT;

typedef struct SHAREDMEMORY_BUFFER_CIRCULAR_STRUCT {
	PASSENGER dataArray[BUFFER_CIRCULAR_SIZE];
	int bufferPos;
}BUFFERCIRCULAR;

typedef struct SHAREDMEMORY_INTEREST {
	TCHAR car[TAXI_PLATE_SIZE];
	TCHAR passenger[PASSENGER_NAME_SIZE];
}INTEREST;