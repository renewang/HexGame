/*
 * Graph.h
 *
 *  Created on: Oct 26, 2013
 *      Author: renewang
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <iostream>
#include <iomanip>
#include <iterator>
#include <list>
#include <cstring>

using namespace std;

template<class Type, class Val>
struct Node
{
	int vertexIdex;
	int numOfNeighbors;
	Type vertexVal;
	Val* edges;
	Node* neighbors;
};

template<class Type, class Val>
class Graph
{
	typedef struct Node<Type, Val> Node;

private:
	unsigned numOfVertices; //size of Nodes or Vertex in Graph
	unsigned numOfEdges; //size of Edges in Graph
	float density; //density of connected edges to generate the random Graph
	float distance; //maximal distance to generate random Graph
	list<Node*>* repGraph;
	//vector<Node*>* repGraph; //adjacent list implementation of Graph representation
	Val** repMatrix; //adjacent matrix implementation of Graph representation
	void initGraph()
	{
		numOfEdges = 0;
		distance = 0.0;
		density = 0;
		numOfVertices = 0;
		repGraph = 0;
		repMatrix = 0;
	}
	void initNode(Node* node, int index)
	{
		node->vertexIdex = (index + 1);
		node->numOfNeighbors = 0;
		node->neighbors = new Node[numOfVertices - 1];
		/*Node temp = *(node->neighbors);
		 for (unsigned j = 0; j < numOfVertices - 1; j++)
		 temp[j] = 0;*/
		node->edges = new Val[numOfVertices - 1];
		for (unsigned j = 0; j < numOfVertices - 1; j++)
			node->edges[j] = 0;
	}
	void randomGraphGenerator()
	{
		//initialize random seed with current time
		srand(clock());
		for (typename list<Node*>::iterator iterSelf = repGraph->begin();
				iterSelf != repGraph->end(); ++iterSelf)
		{ // for each vertex
			Node* n = *iterSelf;
			typename list<Node*>::iterator iterNeigh = iterSelf;
			iterNeigh++;
			for (; iterNeigh != repGraph->end(); ++iterNeigh)
			{ // determine if this edge should be generated according to the density
				float prob = (float) rand() / RAND_MAX;
				if (prob <= density)
				{
					//set the neighbor pointer to the neighboring node
					Val val = static_cast<Val>(rand()
							% static_cast<int>(distance) + 1);
					(n->neighbors)[n->numOfNeighbors] =
							*static_cast<Node*>(*iterNeigh);
					*(n->edges + n->numOfNeighbors) = val;

					//undirected graph, need a symmetric assignment

					Node* neigh = *iterNeigh;
					(neigh->neighbors)[neigh->numOfNeighbors] = *(*iterSelf);
					*(neigh->edges + neigh->numOfNeighbors) = val;
					neigh->numOfNeighbors++;

					numOfEdges++;
					n->numOfNeighbors++;
				}
			}
		}
	}
	;
	void toArrayGraphRep()
	{
		//allocate memory
		deallocRepMatrix();

		repMatrix = new Val*[numOfVertices];
		for (unsigned i = 0; i < numOfVertices; i++)
		{
			repMatrix[i] = new Val[numOfVertices];
			memset(repMatrix[i], 0, numOfVertices);
		}

		//transform the representation from list to matrix
		for (typename list<Node*>::iterator iterSelf = repGraph->begin();
				iterSelf != repGraph->end(); ++iterSelf)
		{
			Node* node = static_cast<Node*>(*iterSelf);
			Node* neighs = node->neighbors;
			Val* edges = node->edges;
			int idxOfSelf = node->vertexIdex - 1;
			for (int i = 0; i < node->numOfNeighbors; i++)
			{
				if (neighs == 0 || edges == 0)
					break;
				int idxOfNeigh = neighs->vertexIdex - 1;
				repMatrix[idxOfSelf][idxOfNeigh] = *edges;
				repMatrix[idxOfNeigh][idxOfSelf] = *edges;
				neighs++;
				edges++;
			}
		}
	}
	;
	void toListGraphRep()
	{
		if (repGraph != 0)
			delete repGraph;

		repGraph = new list<Node*>();
		for (unsigned i = 0; i < numOfVertices; i++)
		{
			Node* node = new Node;
			initNode(node, i);
			repGraph->push_back(node);
		}
		for (typename list<Node*>::iterator iterSelf = repGraph->begin();
				iterSelf != repGraph->end(); ++iterSelf)
		{
			Node* node = (*iterSelf);
			typename list<Node*>::iterator iterNeigh = iterSelf;
			iterNeigh++;
			for (unsigned j = node->vertexIdex; j < numOfVertices; j++)
			{
				Node* neigh = *iterNeigh;
				if (repMatrix[node->vertexIdex - 1][j] > 0)
				{
					//assign neighbors to node
					node->neighbors[j - 1] = *neigh;
					*(node->edges + j - 1) = repMatrix[node->vertexIdex - 1][j];
					node->numOfNeighbors++;

					//symmetric assignment to node's neighbor
					neigh->neighbors[neigh->numOfNeighbors] = *node;
					*(neigh->edges + neigh->numOfNeighbors) =
							repMatrix[node->vertexIdex - 1][j];
					neigh->numOfNeighbors++;
				}
				iterNeigh++;
			}
		}
	}
	;
	void deallocRepMatrix()
	{
		if (repMatrix != 0)
		{
			for (unsigned i = 0; i < numOfVertices; i++)
				delete repMatrix[i];
			delete[] repMatrix;
		}

	}
	;
	Node* findNodeByIndex(int index)
	{
		Node* node = 0;
		for (typename list<Node*>::iterator iter = repGraph->begin();
				iter != repGraph->end(); ++iter)
		{
			if ((*iter)->vertexIdex == index)
			{
				node = *iter;
				break;
			}
		}
		return node;
	}
	;
