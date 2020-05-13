#include "Edge.h"
#include "Node.h"
#include "Core.h"
#include <vector>
#include <fstream>

class TownMap {
	vector<Node*> nodes;

public:
	TownMap();
	~TownMap();
	vector<Node*> getNodes();
};

// functions
vector<Node*> buildNodes();