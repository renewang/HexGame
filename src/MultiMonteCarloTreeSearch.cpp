/*
 * MultiMonteCarloTreeSearch.cpp
 * This file declares a Mont Carlo Tree Search implementation for AI player
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
  babywatsoncolor = mcstimpl.babywatsoncolor;
  oppoenetcolor = mcstimpl.oppoenetcolor;
}
MultiMonteCarloTreeSearch::MultiMonteCarloTreeSearch(const HexBoard* board,
    const Player* aiplayer,
    size_t numberofthreads):AbstractStrategyImpl(board, aiplayer),
mcstimpl(MonteCarloTreeSearch(board, aiplayer)),
ptrtoboard(board),
ptrtoplayer(aiplayer),
numberofthreads(numberofthreads),
numberoftrials(2048) {
  babywatsoncolor = mcstimpl.babywatsoncolor;
  oppoenetcolor = mcstimpl.oppoenetcolor;
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
  babywatsoncolor = mcstimpl.babywatsoncolor;
  oppoenetcolor = mcstimpl.oppoenetcolor;
}
///Overwritten simulation method. See AbstractStrategy.
int MultiMonteCarloTreeSearch::simulation(int currentempty) {
  hexgame::shared_ptr<bool> emptyglobal;
  vector<int> bwglobal, oppglobal;
  initGameState(emptyglobal, bwglobal, oppglobal);
  LockableGameTree gametree(ptrtoplayer->getViewLabel());  //shared and lockable

  for (size_t i = 0; i < (numberoftrials / numberofthreads); ++i) {
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
///the actual task passed to each thread for execution which contains selection, expansion and backpropagation phases
///@param bwglobal is the moves made by AI player in the current actual game state
///@param oppglobal is the moves made by human player in the current actual game state
///@param emptyglobal stores indicator of a position on the hex board is empty or not which will be modified when a simulated game progresses
///@param currentempty is the current empty hexgons or positions left in the actual game state which will be the number of children nodes of root of game tree
///@param gametree is a game tree object which stores the simulation progress and result
///@return NONE
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
  pair<int, int> selectresult = mcstimpl.selection(currentempty, gametree);
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
