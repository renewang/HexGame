/*
 * MinMax.h
 *
 *  Created on: Dec 13, 2013
 *      Author: renewang
 */

#ifndef MONTECARLOTREESEARCH_H_
#define MONTECARLOTREESEARCH_H_

#include "Player.h"
#include "HexBoard.h"
#include "GameTree.h"
#include "AbstractStrategyImpl.h"
#include "MonteCarloTreeSearch.h"

#ifndef NDEBUG
#include "gtest/gtest_prod.h"
#endif

class MonteCarloTreeSearch : public AbstractStrategyImpl {
 private:
  int numofhexgons;
  //the actual playing board in the game. Need to  ensure it not to be modified during the simulation
  const HexBoard* const ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* const ptrtoplayer;
  const std::size_t numberoftrials;
  char babywatsoncolor, oppoenetcolor;
  GameTree& gametree;

  int getBestMove();

  //Monte Carlo tree search steps
  //in-tree phase
  int selection(int currentempty);
  int expansion(int expandedleaf, std::shared_ptr<bool>& emptyindicators,
                int& portionofempty, vector<int>& babywatsons,
                vector<int>& opponents);
  //play-out phase
  int playout(std::shared_ptr<bool>& emptyindicators, int& portionofempty,
              vector<int>& babywatsons, vector<int>& opponents);
  void backpropagation(int backupnode, int winner);

#ifndef NDEBUG
  //for google test framework
  friend class MinMaxTest;FRIEND_TEST(MinMaxTest,MCSTBasic);FRIEND_TEST(MinMaxTest,SimulationCombine);
#endif

 public:
  //constructor
  MonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer,
                       GameTree& gametree);
  MonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer,
                       GameTree& gametree, size_t numberoftrials);
  virtual ~MonteCarloTreeSearch() {
  }
  ;
  int simulation();

  const GameTree& getGametree() const {
    return gametree;
  }
//TODO, don't know why compiler issue the warning that I need to drop const on return type
  //const std::size_t getNumberoftrials() const
  std::size_t getNumberoftrials() {
    return numberoftrials;
  }
};
#endif /* MONTECARLOTREESEARCH_H_ */
