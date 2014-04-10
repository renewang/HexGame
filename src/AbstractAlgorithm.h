/*
 * AbstractAlgorithm.h
 * This file defines an interface for graph algorithms which can be passed into MinSpanTreeAlgo.calculate()
 * calculate() needs to be implemented the actual algorithm
 *
 *  Created on:
 *    Author: renewang
 */

#ifndef ABSTRACTALGORITHM_H_
#define ABSTRACTALGORITHM_H_
#include <iosfwd>
#include <string>
/**
 * AbstractAlgorithm class is used to provide interface for graph algorithm (especially minimum spanning tree).<br/>
 * Example:<br/>
 * class UnionFind : public AbstractAlgorithm {<br/>
 *  private:<br/>
 *   MinSpanTreeAlgo& algo;<br/>
 *  public:<br/>
 *   UnionFind(MinSpanTreeAlgo& algo)<br/>
 *       : algo(algo) {<br/>
 *   };<br/>
 */
class AbstractAlgorithm {
 public:
  ///default constructor, parameterless, shouldn't be called to instantiate any AbstractAlgorithm instance
  AbstractAlgorithm(){};
  ///destructor
  virtual ~AbstractAlgorithm(){};
  ///provide the interface for graph algorithm implementation
  ///@param NONE
  ///@return NONE
  virtual void calculate() = 0;
  ///function to identify the class name
  ///@param NONE
  ///@return NONE
  virtual std::string name() = 0;
};

#endif /* ABSTRACTALGORITHM_H_ */
