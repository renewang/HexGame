/*
 * MinMax.h
 *
 *  Created on: Dec 13, 2013
 *      Author: renewang
 */

#ifndef MINMAX_H_
#define MINMAX_H_

#include "MinMax.h"
#include "Player.h"
#include "HexBoard.h"
#include "AbstractStrategyImpl.h"

#ifndef NDEBUG
#include "gtest/gtest_prod.h"
#endif

class MinMax : public AbstractStrategyImpl {
 private:
  int numofhexgons;
  //the actual playing board in the game. Need to ensure it not to be modified during the simulation
  const HexBoard* ptrtoboard;
  //the actual player computer plays. Need to ensure it not to be modified during the simulation
  const Player* ptrtoplayer;

  //for initializing potentials of empty cells via two-distance evaluation function
  void initPotentials(std::vector<int>&);
  //for calculating potentials of cells with occupied cells via two-distance evaluation function
  void calcPotentials(std::vector<int>&);

#ifndef NDEBUG
  //for google test framework
  friend class MinMaxTest;
  FRIEND_TEST(MinMaxTest,PotentialInitialization);
#endif

 public:
  //constructor
  MinMax(const HexBoard* board, const Player* aiplayer);
  virtual ~MinMax() {
  }
  ;
  int simulation();
};
#endif /* MINMAX_H_ */
