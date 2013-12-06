/*
 * Strategy.cpp
 * This file defines MC simulation implementation for AI player
 *
 */

#include <set>
#include <cstdlib>
#include <algorithm>

#include "Strategy.h"

using namespace std;

Strategy::Strategy(const HexBoard* board, const Player* aiplayer)
    : ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(1000) {
  numofhexgons = ptrtoboard->getNumofhexgons();
}
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
int Strategy::checkWinnerExist(vector<set<int> >& babywatsons,
                               vector<set<int> >& opponents) {
  if (isWinner(babywatsons, ptrtoplayer->getWestToEastCondition()))
    return 1;
  else if (isWinner(opponents, !ptrtoplayer->getWestToEastCondition()))
    return -1;
  else
    return 0;
}
//check if the test vector satisfies the winning condition
//TODO This is extremely hideous. Need a rework.
//INPUT:
//candidates: stores the moves
//iswestoeast: the boolean variable to indicate if the winning condition is west to east
//OUTPUT:
//the boolean variable indicates if this simulation wins the game
bool Strategy::isWinner(vector<set<int> >& candidates, bool iswestoeast) {
  int numofhexgon = ptrtoboard->getNumofhexgons();

  bool* forward = new bool[numofhexgon];
  bool* backward = new bool[numofhexgon];
  bool iswinner = true;

  fill(forward, forward + numofhexgon, false);
  fill(backward, backward + numofhexgon, false);

  for (int j = 0; j < numofhexgon; j++) {
    if (candidates[0].count(j))
      forward[j] = true;
    if (candidates[numofhexgon - 1].count(j))
      backward[j] = true;
  }
  for (int i = 1, k = numofhexgon - 1 - i; i < numofhexgon || k > 0; i++, k--) {
    if (candidates[i].empty() || candidates[k].empty()) {
      fill(forward, forward + numofhexgon, false);
      fill(backward, backward + numofhexgon, false);
      break;
    }
    for (int j = 0; j < numofhexgon; j++) {
      if (j < (numofhexgon - 1) && j > 0) {

        if ((forward[j] || forward[j + 1]) && candidates[i].count(j))
          forward[j] = true;
        else
          forward[j] = false;

        if ((backward[j] || backward[j - 1]) && candidates[k].count(j))
          backward[j] = true;
        else
          backward[j] = false;

      } else if (j == 0) {  //boundary
        if ((forward[j] || forward[j + 1]) && candidates[i].count(j))
          forward[j] = true;
        else
          forward[j] = false;

        if (backward[j] && candidates[k].count(j))
          backward[j] = true;
        else
          backward[j] = false;

      } else if (j == (numofhexgon - 1)) {  //boundary

        if (forward[j] && candidates[i].count(j))
          forward[j] = true;
        else
          forward[j] = false;

        if ((backward[j] || backward[j - 1]) && candidates[k].count(j))
          backward[j] = true;
        else
          backward[j] = false;
      }
    }
    for (int j = 1; j < numofhexgon; j++) {
      if (forward[j - 1] && candidates[i].count(j))
        forward[j] = true;
      if (backward[j - 1] && candidates[k].count(j))
        backward[j] = true;
    }

    for (int j = (numofhexgon - 2); j >= 0; j--) {
      if (forward[j + 1] && candidates[i].count(j))
        forward[j] = true;

      if (backward[j + 1] && candidates[k].count(j))
        backward[j] = true;
    }
  }
  for (int i = 0; i < numofhexgon; i++) {
    iswinner = (backward[i] || forward[i]);
    if (iswinner)
      break;
  }

  delete[] backward;
  delete[] forward;
  return iswinner;
}
//simulation body
//INPUT: NONE
//OUTPUT:
//the index number of next step
int Strategy::simulation() {

  vector<unsigned> result(ptrtoboard->getSizeOfVertices(), 0);
  int cutoff = threshold * (ptrtoboard->getSizeOfVertices());

  for (int i = 0; i < numberoftrials; i++) {

    //initialize the following containers to the current progress of playing board
    bool* emptyindicators = new bool[ptrtoboard->getSizeOfVertices()];
    vector<set<int> > babywatsons(numofhexgons), opponents(numofhexgons);

    initTransformVector(babywatsons);
    initTransformVector(opponents);

    for (int j = 0; j < ptrtoboard->getSizeOfVertices(); j++) {
      //set the current empty location as true
      if (ptrtoboard->getNodeValue(j + 1) == hexgonValKind::EMPTY)
        emptyindicators[j] = true;
      else {
        emptyindicators[j] = false;
        result[j] = -1;
        if (ptrtoboard->getNodeValue(j + 1) == ptrtoplayer->getPlayerlabel())
          assignValue(babywatsons, j + 1,
                      ptrtoplayer->getWestToEastCondition());
        else
          assignValue(opponents, j + 1, !ptrtoplayer->getWestToEastCondition());
      }
    }
    int portionofempty = count(
        emptyindicators, emptyindicators + ptrtoboard->getSizeOfVertices(),
        true);
    int nextmove = -1;
    //random sampling
    while (portionofempty > cutoff) {
      //random generate a move for baby watson
      int move = genNextRandom(emptyindicators, portionofempty);
      if (nextmove < 0)
        nextmove = move;
      assignValue(babywatsons, move, ptrtoplayer->getWestToEastCondition());
      portionofempty--;

      //random generate a move for virtual opponent
      if (portionofempty > cutoff) {
        move = genNextRandom(emptyindicators, portionofempty);
        assignValue(opponents, move, !ptrtoplayer->getWestToEastCondition());
        portionofempty--;
      }
    }
    //fill up the rest of the empty cells
    PriorityQueue<int, int> emptyqueue(ptrtoboard->getSizeOfVertices());
    vector<pair<int, int> > counter(ptrtoboard->getSizeOfVertices());
    for (int j = 0; j < ptrtoboard->getSizeOfVertices(); j++)
      counter[j] = make_pair((j + 1), 0);

    countNeighbors(emptyqueue, emptyindicators, babywatsons, counter);
    countNeighbors(emptyqueue, emptyindicators, opponents, counter);

    while (portionofempty > 0) {
      //fill up the board by non-random
      int move = genNextFill(emptyindicators, emptyqueue, counter);
      if (nextmove < 0)
        nextmove = move;

      assignValue(babywatsons, move, ptrtoplayer->getWestToEastCondition());
      portionofempty--;

      //fill up the board by non-random
      if (portionofempty > 0) {
        move = genNextFill(emptyindicators, emptyqueue, counter);
        assignValue(opponents, move, !ptrtoplayer->getWestToEastCondition());
        portionofempty--;
      }
    }

    int winner = checkWinnerExist(babywatsons, opponents);
    assert(nextmove != -1);
    //assert(winner != 0);
    if (winner == 1)
      result[nextmove - 1]++;

    delete[] emptyindicators;
  }

//find the move with the maximal successful simulated outcome
  vector<int> index(result.size());
  PriorityQueue<int, int> queue(result.size());
  for (unsigned i = 0; i < result.size(); i++) {
    index[i] = (i + 1);
    queue.insert(index[i], -1 * result[i]);
  }
  return queue.minPrioirty();
}
//the genMove called by Game
//INPUT: NONE
//OUTPUT:
//the next move evaluated by simulation
int Strategy::genMove() {
  return (simulation());
}
void Strategy::countNeighbors(PriorityQueue<int, int>& queue,
                              bool* emptyindicators, vector<set<int> >& moves,
                              vector<pair<int, int>>& counter) {
  for (int i = 0; i < ptrtoboard->getSizeOfVertices(); i++) {
    if (emptyindicators[i]) {
      //adding randomness
      srand(clock());
      float prob = (float) rand() / RAND_MAX;
      int neighbors;
      int rowofempty = i / numofhexgons;
      int colofempty = i % numofhexgons;
      if (rowofempty > 0 && rowofempty < (numofhexgons - 1)) {
        neighbors = moves[rowofempty - 1].count(colofempty)  //-1, 0
        + moves[rowofempty - 1].count(colofempty + 1)  //-1, 1
            + moves[rowofempty].count(colofempty - 1)  //0, -1
            + moves[rowofempty].count(colofempty + 1)  //0, 1
            + moves[rowofempty + 1].count(colofempty)  //1, 0
            + moves[rowofempty + 1].count(colofempty - 1);  //1, -1
      } else if (rowofempty == 0) {
        neighbors = moves[rowofempty].count(colofempty - 1)  //0, -1
        + moves[rowofempty].count(colofempty + 1)  //0, 1
            + moves[rowofempty + 1].count(colofempty)  //1, 0
            + moves[rowofempty + 1].count(colofempty - 1);  //1, -1
      } else if (rowofempty == (numofhexgons - 1)) {
        neighbors = moves[rowofempty - 1].count(colofempty)  //-1, 0
        + moves[rowofempty - 1].count(colofempty + 1)  //-1, 1
            + moves[rowofempty].count(colofempty - 1)  //0, -1
            + moves[rowofempty].count(colofempty + 1);  //0, 1
      }
      if (prob <= randomness)
        neighbors += rand() % ptrtoboard->getSizeOfVertices();
      if (neighbors >= counter[i].second) {
        counter[i].second = neighbors;
        if (queue.contains(counter[i].first))
          queue.chgPrioirity(counter[i].first, -1 * counter[i].second);
        else
          queue.insert(counter[i].first, -1 * counter[i].second);
      }
    }
  }
}
//Generate next move by taking into account the most connected cells.
//INPUT:
//emptyindicators: boolean array to indicate the empty positions
//queue: the priority queue stores the empty positions with the priority as the connections to the position adding some randomness
//counter: the auxiliary vector which stores the index of position and priority
//OUPUT:
//next move
int Strategy::genNextFill(bool* emptyindicators, PriorityQueue<int, int>&queue,
                          vector<pair<int, int> >& counter) {
  int nexloc = queue.minPrioirty();
  emptyindicators[nexloc - 1] = false;
  return nexloc;
}
//Utility function to assign new move into a vector
//INPUT:
//moves: the vector stores the past moves made by the simulation
//move: the new move needs to be assigned to the moves vector
//iswestoeast: boolean variable to indicate if the winning conditioni is west to east
//OUPUT: NONE
void Strategy::assignValue(vector<set<int> >& moves, int move,
                           bool iswestoeast) {
  int row = (move - 1) / numofhexgons;
  int col = (move - 1) % numofhexgons;

  assert(row >= 0 && row < numofhexgons);
  assert(col >= 0 && col < numofhexgons);
  assert((row * numofhexgons + col + 1) == move);

  if (iswestoeast)
    moves[col].insert(row);
  else
    moves[row].insert(col);
}
//Utility function to initialize the vector which store the moves generated by simulation
//INPUT:
//moves: the vector stores the past moves made by the simulation
//OUPUT: NONE
void Strategy::initTransformVector(vector<set<int> >& moves) {
  for (int j = 0; j < numofhexgons; j++) {
    set<int> element;
    moves[j] = element;
  }
}
