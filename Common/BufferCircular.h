#pragma once
#include "Core.h"

class BufferCircular
{
	BUFFERCIRCULAR* bfc = new BUFFERCIRCULAR;
	HANDLE hEvents[BUFFER_CIRCULAR_SIZE];
	DWORD currentReadIndex;
	DWORD currentWriteIndex;
	DWORD totalPassengers;
	HANDLE hFile, hFileMapping;

public:
	BufferCircular();
	~BufferCircular();

	DWORD getCurrentReadIndex();
	DWORD getCurrentWriteIndex();
	DWORD getBufferSize();
	DWORD getTotalPassengers();
	BOOL writePassenger(PASSENGER p);
	PASSENGER readPassenger();


private:
	BOOL isEmpty();
};

