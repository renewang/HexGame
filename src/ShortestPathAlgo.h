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

using namespace std;

template<class Type, class Val>
class ShortestPathAlgo
{
	typedef Graph<Type, Val> Graph;
	typedef Node<Type, Val> Node;

private:
	Graph* graph;
	list<Node*>* shortestPath;
	Val shortestPathSize;

	/*
	 * actual dijkstra algorithm implementation
	 */
	void dijkstraImpl(int indexOfSource, int indexOfDest)
	{
		const Val INF = numeric_limits<Val>::max();
		vector<Node*>* close = new vector<Node*>; //visited
		const vector<Node*>* vertices = graph->getAllVertices();

		list<Node*>* path = new list<Node*>;
		Node* source = vertices->at(indexOfSource - 1);

		close->push_back(source);
		path->push_back(source);

		Val* distance = new Val[vertices->size()];
		bool* visited = new bool[vertices->size()]; //marked if the nodes have been visited (put in close set already)
		int* hop = new int[vertices->size()];

		for (unsigned i = 0; i < vertices->size(); i++)
		{
			distance[i] = INF;
			visited[i] = false;
			hop[i] = 0;
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
					hop[(*iterNeigh)->vertexIdex - 1]++;
				}
			}
			/* TODO update here with a priority queue
			 * find the min value of current outgoing edges from close set.
			 */
			Val curMin = INF;
			int curMinIndex = -1;
			for (unsigned i = 0; i < vertices->size(); i++)
			{
				if (!visited[i] && distance[i] < curMin)
				{
					curMin = distance[i];
					curMinIndex = i;
				}
			}
			min = curMin;
			visited[curMinIndex] = true;
			current = vertices->at(curMinIndex);
			if (hop[curMinIndex] < static_cast<int>(close->size()))
				path->pop_back();
			close->push_back(current);
			path->push_back(current);
		}

		shortestPath = path;
		shortestPathSize = min;

		delete close;
		delete[] visited;
		delete[] distance;
		delete[] hop;
	}
	;

public:
	/*
	 * default constructor
	 */
	ShortestPathAlgo() :
			graph(NULL), shortestPath(0), shortestPathSize(-1)
	{
	}
	;
	/*
	 * constructor based on the given graph
	 */
	ShortestPathAlgo(Graph* graph) :
			graph(graph), shortestPath(0), shortestPathSize(-1)
	{
	}
	;
	/*
	 * destructor
	 */
	virtual ~ShortestPathAlgo()
	{
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
		if (shortestPath == 0)
			dijkstraImpl(indexNodeFrom, indexNodeTo);
		list<Node*>* pathVec = new list<Node*>(*shortestPath);
		delete shortestPath;
		shortestPath = 0;
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
	 * calculate the average shortest path cost of a graph
	 */
	//TODO implementation needs to be clarified
	float averagePathSize()
	{
		float mean = 0;
		int count = 0;
		for (int i = 1; i < graph->getSizeOfVertices(); i++)
		{
			for (int j = i + 1; j <= graph->getSizeOfVertices(); j++)
			{
				mean += path_size(i, j);
				count++;
			}
		}
		return (mean / static_cast<float>(count));
	}
};

#endif /* SHORTESTPATHALGO_H_ */
