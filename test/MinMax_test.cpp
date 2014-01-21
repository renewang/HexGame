/*
 * MinMax_test.cpp
 *
 */
#include <bitset>
#include <limits>
#include <cstdlib>
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
  GameTree tree('B');
  tree.expandNode(0, 1, 'B');
  tree.expandNode(0, 2, 'B');
  tree.backpropagatefromSimulation(1, tree.updateNodefromSimulation(1, 0));

  EXPECT_EQ(tree.getNodeValueFeature(1, 0), tree.getNodeValueFeature(0, 0));
  tree.backpropagatefromSimulation(2, tree.updateNodefromSimulation(2, 0));

  EXPECT_EQ(1, tree.getNodeValueFeature(1, 0));
  EXPECT_EQ(1, tree.getNodeValueFeature(2, 0));
  EXPECT_EQ(tree.getNodeValueFeature(1, 0) + tree.getNodeValueFeature(2, 0),
            tree.getNodeValueFeature(0, 0));
  EXPECT_EQ("((1@1:B [0|0|0|1] )0@0:R [0|0|0|2] (2@2:B [0|0|0|1] ))\n",
            tree.printGameTree(0));

  //expand from the leaf node
  tree.expandNode(1, 3);
  tree.backpropagatefromSimulation(3, tree.updateNodefromSimulation(3, 0));
  tree.expandNode(1, 4);
  tree.backpropagatefromSimulation(4, tree.updateNodefromSimulation(4, 0));
  tree.expandNode(3, 5);
  tree.expandNode(2, 6);
  tree.backpropagatefromSimulation(5, tree.updateNodefromSimulation(5, 0));
  tree.backpropagatefromSimulation(6, tree.updateNodefromSimulation(6, 0));
  EXPECT_EQ(tree.getNodeValueFeature(3, 0) + tree.getNodeValueFeature(4, 0) + 1,
            tree.getNodeValueFeature(1, 0));
  EXPECT_EQ(tree.getNodeValueFeature(1, 0) + tree.getNodeValueFeature(2, 0),
            tree.getNodeValueFeature(0, 0));
  EXPECT_EQ(
      "((((5@5:B [0|0|0|1] )3@3:R [0|0|0|2] )1@1:B [0|0|0|4] (4@4:R [0|0|0|1] ))0@0:R [0|0|0|6] ((6@6:R [0|0|0|1] )2@2:B [0|0|0|2] ))\n",
      tree.printGameTree(0));

  // test with updating winning count
  tree.expandNode(5, 7);
  tree.backpropagatefromSimulation(7, tree.updateNodefromSimulation(7, 0));
  tree.expandNode(7, 8);
  tree.backpropagatefromSimulation(8, tree.updateNodefromSimulation(8, 0));
  tree.expandNode(8, 9);
  tree.backpropagatefromSimulation(9, tree.updateNodefromSimulation(9, 1));
  EXPECT_EQ(tree.getNodeValueFeature(3, 0) + tree.getNodeValueFeature(4, 0) + 1,
            tree.getNodeValueFeature(1, 0));  // should be visit count of node(3) + visit count of node(4) = visit count of node(1)
  EXPECT_EQ(tree.getNodeValueFeature(1, 0) + tree.getNodeValueFeature(2, 0),
            tree.getNodeValueFeature(0, 0));
  EXPECT_EQ(
      "(((((((9@9:R [0|0|-1|1] )8@8:B [0|0|1|2] )7@7:R [0|0|-1|3] )5@5:B [0|0|1|4] )3@3:R [0|0|-1|5] )1@1:B [0|0|1|7] (4@4:R [0|0|0|1] ))0@0:R [0|0|-1|9] ((6@6:R [0|0|0|1] )2@2:B [0|0|0|2] ))\n",
      tree.printGameTree(0));

  for (int i = 0; i < 4; i++)
    tree.backpropagatefromSimulation(6, tree.updateNodefromSimulation(6, 1));

  EXPECT_EQ(tree.getNodeValueFeature(6, 0) + 1, tree.getNodeValueFeature(2, 0));
  EXPECT_EQ(tree.getNodeValueFeature(1, 0) + tree.getNodeValueFeature(2, 0),
            tree.getNodeValueFeature(0, 0));
  EXPECT_EQ(
      "(((((((9@9:R [0|0|-1|1] )8@8:B [0|0|1|2] )7@7:R [0|0|-1|3] )5@5:B [0|0|1|4] )3@3:R [0|0|-1|5] )1@1:B [0|0|1|7] (4@4:R [0|0|0|1] ))0@0:R [0|0|-5|13] ((6@6:R [0|0|-4|5] )2@2:B [0|0|4|6] ))\n",
      tree.printGameTree(0));

  //test with back-propagate to the certain level
  tree.backpropagatefromSimulation(9, tree.updateNodefromSimulation(9, 1), 4);
  EXPECT_EQ(
      "(((((((9@9:R [0|0|-2|2] )8@8:B [0|0|2|3] )7@7:R [0|0|-2|4] )5@5:B [0|0|2|5] )3@3:R [0|0|-2|6] )1@1:B [0|0|1|7] (4@4:R [0|0|0|1] ))0@0:R [0|0|-5|13] ((6@6:R [0|0|-4|5] )2@2:B [0|0|4|6] ))\n",
      tree.printGameTree(0));

  //test with reroot by position
  int indexofroot = tree.reRootbyPosition(1);
  EXPECT_EQ(
      "((((((9@9:R [0|0|-2|2] )8@8:B [0|0|2|3] )7@7:R [0|0|-2|4] )5@5:B [0|0|2|5] )3@3:R [0|0|-2|6] )1@1:B [0|0|1|7] (4@4:R [0|0|0|1] ))\n",
      tree.printGameTree(indexofroot));

  //test with prune by position
  //tree.prunebyPosition(9, -1);
  //cout << tree.printGameTree(0);
}
TEST_F(MinMaxTest,MCSTBasic) {
  //test with selection
  //1. always choose root if equal chances are distributed in the children: equal chances: there's no update from simulation
  GameTree tree('R');
  unsigned numofmoves = 9;
  int maxnode = -1, sum = 0, node = 0;
  for (unsigned i = 0; i < numofmoves; ++i) {
    node = tree.selectMaxBalanceNode(numofmoves);
    int leaf = tree.expandNode(node, (i + 1));
    float prob = static_cast<float>(rand()) / RAND_MAX;
    if (prob <= 0.5)
      tree.backpropagatefromSimulation(leaf,
                                       tree.updateNodefromSimulation(leaf, -1));
    else {
      tree.backpropagatefromSimulation(leaf,
                                       tree.updateNodefromSimulation(leaf, 1));
      if (maxnode < 0)
        maxnode = leaf;
    }
    sum += tree.getNodeValueFeature(leaf, 0);
    EXPECT_EQ(sum, tree.getNodeValueFeature(0, 0));
    EXPECT_EQ(0, node);
  }
  node = tree.selectMaxBalanceNode(numofmoves);
  EXPECT_EQ(maxnode, node);
  unsigned numofmaxnodechildren = 0;
  int firstchild = -1;
  //2. play with the most probable node and descend from this node
  while (numofmaxnodechildren < (numofmoves - 1)) {
    EXPECT_TRUE(node <= static_cast<int>(numofmoves));
    size_t position = rand() % numofmoves + 1;
    int leaf = tree.expandNode(node, position);
    if (node == maxnode) {
      tree.backpropagatefromSimulation(leaf,
                                       tree.updateNodefromSimulation(leaf, 1));
      ++numofmaxnodechildren;
      if (firstchild < 0)
        firstchild = leaf;
    } else
      tree.backpropagatefromSimulation(leaf,
                                       tree.updateNodefromSimulation(leaf, -1));
    node = tree.selectMaxBalanceNode(numofmoves);
    sum += tree.getNodeValueFeature(leaf, 0);
    EXPECT_EQ(sum, tree.getNodeValueFeature(0, 0));
  }
  node = tree.selectMaxBalanceNode(numofmoves);
  EXPECT_EQ(firstchild, node);

  //3. select from the middle of the game
  tree.reRootbyPosition(maxnode);
  node = tree.selectMaxBalanceNode(numofmoves - 1);
  EXPECT_EQ(firstchild, node);

  //test with expansion
  //1. expand from a visited node, check if the move is not repeating in the same level
  int numofhexgon = 5;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
  Game hexboardgame(board);
  GameTree gametree(playera.getViewLabel());

  MonteCarloTreeSearch mcst(&board, &playera, gametree);
  std::shared_ptr<bool> emptyglobal;
  vector<int> bwglobal, oppglobal;
  mcst.initGameState(emptyglobal, bwglobal, oppglobal);
  int currentempty = board.getNumofemptyhexgons();

  int maxchild = -1, selectnode;
  int proportionofempty = currentempty;
  vector<int> babywatsons(bwglobal), opponents(oppglobal);
  std::shared_ptr<bool> emptyindicators = std::shared_ptr<bool>(
      new bool[board.getSizeOfVertices()], default_delete<bool[]>());
  copy(emptyglobal.get(), emptyglobal.get() + board.getSizeOfVertices(),
       emptyindicators.get());
  cout << "game current state " << babywatsons.size() << " " << opponents.size()
       << endl;
  for (int i = 0; i < board.getSizeOfVertices(); ++i) {
    selectnode = mcst.selection(currentempty);
    int expandedchild = mcst.expansion(selectnode, emptyindicators,
                                       proportionofempty, babywatsons,
                                       opponents);
    if (maxchild < 0)
      maxchild = expandedchild;

    cout << "[i|selectnode|expandchild|BW's move size|OP's move size] " << i
         << "|" << selectnode << "|" << expandedchild << "|"
         << babywatsons.size() << "|" << opponents.size() << endl;
    ASSERT_FALSE(selectnode == expandedchild);
    mcst.backpropagation(expandedchild, -1);
    cout << gametree.printGameTree(0) << endl;
    EXPECT_EQ(0, selectnode);
  }
  selectnode = mcst.selection(currentempty);
  EXPECT_EQ(maxchild, selectnode);

  //2. expand from a normal leaf, check the game state on the game tree is restored
  copy(emptyglobal.get(), emptyglobal.get() + board.getSizeOfVertices(),
         emptyindicators.get());
  copy(bwglobal.begin(), bwglobal.end(), babywatsons.begin());
  copy(oppglobal.begin(), oppglobal.end(), opponents.begin());
  int cutoff = static_cast<int>(0.5 * board.getSizeOfVertices());
  cout << "[current number of empty | cutoff] " << currentempty << "|" << cutoff << endl;

  while (currentempty >= cutoff) {
    int redmove, redrow, redcol;
    redmove = mcst.genNextRandom(emptyindicators, currentempty);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playera, redrow, redcol);
    babywatsons.push_back(redmove);

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = mcst.genNextRandom(emptyindicators, currentempty);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playerb, bluerow, bluecol);
    opponents.push_back(bluemove);
    cout << "[current number of empty | cutoff] " << currentempty << "|" << cutoff << endl;
  }

  int countofempty = std::count(emptyglobal.get(),
                                emptyglobal.get() + board.getSizeOfVertices(),
                                false);
  ASSERT_TRUE(currentempty == countofempty);
  ASSERT_EQ(countofempty + bwglobal.size() + oppglobal.size(),
            board.getSizeOfVertices());

  //3. expand from a leaf node whose state is the end of the game
}
TEST_F(MinMaxTest,SimulationCombine) {
  //test with 3x3 board
  int numofhexgon = 3;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  GameTree gametree(playera.getViewLabel());
  MonteCarloTreeSearch mcst(&board, &playera, gametree);

  std::shared_ptr<bool> emptyglobal;
  vector<int> bwglobal, oppglobal;
  mcst.initGameState(emptyglobal, bwglobal, oppglobal);

  int currentempty = board.getNumofemptyhexgons();
  int proportionofempty = currentempty;
  size_t numberoftrials = 1;

  for (size_t i = 0; i < numberoftrials; i++) {
    vector<int> babywatsons(bwglobal), opponents(oppglobal);
    std::shared_ptr<bool> emptyindicators = std::shared_ptr<bool>(
        new bool[board.getSizeOfVertices()], default_delete<bool[]>());
    copy(emptyglobal.get(), emptyglobal.get() + board.getSizeOfVertices(),
         emptyindicators.get());
    int selectnode = mcst.selection(currentempty);
    int expandedchild = mcst.expansion(selectnode, emptyglobal,
                                       proportionofempty, babywatsons,
                                       opponents);

    //play-out
    int winner = mcst.playout(emptyindicators, proportionofempty, babywatsons,
                              opponents);

    mcst.backpropagation(expandedchild, winner);
  }

  GameTree tree = mcst.getGametree();
  cout << tree.printGameTree(0);

  int resultmove = mcst.getBestMove();
  cout << resultmove << endl;
  cout << tree.printGameTree(0);
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
