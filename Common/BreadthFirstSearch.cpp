#include "BreadthFirstSearch.h"

BreadthFirstSearch::BreadthFirstSearch(TownMap* townMap){
	this->townMap = townMap;
}

BreadthFirstSearch::~BreadthFirstSearch(){
}

BESTPATH BreadthFirstSearch::getBestPath(Node* src, Node* dest)
{
	BESTPATH data;
	data.path = this->getShortestDistance(src, dest);
	reverse(data.path.begin(), data.path.end());
	return data;
}

BOOL BreadthFirstSearch::BFS(Node* src, Node* dest)
{
	std::map<Node*, int>  dist;
	vector<Node*> nodes = this->townMap->getNodes();
	int vertices = (int)nodes.size();
	std::map<Node*, BOOL> visited;
	vector<Node *> queue;

	for (int i = 0; i < vertices; i++)
	{
		visited[nodes[i]] = false;
		dist[nodes[i]] = INT_MAX;
		this->pred[nodes[i]] = NULL;
	}
	visited[src] = true;
	dist[src] = 0;
	queue.push_back(src);

	while (!queue.empty()) {
		Node* n = queue.front();
		queue.erase(queue.begin());
		for (int i = 0; i < nodes.size(); i++)
		{
			Node* s = nodes[i];
			if (_tcscmp(n->getID(), s->getID()) == 0) {
				vector<Node*> neightbs = s->getNeighbours();
				for (int j = 0; j < neightbs.size(); j++)
				{
					Node* neigh = neightbs[j];
					if (!visited[neigh]) {
						visited[neigh] = true;
						dist[neigh] = dist[n] + 1;
						this->pred[neigh] = n;
						queue.push_back(neigh);
						if (_tcscmp(neigh->getID(), dest->getID()) == 0) {
							this->dist = dist;
							return true;
						}
					}
				}
			}
		}
	}
	this->dist = dist;
	return false;
}

vector<Node*> BreadthFirstSearch::getShortestDistance(Node* src, Node* dest)
{
	vector<Node*> path;
	path.clear();
	if (!BFS(src, dest)) {
		_tprintf(TEXT("[BFS] Não existe ligação entre estes dois pontos!\n"));
		return path;
	}
	Node* crawl = dest;
	path.push_back(dest);
	while (this->pred[crawl] != NULL) {
		path.push_back(this->pred[crawl]);
		crawl = this->pred[crawl];
	}
	return path;
}