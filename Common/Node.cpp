#include "Node.h"

Node::Node(int row, int col, BOOL road) {
	this->id = new TCHAR[20];
	swprintf_s(this->id, 20, TEXT("N_%d"), NODES);
	this->row = row;
	this->col = col;
	this->road = road;
	NODES++;
}
Node::~Node() {
	_tprintf(TEXT("Node#%s deleted!\n"), this->getID());
}


int Node::getCol() const {
	return this->col;
}
int Node::getRow() const {
	return this->row;
}
TCHAR* Node::getID() const {
	return this->id;
}
void Node::toString() const {
	_tprintf(TEXT("Node#%s ROW:%d COL:%d\n"), this->getID(), this->getRow(), this->getCol());
}
BOOL Node::isRoad() const {
	return this->road;
}

vector<Node*> Node::getNeighbours() const
{
	return this->neighbours;
}

void Node::addNeighbour(Node* node){
	this->neighbours.push_back(node);
}

BOOL Node::equals(TCHAR* id)
{
	if (_tcscmp(this->getID(), id) == 0) {
		return TRUE;
	}
	return FALSE;
}
