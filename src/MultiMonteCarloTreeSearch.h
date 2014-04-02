/*
 * MultiMonteCarloTreeSearch.h
 * This file defines a Mont Carlo Tree Search implementation for AI player
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
/** MultiMonteCarloTreeSearch class defines a Parallelized version of Mont Carlo Tree Search implementation for AI player
 * MultiMonteCarloTreeSearch class is the implementation of Parallelized Mont Carlo Tree Search which include four phases:
 * select, expansion, play-out and back-propagation. <br/>
 * This class depends on AbstractGameTree interface which provide tree data structure and methods for game tree construction.
 * The constructors used to instantiate MultiMonteCarloTreeSearch instance are <br/>
 * MultiMonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer): user defined constructor which takes pointer to
 * a hex board object and pointer to AI player; while parameter used for the number of simulated games (numberoftrials) is
 * set as default value (2048) and number of threads (numberofthreads) is set as default value (8)<br/>
 * MultiMonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer, size_t numberofthreads): user defined constructor
 *  which takes pointer to a hex board object and pointer to AI player; while parameter used for the number of simulated games
 *  (numberoftrials) is set as default value (2048) and number of threads (numberofthreads) is given by user <br/>
 * MultiMonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer, size_t numberofthreads, size_t numberoftrials):
 * user defined constructor which takes pointer to a hex board object and pointer to AI player. Also parameter used for the number of simulated games
 *  (numberoftrials) and number of threads (numberofthreads) <br/>
 * Sample Usage: Please see Strategy (similar way to instantiate)
 */
class MultiMonteCarloTreeSearch : public AbstractStrategyImpl {
 private:
  int numofhexgons; ///< the actual playing board in the game. Need to ensure it not to be modified during the simulation
  MonteCarloTreeSearch mcstimpl; ///< the actual playing board in the game. Need to  ensure it not to be modified during the simulation
  const HexBoard* const ptrtoboard; ///< The actual playing board in the game. Need to ensure it not to be modified during the simulation
  const Player* const ptrtoplayer; ///< the actual player computer plays. Need to ensure it not to be modified during the simulation
  const std::size_t numberofthreads;///< The number of threads used in Parallelized Monte Carlo method. 8 by default
  const std::size_t numberoftrials;///< The number of simulated games which affects the sampling size of Parallelized Monte Carlo method. 2048 by default
  char babywatsoncolor; ///< The color of AI player which is represented as single character. For example, if color of AI player is RED, then character is 'R'. BLUE as 'B'
  char oppoenetcolor; ///< The color of AI player's opponent which is represented as single character. For example, if color of AI player is RED, then character for opponent is 'B'. BLUE as 'R'

  ///delegating simulation method which is passed to each thread for execution
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
  ///User defined constructor which takes pointer to a hex board object and pointer to AI player as parameters
  MultiMonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer);
  ///User defined constructor which takes pointer to a hex board object, pointer to AI player and number of threads (numberofthreads) as parameters
  MultiMonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer,
                            size_t numberofthreads);
  ///User defined constructor which takes pointer to a hex board object, pointer to AI player, number of threads (numberofthreads) and number of simulated games (numberoftrials) as parameters
  MultiMonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer, size_t numberofthreads, size_t numberoftrials);
  ///destructor
  virtual ~MultiMonteCarloTreeSearch() {
  }
  ;
  ///return the meaningful class name as "MultiMonteCarloTreeSearch"
  std::string name() {
    return string("MultiMonteCarloTreeSearch");
  }
  ;
  ///Overwritten simulation method. See AbstractStrategy
  int simulation(int currentempty);

  ///Getter for retrieving number of threads
  ///@param NONE
  ///@return number of threads
  std::size_t getNumberofthreads(){
    return numberofthreads;
  }
  ///Getter for retrieving number of simulated games
  ///@param NONE
  ///@return number of simulated games
  std::size_t getNumberoftrials(){
    return numberoftrials;
  }
};

#endif /* MULTIMONTECARLOTREESEARCH_H_ */
