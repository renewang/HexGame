/**
 * Strategy.cpp
 * This file defines a pure/naive Mont Carlo simulation implementation for AI player
 *
 *  Created on:
 *      Author: renewang
 */
#if __cplusplus <= 199711L
#include <boost/random/mersenne_twister.hpp>
#endif

#include "Global.h"
#include "Strategy.h"

using namespace std;

#if __cplusplus > 199711L
Strategy::Strategy(const HexBoard* board, const Player* aiplayer)
    : Strategy(board, aiplayer, 0.3, 0.7, 2048) {
}
Strategy::Strategy(const HexBoard* board, const Player* aiplayer,
                   double threshold, double randomness)
    : Strategy(board, aiplayer, threshold, randomness, 2048) {
}
Strategy::Strategy(const HexBoard* board, const Player* aiplayer,
                   size_t numberoftrials)
    : Strategy(board, aiplayer, 0.3, 0.7, numberoftrials) {
}
#else
Strategy::Strategy(const HexBoard* board, const Player* aiplayer)
: AbstractStrategyImpl(board, aiplayer),
threshold(0.3),
randomness(0.7),
ptrtoboard(board),
ptrtoplayer(aiplayer),
numberoftrials(2048) {
  numofhexgons = ptrtoboard->getNumofhexgons();
}
Strategy::Strategy(const HexBoard* board, const Player* aiplayer,
    double threshold, double randomness)
: AbstractStrategyImpl(board, aiplayer),
threshold(threshold),
randomness(randomness),
ptrtoboard(board),
ptrtoplayer(aiplayer),
numberoftrials(2048) {
  numofhexgons = ptrtoboard->getNumofhexgons();
}
Strategy::Strategy(const HexBoard* board, const Player* aiplayer,
    size_t numberoftrials)
: AbstractStrategyImpl(board, aiplayer),
threshold(0.1),
randomness(0.1),
ptrtoboard(board),
ptrtoplayer(aiplayer),
numberoftrials(numberoftrials) {
  numofhexgons = ptrtoboard->getNumofhexgons();
}
#endif
Strategy::Strategy(const HexBoard* board, const Player* aiplayer,
                   double threshold, double randomness, size_t numberoftrials)
    : AbstractStrategyImpl(board, aiplayer),
      threshold(threshold),
      randomness(randomness),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(numberoftrials) {
  numofhexgons = ptrtoboard->getNumofhexgons();
}
///Simulation implementation
///@param currentempty is the current number of emtpy hexgons in the current game state
///@return the index or position on the board of generated move
int Strategy::simulation(int currentempty) {
  vector<int> result(ptrtoboard->getSizeOfVertices(), 0);
  int cutoff = static_cast<int>(threshold
      * (float) (ptrtoboard->getSizeOfVertices()));
  vector<int> bwglobal, oppglobal;
  hexgame::shared_ptr<bool> emptyglobal;
  initGameState(emptyglobal, bwglobal, oppglobal);

  //fill up the rest of the empty cells
  vector<pair<int, int> > counter(currentempty);

  vector<int> moves(bwglobal.size() + oppglobal.size());
  merge(bwglobal.begin(), bwglobal.end(), oppglobal.begin(), oppglobal.end(),
        moves.begin());

  hexgame::unordered_set<int> allmoves(moves.begin(), moves.end());
  countNeighbors(emptyglobal, allmoves, counter);

  bool* ptrtoemptyglobal = emptyglobal.get();
  for (int k = 0; k < ptrtoboard->getSizeOfVertices(); ++k)
    if (!*(ptrtoemptyglobal + k))
      result[k] = -1; // marked as occupied

  //start the simulation
  for (int i = 0; i < numberoftrials; i++) {
    //initialize the following containers to the current progress of playing board
    vector<int> babywatsons(bwglobal), opponents(oppglobal);
    hexgame::shared_ptr<bool> emptyindicators = hexgame::shared_ptr<bool>(
        new bool[ptrtoboard->getSizeOfVertices()],
        hexgame::default_delete<bool[]>());
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

//Find the move with the maximal successful simulated outcome
  vector<int> index(result.size());
  PriorityQueue<int, int> queue(result.size());
  for (unsigned i = 0; i < result.size(); i++) {
    index[i] = (i + 1);
    queue.insert(index[i], -1 * result[i]);
  }
  return queue.minPrioirty();
}
///Initialize counter of neighbors or connected hexgons for each hexgon or position on the hex board
///@param queue, priority queue used to store the
///@param moves, the current move made by one of player
///@param counter, to record the number of neighbors or connected hexgons
///@return NONE
void Strategy::countNeighbors(hexgame::shared_ptr<bool> emptyindicators,
                              hexgame::unordered_set<int>& moves,
                              vector<pair<int, int> >& counter) {
  int countofempty = 0;
  for (int i = 0; i < ptrtoboard->getSizeOfVertices(); i++) {
    if (emptyindicators.get()[i]) {
      vector<int> indexofneigh = ptrtoboard->getNeighbors((i + 1));
      int neighbors = 0;
#if __cplusplus > 199711L
      for (auto v : indexofneigh)
        neighbors += moves.count(v);
#else
      vector<int>::iterator iter = indexofneigh.begin();
      for (;iter != indexofneigh.end(); ++iter)
      neighbors += moves.count(*iter);
#endif

      counter[countofempty].first = (i + 1);
      counter[countofempty++].second = neighbors;
    }
  }
}
///Generate next move by taking into account the most connected cells.
///@param emptyindicators: boolean array to indicate the empty positions
///@param queue the priority queue stores the empty positions with the priority as the connections to the position adding some randomness
///@param counter the auxiliary vector which stores the index of position and priority
///@return next move represent as index or position on the hex board
int Strategy::genNextFill(hexgame::shared_ptr<bool>& emptyindicators,
                          PriorityQueue<int, int>&queue,
                          int& proportionofempty) {
  int nexloc = queue.minPrioirty();
  emptyindicators.get()[nexloc - 1] = false;
  --proportionofempty;
  return nexloc;
}
///Assign random number to modify the priority of neighbors counter
///@param queue is a priority queue stores the number of neighbors for each empty cell of current game
///@param counter is a vector stores pair of index value of empty cells and count of neighbors
///@return NONE
void Strategy::assignRandomNeighbors(PriorityQueue<int, int>& queue,
                                     vector<pair<int, int> >& counter,
                                     int currentempty) {

  long long seed =
      hexgame::chrono::system_clock::now().time_since_epoch().count();
//TODO refactor to have consistent codes for C++11 and < C++11
#if __cplusplus > 199711L
  default_random_engine generator(seed);
#else
  boost::mt19937 generator(static_cast<unsigned>(seed));
  srand(static_cast<unsigned>(seed));
#endif

  //ensure assign unique number
  vector<int> numbers(currentempty);
  for (int i = 0; i < currentempty; i++)
    numbers[i] = (i + 1);
#if __cplusplus > 199711L
  shuffle(numbers.begin(), numbers.end(), generator);
#else
  std::random_shuffle(numbers.begin(), numbers.end());
#endif
  hexgame::uniform_real_distribution<float> probability(0.0, 1.0);
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
