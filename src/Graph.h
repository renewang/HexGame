/*
 * Graph.h
 * This file defines a Graph data structure which holds a graph's vertices and edges.
 * Also implement Monte Carlo simulation to generate  a random graph given the probability and distance range
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <map>
#include <list>
#include <vector>
#include <string>
#include <utility>
#include <iomanip>
#include <sstream>
#include <typeinfo>
#include <iterator>
#include <iostream>
#include <algorithm>

#include <cstring>
#include <cassert>
#include <cstdlib>

//#include "Global.h"
#include "AbstractParser.h"

/*
 * Graph Class is used as a representation of graph. It contain three constructors:
 * Graph(): default constructor which simply a empty graph with all its members initialized as default value.
 * Graph(unsigned, float, float): which generate a random graph according to Monte Carlo simulation. Sample usage:
 *
 * Graph<string, double> randomG(5, 0.5, 10);
 *
 * Above will generate a random graph with 5 nodes based on density = 0.5 and the edges' weight is within the range [0 ,10]
 *
 * Graph(Val**, int): which takes an adjacent matrix and transform it to a Graph object. Sample usage:
 * double ** testDouble = new double*[6];
 *   for (unsigned i = 0; i < 6; i++)
 *     testDouble[i] = new double[6];
 *
 *   for (unsigned k = 0; k < 6; k++)
 *   {
 *     for (unsigned i = 0; i < (6 - k); i++)
 *     {
 *       testDouble[i][i + k] = k * k;
 *       if (k != 0)
 *         testDouble[i + k][i] = k * k;
 *     }
 *   }
 * Graph<string, double> testDG(testDouble, 6);
 *
 * Graph(PlainParser&): constructor which read graph from file
 * Sample usage:
 * string filename = "Smalldata.txt";
 * PlainParser parser(filename);
 * Graph<string, int> testgraph(parser);
 *
 * Graph(unsigned): constructor which creates a unconnected graph with the specified number of vertices.
 * Sample usage:
 * unsigned sizeoftestgraph = 6;
 * Graph<int, int> emptygraph(sizeoftestgraph);
 *
 * Graph(const Graph&): copy constructor
 */

//TODO add directed graph support and correct edge calculations
template<class Type, class Val>
class Graph {
 protected:
  /* Edge structure is used to hold the edge information which contains:
   * indexoffromnode: vertexindex of the source node
   * indexoftonode: vertexindex of the destination node
   * weight: the weight of the edge
   *
   * To generate an Edge and store in the neighbors of a node, the sample usage is as following;
   * Node<string, double> node;
   * Edge edge;
   * edge.indexoffromnode = node->vertexindex;
   * edge.indexoftonode = node->vertexindex;
   * edge.weight = val;
   * (node->neighbors).push_back(edge);
   */
  struct Edge {
    int indexoffromnode;
    int indexoftonode;
    Val weight;
  };
  /* Node structure is used to hold the vertex information which contains:
   * vertexindex: a unique identifier starting from 1
   * numofneighbors: number of out edges
   * vertexvalue: label of vertex or node
   * neighbors: the list to store neighbors' information
   *
   * Generate a Node by using string type as its label. The sample usage of initialization is as following:
   * Node<string, double> node;
   *
   */
  struct Node {
    int vertexindex;  //an unique identifier starting from 1 to the number of vertices, automatically assigned when constructing the graph class
    unsigned numofneighbors;  //the number of connected vertices
    Type vertexvalue;  //the label or value of the vertex which should be descriptive such as string. But users are free to specify their own type in template syntax.
    //a double linked list to store the weight of the connected edge.
    //The type of edges are user defined but recommend to restrict to numeric type such as double, float and integer
    std::list<Edge> neighbors;
  };

  void setRepgraph(const std::vector<Node>& repgraph) {
    this->repgraph = repgraph;
  }
  inline std::vector<Node>& getRepgraph(){
    return repgraph;
  }
  inline void setSizeOfEdges(unsigned numofedges) {
    this->numofedges = numofedges;
  }
  inline void setSizeOfVertices(unsigned numofvertices) {
    this->numofvertices = numofvertices;
  }
  //Provide the functionality to retrieve node's pointer given the index of source node
  virtual inline const Node* findNodeByIndex(int index) const {
    assert(index >= 1 && index <= static_cast<int>(numofvertices));
    return &(repgraph[index - 1]);
  }
  //non-const this version
  virtual inline Node* findNodeByIndex(int index){
    assert(index >= 1 && index <= static_cast<int>(numofvertices));
    return &(repgraph[index - 1]);
  }
  //Private function to initialize the Node in constructors
  virtual void initNode(Node& node, int index) {
    node.vertexindex = (index + 1);
    node.numofneighbors = 0;
  }
  ;
 private:
  float density;  //density of connected edges to generate the random Graph
  float distance;  //maximal distance to generate random Graph
  Val mindistance;  //hold the minimal distance which should equal to 1
  std::vector<Node> repgraph;  //adjacent list implementation of Graph representation
  std::vector<std::vector<Val> > repmatrix;  //adjacent matrix implementation of Graph representation
  bool isundirected;  //indicator for a undirected graph
  unsigned numofvertices;  //size of Nodes or Vertex in Graph
  unsigned numofedges;  //size of Edges in Graph

