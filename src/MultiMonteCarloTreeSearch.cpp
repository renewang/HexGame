/*
 * MultiMonteCarloTreeSearch.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: renewang
 */

#include "global.h"
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
      lockable_base_type(),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(3000){
  init();
}
#endif
MultiMonteCarloTreeSearch::MultiMonteCarloTreeSearch(const HexBoard* board,
                                                     const Player* aiplayer,
                                                     size_t numberoftrials)
    : MonteCarloTreeSearch(board, aiplayer),
      lockable_base_type(),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(numberoftrials) {
  init();
}
int MultiMonteCarloTreeSearch::simulation() {
  boost::shared_ptr<bool> emptyglobal;
  vector<int> bwglobal, oppglobal;
  initGameState(emptyglobal, bwglobal, oppglobal);
  int currentempty = ptrtoboard->getNumofemptyhexgons();

  GameTree gametree(ptrtoplayer->getViewLabel());  //shared and lockable
  treelock locker(*this, gametree);

  thread_group threads;
  for (size_t i = 0; i < numberoftrials; ++i)
    threads.create_thread(
        boost::bind(boost::mem_fn(&MultiMonteCarloTreeSearch::task),
                    boost::ref(*this), boost::cref(bwglobal),
                    boost::cref(oppglobal), boost::cref(emptyglobal),
                    currentempty, boost::ref(locker)));
  threads.join_all();
  int resultmove = getBestMove(gametree);
  //find the move with the maximal successful simulated outcome
  assert(resultmove != -1);

#ifndef NDEBUG
  DEBUGHEADER();
  cerr << gametree.printGameTree(0);
#endif

  return resultmove;
}
void MultiMonteCarloTreeSearch::task(const vector<int>& bwglobal,
                                     const vector<int>& oppglobal,
                                     const boost::shared_ptr<bool>& emptyglobal,
                                     int currentempty, treelock& locker) {

  //initialize the following containers to the current progress of playing board
  vector<int> babywatsons(bwglobal), opponents(oppglobal);
  boost::shared_ptr<bool> emptyindicators = boost::shared_ptr<bool>(
      new bool[ptrtoboard->getSizeOfVertices()], boost::default_delete<bool[]>());

  copy(emptyglobal.get(), emptyglobal.get() + ptrtoboard->getSizeOfVertices(),
       emptyindicators.get());

  int proportionofempty = currentempty;

  //in-tree phase
  int selectnode = selection(currentempty, locker);
  int expandednode = expansion(selectnode, emptyindicators, proportionofempty,
                               babywatsons, opponents, locker);

  //simulation phase
  /*int winner = playout(emptyindicators, proportionofempty, babywatsons,
                       opponents);
  assert(winner != 0);
  //back-propagate
  backpropagation(expandednode, winner, locker);*/
  cout << expandednode << endl;

}
int MultiMonteCarloTreeSearch::selection(int currentempty, treelock& locker) {
  strict_lock<MultiMonteCarloTreeSearch> guard(*this);
  int expectleaf = MonteCarloTreeSearch::selection(currentempty,
                                                   locker.get(guard));
  return expectleaf;
}
int MultiMonteCarloTreeSearch::expansion(int selectnode,
                                         boost::shared_ptr<bool>& emptyindicators,
                                         int& portionofempty,
                                         vector<int>& babywatsons,
                                         vector<int>& opponents,
                                         treelock& locker) {
  strict_lock<MultiMonteCarloTreeSearch> guard(*this);
  return MonteCarloTreeSearch::expansion(selectnode, emptyindicators,
                                         portionofempty, babywatsons, opponents,
                                         locker.get(guard));
}
void MultiMonteCarloTreeSearch::backpropagation(int backupnode, int winner,
                                                treelock& locker) {
  strict_lock<MultiMonteCarloTreeSearch> guard(*this);
  return MonteCarloTreeSearch::backpropagation(backupnode, winner,
                                               locker.get(guard));

}
void MultiMonteCarloTreeSearch::init(){
  numofhexgons = ptrtoboard->getNumofhexgons();
  babywatsoncolor = 'B', oppoenetcolor = 'R';
  if (babywatsoncolor != ptrtoplayer->getViewLabel()) {
    oppoenetcolor = babywatsoncolor;
    babywatsoncolor = ptrtoplayer->getViewLabel();
  }
}
