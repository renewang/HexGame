/*
 * Strategy_test.cpp
 *
 *  Created on: Nov 20, 2013
 *      Author: renewang
 */

#include <vector>

#include "Game.h"
#include "Player.h"
#include "Strategy.h"
#include "gtest/gtest.h"

using namespace std;

class StrategyTest : public ::testing::Test {
  virtual void SetUp() {
  }
  ;
  virtual void TearDown() {
  }
  ;
};
TEST_F(StrategyTest,CheckWinnerTest) {
  int numofhexgon = 5;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);  //north to south
  Player playerb(board, hexgonValKind::BLUE);  //west to east
  Strategy strategyred(&board, &playera);
  Strategy strategyblue(&board, &playerb);

  //straight move from west to east
  vector<int> testmove1(numofhexgon);
  for (int i = 0; i < numofhexgon; i++)
    testmove1[i] = (i + 1);

  EXPECT_FALSE(strategyred.isWinner(testmove1));  //north to south
  EXPECT_TRUE(strategyblue.isWinner(testmove1));  //west to east

  //straight move from north to south
  vector<int> testmove2(numofhexgon);
  for (int i = 3; i <= 3 + (numofhexgon * (numofhexgon - 1));
      i = i + numofhexgon)
    testmove2[i / numofhexgon] = i;

  EXPECT_TRUE(strategyred.isWinner(testmove2));  //north to south
  EXPECT_FALSE(strategyblue.isWinner(testmove2));  //west to east

  //straight move along anti-diagonal
  vector<int> testmove3(numofhexgon);
  for (int i = 0; i < numofhexgon; i++)
    testmove3[i] = i * numofhexgon + (numofhexgon - i);

  EXPECT_TRUE(strategyred.isWinner(testmove3));  //north to south
  EXPECT_TRUE(strategyblue.isWinner(testmove3));  //west to east

  //negative test case, move diagonal
  vector<int> testmove4(numofhexgon);
  for (int i = 0; i < numofhexgon; i++)
    testmove4[i] = i * numofhexgon + (i + 1);

  EXPECT_FALSE(strategyred.isWinner(testmove4));  //north to south
  EXPECT_FALSE(strategyblue.isWinner(testmove4));  //west to east

  //move west to east firstly and then north to south (zigzag move)
  //1->2->7->8->13->14->19->20
  vector<int> testmove5(8);
  for (int i = 1; i <= 19; i = i + 6) {
    testmove5[i / 3] = i;
    testmove5[i / 3 + 1] = i + 1;
  }
  EXPECT_FALSE(strategyred.isWinner(testmove5));  //north to south
  EXPECT_TRUE(strategyblue.isWinner(testmove5));  //west to east

  //move north to south firstly and then west to east (zigzag move)
  //1->6->7->12->13->18->19->24
  vector<int> testmove6(8);
  for (int i = 1; i <= 19; i = i + 6) {
    testmove6[i / 3] = i;
    testmove6[i / 3 + 1] = i + 5;
  }
  EXPECT_TRUE(strategyred.isWinner(testmove6));  //north to south
  EXPECT_FALSE(strategyblue.isWinner(testmove6));  //west to east

  //negative test case, move west to east then diagonal
  vector<int> testmove7(5);
  testmove7[0] = 1;
  for (int i = 2; i < 3 * numofhexgon; i = i + 6) {
    testmove7[i / 6 + 1] = i;
  }
  testmove7[numofhexgon - 1] = 15;

  EXPECT_FALSE(strategyred.isWinner(testmove7));  //north to south
  EXPECT_FALSE(strategyblue.isWinner(testmove7));  //west to east

  //negative test case, move diagonal and then north to south
  vector<int> testmove8(numofhexgon + 2);
  testmove8[0] = 1;
  testmove8[1] = 2;
  testmove8[2] = 3;
  testmove8[3] = 8;
  testmove8[4] = 14;
  testmove8[5] = 19;
  testmove8[6] = 24;

  EXPECT_FALSE(strategyred.isWinner(testmove8));  //north to south
  EXPECT_FALSE(strategyblue.isWinner(testmove8));  //west to east

  //purely random
  vector<int> testmove9(numofhexgon);
  int randommove[5] = { 1, 10, 8, 12, 19 };
  copy(randommove, randommove + numofhexgon, testmove9.begin());

  EXPECT_FALSE(strategyred.isWinner(testmove9));  //north to south
  EXPECT_FALSE(strategyblue.isWinner(testmove9));  //west to east

  //random test set 2
  vector<int> testmove10(11);
  int randommove2[11] = { 1, 2, 3, 4, 8, 9, 14, 15, 18, 19, 21 };
  copy(randommove2, randommove2 + 11, testmove10.begin());

  EXPECT_FALSE(strategyred.isWinner(testmove10));  //north to south
  EXPECT_FALSE(strategyblue.isWinner(testmove10));  //west to east

  //purely random to compare the result of MST and approximate approach to find a winner for north to south
  for (unsigned i = 0; i < 1000; i++) {
    vector<int> test;
    bool winner = false;
    bool* emptyindicators = new bool[numofhexgon * numofhexgon];
    fill(emptyindicators, emptyindicators + numofhexgon * numofhexgon, true);
    Game hexboardgame(board);
    while (!winner) {
      int move = strategyred.genNextRandom(emptyindicators);
      test.push_back(move);
      int row = (move - 1) / numofhexgon + 1;
      int col = (move - 1) % numofhexgon + 1;
      hexboardgame.setMove(playera, row, col);
      winner = strategyred.isWinner(test);
    }
    string winnerwho = hexboardgame.getWinner(playera, playerb);
    string playername = playera.getPlayername();
    if (playername != winnerwho) {
      for (unsigned i = 0; i < test.size(); i++)
        cout << test[i] << " ";
      cout << endl;
    }
    EXPECT_EQ(winnerwho, playername);
    delete[] emptyindicators;
  }
  //purely random to compare the result of MST and approximate approach to find a winner for west to east
  HexBoard board2(numofhexgon);
  Player playera2(board2, hexgonValKind::RED);  //north to south
  Player playerb2(board2, hexgonValKind::BLUE);  //west to east
  Strategy strategyblue2(&board2, &playerb2);
  for (unsigned i = 0; i < 1000; i++) {
    vector<int> test;
    bool winner = false;
    bool* emptyindicators = new bool[numofhexgon * numofhexgon];
    fill(emptyindicators, emptyindicators + numofhexgon * numofhexgon, true);
    Game hexboardgame(board2);
    while (!winner) {
      int move = strategyblue.genNextRandom(emptyindicators);
      test.push_back(move);
      int row = (move - 1) / numofhexgon + 1;
      int col = (move - 1) % numofhexgon + 1;
      hexboardgame.setMove(playerb2, row, col);
      winner = strategyblue2.isWinner(test);
    }
    string winnerwho = hexboardgame.getWinner(playera2, playerb2);
    string playername = playerb2.getPlayername();
    if (playername != winnerwho) {
      for (unsigned i = 0; i < test.size(); i++)
        cout << test[i] << " ";
      cout << endl;
    }
    EXPECT_EQ(winnerwho, playername);
    delete[] emptyindicators;
  }
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
