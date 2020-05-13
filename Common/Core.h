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
#define MAP_NAME "..\\map10_10.txt"


using namespace std;

typedef struct SHAREDMEMORY_TAXI {
	int row;
	int col;
	int pid;
	TCHAR matricula[TAXI_PLATE_SIZE];
}TAXI;