public:
	Graph()
	{ //default constructor
		initGraph();
	}
	;
	Graph(unsigned numOfVertices, float density, float distance)
	{
		initGraph();
		this->numOfVertices = numOfVertices;
		this->density = density;
		this->distance = distance;

		//allocate memory for the list of graph representation
		repGraph = new list<Node*>();
		for (unsigned i = 0; i < numOfVertices; i++)
		{
			Node* n = new Node;
			initNode(n, i);
			repGraph->push_back(n);
		}
		randomGraphGenerator();
	}
	;
	Graph(Val** clientGraph, int numOfVertices)
	{
		initGraph();
		this->repMatrix = clientGraph;
		this->numOfVertices = numOfVertices;
		toListGraphRep();
	}
	inline list<Node*>* getAllVertices()
	{
		return repGraph;
	}
	inline int getSizeOfVertices() //V (G): returns the number of vertices in the graph
	{
		return repGraph->size() == numOfVertices ? numOfVertices : -1;
	}
	;
	inline int getSizeOfEdgues() //E (G): returns the number of edges in the graph
	{
		return numOfEdges;
	}
	;
	bool isAdjacent(int idxOfNodeA, int idxOfNodeB)
	{ //adjacent (G, x, y): tests whether there is an edge from node x to node y.
		Node* nodeA = findNodeByIndex(idxOfNodeA);
		Node* nodeB = findNodeByIndex(idxOfNodeB);

		// TODO duplicate code, needs to be refactored
		Node* nodeSearched, *node, *neighbor;
		if (nodeA->numOfNeighbors < nodeB->numOfNeighbors)
		{
			neighbor = nodeA->neighbors;
			node = nodeA;
			nodeSearched = nodeB;
		}
		else
		{
			neighbor = nodeB->neighbors;
			node = nodeB;
			nodeSearched = nodeA;
		}
		for (int j = 0; j < node->numOfNeighbors; j++)
		{
			if (nodeSearched->vertexIdex == neighbor->vertexIdex)
				return true;
			neighbor++;
		}
		return false;
	}
	;
	vector<int>& getNeighbors(int idxOfNode)
	{ //neighbors (G, x): lists all nodes y such that there is an edge from x to y.
		Node* node = findNodeByIndex(idxOfNode);
		vector<int>* neighIndicesVec = new vector<int>();
		Node* neigh = node->neighbors;
		for (int i = 0; i < node->numOfNeighbors; i++)
		{
			neighIndicesVec->push_back(neigh->vertexIdex);
			++neigh;
		}
		return *neighIndicesVec;
	}
	;
	void addEdge(int indexOfNodeFrom, int indexOfNodeTo)
	{ //add (G, x, y): adds to G the edge from x to y, if it is not there.
		Node* nodeFrom = findNodeByIndex(indexOfNodeFrom);
		Node* nodeTo = findNodeByIndex(indexOfNodeTo);
		nodeFrom->neighbors[nodeFrom->numOfNeighbors] = *nodeTo;
		*(nodeFrom->edges + nodeFrom->numOfNeighbors) = static_cast<Val>(1);
		nodeFrom->numOfNeighbors++;

		//symmetric assignment
		nodeTo->neighbors[nodeTo->numOfNeighbors] = *nodeFrom;
		*(nodeTo->edges + nodeTo->numOfNeighbors) = static_cast<Val>(1);
		nodeTo->numOfNeighbors++;

		numOfEdges++;
	}
	;
	void deleteEdge(int indexOfNodeFrom, int indexOfNodeTo)
	{ //delete (G, x, y): removes the edge from x to y, if it is there.
		if (isAdjacent(indexOfNodeFrom, indexOfNodeTo))
		{
			Node* nodeFrom = findNodeByIndex(indexOfNodeFrom);
			Node* nodeTo = findNodeByIndex(indexOfNodeTo);
			Node* neighbors = nodeFrom->neighbors;
			// TODO modify code
			for (int j = 0; j < nodeFrom->numOfNeighbors; j++)
			{
				if (nodeTo->vertexIdex == neighbors->vertexIdex)
				{
					*(nodeFrom->neighbors + j) = *(nodeFrom->neighbors + j + 1);
					*(nodeFrom->edges + j) = *(nodeFrom->edges + j + 1);
					nodeFrom->numOfNeighbors--;
					for (int k = j + 1; k < nodeFrom->numOfNeighbors; k++)
					{
						*(nodeFrom->neighbors + k) = *(nodeFrom->neighbors + k
								+ 1);
						*(nodeFrom->edges + k) = *(nodeFrom->edges + k + 1);
					}
					//nodeFrom->neighbors[nodeFrom->numOfNeighbors] = *nullNode;
					nodeFrom->edges[nodeFrom->numOfNeighbors] = NULL;
					break;
				}
				neighbors++;
			}

			//symmetric assignment
			neighbors = nodeTo->neighbors;
			for (int j = 0; j < nodeTo->numOfNeighbors; j++)
			{
				if (nodeFrom->vertexIdex == neighbors->vertexIdex)
				{
					*(nodeTo->neighbors + j) = *(nodeTo->neighbors + j + 1);
					*(nodeTo->edges + j) = *(nodeTo->edges + j + 1);
					nodeTo->numOfNeighbors--;
					for (int k = j + 1; k < nodeTo->numOfNeighbors; k++)
					{
						*(nodeTo->neighbors + k) = *(nodeTo->neighbors + k + 1);
						*(nodeTo->edges + k) = *(nodeTo->edges + k + 1);
					}
					//nodeTo->neighbors[nodeTo->numOfNeighbors] = *nullNode;
					nodeTo->edges[nodeTo->numOfNeighbors] = NULL;
					break;
				}
				neighbors++;
			}
			numOfEdges--;
		}
	}
	;
	Type getNodeValue(int indexOfNode)
	{ //get_node_value (G, x): returns the value associated with the node x.
		Node* node = findNodeByIndex(indexOfNode);
		return node->vertexVal;
	}
	;
	void setNodeValue(int indexOfNode, Val value)
	{ // set_node_value(G, x, a): sets the value associated with the node x to a.
		Node* node = findNodeByIndex(indexOfNode);
		node->vertexVal = value;
	}
	;
	Val getEdgeValue(int indexOfNodeFrom, int indexOfNodeTo)
	{ //get_edge_value(G, x, y): returns the value associated to the edge (x,y).
		Val value = NULL;
		if (isAdjacent(indexOfNodeFrom, indexOfNodeTo))
		{
			Node* nodeFrom = findNodeByIndex(indexOfNodeFrom);
			Node* nodeTo = findNodeByIndex(indexOfNodeTo);
			Node* nodeSearched, *node, *neighbor;
			if (nodeFrom->numOfNeighbors < nodeTo->numOfNeighbors)
			{
				neighbor = nodeFrom->neighbors;
				node = nodeFrom;
				nodeSearched = nodeTo;
			}
			else
			{
				neighbor = nodeTo->neighbors;
				node = nodeTo;
				nodeSearched = nodeFrom;
			}
			for (int j = 0; j < node->numOfNeighbors; j++)
			{
				if (nodeSearched->vertexIdex == neighbor->vertexIdex)
					return *(node->edges + j);
				neighbor++;
			}
		}
		return value;
	}
	;
	void setEdgeValue(int indexOfNodeFrom, int indexOfNodeTo, Val value)
	{ //set_edge_value (G, x, y, v): sets the value associated to the edge (x,y) to v.
		if (isAdjacent(indexOfNodeFrom, indexOfNodeTo))
		{
			Node* nodeFrom = findNodeByIndex(indexOfNodeFrom);
			Node* nodeTo = findNodeByIndex(indexOfNodeTo);
			Node* neighbor = nodeFrom->neighbors;
			// TODO duplicate code, needs to be changed
			for (int j = 0; j < nodeFrom->numOfNeighbors; j++)
			{
				if (nodeTo->vertexIdex == neighbor->vertexIdex)
				{
					*(nodeFrom->edges + j) = value;
					break;
				}
				neighbor++;
			}
			neighbor = nodeTo->neighbors;
			for (int j = 0; j < nodeTo->numOfNeighbors; j++)
			{
				if (nodeFrom->vertexIdex == neighbor->vertexIdex)
				{
					*(nodeTo->edges + j) = value;
					break;
				}
				neighbor++;
			}
		}
	}
	;
	virtual ~Graph()
	{
		if (repGraph != 0)
			delete repGraph;
		deallocRepMatrix();
	}
	;
	void printRepGraph()
	{
		cout << "size of vertices = " << repGraph->size() << "\n";
		toArrayGraphRep();

		cout << setw(9) << " ";
		for (unsigned i = 0; i < numOfVertices; i++)
			cout << i + 1 << setw(5) << " ";
		cout << "\n";
		for (unsigned i = 0; i < numOfVertices; i++)
		{
			cout << setw(5) << i + 1;
			for (unsigned j = 0; j < numOfVertices; j++)
			{
				cout << setw(5) << repMatrix[i][j] << " ";
			}
			cout << "\n";
		}
	}
	;
//friend ostream& operator<<(ostream& os, const Graph<Type, Val>& g);
};
/*
 template<class Type, class Val>
 ostream& operator<<(ostream& os, const Graph<Type, Val>& g)
 {
 for (typename list<Node<Type, Val> >::iterator iterSelf =
 g.repGraph->begin(); iterSelf != g.repGraph->end(); ++iterSelf)
 {
 cout << ((Node<Type, Val> ) (*iterSelf)).vertexIdex << "\n";
 }
 return os;
 }*/
#endif /* GRAPH_H_ */
