/*
 * AbstractStrategyImpl.h
 *
 */

#ifndef ABSTRACTSTRATEGYIMPL_H_
#define ABSTRACTSTRATEGYIMPL_H_

#include "AbstractStrategy.h"

class AbstractStrategyImpl : public AbstractStrategy {
 private:
  //the actual playing board in the game. Need to ensure it not to be modified during the simulation
  const HexBoard* ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* ptrtoplayer;
  int numofhexgons;
 protected:
  //check if the winner exists for this stage of simulation
  virtual int checkWinnerExist(std::vector<int>&, std::vector<int>&);
  virtual bool isWinner(std::vector<int>& test, bool iswestoeast);
  virtual int simulation() = 0;
 public:
  AbstractStrategyImpl():ptrtoboard(nullptr), ptrtoplayer(nullptr),numofhexgons(0){};
  AbstractStrategyImpl(const HexBoard* board, const Player* aiplayer)
      : ptrtoboard(board),
        ptrtoplayer(aiplayer) {
    numofhexgons = ptrtoboard->getNumofhexgons();
  }
  ;
  virtual ~AbstractStrategyImpl() {
  }
  ;
  //called by the client
  virtual int genMove();

  int getNumofhexgons() const {
    return numofhexgons;
  }

  void setNumofhexgons(int numofhexgons) {
    this->numofhexgons = numofhexgons;
  }

  const HexBoard* getPtrtoboard() const {
    return ptrtoboard;
  }

  void setPtrtoboard(const HexBoard*& ptrtoboard) {
    this->ptrtoboard = ptrtoboard;
  }

  const Player* getPtrtoplayer() const {
    return ptrtoplayer;
  }

  void setPtrtoplayer(const Player*& ptrtoplayer) {
    this->ptrtoplayer = ptrtoplayer;
  }
};
#endif /* ABSTRACTSTRATEGYIMPL_H_ */
