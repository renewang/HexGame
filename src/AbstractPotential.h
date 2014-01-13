/*
 * AbstractPotential.h
 *
 *  Created on: Dec 25, 2013
 *      Author: renewang
 */

#ifndef ABSTRACTPOTENTIAL_H_
#define ABSTRACTPOTENTIAL_H_

class AbstractPotential {
 protected:
  //for initializing potentials of empty cells via two-distance evaluation function
  virtual void initPotentials() = 0;
 public:
  AbstractPotential() {
  }
  ;
  virtual ~AbstractPotential() {
  }
  ;
  //for calculating potentials of cells with occupied cells via two-distance evaluation function
  virtual void ReCalcPotentials(std::vector<int>&, std::vector<int>&) = 0;
};

#endif /* ABSTRACTPOTENTIAL_H_ */
