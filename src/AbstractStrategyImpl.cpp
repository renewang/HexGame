/*
 * AbstractStrategyImpl.cpp
 * This file file define the default implementation used for AbstractStrategy interface
 *  Created on:
 *      Author: renewang
 */

#include <cstdlib>

#include "Game.h"
#include "Strategy.h"
#include "MonteCarloTreeSearch.h"
#include "AbstractStrategyImpl.h"
#include "MultiMonteCarloTreeSearch.h"

using namespace std;

///genMove called by Game object in order to generate move via self-play simulation
///@param NONE
///@return: the next move evaluated by self-play simulation
int AbstractStrategyImpl::genMove() {
  if(ptrtoboard->getNumofemptyhexgons() > 0)
    return(simulation(ptrtoboard->getNumofemptyhexgons()));
  else
    return -1; //there's no empty moves
}
///Check if the winner exists for this stage of simulation
///@param babywatsons: the proposed moves made by baby watson so far
///@param opponenets: the proposed moves made by virtual opponent so far
///@return an integer indicates 0: no winner, -1, babywatson loses and 1 babywatson wins
int AbstractStrategyImpl::checkWinnerExist(vector<int>& babywatsons,
                                           vector<int>& opponents) {
  if (isWinner(babywatsons, ptrtoplayer->getWestToEastCondition()))
    return 1;
  else if (isWinner(opponents, !ptrtoplayer->getWestToEastCondition()))
    return -1;
  else
    return 0;
}
///Check if the test vector satisfies the winning condition
///@param candidates: stores the moves
///@param iswestoeast: the boolean variable to indicate if the winning condition is west to east
///@return a boolean variable indicates if provided moves made by a player wins the simulated self-play game
bool AbstractStrategyImpl::isWinner(vector<int>& candidates, bool iswestoeast) {
  bool iswinner = false;

  vector<int> representative(candidates.size());
  vector<int> repsize(candidates.size(), 1);

  for (unsigned i = 0; i < candidates.size(); i++)
    representative[i] = i;

  //link the edge by scanning pairwise node
  for (unsigned i = 0; i < (candidates.size() - 1); i++) {
    for (unsigned j = i + 1; j < candidates.size(); j++) {
      //temporary for test adjacent
      if (ptrtoboard->isAdjacent(candidates[i], candidates[j])//TODO, performance improvement. another loop to access neighbors
          && representative[i] != representative[j]) {
        int smallgroup = representative[j];
        int largegroup =
            (repsize[representative[i]] > repsize[representative[j]]) ?
                representative[i] : representative[j];
        if (largegroup != representative[i])
          smallgroup = representative[i];

        representative[smallgroup] = largegroup;
        for (unsigned k = 0; k < candidates.size(); k++)//TODO, performance improvement. n^3 implementation here...
          if (representative[k] == smallgroup) {
            representative[k] = largegroup;
            repsize[smallgroup]--;
            repsize[largegroup]++;
          }
      }
    }
  }

  //find if there's a path satisfies the winning condition,
  //should start from the largest group
  for (unsigned i = 0; i < candidates.size(); i++) {
    bool touchoneside = false;
    bool touchotherside = false;
    for (unsigned j = 0; j < candidates.size(); j++)
      if (representative[j] == static_cast<int>(i)) {
        int row = (candidates[j] - 1) / numofhexgons;
        int col = (candidates[j] - 1) % numofhexgons;
        if ((iswestoeast && col == 0) || (!iswestoeast && row == 0))
          touchoneside = true;
        else if ((iswestoeast && col == (numofhexgons - 1))
            || (!iswestoeast && row == (numofhexgons - 1)))
          touchotherside = true;
      }
    if (touchoneside && touchotherside) {
      iswinner = (touchoneside && touchotherside);
      break;
    }
  }
  return iswinner;
}
///Generate the random next move representing with index [1, number of hexgon per side]
///@param emptyindicators is used to store the indicator of empty hexgon on hex board for current simulated game state
///@param proportionofempty is the proportion of empty hexgons left in the current simulated game process <br/>
///[Notice]: the value of proportionofempty will subtract one after calling genNextRandom
///@return the index or position of hexgon on hex board
int AbstractStrategyImpl::genNextRandom(hexgame::shared_ptr<bool>& emptyindicators,
                                        int& proportionofempty) {
  bool isoccupied = true;
  int index = -1;
#ifdef NDEBUG
  srand(static_cast<unsigned>(time(NULL)));
#endif

  while (isoccupied && proportionofempty > 0) {
    index = rand() % (ptrtoboard->getSizeOfVertices()) + 1;
    if (emptyindicators.get()[index - 1]) {
      isoccupied = false;
     emptyindicators.get()[index - 1] = false;
    }
  }
  proportionofempty--;
  return index;
}
///Initialize the following containers to the current progress of playing board
///@param emptyglobal is used to store the indicator of empty hexgon on hex board for current real game state
///@param bwglobal is used to store the moves made by AI player in current real game state
///@param oppglobal is used to store the moves made by real human player
///@return NONE
void AbstractStrategyImpl::initGameState(hexgame::shared_ptr<bool>& emptyglobal,vector<int>& bwglobal, vector<int>& oppglobal) {
  emptyglobal = ptrtoboard->getEmptyHexIndicators();
  if(ptrtoplayer->getViewLabel() == 'R'){
    bwglobal = ptrtoboard->getRedmoves();
    oppglobal = ptrtoboard->getBluemoves();
  }else{
    bwglobal = ptrtoboard->getBluemoves();
    oppglobal = ptrtoboard->getRedmoves();
  }
}
///Select AI strategy for AI player
///@param strategykind is the enum type which is used to indicate which AI strategy should be used for AI player
///@param watsonstrategy is uninitialized a strategy object wrapped by unique pointer which will be initialized within this function
///@param player is the reference of the player assigned with AI strategy
///@param board is the reference of the hex board
///@return NONE
///Sample Usage:<br/>
/// HexBoard board(numofhexgon); <br/>
/// Player playerfirst(board, hexgonValKind_BLUE);<br/>
/// Player* babywatson =  &playerfirst; <br/>
/// hexgame::unique_ptr<AbstractStrategy, hexgame::default_delete<AbstractStrategy> > watsonstrategy(nullptr);<br/>
/// ::selectStrategy(AIStrategyKind_MCTS, watsonstrategy, *babywatson, board);<br/>
void selectStrategy(AIStrategyKind strategykind,
                    hexgame::unique_ptr<AbstractStrategy, hexgame::default_delete<AbstractStrategy> >& watsonstrategy,
                    Player& player, HexBoard& board) {
  switch (strategykind) {
    case AIStrategyKind_NAIVE:
    std::cout << player.getPlayername()<<" uses Naive Monte Carlo strategy" << endl;
    watsonstrategy.reset(new Strategy(&board, &player));
    break;
    case AIStrategyKind_PMCTS:
    std::cout << player.getPlayername()<<" uses Parallel Monte Carlo Tree Search strategy" << endl;
    watsonstrategy.reset(new MultiMonteCarloTreeSearch(&board, &player));
    break;
    default:
    std::cout << player.getPlayername()<< " uses Monte Carlo Tree Search strategy" << endl;
    watsonstrategy.reset(new MonteCarloTreeSearch(&board, &player));
    break;
  }
}
///An automatic and simulated game played by two AI players
///@param strategyred is is uninitialized a strategy object for red player wrapped by unique pointer which will be initialized within this function
///@param strategyblue is uninitialized a strategy object for blue player wrapped by unique pointer which will be initialized within this function
///@param redstrategykind is the strategy used for red player
///@param bluestrategykind is the strategy used for blue player
///@param numofhexgon is the size of hexgons per side
///@return NONE
///Sample Usage:<br/>
///int numofhexgon = 11;<br/>
///hexgame::unique_ptr<AbstractStrategy,hexgame::default_delete<AbstractStrategy> > ptrtostrategyforred(nullptr);<br/>
///hexgame::unique_ptr<AbstractStrategy,hexgame::default_delete<AbstractStrategy> > ptrtostrategyforblue(nullptr);<br/>
/// ::simulations(ptrtostrategyforred, ptrtostrategyforblue, AIStrategyKind_NAIVE, AIStrategyKind_MCST, numofhexgon);<br/>
void simulations(
    hexgame::unique_ptr<AbstractStrategy, hexgame::default_delete<AbstractStrategy> >& strategyred,
    hexgame::unique_ptr<AbstractStrategy, hexgame::default_delete<AbstractStrategy> >& strategyblue,
    AIStrategyKind redstrategykind, AIStrategyKind bluestrategykind, int numofhexgon){

  HexBoard board(numofhexgon);

  Player playera(board, hexgonValKind_RED);  //north to south, 'O'
  Player playerb(board, hexgonValKind_BLUE);  //west to east, 'X'

  Game hexboardgame(board);

  selectStrategy(redstrategykind, strategyred, playera, board);
  selectStrategy(bluestrategykind, strategyblue, playerb, board);

  string winner = "UNKNOWN";
  int round = 0;

  while (winner == "UNKNOWN") {
    //the virtual player moves
    int redmove, redrow, redcol;
    redmove = hexboardgame.genMove(*strategyred);
    redrow = (redmove - 1) / numofhexgon + 1;
    redcol = (redmove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playera, redrow, redcol);

    //the virtual opponent moves
    int bluemove, bluerow, bluecol;
    bluemove = hexboardgame.genMove(*strategyblue);
    bluerow = (bluemove - 1) / numofhexgon + 1;
    bluecol = (bluemove - 1) % numofhexgon + 1;
    hexboardgame.setMove(playerb, bluerow, bluecol);

    round++;
    winner = hexboardgame.getWinner(playera, playerb);
  }
  cout << strategyred->name() << " (RED) plays against " << strategyblue->name()
       << " (BLUE)" << endl;
  cout << "simulation: " << round << endl;
  cout << "simulation: the winner is " << winner << endl;
}
