/*
 * TwoDistancePotential.cpp
 *
 *  Created on: Dec 25, 2013
 *      Author: renewang
 */
#include <queue>
#include <vector>
#include <limits>
#include <algorithm>

#include "TwoDistancePotential.h"

using namespace std;

TwoDistancePotential::TwoDistancePotential(const HexBoard* board,
                                           const Player* aiplayer)
    : ptrtoboard(board),
      ptrtoplayer(aiplayer),
      potentials(
          vector<int>(ptrtoboard->getSizeOfVertices(),
                      numeric_limits<int>::max())),
      ends(vector<int>(numofhexgons)) {
  numofhexgons = ptrtoboard->getNumofhexgons();
  initPotentials();
}
void TwoDistancePotential::initPotentials() {
  for (int i = 0; i < static_cast<int>(ends.size()); i++) {
    if (ptrtoplayer->getWestToEastCondition())
      ends[i] = ((i * numofhexgons + 1));  //col = 0
    else
      ends[i] = ((i + 1));  //row = 0
  }

//initialize
  for (int i = 0; i < numofhexgons; i++)
    potentials[ends[i] - 1] = 1;

  calcPotentials();

}
void TwoDistancePotential::calcPotentials() {
  vector<priority_queue<int> > traceback;

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
