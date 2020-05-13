#include <iostream>
#include <sstream>
#include <tchar.h>
#include <Windows.h>

using namespace std;

static int EDGES = 1000;

class Edge {
	TCHAR* id;
	TCHAR* source;
	TCHAR* target;

public:
	Edge(TCHAR* source, TCHAR* target);
	~Edge();
	TCHAR* getSource() const;
	TCHAR* getTarget() const;
	TCHAR* getID() const;
	void toString() const;
};