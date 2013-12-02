/*
 * Strategy_test.cpp
 *
 *  Created on: Nov 20, 2013
 *      Author: renewang
 */

#include <vector>
#include <utility>

#include "Game.h"
#include "Player.h"
#include "Strategy.h"
#include "PriorityQueue.h"
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
bool myfunction(pair<int, int> i, pair<int, int> j) {
  return (i.second < j.second);
}
TEST_F(StrategyTest,CheckWinnerTest) {
  int numofhexgon = 5;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);  //north to south
  Player playerb(board, hexgonValKind::BLUE);  //west to east
  Strategy strategyred(&board, &playera);
  Strategy strategyblue(&board, &playerb);

  //straight move from west to east
  vector<set<int> > testmove1red(numofhexgon);
  strategyred.initTransformVector(testmove1red);
  for (int i = 0; i < numofhexgon; i++)
    strategyred.assignValue(testmove1red, i + 1,
                            playera.getWestToEastCondition());

  vector<set<int> > testmove1blue(numofhexgon);
  strategyblue.initTransformVector(testmove1blue);
  for (int i = 0; i < numofhexgon; i++)
    strategyblue.assignValue(testmove1blue, i + 1,
                             playerb.getWestToEastCondition());

  EXPECT_FALSE(
      strategyred.isWinner(testmove1red, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove1blue, playerb.getWestToEastCondition()));  //west to east

  //straight move from north to south
  vector<set<int> > testmove2red(numofhexgon);
  strategyred.initTransformVector(testmove2red);

  for (int i = 3; i <= 3 + (numofhexgon * (numofhexgon - 1));
      i = i + numofhexgon)
    strategyred.assignValue(testmove2red, i, playera.getWestToEastCondition());

  vector<set<int> > testmove2blue(numofhexgon);
  strategyred.initTransformVector(testmove2blue);

  for (int i = 3; i <= 3 + (numofhexgon * (numofhexgon - 1));
      i = i + numofhexgon)
    strategyblue.assignValue(testmove2blue, i,
                             playerb.getWestToEastCondition());

  EXPECT_TRUE(
      strategyred.isWinner(testmove2red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove2blue, playerb.getWestToEastCondition()));  //west to east

  //straight move along anti-diagonal
  vector<set<int> > testmove3red(numofhexgon);
  strategyred.initTransformVector(testmove3red);

  for (int i = 0; i < numofhexgon; i++)
    strategyred.assignValue(testmove3red, i * numofhexgon + (numofhexgon - i),
                            playera.getWestToEastCondition());

  vector<set<int> > testmove3blue(numofhexgon);
  strategyblue.initTransformVector(testmove3blue);

  for (int i = 0; i < numofhexgon; i++)
    strategyblue.assignValue(testmove3blue, i * numofhexgon + (numofhexgon - i),
                             playerb.getWestToEastCondition());

  EXPECT_TRUE(
      strategyred.isWinner(testmove3red, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove3blue, playerb.getWestToEastCondition()));  //west to east

  //negative test case, move diagonal
  vector<set<int> > testmove4red(numofhexgon);
  strategyred.initTransformVector(testmove4red);

  for (int i = 0; i < numofhexgon; i++)
    strategyred.assignValue(testmove4red, i * numofhexgon + (i + 1),
                            playera.getWestToEastCondition());

  vector<set<int> > testmove4blue(numofhexgon);
  strategyblue.initTransformVector(testmove4blue);

  for (int i = 0; i < numofhexgon; i++)
    strategyblue.assignValue(testmove4blue, i * numofhexgon + (i + 1),
                             playerb.getWestToEastCondition());

  EXPECT_FALSE(
      strategyred.isWinner(testmove4red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove4blue, playerb.getWestToEastCondition()));  //west to east

  //move west to east firstly and then north to south (zigzag move)
  //1->2->7->8->13->14->19->20
  vector<set<int> > testmove5red(numofhexgon), testmove5blue(numofhexgon);
  strategyred.initTransformVector(testmove5red);
  strategyblue.initTransformVector(testmove5blue);
  for (int i = 1; i <= 19; i = i + 6) {
    strategyred.assignValue(testmove5red, i, playera.getWestToEastCondition());
    strategyred.assignValue(testmove5red, i + 1,
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove5blue, i,
                             playerb.getWestToEastCondition());
    strategyblue.assignValue(testmove5blue, i + 1,
                             playerb.getWestToEastCondition());
  }
  EXPECT_FALSE(
      strategyred.isWinner(testmove5red, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove5blue, playerb.getWestToEastCondition()));  //west to east

  //move north to south firstly and then west to east (zigzag move)
  //1->6->7->12->13->18->19->24
  vector<set<int> > testmove6red(numofhexgon), testmove6blue(numofhexgon);
  strategyred.initTransformVector(testmove6red);
  strategyblue.initTransformVector(testmove6blue);
  for (int i = 1; i <= 19; i = i + 6) {
    strategyred.assignValue(testmove6red, i, playera.getWestToEastCondition());
    strategyred.assignValue(testmove6red, i + 5,
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove6blue, i,
                             playerb.getWestToEastCondition());
    strategyblue.assignValue(testmove6blue, i + 5,
                             playerb.getWestToEastCondition());
  }

  EXPECT_TRUE(
      strategyred.isWinner(testmove6red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove6blue, playerb.getWestToEastCondition()));  //west to east

