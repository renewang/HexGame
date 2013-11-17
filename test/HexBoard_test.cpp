/*
 * HexBoard_test.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: renewang
 */

#include <strstream>

#include "Game.h"
#include "Player.h"
#include "HexBoard.h"
#include "MinSpanTreeAlgo.h"

#include "gtest/gtest.h"

using namespace std;
class HexBoardTest : public ::testing::Test {
  virtual void SetUp() {
  }
  ;
  virtual void TearDown() {
  }
  ;
};
TEST_F(HexBoardTest,HexBoardInitialization) {
  //test 3x3 Hexboard
  HexBoard board(3);
  //test northwest and southeast corner (2 edges)
  EXPECT_TRUE(board.isAdjacent(1, 2));
  EXPECT_FALSE(board.isAdjacent(1, 3));
  int expect1[2] = { 2, 4 };
  vector<int> expectNeigh(expect1, expect1 + 2);
  vector<int> actualNeigh = board.getNeighbors(1);  //northwest
  EXPECT_EQ(expectNeigh, actualNeigh);

  expectNeigh.clear();
  EXPECT_TRUE(board.isAdjacent(9, 8));
  EXPECT_FALSE(board.isAdjacent(9, 7));
  int expect2[2] = { 6, 8 };
  expectNeigh.insert(expectNeigh.begin(), expect2, expect2 + 2);
  actualNeigh = board.getNeighbors(9);  //southwest
  sort(actualNeigh.begin(), actualNeigh.end());
  EXPECT_EQ(expectNeigh, actualNeigh);

  //test northeast and southwest corner (3 edges)
  expectNeigh.clear();
  EXPECT_TRUE(board.isAdjacent(3, 2));
  EXPECT_FALSE(board.isAdjacent(3, 4));
  int expect3[3] = { 2, 5, 6 };
  expectNeigh.insert(expectNeigh.begin(), expect3, expect3 + 3);
  actualNeigh = board.getNeighbors(3);  //northeast
  sort(actualNeigh.begin(), actualNeigh.end());
  EXPECT_EQ(expectNeigh, actualNeigh);

  expectNeigh.clear();
  EXPECT_TRUE(board.isAdjacent(7, 8));
  EXPECT_FALSE(board.isAdjacent(7, 9));
  int expect4[3] = { 4, 5, 8 };
  expectNeigh.insert(expectNeigh.begin(), expect4, expect4 + 3);
  actualNeigh = board.getNeighbors(7);  //northeast
  sort(actualNeigh.begin(), actualNeigh.end());
  EXPECT_EQ(expectNeigh, actualNeigh);

  //test nodes on boundary but not at 4 corners
  expectNeigh.clear();
  EXPECT_TRUE(board.isAdjacent(2, 4));
  EXPECT_FALSE(board.isAdjacent(2, 7));
  int expect5[4] = { 1, 3, 4, 5 };
  expectNeigh.insert(expectNeigh.begin(), expect5, expect5 + 4);
  actualNeigh = board.getNeighbors(2);  //north side
  sort(actualNeigh.begin(), actualNeigh.end());
  EXPECT_EQ(expectNeigh, actualNeigh);

  expectNeigh.clear();
  EXPECT_TRUE(board.isAdjacent(4, 5));
  EXPECT_FALSE(board.isAdjacent(4, 9));
  int expect6[4] = { 1, 2, 5, 7 };
  expectNeigh.insert(expectNeigh.begin(), expect6, expect6 + 4);
  actualNeigh = board.getNeighbors(4);  //west side
  sort(actualNeigh.begin(), actualNeigh.end());
  EXPECT_EQ(expectNeigh, actualNeigh);

  expectNeigh.clear();
  EXPECT_TRUE(board.isAdjacent(6, 9));
  EXPECT_FALSE(board.isAdjacent(6, 1));
  int expect7[4] = { 3, 5, 8, 9 };
  expectNeigh.insert(expectNeigh.begin(), expect7, expect7 + 4);
  actualNeigh = board.getNeighbors(6);  //east side
  sort(actualNeigh.begin(), actualNeigh.end());
  EXPECT_EQ(expectNeigh, actualNeigh);

  expectNeigh.clear();
  EXPECT_TRUE(board.isAdjacent(8, 7));
  EXPECT_FALSE(board.isAdjacent(8, 2));
  int expect8[4] = { 5, 6, 7, 9 };
  expectNeigh.insert(expectNeigh.begin(), expect8, expect8 + 4);
  actualNeigh = board.getNeighbors(8);  //south side
  sort(actualNeigh.begin(), actualNeigh.end());
  EXPECT_EQ(expectNeigh, actualNeigh);

  //test internal nodes, 6 edges
  expectNeigh.clear();
  EXPECT_TRUE(board.isAdjacent(5, 2));
  EXPECT_FALSE(board.isAdjacent(5, 9));
  int expect9[6] = { 2, 3, 4, 6, 7, 8 };
  expectNeigh.insert(expectNeigh.begin(), expect9, expect9 + 6);
  actualNeigh = board.getNeighbors(5);
  sort(actualNeigh.begin(), actualNeigh.end());
  EXPECT_EQ(expectNeigh, actualNeigh);

  //check the edges
  EXPECT_EQ(16, board.getSizeOfEdges());
  stringstream strout;
  strout << board.getNodeValue(1);
  EXPECT_EQ("EMPTY", strout.str());

  Game hexboardgame(board);
  Player playera(board, hexgonValKind::BLUE);
  Player playerb(board, hexgonValKind::RED);
  ASSERT_TRUE(hexboardgame.setMove(playera, 1, 3));
  stringstream strout2;
  strout2 << board.getNodeValue(3);
  ASSERT_EQ("BLUE", strout2.str());
  ASSERT_TRUE(hexboardgame.setMove(playerb, 3, 2));
  stringstream strout3;
  strout3 << board.getNodeValue(8);
  ASSERT_EQ("RED", strout3.str());
}
TEST_F(HexBoardTest,HexBoardSetMove) {
  //test 5x5 Hexboard
  HexBoard board(5);
  Game hexboardgame(board);
  Player playera(board, hexgonValKind::RED);

  //test with private set setPlayerBoard method and corresponding MST tree
  ASSERT_TRUE(hexboardgame.setMove(playera, 1, 1));
  HexBoard playerasboard = playera.getPlayersboard();
  EXPECT_EQ(0, playerasboard.getSizeOfEdges());
  ASSERT_TRUE(hexboardgame.setMove(playera, 2, 1));
  playerasboard = playera.getPlayersboard();
  EXPECT_EQ(1, playerasboard.getSizeOfEdges());
  ASSERT_TRUE(hexboardgame.setMove(playera, 3, 1));
  playerasboard = playera.getPlayersboard();
  EXPECT_EQ(2, playerasboard.getSizeOfEdges());
  ASSERT_TRUE(hexboardgame.setMove(playera, 4, 1));
  playerasboard = playera.getPlayersboard();
  EXPECT_EQ(3, playerasboard.getSizeOfEdges());
  ASSERT_TRUE(hexboardgame.setMove(playera, 5, 1));
  playerasboard = playera.getPlayersboard();
  EXPECT_EQ(4, playerasboard.getSizeOfEdges());

  MinSpanTreeAlgo<hexgonValKind, int> mstalgo(playerasboard);
  mstalgo.calculate();

  Graph<hexgonValKind, int> msttree = mstalgo.getMsttree();
  EXPECT_EQ(4, msttree.getSizeOfEdges());
  vector<vector<int> > subgraphs = msttree.getAllSubGraphs();
  EXPECT_EQ(1, subgraphs.size());

  Player playerb(board, hexgonValKind::BLUE);
  ASSERT_TRUE(hexboardgame.setMove(playerb, 1, 2));
  HexBoard playerbsboard = playerb.getPlayersboard();
  EXPECT_EQ(0, playerbsboard.getSizeOfEdges());
  ASSERT_TRUE(hexboardgame.setMove(playerb, 2, 2));
  playerbsboard = playerb.getPlayersboard();
  EXPECT_EQ(1, playerbsboard.getSizeOfEdges());
  ASSERT_TRUE(hexboardgame.setMove(playerb, 3, 2));
  playerbsboard = playerb.getPlayersboard();
  EXPECT_EQ(2, playerbsboard.getSizeOfEdges());
  ASSERT_TRUE(hexboardgame.setMove(playerb, 4, 2));
  playerbsboard = playerb.getPlayersboard();
  EXPECT_EQ(3, playerbsboard.getSizeOfEdges());
  ASSERT_TRUE(hexboardgame.setMove(playerb, 2, 5));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 3, 5));
  playerbsboard = playerb.getPlayersboard();
  EXPECT_EQ(4, playerbsboard.getSizeOfEdges());

  MinSpanTreeAlgo<hexgonValKind, int> mstalgob(playerbsboard);
  mstalgob.calculate();
  Graph<hexgonValKind, int> msttreeb = mstalgob.getMsttree();
  EXPECT_EQ(4, msttreeb.getSizeOfEdges());
  vector<vector<int> > subgraphsb = msttreeb.getAllSubGraphs();
  EXPECT_EQ(2, subgraphsb.size());
}
TEST_F(HexBoardTest,HexBoardWinningTest) {
  //test 5x5 Hexboard
  HexBoard board(5);
  Game hexboardgame(board);
  Player playera(board, hexgonValKind::RED);
  Player playerb(board, hexgonValKind::BLUE);

  //generate a sequence of paths
  ASSERT_TRUE(hexboardgame.setMove(playera, 1, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 1, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 2, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 2, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 3, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 3, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 4, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 4, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 5, 1));
  EXPECT_EQ("RED", hexboardgame.getWinner(playera, playerb));
}
TEST_F(HexBoardTest,HexBoardWinningTest2) {
  //test 5x5 Hexboard
  HexBoard board(5);
  Game hexboardgame(board);
  Player playera(board, hexgonValKind::RED);
  Player playerb(board, hexgonValKind::BLUE);

  //generate a sequence of paths
  ASSERT_TRUE(hexboardgame.setMove(playera, 1, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 1, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 2, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 2, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 3, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 3, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 4, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 4, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 5, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 5, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 1, 3));
  EXPECT_EQ("RED", hexboardgame.getWinner(playera, playerb));
}
TEST_F(HexBoardTest,HexBoardWinningTest3) {
  //test 5x5 Hexboard
  HexBoard board(5);
  Game hexboardgame(board);
  Player playera(board, hexgonValKind::RED);
  Player playerb(board, hexgonValKind::BLUE);

  //generate a sequence of paths
  ASSERT_TRUE(hexboardgame.setMove(playera, 1, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 1, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 2, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 2, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 3, 3));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 3, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 4, 4));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 4, 2));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 5, 5));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 3, 1));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 1, 4));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 4, 3));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 1, 3));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 3, 4));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playera, 2, 3));
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  ASSERT_TRUE(hexboardgame.setMove(playerb, 2, 5));
  EXPECT_EQ("BLUE", hexboardgame.getWinner(playera, playerb));
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
