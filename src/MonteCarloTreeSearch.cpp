/*
 * MonteCarloTreeSearch.cpp
 *
 */
#include "PriorityQueue.h"
#include "MonteCarloTreeSearch.h"

using namespace std;
using namespace boost;

MonteCarloTreeSearch::MonteCarloTreeSearch(const HexBoard* board,
                                           const Player* aiplayer)
    : AbstractStrategyImpl(board, aiplayer),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(3000),
      numofchildren(1) {
  numofhexgons = ptrtoboard->getNumofhexgons();
  registerGameTree(1);
  babywatsoncolor = 'B', oppoenetcolor = 'R';
  if (babywatsoncolor != ptrtoplayer->getViewLabel()) {
    oppoenetcolor = babywatsoncolor;
    babywatsoncolor = ptrtoplayer->getViewLabel();
  }
}
int MonteCarloTreeSearch::simulation() {

  std::shared_ptr<bool> emptyglobal;
  vector<int> bwglobal, oppglobal;
  initGameState(emptyglobal, bwglobal, oppglobal);

  //prune the subtree rooted at opponent's move
  for (size_t j = 0; j < oppglobal.size(); j++) {
    for (size_t k = 0; k < treeholder.size(); k++)
      treeholder.at(k).prunebyPosition(oppglobal[j]);
  }

  int currentempty = ptrtoboard->getNumofemptyhexgons();

  //in-tree phase
  vector<string> leavesforexpanded(treeholder.size());
  selection(leavesforexpanded, currentempty);
  expansion(leavesforexpanded, numofchildren, emptyglobal, currentempty);
  //simulation phase
  playout(emptyglobal, currentempty);
  //back-propagate phase
  backpropagate();

  int resultmove = getBestMove();

  //find the move with the maximal successful simulated outcome
  assert(resultmove != -1);

  //re-root and prune the not winning moves for now
  for (size_t i = 0; i < treeholder.size(); ++i)
    treeholder.at(i).reRootbyPosition(resultmove, true);

  return resultmove;
}
void MonteCarloTreeSearch::registerGameTree(size_t numofgametree) {
  for (size_t i = 0; i < numofgametree; i++) {
    GameTree tree(0);
    treeholder.push_back(tree);
  }
}
//in-tree phase
void MonteCarloTreeSearch::selection(vector<string>& leavesforexpanded,
                                     int currentempty) {
  //when the board is empty, return the root
  if (currentempty == ptrtoboard->getSizeOfVertices())
    fill(leavesforexpanded.begin(), leavesforexpanded.end(),
         string("00000000"));  //TODO, modify
  else
    //when the board is not empty, return the node with the highest expected value
    for (size_t i = 0; i < treeholder.size(); i++)
      leavesforexpanded[i] = treeholder[i].calcUpperConfidenceBound();
}
void MonteCarloTreeSearch::expansion(vector<string>& leavesforexpanded,
                                     size_t numofchildren,
                                     std::shared_ptr<bool>& emptyindicators,
                                     int& portionofempty) {
  for (size_t i = 0; i < treeholder.size(); i++) {
    for (size_t j = 0; j < numofchildren; j++) {
      int move = genNextRandom(emptyindicators, portionofempty);
      //if expanding from root
      if (leavesforexpanded[i] == "00000000")
        treeholder[i].expandLeaf(leavesforexpanded[i], move,
                                 ptrtoplayer->getViewLabel());
      else
        treeholder[i].expandLeaf(leavesforexpanded[i], move);
      portionofempty--;
    }
  }
}
//play-out phase
void MonteCarloTreeSearch::playout(std::shared_ptr<bool>& emptyindicators,
                                   int portionofempty) {
  //start the simulation
  int nextmove = -1;
  for (int i = 0; i < numberoftrials; i++) {

    while (portionofempty > 0) {
      //random generate a move for baby watson
      int move, oppmove;
      move = genNextRandom(emptyindicators, portionofempty);

      //update game tree
      if (treeholder.size() == numberoftrials)
        treeholder.at(i).addSimulatedMove(move, babywatsoncolor);
      else
        treeholder.at(0).addSimulatedMove(move, babywatsoncolor);

      portionofempty--;

      //random generate a move for virtual opponent
      oppmove = genNextRandom(emptyindicators, portionofempty);
      //update game tree
      if (treeholder.size() == numberoftrials)
        treeholder.at(i).addSimulatedMove(move, oppoenetcolor);
      else
        treeholder.at(0).addSimulatedMove(move, oppoenetcolor);

      portionofempty--;
    }
    pair<vector<int>, vector<int> > result;
    vector<int>* babywatsons, *opponents;
    if (treeholder.size() == numberoftrials)
      result = treeholder.at(i).getMovesfromSimulation();
    else
      result = treeholder.at(0).getMovesfromSimulation();

    if (ptrtoplayer->getViewLabel() == 'R') {
      babywatsons = &(result.first);
      opponents = &(result.second);
    } else {
      babywatsons = &(result.second);
      opponents = &(result.first);
    }

    int winner = checkWinnerExist(*babywatsons, *opponents);
    assert(winner != 0);

    //update the winner information
    if (treeholder.size() == numberoftrials)
      treeholder.at(i).updateNodefromSimulation(winner);
    else
      treeholder.at(0).updateNodefromSimulation(winner);
  }
}
//update
void MonteCarloTreeSearch::backpropagate() {
  //update the winner information
  for (size_t i = 0; i < treeholder.size(); ++i)
    treeholder.at(i).backpropogatefromSimulation();
}
int MonteCarloTreeSearch::getBestMove() {
  int bestmove = -1;
  if (treeholder.size() > 1) {
    PriorityQueue<size_t, double> treecomparor(treeholder.size());
    for (size_t i = 0; i < treeholder.size(); ++i)
      treecomparor.insert(i, -treeholder[i].getBestMoveValuefromSimulation());
    size_t besttree = treecomparor.minPrioirty();
    bestmove = treeholder[besttree].getBestMovefromSimulation();
  } else
    bestmove = treeholder[0].getBestMovefromSimulation();
  assert(bestmove != -1);
  return bestmove;
}
