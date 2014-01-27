/*
 * MonteCarloTreeSearch.cpp
 *
 */

#include "global.h"
#include "MonteCarloTreeSearch.h"
#include <algorithm>

using namespace std;
using namespace boost;

MonteCarloTreeSearch::MonteCarloTreeSearch(const HexBoard* board,
                                           const Player* aiplayer)
    : MonteCarloTreeSearch(board, aiplayer, 3000) {
}
MonteCarloTreeSearch::MonteCarloTreeSearch(const HexBoard* board,
                                           const Player* aiplayer,
                                           size_t numberoftrials)
    : AbstractStrategyImpl(board, aiplayer),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(numberoftrials) {
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
  GameTree gametree(ptrtoplayer->getViewLabel());

  for (size_t i = 0; i < numberoftrials; ++i) {
    //initialize the following containers to the current progress of playing board
    vector<int> babywatsons(bwglobal), opponents(oppglobal);
    std::shared_ptr<bool> emptyindicators = std::shared_ptr<bool>(
        new bool[ptrtoboard->getSizeOfVertices()], default_delete<bool[]>());
    copy(emptyglobal.get(), emptyglobal.get() + ptrtoboard->getSizeOfVertices(),
         emptyindicators.get());

    int proportionofempty = currentempty;

    //in-tree phase
    int selectnode = selection(currentempty, gametree);
    int expandednode = expansion(selectnode, emptyindicators, proportionofempty,
                                 babywatsons, opponents, gametree);
    //simulation phase
    int winner = playout(emptyindicators, proportionofempty, babywatsons,
                         opponents);


    //back-propagate
    backpropagation(expandednode, winner, gametree);
  }
  int resultmove = getBestMove(gametree);

  //find the move with the maximal successful simulated outcome
  assert(resultmove != -1);
  return resultmove;
}
//in-tree phase
int MonteCarloTreeSearch::selection(int currentempty, GameTree& gametree) {
  //when the board is not empty, return the node with the highest expected value
  int expectleaf = gametree.selectMaxBalanceNode(currentempty);
  return expectleaf;
}
int MonteCarloTreeSearch::expansion(int selectnode,
                                    std::shared_ptr<bool>& emptyindicators,
                                    int& portionofempty,
                                    vector<int>& babywatsons,
                                    vector<int>& opponents,
                                    GameTree& gametree) {

  int indexofchild = selectnode;
  if (static_cast<int>(gametree.getNodeDepth(selectnode)) != portionofempty)  //the selected node is the end of game
    indexofchild = gametree.expandNode(selectnode, 0);

  gametree.getMovesfromTreeState(indexofchild, babywatsons, opponents);

  //align empty indicators with state of game tree node
  for_each(babywatsons.begin(), babywatsons.end(),
           [&](int i) {if(i!=0) emptyindicators.get()[i-1] = false;});
  for_each(opponents.begin(), opponents.end(),
           [&](int i) {if(i!=0) emptyindicators.get()[i-1] = false;});

  portionofempty = ptrtoboard->getSizeOfVertices()
      - (babywatsons.size() + opponents.size());

  if (indexofchild != selectnode) {  //expanding from the selected node
    ++portionofempty;
    int move = genNextRandom(emptyindicators, portionofempty);
    auto iterofbw = find_if(babywatsons.begin(), babywatsons.end(),
                        [](int i) {return i==0;});
    if (iterofbw != babywatsons.end())
      *iterofbw = move;
    else {
      auto iterofop = find_if(opponents.begin(), opponents.end(),
                     [](int i) {return i==0;});
      assert(iterofop != opponents.end());
      *iterofop = move;
    }
    gametree.setNodePosition(indexofchild, move);
  }
  assert(count(babywatsons.begin(), babywatsons.end(), 0) == 0);
  assert(count(opponents.begin(), opponents.end(), 0) == 0);
  assert(
      portionofempty
          == count(emptyindicators.get(),
                   emptyindicators.get() + ptrtoboard->getSizeOfVertices(),
                   true));

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
int MonteCarloTreeSearch::getBestMove(GameTree& gametree) {
  pair<int, double> result = gametree.getBestMovefromSimulation();
  int bestmove = gametree.getNodePosition(result.first);
  assert(bestmove != -1);
//re-root and prune the not winning moves for now
  //gametree.reRootfromSimulation(result.first, true);
  return bestmove;
}
void MonteCarloTreeSearch::backpropagation(int backupnode, int winner,
                                           GameTree& gametree) {
  int value = gametree.updateNodefromSimulation(backupnode, winner);
  gametree.backpropagatefromSimulation(backupnode, value);
}
