#pragma once

#include <iostream>
#include <sstream>
#include <tchar.h>
#include <vector>
#include <Windows.h>

using namespace std;
static int NODES = 1000;

class Node {
	TCHAR* id;
	int row;
	int col;
	BOOL road;
	vector<Node*> neighbours;
public:
	Node(int row, int col, BOOL road);
	~Node();
	int getRow() const;
	int getCol() const;
	TCHAR* getID() const;

	void toString() const;
	BOOL isRoad() const;
	vector<Node*> getNeighbours() const;
	void addNeighbour(Node* node);
	BOOL equals(TCHAR* id);
};