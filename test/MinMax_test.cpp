/*
 * MinMax_test.cpp
 *
 */
#include <bitset>
#include <limits>
#include <iostream>
#include <functional>

#include "gtest/gtest.h"

#include "Game.h"
#include "Player.h"
#include "GameTree.h"
#include "TwoDistancePotential.h"
#include "MonteCarloTreeSearch.h"

using namespace std;
using namespace boost;

class MinMaxTest : public ::testing::Test {
  virtual void SetUp() {
  }
  ;
  virtual void TearDown() {
  }
  ;
};
TEST_F(MinMaxTest,PotentialInitialization) {
  //test with conventional distance, empty cell
  int numofhexgon = 5;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  TwoDistancePotential potentialred(&board, &playera);
  vector<int> redpotentials = potentialred.getPotentials();
  int red[5][5] = { { 1, 1, 1, 1, 1 }, { 2, 2, 2, 2, 3 }, { 3, 3, 3, 4, 5 }, {
      4, 4, 5, 6, 7 }, { 5, 6, 6, 7, 7 } };
  vector<int> redexpect;
  for (int i = 0; i < numofhexgon; ++i)
    for (int j = 0; j < numofhexgon; ++j)
      redexpect.push_back(red[i][j]);
  EXPECT_EQ(redexpect, redpotentials);

  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
  TwoDistancePotential potentialblue(&board, &playerb);
  vector<int> bluepotentials = potentialblue.getPotentials();
  int blue[5][5] = { { 1, 2, 3, 4, 5 }, { 1, 2, 3, 4, 6 }, { 1, 2, 3, 5, 6 }, {
      1, 2, 4, 6, 7 }, { 1, 3, 5, 7, 7 } };
  vector<int> blueexpect;
  for (int i = 0; i < numofhexgon; i++)
    for (int j = 0; j < numofhexgon; j++)
      blueexpect.push_back(blue[i][j]);
  EXPECT_EQ(blueexpect, bluepotentials);
}
TEST_F(MinMaxTest,PotentialRecalculation) {
  vector<int> bwsmoves;
  bwsmoves.push_back(12);
  bwsmoves.push_back(17);

  vector<int> oppsmoves;

  int numofhexgon = 5;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  TwoDistancePotential potentialred(&board, &playera);
  potentialred.ReCalcPotentials(bwsmoves, oppsmoves);

  vector<int> redpotentials = potentialred.getPotentials();
  int red[5][5] = { { 1, 1, 1, 1, 1 }, { 2, 2, 2, 2, 3 }, { 3, 0, 3, 4, 5 }, {
      3, 0, 3, 5, 5 }, { 3, 3, 4, 4, 4 } };

  vector<int> redexpect;
  for (int i = 0; i < numofhexgon; ++i)
    for (int j = 0; j < numofhexgon; ++j)
      redexpect.push_back(red[i][j]);
  EXPECT_EQ(redexpect, redpotentials);
}
TEST_F(MinMaxTest,GameTreeConstruct) {
  //test expand child
  //expand from root
  GameTree tree(0);
  tree.expandLeaf("0",1,'R');
  tree.expandLeaf("0",2,'B');
  EXPECT_EQ("((1:R)0:W(2:B))",tree.printGameTree("0"));
  //expand from the leaf node
  tree.expandLeaf("1",3);
  tree.expandLeaf("1",4);
  tree.expandLeaf("3",5);
  tree.expandLeaf("2",6);
  EXPECT_EQ("((((5:R)3:B)1:R(4:B))0:W((6:R)2:B))",tree.printGameTree("0"));


}
TEST_F(MinMaxTest,SimulationCombine) {
 //test with 3x3 board


}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
