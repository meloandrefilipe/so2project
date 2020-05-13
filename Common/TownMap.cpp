#include "TownMap.h"

TownMap ::TownMap() {
	this->nodes = buildNodes();
	this->edges = buildEdges(this->nodes);
}

TownMap::~TownMap(){
	_tprintf(TEXT("[MAP] O mapa presente em '%s' foi apagado da memôria!"), TEXT(MAP_NAME));
}

vector<Node*> TownMap::getNodes()
{
	return this->nodes;
}

vector<Edge*> TownMap::getEdges()
{
	return this->edges;
}

vector<Node*> buildNodes() {
    vector<Node*> nodes;
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
        }
        col++;


        // Reset nas vars de coluna e linha
        if (ch == '\n') {
            row++;
            col = 0;
        }
    }
    return nodes;
}
vector<Edge*> buildEdges(vector<Node*> nodes) {
    vector<Edge*> edges;
    for (int i = 0; i < nodes.size(); i++){
        if (nodes[i]->isRoad()) {
            for (int j = 0; j < nodes.size(); j++) {
                if (nodes[j]->isRoad()) {
                    if ((nodes[j]->getCol() == (nodes[i]->getCol() - 1)) && (nodes[j]->getRow() == nodes[i]->getRow())) { // ver o da atras
                        nodes[i]->addNeighbour(nodes[j]);
                        edges.push_back(new Edge(nodes[i]->getID(), nodes[j]->getID()));
                    }
                    if ((nodes[j]->getCol() == (nodes[i]->getCol() + 1)) && (nodes[j]->getRow() == nodes[i]->getRow())) { // ver o da frente
                        nodes[i]->addNeighbour(nodes[j]);
                        edges.push_back(new Edge(nodes[i]->getID(), nodes[j]->getID()));
                    }
                    if ((nodes[j]->getRow() == (nodes[i]->getRow() + 1)) && (nodes[j]->getCol() == nodes[i]->getCol())) { // ver o de baixo
                        nodes[i]->addNeighbour(nodes[j]);
                        edges.push_back(new Edge(nodes[i]->getID(), nodes[j]->getID()));
                    }
                    if ((nodes[j]->getRow() == (nodes[i]->getRow() - 1)) && (nodes[j]->getCol() == nodes[i]->getCol())) { // ver o de cima
                        nodes[i]->addNeighbour(nodes[j]);
                        edges.push_back(new Edge(nodes[i]->getID(), nodes[j]->getID()));
                    }
                }
            }
        }
    }
    return edges;
}