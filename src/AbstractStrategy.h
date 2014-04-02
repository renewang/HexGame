/*
 * AbstractStrategy.h
 * This file defines an abstract interface for AI Strategy class.
 *
 *  Created on:
 *     Author: renewang
 */
#ifndef ABSTRACTSTRATEGY_H_
#define ABSTRACTSTRATEGY_H_

#include <vector>
#include <string>

#include "Player.h"
#include "Global.h"
#include "HexBoard.h"

#if __cplusplus > 199711L
/**
 * Enum class AIStrategyKind is used to define the possible choices of AI MC strategies where <br/>
 * NAIVE or 1, will choose AI player which employs pure/naive MC strategy <br/>
 * MCTS or 2, will choose AI player which employs Monte Carlo Tree Search (MCTS) strategy <br/>
 * PMCTS or 3, will choose AI player which employs Parallel Monte Carlo Tree Search (PMCTS) strategy <br/>
 */
enum class AIStrategyKind {
  NAIVE = 1,
  MCTS,
  PMCTS
};
#define AIStrategyKind_NAIVE AIStrategyKind::NAIVE
#define AIStrategyKind_MCTS AIStrategyKind::MCTS
#define AIStrategyKind_PMCTS AIStrategyKind::PMCTS
#else
enum AIStrategyKind {
  NAIVE,
  MCTS,
  PMCTS
};
#define AIStrategyKind_NAIVE NAIVE
#define AIStrategyKind_MCTS MCTS
#define AIStrategyKind_PMCTS PMCTS
#endif
/**
 * AbstractStrategy class is used to define an abstract interface for strategy AI player <br/>
 * In this class, it declares a public genMove function which should be implemented by extended classes and invoked by client<br/>
 * It also declares 4 protected function checkWinnerExist and isWinner are used to determine winner in the game and their default<br/>
 * implementations are defined in the AbstractStrategyImpl class<br/>
 * <br/>
 * AbstractStrategy(): parameterless default constructor which should not be used to instantiate any instances of AbstractStrategy<br/>
 */
class AbstractStrategy {

 protected:
  ///Check if the winner exists for this stage of simulation
  virtual int checkWinnerExist(std::vector<int>&, std::vector<int>&) = 0;
  ///Check if the self-play game has a winner or not provided the simulated moves
  virtual bool isWinner(std::vector<int>& test, bool iswestoeast) = 0;
  ///Provide the simulation or self-play phase code
  virtual int simulation(int currentempty) = 0;
  ///Generate the random next move during simulation or self play phase. Move is represented in terms of index of row and index of column [1, number of hexgon per side]
  virtual int genNextRandom(hexgame::shared_ptr<bool>& emptyindicators,
                            int& proportionofempty) = 0;
  ///Parameterless default constructor, should not be invoked for instantiation
  AbstractStrategy() {
  }
  ;

 public:
  //called by the client
  ///Generate move through simulation or self-play phase
  virtual int genMove() = 0;
  ///To return polymorphic class name
  virtual std::string name() = 0;
  virtual ~AbstractStrategy() {
  }
  ;
  //destructor
};
//utility and non-class functions
void selectStrategy(
    AIStrategyKind strategykind,
    hexgame::unique_ptr<AbstractStrategy,
        hexgame::default_delete<AbstractStrategy> >& watsonstrategy,
    Player& player, HexBoard& board);  //used by client to choose different AI strategies
void simulations(
    hexgame::unique_ptr<AbstractStrategy,
        hexgame::default_delete<AbstractStrategy> >& strategyred,
    hexgame::unique_ptr<AbstractStrategy,
        hexgame::default_delete<AbstractStrategy> >& strategyblue,
    AIStrategyKind redstrategykind, AIStrategyKind bluestrategykind,
    int numofhexgon = 11);  //used by client to simulate a game in which two AI players might employ the same or different AI strategies
#endif /* ABSTRACTSTRATEGY_H_ */
