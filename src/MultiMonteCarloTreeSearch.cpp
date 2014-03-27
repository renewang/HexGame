/*
 * MultiMonteCarloTreeSearch.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: renewang
 */

#include "Global.h"
#include "LockableGameTree.h"
#include "MultiMonteCarloTreeSearch.h"

#include <algorithm>
#include <boost/thread/detail/memory.hpp>

using namespace std;
using namespace boost;

#if __cplusplus > 199711L
MultiMonteCarloTreeSearch::MultiMonteCarloTreeSearch(const HexBoard* board,
                                                     const Player* aiplayer)
    : MultiMonteCarloTreeSearch(board, aiplayer, 8, 2048) {
}
MultiMonteCarloTreeSearch::MultiMonteCarloTreeSearch(const HexBoard* board,
                                                     const Player* aiplayer,
                                                     size_t numberofthreads)
    : MultiMonteCarloTreeSearch(board, aiplayer, numberofthreads, 2048) {
}
#else
MultiMonteCarloTreeSearch::MultiMonteCarloTreeSearch(const HexBoard* board,
    const Player* aiplayer)
: AbstractStrategyImpl(board, aiplayer),
mcstimpl(MonteCarloTreeSearch(board, aiplayer)),
ptrtoboard(board),
ptrtoplayer(aiplayer),
numberofthreads(4),
numberoftrials(2048) {
  init();
}
MultiMonteCarloTreeSearch::MultiMonteCarloTreeSearch(const HexBoard* board,
    const Player* aiplayer,
    size_t numberofthreads):AbstractStrategyImpl(board, aiplayer),
mcstimpl(MonteCarloTreeSearch(board, aiplayer)),
ptrtoboard(board),
ptrtoplayer(aiplayer),
numberofthreads(numberofthreads),
numberoftrials(2048) {
  init();
  }
#endif
MultiMonteCarloTreeSearch::MultiMonteCarloTreeSearch(const HexBoard* board,
                                                     const Player* aiplayer,
                                                     size_t numberofthreads,
                                                     size_t numberoftrials)
    : AbstractStrategyImpl(board, aiplayer),
      mcstimpl(MonteCarloTreeSearch(board, aiplayer)),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberofthreads(numberofthreads),
      numberoftrials(numberoftrials) {
  init();
}
int MultiMonteCarloTreeSearch::simulation(int currentempty) {
  hexgame::shared_ptr<bool> emptyglobal;
  vector<int> bwglobal, oppglobal;
  initGameState(emptyglobal, bwglobal, oppglobal);
  LockableGameTree gametree(ptrtoplayer->getViewLabel());  //shared and lockable

  for (size_t i = 0; i < (numberoftrials/numberofthreads); ++i){
    thread_group threads;
    for (size_t j = 0; j < numberofthreads; ++j)
      threads.create_thread(
          boost::bind(boost::mem_fn(&MultiMonteCarloTreeSearch::task),
                      boost::ref(*this), boost::cref(bwglobal),
                      boost::cref(oppglobal), boost::cref(emptyglobal),
                      currentempty, boost::ref(gametree)));
      assert(threads.size() == numberofthreads);
      threads.join_all();
  }
  int resultmove = mcstimpl.getBestMove(gametree);
  //find the move with the maximal successful simulated outcome
  assert(resultmove != -1);
  return resultmove;
}
void MultiMonteCarloTreeSearch::task(
    const std::vector<int>& bwglobal, const std::vector<int>& oppglobal,
    const hexgame::shared_ptr<bool>& emptyglobal, int currentempty,
    AbstractGameTree& gametree) {
  //initialize the following containers to the current progress of playing board
  vector<int> babywatsons(bwglobal), opponents(oppglobal);
  hexgame::shared_ptr<bool> emptyindicators = hexgame::shared_ptr<bool>(
      new bool[ptrtoboard->getSizeOfVertices()],
      hexgame::default_delete<bool[]>());

  copy(emptyglobal.get(), emptyglobal.get() + ptrtoboard->getSizeOfVertices(),
       emptyindicators.get());

  int proportionofempty = currentempty;

  //in-tree phase
  pair<int,int> selectresult = mcstimpl.selection(currentempty, gametree);
  int expandednode = mcstimpl.expansion(selectresult, emptyindicators,
                                        proportionofempty, babywatsons,
                                        opponents, gametree);

  //simulation phase
  int winner = mcstimpl.playout(emptyindicators, proportionofempty, babywatsons,
                                opponents);
  assert(winner != 0);
  //back-propagate
  mcstimpl.backpropagation(expandednode, winner, gametree);
}
void MultiMonteCarloTreeSearch::init() {
  numofhexgons = ptrtoboard->getNumofhexgons();
  babywatsoncolor = 'B', oppoenetcolor = 'R';
  if (babywatsoncolor != ptrtoplayer->getViewLabel()) {
    oppoenetcolor = babywatsoncolor;
    babywatsoncolor = ptrtoplayer->getViewLabel();
  }
}
