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
#include <iterator>
#include <iostream>
#include <algorithm>

#include <cstring>
#include <cassert>
#include <cstdlib>

#include "PlainParser.h"

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
//TODO add directed graph support and correct edge calculations
template<class Type, class Val>
class Graph {
 private:
  /* Edge structure
   *
   */
  struct Edge {
    int indexoffromnode;
    int indexoftonode;
    Val weight;
  };
  /* Node structure is used to hold the vertex information which contains:
   *
   * To generate a Node uses string type as its label and sample usage of initialization:
   * Node<string, double> node;
   *
   */
  struct Node {
    int vertexindex;  //an unique identifier starting from 1 to the number of vertices, automatically assigned when constructing the graph class
    unsigned numofneighbors;  //the number of connected vertices
    Type vertexvalue;  //the label or value of the vertex which should be descriptive such as string. But users are free to specify their own type in template syntax.

    //TODO: will combine edges and neighbors lists to form a better data structure
    //a double linked list to store the weight of the connected edge.
    //The type of edges are user defined but recommend to restrict to numeric type such as double, float and integer

    //std::list<Val> edges;
    //std::list<Node*> neighbors;  //a double linked list to store the connected node.
    std::list<Edge> neighbors;
  };
  struct MyTransfom {
   public:
    Val operator()(const std::string str) {
      Val value = NULL;
      if (typeid(Val) == typeid(int)) {
        value = static_cast<Val>(atoi(str.c_str()));
      } else if (typeid(Val) == typeid(double)
          || typeid(Val) == typeid(float)) {
        value = static_cast<Val>(atof(str.c_str()));
      }
      return value;
    }
  };
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
    density = 0;
    distance = 0.0;
    numofedges = 0;
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
    if (!this->isundirected) {
      std::cerr << "random generate is not supported directed graph now!\n";
      return;
    }
    //initialize random seed with current time
    srand(clock());

