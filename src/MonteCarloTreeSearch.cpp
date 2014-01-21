/*
 * MonteCarloTreeSearch.cpp
 *
 */
#include "MonteCarloTreeSearch.h"

using namespace std;
using namespace boost;

MonteCarloTreeSearch::MonteCarloTreeSearch(const HexBoard* board,
                                           const Player* aiplayer,
                                           GameTree& gametree)
    : MonteCarloTreeSearch(board, aiplayer, gametree, 3000) {
}
MonteCarloTreeSearch::MonteCarloTreeSearch(const HexBoard* board,
                                           const Player* aiplayer,
                                           GameTree& gametree,
                                           size_t numberoftrials)
    : AbstractStrategyImpl(board, aiplayer),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(numberoftrials),
      gametree(gametree) {
  numofhexgons = ptrtoboard->getNumofhexgons();
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

  int currentempty = ptrtoboard->getNumofemptyhexgons();
  int proportionofempty = currentempty;

  //re-roote the move made by real opponent
  //gametree.reRootbyPosition(oppglobal.back());

  for (size_t i = 0; i < numberoftrials; ++i) {
    //initialize the following containers to the current progress of playing board
    vector<int> babywatsons(bwglobal), opponents(oppglobal);
    std::shared_ptr<bool> emptyindicators = std::shared_ptr<bool>(
        new bool[ptrtoboard->getSizeOfVertices()], default_delete<bool[]>());
    copy(emptyglobal.get(), emptyglobal.get() + ptrtoboard->getSizeOfVertices(),
         emptyindicators.get());

    //in-tree phase
    int selectnode = selection(currentempty);
    int expandednode = expansion(selectnode, emptyindicators, proportionofempty,
                                 babywatsons, opponents);

    //simulation phase
    int winner = playout(emptyindicators, proportionofempty, bwglobal,
                         oppglobal);
    //back-propagate
    backpropagation(expandednode, winner);
  }
  int resultmove = getBestMove();

  //find the move with the maximal successful simulated outcome
  assert(resultmove != -1);

  return resultmove;
}
//in-tree phase
int MonteCarloTreeSearch::selection(int currentempty) {
  //when the board is not empty, return the node with the highest expected value
  int expectleaf = gametree.selectMaxBalanceNode(currentempty);
  return expectleaf;
}
int MonteCarloTreeSearch::expansion(int selectnode,
                                    std::shared_ptr<bool>& emptyindicators,
                                    int& portionofempty,
                                    vector<int>& babywatsons,
                                    vector<int>& opponents) {
  /*assert(
   opponents.size() - babywatsons.size() == 0
   || abs(opponents.size() - babywatsons.size()) == 1);*/

  gametree.getMovesfromTreeState(selectnode, babywatsons, opponents,
                                 ptrtoplayer->getViewLabel());

  for (unsigned i = 0; i < opponents.size(); ++i)
    emptyindicators.get()[opponents[i] - 1] = false;

  for (unsigned i = 0; i < opponents.size(); ++i)
    emptyindicators.get()[babywatsons[i] - 1] = false;

  int indexofchild = -1;
  //if (babywatsons.size() > 0 && opponents.size() == babywatsons.size())  //the state of selected game is the end of game
  //  indexofchild = selectnode;
  //else {
  int move = genNextRandom(emptyindicators, portionofempty);
  //if expanding from root
  indexofchild = gametree.expandNode(selectnode, move);
  if (babywatsons.size() > opponents.size())
    opponents.push_back(move);
  else
    babywatsons.push_back(move);
//}
  return indexofchild;
}
//play-out phase
int MonteCarloTreeSearch::playout(std::shared_ptr<bool>& emptyindicators,
                                  int& portionofempty, vector<int>& babywatsons,
                                  vector<int>& opponents) {
//start the simulation

  while (portionofempty > 0) {
    //random generate a move for baby watson
    int move = genNextRandom(emptyindicators, portionofempty);
    babywatsons.push_back(move);

    //random generate a move for virtual opponent
    int oppmove = genNextRandom(emptyindicators, portionofempty);
    opponents.push_back(oppmove);
  }

  int winner = checkWinnerExist(babywatsons, opponents);
  assert(winner != 0);

  return winner;
}
int MonteCarloTreeSearch::getBestMove() {
  pair<int, double> result = gametree.getBestMovefromSimulation();
  int bestmove = gametree.getNodePosition(result.first);
  assert(bestmove != -1);
//re-root and prune the not winning moves for now
  //gametree.reRootfromSimulation(result.first, true);
  return bestmove;
}
void MonteCarloTreeSearch::backpropagation(int backupnode, int winner) {
  int value = gametree.updateNodefromSimulation(backupnode, winner);
  gametree.backpropagatefromSimulation(backupnode, value);
}
