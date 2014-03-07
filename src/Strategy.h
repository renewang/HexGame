/*
 * Strategy.h
 * This file defines pure Monte Carlo simulation strategy for AI player. The constructor takes a pointer to
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
#include <algorithm>

#include "Player.h"
#include "HexBoard.h"
#include "PriorityQueue.h"
#include "AbstractStrategyImpl.h"

#ifndef NDEBUG
#include "gtest/gtest_prod.h"
#endif

class Strategy : public AbstractStrategyImpl {
 private:
  double threshold;  //the threshold to indicate when to stop generating next move randomly and just simply fill up the board
  double randomness;
  int numofhexgons;
  //the actual playing board in the game. Need to ensure it not to be modified during the simulation
  const HexBoard* ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* ptrtoplayer;
  const int numberoftrials;
  //fill up the board
  //simulation body
  int simulation(int currentempty);
  //count the number of neighbors for current game progress
  void countNeighbors(hexgame::shared_ptr<bool>, hexgame::unordered_set<int>&,
                      std::vector<std::pair<int, int> >&);
  int genNextFill(hexgame::shared_ptr<bool>& emptyindicators,
                  PriorityQueue<int, int>&queue, int& proportionofempty);
  //assign random number to neighbors counter
  void assignRandomNeighbors(PriorityQueue<int, int>&,
                             std::vector<std::pair<int, int> >&, int);

#ifndef NDEBUG
  //for google test framework
  friend class StrategyTest;FRIEND_TEST(StrategyTest,CheckWinnerTest);FRIEND_TEST(StrategyTest,CheckWinnerTestTwo);FRIEND_TEST(StrategyTest,CheckWinnerElevenTest);FRIEND_TEST(StrategyTest,CheckGenMoveForPair);FRIEND_TEST(StrategyTest,CheckGenNextFillBasic);
#endif

 public:
  //constructor
  Strategy(const HexBoard* board, const Player* aiplayer);
  Strategy(const HexBoard* board, const Player* aiplayer,
           size_t numberoftrials);
  Strategy(const HexBoard* board, const Player* aiplayer, double threshold,
           double randomness);
  Strategy(const HexBoard* board, const Player* aiplayer, double threshold,
           double randomness, size_t numberoftrials);

  virtual ~Strategy() {
  }
  ;
  std::string name() {
    return std::string("NaiveMonteCarlo");
  }
  ;
};

#endif /* STRATEGY_H_ */
