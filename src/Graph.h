/*
 * Graph.h
 * This file defines a Graph data structure which holds a graph's vertices and edges.
 * Also implement Monte Carlo simulation to generate  a random graph given the probability and distance range
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <iostream>
#include <iomanip>
#include <iterator>
#include <list>
#include <cstring>
#include <vector>
#include <cassert>
#include <algorithm>

/*TODO: Make Node structure as private member of Graph to gain the better encapsulation.
 * Node structure is used to hold the vertex information which contains:
 *
 * To generate a Node uses string type as its label and sample usage of initialization:
 * Node<string, double> node;
 *
 */
template<class Type, class Val>
struct Node {
  int vertexindex;  //an unique identifier starting from 1 to the number of vertices, automatically assigned when constructing the graph class
  unsigned numofneighbors;  //the number of connected vertices
  Type vertexvalue;  //the label or value of the vertex which should be descriptive such as string. But users are free to specify their own type in template syntax.

  //TODO: will combine edges and neighbors lists to form a better data structure
  //a double linked list to store the weight of the connected edge.
  //The type of edges are user defined but recommend to restrict to numeric type such as double, float and integer

  std::list<Val> edges;
  std::list<Node*> neighbors;  //a double linked list to store the connected node.
};
/*
 * Graph Class is used as a representation of graph. It contain three constructor:
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
 */
template<class Type, class Val>
class Graph {
  typedef struct Node<Type, Val> Node;

 private:
  unsigned numofvertices;  //size of Nodes or Vertex in Graph
  unsigned numofedges;  //size of Edges in Graph
  float density;  //density of connected edges to generate the random Graph
  float distance;  //maximal distance to generate random Graph
  std::vector<Node> repgraph;  //adjacent list implementation of Graph representation
  std::vector<std::vector<Val> > repmatrix;  //adjacent matrix implementation of Graph representation
  Val mindistance;
  ;  //hold the minimal distance which should equal to 1
  bool isundirected;  //indicator for a undirected graph

