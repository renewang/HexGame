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
	unsigned numOfNeighbors;
	Type vertexVal;
	list<Val>* edges;
	list<Node*>* neighbors;
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
	vector<Node*>* repGraph; //adjacent list implementation of Graph representation
	Val** repMatrix; //adjacent matrix implementation of Graph representation
	Val minDistance;

	void initGraph()
	{
		numOfEdges = 0;
		distance = 0.0;
		density = 0;
		numOfVertices = 0;
		repGraph = 0;
		repMatrix = 0;
		minDistance = static_cast<Val>(1);
	}
	;
	void initNode(Node* node, int index)
	{
		node->vertexIdex = (index + 1);
		node->numOfNeighbors = 0;
		node->neighbors = new list<Node*>;
		node->edges = new list<Val>;
	}
	;
	void randomGraphGenerator()
	{
		//initialize random seed with current time
		srand(clock());

		for (typename vector<Node*>::iterator iterSelf = repGraph->begin();
				iterSelf != repGraph->end(); ++iterSelf)
		{ // for each vertex
			Node* n = *iterSelf;
			typename vector<Node*>::iterator iterNeigh = iterSelf;
			iterNeigh++;
			for (; iterNeigh != repGraph->end(); ++iterNeigh)
			{ // determine if this edge should be generated according to the density
				float prob = (float) rand() / RAND_MAX;
				assert(prob > 0 && prob < 1);

				if (prob <= density)
				{
					//set the neighbor pointer to the neighboring node, the minimal distance is 1
					Val val = static_cast<Val>(rand()
							% static_cast<int>(distance) + minDistance);

					assert(val >= minDistance && val <= distance);

					(n->neighbors)->push_back(*iterNeigh);
					(n->edges)->push_back(val);

					//undirected graph, need a symmetric assignment
					Node* neigh = *iterNeigh;
					(neigh->neighbors)->push_back(*iterSelf);
					(neigh->edges)->push_back(val);

					numOfEdges++;
				}
			}
			n->numOfNeighbors = (n->neighbors->size());
			assert(
					n->numOfNeighbors >= 0 && n->numOfNeighbors < numOfVertices
							&& n->numOfNeighbors == n->edges->size());
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
		for (typename vector<Node*>::iterator iterSelf = repGraph->begin();
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
	{ //given a adjacent matrix representation of graph G, transform it to adjacent list
		if (repGraph != 0)
			delete repGraph;

		repGraph = new vector<Node*>();
		for (unsigned i = 0; i < numOfVertices; i++)
		{
			Node* node = new Node;
			initNode(node, i);
			repGraph->push_back(node);
		}
		for (typename vector<Node*>::iterator iterSelf = repGraph->begin();
				iterSelf != repGraph->end(); ++iterSelf)
		{
			Node* node = (*iterSelf);
			typename vector<Node*>::iterator iterNeigh = iterSelf;
			iterNeigh++;
			for (unsigned j = node->vertexIdex; j < numOfVertices; j++)
			{
				Node* neigh = *iterNeigh;
				if (repMatrix[node->vertexIdex - 1][j] > 0)
				{
					//assign neighbors to node
					(node->neighbors)->push_back(neigh);
					(node->edges)->push_back(
							repMatrix[node->vertexIdex - 1][j]);

					//symmetric assignment to node's neighbor for undirected graph
					(neigh->neighbors)->push_back(node);
					(neigh->edges)->push_back(
							repMatrix[node->vertexIdex - 1][j]);
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
		repMatrix = 0;
	}
	;
	/*
	 * to provide the functionality to retrieve node in repGraph
	 */
	Node* findNodeByIndex(int index)
	{
		assert(index > 0 && index <= static_cast<int>(numOfVertices));
		return repGraph->at(index - 1);
	}
	;
	/*
	 * to provide the functionality to retrieve node of neighbors
	 */
	Node* findNeighborByIndex(Node* node, int index)
	{
		list<Node*>* neighbors = node->neighbors;
		for (typename list<Node*>::iterator iter = neighbors->begin();
				iter != neighbors->end(); ++iter)
		{
			if (index == (*iter)->vertexIdex)
				return (*iter);
		}
		return NULL;
	}
	Val findEdgeByIndex(Node* node, int index)
	{
		list<Node*>* neighbors = node->neighbors;
		list<Val>* edges = node->edges;
		typename list<Node*>::iterator iterNeigh = neighbors->begin();
		typename list<Val>::iterator iterEdge = edges->begin();
		for (; iterNeigh != neighbors->end(); ++iterNeigh, ++iterEdge)
		{
			if (index == (*iterNeigh)->vertexIdex)
			{
				return (*iterEdge);
			}
		}
		return NULL;
	}
public:
	/*
	 * default constructor, initialize graph with zero values
	 * Takes no arguments
	 */
	Graph()
	{
		initGraph();
	}
	;
	/*
	 * constructor to generate random graph
	 */
	Graph(unsigned numOfVertices, float density, float distance)
	{
		//initialize the graph with zero values
		initGraph();
		this->numOfVertices = numOfVertices;
		this->density = density;
		this->distance = distance;

		//allocate memory for the list of graph representation
		repGraph = new vector<Node*>();
		for (unsigned i = 0; i < numOfVertices; i++)
		{
			Node* n = new Node;
			initNode(n, i);
			repGraph->push_back(n);
		}
		randomGraphGenerator();
	}
	;
	/*
	 * constructor to generate a graph according to matrix provided by client
	 */
	Graph(Val** clientGraph, int numOfVertices)
	{
		initGraph();
		this->repMatrix = clientGraph;
		this->numOfVertices = numOfVertices;
		toListGraphRep();
	}
	/*
	 * copy constructor
	 */
	/*Graph(Graph& graph){

	 }*/
	/*
	 * get the underlying list representation of vertices.
	 */
	inline const vector<Node*>* getAllVertices()
	{
		return repGraph;
	}
	/*
	 * get the size of vertices
	 */
	inline int getSizeOfVertices() //V (G): returns the number of vertices in the graph
	{
		return repGraph->size() == numOfVertices ? numOfVertices : -1;
	}
	;
	/*
	 * get the size of edges
	 */
	inline int getSizeOfEdges() //E (G): returns the number of edges in the graph
	{
		return numOfEdges;
	}
	;
	/*
	 * test if two nodes is adjacent
	 */
	bool isAdjacent(int idxOfNodeA, int idxOfNodeB)
	{ //adjacent (G, x, y): tests whether there is an edge from node x to node y.
		Node* nodeA = findNodeByIndex(idxOfNodeA);
		Node* nodeB = findNodeByIndex(idxOfNodeB);

		Node* nodeSearched, *node;

		nodeSearched =
				nodeA->numOfNeighbors < nodeB->numOfNeighbors ? nodeB : nodeA;
		node = nodeA->numOfNeighbors < nodeB->numOfNeighbors ? nodeA : nodeB;

		return nodeSearched
				== findNeighborByIndex(node, nodeSearched->vertexIdex);
	}
	;
	/*
	 * return the neighbors of a node
	 */
	vector<int>& getNeighbors(int idxOfNode)
	{ //neighbors (G, x): lists all nodes y such that there is an edge from x to y.
		Node* node = findNodeByIndex(idxOfNode);
		vector<int>* neighIndicesVec = new vector<int>;
		list<Node*>* neigh = node->neighbors;
		for (typename list<Node*>::iterator iter = neigh->begin();
				iter != neigh->end(); ++iter)
			neighIndicesVec->push_back((*iter)->vertexIdex);
		return *neighIndicesVec;
	}
	;
	/*
	 * add Edge between the two specified nodes with the specified value
	 */
	void addEdge(int indexOfNodeFrom, int indexOfNodeTo, Val value)
	{ //add (G, x, y): adds to G the edge from x to y, if it is not there.
		Node* nodeFrom = findNodeByIndex(indexOfNodeFrom);
		Node* nodeTo = findNodeByIndex(indexOfNodeTo);

		(nodeFrom->neighbors)->push_back(nodeTo);
		(nodeFrom->edges)->push_back(value);
		nodeFrom->numOfNeighbors++;

		//symmetric assignment
		(nodeTo->neighbors)->push_back(nodeFrom);
		(nodeTo->edges)->push_back(value);

		nodeTo->numOfNeighbors++;

		numOfEdges++;
	}
	;
	/*
	 * delete the edge between specified nodes
	 */
	void deleteEdge(int indexOfNodeFrom, int indexOfNodeTo)
	{ //delete (G, x, y): removes the edge from x to y, if it is there.
		if (isAdjacent(indexOfNodeFrom, indexOfNodeTo))
		{
			Node* nodeFrom = findNodeByIndex(indexOfNodeFrom);
			Node* nodeTo = findNodeByIndex(indexOfNodeTo);

			list<Node*>* neighbors = nodeFrom->neighbors;
			list<Val>* edges = nodeFrom->edges;

			typename list<Node*>::iterator iterNeigh = neighbors->begin();
			typename list<Val>::iterator iterEdge = edges->begin();

			for (; iterNeigh != neighbors->end(); ++iterNeigh, ++iterEdge)
			{
				if (nodeTo->vertexIdex == (*iterNeigh)->vertexIdex)
				{
					neighbors->erase(iterNeigh);
					edges->erase(iterEdge);
					nodeFrom->numOfNeighbors--;
					break;
				}
			}

			//symmetric assignment
			neighbors = nodeTo->neighbors;
			edges = nodeTo->edges;
			iterNeigh = neighbors->begin();
			iterEdge = edges->begin();

			for (; iterNeigh != neighbors->end(); ++iterNeigh, ++iterEdge)
			{
				if (nodeFrom->vertexIdex == (*iterNeigh)->vertexIdex)
				{
					neighbors->erase(iterNeigh);
					edges->erase(iterEdge);
					nodeTo->numOfNeighbors--;
					break;
				}
				neighbors++;
			}
			numOfEdges--;
		}
	}
	;
	/*
	 * get value of the Node
	 */
	Type getNodeValue(int indexOfNode)
	{ //get_node_value (G, x): returns the value associated with the node x.
		Node* node = findNodeByIndex(indexOfNode);
		if (node != 0)
			return node->vertexVal;
		else
			return NULL;
	}
	;
	/*
	 * set value of the Node
	 */
	void setNodeValue(int indexOfNode, Val value)
	{ // set_node_value(G, x, a): sets the value associated with the node x to a.
		Node* node = findNodeByIndex(indexOfNode);
		node->vertexVal = value;
	}
	;
	/*
	 * get edge value between specified two nodes
	 */
	Val getEdgeValue(int indexOfNodeFrom, int indexOfNodeTo)
	{ //get_edge_value(G, x, y): returns the value associated to the edge (x,y).
		Val value = NULL;
		if (isAdjacent(indexOfNodeFrom, indexOfNodeTo))
		{
			Node* nodeFrom = findNodeByIndex(indexOfNodeFrom);
			Node* nodeTo = findNodeByIndex(indexOfNodeTo);
			Node* nodeSearched, *node;
			if (nodeFrom->numOfNeighbors < nodeTo->numOfNeighbors)
			{
				node = nodeFrom;
				nodeSearched = nodeTo;
			}
			else
			{
				node = nodeTo;
				nodeSearched = nodeFrom;
			}

			return findEdgeByIndex(node, nodeSearched->vertexIdex);
		}
		return value;
	}
	;
	/*
	 * set the edge between specified two nodes with the specified value
	 */
	void setEdgeValue(int indexOfNodeFrom, int indexOfNodeTo, Val value)
	{ //set_edge_value (G, x, y, v): sets the value associated to the edge (x,y) to v.
		if (isAdjacent(indexOfNodeFrom, indexOfNodeTo))
		{
			Node* nodeFrom = findNodeByIndex(indexOfNodeFrom);
			Node* nodeTo = findNodeByIndex(indexOfNodeTo);

			list<Node*>* neighbors = nodeFrom->neighbors;
			list<Val>* edges = nodeFrom->edges;
			typename list<Node*>::iterator iterNeigh = neighbors->begin();
			typename list<Val>::iterator iterEdge = edges->begin();

			for (; iterNeigh != neighbors->end(); ++iterNeigh, ++iterEdge)
			{
				if (nodeTo->vertexIdex == (*iterNeigh)->vertexIdex)
				{
					(*iterEdge) = value;
					break;
				}
			}

			//symmetric assignment
			neighbors = nodeTo->neighbors;
			edges = nodeTo->edges;
			iterNeigh = neighbors->begin();
			iterEdge = edges->begin();

			for (; iterNeigh != neighbors->end(); ++iterNeigh, ++iterEdge)
			{
				if (nodeFrom->vertexIdex == (*iterNeigh)->vertexIdex)
				{
					(*iterEdge) = value;
					break;
				}
			}
		}
	}
	;
	/*
	 * deconstructor
	 */
	virtual ~Graph()
	{
		if (repGraph != 0)
			delete repGraph;
		//deallocRepMatrix();
	}
	;
	/*
	 * to print underlying graph matrix representation
	 */
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

};
#endif /* GRAPH_H_ */
