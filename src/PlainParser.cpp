/*
 * PlainParser.cpp
 * This file defines the implementations of PlainParser Class.
 * Please refer to header file for comments of functions
 */
#include <ios>
#include <vector>
#include <string>
#include <istream>
#include <exception>
#include <stdexcept>

#include "PlainParser.h"

using namespace std;

PlainParser::PlainParser()
    : delimiter(' ') {
}
;
PlainParser::PlainParser(string filename)
    : filename(filename),
      delimiter(' ') {
  open();
  parse();
  close();
}
PlainParser::~PlainParser() {
}
void PlainParser::parse() {
  int count = 0;
  try {
    string line;
    while (getline(infile, line)) {
      //trim trailing and leading spaces
      trim(line);
      if (!line.empty()) {
        vector<string> elem;
        split(line, elem);
        data.push_back(elem);
      }
      count++;
    }
  } catch (ifstream::failure& e) {
    cerr << "[FATAL]: Error while parsing " << filename << " at line " << count << endl;
    throw ifstream::failure(e);
  }
}
void PlainParser::open() {
  try {
    //infile.exceptions(ifstream::badbit | ifstream::failbit);
    infile.exceptions(ifstream::badbit);
    infile.open(filename.c_str());
    if(!infile.is_open())
      throw ifstream::failure("IO Errors");
  } catch (ifstream::failure& e) {
    //TODO replace with log4cxx
    cerr << "[FATAL]: Error while opening " << filename << " " << e.what() << endl;
    throw ifstream::failure(e);
  }
}
void PlainParser::close() {
  try {
    infile.close();
  } catch (ifstream::failure& e) {
    //TODO replace with log4cxx
    cerr << "[FATAL]: Error while closing " << filename << endl;
    throw ifstream::failure(e);
  }
}
///Split the line according to the delimiter
///@param line is the desired line to be split by delimiters.
///@param vecforline is a vector which will store the split results
///@return NONE
void PlainParser::split(string line, vector<string>& vecforline) {
  unsigned curpos = 0, foundpos;
  while ((foundpos = line.find(delimiter, curpos)) != string::npos) {
    vecforline.push_back(line.substr(curpos, foundpos));
    curpos = ++foundpos;
  }
  vecforline.push_back(line.substr(curpos));
}
///Split the line according to the delimiter
///@param line is the desired line to be trimmed.
///@return NONE
void PlainParser::trim(string& line) {
  unsigned foundpos = line.find_first_not_of(delimiter);
  if (foundpos != string::npos)
    line.erase(0, foundpos);
  foundpos = line.find_last_not_of(delimiter);
  if (foundpos != 0)
    line.erase(foundpos + 1);
}

