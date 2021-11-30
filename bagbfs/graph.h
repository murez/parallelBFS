#ifndef GRAPH_H
#define GRAPH_H

#include "bag.h"
#include "chainadj.h"
#include <iostream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>


class Graph
{
//	friend std::ostream& operator << (std::ostream& stream, const Graph& g);
public:
    int n,m;
    int *distance,*parent;

	Graph(const Graph& other) = default;

	Graph(Graph&& other) = default;

    ~Graph();

	Graph& operator = (const Graph& other) = default;

	Graph& operator = (Graph&& other) = default;

    Graph(std::string filename);

    int sbfs(int vertex);
	int pbfs(int vertex);

//	void save(const std::string &fileName) const;
    void add_edge(int u, int v);
    void reset();

private:

//	Bag processLevel(Bag& inBag, int level, std::vector<int>& dist) const;
    Bag processLevel(Bag& inBag, int level);


//	void processPennant(Pennant &pennant, Bag &outBag, int level, std::vector<int> &dist) const;
    void processPennant(Pennant &pennant, Bag &outBag, int level);

	//void reduce(std::vector<Bag>::iterator a, std::vector<Bag>::iterator b);

	/*int nVertices;
	double pEdge;
	bool directed;*/
	std::vector<std::vector<int>> adj;
    int edge_all;
    int cnt;
    int *to_pre,*next_pre,*head_pre;
    int *to, *edge;
};	// Graph



#endif // GRAPH_H