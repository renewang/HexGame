/*
 * HexBoard.cpp
 * This file defines the implementation for the HexBoard class.
 */

#include "HexBoard.h"

using namespace std;
//default constructor, doing nothing except initializing the member values as defaults
HexBoard::HexBoard()
    : numofhexgons(0),
      numofemptyhexgons(0) {
  initGraph();
}
//constructor to initialize the board according to given hexgon size per side
HexBoard::HexBoard(unsigned numofhexgon)
    : numofhexgons(numofhexgon) {
  initGraph();
  numofvertices = numofhexgon * numofhexgon;
  for (unsigned i = 0; i < numofvertices; i++) {
    Node node;
    initNode(node, i);
    repgraph.push_back(node);
  }
  numofedges /= 2;
  initEmptyHexIndicators();
}
//destructor
HexBoard::~HexBoard() {
}
//private member to initialize the node according to their location
//INPUT:
//node: the intended to initialized node
//index: the identifier of the node
//OUTPUT: NONE
void HexBoard::initNode(Node& node, int index) {
  //Assign the corresponding edges according to the hexgonloctype
  //Assign all empty to all hexgons
  node.vertexindex = (index + 1);
#if __cplusplus > 199711L
  node.vertexvalue = hexgonValKind::EMPTY;
#else
  node.vertexvalue = EMPTY;
#endif
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
//To initialize members of the hex board as default values
//INPUT: NONE
//OUTPUT: NONE
void HexBoard::initGraph() {
  numofedges = 0;
  numofvertices = 0;
  isundirected = true;
}
//To get the location type given the index of row and column
//INPUT:
//row: the index of row (starting from 1 to number of hexgons per side)
//col: the index of row (starting from 1 to number of hexgons per side)
//OUTPUT:
//the location object of HexBoard::HexgonLocType::numLocEdges
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
//To output the hexgonkind
//INPUT:
//os: output stream type
//hexgonkind: the intended hexgonkind for output
//OUTPUT:
//output stream type for pipe
ostream& operator<<(ostream& os, hexgonValKind hexgonkind) {
#if __cplusplus > 199711L
  switch (hexgonkind) {
    case hexgonValKind::EMPTY:
      os << "EMPTY";
      break;
    case hexgonValKind::RED:
      os << "RED";
      break;
    case hexgonValKind::BLUE:
      os << "BLUE";
      break;
  }
#else
  switch (hexgonkind) {
    case EMPTY:
      os << "EMPTY";
      break;
    case RED:
      os << "RED";
      break;
    case BLUE:
      os << "BLUE";
      break;
  }
#endif
  return os;
}
//Set the edge value according to the index of hexgon
//INPUT:
//indexofhexgon: index of hexgon (starting from 1 to number of vertices or hexgon^2)
//OUTPUT: NONE
void HexBoard::setEdgeValue(int indexofhexgon) {
  Node* node = findNodeByIndex(indexofhexgon);
  initNode(*node, indexofhexgon - 1);
  //symmetric assignment
  list<Edge> neigh = node->neighbors;
#if __cplusplus > 199711L
  for (auto e : neigh) {
    Node* neighbor = findNodeByIndex(e.indexoftonode);
    Edge edge;
    edge.indexoffromnode = neighbor->vertexindex;
    edge.indexoftonode = node->vertexindex;
    edge.weight = 1;
    neighbor->neighbors.push_back(edge);
    neighbor->numofneighbors = (neighbor->neighbors).size();
  }
#else
  typename list<Edge>::iterator iter = neigh.begin();
  for ( ; iter!=neigh.end(); ++iter) {
    Node* neighbor = findNodeByIndex((*iter).indexoftonode);
    Edge edge;
    edge.indexoffromnode = neighbor->vertexindex;
    edge.indexoftonode = node->vertexindex;
    edge.weight = 1;
    neighbor->neighbors.push_back(edge);
    neighbor->numofneighbors = (neighbor->neighbors).size();
  }
#endif
}
//Set the number of hexgons and initialize the node as EMPTY and no neighbors
//INPUT:
//numofhexgon: number of hexgon per side
//OUTPUT: NONE
void HexBoard::setNumofhexgons(int numofhexgon) {
  this->numofhexgons = numofhexgon;
  initGraph();
  numofvertices = numofhexgon * numofhexgon;
  for (unsigned i = 0; i < numofvertices; i++) {
    Node node;
    node.vertexindex = (i + 1);
#if __cplusplus > 199711L
    node.vertexvalue = hexgonValKind::EMPTY;
#else
    node.vertexvalue = EMPTY;
#endif
    node.numofneighbors = 0;
    repgraph.push_back(node);
  }
  numofedges = 0;
  initEmptyHexIndicators();
}
//Set the valud of hexgon and push the move into move vector
//INPUT:
//indexofnode: the index of node whose value needs to be set
//value: the value needs to be set
//OUTPUT:
void HexBoard::setNodeValue(int indexofnode, hexgonValKind value) {
  Graph<hexgonValKind, int>::setNodeValue(indexofnode, value);
#if __cplusplus > 199711L
  assert(emptyhexindicators.get() != nullptr);
  if (value == hexgonValKind::RED)
#else
    assert(emptyhexindicators.get() != 0);
    if (value == RED)
#endif
    redmoves.push_back(indexofnode);
  else
    bluemoves.push_back(indexofnode);
  emptyhexindicators.get()[indexofnode - 1] = false;
  numofemptyhexgons--;
}
void HexBoard::initEmptyHexIndicators() {
  if (numofvertices) {
    emptyhexindicators = hexgame::shared_ptr<bool>(new bool[numofvertices],
                                                 hexgame::default_delete<bool[]>());
    bool* ptrtoemptyhexindicators = emptyhexindicators.get();
    fill(ptrtoemptyhexindicators, ptrtoemptyhexindicators + numofvertices,
         true);
    numofemptyhexgons = numofvertices;
  }
}
