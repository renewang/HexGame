/*
 * Strategy.cpp
 *
 *  Created on: Nov 20, 2013
 *      Author: renewang
 */

#include <set>
#include <cstdlib>
#include <algorithm>

#include "Strategy.h"
#include "PriorityQueue.h"

using namespace std;

Strategy::Strategy(const HexBoard* board, const Player* aiplayer)
    : ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(1000) {
}
//TODO: maybe pass a random generator which does some fancy things?
//generate the random next move representing with index [1, number of hexgon per side]
//INPUT: NONE
//OUPUT: NONE
int Strategy::genNextRandom(bool* emptyindicators) {
  bool isoccupied = true;
  int index = -1;
  srand(clock());

  while (isoccupied) {
    index = rand() % (ptrtoboard->getSizeOfVertices()) + 1;
    if (emptyindicators[index - 1]) {
      isoccupied = false;
      emptyindicators[index - 1] = false;
    }
  }
  return index;
}
//check if the winner exists for this stage of simulation
//INPUT:
//babywatsons: the proposed moves made by baby watson so far
//opponenets: the proposed moves made by virtual opponent so far
//OUTPUT:
//an integer indicates 0: no winner, -1, babywatson loses and 1 babywatson wins
int Strategy::checkWinnerExist(vector<int>& babywatsons,
                               vector<int>& opponents) {
  if (isWinner(babywatsons))
    return 1;
  else if (isWinner(opponents))
    return -1;
  else
    return 0;
}
//check if the test vector satisfies the winning condition
bool Strategy::isWinner(std::vector<int>& test) {
  int numofhexgon = ptrtoboard->getNumofhexgons();
  bool* connected = new bool[numofhexgon];
  bool iswinner = false;

  fill(connected, connected + numofhexgon, true);

  vector<set<int> > candidates(numofhexgon);
  for (int i = 0; i < numofhexgon; i++) {
    set<int> element;
    candidates[i] = element;
  }

  for (unsigned i = 0; i < test.size(); i++) {
    int row = (test[i] - 1) / ptrtoboard->getNumofhexgons();
    int col = (test[i] - 1) % ptrtoboard->getNumofhexgons();

    assert(row >= 0 && row < numofhexgon);
    assert(col >= 0 && col < numofhexgon);

    if (ptrtoplayer->getWestToEastCondition())
      candidates[col].insert(row);
    else
      candidates[row].insert(col);
  }

  assert((int )candidates.size() == numofhexgon);

  for (int i = 0; i < (numofhexgon - 1); i++) {
    if (candidates[i].empty() || candidates[i + 1].empty()) {
      fill(connected, connected + numofhexgon, false);
      break;
    }
    for (int j = 0; j < numofhexgon; j++) {
      if ((connected[j] || (j < (numofhexgon - 1) && connected[j + 1])
       || (j > 0 && connected[j - 1])) && candidates[i].count(j)
       && (candidates[i + 1].count(j) || candidates[i + 1].count(j - 1)
       || candidates[i].count(j + 1)))
        connected[j] = true;
      else
        connected[j] = false;
    }
  }

  for (int i = 0; i < numofhexgon; i++) {
    if (connected[i]) {
      iswinner = connected[i];
      break;
    }
  }

  delete[] connected;
  return iswinner;
}
//simulation body
int Strategy::simulation() {

  vector<unsigned> result(ptrtoboard->getSizeOfVertices(), 0);

  for (int i = 0; i < numberoftrials; i++) {

    //initialize the following containers to the current progress of playing board
    bool* emptyindicators = new bool[ptrtoboard->getSizeOfVertices()];
    vector<int> babywatsons, opponents;

    for (int j = 0; j < ptrtoboard->getSizeOfVertices(); j++) {
      //set the current empty location as true
      if (ptrtoboard->getNodeValue(j + 1) == hexgonValKind::EMPTY)
        emptyindicators[j] = true;
      else {
        emptyindicators[j] = false;
        if (ptrtoboard->getNodeValue(j + 1) == ptrtoplayer->getPlayerlabel())
          babywatsons.push_back(j + 1);
        else
          opponents.push_back(j + 1);
      }
    }

    int winner = 0, nextmove = -1;
    while (winner == 0) {

      //random generate a move for baby watson
      int move = genNextRandom(emptyindicators);
      if (nextmove < 0)
        nextmove = move;
      babywatsons.push_back(move);

      //random generate a move for virtual opponent
      opponents.push_back(genNextRandom(emptyindicators));
      winner = checkWinnerExist(babywatsons, opponents);
    }
    if (winner == 1)
      result[nextmove - 1]++;

    delete[] emptyindicators;
  }

//find the move with the maximal successful simulated outcome
  PriorityQueue<int, unsigned> queue(ptrtoboard->getSizeOfVertices());
  for (unsigned i = 0; i < result.size(); i++)
    queue.insert((i + 1), result[i]);

  return queue.minPrioirty();
}
int Strategy::genMove() {
  return (simulation());
}

