#include "BufferCircular.h"

BufferCircular::BufferCircular()
{
	ZeroMemory(this->bfc, sizeof(BUFFERCIRCULAR));
	ZeroMemory(this->hEvents, BUFFER_CIRCULAR_SIZE * sizeof(HANDLE));
	this->currentReadIndex = 0;
	this->currentWriteIndex = 0;
	this->totalPassengers = 0;

	for (int i = 0; i < BUFFER_CIRCULAR_SIZE; i++){
		tstringstream msg;
		msg << "buffer.circular.event." << i << endl;
		this->hEvents[i] = CreateEvent(NULL, TRUE, FALSE, msg.str().c_str());
		if (this->hEvents[i] == NULL) {
			_tprintf(TEXT("Não foi possivel criar eventos do Buffer Circular!\n"));
			return;
		}
	}

	this->hFile = CreateFile(SHAREDMEMORY_CEN_CON, GENERIC_ALL, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == NULL) {
		_tprintf(TEXT("Não foi possível criar o ficheiro de memoria para Buffer-Circular!\n"));
		CloseHandle(this->hFile);
		return;
	}
	this->hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, sizeof(BUFFERCIRCULAR), SHAREDMEMORY_BUFFER_CIRCULAR);
	if (hFileMapping == NULL) {
		_tprintf(TEXT("Não foi possivel criar o file mapping Buffer-circular!\n"));
		CloseHandle(this->hFile);
		CloseHandle(this->hFileMapping);
		return;
	}
	this->bfc = (BUFFERCIRCULAR*)MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(BUFFERCIRCULAR));
	if (this->bfc == NULL) {
		_tprintf(TEXT("Não foi possivel mapear o ficheiro!\n"));
		CloseHandle(this->hFile);
		CloseHandle(this->hFileMapping);
		return;
	}

}
BufferCircular::~BufferCircular()
{
	for (int i = 0; i < BUFFER_CIRCULAR_SIZE; i++) {
		CloseHandle(this->hEvents[i]);
	}
	CloseHandle(this->hFile);
	CloseHandle(this->hFileMapping);
}

DWORD BufferCircular::getCurrentReadIndex()
{
	return this->currentReadIndex;
}

DWORD BufferCircular::getCurrentWriteIndex()
{
	return this->currentWriteIndex;
}

DWORD BufferCircular::getBufferSize()
{
	return BUFFER_CIRCULAR_SIZE;
}

DWORD BufferCircular::getTotalPassengers()
{
	return this->totalPassengers;
}

BOOL BufferCircular::writePassenger(PASSENGER p){
	DWORD nextPos = (this->currentWriteIndex + 1) % this->getBufferSize();
	DWORD pos = this->currentWriteIndex;
	if (nextPos != this->currentReadIndex ) {
		this->bfc->dataArray[this->currentWriteIndex].row = p.row;
		this->bfc->dataArray[this->currentWriteIndex].col = p.col;
		this->bfc->dataArray[this->currentWriteIndex].dest_col = p.dest_col;
		this->bfc->dataArray[this->currentWriteIndex].dest_row = p.dest_row;
		this->bfc->dataArray[this->currentWriteIndex].status = p.status;
		_tcscpy_s(this->bfc->dataArray[this->currentWriteIndex].id, PASSENGER_NAME_SIZE, p.id);
		this->bfc->bufferPos = this->currentWriteIndex;
		_tprintf(TEXT("\nEscrevi o %s no buffer circular na posição %d\nCOMMAND:"), p.id, this->currentWriteIndex);
		SetEvent(this->hEvents[this->currentWriteIndex]);
		this->currentWriteIndex = nextPos;
		this->totalPassengers++;

		return TRUE;
	}
	return FALSE;
}

PASSENGER BufferCircular::readPassenger(){
	PASSENGER p;
	ZeroMemory(&p, sizeof(PASSENGER));
	DWORD nextPos = (this->currentReadIndex + 1) % this->getBufferSize();
	if (!this->isEmpty()) {
		ResetEvent(this->hEvents[this->currentReadIndex]);
		p.col = this->bfc->dataArray[this->currentReadIndex].col;
		p.row = this->bfc->dataArray[this->currentReadIndex].row;
		p.dest_col = this->bfc->dataArray[this->currentReadIndex].dest_col;
		p.dest_row = this->bfc->dataArray[this->currentReadIndex].dest_row;
		p.status = this->bfc->dataArray[this->currentReadIndex].status;
		_tcscpy_s(p.id, PASSENGER_NAME_SIZE, this->bfc->dataArray[this->currentReadIndex].id);
		ZeroMemory(&this->bfc->dataArray[this->currentReadIndex], sizeof(PASSENGER));
		this->currentReadIndex = nextPos;
	}
	return p;
}
BOOL BufferCircular::isEmpty()
{
	if (this->currentReadIndex == this->currentWriteIndex) {
		return true;
	}
	return false;
}
