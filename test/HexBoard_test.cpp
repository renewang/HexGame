/*
 * HexBoard_test.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: renewang
 */

#include <sstream>

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
  Player playera(board, hexgonValKind_BLUE);
  Player playerb(board, hexgonValKind_RED);
  ASSERT_TRUE(hexboardgame.setMove(playera, 1, 3));
  stringstream strout2;
  strout2 << board.getNodeValue(3);
  EXPECT_EQ("BLUE", strout2.str());
  ASSERT_TRUE(hexboardgame.setMove(playerb, 3, 2));
  stringstream strout3;
  strout3 << board.getNodeValue(8);
  EXPECT_EQ("RED", strout3.str());
}
TEST_F(HexBoardTest,HexBoardSetMove) {
  //test 5x5 Hexboard
  HexBoard board(5);
  Game hexboardgame(board);
  Player playera(board, hexgonValKind_RED);

  //test with private set setPlayerBoard method and corresponding MST tree
  ASSERT_TRUE(hexboardgame.setMove(playera, 1, 1));
  HexBoard playerasboard = playera.getPlayersboard();
  EXPECT_EQ(0, playerasboard.getSizeOfEdges());
  EXPECT_EQ(board.getNumofemptyhexgons(), board.getSizeOfVertices() - 1);
  ASSERT_TRUE(hexboardgame.setMove(playera, 2, 1));
  playerasboard = playera.getPlayersboard();
  EXPECT_EQ(1, playerasboard.getSizeOfEdges());
  EXPECT_EQ(board.getNumofemptyhexgons(), board.getSizeOfVertices() - 2);
  ASSERT_TRUE(hexboardgame.setMove(playera, 3, 1));
  playerasboard = playera.getPlayersboard();
  EXPECT_EQ(2, playerasboard.getSizeOfEdges());
  EXPECT_EQ(board.getNumofemptyhexgons(), board.getSizeOfVertices() - 3);
  ASSERT_TRUE(hexboardgame.setMove(playera, 4, 1));
  playerasboard = playera.getPlayersboard();
  EXPECT_EQ(3, playerasboard.getSizeOfEdges());
  EXPECT_EQ(board.getNumofemptyhexgons(), board.getSizeOfVertices() - 4);
  ASSERT_TRUE(hexboardgame.setMove(playera, 5, 1));
  playerasboard = playera.getPlayersboard();
  EXPECT_EQ(4, playerasboard.getSizeOfEdges());
  EXPECT_EQ(board.getNumofemptyhexgons(), board.getSizeOfVertices() - 5);

  MinSpanTreeAlgo<hexgonValKind, int> mstalgo(playerasboard);
  MinSpanTreeAlgo<hexgonValKind, int>::UnionFind unionfind(mstalgo);
  mstalgo.calculate(unionfind);

  Graph<hexgonValKind, int> msttree = mstalgo.getMsttree();
  EXPECT_EQ(4, msttree.getSizeOfEdges());
  vector<vector<int> > subgraphs = msttree.getAllSubGraphs();
  EXPECT_EQ(1, subgraphs.size());

  Player playerb(board, hexgonValKind_BLUE);
  ASSERT_TRUE(hexboardgame.setMove(playerb, 1, 2));
  HexBoard playerbsboard = playerb.getPlayersboard();
  EXPECT_EQ(0, playerbsboard.getSizeOfEdges());
  EXPECT_EQ(board.getNumofemptyhexgons(), board.getSizeOfVertices() - 6);
  ASSERT_TRUE(hexboardgame.setMove(playerb, 2, 2));
  playerbsboard = playerb.getPlayersboard();
  EXPECT_EQ(1, playerbsboard.getSizeOfEdges());
  EXPECT_EQ(board.getNumofemptyhexgons(), board.getSizeOfVertices() - 7);
  ASSERT_TRUE(hexboardgame.setMove(playerb, 3, 2));
  playerbsboard = playerb.getPlayersboard();
  EXPECT_EQ(2, playerbsboard.getSizeOfEdges());
  EXPECT_EQ(board.getNumofemptyhexgons(), board.getSizeOfVertices() - 8);
  ASSERT_TRUE(hexboardgame.setMove(playerb, 4, 2));
  playerbsboard = playerb.getPlayersboard();
  EXPECT_EQ(3, playerbsboard.getSizeOfEdges());
  EXPECT_EQ(board.getNumofemptyhexgons(), board.getSizeOfVertices() - 9);
  ASSERT_TRUE(hexboardgame.setMove(playerb, 2, 5));
  EXPECT_EQ(board.getNumofemptyhexgons(), board.getSizeOfVertices() - 10);
  ASSERT_TRUE(hexboardgame.setMove(playerb, 3, 5));
  EXPECT_EQ(board.getNumofemptyhexgons(), board.getSizeOfVertices() - 11);
  playerbsboard = playerb.getPlayersboard();
  EXPECT_EQ(4, playerbsboard.getSizeOfEdges());

  MinSpanTreeAlgo<hexgonValKind, int> mstalgob(playerbsboard);
  MinSpanTreeAlgo<hexgonValKind, int>::UnionFind unionfindb(mstalgob);
  mstalgo.calculate(unionfindb);
  Graph<hexgonValKind, int> msttreeb = mstalgob.getMsttree();
  EXPECT_EQ(4, msttreeb.getSizeOfEdges());
  vector<vector<int> > subgraphsb = msttreeb.getAllSubGraphs();
  EXPECT_EQ(2, subgraphsb.size());
}
TEST_F(HexBoardTest,HexBoardBasicFuncs) {
  //2. test setNodeValue
  //3. test setNumofhexgons
  //4. test resetHexBoard();
  int numofhexgon = 5;
  int numofedges = (numofhexgon * numofhexgon * 2 + 6);

  HexBoard board(numofhexgon);
  HexBoard playersboard;
  playersboard.setNumofhexgons(numofhexgon);

  ASSERT_EQ(board.getSizeOfVertices(), numofhexgon * numofhexgon);
  ASSERT_EQ(board.getSizeOfEdges(), numofedges);
  ASSERT_EQ(playersboard.getSizeOfVertices(), numofhexgon * numofhexgon);
  ASSERT_EQ(playersboard.getSizeOfEdges(), 0);
  int k = 0;
  for (int i = 1; i <= numofhexgon * numofhexgon; ++i) {
    hexgonValKind playerkind;
    if (i % 2 == 0)
      playerkind = hexgonValKind_RED;
      else
      playerkind = hexgonValKind_BLUE;

    ASSERT_EQ(hexgonValKind_EMPTY, board.getNodeValue(i));
    ASSERT_EQ(hexgonValKind_EMPTY, playersboard.getNodeValue(i));
    playersboard.setEdgeValue(i);
    vector<int> neighbors = playersboard.getNeighbors(i);
    if (i % 2 == 0) {
      board.setNodeValue(i, playerkind);
      playersboard.setNodeValue(i, playerkind);
      EXPECT_EQ(playerkind, board.getNodeValue(i));
      EXPECT_EQ(playerkind, playersboard.getNodeValue(i));
    } else {
      board.setNodeValue(i, playerkind);
      EXPECT_EQ(playerkind, board.getNodeValue(i));
      EXPECT_EQ(hexgonValKind_EMPTY, playersboard.getNodeValue(i));
    }
#if __cplusplus > 199711L
    for (auto j : neighbors) {
#else
      for (vector<int>::iterator iter = neighbors.begin(); iter != neighbors.end(); ++iter) {
        int j = *iter;
#endif
      if (playersboard.getNodeValue(j) != playerkind)
        playersboard.deleteEdge(i, j);
    }
    if (i % 6 == 0 || i == 8 || i == 14 || i == 16 || i == 22)
      k++;
    EXPECT_EQ(playersboard.getSizeOfEdges(), k);
  }
  EXPECT_EQ(board.getNumofemptyhexgons(), 0);
  EXPECT_EQ(playersboard.getNumofemptyhexgons(), 13);
  EXPECT_EQ(playersboard.getSizeOfEdges(), 8);
  EXPECT_EQ(board.getSizeOfVertices(), numofhexgon * numofhexgon);
  EXPECT_EQ(playersboard.getSizeOfVertices(), numofhexgon * numofhexgon);
  int connectnode[8] = { 2, 4, 8, 10, 12, 14, 18, 20 };
  for (unsigned i = 0; i < 8; i++){
    EXPECT_FALSE(playersboard.isAdjacent(connectnode[i], (connectnode[i] + 3)));
    EXPECT_TRUE(playersboard.isAdjacent(connectnode[i], (connectnode[i] + 4)));
    EXPECT_FALSE(playersboard.isAdjacent(connectnode[i], (connectnode[i] + 5)));
  }

  board.resetHexBoard(false);
  EXPECT_EQ(board.getSizeOfVertices(), board.getNumofemptyhexgons());
  EXPECT_EQ(board.getSizeOfEdges(), numofedges);
  EXPECT_EQ(8, playersboard.getSizeOfEdges());
  playersboard.resetHexBoard();
  playersboard.setNumofhexgons(numofhexgon);
  EXPECT_EQ(numofhexgon, playersboard.getNumofhexgons());
  EXPECT_EQ(numofhexgon * numofhexgon, playersboard.getSizeOfVertices());
  EXPECT_EQ(playersboard.getSizeOfVertices(),
            playersboard.getNumofemptyhexgons());
  EXPECT_EQ(0, playersboard.getSizeOfEdges());
  for (unsigned i = 0; i < 8; i++){
    EXPECT_FALSE(playersboard.isAdjacent(connectnode[i], (connectnode[i] + 3)));
    EXPECT_FALSE(playersboard.isAdjacent(connectnode[i], (connectnode[i] + 4)));
    EXPECT_FALSE(playersboard.isAdjacent(connectnode[i], (connectnode[i] + 5)));
  }
  for (int i = 1; i <= numofhexgon * numofhexgon; ++i) {
    EXPECT_EQ(hexgonValKind_EMPTY, board.getNodeValue(i));
    EXPECT_EQ(hexgonValKind_EMPTY, playersboard.getNodeValue(i));
  }
  playersboard.resetHexBoard();
  playersboard.setNumofhexgons(3);
  EXPECT_EQ(playersboard.getSizeOfVertices(), 9);

  for (int i = 1; i <= 9; ++i)
    EXPECT_EQ(hexgonValKind_EMPTY, playersboard.getNodeValue(i));
}
TEST_F(HexBoardTest,HexBoardWinningTest) {
  //test 5x5 Hexboard
  HexBoard board(5);
  Game hexboardgame(board);

  Player playera(board, hexgonValKind_RED);
  Player playerb(board, hexgonValKind_BLUE);
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
  hexboardgame.resetGame(playera, playerb);
  EXPECT_EQ("UNKNOWN", hexboardgame.getWinner(playera, playerb));
  hexboardgame.showView(playera, playerb);
}
TEST_F(HexBoardTest,HexBoardWinningTest2) {
  //test 5x5 Hexboard
  HexBoard board(5);
  Game hexboardgame(board);

  Player playera(board, hexgonValKind_RED);
  Player playerb(board, hexgonValKind_BLUE);
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

  Player playera(board, hexgonValKind_RED);
  Player playerb(board, hexgonValKind_BLUE);
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
