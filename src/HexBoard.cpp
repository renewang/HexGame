/*
 * HexBoard.cpp
 * This file defines the implementation for the HexBoard class.
 */

#include "HexBoard.h"

using namespace std;

#if __cplusplus > 199711L
///default constructor, doing nothing except initializing the member values as defaults
HexBoard::HexBoard()
    : HexBoard(0) {
}
#else
HexBoard::HexBoard()
: Graph<hexgonValKind, int>(),
repgraph(Graph<hexgonValKind, int>::getRepgraph()),
numofhexgons(0) {
  initGraph();
}
#endif
///constructor to initialize the board according to given hexgon size per side
///@param numofhexgon is the number of hexgon per side
HexBoard::HexBoard(unsigned numofhexgon)
    : Graph<hexgonValKind, int>(numofhexgon * numofhexgon),
      repgraph(Graph<hexgonValKind, int>::getRepgraph()),
      numofhexgons(numofhexgon) {
  initGraph();
  numofvertices = numofhexgon * numofhexgon;
  for (unsigned i = 0; i < numofvertices; i++)
    initNode(repgraph[i], i);
  numofedges /= 2;
  initEmptyHexIndicators();
  Graph<hexgonValKind, int>::setIsundirected(isundirected);
  Graph<hexgonValKind, int>::setSizeOfVertices(numofvertices);
  Graph<hexgonValKind, int>::setSizeOfEdges(numofedges);
}
///copy constructor
///@param otherboard is the other HexBoard object
HexBoard::HexBoard(const HexBoard& otherboard)
    : Graph<hexgonValKind, int>(otherboard),
      repgraph(Graph<hexgonValKind, int>::getRepgraph()) {
  this->redmoves = otherboard.redmoves;
  this->bluemoves = otherboard.bluemoves;
  this->emptyhexindicators = otherboard.emptyhexindicators;
  this->numofemptyhexgons = otherboard.numofemptyhexgons;
  this->numofhexgons = otherboard.numofhexgons;
  ;
  this->isundirected = otherboard.isundirected;
  this->numofedges = otherboard.numofedges;
  this->numofvertices = otherboard.numofvertices;

  Graph<hexgonValKind, int>::setIsundirected(isundirected);
  Graph<hexgonValKind, int>::setSizeOfEdges(numofedges);
  Graph<hexgonValKind, int>::setSizeOfVertices(numofvertices);
  ;
}
///copy assignment using copy and swap semantics
///@param otherboard is the other HexBoard object
///@return return this reference
HexBoard& HexBoard::operator =(const HexBoard& otherboard) {
  HexBoard tmp(otherboard);
  this->redmoves.swap(tmp.redmoves);
  this->bluemoves.swap(tmp.bluemoves);
  this->emptyhexindicators.swap(tmp.emptyhexindicators);
  this->repgraph.swap(tmp.repgraph);
  this->numofemptyhexgons = tmp.numofemptyhexgons;
  this->numofhexgons = tmp.numofhexgons;
  this->isundirected = tmp.isundirected;
  this->numofedges = tmp.numofedges;
  this->numofvertices = tmp.numofvertices;

  Graph<hexgonValKind, int>::setIsundirected(isundirected);
  Graph<hexgonValKind, int>::setSizeOfEdges(numofedges);
  Graph<hexgonValKind, int>::setSizeOfVertices(numofvertices);

  return *this;
}
///destructor
HexBoard::~HexBoard() {
}
///private member to initialize the node according to their location
///@param node the intended to initialized node
///@param index the identifier of the node
///@return NONE
void HexBoard::initNode(Node& node, int index) {
  //Assign the corresponding edges according to the hexgonloctype
  //Assign all empty to all hexgons
  node.vertexindex = (index + 1);
  node.vertexvalue = hexgonValKind_EMPTY;
  int row = index / numofhexgons;
  int col = index % numofhexgons;
  HexBoard::HexgonLocType locafinder(*this);
  HexBoard::HexgonLocType::numLocEdges type = locafinder.getLocEdges(row, col);
  switch (type) {
    case HexBoard::HexgonLocType::DIAGNOLCONER: {
      node.numofneighbors = 2;
      //push neighbors
      for (int i = 1; i <= numofhexgons; i = i + (numofhexgons - 1)) {
        Edge edge;
        edge.indexoffromnode = node.vertexindex;
        edge.weight = 1;
        //index + 1 or index + numofhexgons for row = 0
        //index - 1 or index - numofhexgons for row = (numofhexgons - 1)
        if (row == 0)
          edge.indexoftonode = node.vertexindex + i;
        else
          edge.indexoftonode = node.vertexindex - i;
        node.neighbors.push_back(edge);
        numofedges++;
      }
      break;
    }
    case HexBoard::HexgonLocType::ANTIDOAGNOLCORNER: {
      node.numofneighbors = 3;
      //push neighbors
      int diffforantidia[3] = { 1, -(numofhexgons - 1), -numofhexgons };
      for (unsigned i = 0; i < 3; i++) {
        Edge edge;
        edge.indexoffromnode = node.vertexindex;
        edge.weight = 1;
        //index - 1, index + (numofhexgons - 1) or index + (numofhexgons) for row = 0
        //index + 1 or index - (numofhexgons -1 ) or index - (numofhexgons) for row = (numofhexgons - 1)
        if (row == 0)
          edge.indexoftonode = node.vertexindex - diffforantidia[i];
        else
          edge.indexoftonode = node.vertexindex + diffforantidia[i];
        node.neighbors.push_back(edge);
        numofedges++;
      }
      break;
    }
    case HexBoard::HexgonLocType::BOUNDARY: {
      node.numofneighbors = 4;
      //push neighbors
      for (unsigned i = 0; i < 4; i++) {
        Edge edge;
        edge.indexoffromnode = node.vertexindex;
        edge.weight = 1;
        int difffortopbottom[4] = { -1, 1, (numofhexgons - 1), numofhexgons };
        int diffforleftright[4] = { 1, -(numofhexgons - 1), -numofhexgons,
            numofhexgons };
        //north side, row = 0, index - 1. index + 1, index + (numofhexgons -1), index + numofhexgons
        //south side, row = (numofhexgons -1), index - 1. index + 1, index - (numofhexgons -1), index - numofhexgons
        //west side, col = 0, index + 1, index + numofhexgons, index - numofhexgons, index - (numofhexgons - 1)
        //east side, col = (numofhexgons -1), index - 1, index + numofhexgons, index - numofhexgons, index + (numofhexgons - 1)
        if (row == 0)			//north
          edge.indexoftonode = node.vertexindex + difffortopbottom[i];
        else if (row == (numofhexgons - 1))			//south
          edge.indexoftonode = node.vertexindex - difffortopbottom[i];
        else if (col == 0)			//west
          edge.indexoftonode = node.vertexindex + diffforleftright[i];
        else if (col == (numofhexgons - 1))			//east
          edge.indexoftonode = node.vertexindex - diffforleftright[i];
        node.neighbors.push_back(edge);
        numofedges++;
      }
      break;
    }
    case HexBoard::HexgonLocType::INTERNAL: {
      node.numofneighbors = 6;
      //push neighbors
      //(row-1, col), (row-1, col+1), (row, col-1), (row, col+1), (row+1, col-1) and (row+1, col)
      for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
          if (i == j)
            continue;
          Edge edge;
          edge.indexoffromnode = node.vertexindex;
          edge.weight = 1;
          edge.indexoftonode = (row + i) * this->numofhexgons + (col + j) + 1;
          node.neighbors.push_back(edge);
          numofedges++;
        }
      }
      break;
    }
  }
}
///To initialize members of the hex board as default values
///@param NONE
///@return NONE
void HexBoard::initGraph() {
  numofedges = 0;
  numofvertices = 0;
  isundirected = true;
}
///To get the location type given the index of row and column
///@param row the index of row (starting from 1 to number of hexgons per side)
///@param col the index of row (starting from 1 to number of hexgons per side)
///@return the location object of HexBoard::HexgonLocType::numLocEdges
HexBoard::HexgonLocType::numLocEdges HexBoard::HexgonLocType::getLocEdges(
    int row, int col) {
  if ((col - row) == 0 && (row == 0 || row == (board.numofhexgons - 1)))
    return HexBoard::HexgonLocType::DIAGNOLCONER;
  else if ((row + col) == (board.numofhexgons - 1)
      && (row == 0 || row == (board.numofhexgons - 1)))
    return HexBoard::HexgonLocType::ANTIDOAGNOLCORNER;
  else if (row == 0 || row == (board.numofhexgons - 1) || col == 0
      || col == (board.numofhexgons - 1))
    return HexBoard::HexgonLocType::BOUNDARY;
  else
    return HexBoard::HexgonLocType::INTERNAL;
}
///To output the hexgonkind
///@param os: output stream type
///@param hexgonkind the intended hexgonkind for output
///@return output stream type for pipe
ostream& operator<<(ostream& os, hexgonValKind hexgonkind) {
  switch (hexgonkind) {
    case hexgonValKind_EMPTY:
    os << "EMPTY";
    break;
    case hexgonValKind_RED:
    os << "RED";
    break;
    case hexgonValKind_BLUE:
    os << "BLUE";
    break;
  }
  return os;
}
///Set the edge value according to the index of hexgon
///@param indexofhexgon: index of hexgon (starting from 1 to number of vertices or hexgon^2)
///@return NONE
void HexBoard::setEdgeValue(int indexofhexgon) {
  Node* node = findNodeByIndex(indexofhexgon);
  initNode(*node, indexofhexgon - 1);
  //symmetric assignment
  list<Edge> neigh = node->neighbors;
#if __cplusplus > 199711L
  for (auto e : neigh) {
#else
    for (typename list<Edge>::iterator iter = neigh.begin(); iter!=neigh.end(); ++iter) {
      Edge e = *iter;
#endif
    Node* neighbor = findNodeByIndex(e.indexoftonode);
    Edge edge;
    edge.indexoffromnode = neighbor->vertexindex;
    edge.indexoftonode = node->vertexindex;
    edge.weight = 1;
    neighbor->neighbors.push_back(edge);
    neighbor->numofneighbors = (neighbor->neighbors).size();
  }
  Graph<hexgonValKind, int>::setSizeOfEdges(numofedges);
}
///Set the number of hexgons and initialize the node as EMPTY and no neighbors
///@param numofhexgon: number of hexgon per side
///@return NONE
void HexBoard::setNumofhexgons(int numofhexgon) {
  initGraph();
  this->numofhexgons = numofhexgon;
  numofvertices = numofhexgon * numofhexgon;
  repgraph.clear();
  assert(repgraph.size() == 0);
  Graph<hexgonValKind, int>::setSizeOfVertices(numofvertices);
  for (unsigned i = 0; i < numofvertices; i++) {
    Node node;
    Graph<hexgonValKind, int>::initNode(node, i);
    repgraph.push_back(node);
  }
  resetHexBoard();
}
///Set the value of hexgon and push the move into move vector
///@param indexofnode the index of node whose value needs to be set
///@param value the value needs to be set
///@return NONE
void HexBoard::setNodeValue(int indexofnode, hexgonValKind value) {
  Graph<hexgonValKind, int>::setNodeValue(indexofnode, value);
  assert(emptyhexindicators.get() != nullptr);
  if (value == hexgonValKind_RED)
  redmoves.push_back(indexofnode);
  else
  bluemoves.push_back(indexofnode);
  emptyhexindicators.get()[indexofnode - 1] = false;
  numofemptyhexgons--;
}
///initialize empty hexgon indicators which can be used to track which position on hex board is empty or not
///@param NONE
///@return NONE
void HexBoard::initEmptyHexIndicators() {
  if (numofvertices) {
    emptyhexindicators = hexgame::shared_ptr<bool>(
        new bool[numofvertices], hexgame::default_delete<bool[]>());
    bool* ptrtoemptyhexindicators = emptyhexindicators.get();
    fill(ptrtoemptyhexindicators, ptrtoemptyhexindicators + numofvertices,
    true);
    numofemptyhexgons = numofvertices;
  }
}
///Reset the HexBoard to the initial state
///@param isresetedges the boolean variable which is used to indicate if the edges are all removed too
///TRUE: remove all the edges too (for restoring the initial state of playersboard) <br/>
///FALSE: not remove all the edges which has been established in game set up (for restoring the initial state of general board)<br/>
///@return NONE
void HexBoard::resetHexBoard(bool isresetedges) {
  assert(repgraph.size() == numofvertices);
  for (unsigned i = 0; i < numofvertices; i++) {
    repgraph[i].vertexvalue = hexgonValKind_EMPTY;
    if(isresetedges){
    repgraph[i].numofneighbors = 0;
    repgraph[i].neighbors.clear();
    }
  }
  if (isresetedges)
    numofedges = 0;

  Graph<hexgonValKind, int>::setIsundirected(isundirected);
  Graph<hexgonValKind, int>::setSizeOfEdges(numofedges);
  Graph<hexgonValKind, int>::setSizeOfVertices(numofvertices);

  initEmptyHexIndicators();
}
///Delete the edge between the specified nodes
///@param indexofnodefrom the vertexindex of the source node
///@param indexofnodeto the vertexindex of the destination node
///@return Output None
void HexBoard::deleteEdge(int indexofnodefrom, int indexofnodeto) {
  Graph<hexgonValKind, int>::deleteEdge(indexofnodefrom, indexofnodeto);
  numofedges = Graph<hexgonValKind, int>::getSizeOfEdges();
}
