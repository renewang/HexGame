/*
 * Strategy_test.cpp
 *
 *  Created on: Nov 20, 2013
 *      Author: renewang
 */

#include <vector>
#include <utility>
#include <algorithm>

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
  vector<int> testmove1(numofhexgon);
  for (int i = 0; i < numofhexgon; i++)
    testmove1[i] = (i + 1);

  EXPECT_FALSE(
      strategyred.isWinner(testmove1, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove1, playerb.getWestToEastCondition()));  //west to east

  //straight move from north to south
  vector<int> testmove2(numofhexgon);
  for (int i = 3; i <= 3 + (numofhexgon * (numofhexgon - 1));
      i = i + numofhexgon)
    testmove2[i / numofhexgon] = i;

  EXPECT_TRUE(
      strategyred.isWinner(testmove2, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove2, playerb.getWestToEastCondition()));  //west to east

  //straight move along anti-diagonal
  vector<int> testmove3(numofhexgon);
  for (int i = 0; i < numofhexgon; i++)
    testmove3[i] = i * numofhexgon + (numofhexgon - i);

  EXPECT_TRUE(
      strategyred.isWinner(testmove3, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove3, playerb.getWestToEastCondition()));  //west to east

  //negative test case, move diagonal
  vector<int> testmove4(numofhexgon);
  for (int i = 0; i < numofhexgon; i++)
    testmove4[i] = i * numofhexgon + (i + 1);

  EXPECT_FALSE(
      strategyred.isWinner(testmove4, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove4, playerb.getWestToEastCondition()));  //west to east

  //move west to east firstly and then north to south (zigzag move)
  //1->2->7->8->13->14->19->20
  vector<int> testmove5(8);
  for (int i = 1; i <= 19; i = i + 6) {
    testmove5[i / 3] = i;
    testmove5[i / 3 + 1] = i + 1;
  }
  EXPECT_FALSE(
      strategyred.isWinner(testmove5, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove5, playerb.getWestToEastCondition()));  //west to east

  //move north to south firstly and then west to east (zigzag move)
  //1->6->7->12->13->18->19->24
  vector<int> testmove6(8);
  for (int i = 1; i <= 19; i = i + 6) {
    testmove6[i / 3] = i;
    testmove6[i / 3 + 1] = i + 5;
  }

  EXPECT_TRUE(
      strategyred.isWinner(testmove6, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove6, playerb.getWestToEastCondition()));  //west to east

  //negative test case, move west to east then diagonal
  vector<int> testmove7(numofhexgon);
  testmove7[0] = 1;
  for (int i = 2; i < 3 * numofhexgon; i = i + 6)
    testmove7[i / 6 + 1] = i;
  testmove7[numofhexgon - 1] = 15;

  EXPECT_FALSE(
      strategyred.isWinner(testmove7, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove7, playerb.getWestToEastCondition()));  //west to east

  //negative test case, move diagonal and then north to south
  vector<int> testmove8(numofhexgon + 2);
  testmove8[0] = 1;
  testmove8[1] = 2;
  testmove8[2] = 3;
  testmove8[3] = 8;
  testmove8[4] = 14;
  testmove8[5] = 19;
  testmove8[6] = 24;

  EXPECT_FALSE(
      strategyred.isWinner(testmove8, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove8, playerb.getWestToEastCondition()));  //west to east

  //purely random
  vector<int> testmove9(numofhexgon);
  int randommove[5] = { 1, 10, 8, 12, 19 };
  copy(randommove, randommove + numofhexgon, testmove9.begin());

  EXPECT_FALSE(
      strategyred.isWinner(testmove9, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove9, playerb.getWestToEastCondition()));  //west to east

  //random test set 2
  vector<int> testmove10(11);
  int randommove2[11] = { 1, 2, 3, 4, 8, 9, 14, 15, 18, 19, 21 };
  copy(randommove2, randommove2 + 11, testmove10.begin());

  EXPECT_FALSE(
      strategyred.isWinner(testmove10, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove10, playerb.getWestToEastCondition()));  //west to east

  //random test set 3
  vector<int> testmove11(12);
  int randommove3[12] = { 2, 17, 13, 6, 15, 24, 4, 9, 22, 19, 7, 8 };
  copy(randommove3, randommove3 + 12, testmove11.begin());

  EXPECT_TRUE(
      strategyred.isWinner(testmove11, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove11, playerb.getWestToEastCondition()));  //west to east

  //additional test
  vector<int> testmove12(5);
  int randommove4[12] = { 5, 10, 15, 19, 24 };
  copy(randommove4, randommove4 + 5, testmove12.begin());

  EXPECT_TRUE(
      strategyred.isWinner(testmove12, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove12, playerb.getWestToEastCondition()));  //west to east

  //additional test
  vector<int> testmove13(13);
  for (unsigned i = 0; i < 7; i++)
    testmove13[i] = i + 12;

  testmove13[7] = 1;
  testmove13[8] = 3;
  testmove13[9] = 4;
  testmove13[10] = 7;
  testmove13[11] = 8;
  testmove13[12] = 10;

  EXPECT_FALSE(
      strategyred.isWinner(testmove13, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove13, playerb.getWestToEastCondition()));  //west to east

  //test reverse for column
  vector<int> testmove14(6);
  for (unsigned i = 0; i < 3; i++)
    testmove14[i] = i + 16;
  testmove14[3] = 14;
  testmove14[4] = 9;
  testmove14[5] = 5;

  EXPECT_FALSE(
      strategyred.isWinner(testmove14, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove14, playerb.getWestToEastCondition()));  //west to east

  //test reverse path
  vector<int> testmove15(12);
  int randommove5[12] = { 1, 6, 11, 16, 17, 13, 8, 9, 10, 15, 20, 25 };
  copy(randommove5, randommove5 + 12, testmove15.begin());

  EXPECT_TRUE(
      strategyred.isWinner(testmove15, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove15, playerb.getWestToEastCondition()));  //west to east

  //test reverse path
  vector<int> testmove17(9);
  int randommove7[9] = { 2, 7, 9, 10, 12, 13, 15, 19, 24 };
  copy(randommove7, randommove7 + 9, testmove17.begin());

  EXPECT_TRUE(
      strategyred.isWinner(testmove17, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove17, playerb.getWestToEastCondition()));  //west to east

  vector<int> testmove18(18);
  int randommove8[18] = { 9, 20, 19, 8, 22, 24, 21, 23, 11, 12, 7, 1, 15, 5, 3,
      25, 2, 18 };
  copy(randommove8, randommove8 + 18, testmove18.begin());

  EXPECT_FALSE(
      strategyred.isWinner(testmove18, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove18, playerb.getWestToEastCondition()));  //west to east

  vector<int> testmove19(11);
  int randommove9[11] = { 6, 24, 5, 22, 13, 18, 16, 2, 1, 8, 12 };
  copy(randommove9, randommove9 + 11, testmove19.begin());

  EXPECT_FALSE(
      strategyred.isWinner(testmove19, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove19, playerb.getWestToEastCondition()));  //west to east

  vector<int> testmove20(9);
  int randommove10[9] = { 11, 7, 14, 17, 13, 5, 21, 12, 18 };
  copy(randommove10, randommove10 + 9, testmove20.begin());

  EXPECT_FALSE(
      strategyred.isWinner(testmove20, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove20, playerb.getWestToEastCondition()));  //west to east

  //potential false positive problem

  vector<int> testmove21(8);
  int randommove11[8] = { 1, 6, 7, 13, 14, 17, 18, 21 };
  copy(randommove11, randommove11 + 8, testmove21.begin());

  EXPECT_FALSE(
      strategyred.isWinner(testmove21, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove21, playerb.getWestToEastCondition()));  //west to east

  vector<int> testmove22(12);
  int randommove12[12] = { 16, 15, 8, 22, 9, 2, 23, 24, 7, 5, 10, 21 };
  copy(randommove12, randommove12 + 12, testmove22.begin());
  EXPECT_FALSE(
      strategyred.isWinner(testmove22, playera.getWestToEastCondition()));  //north to south
  EXPECT_FALSE(
      strategyblue.isWinner(testmove22, playerb.getWestToEastCondition()));  //west to east

  vector<int> testmove23(12);
  int randommove13[12] = { 18, 6, 13, 12, 1, 20, 4, 17, 11, 19, 14, 3 };
  copy(randommove13, randommove13 + 12, testmove23.begin());
  EXPECT_FALSE(
      strategyred.isWinner(testmove23, playera.getWestToEastCondition()));  //north to south
  EXPECT_TRUE(
      strategyblue.isWinner(testmove23, playerb.getWestToEastCondition()));  //west to east

}
TEST_F(StrategyTest,CheckWinnerTestTwo) {
  int numofhexgon = 5;

//purely random to compare the result of MST and approximate approach to find a winner for north to south
  for (unsigned i = 0; i < 3000; i++) {
    HexBoard board(numofhexgon);
    Player playera(board, hexgonValKind::RED);  //north to south
    Player playerb(board, hexgonValKind::BLUE);  //west to east
    Strategy strategyred(&board, &playera);
    vector<int> test;
    bool winner = false;
    shared_ptr<bool>& emptyindicators = const_cast<shared_ptr<bool>&>(board.getEmptyHexIndicators());
    Game hexboardgame(board);
    while (!winner) {
      unsigned portionofempty = board.getNumofemptyhexgons();
      int move = strategyred.genNextRandom(emptyindicators, portionofempty);
      test.push_back(move);
      int row = (move - 1) / numofhexgon + 1;
      int col = (move - 1) % numofhexgon + 1;
      hexboardgame.setMove(playera, row, col);
      winner = strategyred.isWinner(test, playera.getWestToEastCondition());
    }
    string winnerwho = hexboardgame.getWinner(playera, playerb);
    string playername = playera.getPlayername();
    if (playername != winnerwho) {
      for (unsigned k = 0; k < test.size(); k++)
        cout << test[i] << " ";
      cout << endl;
      cout << hexboardgame.showView(playera, playerb);
    }
    EXPECT_EQ(winnerwho, playername);
  }
//purely random to compare the result of MST and approximate approach to find a winner for west to east
  for (unsigned i = 0; i < 3000; i++) {
    HexBoard board(numofhexgon);
    Game hexboardgame(board);
    Player playera(board, hexgonValKind::RED);  //north to south
    Player playerb(board, hexgonValKind::BLUE);  //west to east
    Strategy strategyblue(&board, &playerb);
    vector<int> test;
    bool winner = false;
    shared_ptr<bool>& emptyindicators = const_cast<shared_ptr<bool>&>(board.getEmptyHexIndicators());
    while (!winner) {
      unsigned portionofempty = board.getNumofemptyhexgons();
      int move = strategyblue.genNextRandom(emptyindicators, portionofempty);
      test.push_back(move);
      int row = (move - 1) / numofhexgon + 1;
      int col = (move - 1) % numofhexgon + 1;
      hexboardgame.setMove(playerb, row, col);
      winner = strategyblue.isWinner(test, playerb.getWestToEastCondition());
    }
    string winnerwho = hexboardgame.getWinner(playera, playerb);
    string playername = playerb.getPlayername();
    if (playername != winnerwho) {
      for (unsigned k = 0; k < test.size(); k++)
        cout << test[k] << " ";
      cout << endl;
      cout << hexboardgame.showView(playera, playerb);
    }
    EXPECT_EQ(winnerwho, playername);
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
    vector<int> test;
    bool winner = false;
    shared_ptr<bool>& emptyindicators = const_cast<shared_ptr<bool>&>(board.getEmptyHexIndicators());
    while (!winner) {
      unsigned portionofempty = (unsigned) board.getNumofemptyhexgons();
      int move = strategyred.genNextRandom(emptyindicators, portionofempty);
      test.push_back(move);
      int row = (move - 1) / numofhexgon + 1;
      int col = (move - 1) % numofhexgon + 1;
      hexboardgame.setMove(playera, row, col);
      winner = strategyred.isWinner(test, playera.getWestToEastCondition());
    }
    string winnerwho = hexboardgame.getWinner(playera, playerb);
    string playername = playera.getPlayername();
    if (playername != winnerwho) {
      for (unsigned k = 0; k < test.size(); k++)
        cout << test[k] << " ";
      cout << endl;
      cout << hexboardgame.showView(playera, playerb);
    }
    EXPECT_EQ(winnerwho, playername);
  }
  for (unsigned i = 0; i < 3000; i++) {
    HexBoard board(numofhexgon);
    Player playera(board, hexgonValKind::RED);  //north to south
    Player playerb(board, hexgonValKind::BLUE);  //west to east
    Strategy strategyblue(&board, &playerb);
    Game hexboardgame(board);
    vector<int> test;
    bool winner = false;
    shared_ptr<bool>& emptyindicators = const_cast<shared_ptr<bool>&>(board.getEmptyHexIndicators());

    while (!winner) {
      unsigned portionofempty = (unsigned) board.getNumofemptyhexgons();
      int move = strategyblue.genNextRandom(emptyindicators, portionofempty);
      test.push_back(move);
      int row = (move - 1) / numofhexgon + 1;
      int col = (move - 1) % numofhexgon + 1;
      hexboardgame.setMove(playerb, row, col);
      winner = strategyblue.isWinner(test, playerb.getWestToEastCondition());
    }
    string winnerwho = hexboardgame.getWinner(playera, playerb);
    string playername = playerb.getPlayername();
    if (playername != winnerwho) {
      for (unsigned k = 0; k < test.size(); k++)
        cout << test[k] << " ";
      cout << endl;
      cout << hexboardgame.showView(playera, playerb);
    }
    EXPECT_EQ(winnerwho, playername);
  }
}
TEST_F(StrategyTest,CheckGenNextFillBasic) {
  int numofhexgon = 5;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);   //north to south, babywatson, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
  Strategy strategyred(&board, &playera, 1.0, 0.0);
  Game hexboardgame(board);
  shared_ptr<bool>& emptyindicators = const_cast<shared_ptr<bool>&>(board.getEmptyHexIndicators());
  vector<int>& babywatsons = const_cast<vector<int>&>(board.getRedmoves());

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

  cout << hexboardgame.showView(playera, playerb);

  int currentempty = board.getNumofemptyhexgons();

  PriorityQueue<int, int> queue(board.getSizeOfVertices());
  vector<pair<int, int> > counter(currentempty);

  unordered_set<int> allmoves(babywatsons.begin(), babywatsons.end());

  strategyred.countNeighbors(emptyindicators, allmoves, counter);
  strategyred.assignRandomNeighbors(queue, counter, currentempty);

  int move = strategyred.genNextFill(emptyindicators, queue);
  EXPECT_EQ(8, move);

  move = strategyred.genNextFill(emptyindicators, queue);
  EXPECT_EQ(17, move);

  move = strategyred.genNextFill(emptyindicators, queue);
  EXPECT_EQ(5, move);

  move = strategyred.genNextFill(emptyindicators, queue);
  EXPECT_EQ(11, move);

  move = strategyred.genNextFill(emptyindicators, queue);
  EXPECT_EQ(18, move);

  move = strategyred.genNextFill(emptyindicators, queue);
  EXPECT_EQ(19, move);

  move = strategyred.genNextFill(emptyindicators, queue);
  EXPECT_EQ(2, move);
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

    cout << "simulation " << round << " : " << redmove << " " << bluemove
         << endl;
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

    cout << "simulation " << round << " : " << redmove << " " << bluemove
         << endl;
    round++;
    ASSERT_NE(redmove, bluemove);
    winner = hexboardgame.getWinner(playera, playerb);
  }
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();;
}