  //Private function to initialize the members of Graph in constructors
  void initGraph() {
    numofedges = 0;
    distance = 0.0;
    density = 0;
    numofvertices = 0;
    isundirected = true;
    mindistance = static_cast<Val>(1);
  }
  ;
  //Private function to initialize the Node in constructors
  void initNode(Node& node, int index) {
    node.vertexindex = (index + 1);
    node.numofneighbors = 0;
  }
  ;
  //Implementation of Monte Carlo simulation
  void randomGraphGenerator() {
    //initialize random seed with current time
    srand(clock());

    for (typename std::vector<Node>::iterator iterself = repgraph.begin();
        iterself != repgraph.end(); ++iterself) {  // for each vertex
      Node* n = &(*iterself);
      typename std::vector<Node>::iterator iterneigh = iterself;
      iterneigh++;

      for (; iterneigh != repgraph.end(); ++iterneigh) {  // determine if this edge should be generated according to the density
        float prob = (float) rand() / RAND_MAX;
        assert(prob > 0 && prob < 1);

        if (prob <= density) {
          //set the neighbor pointer to the neighboring node, the minimal distance is 1
          Val val = static_cast<Val>(rand() % static_cast<int>(distance)
              + mindistance);

          assert(val >= mindistance && val <= distance);

          (n->neighbors).push_back(&(*iterneigh));
          (n->edges).push_back(val);

          //undirected graph, need a symmetric assignment
          Node* neigh = &(*iterneigh);
          (neigh->neighbors).push_back(&(*iterself));
          (neigh->edges).push_back(val);

          numofedges++;
        }
      }
      n->numofneighbors = (n->neighbors.size());
      assert(
          n->numofneighbors >= 0 && n->numofneighbors < numofvertices
              && n->numofneighbors == n->edges.size());
    }
  }
  ;
  //Transform underlying representation of the graph from adjacent matrix to linked list
  void toArrayGraphRep() {
    //allocate memory
    if (repmatrix.size() != 0)
      repmatrix.clear();

    repmatrix.reserve(numofvertices);
    for (unsigned i = 0; i < numofvertices; i++) {
      std::vector<Val> vec(numofvertices, 0);
      repmatrix.push_back(vec);
    }

    //Transform the representation from list to matrix
    for (typename std::vector<Node>::iterator iterself = repgraph.begin();
        iterself != repgraph.end(); ++iterself) {
      Node* node = &(*iterself);
      std::list<Node*> neighs = node->neighbors;
      std::list<Val> edges = node->edges;
      int idxofself = node->vertexindex - 1;
      typename std::list<Node*>::iterator iterneigh = neighs.begin();
      typename std::list<Val>::iterator iteredge = edges.begin();
      for (; iterneigh != neighs.end(); ++iterneigh, ++iteredge) {
        int idxofneigh = (*iterneigh)->vertexindex - 1;
        repmatrix[idxofself][idxofneigh] = (*iteredge);
        if (isundirected)
          repmatrix[idxofneigh][idxofself] = (*iteredge);
      }
    }
  }
  ;
  //Transform underlying representation of the graph from adjacent linked list to matrix
  void toListGraphRep() {

    for (unsigned i = 0; i < numofvertices; i++) {
      Node node;
      initNode(node, i);
      repgraph.push_back(node);
    }
    for (typename std::vector<Node>::iterator iterself = repgraph.begin();
        iterself != repgraph.end(); ++iterself) {
      Node* node = &(*iterself);
      typename std::vector<Node>::iterator iterneigh = iterself;
      iterneigh++;
      for (unsigned j = node->vertexindex; j < numofvertices; j++) {
        Node* neigh = &(*iterneigh);
        if (repmatrix[node->vertexindex - 1][j] > 0) {
          //assign neighbors to the node
          (node->neighbors).push_back(neigh);
          (node->edges).push_back(repmatrix[node->vertexindex - 1][j]);

          if (isundirected) {
            //symmetric assignment to node's neighbor for undirected graph
            (neigh->neighbors).push_back(node);
            (neigh->edges).push_back(repmatrix[node->vertexindex - 1][j]);
          }
        }
        iterneigh++;
      }
      assert((node->neighbors).size() == (node->edges).size());
      node->numofneighbors = (node->neighbors).size();
    }
  }
  ;
  //Provide the functionality to retrieve node's pointer given the index of source node
  Node* findNodeByIndex(int index) {
    assert(index >= 1 && index <= static_cast<int>(numofvertices));
    return &(repgraph[index - 1]);
  }

  //Provide the functionality to retrieve the Node pointer of a connected node given the source node object
  //and the index of connected node
  Node* findNeighborByIndex(Node node, int index) {
    std::list<Node*> neighbors = node.neighbors;
    for (typename std::list<Node*>::iterator iter = neighbors.begin();
        iter != neighbors.end(); ++iter) {
      if (index == (*iter)->vertexindex)
        return (*iter);
    }
    return NULL;
  }
  //Provide the functionality to retrieve edge of a neighboring node given the source node object
  //and the index of connected node
  Val findEdgeByIndex(Node node, int index) {
    std::list<Node*> neighbors = node.neighbors;
    std::list<Val> edges = node.edges;
    typename std::list<Node*>::iterator iterneigh = neighbors.begin();
    typename std::list<Val>::iterator iteredge = edges.begin();
    for (; iterneigh != neighbors.end(); ++iterneigh, ++iteredge) {
      if (index == (*iterneigh)->vertexindex) {
        return (*iteredge);
      }
    }
    return NULL;
  }
 public:

