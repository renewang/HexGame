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
#include "MinSpanTreeAlgo.h"

using namespace std;

class MinSpanTreeTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    testAlter = new int*[6];
    for (unsigned i = 0; i < 6; i++)
      testAlter[i] = new int[6];

    for (unsigned k = 0; k < 6; k++) {
      for (unsigned i = 0; i < (6 - k); i++) {
        testAlter[i][i + k] = k * k;
        if (k != 0)
          testAlter[i + k][i] = k * k;
      }
    }
  }
  virtual void TearDown() {
    if (testAlter != 0) {
      for (unsigned i = 0; i < 6; i++)
        delete testAlter[i];
      delete[] testAlter;
    }
  }
  int** testAlter;
}
;
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
  int expectRepMatrix[6][6] = { { 0, 17, 2, 9, 24, 28 }, { 17, 0, 0, 0, 0, 0 },
      { 2, 0, 0, 0, 0, 0 }, { 9, 0, 0, 0, 0, 0 }, { 24, 0, 0, 0, 0, 0 }, { 28,
          0, 0, 0, 0, 0 } };

  //test the repGraph representation
  testG.printRepGraph();
  for (unsigned i = 0; i < 6; i++) {
    vector<int> expectVec(expectRepMatrix[i], expectRepMatrix[i] + 6);
    EXPECT_EQ(expectVec, testGsMatrix[i]);
  }
}
TEST_F(MinSpanTreeTest,KruskalMSTCheck) {
  unsigned sizeoftestgraph = 6;
  Graph<string, int> testG(testAlter, sizeoftestgraph);
  MinSpanTreeAlgo mstalgo(testG);

  mstalgo.calculate();

  EXPECT_EQ(5, mstalgo.getTotalminwieght());
  Graph<string, int> msttree = mstalgo.getMsttree();
  string expecttree[6] = { "(1(2(3(4(5(6))))))", "(2(1,3(4(5(6)))))",
      "(3(4(5(6),1(2)))", "(4(5(6),3(2(1))))", "(5(6,4(3(2(1)))))",
      "(6(5(4(3(2(1))))))" };
  for (unsigned i = 1; i <= sizeoftestgraph; i++) {
    const string printMST = msttree.printMST(i);
    EXPECT_EQ(expecttree[i], expecttree[i]);
    EXPECT_EQ(expecttree[i], printMST);
  }
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
