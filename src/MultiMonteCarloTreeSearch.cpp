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
    : MultiMonteCarloTreeSearch(board, aiplayer, 3000) {
}
#else
MultiMonteCarloTreeSearch::MultiMonteCarloTreeSearch(const HexBoard* board,
    const Player* aiplayer)
: MonteCarloTreeSearch(board, aiplayer),
ptrtoboard(board),
ptrtoplayer(aiplayer),
numberoftrials(3000) {
  init();
}
#endif
MultiMonteCarloTreeSearch::MultiMonteCarloTreeSearch(const HexBoard* board,
                                                     const Player* aiplayer,
                                                     size_t numberoftrials)
    : MonteCarloTreeSearch(board, aiplayer),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(numberoftrials) {
  init();
}
int MultiMonteCarloTreeSearch::simulation(int currentempty) {
  hexgame::shared_ptr<bool> emptyglobal;
  vector<int> bwglobal, oppglobal;
  initGameState(emptyglobal, bwglobal, oppglobal);
  LockableGameTree gametree(ptrtoplayer->getViewLabel());  //shared and lockable
  thread_group threads;
  for (size_t i = 0; i < numberoftrials; ++i)
    threads.create_thread(
        boost::bind(boost::mem_fn(&MultiMonteCarloTreeSearch::task),
                    boost::ref(*this), boost::cref(bwglobal),
                    boost::cref(oppglobal), boost::cref(emptyglobal),
                    currentempty, boost::ref(gametree)));
  threads.join_all();
  int resultmove = getBestMove(gametree);
  //find the move with the maximal successful simulated outcome
  assert(resultmove != -1);

#ifndef NDEBUG
  {
   DEBUGHEADER();
   cerr << gametree.name() << " current tree size = " << gametree.getNumofTotalNodes() << endl;
  }
#endif

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
  int selectnode = selection(currentempty, gametree);
  int expandednode = expansion(selectnode, emptyindicators, proportionofempty,
                               babywatsons, opponents, gametree);

  //simulation phase
  int winner = playout(emptyindicators, proportionofempty, babywatsons,
                       opponents);
  assert(winner != 0);
  //back-propagate
  backpropagation(expandednode, winner, gametree);
}
void MultiMonteCarloTreeSearch::init() {
  numofhexgons = ptrtoboard->getNumofhexgons();
  babywatsoncolor = 'B', oppoenetcolor = 'R';
  if (babywatsoncolor != ptrtoplayer->getViewLabel()) {
    oppoenetcolor = babywatsoncolor;
    babywatsoncolor = ptrtoplayer->getViewLabel();
  }
}
