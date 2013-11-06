//============================================================================
// Name        : PlainParser.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : PlainParser implementation in C++, Ansi-style
//============================================================================

#ifndef PLAINPARSER_H_
#define PLAINPARSER_H_

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

//TODO to inherit from AbstractParser
//#include "AbstractParser.h"
//class PlainParser: public AbstractParser {
class PlainParser {
 private:
  std::ifstream infile;
  char delimiter;
  std::vector<std::vector<std::string> > data;
  //split the line according to the delimiter
  void split(std::string, std::vector<std::string>&);
  //trim the leading and trailing spaces
  void trim(std::string&);

 protected:
  void open(std::string filename);
  void parse();
  void close();

 public:
  PlainParser();
  PlainParser(std::string filename);
  virtual ~PlainParser();
  const std::vector<std::vector<std::string> >& getData() const {
    return data;
  }

  char getDelimiter() const {
    return delimiter;
  }

  void setDelimiter(char delimiter) {
    this->delimiter = delimiter;
  }
  //TODO below functions need to be changed to friend function so that Google Test will be able to access the private function
  void testSplit(PlainParser& parser, std::string line,
                 std::vector<std::string>& vec) {
    parser.split(line, vec);
  }
  ;
  void testTrim(PlainParser& parser, std::string& line){
    parser.trim(line);
  }
};

#endif /* PLAINPARSER_H_ */