  //Private function to initialize the members of Graph in constructors
  virtual void initGraph();
  //Transform underlying representation of the graph from adjacent matrix to linked list
  void toArrayGraphRep();
  //Transform underlying representation of the graph from adjacent linked list to matrix
  void toListGraphRep();
  //Provide the functionality to retrieve the Node pointer of a connected node given the source node object
  //and the index of connected node
  int findNeighborByIndex(const Node* node, int index) const;
  //Provide the functionality to retrieve edge of a neighboring node given the source node object
  //and the index of connected node
  Val findEdgeByIndex(const Node* node, int index) const;
  //Provide the functionality to traverse the tree by Depth First Search (DFS) method
  //Loop is also detected in this fashion when a back link is found or a non-parental node is revisited in DFS.
  bool TraverseDFS(int indexofcurrent, bool* visited, int indexofparent = -1,
                   std::stringstream* strstream = 0) {
    //Provide the functionality to traverse the tree by Depth First Search (DFS) method
    //Loop is also detected in this fashion when a back link is found or a non-parental node is revisited in DFS.
    bool isLoopExisting = false;
    visited[indexofcurrent - 1] = true;
    int numofneighbors = getNeighborsSize(indexofcurrent);
    std::vector<int> neighwoparents;

    //discard the parental link
    if (numofneighbors != 0) {
      std::vector<int> neighbors = getNeighbors(indexofcurrent);
      for (std::vector<int>::iterator iter = neighbors.begin();
          iter != neighbors.end(); ++iter)
        if (*iter != indexofparent)
          neighwoparents.push_back(*iter);
      numofneighbors = neighwoparents.size();
    }

    if (numofneighbors > 0) {
      if (strstream != 0)
        *strstream << '(';

      for (unsigned i = 0; i < neighwoparents.size(); i++) {
        if (strstream != 0)
          *strstream << neighwoparents[i];

        isLoopExisting = visited[neighwoparents[i] - 1];

        if (!isLoopExisting)
          isLoopExisting = TraverseDFS(neighwoparents[i], visited,
                                       indexofcurrent, strstream);

        if (isLoopExisting)
          break;

        if (strstream != 0) {
          if (i < (neighwoparents.size() - 1))
            *strstream << ',';
        }
      }
      if (strstream != 0)
        *strstream << ')';
    }
    return isLoopExisting;
  }
  ;
  void TraverseDFS(int indexofcurrent, bool* visited, int indexofparent,
                   std::vector<int>* subgraph) {
    visited[indexofcurrent - 1] = true;
    subgraph->push_back(indexofcurrent);
    int numofneighbors = getNeighborsSize(indexofcurrent);

    //discard the parental link
    std::vector<int> neighwoparents;
    if (numofneighbors != 0) {
      std::vector<int> neighbors = getNeighbors(indexofcurrent);
      for (std::vector<int>::iterator iter = neighbors.begin();
          iter != neighbors.end(); ++iter)
        if (*iter != indexofparent)
          neighwoparents.push_back(*iter);
      numofneighbors = neighwoparents.size();
    }

    if (numofneighbors > 0) {
      for (unsigned i = 0; i < neighwoparents.size(); i++)
        TraverseDFS(neighwoparents[i], visited, indexofcurrent, subgraph);
    }
    return;
  }
  ;
  //Implementation of Monte Carlo simulation to generate undirected graph.
  void randomGraphGenerator();
 public:
//Default constructor, initialize graph with zero values
//Takes no arguments
  Graph() {
    initGraph();
  }
  ;
//Constructor to generate a random graph
  Graph(unsigned numofvertices, float density, float distance);
//Constructor to generate a graph according to the matrix provided by client
  Graph(Val** clientgraph, int numofvertices);
//Constructor to generate a graph according to the input file
  explicit Graph(AbstractParser& parser);
//Create a empty graph
  explicit Graph(unsigned numofvertices);
//destructor
  virtual ~Graph() {
  }
  ;
//Get the size of vertices
  virtual inline int getSizeOfVertices() const {
    return repgraph.size() == numofvertices ? numofvertices : -1;
  }
  ;
//Get the size of edges
  virtual inline int getSizeOfEdges() const {
    return numofedges;
  }
  ;
//Test if two nodes is adjacent
//Input:
//idxofnodea: the vertexindex of first node
//idxofnodeb: the vertexindex of second node
//Output:
//The boolean variable to indicate if the specified two nodes are connected.
//TRUE: is connected or at adjacency
//FALSE: is not connected or not at adjacency
  bool isAdjacent(int idxofnodefrom, int idxofnodeto) const;
//Return the neighbors of a node
//Input:
//idxofnode: the vertexindex of the inquiring node
//Output:
//The vector which stores the vertexindices of the neighbors of the inquiring node
  std::vector<int> getNeighbors(int idxofnode) const;
//Return the edges of neighbors of a node
//Input:
//idxofnode: the vertexindex of the inquiring node
//Output:
//The list which stores the edges of the neighbors of the inquiring node
  const std::vector<Val> getNeighborsEdgeValues(int idxofnode) const;
//Return the number of neighbors of a node
//Input:
//idxofnode: the vertexindex of the inquiring node
//Output:
//The size of connected neighbors
  int getNeighborsSize(int idxofnode);
//Add Edge between the two specified nodes with the specified value
//Input:
//indexofsource: the vertexindex of the source node
//indexofdest: the vertexindex of the destination node
//value: the weight of intending adding edge
//Output: NONE
  void addEdge(int indexofsource, int indexofdest, Val value);
//Delete the edge between the specified nodes
//Input:
//indexofnodefrom: the vertexindex of the source node
//indexofnodeto, the vertexindex of the destination node
//Output: None
  void deleteEdge(int indexofnodefrom, int indexofnodeto);
//Get the value of the Node
//Input:
//indexofnode: the vertexindex of the inquiring node
//Output:
//The label or representing value of the inquiring node
  Type getNodeValue(int indexofnode) const {
    const Node* node = findNodeByIndex(indexofnode);
    return node->vertexvalue;
  }
  ;
//Set value of the Node
//Input:
//indexofnode: the vertexindex of the inquiring node
//value: The label or the representing value of the inquiring node
//Output: None
  virtual void setNodeValue(int indexofnode, Type value) {
    Node* node = findNodeByIndex(indexofnode);
    node->vertexvalue = value;
  }
  ;
//Get edge value between specified two nodes
//Input:
//indexofnodefrom: the vertexindex of the source node
//indexOfnodeto: the vertexindex of the destination node
//Output:
//Return the weight of edge between the input nodes
  Val getEdgeValue(int indexofnodefrom, int indexofnodeto) const;
//Set the edge between specified two nodes with the specified value
//Input:
//indexofnodefrom: the vertexindex of the source node
//indexofnodeto: the vertexindex of the destination node
//value: the weight of edge
//Output: NONE
  virtual void setEdgeValue(int indexofnodefrom, int indexofnodeto, Val value);
//Print the underlying adjacent matrix representation
//Input: NONE
//Output:
//The string representation of the output
  std::string printRepGraph();
//Return the adjacent matrix
//INPUT: NONE
//OUTPUT:
//The adjacent matrix which is a two dimensional matrix or a vector of vector
  const std::vector<std::vector<Val> >& getRepmatrix() const {
    return repmatrix;
  }
//Set the adjacent matrix by client
//INPUT: a two dimensional matrix or vector of vector
//OUTPUT: NONE
  void setRepmatrix(const std::vector<std::vector<Val> >& repmatrix) {
    this->repmatrix = repmatrix;
  }
  ;
//Get all nodes in this graph
//INPUT: NONE
//OUTPUT:
//a vector which stores the indices of nodes
  std::vector<int> getAllNodes() const;
//Get all the values of edges in this graph
//INPUT: NONE
//OUTPUT:
//All the edges for directed graph and half of the edges for undirected graph.
//Symmetric edges of undirected graph whose indexoffromnode is greater than indexoftonode will not be included.
//The edges will be in the map structure with
//key as "(index of from node-1)"x"size of vertices" + "(index of to node-1)" and
//value as the edge's weight
  std::map<int, Val> getAllEdgesValues() const;
//TODO separate this method in the inherited tree structure. This should be specific to tree structure
//DFS implementation for undirected graph. If DFS found a visited nodes, then loop detected.
//Input:
//indexofroot: index of root
//Output:
//a boolean value to indicate if there's a loop in there
  bool isLoopExisting(int indexofroot);
//Return if this graph is undirected or not
//Input: NONE
//Output:
//a boolean indicator to indicate if this graph is undirected or not
//TRUE: this graph is undirected
//FALSE: this graph is directed
  inline bool isIsundirected() const {
    return isundirected;
  }
//Set the graph as undirected or directed
//Input:
//isundirected: a boolean indicator to indicate if this graph is undirected or not
//TRUE: this graph is undirected
//FALSE: this graph is directed
//Output: NONE
  inline void setIsundirected(bool isundirected) {
    this->isundirected = isundirected;
  }
//Overloading assignment
//Input:
//graph: the source of graph reference.
//Output:
//Graph reference with the same values of data members as the source graph
  Graph& operator =(const Graph& graph);
//printout MST
//Input:
//indexofroot: vertexindex of the selected root
//Output:
//string representation of the MST
  const std::string printMST(int indexofroot);

//remove the singletons
  std::vector<std::vector<int> > getAllSubGraphs();
};

#include "Graph.cpp"

#endif /* GRAPH_H_ */
