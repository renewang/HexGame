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
#include "AbstractGameTree.h"
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

 protected:
  int getBestMove(AbstractGameTree& gametree);
  virtual int simulation(int currentempty);
  //Monte Carlo tree search steps
  //in-tree phase
  int selection(int currentempty, AbstractGameTree& gametree);
  int expansion(int selectnode, hexgame::shared_ptr<bool>& emptyindicators,
                int& portionofempty, std::vector<int>& babywatsons,
                std::vector<int>& opponents, AbstractGameTree& gametree);
  //play-out phase
  int playout(hexgame::shared_ptr<bool>& emptyindicators, int& portionofempty,
              std::vector<int>& babywatsons, std::vector<int>& opponents);
  int backpropagation(int backupnode, int winner, AbstractGameTree& gametree);
  void init();

#ifndef NDEBUG
  //for google test framework
  friend class MinMaxTest;
  FRIEND_TEST(MinMaxTest,MCSTExpansion);
  FRIEND_TEST(MinMaxTest,SimulationCombine);
#endif

 public:
  //constructor
  MonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer);
  MonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer,
                       size_t numberoftrials);
  virtual ~MonteCarloTreeSearch() {
  }
  ;
  std::string name() {
    return std::string("MonteCarloTreeSearch");
  }
  ;

  std::size_t getNumberoftrials() {
    return numberoftrials;
  }
};
#endif /* MONTECARLOTREESEARCH_H_ */
