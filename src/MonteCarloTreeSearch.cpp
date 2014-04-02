/*
 * MonteCarloTreeSearch.cpp
 * This file declares a Mont Carlo Tree Search implementation for AI player
 *   Created on: Dec 13, 2013
 *      Author: renewang
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
///Overwritten simulation method. See AbstractStrategy.
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
///The first phase in MCTS. Selection phase is according to UTC Policy and maximizing winning rate in play-out phase
///@param currentempty is the current empty hexgons or positions left in the actual game state which will be the number of children nodes of root of game tree
///@param gametree is a game tree object which stores the simulation progress and result
///@return a pair of integers; the first is the index of selected node of game tree (Notice: not the index of position on hex board which is bound by the size of hex board) and the second is the depth or level of selected node on game tree.
pair<int,int> MonteCarloTreeSearch::selection(int currentempty, AbstractGameTree& gametree) {
  //when the board is not empty, return the node with the highest expected value
  return gametree.selectMaxBalanceNode(currentempty, true);
}
///The second phase in MCTS. Expansion phase will take the result from selection phase and return the index of expanded node. Several required containers are also processed according to the game state of the selected tree node
///@param selectresult is the pair of integers returned by selection method
///@param emptyindicators stores indicator of a position on the hex board is empty or not which will be modified when a simulated game progresses
///@param portionofempty is the number of empty hexgons left in the current actual game state which will be modified during simulated games
///@param babywatsons is the moves made by AI player in the current actual game state and will serve as a container for moves made in simulated games
///@param opponents is the moves made by human player in the current actual game state and will serve as a container for moves made in simulated games
///@param gametree is a game tree object which stores the simulation progress and result
///@return the index of expanded child if the tree is still expandable or the index of selected node if the tree is unable to be expanded (reach the end of game)
///<br/>Sample Usage:<br/>
///
/// GameTree gametree(ptrtoplayer->getViewLabel());<br/>
/// hexgame::shared_ptr<bool> emptyinit;<br/>
/// vector<int> bwinit, oppinit;<br/>
/// initGameState(emptyinit, bwinit, oppinit); //restore the current game progress<br/>
/// vector<int> babywatsons(bwinit), opponents(oppinit);<br/>
/// hexgame::shared_ptr<bool> emptyindicators = hexgame::shared_ptr<bool>(new bool[ptrtoboard->getSizeOfVertices()], hexgame::default_delete<bool[]>());<br/>
/// copy(emptyinit.get(), emptyinit.get() + board.getSizeOfVertices(), emptyindicators.get()); <br/>
/// int proportionofempty = ptrboard->getNumofemptyhexgons();<br/>
/// pair<int, int>selectresult = selection(proportionofempty, gametree); <br/>
/// int expandedchild = expansion(selectresult, emptyindicators, proportionofempty, babywatsons, opponents, gametree); <br/>
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
///The third phase in MCTS. The play-out phase will take the containers processed in expansion phase and start simulation by self-playing. The play result will back-propagate update in game tree
///@param emptyindicators is the number of empty hexgons left in the current actual game state which will be modified during simulated games
///@param portionofempty is the number of empty hexgons left in the current actual game state which will be modified during simulated games
///@param babywatsons is the moves made by AI player in the current actual game state
///@param opponents is the moves made by human player in the current actual game state
///@return an integer indicates 0: no winner, -1, babywatson loses and 1 babywatson wins
//[Notice]: All parameters used in playout need to be processed by expansion firstly
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
///The fourth and last phase in MCTS. The backpropagation phase will take the simulated result from play-out phase and expanded node from expansion phase
///@param expandednode is the node expanded at the expansioni phase
///@param winner is the play result of play-out phase.
///@param gametree is a game tree object which stores the simulation progress and result
///@return the resulting value updated by game tree
void MonteCarloTreeSearch::backpropagation(int expandednode, int winner,
                                           AbstractGameTree& gametree) {
  gametree.updateNodefromSimulation(expandednode, winner, -1);
}
///Get the best move according to estimation result from game tree
///@param gametree is a game tree object which stores the simulation progress and result
///@return the best move estimated by gametree which will be passed to genMove
int MonteCarloTreeSearch::getBestMove(AbstractGameTree& gametree) {
  pair<int, double> result = gametree.getBestMovefromSimulation();
  int bestmove = gametree.getNodePosition(result.first);
  assert(bestmove != -1);
  return bestmove;
}
///initialize babywatsoncolor and oppoenetcolor
///@param NONE
///@return NONE
void MonteCarloTreeSearch::init() {
  numofhexgons = ptrtoboard->getNumofhexgons();
  babywatsoncolor = 'B', oppoenetcolor = 'R';
  if (babywatsoncolor != ptrtoplayer->getViewLabel()) {
    oppoenetcolor = babywatsoncolor;
    babywatsoncolor = ptrtoplayer->getViewLabel();
  }
}
