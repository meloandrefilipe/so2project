#pragma once
#include "Passageiro.h"
#include "WaitableTimer.h"
#include <vector>
#include "BreadthFirstSearch.h"

class Estacao
{
	vector<Passageiro*> passageiros;
	vector<HANDLE *> handles;
	BOOL exit;
	HANDLE hReadPipe, hWritePipe;

public:

	Estacao();
	~Estacao();

	DLLProfessores* dll;
	
	vector<Passageiro*> getPassageiros();
	vector<HANDLE*> getHandles();
	Passageiro* getPassageiro(TCHAR* id);
	Passageiro* updateClient(PASSENGER p);
	int getPassageirosSize() const;
	BOOL isExit() const;

	void addPassageiro(Passageiro* p );
	void deletePassageiro(Passageiro* p );
	void execStatus(Passageiro* p);
	void setExit(BOOL val);
	void addHandle(HANDLE *h);

	Passageiro* addMove(TCHAR* id, int row, int col);
	PASSENGER readNamedPipe();
	BOOL writeNamedPipe(Passageiro* client);


};

