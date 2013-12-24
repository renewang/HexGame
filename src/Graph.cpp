#include "Graph.h"

using namespace std;

template<class Type, class Val>
vector<vector<int> > Graph<Type, Val>::getAllSubGraphs() {
  bool* visited = new bool[this->numofvertices];
  fill(visited, visited + this->numofvertices, false);
  vector<vector<int> > forest;
  int ttlsize = 0, current = 1;
  while (ttlsize < this->numofvertices) {
    vector<int> subgraph;
    TraverseDFS(current, visited, -1, &subgraph);  //overloading
    bool* cur = find(visited, visited + this->numofvertices, false);
    if (cur != visited + this->numofvertices)
      current = (cur - visited) / sizeof(bool) + 1;
    ttlsize += subgraph.size();
    if (subgraph.size() > 1)
      forest.push_back(subgraph);
  }
  delete[] visited;
  return vector<vector<int> >(forest);
}
template<class Type, class Val>
const string Graph<Type, Val>::printMST(int indexofroot) {
  bool* visited = new bool[this->numofvertices];
  std::fill(visited, visited + this->numofvertices, false);
  std::stringstream treestream;
  treestream << '(' << indexofroot;
  bool isloopexisting = TraverseDFS(indexofroot, visited, -1, &treestream);
  treestream << ')';
  if (isloopexisting) {
    treestream.clear();
    treestream << "Loop existing in this tree! Abort!";
  }
  delete[] visited;
  return treestream.str();
}
//Overloading assignment
//Input:
//graph: the source of graph reference.
//Output:
//Graph reference with the same values of data members as the source graph
template<class Type, class Val>
Graph<Type, Val>& Graph<Type, Val>::operator =(const Graph<Type, Val>& graph) {
  this->density = graph.density;
  this->distance = graph.distance;
  this->isundirected = graph.isundirected;
  this->mindistance = graph.mindistance;
  this->numofedges = graph.numofedges;
  this->numofvertices = graph.numofvertices;
  this->repgraph = std::vector<Node>(graph.repgraph);
  this->repmatrix = std::vector<std::vector<Val> >(graph.repmatrix);
  return *this;
}
//TODO separate this method in the inherited tree structure. This should be specific to tree structure
//DFS implementation for undirected graph. If DFS found a visited nodes, then loop detected.
//Input:
//indexofroot: index of root
//Output:
//a boolean value to indicate if there's a loop in there
template<class Type, class Val>
bool Graph<Type, Val>::isLoopExisting(int indexofroot) {
  bool* visited = new bool[this->numofvertices];
  std::fill(visited, visited + this->numofvertices, false);
  bool isloopexisting = TraverseDFS(indexofroot, visited);
  delete[] visited;
  return isloopexisting;
}
//Get all the values of edges in this graph
//INPUT: NONE
//OUTPUT:
//All the edges for directed graph and half of the edges for undirected graph.
//Symmetric edges of undirected graph whose indexoffromnode is greater than indexoftonode will not be included.
//The edges will be in the map structure with
//key as "(index of from node-1)"x"size of vertices" + "(index of to node-1)" and
//value as the edge's weight
template<class Type, class Val>
std::map<int, Val> Graph<Type, Val>::getAllEdgesValues() const {
  std::map<int, Val> mapalledges;
  for (unsigned i = 1; i <= this->numofvertices; i++) {
    std::list<Edge> neigh = repgraph[i - 1].neighbors;
    typename std::list<Edge>::iterator iteredge = neigh.begin();
    for (; iteredge != neigh.end(); ++iteredge) {
      assert(static_cast<int>(i) == (*iteredge).indexoffromnode);
      //eliminate the redundant edges
      if (isundirected
          && (*iteredge).indexoffromnode > (*iteredge).indexoftonode)
        continue;
      int key = ((*iteredge).indexoffromnode - 1) * this->numofvertices
          + ((*iteredge).indexoftonode - 1);
      assert(
          key >= 0
              && key
                  <= static_cast<int>(this->numofvertices * this->numofvertices)
              && (key % static_cast<int>(this->numofvertices + 1)) != 0);
      mapalledges.insert(std::make_pair(key, (*iteredge).weight));
    }
  }
  return std::map<int, Val>(mapalledges);
}
//Get all nodes in this graph
//INPUT: NONE
//OUTPUT:
//a vector which stores the indices of nodes
template<class Type, class Val>
vector<int> Graph<Type, Val>::getAllNodes() const {
  std::vector<int> nodes;
  for (auto n : this->repgraph)
    nodes.push_back(n.vertexindex);
  return vector<int>(nodes);
}
//Implementation of Monte Carlo simulation to generate undirected graph.
template<class Type, class Val>
void Graph<Type, Val>::randomGraphGenerator() {
  if (!this->isundirected) {
    std::cerr
        << "random graph generation is not supported directed graph now!\n";
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
//Print the underlying adjacent matrix representation
//Input: NONE
//Output:
//The string representation of the output
template<class Type, class Val>
string Graph<Type, Val>::printRepGraph() {
  std::stringstream strstream;
  strstream << "size of vertices = " << repgraph.size() << "\n";
  toArrayGraphRep();

  strstream << std::setw(9) << " ";
  for (unsigned i = 0; i < numofvertices; i++)
    strstream << i + 1 << std::setw(5) << " ";
  strstream << "\n";
  for (unsigned i = 0; i < numofvertices; i++) {
    strstream << std::setw(5) << i + 1;
    for (unsigned j = 0; j < numofvertices; j++) {
      strstream << std::setw(5) << repmatrix[i][j] << " ";
    }
    strstream << "\n";
  }
  return strstream.str();
}
//Set the edge between specified two nodes with the specified value
//Input:
//indexofnodefrom: the vertexindex of the source node
//indexofnodeto: the vertexindex of the destination node
//value: the weight of edge
//Output: NONE
template<class Type, class Val>
void Graph<Type, Val>::setEdgeValue(int indexofnodefrom, int indexofnodeto,
                                    Val value) {
  if (isAdjacent(indexofnodefrom, indexofnodeto)) {
    Node* nodefrom = const_cast<Node*>(findNodeByIndex(indexofnodefrom));
    Node* nodeto = const_cast<Node*>(findNodeByIndex(indexofnodeto));

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
//Get edge value between specified two nodes
//Input:
//indexofnodefrom: the vertexindex of the source node
//indexOfnodeto: the vertexindex of the destination node
//Output:
//Return the weight of edge between the input nodes
template<class Type, class Val>
Val Graph<Type, Val>::getEdgeValue(int indexofnodefrom,
                                   int indexofnodeto) const {
  Val value = static_cast<Val>(0);
  if (isAdjacent(indexofnodefrom, indexofnodeto)) {
    const Node* nodefrom = findNodeByIndex(indexofnodefrom);
    return findEdgeByIndex(nodefrom, indexofnodeto);
  }
  return value;
}
//Delete the edge between the specified nodes
//Input:
//indexofnodefrom: the vertexindex of the source node
//indexofnodeto, the vertexindex of the destination node
//Output: None
template<class Type, class Val>
void Graph<Type, Val>::deleteEdge(int indexofnodefrom, int indexofnodeto) {
  if (isAdjacent(indexofnodefrom, indexofnodeto)) {
    Node* nodefrom = const_cast<Node*>(findNodeByIndex(indexofnodefrom));
    Node* nodeto = const_cast<Node*>(findNodeByIndex(indexofnodeto));

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
//Add Edge between the two specified nodes with the specified value
//Input:
//indexofsource: the vertexindex of the source node
//indexofdest: the vertexindex of the destination node
//value: the weight of intending adding edge
//Output: NONE
template<class Type, class Val>
void Graph<Type, Val>::addEdge(int indexofsource, int indexofdest, Val value) {
  Node* nodefrom = const_cast<Node*>(findNodeByIndex(indexofsource));
  Node* nodeto = const_cast<Node*>(findNodeByIndex(indexofdest));

  Edge edge;
  edge.indexoffromnode = indexofsource;
  edge.indexoftonode = indexofdest;
  edge.weight = value;
  (nodefrom->neighbors).push_back(edge);
  nodefrom->numofneighbors++;

  //symmetric assignment for the undirected graph
  if (isundirected) {
    Edge symedge;
    symedge.indexoffromnode = indexofdest;
    symedge.indexoftonode = indexofsource;
    symedge.weight = value;
    (nodeto->neighbors).push_back(symedge);

    nodeto->numofneighbors++;
  }
  numofedges++;
}
//Return the number of neighbors of a node
//Input:
//idxofnode: the vertexindex of the inquiring node
//Output:
//The size of connected neighbors
template<class Type, class Val>
const int Graph<Type, Val>::getNeighborsSize(int idxofnode) {
  const Node* node = findNodeByIndex(idxofnode);
  return node->numofneighbors;
}

//Return the edges of neighbors of a node
//Input:
//idxofnode: the vertexindex of the inquiring node
//Output:
//The list which stores the edges of the neighbors of the inquiring node
template<class Type, class Val>
const vector<Val> Graph<Type, Val>::getNeighborsEdgeValues(
    int idxofnode) const {
  const Node* node = findNodeByIndex(idxofnode);
  vector<Val> vec;
  list<Edge> neigh = node->neighbors;
  for (typename list<Edge>::iterator iter = neigh.begin(); iter != neigh.end();
      ++iter)
    vec.push_back((*iter).weight);
  return vector<Val>(vec);
}
//Return the neighbors of a node
//Input:
//idxofnode: the vertexindex of the inquiring node
//Output:
//The vector which stores the vertexindices of the neighbors of the inquiring node
template<class Type, class Val>
vector<int> Graph<Type, Val>::getNeighbors(int idxofnode) const {
  const Node* node = findNodeByIndex(idxofnode);
  vector<int> neighindicesvec;
  list<Edge> neigh = node->neighbors;
  for (typename list<Edge>::iterator iter = neigh.begin(); iter != neigh.end();
      ++iter)
    neighindicesvec.push_back((*iter).indexoftonode);
  return vector<int>(neighindicesvec);
}
//Test if two nodes is adjacent
//Input:
//idxofnodea: the vertexindex of first node
//idxofnodeb: the vertexindex of second node
//Output:
//The boolean variable to indicate if the specified two nodes are connected.
//TRUE: is connected or at adjacency
//FALSE: is not connected or not at adjacency
template<class Type, class Val>
bool Graph<Type, Val>::isAdjacent(int idxofnodefrom, int idxofnodeto) const {
  const Node* nodefrom = findNodeByIndex(idxofnodefrom);
  int neigh = findNeighborByIndex(nodefrom, idxofnodeto);
  return idxofnodeto == neigh;
}
//Copy constructor
template<class Type, class Val>
Graph<Type, Val>::Graph(const Graph& graph) {
  this->density = graph.density;
  this->distance = graph.distance;
  this->isundirected = graph.isundirected;
  this->mindistance = graph.mindistance;
  this->numofedges = graph.numofedges;
  this->numofvertices = graph.numofvertices;
  this->repgraph = std::vector<Node>(graph.repgraph);
  this->repmatrix = std::vector<std::vector<Val> >(graph.repmatrix);
}
//Create a empty graph
template<class Type, class Val>
Graph<Type, Val>::Graph(unsigned numofvertices) {
  initGraph();
  this->numofvertices = numofvertices;
  for (unsigned i = 0; i < numofvertices; i++) {
    Node node;
    initNode(node, i);
    repgraph.push_back(node);
  }
}
template<class Type, class Val>
Graph<Type, Val>::Graph(AbstractParser& parser) {
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
//Constructor to generate a graph according to the matrix provided by client
template<class Type, class Val>
Graph<Type, Val>::Graph(Val** clientgraph, int numofvertices) {
  initGraph();
  repmatrix.reserve(numofvertices);
  for (int i = 0; i < numofvertices; i++) {
    std::vector<Val> vec(clientgraph[i], clientgraph[i] + numofvertices);
    repmatrix.push_back(vec);
  }
  this->numofvertices = numofvertices;
  toListGraphRep();
}
template<class Type, class Val>
Graph<Type, Val>::Graph(unsigned numofvertices, float density, float distance) {

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
//Provide the functionality to retrieve edge of a neighboring node given the source node object
//and the index of connected node
template<class Type, class Val>
Val Graph<Type, Val>::findEdgeByIndex(const Node* node, int index) const {
  list<Edge> neighbors = node->neighbors;
  typename list<Edge>::iterator iterneigh = neighbors.begin();
  for (; iterneigh != neighbors.end(); ++iterneigh) {
    if (index == (*iterneigh).indexoftonode) {
      return (*iterneigh).weight;
    }
  }
  return static_cast<Val>(0);
}
template<class Type, class Val>
int Graph<Type, Val>::findNeighborByIndex(const Node* node, int index) const {
  list<Edge> neighbors = node->neighbors;
  for (typename std::list<Edge>::iterator iter = neighbors.begin();
      iter != neighbors.end(); ++iter) {
    if (index == (*iter).indexoftonode)
      return (*iter).indexoftonode;
  }
  return 0;
}
template<class Type, class Val>
void Graph<Type, Val>::toListGraphRep() {

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
//Transform underlying representation of the graph from adjacent matrix to linked list
template<class Type, class Val>
void Graph<Type, Val>::toArrayGraphRep() {
  //clear the repmatrix if is allocated before
  if (repmatrix.size() != 0)
    repmatrix.clear();

  //allocate memory
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
//Private function to initialize the members of Graph in constructors
template<class Type, class Val>
void Graph<Type, Val>::initGraph() {
  density = 0;
  distance = 0.0;
  numofedges = 0;
  numofvertices = 0;
  isundirected = true;
  mindistance = static_cast<Val>(1);
}
