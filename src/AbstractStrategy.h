#ifndef ABSTRACTSTRATEGY_H_
#define ABSTRACTSTRATEGY_H_

#include <vector>
#include <string>

#include "Player.h"
#include "HexBoard.h"

class AbstractStrategy {

 protected:
  //check if the winner exists for this stage of simulation
  virtual int checkWinnerExist(std::vector<int>&, std::vector<int>&) = 0;
  virtual bool isWinner(std::vector<int>& test, bool iswestoeast) = 0;
  virtual int simulation(int currentempty) = 0;
  virtual int genNextRandom(hexgame::shared_ptr<bool>& emptyindicators, int& proportionofempty) = 0;

 public:
  AbstractStrategy() {
  }
  ;
  virtual ~AbstractStrategy() {
  }
  ;
  //called by the client
  virtual int genMove() = 0;
  virtual std::string name() = 0;
};

#endif /* ABSTRACTSTRATEGY_H_ */