//negative test case, move west to east then diagonal
  vector<set<int> > testmove7red(numofhexgon), testmove7blue(numofhexgon);
  strategyred.initTransformVector(testmove7red);
  strategyblue.initTransformVector(testmove7blue);

  strategyred.assignValue(testmove6red, 1, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove6blue, 1, playerb.getWestToEastCondition());
  for (int i = 2; i < 3 * numofhexgon; i = i + 6) {
    strategyred.assignValue(testmove7red, i, playera.getWestToEastCondition());
    strategyblue.assignValue(testmove7blue, i,
                             playerb.getWestToEastCondition());
  }
  strategyred.assignValue(testmove7red, 15, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove7blue, 15, playerb.getWestToEastCondition());

  EXPECT_FALSE(
      strategyred.isWinner(testmove7red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove7blue, playerb.getWestToEastCondition()));  //west to east

//negative test case, move diagonal and then north to south
  vector<set<int> > testmove8red(numofhexgon), testmove8blue(numofhexgon);
  strategyred.initTransformVector(testmove8red);
  strategyblue.initTransformVector(testmove8blue);

  strategyred.assignValue(testmove8red, 1, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove8blue, 1, playerb.getWestToEastCondition());
  strategyred.assignValue(testmove8red, 2, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove8blue, 2, playerb.getWestToEastCondition());
  strategyred.assignValue(testmove8red, 3, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove8blue, 3, playerb.getWestToEastCondition());
  strategyred.assignValue(testmove8red, 8, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove8blue, 8, playerb.getWestToEastCondition());
  strategyred.assignValue(testmove8red, 14, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove8blue, 14, playerb.getWestToEastCondition());
  strategyred.assignValue(testmove8red, 19, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove8blue, 19, playerb.getWestToEastCondition());
  strategyred.assignValue(testmove8red, 24, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove8blue, 24, playerb.getWestToEastCondition());

  EXPECT_FALSE(
      strategyred.isWinner(testmove8red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove8blue, playerb.getWestToEastCondition()));  //west to east

//purely random
  vector<set<int> > testmove9red(numofhexgon), testmove9blue(numofhexgon);
  strategyred.initTransformVector(testmove9red);
  strategyblue.initTransformVector(testmove9blue);

  int randommove[5] = { 1, 10, 8, 12, 19 };
  for (int i = 0; i < 5; i++) {
    strategyred.assignValue(testmove9red, randommove[i],
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove9blue, randommove[i],
                             playerb.getWestToEastCondition());
  }

  EXPECT_FALSE(
      strategyred.isWinner(testmove9red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove9blue, playerb.getWestToEastCondition()));  //west to east

//random test set 2
  vector<set<int> > testmove10red(numofhexgon), testmove10blue(numofhexgon);
  strategyred.initTransformVector(testmove10red);
  strategyblue.initTransformVector(testmove10blue);
  int randommove2[11] = { 1, 2, 3, 4, 8, 9, 14, 15, 18, 19, 21 };
  for (int i = 0; i < 11; i++) {
    strategyred.assignValue(testmove10red, randommove2[i],
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove10blue, randommove2[i],
                             playerb.getWestToEastCondition());
  }

  EXPECT_FALSE(
      strategyred.isWinner(testmove10red, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove10blue, playerb.getWestToEastCondition()));  //west to east

