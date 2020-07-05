#pragma once
//#include "Core.h"
#include "BreadthFirstSearch.h"
#include "Car.h"
#include "Passageiro.h"
#include "WaitableTimer.h"
#include "BufferCircular.h"

#define DEFAULT_WAIT_TIME 20

class Central
{
	TownMap* townMap;
	BOOL exit;
	TCHAR* textCleanMap;
	TCHAR* textFilledMap;
	HANDLE hMutex;
	int sizeMap;
	int waitTime;
	BOOL takingIn;
	BOOL pipesOn;
	vector<Passageiro*> clients;
	vector<HANDLE*> handles;


	// Named Pipe Conpass
	HANDLE hConPassNPRead;
	HANDLE hConPassNPWrite;

public:
	BufferCircular* bufferCircular;
	DLLProfessores* dll;
	vector<Car*> cars;

	Central();
	~Central();
	int getSizeCars() const;
	int getSizeMap() const;
	int getWaitTime() const;
	vector<HANDLE*> getHandles();
	STATUS validateClient(PASSENGER* p);
	vector<Car*> getCars();
	Car* getCar(TCHAR* matricula);
	Passageiro* getClient(TCHAR* id);
	TownMap* getTownMap();
	TCHAR* getFilledMap();
	TCHAR* getCleanMap();
	vector<Passageiro*> getClients();
	Passageiro* getPassageiro(PASSENGER* p);
	PASSENGER* readConpassNP();
	MAPINFODATA getMapInfoData();
    void writeConpassNP(PASSENGER* p);


	BOOL isPipesOn();
	void setPipesOn(BOOL val);
	DWORD expulsar(Car* car);
	DWORD connectConpass();
	DWORD closeConpass();
	DWORD sendAnswer(Car * car, Passageiro* client);
	void setWaitTime(int time);
	BOOL addCar(Car* car);
	void setExit(BOOL exit);
	void setTakingIn(BOOL val);
	void updateCar(TAXI* car);
	void updateClient(PASSENGER* p);
	void deleteClient(PASSENGER* p);
	void deleteCar(TAXI* car);
	void deleteCar(Car* car);
	void addClient(Passageiro * p);
	void addHandle(HANDLE * h);
	void addInterest(Car *car, Passageiro* client);

	BOOL isStreet(int row, int col) const;
	BOOL isExit();
	BOOL isTakingIn();
	BOOL carExists(TAXI* taxi);


private:
	void buildFilledMap();
	void buildCleanMap();
};

