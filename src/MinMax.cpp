/*
 * MinMax.cpp
 *
 */
#include <vector>
#include <limits>
#include <algorithm>

#include "MinMax.h"

using namespace std;

MinMax::MinMax(const HexBoard* board, const Player* aiplayer)
    : ptrtoboard(board),
      ptrtoplayer(aiplayer) {
  numofhexgons = ptrtoboard->getNumofhexgons();
}
void MinMax::initPotentials(vector<int>& potentials) {
  vector<int> ends(2 * numofhexgons);
  for (int i = 0; i < static_cast<int>(ends.size()); i++) {
    if (ptrtoplayer->getWestToEastCondition()) {
      if (i < numofhexgons)
        ends[i] = ((i * numofhexgons + 1));  //col = 0
      else
        ends[i] = (((i - numofhexgons) * numofhexgons + numofhexgons));  //col = (numofhexgons - 1)
    } else {
      if (i < numofhexgons)
        ends[i] = ((i + 1));  //row = 0
      else
        ends[i] =
            (((numofhexgons - 1) * numofhexgons + (i - numofhexgons) + 1));  //row = (numofhexgons - 1)
    }
  }

  //initialize
  for (int i = 0; i < numofhexgons; i++)
    potentials[ends[i] - 1] = 1;

  for (int i = 0; i < numofhexgons; i++) {
    for (int j = 0; j < numofhexgons; j++) {
      int index;
      if (ptrtoplayer->getWestToEastCondition())
        index = j * numofhexgons + i + 1;
      else
        index = i * numofhexgons + j + 1;

      vector<int> neighs = ptrtoboard->getNeighbors(index);
      for (auto n : neighs) {
        int val = potentials[index - 1] + 1;
        if (val < potentials[n - 1])
          potentials[n - 1] = val;
      }
    }
  }

  //opposite direction
  for (int i = 0; i < numofhexgons; i++)
    potentials[ends[i + numofhexgons] - 1] = 1;

  for (int i = numofhexgons - 1; i >= 0; i--) {
    for (int j = numofhexgons - 1; j >= 0; j--) {
      int index;
      if (ptrtoplayer->getWestToEastCondition())
        index = j * numofhexgons + i + 1;
      else
        index = i * numofhexgons + j + 1;

      vector<int> neighs = ptrtoboard->getNeighbors(index);
      for (auto n : neighs) {
        int val = potentials[index - 1] + 1;
        if (val < potentials[n - 1])
          potentials[n - 1] = val;
      }
    }
  }
}
void MinMax::calcPotentials(vector<int>& potentials) {

}

int MinMax::simulation() {
//construct a game tree
//which evaluation function => Queenbee's two distance
//1) modify it as two distance
//2) calculate the total potential by summing the R-potential and B-potential
//recursive form, top-bottom and stop at the leaves
//a) self' position: 1) maximize the gain from the  level, 2) assign score to leaves
//b) opponent's position: 1) minimize the loss from
//combine a) and b) use the single min function (due to distance) to calculate the position for the next player
//the 2D matrices to store the potentials/distances
  vector<int> potentials(numofhexgons * numofhexgons, 0);
  int nextmove = -1;
  return nextmove;
}
int MinMax::genMove() {
  return (simulation());
}
//check if the winner exists for this stage of simulation
//INPUT:
//babywatsons: the proposed moves made by baby watson so far
//opponenets: the proposed moves made by virtual opponent so far
//OUTPUT:
//an integer indicates 0: no winner, -1, babywatson loses and 1 babywatson wins
int MinMax::checkWinnerExist(vector<int>& babywatsons, vector<int>& opponents) {
  if (isWinner(babywatsons, ptrtoplayer->getWestToEastCondition()))
    return 1;
  else if (isWinner(opponents, !ptrtoplayer->getWestToEastCondition()))
    return -1;
  else
    return 0;
}
//check if the test vector satisfies the winning condition
//INPUT:
//candidates: stores the moves
//iswestoeast: the boolean variable to indicate if the winning condition is west to east
//OUTPUT:
//the boolean variable indicates if this simulation wins the game
bool MinMax::isWinner(vector<int>& candidates, bool iswestoeast) {
  bool iswinner = false;

  vector<int> representative(candidates.size());
  vector<int> repsize(candidates.size(), 1);

  for (unsigned i = 0; i < candidates.size(); i++)
    representative[i] = i;

//link the edge by scanning pairwise node
  for (unsigned i = 0; i < (candidates.size() - 1); i++) {
    for (unsigned j = i + 1; j < candidates.size(); j++) {
      //temporary for test adjacent
      if (ptrtoboard->isAdjacent(candidates[i], candidates[j])
          && representative[i] != representative[j]) {
        int smallgroup = representative[j];
        int largegroup =
            (repsize[representative[i]] > repsize[representative[j]]) ?
                representative[i] : representative[j];
        if (largegroup != representative[i])
          smallgroup = representative[i];

        representative[smallgroup] = largegroup;
        for (unsigned k = 0; k < candidates.size(); k++)
          if (representative[k] == smallgroup) {
            representative[k] = largegroup;
            repsize[smallgroup]--;
            repsize[largegroup]++;
          }
      }
    }
  }

//find if there's a path satisfies the winning condition,
//should start from the largest group
  for (unsigned i = 0; i < candidates.size(); i++) {
    bool touchoneside = false;
    bool touchotherside = false;
    for (unsigned j = 0; j < candidates.size(); j++)
      if (representative[j] == static_cast<int>(i)) {
        int row = (candidates[j] - 1) / numofhexgons;
        int col = (candidates[j] - 1) % numofhexgons;
        if ((iswestoeast && col == 0) || (!iswestoeast && row == 0))
          touchoneside = true;
        else if ((iswestoeast && col == (numofhexgons - 1))
            || (!iswestoeast && row == (numofhexgons - 1)))
          touchotherside = true;
      }
    if (touchoneside && touchotherside) {
      iswinner = (touchoneside && touchotherside);
      break;
    }
  }
  return iswinner;
}