//random test set 3
  vector<set<int> > testmove11red(numofhexgon), testmove11blue(numofhexgon);
  strategyred.initTransformVector(testmove10red);
  strategyblue.initTransformVector(testmove10blue);
  int randommove3[12] = { 2, 17, 13, 6, 15, 24, 4, 9, 22, 19, 7, 8 };
  for (int i = 0; i < 12; i++) {
    strategyred.assignValue(testmove11red, randommove3[i],
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove11blue, randommove3[i],
                             playerb.getWestToEastCondition());
  }

  EXPECT_TRUE(
      strategyred.isWinner(testmove11red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove11blue, playerb.getWestToEastCondition()));  //west to east

//additional test
  vector<set<int> > testmove12red(numofhexgon), testmove12blue(numofhexgon);
  strategyred.initTransformVector(testmove12red);
  strategyblue.initTransformVector(testmove12blue);
  int randommove4[5] = { 5, 10, 15, 19, 24 };
  for (int i = 0; i < 5; i++) {
    strategyred.assignValue(testmove12red, randommove4[i],
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove12blue, randommove4[i],
                             playerb.getWestToEastCondition());
  }

  EXPECT_TRUE(
      strategyred.isWinner(testmove12red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove12blue, playerb.getWestToEastCondition()));  //west to east

//additional test
  vector<set<int> > testmove13red(numofhexgon), testmove13blue(numofhexgon);
  strategyred.initTransformVector(testmove13red);
  strategyblue.initTransformVector(testmove13blue);

  for (unsigned i = 0; i < 7; i++) {
    strategyred.assignValue(testmove13red, i + 12,
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove13blue, i + 12,
                             playerb.getWestToEastCondition());
  }
  strategyred.assignValue(testmove13red, 1, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove13blue, 1, playerb.getWestToEastCondition());

  strategyred.assignValue(testmove13red, 3, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove13blue, 3, playerb.getWestToEastCondition());

  strategyred.assignValue(testmove13red, 4, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove13blue, 4, playerb.getWestToEastCondition());

  strategyred.assignValue(testmove13red, 7, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove13blue, 7, playerb.getWestToEastCondition());

  strategyred.assignValue(testmove13red, 8, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove13blue, 8, playerb.getWestToEastCondition());

  strategyred.assignValue(testmove13red, 10, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove13blue, 10,
                           playerb.getWestToEastCondition());

  EXPECT_FALSE(
      strategyred.isWinner(testmove13red, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove13blue, playerb.getWestToEastCondition()));  //west to east

//test reverse for column
  vector<set<int> > testmove14red(numofhexgon), testmove14blue(numofhexgon);
  strategyred.initTransformVector(testmove14red);
  strategyblue.initTransformVector(testmove14blue);
  for (unsigned i = 0; i < 3; i++) {
    strategyred.assignValue(testmove14red, i + 16,
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove14blue, i + 16,
                             playerb.getWestToEastCondition());
  }
  strategyred.assignValue(testmove14red, 14, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove14blue, 14,
                           playerb.getWestToEastCondition());

  strategyred.assignValue(testmove14red, 9, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove14blue, 9, playerb.getWestToEastCondition());

  strategyred.assignValue(testmove14red, 5, playera.getWestToEastCondition());
  strategyblue.assignValue(testmove14blue, 5, playerb.getWestToEastCondition());

  EXPECT_FALSE(
      strategyred.isWinner(testmove14red, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove14blue, playerb.getWestToEastCondition()));  //west to east

