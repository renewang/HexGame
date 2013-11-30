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
      numberoftrials(3000) {
}
//TODO: maybe pass a random generator which does some fancy things?
//generate the random next move representing with index [1, number of hexgon per side]
//INPUT: NONE
//OUPUT: NONE
int Strategy::genNextRandom(bool* emptyindicators, unsigned proportionofempty) {
  bool isoccupied = true;
  int index = -1;
  srand(clock());

  while (isoccupied && proportionofempty > 0) {
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
  if (isWinner(babywatsons, ptrtoplayer->getWestToEastCondition()))
    return 1;
  else if (isWinner(opponents, !ptrtoplayer->getWestToEastCondition()))
    return -1;
  else
    return 0;
}
//check if the test vector satisfies the winning condition
//TODO This is extremely hideous. Need a rework.
bool Strategy::isWinner(std::vector<int>& test, bool iswestoeast) {
  int numofhexgon = ptrtoboard->getNumofhexgons();
  bool* connected = new bool[numofhexgon];  //indicator of connected path leading to the positions in previous row
  bool iswinner = true;

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

    if (iswestoeast)
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
              || candidates[i].count(j + 1) || candidates[i].count(j - 1)))
        connected[j] = true;
      else
        connected[j] = false;
    }

    //reverse
    for (int k = (numofhexgon - 1); k >= 0; k--) {
      if ((connected[k] || (k < (numofhexgon - 1) && connected[k + 1])
          || (k > 0 && connected[k - 1])) && candidates[i].count(k)
          && (candidates[i + 1].count(k) || candidates[i + 1].count(k - 1)
              || candidates[i].count(k + 1) || candidates[i].count(k - 1)))
        connected[k] = true;
      else
        connected[k] = false;
    }
    //backward path detection
    int start = 2, end = start, b = start;
    if (i > 0 && i < (numofhexgon - 2)) {
      while (b < (numofhexgon - 1)) {
        if (candidates[i].count(b) && (candidates[i + 1].count(b - 1))
            && !connected[b])  //potential backward path
            {
          start = b;
          int k = b + 1;
          while (candidates[i].count(k))
            k++;
          end = k - 1;
          if (end - start >= 1
              && (candidates[i + 1].count(end)
                  || candidates[i + 1].count(end - 1))) {
            for (int c = start; c <= end; c++)
              connected[c] = true;
          }
        }
        b += (end - start + 1);
      }
    }
    //not allowed a all false connected
    if (find(connected, connected + numofhexgon, true)
        == connected + numofhexgon)
      break;
  }
  for (int i = 0; i < numofhexgon; i++) {
    iswinner = connected[i]
        && (candidates[numofhexgon - 1].count(i)
            || candidates[numofhexgon - 1].count(i - 1));
    if (iswinner)
      break;
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
    unsigned portionofempty = count(
        emptyindicators, emptyindicators + ptrtoboard->getSizeOfVertices(),
        true);

    int nextmove = -1;
    //random sampling
    while (portionofempty > 0) {

      //random generate a move for baby watson
      if (portionofempty > threshold) {
        int move = genNextRandom(emptyindicators, portionofempty);
        if (nextmove < 0)
          nextmove = move;
        babywatsons.push_back(move);
        portionofempty--;
      } else if(portionofempty > 0){  //fill up the board by non-random
        int move = genNextFill(emptyindicators, babywatsons,
                               ptrtoplayer->getWestToEastCondition());
        babywatsons.push_back(move);
        portionofempty--;
      }

      //random generate a move for virtual opponent
      if (portionofempty > threshold) {
        opponents.push_back(genNextRandom(emptyindicators, portionofempty));
        portionofempty--;
      } else if(portionofempty > 0){  //fill up the board by non-random
        int move = genNextFill(emptyindicators, opponents,
                               !ptrtoplayer->getWestToEastCondition());
        opponents.push_back(move);
        portionofempty--;
      }
    }

    int winner = checkWinnerExist(babywatsons, opponents);
    //assert(winner != 0);
    if (winner == 1)
      result[nextmove - 1]++;

    delete[] emptyindicators;
  }

//find the move with the maximal successful simulated outcome
//somehow doesn't work
  vector<int> index(result.size());
  PriorityQueue<int, int> queue(result.size());
  for (unsigned i = 0; i < result.size(); i++) {
    index[i] = (i + 1);
    queue.insert(index[i], -1 * result[i]);
  }
  return queue.minPrioirty();
}
int Strategy::genMove() {
  return (simulation());
}
int Strategy::genNextFill(bool* emptyindicators, std::vector<int>& moves,
                          bool iswestoeast) {
  int numofhexgon = ptrtoboard->getNumofhexgons();
  vector<set<int> > candidates(numofhexgon);
  for (int i = 0; i < numofhexgon; i++) {
    set<int> element;
    candidates[i] = element;
  }

  for (unsigned i = 0; i < moves.size(); i++) {
    int row = (moves[i] - 1) / ptrtoboard->getNumofhexgons();
    int col = (moves[i] - 1) % ptrtoboard->getNumofhexgons();

    assert(row >= 0 && row < numofhexgon);
    assert(col >= 0 && col < numofhexgon);

    if (iswestoeast)
      candidates[col].insert(row);
    else
      candidates[row].insert(col);
  }
  assert((int )candidates.size() == numofhexgon);
//find the empty location with most connected moves
  int maxcnt = -1, maxcntloc = -1;
  for (int i = 0; i < (numofhexgon * numofhexgon); i++) {
    if (emptyindicators[i]) {
      int rowofempty = i / numofhexgon;
      int colofempty = i % numofhexgon;
      int neighbors;
      if (rowofempty > 0 && rowofempty < (numofhexgon - 1)) {
        neighbors = candidates[rowofempty - 1].count(colofempty)  //-1, 0
        + candidates[rowofempty - 1].count(colofempty + 1)  //-1, 1
            + candidates[rowofempty].count(colofempty - 1)  //0, -1
            + candidates[rowofempty].count(colofempty + 1)  //0, 1
            + candidates[rowofempty + 1].count(colofempty)  //1, 0
            + candidates[rowofempty + 1].count(colofempty - 1);  //1, -1
      } else if (rowofempty == 0) {
        neighbors = candidates[rowofempty].count(colofempty - 1)  //0, -1
        + candidates[rowofempty].count(colofempty + 1)  //0, 1
            + candidates[rowofempty + 1].count(colofempty)  //1, 0
            + candidates[rowofempty + 1].count(colofempty - 1);  //1, -1
      } else if (rowofempty == (numofhexgon - 1)) {
        neighbors = candidates[rowofempty - 1].count(colofempty)  //-1, 0
                + candidates[rowofempty - 1].count(colofempty + 1)  //-1, 1
                    + candidates[rowofempty].count(colofempty - 1)  //0, -1
                    + candidates[rowofempty].count(colofempty + 1);  //0, 1
      }
      if (neighbors > maxcnt) {
        maxcnt = neighbors;
        maxcntloc = (i + 1);
      }
    }
  }
  assert(maxcntloc != -1);
  emptyindicators[maxcntloc - 1] = false;
  return maxcntloc;
}

