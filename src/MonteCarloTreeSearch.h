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

class MonteCarloTreeSearch : public AbstractStrategyImpl {
 private:
  int numofhexgons;
  //the actual playing board in the game. Need to  ensure it not to be modified during the simulation
  const HexBoard* const ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* const ptrtoplayer;
  std::vector<GameTree> treeholder;
  const int numberoftrials;
  const std::size_t numofchildren;
  char babywatsoncolor, oppoenetcolor;

  //for the shared game tree or unshared game trees (one tree per simulation) to record the simulated moves through the game
  void registerGameTree(std::size_t);  //initialize the game tree and register them
  int getBestMove();

  //Monte Carlo tree search steps
  //in-tree phase
  void selection(std::vector<std::string>& leavesforexpanded,
                 int currentempty);
  void expansion(std::vector<std::string>& leavesforexpanded,
                 std::size_t numofchildren,
                 std::shared_ptr<bool>& emptyindicators, int& portionofempty);
  //play-out phase
  void playout(std::shared_ptr<bool>& emptyindicators, int portionofempty);  //simulation on particular node
  //update
  void backpropagate();

 public:
  //constructor
  MonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer);
  virtual ~MonteCarloTreeSearch() {
  }
  ;
  int simulation();
};
#endif /* MONTECARLOTREESEARCH_H_ */
