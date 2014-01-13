/*
 * TwoDistancePotential.h
 *
 *  Created on: Dec 25, 2013
 *      Author: renewang
 */

#ifndef TWODISTANCEPOTENTIAL_H_
#define TWODISTANCEPOTENTIAL_H_

#include "Player.h"
#include "HexBoard.h"
#include "AbstractPotential.h"

class TwoDistancePotential : public AbstractPotential {
 private:
  int numofhexgons;
  //the actual playing board in the game. Need to ensure it not to be modified during the simulation
  const HexBoard* ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* ptrtoplayer;
  //for initializing potentials of empty cells via two-distance evaluation function
  void initPotentials();
  void calcPotentials();
  std::vector<int> potentials;
  vector<int> ends;

 public:
  TwoDistancePotential(const HexBoard* board, const Player* aiplayer);
  TwoDistancePotential()
      : numofhexgons(0),
        ptrtoboard(nullptr),
        ptrtoplayer(nullptr) {
  }
  ;
  virtual ~TwoDistancePotential() {
  }
  ;

  //for calculating potentials of cells with occupied cells via two-distance evaluation function
  void ReCalcPotentials(std::vector<int>&, std::vector<int>&){};

  const std::vector<int>& getPotentials() const {
    return potentials;
  }

  void setPotentials(const std::vector<int>& potentials) {
    this->potentials = potentials;
  }
};

#endif /* TWODISTANCEPOTENTIAL_H_ */
