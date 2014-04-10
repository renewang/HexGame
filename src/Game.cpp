/*
 * Game.cpp
 * This file defines the implementation for the Game class.
 *   Created on:
 *     Author: renewang
 */
#include <cstdlib>
#include <sstream>

#include "Game.h"

using namespace std;

///To move to the specified location
///@param player is the player makes the move
///@param indexofrow is index of row, starting from 1 to number of hexgon per side
///@param indexofcol is index of column, starting from 1 to number of hexgon per side
///@return  a boolean variable to indicate if this move is successful or not
bool Game::setMove(Player& player, int indexofrow, int indexofcol) {
  if(board.getNumofemptyhexgons() == 0)//empty, return true
    return true;
  else
    return player.setMove(indexofrow, indexofcol);
}
///Show current board view
///@param NONE
///@return the view of board in string
string Game::showView(Player& playera, Player& playerb) {
  stringstream strout;
  int side = board.getNumofhexgons();
  int step = 3;
  int totoalwide = ((step - 1) * side) + step * side + (step - 1) * (side - 1);
  strout << setw(totoalwide / 2) << ' ' << "NORTH" << '\n';
  string alphabets = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  strout << setw(step) << ' ';
  for (int i = 0; i < side; i++) {
    strout << setw(step) << alphabets[i];
    if (i < (side - 1))
      strout << setw(step - 1) << ' ';
  }

  strout << '\n';
  for (int i = 0; i < side; i++) {
    if (i == side / 2)
      strout
          << setw((step - 1) * (i + 1) / 2) << "WEST"
          << setw((step - 1) * (i + 1) - (step - 1) * (i + 1) / 2 - (i + 1) % 2)
          << ' ' << setw(step - 1) << (i + 1);
    else
      strout << setw((step - 1) * (i + 1)) << ' ' << setw(step - 1) << (i + 1);
    for (int j = 0; j < side; j++) {
      int index = i * side + j + 1;
      strout << setw(step - 1);
#if __cplusplus  > 199711L
      if (board.getNodeValue(index) == hexgonValKind::EMPTY)
#else
        if (board.getNodeValue(index) == EMPTY)
#endif
        strout << '.';
      else if (board.getNodeValue(index) == playera.getPlayerlabel())
        strout << playera.getViewLabel();
      else if (board.getNodeValue(index) == playerb.getPlayerlabel())
        strout << playerb.getViewLabel();

      if (j < (side - 1))
        strout << " - ";
    }
    if (i == side / 2)
      strout << setw(2 * step) << "EAST";
    strout << '\n';
    //print edge
    if (i < (side - 1)) {
      strout << setw((step - 1) * (i + 2)) << ' ';
      for (int j = 0; j < 2 * side; j++) {
        if (j % 2 == 0)
          strout << setw(step) << '\\';
        else if (j % 2 == 1 && j != (2 * side - 1))
          strout << setw(step - 1) << '/';
      }
      strout << setw(step) << '\n';
    }
  }
  strout << setw(totoalwide / 2) << ' ' << "SOUTH" << '\n';
  return strout.str();
}
///Get the winner
///@param playera: the first player maybe red or blue
///@param playerb: the other player with different color with the first one
///@return the winner's name if there's a winer; otherwise, return "UNKNWON".
string Game::getWinner(Player& playera, Player& playerb) {
  if (playera.isArriveOpposite())
    return playera.getPlayername();
  else if (playerb.isArriveOpposite())
    return playerb.getPlayername();
  else
    return "UNKNOWN";
}
///Generate an intelligent move for Baby Watson
///@param a strategy object
///@return the index of next move (starting from 1 to number of hexgon per side)
int Game::genMove(AbstractStrategy& aistrategy) {
  return (aistrategy.genMove());
}
///Reset board for the next round
///@param playera: the first player maybe red or blue
///@param playerb: the other player with different color with the first one
///@return NONE
void Game::resetGame(Player& playera, Player& playerb){
  board.resetHexBoard(false);
  playera.resetPlayersboard();
  playerb.resetPlayersboard();
}
