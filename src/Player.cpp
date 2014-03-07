/*
 * Player.cpp
 * This file defines the implementation for the Player class.
 */

#include <vector>
#include "Player.h"
#include "MinSpanTreeAlgo.h"

using namespace std;
//To move to the specified location
//INPUT:
//indexofrow: index of row, starting from 1 to number of hexgon per side
//indexofcol: index of column, starting from 1 to number of hexgon per side
//OUTPUT:
//a boolean variable to indicate if this move is successful or not
bool Player::setMove(int indexofrow, int indexofcol) {
  bool issuccess = false;
  int numofhexgon = board.getNumofhexgons();
  int index = (indexofrow - 1) * numofhexgon + (indexofcol - 1) + 1;
  assert(index > 0);
#if __cplusplus > 199711L
  if (board.getNodeValue(index) == hexgonValKind::EMPTY)
#else
  if (board.getNodeValue(index) == EMPTY)
#endif
  {
    board.setNodeValue(index, this->playerkind);
    setPlayerBoard(index);
    issuccess = true;
  }
  return issuccess;
}
//To test if player has moved to opposite side
//INPUT: NONE
//OUTPTU:
//a boolean indicator to indicate if this player's move reached the other side
bool Player::isArriveOpposite() {
  bool isendfound = false, isotherendfound = false;
  //calculate current MST, actually is a MSF (Minimal Spanning Forests)
  Graph<hexgonValKind, int> msttree = calMST();
  //to check if end-to-end path existing in MST
  vector<vector<int> > subgraphs = msttree.getAllSubGraphs();
  int numofhexgons = board.getNumofhexgons();
#if __cplusplus > 199711L
  for (auto vec : subgraphs) {
    isendfound = false;
    isotherendfound = false;
    for (auto i : vec) {
      int row = (i - 1) / numofhexgons;
      int col = (i - 1) % numofhexgons;
      if (this->condition == winConditions::WESTTOEAST) {
        if (col == 0)
          isendfound = true;
        else if (col == (numofhexgons - 1))
          isotherendfound = true;
      } else if (this->condition == winConditions::NORTHTOSOUTH) {
        if (row == 0)
          isendfound = true;
        else if (row == (numofhexgons - 1))
          isotherendfound = true;
      }
    }
    if (isendfound && isotherendfound)
      break;
  }
#else
  vector<vector<int> >::iterator iterofsubgraph = subgraphs.begin();
  for (;iterofsubgraph!=subgraphs.end(); ++iterofsubgraph) {
    isendfound = false;
    isotherendfound = false;
    vector<int>::iterator iterofelem = (*iterofsubgraph).begin();
    for (;iterofelem!=(*iterofsubgraph).end(); ++iterofelem) {
      int i = *(iterofelem);
      int row = (i - 1) / numofhexgons;
      int col = (i - 1) % numofhexgons;
      if (this->condition == WESTTOEAST) {
        if (col == 0)
          isendfound = true;
        else if (col == (numofhexgons - 1))
          isotherendfound = true;
      } else if (this->condition == NORTHTOSOUTH) {
        if (row == 0)
          isendfound = true;
        else if (row == (numofhexgons - 1))
          isotherendfound = true;
      }
    }
    if (isendfound && isotherendfound)
      break;
  }
#endif
  return (isendfound && isotherendfound);
}
//Call Minimal Spanning Tree calculation in order to calculate the connected path for every move the player has made
//INPUT: NONE
//OUTPUT:
//a graph which holds minimal spanning tree for all of moves made by the player
Graph<hexgonValKind, int> Player::calMST() {
  MinSpanTreeAlgo<hexgonValKind, int> mstalgo(playersboard);
  MinSpanTreeAlgo<hexgonValKind, int>::UnionFind unionfind(mstalgo);
  mstalgo.calculate(unionfind);
  Graph<hexgonValKind, int> msttree = mstalgo.getMsttree();
  return Graph<hexgonValKind, int>(msttree);
}
//Private Method to set the connected edges hidden from player's private board
//INPUT:
//indexofrow: index of row, starting from 1 to number of hexgon per side
//indexofcol: index of column, starting from 1 to number of hexgon per side
//OUTPTU: NONE
void Player::setPlayerBoard(int index) {
  //initialize a
  playersboard.setEdgeValue(index);
  playersboard.setNodeValue(index, playerkind);
  //remove the edges without the same label
  vector<int> neighbors = playersboard.getNeighbors(index);
#if __cplusplus > 199711L
  for (auto i : neighbors) {
    if (playersboard.getNodeValue(i) != playerkind)
      playersboard.deleteEdge(index, i);
  }
#else
  for (vector<int>::iterator iter = neighbors.begin(); iter != neighbors.end(); ++iter) {
    if (playersboard.getNodeValue(*iter) != playerkind)
      playersboard.deleteEdge(index, *iter);
  }
#endif
}
