/*
 * MonteCarloTreeSearch.cpp
 *
 */

#include "Global.h"
#include "MonteCarloTreeSearch.h"

#include <algorithm>

using namespace std;
using namespace boost;

#if __cplusplus > 199711L
MonteCarloTreeSearch::MonteCarloTreeSearch(const HexBoard* board,
                                           const Player* aiplayer)
    : MonteCarloTreeSearch(board, aiplayer, 3000) {
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
numberoftrials(3000) {
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
    int selectnode = selection(currentempty, gametree);
    int expandednode = expansion(selectnode, emptyindicators, proportionofempty,
                                 babywatsons, opponents, gametree);

#ifdef NDEBUG //temporary turn off
//#ifndef NDEBUG
    DEBUGHEADER();
    cerr
    << "[selectnode|expandednode|currentempty|proportionofempty|size of babywatsons|size of opponents] "
    << selectnode << "|" << expandednode << "|" << currentempty << "|"
    << proportionofempty << "|" << babywatsons.size() << "|"
    << opponents.size() << endl;
    cerr << "babywatsons: ";
    for_each(babywatsons.begin(), babywatsons.end(),
        [](int i) {cerr << i << " ";});
    cerr << endl;
    cerr << "opponents: ";
    for_each(opponents.begin(), opponents.end(), [](int i) {cerr << i << " ";});
    cerr << endl;
#endif

    //simulation phase
    int winner = playout(emptyindicators, proportionofempty, babywatsons,
                         opponents);
    assert(winner != 0);
    //back-propagate
    backpropagation(expandednode, winner, gametree);
#ifdef NDEBUG //temporary turn off
//#ifndef NDEBUG
    DEBUGHEADER();
    cerr << gametree.printGameTree(0);
#endif
  }
  int resultmove = getBestMove(gametree);
  //find the move with the maximal successful simulated outcome
  assert(resultmove != -1);
  return resultmove;
}
//in-tree phase
int MonteCarloTreeSearch::selection(int currentempty, GameTree& gametree) {
  //when the board is not empty, return the node with the highest expected value
  return gametree.selectMaxBalanceNode(currentempty);
}
int MonteCarloTreeSearch::expansion(int selectnode,
                                    hexgame::shared_ptr<bool>& emptyindicators,
                                    int& portionofempty,
                                    vector<int>& babywatsons,
                                    vector<int>& opponents,
                                    GameTree& gametree) {
  int indexofchild = selectnode;

  if (static_cast<int>(gametree.getNodeDepth(selectnode)) != portionofempty)  //the selected node is the end of game which might be root (not empty cell for move) or any leaf which cannot be expanded no more
      indexofchild = gametree.expandNode(selectnode, 0);

  gametree.getMovesfromTreeState(indexofchild, babywatsons, opponents);

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

  if (indexofchild != selectnode) {  //expanding from the selected node
    hexgame::unordered_set<int> remainingmoves;
    for (int i = 0; i < ptrtoboard->getSizeOfVertices(); ++i) {
      if (emptyindicators.get()[i])
        remainingmoves.insert(i + 1);
    }

    vector<size_t> siblings = gametree.getSiblings(indexofchild);
#if __cplusplus > 199711L
    for (auto iter = siblings.begin(); iter != siblings.end(); ++iter) {
      size_t pos = gametree.getNodePosition(*iter);
      if (emptyindicators.get()[pos - 1])
        remainingmoves.erase(pos);

    }
#else
    for (vector<size_t>::iterator iter = siblings.begin(); iter != siblings.end(); ++iter) {
      size_t pos = gametree.getNodePosition(*iter);
      if (emptyindicators.get()[pos - 1])
      remainingmoves.erase(pos);
    }
#endif
#ifdef NDEBUG
    srand((unsigned long)clock());
#endif
    size_t index = rand() % remainingmoves.size();
    hexgame::unordered_set<int>::iterator iter = remainingmoves.begin();
    for (size_t i = 0; i < index; ++i)
      ++iter;

    int move = *iter;
    assert(remainingmoves.count(move));
#if __cplusplus > 199711L
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
#else
    Indicator functor;
    vector<int>::iterator iterofbw = find_if(babywatsons.begin(), babywatsons.end(),functor);
    if (iterofbw != babywatsons.end())
    *iterofbw = move;
    else {
      vector<int>::iterator iterofop = find_if(opponents.begin(), opponents.end(),functor);
      assert(iterofop != opponents.end());
      *iterofop = move;
    }
#endif
    gametree.setNodePosition(indexofchild, move);
    emptyindicators.get()[move - 1] = false;
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
int MonteCarloTreeSearch::getBestMove(GameTree& gametree) {
  pair<int, double> result = gametree.getBestMovefromSimulation();
  int bestmove = gametree.getNodePosition(result.first);
  assert(bestmove != -1);
  return bestmove;
}
void MonteCarloTreeSearch::backpropagation(int backupnode, int winner,
                                           GameTree& gametree) {
  int value = gametree.updateNodefromSimulation(backupnode, winner);
  gametree.backpropagatefromSimulation(backupnode, value);
}
void MonteCarloTreeSearch::init() {
  numofhexgons = ptrtoboard->getNumofhexgons();
  babywatsoncolor = 'B', oppoenetcolor = 'R';
  if (babywatsoncolor != ptrtoplayer->getViewLabel()) {
    oppoenetcolor = babywatsoncolor;
    babywatsoncolor = ptrtoplayer->getViewLabel();
  }
}
