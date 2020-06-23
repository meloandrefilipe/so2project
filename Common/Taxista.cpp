#include "Taxista.h"
#include <cstdlib>

Taxista::Taxista()
{
	this->move.dest_col = 0;
	this->move.dest_row = 0;
	this->canRegist = true;
	this->randomMove = true;
	this->smartPath = false;
	this->townMap = nullptr;
	this->car = nullptr;
	this->exit = false;
	this->canInterest = true;
	this->textmap = nullptr;
	this->mapSize = 0;
	this->dll = new DLLProfessores();
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	if (this->hMutex == NULL)
	{
		_tprintf(TEXT("Taxita CreateMutex error: %d\n"), GetLastError());
		return;
	}
	this->atransportar = new TCHAR[PASSENGER_NAME_SIZE];
	ZeroMemory(this->atransportar, sizeof(TCHAR) * PASSENGER_NAME_SIZE);
}

Taxista::~Taxista()
{
	delete this->dll;
	delete this->townMap;
	delete this->textmap;
	delete this->car;
	CloseHandle(this->hMutex);
}

void Taxista::buildMapText()
{
	int cols = this->townMap->getCols();
	vector<Node*> nodes = this->townMap->getNodes();
	tstringstream map;
	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodes[i]->isRoad()) {
			map << "_";
		}
		else {
			map << "X";
		}

		if (nodes[i]->getCol() == cols - 1) {
			map << endl;
		}

	}
	this->textmap = new TCHAR[this->mapSize];
	_tcscpy_s(this->textmap, this->mapSize, map.str().c_str());
}

Node* Taxista::getNodeAt(int row, int col)
{
	return this->townMap->getNodeAt(row,col);
}

TCHAR* Taxista::getMapText()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->buildMapText();
	ReleaseMutex(this->hMutex);

	return this->textmap;
}

TCHAR* Taxista::getATransportar()
{
	return this->atransportar;
}

void Taxista::setMap(TownMap* m)
{
	this->townMap = m;
}

void Taxista::setMapSize(int size)
{
	this->mapSize = size;
}

void Taxista::transportar(TCHAR* id)
{
	_tcscpy_s(this->atransportar, PASSENGER_NAME_SIZE, id);
}

void Taxista::clearATransportar()
{
	ZeroMemory(this->atransportar, sizeof(TCHAR) * PASSENGER_NAME_SIZE);
}

BOOL Taxista::isRandomMove() const
{
	return this->randomMove;
}

void Taxista::enableRandomMove()
{
	this->randomMove = true;
}

void Taxista::disableRandomMove()
{
	this->randomMove = false;
}

void Taxista::resetSmartPath()
{
	this->smartPath = !this->smartPath;
}

BOOL Taxista::getCanRegist() const
{
	return this->canRegist;
}

void Taxista::setCanRegist(BOOL val)
{
	this->canRegist = val;
}

BOOL Taxista::getSmartPath() const
{
	return this->smartPath;
}

BOOL Taxista::isExit() const
{
	return this->exit;
}

BOOL Taxista::isInNq(PASSENGER p) const
{
	if ((abs(this->car->getRow() - p.row) <= this->car->getNq()) && (abs(this->car->getCol() - p.col) <= this->car->getNq())) {
		return TRUE;
	}
	return FALSE;
}

BOOL Taxista::getCanInterest() const
{
	return this->canInterest;
}

void Taxista::setCanInterest(BOOL val)
{
	this->canInterest = val;
}

void Taxista::setExit(BOOL exit)
{
	this->exit = exit;
}

Node* Taxista::getRandomRoad()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	
	vector<Node*> nodes = this->townMap->getNodes();
	vector<Node*> temp;

	for (int i = 0; i < nodes.size() ; i++)
	{
		if (nodes[i]->isRoad()) {
			temp.push_back(nodes[i]);
		}
	}
	int size = (int)temp.size();
	srand((unsigned int)time(NULL));
	int select = (int)rand() % size;
	ReleaseMutex(this->hMutex);
	return temp[select];
}

TownMap* Taxista::getMap()
{
	return this->townMap;
}
