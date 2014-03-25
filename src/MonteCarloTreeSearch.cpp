/*
 * MonteCarloTreeSearch.cpp
 *
 */

#include "Global.h"
#include "GameTree.h"
#include "MonteCarloTreeSearch.h"

#include <algorithm>

using namespace std;
using namespace boost;

#if __cplusplus > 199711L
MonteCarloTreeSearch::MonteCarloTreeSearch(const HexBoard* board,
                                           const Player* aiplayer)
    : MonteCarloTreeSearch(board, aiplayer, 2048) {
}
#else
class Util {
private:
  hexgame::shared_ptr<bool>& emptyindicators;
public:
  Util(hexgame::shared_ptr<bool>& indicators):emptyindicators(indicators) {}
  void operator()(int i) {
    if(i!=0)
    emptyindicators.get()[i-1] = false;
  }
};
struct Indicator {
  bool operator()(int i) {
    return i==0;
  }
};
MonteCarloTreeSearch::MonteCarloTreeSearch(const HexBoard* board,
    const Player* aiplayer)
: AbstractStrategyImpl(board, aiplayer),
ptrtoboard(board),
ptrtoplayer(aiplayer),
numberoftrials(2048) {
  init();
}
#endif
MonteCarloTreeSearch::MonteCarloTreeSearch(const HexBoard* board,
                                           const Player* aiplayer,
                                           size_t numberoftrials)
    : AbstractStrategyImpl(board, aiplayer),
      ptrtoboard(board),
      ptrtoplayer(aiplayer),
      numberoftrials(numberoftrials) {
  init();
}
int MonteCarloTreeSearch::simulation(int currentempty) {
  hexgame::shared_ptr<bool> emptyglobal;
  vector<int> bwglobal, oppglobal;
  initGameState(emptyglobal, bwglobal, oppglobal);
  GameTree gametree(ptrtoplayer->getViewLabel());
  for (size_t i = 0; i < numberoftrials; ++i) {
    //initialize the following containers to the current progress of playing board
    vector<int> babywatsons(bwglobal), opponents(oppglobal);
    hexgame::shared_ptr<bool> emptyindicators = hexgame::shared_ptr<bool>(
        new bool[ptrtoboard->getSizeOfVertices()],
        hexgame::default_delete<bool[]>());
    copy(emptyglobal.get(), emptyglobal.get() + ptrtoboard->getSizeOfVertices(),
         emptyindicators.get());
    int proportionofempty = currentempty;

    //in-tree phase
    pair<int,int> selecresult = selection(currentempty, gametree);
    int expandednode = expansion(selecresult, emptyindicators, proportionofempty,
                                 babywatsons, opponents, gametree);
    //simulation phase
    int winner = playout(emptyindicators, proportionofempty, babywatsons,
                         opponents);
    assert(winner != 0);
    //back-propagate
    backpropagation(expandednode, winner, gametree);
  }
  int resultmove = getBestMove(gametree);
  //find the move with the maximal successful simulated outcome
  assert(resultmove != -1);
  return resultmove;
}
//in-tree phase
pair<int,int> MonteCarloTreeSearch::selection(int currentempty, AbstractGameTree& gametree) {
  //when the board is not empty, return the node with the highest expected value
  return gametree.selectMaxBalanceNode(currentempty, true);
}
int MonteCarloTreeSearch::expansion(pair<int,int> selectresult,
                                    hexgame::shared_ptr<bool>& emptyindicators,
                                    int& portionofempty,
                                    vector<int>& babywatsons,
                                    vector<int>& opponents,
                                    AbstractGameTree& gametree) {
  int indexofchild = selectresult.first;

  if (selectresult.second != portionofempty)  //the selected node is the end of game which might be root (not empty cell for move) or any leaf which cannot be expanded no more
      indexofchild = gametree.expandNode(selectresult.first, 0, 'W');

  hexgame::unordered_set<int> remainingmoves;
  for (int i = 0; i < ptrtoboard->getSizeOfVertices(); ++i) {
    if (emptyindicators.get()[i])
      remainingmoves.insert(i + 1);
  }
  assert(static_cast<int>(remainingmoves.size()) == portionofempty);
  gametree.getMovesfromTreeState(indexofchild, babywatsons, opponents, remainingmoves);

  //align empty indicators with state of game tree node
#if __cplusplus > 199711L
  for_each(babywatsons.begin(), babywatsons.end(),
           [&](int i) {if(i!=0) emptyindicators.get()[i-1] = false;});
  for_each(opponents.begin(), opponents.end(),
           [&](int i) {if(i!=0) emptyindicators.get()[i-1] = false;});
#else
  Util functor(emptyindicators);
  for_each(babywatsons.begin(), babywatsons.end(), functor);
  for_each(opponents.begin(), opponents.end(), functor);
#endif

  portionofempty = ptrtoboard->getSizeOfVertices()
      - (babywatsons.size() + opponents.size());

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
int MonteCarloTreeSearch::playout(hexgame::shared_ptr<bool>& emptyindicators,
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
int MonteCarloTreeSearch::getBestMove(AbstractGameTree& gametree) {
  pair<int, double> result = gametree.getBestMovefromSimulation();
  int bestmove = gametree.getNodePosition(result.first);
  assert(bestmove != -1);
  return bestmove;
}
int MonteCarloTreeSearch::backpropagation(int backupnode, int winner,
                                           AbstractGameTree& gametree) {
  int value = gametree.updateNodefromSimulation(backupnode, winner, -1);
  return value;
}
void MonteCarloTreeSearch::init() {
  numofhexgons = ptrtoboard->getNumofhexgons();
  babywatsoncolor = 'B', oppoenetcolor = 'R';
  if (babywatsoncolor != ptrtoplayer->getViewLabel()) {
    oppoenetcolor = babywatsoncolor;
    babywatsoncolor = ptrtoplayer->getViewLabel();
  }
}
