/*
 * MultiMonteCarloTreeSearch.h
 *
 *  Created on: Feb 11, 2014
 *      Author: renewang
 */

#ifndef MULTIMONTECARLOTREESEARCH_H_
#define MULTIMONTECARLOTREESEARCH_H_

#include "Global.h"
#include "Player.h"
#include "HexBoard.h"
#include "MonteCarloTreeSearch.h"

#include <boost/thread/thread.hpp>

#ifndef NDEBUG
#include "gtest/gtest_prod.h"
#endif

class MultiMonteCarloTreeSearch : public AbstractStrategyImpl {
 private:
  int numofhexgons;
  MonteCarloTreeSearch mcstimpl;
  //the actual playing board in the game. Need to  ensure it not to be modified during the simulation
  const HexBoard* const ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* const ptrtoplayer;
  const std::size_t numberofthreads;
  const std::size_t numberoftrials;
  char babywatsoncolor, oppoenetcolor;

  void init();
  void task(const std::vector<int>& bwglobal, const std::vector<int>& oppglobal,
            const hexgame::shared_ptr<bool>& emptyglobal, int currentempty,
            AbstractGameTree& gametree);

#ifndef NDEBUG
  //for google test framework
  friend class MinMaxTest;
  FRIEND_TEST(MinMaxTest,CompeteHexParallelGame);
#endif

 public:
  //constructor
  MultiMonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer);
  MultiMonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer,
                            size_t numberofthreads);
  MultiMonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer, size_t numberofthreads, size_t numberoftrials);
  virtual ~MultiMonteCarloTreeSearch() {
  }
  ;
  std::string name() {
    return string("MultiMonteCarloTreeSearch");
  }
  ;
  int simulation(int currentempty);

  std::size_t getNumberofthreads(){
    return numberofthreads;
  }

  std::size_t getNumberoftrials(){
    return numberoftrials;
  }
};

#endif /* MULTIMONTECARLOTREESEARCH_H_ */
