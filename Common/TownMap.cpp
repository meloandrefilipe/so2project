#include "TownMap.h"

TownMap::TownMap() {
    this->cols = 0;
    this->rows = 0;
    buildTown();
}

TownMap::TownMap(TCHAR* textmap)
{
    this->cols = 0;
    this->rows = 0;
    buildTownFromText(textmap);
}

TownMap::~TownMap(){
	_tprintf(TEXT("[MAP] O mapa presente em '%s' foi apagado da memôria!"), TEXT(MAP_NAME));
}

vector<Node*> TownMap::getNodes()
{
	return this->nodes;
}

int TownMap::getRows() const
{
    return this->rows;
}

int TownMap::getCols() const
{
    return this->cols;
}

void TownMap::buildTown()
{
    char ch;
    int row = 0, col = 0;
    fstream fin(MAP_NAME, fstream::in);
    while (fin >> noskipws >> ch) {

        // Adicionar nodes
        if (ch != '\n') {
            if (ch == '_') {
                nodes.push_back(new Node(row, col, true));
            }
            else {
                nodes.push_back(new Node(row, col, false));
            }
            col++;
        }
        // Reset nas vars de coluna e linha
        if (ch == '\n') {
            row++;
            this->cols = col;
            col = 0;
        }
    }
    this->rows = row -1;
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->isRoad()) {
            for (int j = 0; j < nodes.size(); j++) {
                if (nodes[j]->isRoad()) {
                    if ((nodes[j]->getCol() == (nodes[i]->getCol() - 1)) && (nodes[j]->getRow() == nodes[i]->getRow())) { // ver o da atras
                        nodes[i]->addNeighbour(nodes[j]);
                    }
                    if ((nodes[j]->getCol() == (nodes[i]->getCol() + 1)) && (nodes[j]->getRow() == nodes[i]->getRow())) { // ver o da frente
                        nodes[i]->addNeighbour(nodes[j]);
                    }
                    if ((nodes[j]->getRow() == (nodes[i]->getRow() + 1)) && (nodes[j]->getCol() == nodes[i]->getCol())) { // ver o de baixo
                        nodes[i]->addNeighbour(nodes[j]);
                    }
                    if ((nodes[j]->getRow() == (nodes[i]->getRow() - 1)) && (nodes[j]->getCol() == nodes[i]->getCol())) { // ver o de cima
                        nodes[i]->addNeighbour(nodes[j]);
                    }
                }
            }
        }
    }
}

void TownMap::buildTownFromText(TCHAR* textmap)
{
    int col = 0;
    int row = 0;
    int size = (int)_tcsclen(textmap);
    TCHAR* map = textmap;
    for (int i = 0; i < size; i++)
    {
        if (map[i] != '\n') {
            if (map[i] == '_') {
                nodes.push_back(new Node(row, col, true));
            }
            else {
                nodes.push_back(new Node(row, col, false));
            }
            col++;
        }
        // Reset nas vars de coluna e linha
        if (map[i] == '\n') {
            row++;
            this->cols = col;
            col = 0;
        }
    }
    this->rows = row -1;
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->isRoad()) {
            for (int j = 0; j < nodes.size(); j++) {
                if (nodes[j]->isRoad()) {
                    if ((nodes[j]->getCol() == (nodes[i]->getCol() - 1)) && (nodes[j]->getRow() == nodes[i]->getRow())) { // ver o da atras
                        nodes[i]->addNeighbour(nodes[j]);
                    }
                    if ((nodes[j]->getCol() == (nodes[i]->getCol() + 1)) && (nodes[j]->getRow() == nodes[i]->getRow())) { // ver o da frente
                        nodes[i]->addNeighbour(nodes[j]);
                    }
                    if ((nodes[j]->getRow() == (nodes[i]->getRow() + 1)) && (nodes[j]->getCol() == nodes[i]->getCol())) { // ver o de baixo
                        nodes[i]->addNeighbour(nodes[j]);
                    }
                    if ((nodes[j]->getRow() == (nodes[i]->getRow() - 1)) && (nodes[j]->getCol() == nodes[i]->getCol())) { // ver o de cima
                        nodes[i]->addNeighbour(nodes[j]);
                    }
                }
            }
        }
    }
}
Node* TownMap::getNodeAt(int row, int col)
{
    vector<Node*> nodes = this->getNodes();
    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i]->getCol() == col && nodes[i]->getRow() == row) {
            return nodes[i];
        }
    }
    return nullptr;
}

BOOL TownMap::isStreet(int row, int col)
{
    vector<Node*> nodes = this->getNodes();
    for (int i = 0; i < nodes.size(); i++)
    {
        if (nodes[i]->getCol() == col && nodes[i]->getRow() == row && nodes[i]->isRoad()) {
            return true;
        }
    }
    return false;
}