    for (typename std::vector<Node>::iterator iterself = repgraph.begin();
        iterself != repgraph.end(); ++iterself) {  // for each vertex
      Node* nodefrom = &(*iterself);
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

          Node* nodeto = &(*iterneigh);

          Edge edge;
          edge.indexoffromnode = nodefrom->vertexindex;
          edge.indexoftonode = nodeto->vertexindex;
          edge.weight = val;
          (nodefrom->neighbors).push_back(edge);

          //undirected graph, need a symmetric assignment
          Edge symedge;
          symedge.indexoffromnode = nodefrom->vertexindex;
          symedge.indexoftonode = nodeto->vertexindex;
          symedge.weight = val;
          (nodeto->neighbors).push_back(symedge);
          this->numofedges++;
        }
      }
      nodefrom->numofneighbors = (nodefrom->neighbors.size());
      assert(
          nodefrom->numofneighbors >= 0
              && nodefrom->numofneighbors < numofvertices);
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
      std::list<Edge> neighs = node->neighbors;
      int idxofself = node->vertexindex - 1;
      typename std::list<Edge>::iterator iterneigh = neighs.begin();
      for (; iterneigh != neighs.end(); ++iterneigh) {
        int idxofneigh = (*iterneigh).indexoftonode - 1;
        repmatrix[idxofself][idxofneigh] = (*iterneigh).weight;
        if (isundirected)
          repmatrix[idxofneigh][idxofself] = (*iterneigh).weight;
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
          Edge edge;
          edge.indexoffromnode = node->vertexindex;
          edge.indexoftonode = neigh->vertexindex;
          edge.weight = repmatrix[node->vertexindex - 1][j];
          //assign neighbors to the node
          (node->neighbors).push_back(edge);

          if (isundirected) {
            //symmetric assignment to node's neighbor for undirected graph
            Edge symedge;
            symedge.indexoffromnode = neigh->vertexindex;
            symedge.indexoftonode = node->vertexindex;
            symedge.weight = repmatrix[node->vertexindex - 1][j];
            //assign neighbors to the node
            (neigh->neighbors).push_back(symedge);
          }
          this->numofedges++;
        }
        iterneigh++;
      }
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
  int findNeighborByIndex(Node* node, int index) {
    std::list<Edge> neighbors = node->neighbors;
    for (typename std::list<Edge>::iterator iter = neighbors.begin();
        iter != neighbors.end(); ++iter) {
      if (index == (*iter).indexoftonode)
        return (*iter).indexoftonode;
    }
    return 0;
  }
  //Provide the functionality to retrieve edge of a neighboring node given the source node object
  //and the index of connected node
  Val findEdgeByIndex(Node* node, int index) {
    std::list<Edge> neighbors = node->neighbors;
    typename std::list<Edge>::iterator iterneigh = neighbors.begin();
    for (; iterneigh != neighbors.end(); ++iterneigh) {
      if (index == (*iterneigh).indexoftonode) {
        return (*iterneigh).weight;
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
  //Constructor to generate a graph according to the input file
  //TODO modify to pass an AbstractParser type
  Graph(PlainParser& parser) {
    initGraph();
    std::vector<std::vector<std::string> > graphfromtext = parser.getData();
    int graphsize = atoi(graphfromtext[0][0].c_str());
    if (graphsize <= 0)
      return;

    this->numofvertices = graphsize;

    //initialize vector
    repmatrix.reserve(graphsize);
    for (unsigned i = 0; i < numofvertices; i++) {
      std::vector<Val> vec(numofvertices, 0);
      repmatrix.push_back(vec);
    }

    for (unsigned i = 1; i < graphfromtext.size(); i++) {
      std::vector<Val> vecTrans(graphfromtext[i].size());
      transform(graphfromtext[i].begin(), graphfromtext[i].end(),
                vecTrans.begin(), MyTransfom());
      assert(graphfromtext[i].size() == 3);
      repmatrix[vecTrans[0]][vecTrans[1]] = vecTrans[2];
    }
    toListGraphRep();
  }
  //Greate a empty graph
  Graph(unsigned numofvertices) {
    initGraph();
    this->numofvertices = numofvertices;
    for (unsigned i = 0; i < numofvertices; i++) {
      Node node;
      initNode(node, i);
      repgraph.push_back(node);
    }
  }
  //deconstructor
  virtual ~Graph() {
  }
  ;
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
  bool isAdjacent(int idxofnodefrom, int idxofnodeto) {
    Node* nodefrom = findNodeByIndex(idxofnodefrom);
    int neigh = findNeighborByIndex(nodefrom, idxofnodeto);
    return idxofnodeto == neigh;
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
    std::list<Edge> neigh = node->neighbors;
    for (typename std::list<Edge>::iterator iter = neigh.begin();
        iter != neigh.end(); ++iter)
      neighindicesvec.push_back((*iter).indexoftonode);
    return std::vector<int>(neighindicesvec);
  }
  ;
  //Return the edges of neighbors of a node
  //Input:
  //idxofnode: the vertexindex of the inquiring node
  //Output:
  //The list which stores the edges of the neighbors of the inquiring node
  const std::vector<Val> getNeighborsEdgeValues(int idxofnode) {
    Node* node = findNodeByIndex(idxofnode);
    std::vector<Val> vec;
    std::list<Edge> neigh = node->neighbors;
    for (typename std::list<Edge>::iterator iter = neigh.begin();
        iter != neigh.end(); ++iter)
      vec.push_back((*iter).weight);
    return std::vector<Val>(vec);
  }
  //Return the number of neighbors of a node
  //Input:
  //idxofnode: the vertexindex of the inquiring node
  //Output:
  //The size of conneted neighbors
  const int getNeighborsSize(int idxofnode) {
    Node* node = findNodeByIndex(idxofnode);
    return node->numofneighbors;
  }
//Add Edge between the two specified nodes with the specified value
//Input:
//indexofsource: the vertexindex of the source node
//indexofdest: the vertexindex of the destination node
//value: the weight of intending adding edge
//Output: NONE
  void addEdge(int indexofsource, int indexofdest, Val value) {
    Node* nodefrom = findNodeByIndex(indexofsource);
    Node* nodeto = findNodeByIndex(indexofdest);

    Edge edge;
    edge.indexoffromnode = indexofsource;
    edge.indexoftonode = indexofdest;
    edge.weight = value;
    (nodefrom->neighbors).push_back(edge);
    nodefrom->numofneighbors++;

    //symmetric assignment for the undirected graph
    if (isundirected) {
      Edge symedge;
      symedge.indexoffromnode = indexofsource;
      symedge.indexoftonode = indexofdest;
      symedge.weight = value;
      (nodeto->neighbors).push_back(symedge);

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

      std::list<Edge>* neighbors = &(nodefrom->neighbors);

      typename std::list<Edge>::iterator iterneigh = neighbors->begin();

      for (; iterneigh != neighbors->end(); ++iterneigh) {
        if (nodeto->vertexindex == (*iterneigh).indexoftonode) {
          neighbors->erase(iterneigh);
          nodefrom->numofneighbors--;
          break;
        }
      }

      //symmetric assignment for undirected graph
      if (isundirected) {
        neighbors = &(nodeto->neighbors);
        iterneigh = neighbors->begin();

        for (; iterneigh != neighbors->end(); ++iterneigh) {
          if (nodefrom->vertexindex == (*iterneigh).indexoftonode) {
            neighbors->erase(iterneigh);
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
      return findEdgeByIndex(nodefrom, indexofnodeto);
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

      std::list<Edge>* neighbors = &(nodefrom->neighbors);

      typename std::list<Edge>::iterator iterneigh = neighbors->begin();

      for (; iterneigh != neighbors->end(); ++iterneigh) {
        if (nodeto->vertexindex == (*iterneigh).indexoftonode) {
          (*iterneigh).weight = value;
          break;
        }
      }

      //symmetric assignment for undirected graph
      if (isundirected) {
        neighbors = &(nodeto->neighbors);
        iterneigh = neighbors->begin();

        for (; iterneigh != neighbors->end(); ++iterneigh) {
          if (nodefrom->vertexindex == (*iterneigh).indexoftonode) {
            (*iterneigh).weight = value;
            break;
          }
        }
      }
    }
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

  const std::vector<std::vector<Val> >& getRepmatrix() const {
    return repmatrix;
  }

  void setRepmatrix(const std::vector<std::vector<Val> >& repmatrix) {
    this->repmatrix = repmatrix;
  }
  ;
  std::map<int, int> getAllEdgesValues() {
    std::map<int, int> mapalledges;
    for (unsigned i = 1; i <= this->numofvertices; i++) {
      std::list<Edge> neigh = repgraph[i - 1].neighbors;
      typename std::list<Edge>::iterator iteredge = neigh.begin();
      for (; iteredge != neigh.end(); ++iteredge) {
        assert(static_cast<int>(i) == (*iteredge).indexoffromnode);
        if (isundirected
            && (*iteredge).indexoffromnode > (*iteredge).indexoftonode)
          continue;
        int key = ((*iteredge).indexoffromnode - 1) * this->numofvertices
            + ((*iteredge).indexoftonode - 1);
        assert(
            key >= 0
                && key
                    <= static_cast<int>(this->numofvertices
                        * this->numofvertices)
                && (key % static_cast<int>(this->numofvertices + 1)) != 0);
        //eliminate the redundant edges
        mapalledges.insert(make_pair(key, (*iteredge).weight));
      }
    }
    return std::map<int, int>(mapalledges);
  }
//TODO separate this method in the inherited tree structure
//should be specific to tree structure
//DFS implementation for undirected graph. If DFS found a visited nodes, then loop detected.
//Input:
//indexofroot: index of root
//Output:
//a boolean value to indicate if there's a loop in there
  bool isLoopExisting(int indexofroot) {
    bool isloopexisting = false;
    return isloopexisting;
  }
};
#endif /* GRAPH_H_ */
