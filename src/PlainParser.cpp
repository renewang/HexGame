/*
 * PlainParser.cpp
 * This file defines the implementations of PlainParser Class.
 * Please refer to header file for comments of functions
 */
#include <vector>
#include <string>

#include "PlainParser.h"

using namespace std;

PlainParser::PlainParser()
    : delimiter(' ') {
}
;
PlainParser::PlainParser(string filename)
    : delimiter(' ') {
  open(filename);
  parse();
  close();
}
PlainParser::~PlainParser() {
}
void PlainParser::parse() {
  string line;
  while (!infile.eof()) {
    getline(infile, line);
    //trim trailing and leading spaces
    trim(line);
    if (!line.empty()) {
      vector<string> elem;
      split(line, elem);
      data.push_back(elem);
    }
  }
}
void PlainParser::open(string filename) {
  infile.open(filename.c_str());
}
void PlainParser::close() {
  infile.close();
}
void PlainParser::split(string line, vector<string>& vecforline) {
  unsigned curpos = 0, foundpos;
  while ((foundpos = line.find(delimiter, curpos)) != string::npos) {
    vecforline.push_back(line.substr(curpos, foundpos));
    curpos = ++foundpos;
  }
  vecforline.push_back(line.substr(curpos));
}
void PlainParser::trim(string& line) {
  unsigned foundpos = line.find_first_not_of(delimiter);
  if (foundpos != string::npos)
    line.erase(0, foundpos);
  foundpos = line.find_last_not_of(delimiter);
  if (foundpos != 0)
    line.erase(foundpos + 1);
}
