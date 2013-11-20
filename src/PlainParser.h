/*
 * PlainParser.h
 * This file defines a simple parser to parser plain text file whose delimiter is space.
 * The size of nodes is defined in the first line while the edges are defined in the following lines in the following format.
 * (index of source node ranging [1 to (size of nodes -1)]) (index of destination node ranging [1 to (size of nodes -1)]) (weight)
 * Expect to have even number of edges for undirected graph
 */

#ifndef PLAINPARSER_H_
#define PLAINPARSER_H_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "AbstractParser.h"
#include "gtest/gtest_prod.h"

//#include "AbstractParser.h"
//class PlainParser: public AbstractParser {
/*
 * PlainParser Class is a class define the behaviors of a parser for plain test file
 * PlainParser(): default constructor, takes no parameters and do nothing
 *
 * PlainParser(std::string filename): constructor takes the input file name as argument. Sample Usage:
 * string filename = "Smalldata.txt";
 * PlainParser parser(filename);
 */
class PlainParser: public AbstractParser{
 private:
  std::string filename;
  std::ifstream infile; //the file stream
  char delimiter; //the delimiter to distinguish columns
  //a two dimensional vector to store the parsed information of the file
  //the first dimension represents the line and second dimension represents the parsed result of each column
  std::vector<std::vector<std::string> > data;
  void split(std::string, std::vector<std::string>&);//split the line according to the delimiter
  void trim(std::string&);//trim the leading and trailing spaces

  //for test
  friend class MinSpanTreeTest;
  FRIEND_TEST(MinSpanTreeTest,PlainParserCheck);


 protected:
  void open();
  void parse();
  void close();

 public:
  PlainParser();//default constructor
  PlainParser(std::string filename);
  virtual ~PlainParser();

  //Getter for parser data
  const std::vector<std::vector<std::string> >& getData() const {
    return data;
  }
  //Getter for current delimiter
  //INPUT: NONE
  //OUTPUT:
  //the character used as delimiter
  char getDelimiter() const {
    return delimiter;
  }
  //Setter for delimiter
  //INPUT:
  //delimiter: delimiter character
  //OUTPUT: NONE
  void setDelimiter(char delimiter) {
    this->delimiter = delimiter;
  }
};

#endif /* PLAINPARSER_H_ */
