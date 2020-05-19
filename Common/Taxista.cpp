#include "Taxista.h"

Taxista::Taxista()
{
	this->townMap = nullptr;
	this->car = nullptr;
	this->exit = false;
	this->textmap = new TCHAR[MAP_SHARE_SIZE];
}

Taxista::~Taxista()
{
	delete this->townMap;
	delete this->car;
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
	_tcscpy_s(this->textmap, MAP_SHARE_SIZE, map.str().c_str());
}

TCHAR* Taxista::getMapText()
{
	this->buildMapText();
	return this->textmap;
}

void Taxista::setMap(TownMap* m)
{
	this->townMap = m;
}

BOOL Taxista::isExit() const
{
	return this->exit;
}

void Taxista::setExit(BOOL exit)
{
	this->exit = exit;
}

Node* Taxista::getRandomRoad()
{
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
	return temp[select];
}

TownMap* Taxista::getMap()
{
	return this->townMap;
}
