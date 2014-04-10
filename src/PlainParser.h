/*
 * PlainParser.h
 * This file defines a simple parser to parser plain text file whose delimiter is space.
 */

#ifndef PLAINPARSER_H_
#define PLAINPARSER_H_

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "AbstractParser.h"

#ifndef NDEBUG
#include "gtest/gtest_prod.h"
#endif

/**
 * PlainParser class defines a simple parser to parser plain text file whose delimiter is space. <br/>
 * The size of nodes is defined in the first line while the edges are defined in the following lines in the following format.<br/>
 * (index of source node ranging [1 to (size of nodes -1)]) (index of destination node ranging [1 to (size of nodes -1)]) (weight)<br/>
 * Expect to have even number of edges for undirected graph<br/>
 * PlainParser Class is a class define the behaviors of a parser for plain test file<br/>
 * PlainParser(): default constructor, takes no parameters and do nothing<br/>
 *
 * PlainParser(std::string filename): constructor takes the input file name as argument. <br/>
 * Sample Usage:<br/>
 * string filename = "Smalldata.txt";<br/>
 * PlainParser parser(filename);<br/>
 */
class PlainParser: public AbstractParser{
 private:
  std::string filename; ///< the file name
  std::ifstream infile; ///< the file stream
  char delimiter; ///<the delimiter to distinguish columns
  std::vector<std::vector<std::string> > data;   /*!< a two dimensional vector to store the parsed information of the
                                                      file. the first dimension represents the line and second
                                                      dimension represents the parsed result of each column */
  //Split the line according to the delimiter
  void split(std::string, std::vector<std::string>&);
  //trim the leading and trailing spaces
  void trim(std::string&);


#ifndef NDEBUG
  //for test
  friend class MinSpanTreeTest;
  FRIEND_TEST(MinSpanTreeTest,PlainParserCheck);
#endif


 protected:
  ///See AbstractParser::open()
  void open();
  ///See AbstractParser::parse()
  void parse();
  ///See AbstractParser::close()
  void close();

 public:
  ///default constructor
  PlainParser();
  ///User defined constructor which take filename as input
  ///@param filename is filename of the paring graph file
  PlainParser(std::string filename);
  ///destructor
  virtual ~PlainParser();

  ///Getter for parser data, See AbstractParser::getData()
  const std::vector<std::vector<std::string> >& getData() const {
    return data;
  }
  ///Getter for current delimiter
  ///@param NONE
  ///@return the character used as delimiter
  char getDelimiter() const {
    return delimiter;
  }
  ///Setter for delimiter
  ///@param:delimiter: delimiter character
  ///@return: NONE
  void setDelimiter(char delimiter) {
    this->delimiter = delimiter;
  }
};

#endif /* PLAINPARSER_H_ */
