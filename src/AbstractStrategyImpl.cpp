/*
 * AbstractStrategy.cpp
 *
 */

#include <cstdlib>

#include "Game.h"
#include "Strategy.h"
#include "MonteCarloTreeSearch.h"
#include "AbstractStrategyImpl.h"
#include "MultiMonteCarloTreeSearch.h"

using namespace std;

//the genMove called by Game
//INPUT: NONE
//OUTPUT:
//the next move evaluated by simulation
int AbstractStrategyImpl::genMove() {
  if(ptrtoboard->getNumofemptyhexgons() > 0)
    return(simulation(ptrtoboard->getNumofemptyhexgons()));
  else
    return -1; //there's no empty moves
}
//check if the winner exists for this stage of simulation
//INPUT:
//babywatsons: the proposed moves made by baby watson so far
//opponenets: the proposed moves made by virtual opponent so far
//OUTPUT:
//an integer indicates 0: no winner, -1, babywatson loses and 1 babywatson wins
int AbstractStrategyImpl::checkWinnerExist(vector<int>& babywatsons,
                                           vector<int>& opponents) {
  if (isWinner(babywatsons, ptrtoplayer->getWestToEastCondition()))
    return 1;
  else if (isWinner(opponents, !ptrtoplayer->getWestToEastCondition()))
    return -1;
  else
    return 0;
}
//check if the test vector satisfies the winning condition
//INPUT:
//candidates: stores the moves
//iswestoeast: the boolean variable to indicate if the winning condition is west to east
//OUTPUT:
//the boolean variable indicates if this simulation wins the game
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
      if (ptrtoboard->isAdjacent(candidates[i], candidates[j])
          && representative[i] != representative[j]) {
        int smallgroup = representative[j];
        int largegroup =
            (repsize[representative[i]] > repsize[representative[j]]) ?
                representative[i] : representative[j];
        if (largegroup != representative[i])
          smallgroup = representative[i];

        representative[smallgroup] = largegroup;
        for (unsigned k = 0; k < candidates.size(); k++)
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
//generate the random next move representing with index [1, number of hexgon per side]
//INPUT: NONE
//OUPUT: NONE
int AbstractStrategyImpl::genNextRandom(hexgame::shared_ptr<bool>& emptyindicators,
                                        int& proportionofempty) {
  bool isoccupied = true;
  int index = -1;
#ifdef NDEBUG
  srand((unsigned long)clock());
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
//initialize the following containers to the current progress of playing board
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
void selectStrategy(AIStrategyKind strategykind,
                    hexgame::unique_ptr<AbstractStrategy, hexgame::default_delete<AbstractStrategy> >& watsonstrategy,
                    Player& player, HexBoard& board) {
  switch (strategykind) {
    case AIStrategyKind_NAIVE:
    std::cout <<"\n"<< player.getPlayername()<<" uses naive strategy" << endl;
    watsonstrategy.reset(new Strategy(&board, &player));
    break;
    case AIStrategyKind_PMCST:
    std::cout <<"\n"<< player.getPlayername()<<" uses PMCST strategy" << endl;
    watsonstrategy.reset(new MultiMonteCarloTreeSearch(&board, &player));
    break;
    default:
    std::cout <<"\n"<< player.getPlayername()<< " uses MCST strategy" << endl;
    watsonstrategy.reset(new MonteCarloTreeSearch(&board, &player));
    break;
  }
}
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
