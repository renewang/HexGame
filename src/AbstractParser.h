//============================================================================
// Name        : AbstractParser.h
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Base class for file parser
//============================================================================

#ifndef ABSTRACTPARSER_H_
#define ABSTRACTPARSER_H_

#include <iostream>
#include <fstream>

class AbstractParser {

 public:
  AbstractParser();
  AbstractParser(std::string filename) {

  }
  ;
  virtual ~AbstractParser();

 protected:
   void open();
   void parse();
   void close();
};

#endif /* ABSTRACTPARSER_H_ */
