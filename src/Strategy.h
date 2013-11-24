/*
 * Strategy.h
 *
 *  Created on: Nov 20, 2013
 *      Author: renewang
 */

#ifndef STRATEGY_H_
#define STRATEGY_H_

#include <vector>

#include "Player.h"
#include "HexBoard.h"

#include "gtest/gtest_prod.h"

class Strategy {
 private:
  //the actual playing board in the game. Need to ensure it not to be modified during the simulation
  const HexBoard* ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* ptrtoplayer;
  const int numberoftrials;
  //generate the random next move in terms of index of row and index of column [1, number of hexgon per side]
  int genNextRandom(bool* emptyindicators);
  //check if the winner exists for this stage of simulation
  int checkWinnerExist(std::vector<int>&, std::vector<int>&);
  bool isWinner(std::vector<int>& test);
  //simulation body
  int simulation();

  //for google test framework
  friend class StrategyTest;
  FRIEND_TEST(StrategyTest,CheckWinnerTest);

 public:
  Strategy(const HexBoard* board, const Player* aiplayer);
  virtual ~Strategy(){};
  //called by the client
  int genMove();
};

#endif /* STRATEGY_H_ */
