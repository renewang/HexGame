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
#include "Player.h"
#include "GameTree.h"
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
            tree.getNodeValueFeature(1, 0));
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
//test with selection
TEST_F(MinMaxTest, MCSTSelection) {
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

  //3. test with the end of the game
  node = tree.selectMaxBalanceNode(2);
  EXPECT_EQ(firstchild, node);

  //4. select from the middle of the game
  tree.reRootbyPosition(maxnode);
  node = tree.selectMaxBalanceNode(numofmoves - 1);
  EXPECT_EQ(firstchild, node);
}
//test with expansion
TEST_F(MinMaxTest,MCSTExpansion) {
  //1. expand from a visited node, check if the move is not repeating in the same level
  int numofhexgon = 5;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
  Game hexboardgame(board);
  GameTree gametree(playera.getViewLabel());
  MonteCarloTreeSearch mcst(&board, &playera);

  //restore the current game progress
  std::shared_ptr<bool> emptyinit;
  vector<int> bwinit, oppinit;
  mcst.initGameState(emptyinit, bwinit, oppinit);
  int initempty = board.getNumofemptyhexgons();

  int maxchild = -1, selectnode;

  for (int i = 0; i < board.getSizeOfVertices(); ++i) {
    //initialize containers for simulation
    vector<int> babywatsons(bwinit), opponents(oppinit);
    std::shared_ptr<bool> emptyindicators = std::shared_ptr<bool>(
        new bool[board.getSizeOfVertices()], default_delete<bool[]>());
    copy(emptyinit.get(), emptyinit.get() + board.getSizeOfVertices(),
         emptyindicators.get());

    int proportionofempty = initempty;

    selectnode = mcst.selection(proportionofempty, gametree);
    int expandedchild = mcst.expansion(selectnode, emptyindicators,
                                       proportionofempty, babywatsons,
                                       opponents, gametree);

    for(auto iter = babywatsons.begin(); iter != babywatsons.end(); ++iter)
        ASSERT_FALSE(emptyindicators.get()[*iter-1]);

    for(auto iter = opponents.begin(); iter != opponents.end(); ++iter)
          ASSERT_FALSE(emptyindicators.get()[*iter-1]);

    vector<size_t> siblings = gametree.getSiblings(expandedchild);
    vector<size_t> positions;
    positions.reserve(siblings.size());
    for(auto iter = siblings.begin(); iter != siblings.end(); ++iter)
        positions.push_back(gametree.getNodePosition(*iter));
    sort(positions.begin(), positions.end());
    for(unsigned i = 0; i < (positions.size()-1); ++i)
      ASSERT_NE(positions[i], positions[i+1]);

    if (maxchild < 0)
      maxchild = expandedchild;

    ASSERT_FALSE(selectnode == expandedchild);
    mcst.backpropagation(expandedchild, -1, gametree);

    EXPECT_EQ(0, selectnode);
  }
  selectnode = mcst.selection(initempty, gametree);
  EXPECT_EQ(maxchild, selectnode);

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

  std::shared_ptr<bool> emptycurrent;
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
    std::shared_ptr<bool> emptyindicators = std::shared_ptr<bool>(
        new bool[board.getSizeOfVertices()], default_delete<bool[]>());
    copy(emptycurrent.get(), emptycurrent.get() + board.getSizeOfVertices(),
         emptyindicators.get());

    int proportionofempty = currentempty;

    selectnode = mcst.selection(proportionofempty, gametree);
    int expandedchild = mcst.expansion(selectnode, emptyindicators,
                                       proportionofempty, babywatsons,
                                       opponents, gametree);

    for(auto iter = babywatsons.begin(); iter != babywatsons.end(); ++iter)
        ASSERT_FALSE(emptyindicators.get()[*iter-1]);

    for(auto iter = opponents.begin(); iter != opponents.end(); ++iter)
          ASSERT_FALSE(emptyindicators.get()[*iter-1]);

    vector<size_t> siblings = gametree.getSiblings(expandedchild);
    vector<size_t> positions;
    positions.reserve(siblings.size());
    for(auto iter = siblings.begin(); iter != siblings.end(); ++iter)
        positions.push_back(gametree.getNodePosition(*iter));
    sort(positions.begin(), positions.end());
    for(unsigned i = 0; i < (positions.size()-1); ++i)
      ASSERT_NE(positions[i], positions[i+1]);

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
    std::shared_ptr<bool> emptyindicators = std::shared_ptr<bool>(
        new bool[board.getSizeOfVertices()], default_delete<bool[]>());
    copy(emptyinit.get(), emptyinit.get() + board.getSizeOfVertices(),
         emptyindicators.get());

    int proportionofempty = currentempty;

    selectnode = mcst.selection(proportionofempty, gametree);
    depth = gametree.getNodeDepth(selectnode);
    int expandedchild = mcst.expansion(selectnode, emptyindicators,
                                       proportionofempty, babywatsons,
                                       opponents, gametree);

    for(auto iter = babywatsons.begin(); iter != babywatsons.end(); ++iter)
        ASSERT_FALSE(emptyindicators.get()[*iter-1]);

    for(auto iter = opponents.begin(); iter != opponents.end(); ++iter)
          ASSERT_FALSE(emptyindicators.get()[*iter-1]);

    vector<size_t> siblings = gametree.getSiblings(expandedchild);
    vector<size_t> positions;
    positions.reserve(siblings.size());
    for(auto iter = siblings.begin(); iter != siblings.end(); ++iter)
        positions.push_back(gametree.getNodePosition(*iter));
    sort(positions.begin(), positions.end());
    for(unsigned i = 0; i < (positions.size()-1); ++i)
      ASSERT_NE(positions[i], positions[i+1]);

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
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
  GameTree gametree(playerb.getViewLabel());
  MonteCarloTreeSearch mcst(&board, &playerb);

  int currentempty = board.getNumofemptyhexgons();
  size_t numberoftrials = 3000;

  std::shared_ptr<bool> emptyglobal;
  vector<int> bwglobal, oppglobal;
  mcst.initGameState(emptyglobal, bwglobal, oppglobal);

  //checking from the beginning of the game
  for (size_t i = 0; i < numberoftrials; ++i) {
    //initialize the following containers to the current progress of playing board
    int proportionofempty = currentempty;
    vector<int> babywatsons(bwglobal), opponents(oppglobal);
    std::shared_ptr<bool> emptyindicators = std::shared_ptr<bool>(
        new bool[board.getSizeOfVertices()], default_delete<bool[]>());
    copy(emptyglobal.get(), emptyglobal.get() + board.getSizeOfVertices(),
         emptyindicators.get());

    //in-tree phase
    int selectnode = mcst.selection(currentempty, gametree);
    int expandednode = mcst.expansion(selectnode, emptyindicators,
                                      proportionofempty, babywatsons, opponents,
                                      gametree);
    //check the newly expanded node has duplicated position in board
    vector<size_t> siblings = gametree.getSiblings(expandednode);
    vector<size_t> positions;
    positions.reserve(siblings.size());
    for(auto iter = siblings.begin(); iter != siblings.end(); ++iter)
        positions.push_back(gametree.getNodePosition(*iter));
    sort(positions.begin(), positions.end());
    for(unsigned i = 0; i < (positions.size()-1); ++i)
      ASSERT_NE(positions[i], positions[i+1]);

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
    sum += gametree.getNodeValueFeature(i, 0);
    float value = static_cast<float>(gametree.getNodeValueFeature(i, 1))
        / static_cast<float>(gametree.getNodeValueFeature(i, 0));
    if(max < value){
      max = value;
      indexofmax = i;
    }
  }
  EXPECT_EQ(sum, numberoftrials);
  EXPECT_EQ(resultmove, gametree.getNodePosition(indexofmax));
}
TEST_F(MinMaxTest,CheckHexFiveGame) {
  int numofhexgon = 7;
  HexBoard board(numofhexgon);
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
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

    hexboardgame.setMove(playera, redrow, redcol);

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(mcstblue);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playerb, bluerow, bluecol);

    cout << "simulation " << round << " : " << redmove << " " << bluemove
         << endl;
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
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
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

    hexboardgame.setMove(playera, redrow, redcol);

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(mcstblue);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playerb, bluerow, bluecol);

    cout << "simulation " << round << " : " << redmove << " " << bluemove
         << endl;
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
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
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

    hexboardgame.setMove(playera, redrow, redcol);

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(mcstblue);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playerb, bluerow, bluecol);

    cout << "simulation " << round << " : " << redmove << " " << bluemove
         << endl;
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
  Player playera(board, hexgonValKind::RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind::BLUE);  //west to east, 'X'
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

    hexboardgame.setMove(playera, redrow, redcol);

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(mcstblue);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playerb, bluerow, bluecol);

    cout << "simulation " << round << " : " << redmove << " " << bluemove
         << endl;
    round++;
    ASSERT_NE(redmove, bluemove);
    //cout << hexboardgame.showView(playera, playerb);
    winner = hexboardgame.getWinner(playera, playerb);
  }
  cout << "winner is " << winner << endl;
}
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
