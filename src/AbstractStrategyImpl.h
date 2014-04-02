/*
 * AbstractStrategyImpl.h
 * This file defines the default implementations declared in AbstractStrategy interface and declares functions and their default implementation
 *
 *   Created on:
 *      Author: renewang
 */

#ifndef ABSTRACTSTRATEGYIMPL_H_
#define ABSTRACTSTRATEGYIMPL_H_

#include "Global.h"
#include "AbstractStrategy.h"

#ifndef NDEBUG
#include "gtest/gtest_prod.h"
#endif
/**
 * AbstractStrategyImpl class defines the default implementations declared in AbstractStrategy interface and also
 * declares additional utility functions and their default implementation <br/>
 * AbstractStrategyImpl(): parameterless default constructor which will initiate a empty board without any hexgons
 * and give default initial values for data member. <br/>
 * AbstractStrategyImpl(const HexBoard* board, const Player* aiplayer): pass already created Board and Player objects <br/>
 *
 * These two constructors should not be invoked by client to instantiate any AbstractStrategy instances.
 */
class AbstractStrategyImpl : public AbstractStrategy {
 private:
  //the actual playing board in the game. Need to ensure it not to be modified during the simulation
  const HexBoard* const ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* const ptrtoplayer;
  int numofhexgons; //number of hexgons per side. the total board should have numofhexgons*numofhexgons hexgons

 protected:
  ///To initialize required containers which store necessary information about game progress
  virtual void initGameState(hexgame::shared_ptr<bool>& emptyglobal, std::vector<int>& bwglobal, std::vector<int>& oppglobal);

  //check if the winner exists for this stage of simulation
  ///See AbstractStrategy, checkWinnerExist
  virtual int checkWinnerExist(std::vector<int>& babywatsons, std::vector<int>& opponents);
  ///See AbstractStrategy, isWinner
  virtual bool isWinner(std::vector<int>& test, bool iswestoeast);
  ///See AbstractStrategy, simulation
  virtual int simulation(int currentempty) = 0;
  ///See AbstractStrategy, genNextRandom
  virtual int genNextRandom(hexgame::shared_ptr<bool>& emptyindicators, int& proportionofempty);
  ///Parameterless default constructor, initialize an empty board. This should be invoked by client to instantiate any AbstractStrategyImpl instances
  AbstractStrategyImpl():ptrtoboard(nullptr), ptrtoplayer(nullptr),numofhexgons(0){};
  ///User-provided constructor which can construct AI strategy based on given HexBoard and Player objects pointers
  AbstractStrategyImpl(const HexBoard* board, const Player* aiplayer)
      : ptrtoboard(board),
        ptrtoplayer(aiplayer) {
    numofhexgons = ptrtoboard->getNumofhexgons();
  }
  ;

#ifndef NDEBUG
  friend class StrategyTest;
  FRIEND_TEST(StrategyTest, DISABLED_CheckWinnerElevenTest);
#endif

 public:
  virtual ~AbstractStrategyImpl() {
  }
  ;
  //called by the client
  ///See AbstractStrategy, genMove
  virtual int genMove();
  ///See AbstractStrategy, name
  virtual std::string name() = 0;

  ///Getter to retrieve information about number of hexgons per side
  ///@param  NONE
  ///@return numofhexgons: number of hexgons per side
  int getNumofhexgons() const {
    return numofhexgons;
  }
  ///Getter to retrieve the HexBoard pointer
  ///@param NONE
  ///@return pointer to the HexBoard object used in this game
  const HexBoard* getPtrtoboard() const {
    return ptrtoboard;
  }
  ///Getter to retrieve the HexBoard pointer
  ///@param NONE
  ///@return pointer to the Player object used in this game and for this AI strategy
  const Player* getPtrtoplayer() const {
    return ptrtoplayer;
  }
};
#endif /* ABSTRACTSTRATEGYIMPL_H_ */