  //Default constructor, initialize graph with zero values
  //Takes no arguments
  Graph() {
    initGraph();
  }
  ;
  //Constructor to generate a random graph
  Graph(unsigned numofvertices, float density, float distance) {

    //initialize the graph with zero values
    initGraph();
    this->numofvertices = numofvertices;
    this->density = density;
    this->distance = distance;

    for (unsigned i = 0; i < numofvertices; i++) {
      Node n;
      initNode(n, i);
      repgraph.push_back(n);
    }
    randomGraphGenerator();
  }
  ;
  //Constructor to generate a graph according to the matrix provided by client
  Graph(Val** clientgraph, int numofvertices) {
    initGraph();
    repmatrix.reserve(numofvertices);
    for (int i = 0; i < numofvertices; i++) {
      std::vector<Val> vec(clientgraph[i], clientgraph[i] + numofvertices);
      repmatrix.push_back(vec);
    }
    this->numofvertices = numofvertices;
    toListGraphRep();
  }
  //Get the underlying adjacent list representation of vertices.
  inline const std::vector<Node> getAllVertices() {
    return std::vector<Node>(repgraph);
  }
  //Get the size of vertices
  inline int getSizeOfVertices() {
    return repgraph.size() == numofvertices ? numofvertices : -1;
  }
  ;
  //Get the size of edges
  inline int getSizeOfEdges() {
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
  bool isAdjacent(int idxofnodea, int idxofnodeb) {
    Node* nodea = findNodeByIndex(idxofnodea);
    Node* nodeb = findNodeByIndex(idxofnodeb);

    Node* nodesearched = (
        nodea->numofneighbors < nodeb->numofneighbors ? nodeb : nodea);
    Node* node = (nodea->numofneighbors < nodeb->numofneighbors ? nodea : nodeb);

    Node* neigh = findNeighborByIndex(*node, nodesearched->vertexindex);
    return nodesearched == neigh;
  }
  ;
  //Return the neighbors of a node
  //Input:
  //idxofnode: the vertexindex of the inquiring node
  //Output:
  //The vector which stores the vertexindices of the neighbors of the inquiring node
  std::vector<int> getNeighbors(int idxofnode) {
    Node* node = findNodeByIndex(idxofnode);

    std::vector<int> neighindicesvec;
    std::list<Node*> neigh = node->neighbors;
    for (typename std::list<Node*>::iterator iter = neigh.begin();
        iter != neigh.end(); ++iter)
      neighindicesvec.push_back((*iter)->vertexindex);
    return neighindicesvec;
  }
  ;
  //Add Edge between the two specified nodes with the specified value
  //Input:
  //indexofsource: the vertexindex of the source node
  //indexofdest: the vertexindex of the destination node
  //value: the weight of intending adding edge
  //Output: NONE
  void addEdge(int indexofsource, int indexofdest, Val value) {
    Node* nodefrom = findNodeByIndex(indexofsource);
    Node* nodeto = findNodeByIndex(indexofdest);

    (nodefrom->neighbors).push_back(nodeto);
    (nodefrom->edges).push_back(value);
    nodefrom->numofneighbors++;

    //symmetric assignment for the undirected graph
    if (isundirected) {
      (nodeto->neighbors).push_back(nodefrom);
      (nodeto->edges).push_back(value);

      nodeto->numofneighbors++;
    }

    numofedges++;
  }
  ;
  //Delete the edge between the specified nodes
  //Input:
  //indexofnodefrom: the vertexindex of the source node
  //indexofnodeto, the vertexindex of the destination node
  //Output: None
  void deleteEdge(int indexofnodefrom, int indexofnodeto) {
    if (isAdjacent(indexofnodefrom, indexofnodeto)) {
      Node* nodefrom = findNodeByIndex(indexofnodefrom);
      Node* nodeto = findNodeByIndex(indexofnodeto);

      std::list<Node*>* neighbors = &(nodefrom->neighbors);
      std::list<Val>* edges = &(nodefrom->edges);

      typename std::list<Node*>::iterator iterneigh = neighbors->begin();
      typename std::list<Val>::iterator iteredge = edges->begin();

      for (; iterneigh != neighbors->end(); ++iterneigh, ++iteredge) {
        if (nodeto->vertexindex == (*iterneigh)->vertexindex) {
          neighbors->erase(iterneigh);
          edges->erase(iteredge);
          nodefrom->numofneighbors--;
          break;
        }
      }

      //symmetric assignment for undirected graph
      if (isundirected) {
        neighbors = &(nodeto->neighbors);
        edges = &(nodeto->edges);
        iterneigh = neighbors->begin();
        iteredge = edges->begin();

        for (; iterneigh != neighbors->end(); ++iterneigh, ++iteredge) {
          if (nodefrom->vertexindex == (*iterneigh)->vertexindex) {
            neighbors->erase(iterneigh);
            edges->erase(iteredge);
            nodeto->numofneighbors--;
            break;
          }
        }
      }
      numofedges--;
    }
  }
  ;
  //Get the value of the Node
  //Input:
  //indexofnode: the vertexindex of the inquiring node
  //Output:
  //The label or representing value of the inquiring node
  Type getNodeValue(int indexofnode) {
    Node node = repgraph[indexofnode];
    return node.vertexvalue;
  }
  ;
  //Set value of the Node
  //Input:
  //indexofnode: the vertexindex of the inquiring node
  //value: The label or the representing value of the inquiring node
  //Output: None
  void setNodeValue(int indexofnode, Val value) {
    repgraph[indexofnode].vertexvalue = value;
  }
  ;
  //Get edge value between specified two nodes
  //Input:
  //indexofnodefrom: the vertexindex of the source node
  //indexOfnodeto: the vertexindex of the destination node
  //Output:
  //Return the weight of edge between the input nodes
  Val getEdgeValue(int indexofnodefrom, int indexofnodeto) {
    Val value = NULL;
    if (isAdjacent(indexofnodefrom, indexofnodeto)) {
      Node* nodefrom = findNodeByIndex(indexofnodefrom);
      Node* nodeto = findNodeByIndex(indexofnodeto);
      Node* nodesearched, *node;
      if (nodefrom->numofneighbors < nodeto->numofneighbors) {
        node = nodefrom;
        nodesearched = nodeto;
      } else {
        node = nodeto;
        nodesearched = nodefrom;
      }

      return findEdgeByIndex(*node, nodesearched->vertexindex);
    }
    return value;
  }
  ;
  //Set the edge between specified two nodes with the specified value
  //Input:
  //indexofnodefrom: the vertexindex of the source node
  //indexofnodeto: the vertexindex of the destination node
  //value: the weight of edge
  //Output: NONE
  void setEdgeValue(int indexofnodefrom, int indexofnodeto, Val value) {
    if (isAdjacent(indexofnodefrom, indexofnodeto)) {
      Node* nodefrom = findNodeByIndex(indexofnodefrom);
      Node* nodeto = findNodeByIndex(indexofnodeto);

      std::list<Node*>* neighbors = &(nodefrom->neighbors);
      std::list<Val>* edges = &(nodefrom->edges);
      typename std::list<Node*>::iterator iterneigh = neighbors->begin();
      typename std::list<Val>::iterator iteredge = edges->begin();

      for (; iterneigh != neighbors->end(); ++iterneigh, ++iteredge) {
        if (nodeto->vertexindex == (*iterneigh)->vertexindex) {
          (*iteredge) = value;
          break;
        }
      }

      //symmetric assignment for undirected graph
      if (isundirected) {
        neighbors = &(nodeto->neighbors);
        edges = &(nodeto->edges);
        iterneigh = neighbors->begin();
        iteredge = edges->begin();

        for (; iterneigh != neighbors->end(); ++iterneigh, ++iteredge) {
          if (nodefrom->vertexindex == (*iterneigh)->vertexindex) {
            (*iteredge) = value;
            break;
          }
        }
      }
    }
  }
  ;
  //deconstructor
  virtual ~Graph() {
  }
  ;
  //Print the underlying adjacent matrix representation
  bool printRepGraph() {
    std::cout << "size of vertices = " << repgraph.size() << "\n";
    toArrayGraphRep();

    std::cout << std::setw(9) << " ";
    for (unsigned i = 0; i < numofvertices; i++)
      std::cout << i + 1 << std::setw(5) << " ";
    std::cout << "\n";
    for (unsigned i = 0; i < numofvertices; i++) {
      std::cout << std::setw(5) << i + 1;
      for (unsigned j = 0; j < numofvertices; j++) {
        std::cout << std::setw(5) << repmatrix[i][j] << " ";
      }
      std::cout << "\n";
    }
    return true;
  }
  ;

};
#endif /* GRAPH_H_ */
