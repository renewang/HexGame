/*
 * ShortestPathAlgo.h
 *
 *  Created on: Oct 30, 2013
 *      Author: renewang
 */

#ifndef SHORTESTPATHALGO_H_
#define SHORTESTPATHALGO_H_

#include <list>
#include <vector>
#include <limits>

#include "Graph.h"
#include "PriorityQueue.h"

using namespace std;

template<class Type, class Val>
class ShortestPathAlgo
{
	typedef Graph<Type, Val> Graph;
	typedef Node<Type, Val> Node;
	typedef PriorityQueue<Type, Val> PriorityQueue;

private:
	Graph* graph;
	list<Node*>* shortestPath;
	Val shortestPathSize;

	/*
	 * actual dijkstra algorithm implementation
	 */
	void dijkstraImpl(int indexOfSource, int indexOfDest)
	{
		vector<Node*>* close = new vector<Node*>; //visited or close set
		PriorityQueue* open = new PriorityQueue; //unvisited or open set
		const vector<Node*>* vertices = graph->getAllVertices(); //get the all vertices in graph
		const Val INF = numeric_limits<Val>::max(); //set the INF as the maximal value of the type

		Node* source = vertices->at(indexOfSource - 1);

		close->push_back(source); //initialize the close set with source node
		shortestPath->push_back(source);

		//initialize PriorityQueue with all infinity
		for (unsigned i = 0; i < vertices->size(); i++)
		{
			if ((i + 1) != static_cast<unsigned>(source->vertexIdex))
				open->insert(vertices->at(i), INF);
		}

		Val* distance = new Val[vertices->size()];
		bool* visited = new bool[vertices->size()]; //marked if the nodes have been visited (put in close set already)
		int* prevNode = new int[vertices->size()];

		for (unsigned i = 0; i < vertices->size(); i++)
		{
			distance[i] = INF;
			visited[i] = false;
			prevNode[i] = -1;
		}
		for (unsigned i = 0; i < vertices->size(); i++)
			visited[source->vertexIdex - 1] = true;

		Val min = static_cast<Val>(0);
		Node* current = source;
		while (close->size() < vertices->size()
				&& current->vertexIdex != indexOfDest)
		{
			list<Node*>* neighbors = current->neighbors;
			list<Val>* edges = current->edges;
			typename list<Node*>::iterator iterNeigh = neighbors->begin();
			typename list<Val>::iterator iterEdge = edges->begin();

			for (; iterNeigh != neighbors->end(); ++iterNeigh, ++iterEdge)
			{
				Val edgeVal = min + *(iterEdge);

				if (edgeVal < distance[(*iterNeigh)->vertexIdex - 1]
						&& !visited[(*iterNeigh)->vertexIdex - 1])
				{
					distance[(*iterNeigh)->vertexIdex - 1] = edgeVal;
					prevNode[(*iterNeigh)->vertexIdex - 1] =
							current->vertexIdex;
					open->chgPrioirity((*iterNeigh), edgeVal);
				}
			}

			current = open->minPrioirty();
			min = distance[current->vertexIdex - 1];
			visited[current->vertexIdex - 1] = true;

			while ((shortestPath->back())->vertexIdex != source->vertexIdex
					&& prevNode[current->vertexIdex - 1]
							!= (shortestPath->back())->vertexIdex)
				shortestPath->pop_back();

			if (prevNode[current->vertexIdex - 1] != (shortestPath->back())->vertexIdex)
				shortestPath->push_back(
						vertices->at(prevNode[current->vertexIdex - 1] - 1));

			close->push_back(current);
			shortestPath->push_back(current);
		}

		shortestPathSize = min;

		delete close;
		delete[] visited;
		delete[] distance;
		delete[] prevNode;
	}
	;

public:
	/*
	 * default constructor
	 */
	ShortestPathAlgo() :
			graph(NULL), shortestPathSize(-1)
	{
		shortestPath = new list<Node*>;
	}
	;
	/*
	 * constructor based on the given graph
	 */
	ShortestPathAlgo(Graph* graph) :
			graph(graph), shortestPathSize(-1)
	{
		shortestPath = new list<Node*>;
	}
	;
	/*
	 * destructor
	 */
	virtual ~ShortestPathAlgo()
	{
		delete shortestPath;
	}
	;
	/*
	 * don't know what it is for...
	 */
	void
	vertices(list<Node*> vertices); //list of vertices in G(V,E).
	/*
	 * return the shortest path given two nodes
	 */
	list<Node*>*
	path(int indexNodeFrom, int indexNodeTo)
	{
		//find shortest path between u-w and returns the sequence of vertices representing shorest path u-v1-v2-É-vn-w.
		if (shortestPath == 0 || shortestPath->size() == 0)
			dijkstraImpl(indexNodeFrom, indexNodeTo);
		list<Node*>* pathVec = new list<Node*>(*shortestPath);
		shortestPath->clear();
		return pathVec;
	}
	;
	/*
	 * return the cost of shortest path given two nodes
	 */
	Val path_size(int indexNodeFrom, int indexNodeTo)
	{ //return the path cost associated with the shortest path
		if (shortestPathSize == -1)
			dijkstraImpl(indexNodeFrom, indexNodeTo);
		Val pathCost = shortestPathSize;
		shortestPathSize = -1;
		return pathCost;
	}
	;
	/*
	 * clear up the global list and size
	 */
	void clean()
	{
		shortestPath->clear();
		shortestPathSize = -1;
	}
	/*
	 * calculate the average shortest path cost of a graph
	 */
	float averagePathSize()
	{
		float mean = 0;
		int count = 0;
		for (int i = 1; i < graph->getSizeOfVertices(); i++)
		{
			for (int j = i + 1; j <= graph->getSizeOfVertices(); j++)
			{
				mean += path_size(i, j);
				clean();
				count++;
			}
		}
		return (mean / static_cast<float>(count));
	}
};

#endif /* SHORTESTPATHALGO_H_ */
