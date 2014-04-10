/*
 * MonteCarloTreeSearch.h
 * This file defines a Mont Carlo Tree Search implementation for AI player
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
/** MonteCarloTreeSearch class defines a Monte Carlo Tree Search implementation for AI player
 * MonteCarloTreeSearch class is the implementation of Monte Carlo Tree Search which include four phases: select, expansion,
 * play-out and back-propagation. <br/>
 * This class depends on AbstractGameTree interface which provide tree data structure and methods for game tree construction.
 * The constructors used to instantiate MonteCarloTreeSearch instance are <br/>
 * MonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer): user defined constructor which takes pointer to a
 * hex board object and pointer to AI player; while parameter used for the number of simulated games (numberoftrials) is set
 * as default value (2048) <br/>
 * MonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer, size_t numberoftrials): user defined constructor which
 * takes pointer to a hex board object and pointer to AI player; while parameter used for the number of simulated games
 * (numberoftrials) is provided by user <br/>
 * Sample Usage: Please see Strategy (similar way to instantiate)
 */
class MonteCarloTreeSearch : public AbstractStrategyImpl {
 private:
  int numofhexgons; ///< the actual playing board in the game. Need to ensure it not to be modified during the simulation
  const HexBoard* const ptrtoboard; ///< The actual playing board in the game. Need to ensure it not to be modified during the simulation
  const Player* const ptrtoplayer; ///< the actual player computer plays. Need to ensure it not to be modified during the simulation
  const std::size_t numberoftrials; ///< The number of simulated games which affects the sampling size of Monte Carlo method. 2048 by default
  char babywatsoncolor; ///< The color of AI player which is represented as single character. For example, if color of AI player is RED, then character is 'R'. BLUE as 'B'
  char oppoenetcolor; ///< The color of AI player's opponent which is represented as single character. For example, if color of AI player is RED, then character for opponent is 'B'. BLUE as 'R'

 private:
  ///get the best move from game tree
  int getBestMove(AbstractGameTree& gametree);
  ///Overwritten simulation method. See AbstractStrategy.
  int simulation(int currentempty);
  //Monte Carlo tree search steps
  //in-tree phase
  ///selection phase implementation
  std::pair<int,int> selection(int currentempty, AbstractGameTree& gametree);
  ///expansion phase implementation
  int expansion(std::pair<int,int> selectresult, hexgame::shared_ptr<bool>& emptyindicators,
                int& portionofempty, std::vector<int>& babywatsons,
                std::vector<int>& opponents, AbstractGameTree& gametree);
  ///play-out phase implementation
  int playout(hexgame::shared_ptr<bool>& emptyindicators, int& portionofempty,
              std::vector<int>& babywatsons, std::vector<int>& opponents);
  ///back-propagation phase implementation
  void backpropagation(int expandednode, int winner, AbstractGameTree& gametree);
  ///initialize babywatsoncolor and oppoenetcolor
  void init();

  friend class MultiMonteCarloTreeSearch;

#ifndef NDEBUG
  //for google test framework
  friend class MinMaxTest;
  FRIEND_TEST(MinMaxTest,MCSTExpansion);
  FRIEND_TEST(MinMaxTest,SimulationCombine);
#endif

 public:
  //constructor
  ///User defined constructor which takes pointer to a hex board object and pointer to AI player as parameters
  MonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer);
  ///User defined constructor which takes pointer to a hex board object, pointer to AI player, and number of simulated games (numberoftrials) as parameters
  MonteCarloTreeSearch(const HexBoard* board, const Player* aiplayer,
                       size_t numberoftrials);
  ///destructor
  virtual ~MonteCarloTreeSearch() {
  }
  ;
  ///return the meaningful class name as "MonteCarloTreeSearch"
  std::string name() {
    return std::string("MonteCarloTreeSearch");
  }
  ;
  ///Getter for retrieving number of simulated games
  ///@param NONE
  ///@return number of simulated games
  std::size_t getNumberoftrials() {
    return numberoftrials;
  }
};
#endif /* MONTECARLOTREESEARCH_H_ */
