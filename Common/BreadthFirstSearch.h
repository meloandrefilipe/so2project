#include "TownMap.h"
#include <vector>
#include <fstream>
#include <map>

using namespace std;

typedef struct BREADTHFIRSTSEARCH_BESTPATH {
	int cost;
	vector<Node*> path;
}BESTPATH;


class BreadthFirstSearch{
	TownMap* townMap;
	std::map<Node*, Node *>  pred;
	std::map<Node*, int>  dist;
public:
	BreadthFirstSearch(TownMap* map);
	~BreadthFirstSearch();
	BESTPATH getBestPath(Node* src, Node* dest);
private:
	vector<Node*> getShortestDistance(Node* src, Node* dest);
	BOOL BFS(Node* src, Node* dest);
};
