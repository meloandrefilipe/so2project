#pragma once

#include <iostream>
#include <sstream>
#include <tchar.h>
#include <Windows.h>

#define SHAREDMEMORY_CEN_CON TEXT("C:\\temp\\cencon.txt")
#define SHAREDMEMORY_CEN_CON_ZONE TEXT("SharedMemoryCenTaxiConTaxi")
#define SEMAPHORE_CAN_READ_CENCON TEXT("CenConCanRead")
#define SEMAPHORE_CAN_WRITE_CENCON TEXT("CenConCanWrite")
#define SEMAPHORE_COUNT 10
#define WAIT_ONE_SECOND -10000000LL
#define WAIT_TEN_SECONDS -100000000LL



using namespace std;

static int NODES = 1000;
static int EDGES = 1000;

namespace Core {
	// Structs 
	typedef struct THREAD_PARAMETERS {
		TCHAR* car;
		int pid;
		BOOL exit;
	}PARAMETERS;

	class Node {
		TCHAR* id;
		int row;
		int col;
	public:
		Node(int row, int col);
		~Node();
		int getRow() const;
		int getCol() const;
		TCHAR* getID() const;
		void toString() const;
	};

	class Edge {
		TCHAR* id;
		TCHAR* source;
		TCHAR* target;

	public:
		Edge(TCHAR* source, TCHAR* target);
		~Edge();
		TCHAR* getSource() const;
		TCHAR* getTarget() const;
		TCHAR* getID() const;
		void toString() const;
	};

}