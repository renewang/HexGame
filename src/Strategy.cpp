/*
 * Strategy.cpp
 * This file defines MC simulation implementation for AI player
 *
 */
#include <chrono>
#include "Strategy.h"

using namespace std;

Strategy::Strategy(const HexBoard* board, const Player* aiplayer)
    : AbstractStrategyImpl(board, aiplayer),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(3000) {
  numofhexgons = ptrtoboard->getNumofhexgons();
}
Strategy::Strategy(const HexBoard* board, const Player* aiplayer,
                   float threshold, float randomness)
    : AbstractStrategyImpl(board, aiplayer),
      threshold(threshold),
      randomness(randomness),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(3000) {
  numofhexgons = ptrtoboard->getNumofhexgons();
}
//simulation body
//INPUT: NONE
//OUTPUT:
//the index number of next step
int Strategy::simulation() {

  vector<unsigned> result(ptrtoboard->getSizeOfVertices(), 0);
  int cutoff = static_cast<int>(threshold
      * (float) (ptrtoboard->getSizeOfVertices()));

  shared_ptr<bool> emptyglobal;
  vector<int> bwglobal, oppglobal;
  initGameState(emptyglobal, bwglobal, oppglobal);

  int currentempty = ptrtoboard->getNumofemptyhexgons();

  //fill up the rest of the empty cells
  vector<pair<int, int> > counter(currentempty);

  vector<int> moves(bwglobal.size() + oppglobal.size());
  merge(bwglobal.begin(), bwglobal.end(), oppglobal.begin(), oppglobal.end(),
        moves.begin());

  unordered_set<int> allmoves(moves.begin(), moves.end());
  countNeighbors(emptyglobal, allmoves, counter);
  shared_ptr<bool> emptyindicators = shared_ptr<bool>(
      new bool[ptrtoboard->getSizeOfVertices()], default_delete<bool[]>());
  bool* ptrtoemptyglobal = emptyglobal.get();

  //start the simulation
  for (int i = 0; i < numberoftrials; i++) {

    //initialize the following containers to the current progress of playing board
    vector<int> babywatsons(bwglobal), opponents(oppglobal);
    copy(ptrtoemptyglobal, ptrtoemptyglobal + ptrtoboard->getSizeOfVertices(),
         emptyindicators.get());

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
        move = genNextFill(emptyindicators, emptyqueue, portionofempty);

      if (nextmove < 0)
        nextmove = move;

      babywatsons.push_back(move);

      //random generate a move for virtual opponent
      if (currentempty > cutoff && portionofempty > 0)
        oppmove = genNextRandom(emptyindicators, portionofempty);
      else if (portionofempty > 0)
        oppmove = genNextFill(emptyindicators, emptyqueue, portionofempty);

      opponents.push_back(oppmove);
    }
    int winner = checkWinnerExist(babywatsons, opponents);
    assert(nextmove != -1);
    assert(winner != 0);
    if (winner == 1)
      result[nextmove - 1]++;
  }

//find the move with the maximal successful simulated outcome
  vector<int> index(result.size());
  PriorityQueue<int, int> queue(result.size());
  for (unsigned i = 0; i < result.size(); i++) {
    index[i] = (i + 1);
    queue.insert(index[i], -1 * result[i]);
  }
#ifndef NDEBUG
    DEBUGHEADER();
    cerr <<"log the simulation statistics for final choice"<<endl;
    for (unsigned i = 0; i != result.size(); ++i)
      cerr << (i+1) << ":" << result[i] <<" ";
    cerr << endl;
#endif
  return queue.minPrioirty();
}
//initialize counter for neighbors
//INPUT:
//queue, priority queue used to store the
//moves, the current move made by one of player
//counter, the counter to record the number of neighbors
//OUTPUT: NONE
void Strategy::countNeighbors(shared_ptr<bool> emptyindicators,
                              unordered_set<int>& moves,
                              vector<pair<int, int> >& counter) {
  int countofempty = 0;
  for (int i = 0; i < ptrtoboard->getSizeOfVertices(); i++) {
    if (emptyindicators.get()[i]) {
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
int Strategy::genNextFill(shared_ptr<bool>& emptyindicators,
                          PriorityQueue<int, int>&queue, int& proportionofempty) {
  int nexloc = queue.minPrioirty();
  emptyindicators.get()[nexloc - 1] = false;
  --proportionofempty;
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

  long long seed = chrono::system_clock::now().time_since_epoch().count();
  default_random_engine generator(seed);

  //ensure assign unique number
  vector<int> numbers(currentempty);
  for (int i = 0; i < currentempty; i++)
    numbers[i] = (i + 1);
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
