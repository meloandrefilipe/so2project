#include "Central.h"

Central::Central()
{
	this->townMap = new TownMap();
	this->dll = new DLLProfessores();
	this->exit = false;
	this->textCleanMap = nullptr;
	this->textFilledMap = nullptr;
	this->sizeMap = 0;
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	if (this->hMutex == NULL)
	{
		_tprintf(TEXT("Central CreateMutex error: %d\n"), GetLastError());
		return;
	}
	WaitForSingleObject(this->hMutex, INFINITE);
	this->buildCleanMap();
	ReleaseMutex(this->hMutex);
}

Central::~Central()
{
	delete this->dll;
	delete this->townMap;
	delete this->textCleanMap;
	delete this->textFilledMap;
	CloseHandle(this->hMutex);
}

int Central::getSizeCars() const
{
	return (int)this->cars.size();
}

int Central::getSizeMap() const
{
	return this->sizeMap;
}
void Central::addCar(Car* car)
{
	this->cars.push_back(car);
}

void Central::setExit(BOOL exit)
{
	this->exit = exit;
}

BOOL Central::isExit()
{
	return this->exit;
}

BOOL Central::carExists(TAXI* taxi)
{
	BOOL exists = false;
	for (int i = 0; i < cars.size(); i++)
	{
		if (cars[i]->isSamePlate(taxi->plate)) {
			exists = true;
			break;
		}
	}
	return exists;
}

vector<Car*> Central::getCars()
{
	return this->cars;
}

void Central::buildFilledMap()
{
	int cols = this->townMap->getCols();
	vector<Node*> nodes = this->townMap->getNodes();
	vector<Car*> cars = this->getCars();
	tstringstream map;

	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodes[i]->isRoad()) {
			BOOL hasCar = false;
			for (int c = 0; c < cars.size(); c++)
			{
				if (cars[c]->getCol() == nodes[i]->getCol() && cars[c]->getRow() == nodes[i]->getRow()) {
					hasCar = true;
					break;
				}
			}
			if (hasCar) {
				map << "C";
			}
			else {
				map << "_";
			}
		}
		else {
			map << "X";
		}
		if (nodes[i]->getCol() == cols - 1) {
			map << endl;
		}
	}

	this->textFilledMap = new TCHAR[this->sizeMap];
	_tcscpy_s(this->textFilledMap, this->sizeMap, map.str().c_str());
}

void Central::buildCleanMap()
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

	int size = (int)(_tcslen(map.str().c_str()) * sizeof(TCHAR));
	this->textCleanMap = new TCHAR[size];
	_tcscpy_s(this->textCleanMap, size, map.str().c_str());
	this->sizeMap = size;
}

TownMap* Central::getTownMap()
{
	return this->townMap;
}

TCHAR* Central::getFilledMap()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->buildFilledMap();
	ReleaseMutex(this->hMutex);
	return this->textFilledMap;
}

TCHAR* Central::getCleanMap()
{
	return this->textCleanMap;

}

void Central::updateCar(TAXI* car)
{
	WaitForSingleObject(this->hMutex, INFINITE);
	for (int i = 0; i < cars.size(); i++)
	{
		if (cars[i]->isSamePlate(car->plate)) {
			cars[i]->setPosition(car->row, car->col);
		}
	}
	ReleaseMutex(this->hMutex);
}
