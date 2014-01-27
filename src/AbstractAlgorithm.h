/*
 * AbstractAlgorithm.h
 *
 */

#ifndef ABSTRACTALGORITHM_H_
#define ABSTRACTALGORITHM_H_
#include <iosfwd>
#include <string>

class AbstractAlgorithm {
 public:
  AbstractAlgorithm(){};
  virtual ~AbstractAlgorithm(){};
  virtual void calculate() = 0;
  virtual std::string name() = 0;
};

#endif /* ABSTRACTALGORITHM_H_ */
