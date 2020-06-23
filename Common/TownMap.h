#include "Node.h"
#include "Core.h"
#include <vector>
#include <fstream>

class TownMap {
	vector<Node*> nodes;
	int rows;
	int cols;

public:
	TownMap();
	TownMap(TCHAR * textmap);
	~TownMap();
	vector<Node*> getNodes();
	Node* getNodeAt(int row, int col);
	BOOL isStreet(int row, int col);

	int getRows() const;
	int getCols() const;



private:
	void buildTown();
	void buildTownFromText(TCHAR * textmap);
};

// functions
vector<Node*> buildNodes();