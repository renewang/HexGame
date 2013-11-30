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
#ifdef _DEBUG_
#include "gtest/gtest_prod.h"
#endif

class Strategy {
 private:
  //the actual playing board in the game. Need to ensure it not to be modified during the simulation
  const HexBoard* ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* ptrtoplayer;
  const int numberoftrials;
  //generate the random next move in terms of index of row and index of column [1, number of hexgon per side]
  int genNextRandom(bool* emptyindicators, unsigned proportionofempty);
  //fill up the board
  int genNextFill(bool* emptyindicators, std::vector<int>&, bool);
  //check if the winner exists for this stage of simulation
  int checkWinnerExist(std::vector<int>&, std::vector<int>&);
  bool isWinner(std::vector<int>& test, bool iswestoeast);
  //simulation body
  int simulation();
  float threshold = 0.0;  //the threshold to indicate when to stop generating next move randomly and just simply fill up the board

#ifdef _DEBUG_
  //for google test framework
  friend class StrategyTest;
  FRIEND_TEST(StrategyTest,CheckWinnerTest);
  FRIEND_TEST(StrategyTest,CheckWinnerTestTwo);
  FRIEND_TEST(StrategyTest,CheckWinnerElevenTest);
  FRIEND_TEST(StrategyTest,CheckGenMoveForPair);
  FRIEND_TEST(StrategyTest,CheckGenNextFill);
  FRIEND_TEST(StrategyTest,CheckGenNextFillBasic);
#endif

 public:
  Strategy(const HexBoard* board, const Player* aiplayer);
  virtual ~Strategy() {
  }
  ;
  //called by the client
  int genMove();
};

#endif /* STRATEGY_H_ */
