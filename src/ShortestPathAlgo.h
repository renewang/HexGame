/*
 * ShortestPathAlgo.h
 *
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
	Graph& graph;
	list<Node> shortestPath;
	Val shortestPathSize;

	/*
	 * actual dijkstra algorithm implementation
	 */
	void dijkstraImpl(int indexOfSource, int indexOfDest)
	{
		const vector<Node> vertices = graph.getAllVertices(); //get the all vertices in graph
		vector<Node*> close; //visited or close set
		PriorityQueue open; //unvisited or open set
		const Node* source = &(vertices[indexOfSource - 1]); //get the source node
		const Val INF = numeric_limits<Val>::max(); //set the INF as the maximal value of the type
		Val* distance = new Val[vertices.size()]; //store the min distance between source to any node
		bool* visited = new bool[vertices.size()]; //marked if the nodes have been visited (put in close set already)
		int* prevNode = new int[vertices.size()]; //store the previous node in path of the min distance between source to any node

		//initialize all temporary arrays provided for calculation and PriorityQueue with all infinity priority
		for (unsigned i = 0; i < vertices.size(); i++)
		{
			if ((i + 1) == static_cast<unsigned>(source->vertexIdex))
			{
				visited[source->vertexIdex - 1] = true;
				distance[source->vertexIdex - 1] = 0;
			}
			else
			{
				visited[i] = false;
				distance[i] = INF;
				open.insert(&(vertices[i]), INF);
				prevNode[i] = -1;
			}
		}
		close.push_back(const_cast<Node*>(source)); //initialize the close set with source node

		Val min = static_cast<Val>(0);
		Node* current = const_cast<Node*>(source);

		/*
		 * terminate condition: all vertices are visited. close set has size = graph's node size or
		 * the target/destination node has been reached
		 */
		while (close.size() < vertices.size()
				&& current->vertexIdex != indexOfDest)
		{
			list<Node*> neighbors = current->neighbors;
			list<Val> edges = current->edges;
			typename list<Node*>::iterator iterNeigh = neighbors.begin();
			typename list<Val>::iterator iterEdge = edges.begin();

			for (; iterNeigh != neighbors.end(); ++iterNeigh, ++iterEdge)
			{
				Val edgeVal = min + *(iterEdge);

				if (edgeVal < distance[(*iterNeigh)->vertexIdex - 1]
						&& !visited[(*iterNeigh)->vertexIdex - 1])
				{
					distance[(*iterNeigh)->vertexIdex - 1] = edgeVal;
					prevNode[(*iterNeigh)->vertexIdex - 1] =
							current->vertexIdex;
					open.chgPrioirity((*iterNeigh), edgeVal);
				}
			}

			current = const_cast<Node*>(open.minPrioirty());
			min = distance[current->vertexIdex - 1];
			visited[current->vertexIdex - 1] = true;
			close.push_back(current);
		}
		if (min != INF)
			shortestPathSize = min;
		else
			shortestPathSize = 0; //unreachable

		//trace back the shortest path
		while (current->vertexIdex != source->vertexIdex
				&& prevNode[current->vertexIdex - 1] > 0)
		{
			shortestPath.push_front(*current);
			current = const_cast<Node*>(&(vertices[prevNode[current->vertexIdex
					- 1] - 1]));
		}
		shortestPath.push_front(*source);

		//free memory
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
	}
	;
	/*
	 * constructor based on the given graph
	 */
	ShortestPathAlgo(Graph& graph) :
			graph(graph), shortestPathSize(-1)
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
	 * return the shortest path given two nodes
	 */
	list<Node> path(int indexNodeFrom, int indexNodeTo)
	{
		//find shortest path between u-w and returns the sequence of vertices representing shorest path u-v1-v2-É-vn-w.
		if (shortestPath.size() == 0)
			dijkstraImpl(indexNodeFrom, indexNodeTo);
		list<Node> pathVec(shortestPath);
		shortestPath.clear();
		return list<Node>(pathVec);
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
		shortestPath.clear();
		shortestPathSize = -1;
	}
	/*
	 * calculate the average shortest path cost of a graph
	 */
	double averageAllPathSize()
	{
		double mean = 0;
		int count = 0;
		for (int i = 1; i < graph.getSizeOfVertices(); i++)
		{
			for (int j = i + 1; j <= graph.getSizeOfVertices(); j++)
			{
				float minPathSize = path_size(i, j);
				if (minPathSize > 0)
				{
					mean += minPathSize;
					count++;
				}
				clean();
			}
		}
		return (mean / static_cast<double>(count));
	}
	/*
	 * calculate the average shortest path cost of a given node
	 */
	double averagePathSize(int indexOfSource)
	{
		float mean = 0;
		int count = 0;
		for (int j = 1; j <= graph.getSizeOfVertices(); j++)
		{
			float minPathSize = path_size(indexOfSource, j);
			if (minPathSize > 0)
			{
				mean += minPathSize;
				count++;
			}
			clean();
		}
		return (mean / static_cast<double>(count));
	}
};

#endif /* SHORTESTPATHALGO_H_ */
