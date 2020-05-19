#include "Central.h"

Central::Central()
{
	this->townMap = new TownMap();
	this->exit = false;
	this->textMap = new TCHAR[MAP_SHARE_SIZE];
	this->hMutex = CreateMutex(NULL, FALSE, NULL);
	if (this->hMutex == NULL)
	{
		_tprintf(TEXT("Central CreateMutex error: %d\n"), GetLastError());
		return;
	}
}

Central::~Central()
{
	delete this->townMap;
	delete this->textMap;
	CloseHandle(this->hMutex);
}

int Central::getSizeCars() const
{
	return (int)this->cars.size();
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

void Central::buildMapToShare()
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

	_tcscpy_s(this->textMap, MAP_SHARE_SIZE, map.str().c_str());
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

	_tcscpy_s(this->textMap, MAP_SHARE_SIZE, map.str().c_str());

}

TownMap* Central::getTownMap()
{
	return this->townMap;
}

TCHAR* Central::getMapToShare()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->buildMapToShare();
	ReleaseMutex(this->hMutex);
	return this->textMap;
}

TCHAR* Central::getCleanMap()
{
	WaitForSingleObject(this->hMutex, INFINITE);
	this->buildCleanMap();
	ReleaseMutex(this->hMutex);
	return this->textMap;

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
