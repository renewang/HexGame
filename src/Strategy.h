/*
 * Strategy.h
 * This file defines MC simulation strategy for AI player. The constructor takes a pointer to
 * hexboard and a pointer to player.
 *
 * Sample Usage:
 * HexBoard board(numofhexgon);
 * Game hexboardgame(board);
 * Player* babywatson;
 * Player playersecond(board, hexgonValKind::RED);
 * babywatson = &playersecond;
 * Strategy watsonstrategy(&board, babywatson);
 * int watsonmove = hexboardgame.genMove(watsonstrategy);
 */

#ifndef STRATEGY_H_
#define STRATEGY_H_

#include <set>
#include <vector>
#include <utility>

#include "Player.h"
#include "HexBoard.h"
#include "PriorityQueue.h"
#ifdef _DEBUG_
#include "gtest/gtest_prod.h"
#endif

class Strategy {
 private:
  float threshold = 0.1;  //the threshold to indicate when to stop generating next move randomly and just simply fill up the board
  float randomness = 0.7;
  int numofhexgons;
  //the actual playing board in the game. Need to ensure it not to be modified during the simulation
  const HexBoard* ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* ptrtoplayer;
  const int numberoftrials;
  //generate the random next move in terms of index of row and index of column [1, number of hexgon per side]
  int genNextRandom(bool* emptyindicators, unsigned proportionofempty);
  //fill up the board
  int genNextFill(bool* emptyindicators, PriorityQueue<int, int>&queue,
                  std::vector<std::pair<int,int> >& dummy);
  //check if the winner exists for this stage of simulation
  int checkWinnerExist(std::vector<std::set<int> >&,
                       std::vector<std::set<int> >&);
  bool isWinner(std::vector<std::set<int> >& test, bool iswestoeast);
  //simulation body
  int simulation();
  void assignValue(std::vector<std::set<int> >&, int, bool);
  void initTransformVector(std::vector<std::set<int> >&);
  void countNeighbors(PriorityQueue<int, int>&, bool*,
                      std::vector<std::set<int> >&,
                      std::vector<std::pair<int, int>>&);

#ifdef _DEBUG_
  //for google test framework
  friend class StrategyTest;FRIEND_TEST(StrategyTest,CheckWinnerTest);FRIEND_TEST(StrategyTest,CheckWinnerTestTwo);FRIEND_TEST(StrategyTest,CheckWinnerElevenTest);FRIEND_TEST(StrategyTest,CheckGenMoveForPair);FRIEND_TEST(StrategyTest,CheckGenNextFill);FRIEND_TEST(StrategyTest,CheckGenNextFillBasic);
#endif

 public:
  //constructor
  Strategy(const HexBoard* board, const Player* aiplayer);
  virtual ~Strategy() {
  }
  ;
  //called by the client
  int genMove();
};

#endif /* STRATEGY_H_ */
