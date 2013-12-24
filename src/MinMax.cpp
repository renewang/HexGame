/*
 * MinMax.cpp
 *
 */

#include <queue>
#include <vector>
#include <limits>
#include <algorithm>

#include "MinMax.h"

using namespace std;

MinMax::MinMax(const HexBoard* board, const Player* aiplayer)
    : AbstractStrategyImpl(board, aiplayer),
      ptrtoboard(board),
      ptrtoplayer(aiplayer) {
  numofhexgons = ptrtoboard->getNumofhexgons();
}
void MinMax::initPotentials(vector<int>& potentials) {
  vector<int> ends(numofhexgons);
  for (int i = 0; i < static_cast<int>(ends.size()); i++) {
    if (ptrtoplayer->getWestToEastCondition())
      ends[i] = ((i * numofhexgons + 1));  //col = 0
    else
      ends[i] = ((i + 1));  //row = 0
  }

  vector<priority_queue<int> > traceback;

//initialize
  for (int i = 0; i < numofhexgons; i++)
    potentials[ends[i] - 1] = 1;

  for (int i = 0; i < numofhexgons * numofhexgons; i++)
    traceback.push_back(priority_queue<int>());

//update minimums
  for (int i = 0; i < numofhexgons; i++) {
    for (int j = 0; j < numofhexgons; j++) {
      int index;
      if (ptrtoplayer->getWestToEastCondition())
        index = j * numofhexgons + i + 1;
      else
        index = i * numofhexgons + j + 1;

      vector<int> neighs = ptrtoboard->getNeighbors(index);
      for (auto n : neighs) {
        int val = potentials[n - 1] + 1;
        traceback[index - 1].push(-val);
      }
      //choose the second minimum
      auto found = find(ends.begin(), ends.begin() + numofhexgons, index);
      traceback[index - 1].pop();
      int secondmin = -traceback[index - 1].top();
      if ((found == ends.end() || found >= ends.begin() + numofhexgons)
          && secondmin < potentials[index - 1])
        potentials[index - 1] = secondmin;
    }
  }

#ifndef NDEBUG
  cout << "actual potentials (second minimum)" << endl;
  for (int i = 0; i < numofhexgons; i++) {
    for (int j = 0; j < numofhexgons; j++)
      cout << potentials[i * numofhexgons + j] << " ";
    cout << endl;
  }
#endif
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
