#include "Edge.h"
#include "Node.h"
#include "Core.h"
#include <vector>
#include <fstream>

class TownMap {
	vector<Node*> nodes;
	vector<Edge*> edges;

public:
	TownMap();
	~TownMap();
	vector<Node*> getNodes();
	vector<Edge*> getEdges();
};

// functions
vector<Node*> buildNodes();
vector<Edge*> buildEdges(vector<Node*> nodes);