/*
 * Strategy.h
 * This file declares pure/naive Monte Carlo simulation strategy implementation for AI player.
 * hexboard and a pointer to player.
 *
 * Created on:
 *     Author: renewang
 */

#ifndef STRATEGY_H_
#define STRATEGY_H_

#include <vector>
#include <algorithm>

#include "Player.h"
#include "Global.h"
#include "HexBoard.h"
#include "PriorityQueue.h"
#include "AbstractStrategyImpl.h"

#ifndef NDEBUG
#include "gtest/gtest_prod.h"
#endif
/**
 * Strategy Class is used to declare a pure/naive MC strategy object.<br/>
 * Depends on the parameters flexibility, the user-defined constructors can have the following forms:<br/>
 * Strategy(const HexBoard* board, const Player* aiplayer): used to initialize Strategy object with given board and AI player with default values for parameters where numberoftrials = 2048, threshold = 0.3 and randomness = 0.7<br/>
 * Strategy(const HexBoard* board, const Player* aiplayer, size_t numberoftrials): used to initialize Strategy object with given board, AI player and number of simulated games (numberoftrials). The rest of parameters are assigned default values where threshold = 0.3 and randomness = 0.7<br/>
 * Strategy(const HexBoard* board, const Player* aiplayer, double threshold, double randomness): used to initialize Strategy object with given board, AI player and number of simulated games (numberoftrials). The rest of parameters are assigned default values where numberoftrials = 2048<br/>
 * Strategy(const HexBoard* board, const Player* aiplayer, double threshold, double randomness, size_t numberoftrials): used to initialize Strategy object with given board, AI player, number of simulated games (numberoftrials), randomness and threshold<br/>
 * Sample Usage:<br/>
 * int numofhexgon = 11;<br/>
 * HexBoard board(numofhexgon);<br/>
 * Player playera(board, hexgonValKind_RED);<br/>
 * Strategy naivered(&board, &playera);<br/>
 */
class Strategy : public AbstractStrategyImpl {
 private:
  double threshold;  ///< Used to indicate when to stop generating next move randomly and just simply fill up the board
  double randomness; ///< Used to associate with a simple heuristic which will more likely give higher priority to more connected hexgon if randomness is low
  int numofhexgons; ///< The number of hexgons per side. The whole board should have numofhexgons*numofhexgons hexgons
  const HexBoard* ptrtoboard; ///< The actual playing board in the game. Need to ensure it not to be modified during the simulation
  const Player* ptrtoplayer; ///< The actual player computer plays. Need to ensure it not to be modified during the simulation
  const int numberoftrials; ///< The number of simulated games which affects the sampling size of Monte Carlo method. 2048 by default
  ///Overwritten simulation, see AbstractStrategy
  int simulation(int currentempty);   //simulation body
  ///Initialize counter of neighbors for each hexgon given current game state
  void countNeighbors(hexgame::shared_ptr<bool> emptyindicators, hexgame::unordered_set<int>& moves,
                      std::vector<std::pair<int, int> >& counter);
  ///Generate next move by shuffling the remaining empty positions and fill up the board
  int genNextFill(hexgame::shared_ptr<bool>& emptyindicators,
                  PriorityQueue<int, int>&queue, int& proportionofempty);  //fill up the board
  ///Assign random number to modify the priority which is a rough function of connected hexgons (tracked by counter)
  void assignRandomNeighbors(PriorityQueue<int, int>& queue,
                             std::vector<std::pair<int, int> >& counter, int currentempty);

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
  ///User-defined constructor which take pointer to hexboard and pointer to AI player as parameters
  Strategy(const HexBoard* board, const Player* aiplayer);
  ///User-defined constructor which take pointer to hexboard, pointer to AI player and number of simulated game (numberoftrials) as parameters
  Strategy(const HexBoard* board, const Player* aiplayer,
           size_t numberoftrials);
  ///User-defined constructor which take pointer to hexboard, pointer to AI player, randomness and threshold as parameters
  Strategy(const HexBoard* board, const Player* aiplayer, double threshold,
           double randomness);
  ///User-defined constructor which take pointer to hexboard, pointer to AI player, randomness, threshold and number of simulated game (numberoftrials) as parameters
  Strategy(const HexBoard* board, const Player* aiplayer, double threshold,
           double randomness, size_t numberoftrials);
 ///destructor
  virtual ~Strategy() {
  }
  ;
  ///return the meaningful class name as "NaiveMonteCarlo"
  std::string name() {
    return std::string("NaiveMonteCarlo");
  }
  ;
};

#endif /* STRATEGY_H_ */
