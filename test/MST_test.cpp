//============================================================================
// Name        : MST.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Google Test for Minimal Spanning Tree
//============================================================================
#include <algorithm>
#include <string>
#include <vector>
#include <cstdlib>
#include <typeinfo>

#include "Graph.h"
#include "PlainParser.h"
#include "gtest/gtest.h"

using namespace std;

class MinSpanTreeTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
  }
  virtual void TearDown() {
  }
};
TEST_F(MinSpanTreeTest,PlainParserCheck) {

  //test split with default delimiter, space
  PlainParser testParser1;
  string testSplitArray[3] = { "1", "1 2", "1 2 3" };

  for (unsigned i = 0; i < 3; i++) {
    vector<string> vecActual;
    vector<int> vecActualInt;
    testParser1.testSplit(testParser1, testSplitArray[i], vecActual);
    vector<int> vecExpect;
    for (unsigned j = 1; j <= (i + 1); j++)
      vecExpect.push_back(static_cast<int>(j));
    for (unsigned j = 0; j < vecActual.size(); j++)
      vecActualInt.push_back(atoi(vecActual[j].c_str()));
    EXPECT_EQ(vecExpect, vecActualInt);
  }

  //test split with default delimiter, tab
  PlainParser testParser2;
  testParser2.setDelimiter('\t');
  string testSplitArray2[3] = { "1", "1\t2", "1\t2\t3" };

  for (unsigned i = 0; i < 3; i++) {
    vector<string> vecActual;
    vector<int> vecActualInt;
    testParser2.testSplit(testParser2, testSplitArray2[i], vecActual);
    vector<int> vecExpect;
    for (unsigned j = 1; j <= (i + 1); j++)
      vecExpect.push_back(static_cast<int>(j));
    for (unsigned j = 0; j < vecActual.size(); j++)
      vecActualInt.push_back(atoi(vecActual[j].c_str()));
    EXPECT_EQ(vecExpect, vecActualInt);
  }

  //test trim
  string test = "   I want to trim this string    ";
  string actual(test);
  string expect = "I want to trim this string";

  PlainParser testParser3;
  testParser3.testTrim(testParser3, actual);
  EXPECT_EQ(expect, actual);

  //test the real sample data
  string filename =
      "/Users/renewang/Documents/workspace/MST/resource/Smalldata.txt";
  PlainParser parser(filename);
  vector<vector<string> > expectData = parser.getData();
  ASSERT_EQ(11, static_cast<int>(expectData.size()));
  for (unsigned i = 0; i < expectData.size(); i++) {
    if (i == 0)
      EXPECT_EQ(1, static_cast<int>(expectData[0].size()));
    else
      EXPECT_EQ(3, static_cast<int>(expectData[i].size()));
  }
  EXPECT_EQ("6", expectData[0][0]);

  //test the transformation
  Graph<string, int> testG(parser);
  vector<vector<int> > testGsMatrix = testG.getRepmatrix();
  int expectRepMatrix[6][6] = {
      { 0,17, 2, 9,24,28 },
      {17, 0, 0, 0, 0, 0 },
      { 2, 0, 0, 0, 0, 0 },
      { 9, 0, 0, 0, 0, 0 },
      {24, 0, 0, 0, 0, 0 },
      {28, 0, 0, 0, 0, 0 } };

  //test the repGraph representation
  testG.printRepGraph();
  for(unsigned i = 0; i < 6; i++){
    vector<int> expectVec(expectRepMatrix[i],expectRepMatrix[i]+6);
    EXPECT_EQ(expectVec, testGsMatrix[i]);
  }
}
TEST_F(MinSpanTreeTest,KruskalMSTCheck) {

}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
