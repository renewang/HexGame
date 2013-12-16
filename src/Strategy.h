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

#include <vector>
#include <random>
#include <algorithm>
#include <unordered_set>

#include "Player.h"
#include "HexBoard.h"
#include "PriorityQueue.h"
#include "AbstractStrategy.h"
#ifndef NDEBUG
#include "gtest/gtest_prod.h"
#endif

class Strategy : public AbstractStrategy {
 private:
  float threshold = 1.0;  //the threshold to indicate when to stop generating next move randomly and just simply fill up the board
  float randomness = 1.0;
  int numofhexgons;
  //the actual playing board in the game. Need to ensure it not to be modified during the simulation
  const HexBoard* ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* ptrtoplayer;
  const int numberoftrials;
  //generate the random next move in terms of index of row and index of column [1, number of hexgon per side]
  int genNextRandom(bool* emptyindicators, unsigned proportionofempty);
  //fill up the board
  int genNextFill(bool* emptyindicators, PriorityQueue<int, int>&queue);
  int checkWinnerExist(std::vector<int>&, std::vector<int>&);
  bool isWinner(std::vector<int>& test, bool iswestoeast);
  //simulation body
  int simulation();
  //count the number of neighbors for current game progress
  void countNeighbors(bool*, std::unordered_set<int>&,
                      std::vector<std::pair<int, int> >&);
  //assign random number to neighbors counter
  void assignRandomNeighbors(PriorityQueue<int, int>&,
                             std::vector<std::pair<int, int> >&, int);

#ifndef NDEBUG
  //for google test framework
  friend class StrategyTest;
  FRIEND_TEST(StrategyTest,CheckWinnerTest);
  FRIEND_TEST(StrategyTest,CheckWinnerTestTwo);
  FRIEND_TEST(StrategyTest,CheckWinnerElevenTest);
  FRIEND_TEST(StrategyTest,CheckGenMoveForPair);
  FRIEND_TEST(StrategyTest,CheckGenNextFillBasic);
#endif

 public:
  //constructor
  Strategy(const HexBoard* board, const Player* aiplayer);
  Strategy(const HexBoard* board, const Player* aiplayer, float threshold,
           float randomness);
  virtual ~Strategy() {
  }
  ;
  int genMove();
};

#endif /* STRATEGY_H_ */