//test reverse path
  vector<set<int> > testmove15red(numofhexgon), testmove15blue(numofhexgon);
  strategyred.initTransformVector(testmove15red);
  strategyblue.initTransformVector(testmove15blue);
  int randommove5[12] = { 1, 6, 11, 16, 17, 13, 8, 9, 10, 15, 20, 25 };
  Game hexboardgame(board);
  for (unsigned i = 0; i < 12; i++) {
    strategyred.assignValue(testmove15red, randommove5[i],
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove15blue, randommove5[i],
                             playerb.getWestToEastCondition());
    hexboardgame.setMove(playera, (randommove5[i]-1)/5+1, (randommove5[i]-1)%5+1);
  }

  EXPECT_TRUE(
      strategyred.isWinner(testmove15red, playera.getWestToEastCondition()));  //north to south
  cout << hexboardgame.showView(playera, playerb) <<endl;
  EXPECT_TRUE(
      strategyblue.isWinner(testmove15blue, playerb.getWestToEastCondition()));  //west to east

  vector<set<int> > testmove17red(numofhexgon), testmove17blue(numofhexgon);
  strategyred.initTransformVector(testmove17red);
  strategyblue.initTransformVector(testmove17blue);
  int randommove7[9] = { 2, 7, 9, 10, 12, 13, 15, 19, 24 };
  for (unsigned i = 0; i < 9; i++) {
    strategyred.assignValue(testmove17red, randommove7[i],
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove17blue, randommove7[i],
                             playerb.getWestToEastCondition());
  }

  EXPECT_TRUE(
      strategyred.isWinner(testmove17red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove17blue, playerb.getWestToEastCondition()));  //west to east

  vector<set<int> > testmove18red(numofhexgon), testmove18blue(numofhexgon);
  strategyred.initTransformVector(testmove18red);
  strategyblue.initTransformVector(testmove18blue);
  int randommove8[18] = { 9, 20, 19, 8, 22, 24, 21, 23, 11, 12, 7, 1, 15, 5, 3,
      25, 2, 18 };
  for (unsigned i = 0; i < 18; i++) {
    strategyred.assignValue(testmove18red, randommove8[i],
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove18blue, randommove8[i],
                             playerb.getWestToEastCondition());
  }

  EXPECT_FALSE(
      strategyred.isWinner(testmove18red, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove18blue, playerb.getWestToEastCondition()));  //west to east

  vector<set<int> > testmove19red(numofhexgon), testmove19blue(numofhexgon);
  strategyred.initTransformVector(testmove19red);
  strategyblue.initTransformVector(testmove19blue);
  int randommove9[11] = { 6, 24, 5, 22, 13, 18, 16, 2, 1, 8, 12 };
  for (unsigned i = 0; i < 11; i++) {
    strategyred.assignValue(testmove19red, randommove9[i],
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove19blue, randommove9[i],
                             playerb.getWestToEastCondition());
  }
  EXPECT_FALSE(
      strategyred.isWinner(testmove19red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove19blue, playerb.getWestToEastCondition()));  //west to east

  vector<set<int> > testmove20red(numofhexgon), testmove20blue(numofhexgon);
  strategyred.initTransformVector(testmove20red);
  strategyblue.initTransformVector(testmove20blue);
  int randommove10[9] = { 11, 7, 14, 17, 13, 5, 21, 12, 18 };
  for (unsigned i = 0; i < 9; i++) {
    strategyred.assignValue(testmove20red, randommove10[i],
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove20blue, randommove10[i],
                             playerb.getWestToEastCondition());
  }

  EXPECT_FALSE(
      strategyred.isWinner(testmove20red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove20red, playerb.getWestToEastCondition()));  //west to east

