/*
 * AbstractStrategy.cpp
 *
 */

#include <cstdlib>
#include <memory>
#include "AbstractStrategyImpl.h"

using namespace std;

//the genMove called by Game
//INPUT: NONE
//OUTPUT:
//the next move evaluated by simulation
int AbstractStrategyImpl::genMove() {
  return (simulation());
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
int AbstractStrategyImpl::genNextRandom(shared_ptr<bool>& emptyindicators,
                                        unsigned proportionofempty) {
  bool isoccupied = true;
  int index = -1;
  srand((unsigned long)clock());

  while (isoccupied && proportionofempty > 0) {
    index = rand() % (ptrtoboard->getSizeOfVertices()) + 1;
    if (emptyindicators.get()[index - 1]) {
      isoccupied = false;
     emptyindicators.get()[index - 1] = false;
    }
  }
  return index;
}
//initialize the following containers to the current progress of playing board
void AbstractStrategyImpl::initGameState(shared_ptr<bool>& emptyglobal,vector<int>& bwglobal, vector<int>& oppglobal) {
  emptyglobal = ptrtoboard->getEmptyHexIndicators();
  if(ptrtoplayer->getViewLabel() == 'R'){
    bwglobal = ptrtoboard->getRedmoves();
    oppglobal = ptrtoboard->getBluemoves();
  }else{
    bwglobal = ptrtoboard->getBluemoves();
    oppglobal = ptrtoboard->getRedmoves();
  }
}
