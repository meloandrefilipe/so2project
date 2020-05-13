#include "Edge.h"

Edge::Edge(TCHAR* source, TCHAR* target) {
	this->source = new TCHAR[20];
	_tcscpy_s(this->source, 20, source);
	this->target = new TCHAR[20];
	_tcscpy_s(this->target, 20, target);
	this->id = new TCHAR[20];
	swprintf_s(this->id, 20, TEXT("E_%d"), EDGES);
	EDGES++;
}
Edge::~Edge() {
	_tprintf(TEXT("Edge#%s deleted!\n"), this->getID());
}

TCHAR* Edge::getSource() const {
	return this->source;
}
TCHAR* Edge::getTarget() const {
	return this->target;
}
TCHAR* Edge::getID() const {
	return this->id;
}
void Edge::toString() const {
	_tprintf(TEXT("Edge#%s Source:%s Target:%s\n"), this->getID(), this->getSource(), this->getTarget());
}