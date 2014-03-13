/*
 * MinMax_test.cpp
 *
 */
#include <bitset>
#include <limits>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <functional>

#include "gtest/gtest.h"

#include "Game.h"
#include "Global.h"
#include "Player.h"
#include "GameTree.h"
#include "MonteCarloTreeSearch.h"
#include "MultiMonteCarloTreeSearch.h"

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
TEST_F(MinMaxTest,GameTreeConstruct) {
  //test expand child
  //expand from root
  GameTree tree('B');
  tree.expandNode(0, 1, 'B');
  tree.expandNode(0, 2, 'B');
  EXPECT_EQ(tree.updateNodefromSimulation(1, 0), 0);
  EXPECT_EQ(tree.getNodeValueFeature(1, AbstractUTCPolicy::visitcount),
            tree.getNodeValueFeature(0, AbstractUTCPolicy::visitcount));
  EXPECT_EQ(tree.updateNodefromSimulation(2, 0), 0);
  EXPECT_EQ(1, tree.getNodeValueFeature(1, AbstractUTCPolicy::visitcount));
  EXPECT_EQ(1, tree.getNodeValueFeature(2, AbstractUTCPolicy::visitcount));
  EXPECT_EQ(
      tree.getNodeValueFeature(1, AbstractUTCPolicy::visitcount)
          + tree.getNodeValueFeature(2, AbstractUTCPolicy::visitcount),
      tree.getNodeValueFeature(0, AbstractUTCPolicy::visitcount));
  EXPECT_EQ("((1@1:B [0|0|0|1] )0@0:R [0|0|0|2] (2@2:B [0|0|0|1] ))\n",
            tree.printGameTree(0));

  //expand from the leaf node
  tree.expandNode(1, 3);
  EXPECT_EQ(tree.updateNodefromSimulation(3, 0), 0);
  tree.expandNode(1, 4);
  EXPECT_EQ(tree.updateNodefromSimulation(4, 0), 0);
  tree.expandNode(3, 5);
  tree.expandNode(2, 6);
  EXPECT_EQ(tree.updateNodefromSimulation(5, 0), 0);
  EXPECT_EQ(tree.updateNodefromSimulation(6, 0), 0);
  EXPECT_EQ(
      tree.getNodeValueFeature(3, AbstractUTCPolicy::visitcount)
          + tree.getNodeValueFeature(4, AbstractUTCPolicy::visitcount) + 1,
      tree.getNodeValueFeature(1, AbstractUTCPolicy::visitcount));
  EXPECT_EQ(
      tree.getNodeValueFeature(1, AbstractUTCPolicy::visitcount)
          + tree.getNodeValueFeature(2, AbstractUTCPolicy::visitcount),
      tree.getNodeValueFeature(0, AbstractUTCPolicy::visitcount));
  EXPECT_EQ(
      "((((5@5:B [0|0|0|1] )3@3:R [0|0|0|2] )1@1:B [0|0|0|4] (4@4:R [0|0|0|1] ))0@0:R [0|0|0|6] ((6@6:R [0|0|0|1] )2@2:B [0|0|0|2] ))\n",
      tree.printGameTree(0));

  // test with updating winning count
  tree.expandNode(5, 7);
  EXPECT_EQ(tree.updateNodefromSimulation(7, 0), 0);
  tree.expandNode(7, 8);
  EXPECT_EQ(tree.updateNodefromSimulation(8, 0), 0);
  tree.expandNode(8, 9);
  EXPECT_EQ(tree.updateNodefromSimulation(9, 1), -1);
  EXPECT_EQ(
      tree.getNodeValueFeature(3, AbstractUTCPolicy::visitcount)
          + tree.getNodeValueFeature(4, AbstractUTCPolicy::visitcount) + 1,
      tree.getNodeValueFeature(1, AbstractUTCPolicy::visitcount));
  EXPECT_EQ(
      tree.getNodeValueFeature(1, AbstractUTCPolicy::visitcount)
          + tree.getNodeValueFeature(2, AbstractUTCPolicy::visitcount),
      tree.getNodeValueFeature(0, AbstractUTCPolicy::visitcount));
  EXPECT_EQ(
      "(((((((9@9:R [0|0|-1|1] )8@8:B [0|0|1|2] )7@7:R [0|0|-1|3] )5@5:B [0|0|1|4] )3@3:R [0|0|-1|5] )1@1:B [0|0|1|7] (4@4:R [0|0|0|1] ))0@0:R [0|0|-1|9] ((6@6:R [0|0|0|1] )2@2:B [0|0|0|2] ))\n",
      tree.printGameTree(0));

  for (int i = 0; i < 4; i++)
    EXPECT_EQ(tree.updateNodefromSimulation(6, 1), -1);

  EXPECT_EQ(tree.getNodeValueFeature(6, AbstractUTCPolicy::visitcount) + 1,
            tree.getNodeValueFeature(2, AbstractUTCPolicy::visitcount));
  EXPECT_EQ(
      tree.getNodeValueFeature(1, AbstractUTCPolicy::visitcount)
          + tree.getNodeValueFeature(2, AbstractUTCPolicy::visitcount),
      tree.getNodeValueFeature(0, AbstractUTCPolicy::visitcount));
  EXPECT_EQ(
      "(((((((9@9:R [0|0|-1|1] )8@8:B [0|0|1|2] )7@7:R [0|0|-1|3] )5@5:B [0|0|1|4] )3@3:R [0|0|-1|5] )1@1:B [0|0|1|7] (4@4:R [0|0|0|1] ))0@0:R [0|0|-5|13] ((6@6:R [0|0|-4|5] )2@2:B [0|0|4|6] ))\n",
      tree.printGameTree(0));

  //test with back-propagate to the certain level
  tree.updateNodefromSimulation(9, 1, 4);
  EXPECT_EQ(
      "(((((((9@9:R [0|0|-2|2] )8@8:B [0|0|2|3] )7@7:R [0|0|-2|4] )5@5:B [0|0|2|5] )3@3:R [0|0|-2|6] )1@1:B [0|0|1|7] (4@4:R [0|0|0|1] ))0@0:R [0|0|-5|13] ((6@6:R [0|0|-4|5] )2@2:B [0|0|4|6] ))\n",
      tree.printGameTree(0));
}
//test with selection
TEST_F(MinMaxTest, MCSTSelection) {
  //1. always choose root if equal chances are distributed in the children: equal chances: there's no update from simulation
  GameTree tree('R');
  unsigned numofmoves = 9;
  int maxnode = -1, sum = 0, node = 0;
  for (unsigned i = 0; i < numofmoves; ++i) {
    node = tree.selectMaxBalanceNode(numofmoves, false);
    int leaf = tree.expandNode(node, (i + 1));
    float prob = static_cast<float>(rand()) / RAND_MAX;
    if (prob <= 0.5)
      tree.updateNodefromSimulation(leaf, -1);
    else {
      tree.updateNodefromSimulation(leaf, 1);
      if (maxnode < 0)
        maxnode = leaf;
    }
    sum += tree.getNodeValueFeature(leaf, AbstractUTCPolicy::visitcount);
    EXPECT_EQ(sum, tree.getNodeValueFeature(0, AbstractUTCPolicy::visitcount));
    EXPECT_EQ(0, node);
  }
  node = tree.selectMaxBalanceNode(numofmoves, false);
  EXPECT_EQ(maxnode, node);
  unsigned numofmaxnodechildren = 0;
  int firstchild = -1;
  //2. play with the most probable node and descend from this node
  while (numofmaxnodechildren < (numofmoves - 1)) {
    EXPECT_TRUE(node <= static_cast<int>(numofmoves));
    size_t position = rand() % numofmoves + 1;
    int leaf = tree.expandNode(node, position);
    if (node == maxnode) {
      tree.updateNodefromSimulation(leaf, 1);
      ++numofmaxnodechildren;
      if (firstchild < 0)
        firstchild = leaf;
    } else
      tree.updateNodefromSimulation(leaf, -1);
    node = tree.selectMaxBalanceNode(numofmoves, false);
    sum += tree.getNodeValueFeature(leaf, AbstractUTCPolicy::visitcount);
    EXPECT_EQ(sum, tree.getNodeValueFeature(0, AbstractUTCPolicy::visitcount));
  }
  //add
  size_t position = rand() % numofmoves + 1;
  int leaf = tree.expandNode(node, position);
  EXPECT_NE(leaf, node);

  node = tree.selectMaxBalanceNode(numofmoves, false);
  EXPECT_EQ(firstchild, node);
  position = rand() % numofmoves + 1;
  leaf = tree.expandNode(node, position);
  EXPECT_NE(leaf, node);

  //3. test with the end of the game
  tree.clearAll();
  firstchild = -1;
  for (unsigned i = 0; i < 2; ++i) {
    node = tree.selectMaxBalanceNode(2, false);
    position = rand() % numofmoves + 1;
    leaf = tree.expandNode(node, position);
    if (firstchild < 0)
      firstchild = leaf;
    if (node == firstchild)
      tree.updateNodefromSimulation(leaf, 1);
    else
      tree.updateNodefromSimulation(leaf, -1);
  }
  node = tree.selectMaxBalanceNode(2, false);
  EXPECT_EQ(firstchild, node);

  //4. test with tie
  for (unsigned i = 0; i < (numofmoves - 1); ++i) {
    node = tree.selectMaxBalanceNode(numofmoves - 1);
    position = rand() % numofmoves + 1;
    leaf = tree.expandNode(node, position);
    tree.updateNodefromSimulation(leaf, -1);
    cout << tree.printGameTree(0);
    EXPECT_NE(firstchild, node);
  }
}
//test with expansion
TEST_F(MinMaxTest,MCSTExpansion) {
  //1. expand from a visited node, check if the move is not repeating in the same level
  int numofhexgon = 5;
  HexBoard board(numofhexgon);
#if __cplusplus > 199711L
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
#else
  Player playera(board, RED);  //north to south, 'O'
  Player playerb(board, BLUE);//west to east, 'X'
#endif
  Game hexboardgame(board);
  GameTree gametree(playera.getViewLabel());
  MonteCarloTreeSearch mcst(&board, &playera);

  //restore the current game progress
  hexgame::shared_ptr<bool> emptyinit;
  vector<int> bwinit, oppinit;
  mcst.initGameState(emptyinit, bwinit, oppinit);
  int initempty = board.getNumofemptyhexgons();

  int maxchild = -1, selectnode;

  for (int i = 0; i < board.getSizeOfVertices(); ++i) {
    //initialize containers for simulation
    vector<int> babywatsons(bwinit), opponents(oppinit);
    hexgame::shared_ptr<bool> emptyindicators = hexgame::shared_ptr<bool>(
        new bool[board.getSizeOfVertices()], hexgame::default_delete<bool[]>());
    copy(emptyinit.get(), emptyinit.get() + board.getSizeOfVertices(),
         emptyindicators.get());

    int proportionofempty = initempty;

    selectnode = mcst.selection(proportionofempty, gametree);
    int expandedchild = mcst.expansion(selectnode, emptyindicators,
                                       proportionofempty, babywatsons,
                                       opponents, gametree);
#if __cplusplus > 199711L
    for (auto iter = babywatsons.begin(); iter != babywatsons.end(); ++iter)
      ASSERT_FALSE(emptyindicators.get()[*iter - 1]);

    for (auto iter = opponents.begin(); iter != opponents.end(); ++iter)
      ASSERT_FALSE(emptyindicators.get()[*iter - 1]);
#else
    for (vector<int>::iterator iter = babywatsons.begin(); iter != babywatsons.end(); ++iter)
    ASSERT_FALSE(emptyindicators.get()[*iter - 1]);

    for (vector<int>::iterator iter = opponents.begin(); iter != opponents.end(); ++iter)
    ASSERT_FALSE(emptyindicators.get()[*iter - 1]);
#endif

    vector<size_t> siblings = gametree.getSiblings(expandedchild);
    if (!siblings.empty()) {
      vector<size_t> positions;
      positions.reserve(siblings.size());
#if __cplusplus > 199711L
      for (auto iter = siblings.begin(); iter != siblings.end(); ++iter)
        positions.push_back(gametree.getNodePosition(*iter));
#else
      for (vector<size_t>::iterator iter = siblings.begin(); iter != siblings.end(); ++iter)
      positions.push_back(gametree.getNodePosition(*iter));
#endif
      sort(positions.begin(), positions.end());
      for (unsigned i = 0; i < (positions.size() - 1); ++i)
        ASSERT_NE(positions[i], positions[i + 1]);
    }
    if (maxchild < 0)
      maxchild = expandedchild;

    ASSERT_FALSE(selectnode == expandedchild);
    mcst.backpropagation(expandedchild, -1, gametree);

    EXPECT_EQ(0, selectnode);
  }
  selectnode = mcst.selection(initempty, gametree);
  //EXPECT_EQ(maxchild, selectnode);

  //clean out game tree for the current round of estimation
  gametree.clearAll();
  ASSERT_TRUE(gametree.getSizeofNodes() == 1);
  ASSERT_TRUE(gametree.getSizeofEdges() == 0);

  //2. expand from a normal leaf, check the game state on the game tree is restored
  int cutoff = static_cast<int>(0.75 * board.getSizeOfVertices());

  //push the game forward towards 1/4 of the game
  while (initempty >= cutoff) {
    int redmove, redrow, redcol;
    redmove = mcst.genNextRandom(emptyinit, initempty);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;
    ASSERT_TRUE(hexboardgame.setMove(playera, redrow, redcol));
    bwinit.push_back(redmove);

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = mcst.genNextRandom(emptyinit, initempty);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    ASSERT_TRUE(hexboardgame.setMove(playerb, bluerow, bluecol));
    oppinit.push_back(bluemove);
  }
  hexgame::shared_ptr<bool> emptycurrent;
  vector<int> bwcurrent, oppcurrent;
  mcst.initGameState(emptycurrent, bwcurrent, oppcurrent);
  int currentempty = board.getNumofemptyhexgons();

  ASSERT_TRUE(
      static_cast<int>(currentempty + bwcurrent.size() + oppcurrent.size())
          == board.getSizeOfVertices());
  int countofempty = std::count(emptyinit.get(),
                                emptyinit.get() + board.getSizeOfVertices(),
                                true);

  ASSERT_TRUE(initempty == countofempty);
  ASSERT_TRUE(currentempty == initempty);
  ASSERT_EQ(bwinit.size(), bwcurrent.size());
  ASSERT_EQ(oppinit.size(), oppcurrent.size());

  //fast forward game tree construction till 1/2 of game
  int halfgame = static_cast<int>(0.5 * board.getSizeOfVertices());
  int treestate = 0, prevtreestate = treestate;
  maxchild = 0;
  while (treestate < halfgame) {
    //initialize containers for simulation
    vector<int> babywatsons(bwcurrent), opponents(oppcurrent);

    hexgame::shared_ptr<bool> emptyindicators = hexgame::shared_ptr<bool>(
        new bool[board.getSizeOfVertices()], default_delete<bool[]>());

    copy(emptycurrent.get(), emptycurrent.get() + board.getSizeOfVertices(),
         emptyindicators.get());

    int proportionofempty = currentempty;

    selectnode = mcst.selection(proportionofempty, gametree);
    int expandedchild = mcst.expansion(selectnode, emptyindicators,
                                       proportionofempty, babywatsons,
                                       opponents, gametree);
#if __cplusplus > 199711L
    for (auto iter = babywatsons.begin(); iter != babywatsons.end(); ++iter)
      ASSERT_FALSE(emptyindicators.get()[*iter - 1]);

    for (auto iter = opponents.begin(); iter != opponents.end(); ++iter)
      ASSERT_FALSE(emptyindicators.get()[*iter - 1]);
#else
    for (vector<int>::iterator iter = babywatsons.begin(); iter != babywatsons.end(); ++iter)
    ASSERT_FALSE(emptyindicators.get()[*iter - 1]);

    for (vector<int>::iterator iter = opponents.begin(); iter != opponents.end(); ++iter)
    ASSERT_FALSE(emptyindicators.get()[*iter - 1]);
#endif

    vector<size_t> siblings = gametree.getSiblings(expandedchild);
    if(!siblings.empty()){
    vector<size_t> positions;
    positions.reserve(siblings.size());
#if __cplusplus > 199711L
    for (auto iter = siblings.begin(); iter != siblings.end(); ++iter)
#else
      for (vector<size_t>::iterator iter = siblings.begin(); iter != siblings.end(); ++iter)
#endif
      positions.push_back(gametree.getNodePosition(*iter));
    sort(positions.begin(), positions.end());
    for (unsigned i = 0; i < (positions.size() - 1); ++i)
      ASSERT_NE(positions[i], positions[i + 1]);
    }
    prevtreestate = treestate;
    treestate = (babywatsons.size() + opponents.size());
    if (prevtreestate != treestate)
      maxchild = expandedchild;

    int depth = gametree.getNodeDepth(expandedchild);

    ASSERT_FALSE(selectnode == expandedchild);
    ASSERT_TRUE(
        (treestate + (currentempty - depth)
            == static_cast<int>(board.getSizeOfVertices())));

    //check if the game state is restored
    vector<int> actual(babywatsons.size() + opponents.size());
    sort(babywatsons.begin(), babywatsons.end());
    sort(opponents.begin(), opponents.end());
    merge(babywatsons.begin(), babywatsons.end(), opponents.begin(),
          opponents.end(), actual.begin());
    for (unsigned i = 1; i < actual.size(); ++i)
      EXPECT_NE(actual[i - 1], actual[i]);

    if (selectnode == maxchild)
      mcst.backpropagation(expandedchild, 1, gametree);
    else
      mcst.backpropagation(expandedchild, -1, gametree);
  }

  //3. expand from a leaf node whose state is the end of the game
  //push the game forward towards the end of the game
  cutoff = 3;

  //push the game forward towards end of the game
  while (initempty > cutoff) {
    int redmove, redrow, redcol;
    redmove = mcst.genNextRandom(emptyinit, initempty);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playera, redrow, redcol);
    bwinit.push_back(redmove);

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = mcst.genNextRandom(emptyinit, initempty);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playerb, bluerow, bluecol);
    oppinit.push_back(bluemove);
  }
  countofempty = std::count(emptyinit.get(),
                            emptyinit.get() + board.getSizeOfVertices(), true);

  ASSERT_TRUE(initempty == countofempty);
  gametree.clearAll();
  currentempty = initempty;
  int depth = 0, predepth = depth;
  for (int i = 0; i < (cutoff + 18); ++i) {
    //initialize containers for simulation
    vector<int> babywatsons(bwinit), opponents(oppinit);
    hexgame::shared_ptr<bool> emptyindicators = hexgame::shared_ptr<bool>(
        new bool[board.getSizeOfVertices()], hexgame::default_delete<bool[]>());
    copy(emptyinit.get(), emptyinit.get() + board.getSizeOfVertices(),
         emptyindicators.get());

    int proportionofempty = currentempty;

    selectnode = mcst.selection(proportionofempty, gametree);
    depth = gametree.getNodeDepth(selectnode);
    int expandedchild = mcst.expansion(selectnode, emptyindicators,
                                       proportionofempty, babywatsons,
                                       opponents, gametree);
#if __cplusplus > 199711L
    for (auto iter = babywatsons.begin(); iter != babywatsons.end(); ++iter)
      ASSERT_FALSE(emptyindicators.get()[*iter - 1]);

    for (auto iter = opponents.begin(); iter != opponents.end(); ++iter)
      ASSERT_FALSE(emptyindicators.get()[*iter - 1]);
#else
    for (vector<int>::iterator iter = babywatsons.begin(); iter != babywatsons.end(); ++iter)
    ASSERT_FALSE(emptyindicators.get()[*iter - 1]);

    for (vector<int>::iterator iter = opponents.begin(); iter != opponents.end(); ++iter)
    ASSERT_FALSE(emptyindicators.get()[*iter - 1]);
#endif

    vector<size_t> siblings = gametree.getSiblings(expandedchild);
    if(!siblings.empty()){
    vector<size_t> positions;
    positions.reserve(siblings.size());
#if __cplusplus > 199711L
    for (auto iter = siblings.begin(); iter != siblings.end(); ++iter)
#else
      for (vector<size_t>::iterator iter = siblings.begin(); iter != siblings.end(); ++iter)
#endif
      positions.push_back(gametree.getNodePosition(*iter));
    sort(positions.begin(), positions.end());
    for (unsigned j = 0; j < (positions.size() - 1); ++j)
      ASSERT_NE(positions[j], positions[j + 1]);
    }
    if (predepth != depth)
      mcst.backpropagation(expandedchild, 1, gametree);
    else
      mcst.backpropagation(expandedchild, -1, gametree);

    if (i < cutoff)
      EXPECT_EQ(selectnode, 0);
    else if (depth == cutoff) {
      EXPECT_EQ(expandedchild, selectnode);
      EXPECT_TRUE(
          static_cast<int>(babywatsons.size() + opponents.size())
              == board.getSizeOfVertices());
    }
  }
}
//test with the whole simulation code
TEST_F(MinMaxTest,SimulationCombine) {
  int numofhexgon = 5;
  HexBoard board(numofhexgon);
#if __cplusplus > 199711L
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
#else
                 Player playerb(board, BLUE);  //west to east, 'X'
#endif
  GameTree gametree(playerb.getViewLabel());
  MonteCarloTreeSearch mcst(&board, &playerb);

  int currentempty = board.getNumofemptyhexgons();
  size_t numberoftrials = 3000;
  hexgame::shared_ptr<bool> emptyglobal;
  vector<int> bwglobal, oppglobal;
  mcst.initGameState(emptyglobal, bwglobal, oppglobal);

  //checking from the beginning of the game
  for (size_t i = 0; i < numberoftrials; ++i) {
    //initialize the following containers to the current progress of playing board
    int proportionofempty = currentempty;
    vector<int> babywatsons(bwglobal), opponents(oppglobal);
    hexgame::shared_ptr<bool> emptyindicators = hexgame::shared_ptr<bool>(
        new bool[board.getSizeOfVertices()], hexgame::default_delete<bool[]>());
    copy(emptyglobal.get(), emptyglobal.get() + board.getSizeOfVertices(),
         emptyindicators.get());

    //in-tree phase
    int selectnode = mcst.selection(currentempty, gametree);
    int expandednode = mcst.expansion(selectnode, emptyindicators,
                                      proportionofempty, babywatsons, opponents,
                                      gametree);
    //check the newly expanded node has duplicated position in board
    vector<size_t> siblings = gametree.getSiblings(expandednode);
    if(!siblings.empty()){
    vector<size_t> positions;
    positions.reserve(siblings.size());
#if __cplusplus > 199711L
    for (auto iter = siblings.begin(); iter != siblings.end(); ++iter)
#else
      for (vector<size_t>::iterator iter = siblings.begin(); iter != siblings.end(); ++iter)
#endif
      positions.push_back(gametree.getNodePosition(*iter));
    sort(positions.begin(), positions.end());
    for (unsigned i = 0; i < (positions.size() - 1); ++i)
      ASSERT_NE(positions[i], positions[i + 1]);
    }
    //simulation phase
    int winner = mcst.playout(emptyindicators, proportionofempty, babywatsons,
                              opponents);
    //back-propagate
    mcst.backpropagation(expandednode, winner, gametree);
  }
  int resultmove = mcst.getBestMove(gametree);

  //find the move with the maximal successful simulated outcome
  ASSERT_TRUE(resultmove != -1);

  //adding more checking points
  //1. check the visited count of root which should sum up as numberoftrials
  //2. make sure the node with maximal winning count wins
  size_t sum = 0;
  float max = 0;
  int indexofmax = -1;
  for (int i = 1; i <= board.getSizeOfVertices(); ++i) {
    ASSERT_EQ(gametree.getNodeDepth(i), 1);
    sum += gametree.getNodeValueFeature(i, AbstractUTCPolicy::visitcount);
    float value = static_cast<float>(gametree.getNodeValueFeature(
        i, AbstractUTCPolicy::wincount))
        / static_cast<float>(gametree.getNodeValueFeature(
            i, AbstractUTCPolicy::visitcount));
    if (max < value) {
      max = value;
      indexofmax = i;
    }
  }
  EXPECT_EQ(sum, numberoftrials);
  EXPECT_EQ(resultmove, gametree.getNodePosition(indexofmax));
}
TEST_F(MinMaxTest,CheckEndofGame) {
  int numofhexgon = 5;
  AbstractStrategy* bluestrategy;

  for (unsigned k = 0; k < 3; ++k) {  //Strategy, MonteCarloTreeSearch, MultiMonteCarloTreeSearch
    HexBoard board(numofhexgon);
    Game hexboardgame(board);
#if __cplusplus > 199711L
    Player playera(board, hexgonValKind::RED);  //north to south, 'O'
    Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
#else
    Player playera(board, RED);  //north to south, 'O'
    Player playerb(board, BLUE);//west to east, 'X'
#endif

    MonteCarloTreeSearch mcstred(&board, &playera, 1);
    switch (k) {
      case 0:
        bluestrategy = new Strategy(&board, &playera, 1);
        break;
      case 1:
        bluestrategy = new MonteCarloTreeSearch(&board, &playera, 1);
        break;
      case 2:
        bluestrategy = new MultiMonteCarloTreeSearch(&board, &playera, 1);
        break;
    }

    string winner = "UNKNOWN";
    int round = 0, currentempty = board.getNumofemptyhexgons();
    for (int j = 0; j < (board.getSizeOfVertices() / 2 + 1); ++j) {
      cout << "current number of empty = " << currentempty << endl;

      //the virtual player moves
      int redmove, redrow, redcol;
      redmove = hexboardgame.genMove(mcstred);
      redrow = (redmove - 1) / numofhexgon + 1;
      redcol = (redmove - 1) % numofhexgon + 1;
      ASSERT_TRUE(hexboardgame.setMove(playera, redrow, redcol));

      if (j < board.getSizeOfVertices() / 2) {
        //the virtual opponent moves
        int bluemove, bluerow, bluecol;
        bluemove = hexboardgame.genMove(*bluestrategy);
        bluerow = (bluemove - 1) / numofhexgon + 1;
        bluecol = (bluemove - 1) % numofhexgon + 1;
        cout << __LINE__ << " " << (*bluestrategy).name() << " " << bluemove
             << " " << bluerow << " " << bluecol << endl;
        ASSERT_TRUE(hexboardgame.setMove(playerb, bluerow, bluecol));

        cout << "simulation " << round << " : " << mcstred.name() << " "
             << redmove << " " << (*bluestrategy).name() << " " << bluemove
             << endl;
        cout << hexboardgame.showView(playera, playerb);
        ASSERT_NE(redmove, bluemove);
      }

      currentempty = board.getNumofemptyhexgons();
      cout << hexboardgame.showView(playera, playerb);
      winner = hexboardgame.getWinner(playera, playerb);
      round++;
    }

    ASSERT_EQ(currentempty, 0);

    int move = hexboardgame.genMove(*bluestrategy);
    int row = (move - 1) / numofhexgon + 1;
    int col = (move - 1) % numofhexgon + 1;

    ASSERT_TRUE(hexboardgame.setMove(playerb, row, col));
    EXPECT_EQ(move, -1);
    cout << "winner is " << winner << endl;
  }
  delete bluestrategy;
}
TEST_F(MinMaxTest,CheckHexFiveGame) {
  int numofhexgon = 3;
  HexBoard board(numofhexgon);
#if __cplusplus > 199711L
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
#else
  Player playera(board, RED);  //north to south, 'O'
  Player playerb(board, BLUE);//west to east, 'X'
#endif
  MonteCarloTreeSearch mcstred(&board, &playera, 2000);
  MonteCarloTreeSearch mcstblue(&board, &playerb, 2000);
  Game hexboardgame(board);
  string winner = "UNKNOWN";
  int round = 0;
  while (winner == "UNKNOWN") {
    //the virtual player moves
    int redmove, redrow, redcol;
    redmove = hexboardgame.genMove(mcstred);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;

    ASSERT_TRUE(hexboardgame.setMove(playera, redrow, redcol));

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(mcstblue);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    ASSERT_TRUE(hexboardgame.setMove(playerb, bluerow, bluecol));

    cout << "simulation " << round << " : " << mcstred.name() << " " << redmove
         << " " << mcstblue.name() << " " << bluemove << endl;
    cout << hexboardgame.showView(playera, playerb);
    round++;
    ASSERT_NE(redmove, bluemove);
    winner = hexboardgame.getWinner(playera, playerb);
  }
  cout << "winner is " << winner << endl;
}
TEST_F(MinMaxTest,CompeteHexFiveGame) {
  int numofhexgon = 5;
  HexBoard board(numofhexgon);
#if __cplusplus > 199711L
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
#else
  Player playera(board, RED);  //north to south, 'O'
  Player playerb(board, BLUE);//west to east, 'X'
#endif
  Strategy naivered(&board, &playera, 2000);
  MonteCarloTreeSearch mcstblue(&board, &playerb, 2000);
  Game hexboardgame(board);
  string winner = "UNKNOWN";
  int round = 0;
  while (winner == "UNKNOWN") {
    //the virtual player moves
    int redmove, redrow, redcol;
    redmove = hexboardgame.genMove(naivered);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;

    ASSERT_TRUE(hexboardgame.setMove(playera, redrow, redcol));

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(mcstblue);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    ASSERT_TRUE(hexboardgame.setMove(playerb, bluerow, bluecol));

    cout << "simulation " << round << " : " << naivered.name() << " " << redmove
         << " " << mcstblue.name() << " " << bluemove << endl;
    round++;
    ASSERT_NE(redmove, bluemove);
    cout << hexboardgame.showView(playera, playerb);
    winner = hexboardgame.getWinner(playera, playerb);
  }
  cout << "winner is " << winner << endl;
}
TEST_F(MinMaxTest,CheckHexELEVENGame) {
  int numofhexgon = 11;
  HexBoard board(numofhexgon);
#if __cplusplus > 199711L
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
#else
  Player playera(board, RED);  //north to south, 'O'
  Player playerb(board, BLUE);//west to east, 'X'
#endif
  MonteCarloTreeSearch mcstred(&board, &playera);
  MonteCarloTreeSearch mcstblue(&board, &playerb);
  Game hexboardgame(board);
  string winner = "UNKNOWN";
  int round = 0;
  while (winner == "UNKNOWN") {
    //the virtual player moves
    int redmove, redrow, redcol;
    redmove = hexboardgame.genMove(mcstred);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;

    ASSERT_TRUE(hexboardgame.setMove(playera, redrow, redcol));

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(mcstblue);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    ASSERT_TRUE(hexboardgame.setMove(playerb, bluerow, bluecol));

    cout << "simulation " << round << " : " << mcstred.name() << " " << redmove
         << " " << mcstblue.name() << " " << bluemove << endl;
    round++;
    ASSERT_NE(redmove, bluemove);
    //cout << hexboardgame.showView(playera, playerb);
    winner = hexboardgame.getWinner(playera, playerb);
  }
  cout << "winner is " << winner << endl;
}
TEST_F(MinMaxTest,CompeteHexELEVENGame) {
  int numofhexgon = 11;
  HexBoard board(numofhexgon);
#if __cplusplus > 199711L
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
#else
  Player playera(board, RED);  //north to south, 'O'
  Player playerb(board, BLUE);//west to east, 'X'
#endif
  Strategy naivered(&board, &playera);
  MonteCarloTreeSearch mcstblue(&board, &playerb);
  Game hexboardgame(board);
  string winner = "UNKNOWN";
  int round = 0;
  while (winner == "UNKNOWN") {
    //the virtual player moves
    int redmove, redrow, redcol;
    redmove = hexboardgame.genMove(naivered);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;

    ASSERT_TRUE(hexboardgame.setMove(playera, redrow, redcol));

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(mcstblue);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    ASSERT_TRUE(hexboardgame.setMove(playerb, bluerow, bluecol));

    cout << "simulation " << round << " : " << naivered.name() << " " << redmove
         << " " << mcstblue.name() << " " << bluemove << endl;
    round++;
    ASSERT_NE(redmove, bluemove);
    cout << hexboardgame.showView(playera, playerb);
    winner = hexboardgame.getWinner(playera, playerb);
  }
  cout << "winner is " << winner << endl;
}
TEST_F(MinMaxTest,CheckHexParallelGame) {
  int numofhexgon = 3;
  HexBoard board(numofhexgon);

#if __cplusplus > 199711L
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
#else
  Player playera(board, RED);  //north to south, 'O'
  Player playerb(board, BLUE);//west to east, 'X'
#endif

  MultiMonteCarloTreeSearch mcstred(&board, &playera, 2000);
  MultiMonteCarloTreeSearch mcstblue(&board, &playerb, 2000);

  Game hexboardgame(board);
  string winner = "UNKNOWN";
  int round = 0;
  while (winner == "UNKNOWN") {
    //the virtual player moves
    int redmove, redrow, redcol;
    redmove = hexboardgame.genMove(mcstred);
    if (board.getNumofemptyhexgons() > 0)
      EXPECT_TRUE(redmove != -1);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;

    ASSERT_TRUE(hexboardgame.setMove(playera, redrow, redcol));
    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(mcstblue);
    if (board.getNumofemptyhexgons() > 0)
      EXPECT_TRUE(bluemove != -1);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    ASSERT_TRUE(hexboardgame.setMove(playerb, bluerow, bluecol));

    cout << "simulation " << round << " : " << mcstred.name() << " " << redmove
         << " " << mcstblue.name() << " " << bluemove << endl;
    round++;
    ASSERT_NE(redmove, bluemove);
    cout << hexboardgame.showView(playera, playerb);
    winner = hexboardgame.getWinner(playera, playerb);
  }
  cout << "winner is " << winner << endl;
}
TEST_F(MinMaxTest,CompeteHexParallelGame) {
  int numofhexgon = 3;
  HexBoard board(numofhexgon);

#if __cplusplus > 199711L
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
#else
  Player playera(board, RED);  //north to south, 'O'
  Player playerb(board, BLUE);//west to east, 'X'
#endif

   MonteCarloTreeSearch mcstred(&board, &playera, 2000);
   MultiMonteCarloTreeSearch mcstblue(&board, &playerb, 2000);

  Game hexboardgame(board);
  string winner = "UNKNOWN";
  int round = 0;
  while (winner == "UNKNOWN") {
    //the virtual player moves
    int redmove, redrow, redcol;
    redmove = hexboardgame.genMove(mcstred);
    if (board.getNumofemptyhexgons() > 0)
      EXPECT_TRUE(redmove != -1);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;

    ASSERT_TRUE(hexboardgame.setMove(playera, redrow, redcol));
    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(mcstblue);
    if (board.getNumofemptyhexgons() > 0)
      EXPECT_TRUE(bluemove != -1);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    ASSERT_TRUE(hexboardgame.setMove(playerb, bluerow, bluecol));

    cout << "simulation " << round << " : " << mcstred.name() << " " << redmove
         << " " << mcstblue.name() << " " << bluemove << endl;
    round++;
    ASSERT_NE(redmove, bluemove);
    cout << hexboardgame.showView(playera, playerb);
    winner = hexboardgame.getWinner(playera, playerb);
  }
  cout << "winner is " << winner << endl;
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
