/*
 * Strategy.cpp
 * This file defines MC simulation implementation for AI player
 *
 */
#include <chrono>
#include <cstdlib>

#include "Strategy.h"

using namespace std;

Strategy::Strategy(const HexBoard* board, const Player* aiplayer)
    : ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(3000) {
  numofhexgons = ptrtoboard->getNumofhexgons();
}
Strategy::Strategy(const HexBoard* board, const Player* aiplayer,
                   float threshold, float randomness)
    : threshold(threshold),
      randomness(randomness),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(3000) {
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
//simulation body
//INPUT: NONE
//OUTPUT:
//the index number of next step
int Strategy::simulation() {

  vector<unsigned> result(ptrtoboard->getSizeOfVertices(), 0);
  int cutoff = static_cast<int>(threshold * (ptrtoboard->getSizeOfVertices()));

  //initialize the following containers to the current progress of playing board
  bool* emptyglobal = new bool[ptrtoboard->getSizeOfVertices()];
  vector<int> bwglobal, oppglobal;

  for (int j = 0; j < ptrtoboard->getSizeOfVertices(); j++) {
    //set the current empty location as true
    if (ptrtoboard->getNodeValue(j + 1) == hexgonValKind::EMPTY)
      emptyglobal[j] = true;
    else {
      emptyglobal[j] = false;
      result[j] = -1;
      if (ptrtoboard->getNodeValue(j + 1) == ptrtoplayer->getPlayerlabel())
        bwglobal.push_back(j + 1);
      else
        oppglobal.push_back(j + 1);
    }
  }
  int currentempty = count(emptyglobal,
                           emptyglobal + ptrtoboard->getSizeOfVertices(), true);

  //fill up the rest of the empty cells
  vector<pair<int, int> > counter(currentempty);

  vector<int> moves(bwglobal.size() + oppglobal.size());
  merge(bwglobal.begin(), bwglobal.end(), oppglobal.begin(), oppglobal.end(),
        moves.begin());

  unordered_set<int> allmoves(moves.begin(), moves.end());
  countNeighbors(emptyglobal, allmoves, counter);

  //start the simulation
  for (int i = 0; i < numberoftrials; i++) {

    //initialize the following containers to the current progress of playing board
    bool* emptyindicators = new bool[ptrtoboard->getSizeOfVertices()];
    vector<int> babywatsons(bwglobal), opponents(oppglobal);
    copy(emptyglobal, emptyglobal + ptrtoboard->getSizeOfVertices(),
         emptyindicators);
    PriorityQueue<int, int> emptyqueue(ptrtoboard->getSizeOfVertices());
    int portionofempty = currentempty;

    if (currentempty <= cutoff)
      assignRandomNeighbors(emptyqueue, counter, currentempty);

    int nextmove = -1;

    while (portionofempty > 0) {
      //random generate a move for baby watson
      int move, oppmove;
      if (currentempty > cutoff)
        move = genNextRandom(emptyindicators, portionofempty);
      else
        //fill up the board by non-random
        move = genNextFill(emptyindicators, emptyqueue);

      if (nextmove < 0)
        nextmove = move;

      babywatsons.push_back(move);
      portionofempty--;

      //random generate a move for virtual opponent
      if (currentempty > cutoff && portionofempty > 0)
        oppmove = genNextRandom(emptyindicators, portionofempty);
      else if (portionofempty > 0)
        oppmove = genNextFill(emptyindicators, emptyqueue);

      opponents.push_back(oppmove);
      portionofempty--;
    }
    int winner = checkWinnerExist(babywatsons, opponents);
    assert(nextmove != -1);
    assert(winner != 0);
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

  delete[] emptyglobal;

  return queue.minPrioirty();
}
//initialize counter for neighbors
//INPUT:
//queue, priority queue used to store the
//moves, the current move made by one of player
//counter, the counter to record the number of neighbors
//OUTPUT: NONE
void Strategy::countNeighbors(bool* emptyindicators, unordered_set<int>& moves,
                              vector<pair<int, int> >& counter) {
  int countofempty = 0;
  for (int i = 0; i < ptrtoboard->getSizeOfVertices(); i++) {
    if (emptyindicators[i]) {
      vector<int> indexofneigh = ptrtoboard->getNeighbors((i + 1));
      int neighbors = 0;
      for (auto v : indexofneigh)
        neighbors += moves.count(v);

      counter[countofempty].first = (i + 1);
      counter[countofempty++].second = neighbors;
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
int Strategy::genNextFill(bool* emptyindicators, PriorityQueue<int, int>&queue) {
  int nexloc = queue.minPrioirty();
  emptyindicators[nexloc - 1] = false;
  return nexloc;
}
//assign random number to neighbors counter
//INPUT:
//queue: priority queue stores the number of neighbors for each empty cell of current game
//counter: vector stores pair of index value of empty cells and count of neighbors
//OUTPUT: NONE
void Strategy::assignRandomNeighbors(PriorityQueue<int, int>& queue,
                                     vector<pair<int, int> >& counter,
                                     int currentempty) {

  unsigned long seed = chrono::system_clock::now().time_since_epoch().count();
  default_random_engine generator(seed);

  //ensure assign unique number
  vector<int> numbers(currentempty);
  for (int i = 0; i < currentempty; i++)
    numbers[i] = (i+1);
  shuffle(numbers.begin(), numbers.end(), generator);

  uniform_real_distribution<float> probability(0.0, 1.0);

  for (int i = 0; i < currentempty; i++) {
    float prob = probability(generator);
    //if randomness = 1, then doing shuffle
    int weight = static_cast<int>((counter[i].second) * (1.0 - randomness));
    if (prob <= randomness)
      queue.insert(counter[i].first, -(weight + numbers[i]));
    else
      queue.insert(counter[i].first, -weight);
  }
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
//INPUT:
//candidates: stores the moves
//iswestoeast: the boolean variable to indicate if the winning condition is west to east
//OUTPUT:
//the boolean variable indicates if this simulation wins the game
bool Strategy::isWinner(vector<int>& candidates, bool iswestoeast) {
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
//the genMove called by Game
//INPUT: NONE
//OUTPUT:
//the next move evaluated by simulation
int Strategy::genMove() {
  return (simulation());
}
