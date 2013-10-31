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

#define INF 1000000;

using namespace std;

template<class Type, class Val>
class ShortestPathAlgo
{
	typedef Graph<Type, Val> Graph;
	typedef Node<Type, Val> Node;

private:
	Graph* graph;

	list<Node*>*
	dijkstraImpl(int indexOfSource, int indexOfDest)
	{
		vector<Node*>* close = new vector<Node*>; //visited
		list<Node*>* v = graph->getAllVertices();
		vector<Node*> vertices;
		// TODO need to fix copy constructor
		for (typename list<Node*>::iterator iter = v->begin(); iter != v->end();
				++iter)
		{
			vertices.push_back(*iter);
		}
		list<Node*>* path = new list<Node*>;
		Node* source = vertices.at(indexOfSource - 1);

		//1. put the source in the close and all the its neighbors in the open
		close->push_back(source);
		path->push_back(source);

		//2. start the iterations to put the nodes in open set into close set
		Val* distance = new Val[vertices.size()];
		bool* visited = new bool[vertices.size()];
		int* hop = new int[vertices.size()];

		for (unsigned i = 0; i < vertices.size(); i++)
		{
			distance[i] = INF
			;
			visited[i] = false;
			hop[i] = 0;
		}
		for (unsigned i = 0; i < vertices.size(); i++)

			visited[source->vertexIdex - 1] = true;

		Val min = static_cast<Val>(0);
		Node* current = source;
		while (close->size() < vertices.size()
				&& current->vertexIdex != indexOfDest)
		{
			Node* neigh = current->neighbors;
			for (int j = 0; j < current->numOfNeighbors; j++)
			{
				Val edgeVal = min + *(current->edges + j);
				if (edgeVal < distance[neigh->vertexIdex - 1]
						&& !visited[neigh->vertexIdex - 1])
				{
					distance[neigh->vertexIdex - 1] = edgeVal;
					hop[neigh->vertexIdex - 1]++;
				}
				neigh++;
			}
			/* TODO update here with a priority queue
			 * find the min value of current outgoing edges from close set.
			 */
			Val curMin = INF
			;
			int curMinIndex = -1;
			for (unsigned i = 0; i < vertices.size(); i++)
			{
				if (!visited[i] && distance[i] < curMin)
				{
					curMin = distance[i];
					curMinIndex = i;
				}
			}
			min = curMin;
			visited[curMinIndex] = true;
			current = vertices.at(curMinIndex);
			if (hop[curMinIndex] < static_cast<int>(close->size()))
				path->pop_back();
			close->push_back(current);
			path->push_back(current);
		}
		delete close;
		delete[] visited;
		delete[] distance;
		delete[] hop;
		return path;
	}
	;

public:
	ShortestPathAlgo();
	ShortestPathAlgo(Graph* graph) :
			graph(graph)
	{
	}
	;
	virtual ~ShortestPathAlgo()
	{
	}
	;

	void
	vertices(list<Node*> vertices); //list of vertices in G(V,E).
	list<Node*>*
	path(int indexNodeFrom, int indexNodeTo)
	{
		//find shortest path between u-w and returns the sequence of vertices representing shorest path u-v1-v2-É-vn-w.
		return dijkstraImpl(indexNodeFrom, indexNodeTo);
	}
	;
	Val path_size(int indexNodeFrom, int indexNodeTo)
	{ //return the path cost associated with the shortest path
		list<Node*>* path = dijkstraImpl(indexNodeFrom, indexNodeTo);
		Val pathSize = static_cast<Val>(0);
		int from = indexNodeFrom;
		for (typename list<Node*>::iterator iter = path->begin();
				iter != path->end(); ++iter)
		{
			iter++;
			pathSize += graph->getEdgeValue(from, (*iter)->vertexIdex);
			from = (*iter)->vertexIdex;
		}
		return pathSize;
	}
	;
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
