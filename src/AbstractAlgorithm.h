/*
 * AbstractAlgorithm.h
 * This file defines an interface for graph algorithms which can be passed into MinSpanTreeAlgo.calculate()
 * calculate() needs to be implemented the actual algorithm
 */

#ifndef ABSTRACTALGORITHM_H_
#define ABSTRACTALGORITHM_H_
#include <iosfwd>
#include <string>
/*
 * AbstractAlgorithm class is used to provide interface for graph algorithm (especially minimum spanning tree).
 * Example:
 * class UnionFind : public AbstractAlgorithm {
 *  private:
 *   MinSpanTreeAlgo& algo;
 *  public:
 *   UnionFind(MinSpanTreeAlgo& algo)
 *       : algo(algo) {
 *   };
 */
class AbstractAlgorithm {
 public:
  AbstractAlgorithm(){};//default constructor, parameterless
  virtual ~AbstractAlgorithm(){}; //destructor
  virtual void calculate() = 0; //provide the interface to define graph algorithm implementation
  virtual std::string name() = 0; //function to identify the class name
};

#endif /* ABSTRACTALGORITHM_H_ */
