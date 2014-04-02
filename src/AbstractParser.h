/*
 * AbstractGameTree.h
 * This file defines an interface for Graph file format parser.
 *   Created on:
 *      Author: renewang
 */

#ifndef ABSTRACTPARSER_H_
#define ABSTRACTPARSER_H_

#include <vector>
#include <fstream>
#include <iostream>
/**
 * AbstractParser declares an abstract interface for parser of graph file<br/>
 * AbstractParser(): is the parameter default constructor. Shouldn't be invoked at any time <br/>
 */
class AbstractParser {

 protected:
  ///Declare a function to open file
  virtual void open() = 0;
  ///Declare a function to parse file
  virtual void parse() = 0;
  ///Declare a function to close file
  virtual void close() = 0;

 public:
  ///Default constructor which shouldn't be invoked at client side to instantiate instances
  AbstractParser() {};
  ///Destructor
  virtual ~AbstractParser() {};
  ///Declares an public method to get parsed data
  virtual const std::vector<std::vector<std::string> >& getData() const = 0;
};

#endif /* ABSTRACTPARSER_H_ */
