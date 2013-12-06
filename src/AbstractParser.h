//============================================================================
// Name        : AbstractParser.h
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Base class for file parser
//============================================================================

#ifndef ABSTRACTPARSER_H_
#define ABSTRACTPARSER_H_

#include <vector>
#include <fstream>
#include <iostream>

class AbstractParser {

 protected:
  virtual void open() = 0;
  virtual void parse() = 0;
  virtual void close() = 0;

 public:
  AbstractParser() { std::cout << std::endl;};
  virtual ~AbstractParser() {};
  virtual const std::vector<std::vector<std::string> >& getData() const = 0;
};

#endif /* ABSTRACTPARSER_H_ */