//potential false positive problem

  vector<set<int> > testmove21red(numofhexgon), testmove21blue(numofhexgon);
  strategyred.initTransformVector(testmove21red);
  strategyblue.initTransformVector(testmove21blue);
  int randommove11[8] = { 1, 6, 7, 13, 14, 17, 18, 21 };

  for (unsigned i = 0; i < 8; i++) {
    strategyred.assignValue(testmove21red, randommove11[i],
                            playera.getWestToEastCondition());
    strategyblue.assignValue(testmove21blue, randommove11[i],
                             playerb.getWestToEastCondition());
  }
  EXPECT_FALSE(
      strategyred.isWinner(testmove21red, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove21blue, playerb.getWestToEastCondition()));  //west to east

}
TEST_F(StrategyTest,CheckWinnerTestTwo) {
  int numofhexgon = 5;

//purely random to compare the result of MST and approximate approach to find a winner for north to south
  for (unsigned i = 0; i < 3000; i++) {
    HexBoard board(numofhexgon);
    Player playera(board, hexgonValKind::RED);  //north to south
    Player playerb(board, hexgonValKind::BLUE);  //west to east
    Strategy strategyred(&board, &playera);
    vector<set<int> > test(numofhexgon);
    strategyred.initTransformVector(test);
    bool winner = false;
    bool* emptyindicators = new bool[numofhexgon * numofhexgon];
    fill(emptyindicators, emptyindicators + numofhexgon * numofhexgon, true);
    Game hexboardgame(board);
    while (!winner) {
      unsigned portionofempty = (unsigned) ((count(
          emptyindicators, emptyindicators + numofhexgon * numofhexgon, true)
          / (float) numofhexgon * numofhexgon));
      int move = strategyred.genNextRandom(emptyindicators, portionofempty);
      strategyred.assignValue(test, move, playera.getWestToEastCondition());
      int row = (move - 1) / numofhexgon + 1;
      int col = (move - 1) % numofhexgon + 1;
      hexboardgame.setMove(playera, row, col);
      winner = strategyred.isWinner(test, playera.getWestToEastCondition());
    }
    string winnerwho = hexboardgame.getWinner(playera, playerb);
    string playername = playera.getPlayername();
    if (playername != winnerwho) {
      for (unsigned k = 0; k < test.size(); k++)
        for (auto j : test[k])
          cout << (k * numofhexgon + j + 1) << " ";
      cout << endl;
      cout << hexboardgame.showView(playera, playerb);
    }
    EXPECT_EQ(winnerwho, playername);
    delete[] emptyindicators;
  }
//purely random to compare the result of MST and approximate approach to find a winner for west to east
  for (unsigned i = 0; i < 3000; i++) {
    HexBoard board(numofhexgon);
    Game hexboardgame(board);
    Player playera(board, hexgonValKind::RED);  //north to south
    Player playerb(board, hexgonValKind::BLUE);  //west to east
    Strategy strategyblue(&board, &playerb);
    vector<set<int> > test(numofhexgon);
    strategyblue.initTransformVector(test);
    bool winner = false;
    bool* emptyindicators = new bool[numofhexgon * numofhexgon];
    fill(emptyindicators, emptyindicators + numofhexgon * numofhexgon, true);
    while (!winner) {
      unsigned portionofempty = count(
          emptyindicators, emptyindicators + numofhexgon * numofhexgon, true);
      int move = strategyblue.genNextRandom(emptyindicators, portionofempty);
      strategyblue.assignValue(test, move, playerb.getWestToEastCondition());
      int row = (move - 1) / numofhexgon + 1;
      int col = (move - 1) % numofhexgon + 1;
      hexboardgame.setMove(playerb, row, col);
      winner = strategyblue.isWinner(test, playerb.getWestToEastCondition());
    }
    string winnerwho = hexboardgame.getWinner(playera, playerb);
    string playername = playerb.getPlayername();
    if (playername != winnerwho) {
      for (unsigned k = 0; k < test.size(); k++)
        for (auto j : test[k])
          cout << (k * numofhexgon + j + 1) << " ";
      cout << endl;
      cout << hexboardgame.showView(playera, playerb);
    }
    EXPECT_EQ(winnerwho, playername);
    delete[] emptyindicators;
  }
}
TEST_F(StrategyTest,CheckWinnerElevenTest) {
  int numofhexgon = 11;
  for (unsigned i = 0; i < 3000; i++) {
    HexBoard board(numofhexgon);
    Player playera(board, hexgonValKind::RED);  //north to south
    Player playerb(board, hexgonValKind::BLUE);  //west to east
    Strategy strategyred(&board, &playera);
    Strategy strategyblue(&board, &playerb);
    Game hexboardgame(board);
    vector<set<int> > test(numofhexgon);
    strategyred.initTransformVector(test);
    bool winner = false;
    bool* emptyindicators = new bool[numofhexgon * numofhexgon];
    fill(emptyindicators, emptyindicators + numofhexgon * numofhexgon, true);
    while (!winner) {
      unsigned portionofempty = (unsigned) ((count(
          emptyindicators, emptyindicators + numofhexgon * numofhexgon, true)
          / (float) numofhexgon * numofhexgon));
      int move = strategyred.genNextRandom(emptyindicators, portionofempty);
      strategyred.assignValue(test, move, playera.getWestToEastCondition());
      int row = (move - 1) / numofhexgon + 1;
      int col = (move - 1) % numofhexgon + 1;
      hexboardgame.setMove(playera, row, col);
      winner = strategyred.isWinner(test, playera.getWestToEastCondition());
    }
    string winnerwho = hexboardgame.getWinner(playera, playerb);
    string playername = playera.getPlayername();
    if (playername != winnerwho) {
      for (unsigned k = 0; k < test.size(); k++)
        for (auto j : test[k])
          cout << (k * numofhexgon + j + 1) << " ";
      cout << endl;
      cout << hexboardgame.showView(playera, playerb);
    }
    EXPECT_EQ(winnerwho, playername);
    delete[] emptyindicators;
  }
  for (unsigned i = 0; i < 3000; i++) {
    HexBoard board(numofhexgon);
    Player playera(board, hexgonValKind::RED);  //north to south
    Player playerb(board, hexgonValKind::BLUE);  //west to east
    Strategy strategyblue(&board, &playerb);
    Game hexboardgame(board);
    vector<set<int> > test(numofhexgon);
    strategyblue.initTransformVector(test);
    bool winner = false;
    bool* emptyindicators = new bool[numofhexgon * numofhexgon];
    fill(emptyindicators, emptyindicators + numofhexgon * numofhexgon, true);
    while (!winner) {
      unsigned portionofempty = count(
          emptyindicators, emptyindicators + numofhexgon * numofhexgon, true);
      int move = strategyblue.genNextRandom(emptyindicators, portionofempty);
      strategyblue.assignValue(test, move, playerb.getWestToEastCondition());
      int row = (move - 1) / numofhexgon + 1;
      int col = (move - 1) % numofhexgon + 1;
      hexboardgame.setMove(playerb, row, col);
      winner = strategyblue.isWinner(test, playerb.getWestToEastCondition());
    }
    string winnerwho = hexboardgame.getWinner(playera, playerb);
    string playername = playerb.getPlayername();
    if (playername != winnerwho) {
      for (unsigned k = 0; k < test.size(); k++)
        for (auto j : test[k])
          cout << (k * numofhexgon + j + 1) << " ";
      cout << endl;
      cout << hexboardgame.showView(playera, playerb);
    }
    EXPECT_EQ(winnerwho, playername);
    delete[] emptyindicators;
  }
}
TEST_F(StrategyTest,CheckGenMoveForPair) {
  int numofhexgon = 5;
  for (unsigned i = 0; i < 1000; i++) {
    HexBoard board(numofhexgon);
    Player playera(board, hexgonValKind::RED);  //north to south, babywatson, 'O'
    Player playerb(board, hexgonValKind::BLUE);  //west to east, virtual opponent, 'X'
    Strategy strategyred(&board, &playera);

    Game hexboardgame(board);

    bool* emptyindicators = new bool[board.getSizeOfVertices()];
    vector<set<int> > babywatsons(numofhexgon), opponents(numofhexgon);
    strategyred.initTransformVector(babywatsons);
    strategyred.initTransformVector(opponents);

    for (int j = 0; j < board.getSizeOfVertices(); j++) {
      //set the current empty location as true
      if (board.getNodeValue(j + 1) == hexgonValKind::EMPTY)
        emptyindicators[j] = true;
      else {
        emptyindicators[j] = false;
        if (board.getNodeValue(j + 1) == playera.getPlayerlabel())
          strategyred.assignValue(babywatsons, j + 1,
                                  playera.getWestToEastCondition());
        else
          strategyred.assignValue(opponents, j + 1,
                                  playerb.getWestToEastCondition());
      }
    }
    unsigned portionofempty = count(emptyindicators,
                                    emptyindicators + board.getSizeOfVertices(),
                                    true);

    int winner = 0, nextmove = -1;
    while (winner == 0 && portionofempty > 0) {

      //random generate a move for baby watson
      if (portionofempty > 0) {
        int move = strategyred.genNextRandom(emptyindicators, portionofempty);
        if (nextmove < 0)
          nextmove = move;
        strategyred.assignValue(babywatsons, move,
                                playera.getWestToEastCondition());
        portionofempty--;
      }

      //random generate a move for virtual opponent
      if (portionofempty > 0) {
        int move = strategyred.genNextRandom(emptyindicators, portionofempty);
        strategyred.assignValue(opponents, move,
                                playerb.getWestToEastCondition());
        portionofempty--;
      }
    }
    winner = strategyred.checkWinnerExist(babywatsons, opponents);
    if (winner == 0) {
      //set move for babywatsons
      cout << "babywatsons has moved " << babywatsons.size() << endl;
      for (unsigned k = 0; k < babywatsons.size(); k++) {
        for (auto j : babywatsons[k]) {
          int moverow = k + 1;
          int movecol = j + 1;
          hexboardgame.setMove(playera, moverow, movecol);
        }
      }
      //set move for virtual opponent
      cout << "virtual opponent has moved " << opponents.size() << endl;
      for (unsigned k = 0; k < opponents.size(); k++) {
        for (auto j : opponents[k]) {
          int moverow = k + 1;
          int movecol = j + 1;
          hexboardgame.setMove(playerb, moverow, movecol);
        }
      }
      string winnersname = hexboardgame.getWinner(playera, playerb);
      cout << "actual winner = " << winnersname << endl;
      cout << hexboardgame.showView(playera, playerb);
    }
    EXPECT_NE(0, winner);
  }
}
TEST_F(StrategyTest,CheckGenNextFillBasic) {
  int numofhexgon = 5;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);   //north to south, babywatson, 'O'
  Strategy strategyred(&board, &playera);
  Game hexboardgame(board);
  bool* emptyindicators = new bool[board.getSizeOfVertices()];
  vector<set<int> > babywatsons(numofhexgon);
  strategyred.initTransformVector(babywatsons);

  hexboardgame.setMove(playera, 1, 3);
  hexboardgame.setMove(playera, 1, 4);
  hexboardgame.setMove(playera, 2, 2);
  hexboardgame.setMove(playera, 2, 4);
  hexboardgame.setMove(playera, 2, 5);
  hexboardgame.setMove(playera, 3, 2);
  hexboardgame.setMove(playera, 3, 3);
  hexboardgame.setMove(playera, 3, 4);
  hexboardgame.setMove(playera, 3, 5);
  hexboardgame.setMove(playera, 4, 1);
  hexboardgame.setMove(playera, 5, 2);
  hexboardgame.setMove(playera, 5, 4);

  for (int j = 0; j < board.getSizeOfVertices(); j++) {
    //set the current empty location as true
    if (board.getNodeValue(j + 1) == hexgonValKind::EMPTY)
      emptyindicators[j] = true;
    else {
      emptyindicators[j] = false;
      if (board.getNodeValue(j + 1) == playera.getPlayerlabel())
        strategyred.assignValue(babywatsons, j + 1,
                                playera.getWestToEastCondition());
    }
  }

  PriorityQueue<int, int> queue(board.getSizeOfVertices());
  vector<pair<int, int> > dummy(board.getSizeOfVertices());
  for (int j = 0; j < board.getSizeOfVertices(); j++)
    dummy[j] = make_pair((j + 1), 0);

  strategyred.countNeighbors(queue, emptyindicators, babywatsons, dummy);
  strategyred.countNeighbors(queue, emptyindicators, babywatsons, dummy);

  int move = strategyred.genNextFill(emptyindicators, queue, dummy);
  EXPECT_EQ(8, move);
  strategyred.assignValue(babywatsons, 8, playera.getWestToEastCondition());

  move = strategyred.genNextFill(emptyindicators, queue, dummy);
  EXPECT_EQ(17, move);
  strategyred.assignValue(babywatsons, 17, playera.getWestToEastCondition());

  move = strategyred.genNextFill(emptyindicators, queue, dummy);
  EXPECT_EQ(18, move);
  strategyred.assignValue(babywatsons, 18, playera.getWestToEastCondition());

  move = strategyred.genNextFill(emptyindicators, queue, dummy);
  EXPECT_EQ(19, move);
  strategyred.assignValue(babywatsons, 19, playera.getWestToEastCondition());

  move = strategyred.genNextFill(emptyindicators, queue, dummy);
  EXPECT_EQ(23, move);
  strategyred.assignValue(babywatsons, 23, playera.getWestToEastCondition());

  move = strategyred.genNextFill(emptyindicators, queue, dummy);
  EXPECT_EQ(5, move);
  strategyred.assignValue(babywatsons, 5, playera.getWestToEastCondition());

  move = strategyred.genNextFill(emptyindicators, queue, dummy);
  EXPECT_EQ(11, move);
  strategyred.assignValue(babywatsons, 11, playera.getWestToEastCondition());
}
TEST_F(StrategyTest,CheckGenNextFill) {
//fill up 5x5 board
  int numofhexgon = 5;
  float threshold = 0.1;
  int cutoff = threshold * (numofhexgon * numofhexgon);
  for (unsigned i = 0; i < 1000; i++) {
    HexBoard board(numofhexgon);
    Player playera(board, hexgonValKind::RED);  //north to south, babywatson, 'O'
    Player playerb(board, hexgonValKind::BLUE);  //west to east, virtual opponent, 'X'
    Strategy strategyred(&board, &playera);
    Game hexboardgame(board);

    bool* emptyindicators = new bool[board.getSizeOfVertices()];
    vector<set<int> > babywatsons(numofhexgon), opponents(numofhexgon);
    strategyred.initTransformVector(babywatsons);
    strategyred.initTransformVector(opponents);

    for (int j = 0; j < board.getSizeOfVertices(); j++) {
      //set the current empty location as true
      if (board.getNodeValue(j + 1) == hexgonValKind::EMPTY)
        emptyindicators[j] = true;
      else {
        emptyindicators[j] = false;
        if (board.getNodeValue(j + 1) == playera.getPlayerlabel())
          strategyred.assignValue(babywatsons, j + 1,
                                  playera.getWestToEastCondition());
        else
          strategyred.assignValue(opponents, j + 1,
                                  playerb.getWestToEastCondition());
      }
    }
    int portionofempty = count(emptyindicators,
                               emptyindicators + board.getSizeOfVertices(),
                               true);
    int winner = 0, nextmove = -1;
    while (portionofempty > cutoff) {
      //random generate a move for baby watson
      int move = strategyred.genNextRandom(emptyindicators, portionofempty);
      if (nextmove < 0)
        nextmove = move;
      strategyred.assignValue(babywatsons, move,
                              playera.getWestToEastCondition());
      portionofempty--;

      //random generate a move for virtual opponent
      if (portionofempty > 0) {
        move = strategyred.genNextRandom(emptyindicators, portionofempty);
        strategyred.assignValue(opponents, move,
                                playerb.getWestToEastCondition());
        portionofempty--;
      }
    }
    //fill up the rest of the empty cells
    PriorityQueue<int, int> queue(board.getSizeOfVertices());
    vector<pair<int, int> > dummy(board.getSizeOfVertices());
    for (int j = 0; j < board.getSizeOfVertices(); j++)
      dummy[j] = make_pair((j + 1), 0);

    strategyred.countNeighbors(queue, emptyindicators, babywatsons, dummy);

    while (portionofempty > 0) {
      //fill up the board by non-random
      int move = strategyred.genNextFill(emptyindicators, queue, dummy);
      strategyred.assignValue(babywatsons, move,
                              playera.getWestToEastCondition());
      portionofempty--;

      //fill up the board by non-random
      if (portionofempty > 0)
        move = strategyred.genNextFill(emptyindicators, queue, dummy);
      strategyred.assignValue(opponents, move,
                              playerb.getWestToEastCondition());
      portionofempty--;
    }
    winner = strategyred.checkWinnerExist(babywatsons, opponents);
    if (winner == 0) {
      //set move for babywatsons
      int cnt = 0;
      for (unsigned k = 0; k < babywatsons.size(); k++) {
        for (auto j : babywatsons[k]) {
          int moverow = k + 1;
          int movecol = j + 1;
          hexboardgame.setMove(playera, moverow, movecol);
          cnt++;
        }
      }
      cout << "babywatsons has moved " << cnt << endl;
      //set move for virtual opponent
      cnt = 0;
      for (unsigned k = 0; k < opponents.size(); k++) {
        for (auto j : opponents[k]) {
          int moverow = k + 1;
          int movecol = j + 1;
          hexboardgame.setMove(playerb, moverow, movecol);
        }
        cnt++;
      }
      cout << "virtual opponent has moved " << cnt << endl;
      string winnersname = hexboardgame.getWinner(playera, playerb);
      cout << "actual winner = " << winnersname << endl;
      cout << hexboardgame.showView(playera, playerb);
    }
    EXPECT_NE(0, winner);
  }
}
TEST_F(StrategyTest, CheckSimulationTest) {
  int numofhexgon = 5;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
  Strategy strategyred(&board, &playera);
  Strategy strategyblue(&board, &playerb);
  Game hexboardgame(board);
  string winner = "UNKNOWN";
  int round = 0;
  while (winner == "UNKNOWN") {
    //the virtual player moves
    int redmove, redrow, redcol;
    redmove = hexboardgame.genMove(strategyred);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playera, redrow, redcol);

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(strategyblue);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playerb, bluerow, bluecol);

    cout << round << " " << redmove << " " << bluemove << endl;
    round++;
    ASSERT_NE(redmove, bluemove);
    winner = hexboardgame.getWinner(playera, playerb);
  }
}
TEST_F(StrategyTest, CheckSimulationELEVENTest) {
  int numofhexgon = 11;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
  Strategy strategyred(&board, &playera);
  Strategy strategyblue(&board, &playerb);
  Game hexboardgame(board);
  string winner = "UNKNOWN";
  int round = 0;
  while (winner == "UNKNOWN") {
    //the virtual player moves
    int redmove, redrow, redcol;
    redmove = hexboardgame.genMove(strategyred);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playera, redrow, redcol);

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(strategyblue);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playerb, bluerow, bluecol);

    cout << round << " " << redmove << " " << bluemove << endl;
    round++;
    ASSERT_NE(redmove, bluemove);
    winner = hexboardgame.getWinner(playera, playerb);
  }
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();;
}
