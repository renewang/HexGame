/*
 * AbstractAlgorithm.h
 *
 */

#ifndef ABSTRACTALGORITHM_H_
#define ABSTRACTALGORITHM_H_

class AbstractAlgorithm {
 public:
  AbstractAlgorithm(){};
  virtual ~AbstractAlgorithm(){};
  virtual void calculate() = 0;
};

#endif /* ABSTRACTALGORITHM_H_ */
