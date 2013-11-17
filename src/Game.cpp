/*
 * Game.cpp
 * This file defines the implementation for the Game class.
 */
#include <cstdlib>
#include <sstream>
#include "Game.h"

using namespace std;

//To move to the specified location
//INPUT:
//player: the player makes the move
//indexofrow: index of row, starting from 1 to number of hexgon per side
//indexofcol: index of column, starting from 1 to number of hexgon per side
//OUTPUT:
//a boolean variable to indicate if this move is successful or not
bool Game::setMove(Player& player, int indexofrow, int indexofcol) {
  return player.setMove(indexofrow, indexofcol);
}
//Show current board view
//INPUT: NONE
//OUTPUT:
//the view of board in string
string Game::showView() {
  stringstream strout;
  int side = board.getNumofhexgons();
  int step = 3;
  int totoalwide = ((step - 1) * side) + step * side + (step - 1) * (side - 1);
  strout << setw(totoalwide / 2) << ' ' << "NORTH" << endl;
  string alphabets = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  strout << setw(step) << ' ';
  for (int i = 0; i < side; i++) {
    strout << setw(step) << alphabets[i];
    if (i < (side - 1))
      strout << setw(step - 1) << ' ';
  }

  strout << endl;
  for (int i = 0; i < side; i++) {
    if (i == side / 2)
      strout << setw((step - 1) * (i + 1) / 2) << "WEST"
             << setw((step - 1) * (i + 1) - (step - 1) * (i + 1) / 2) << ' '
             << setw(step - 1) << (i + 1);
    else
      strout << setw((step - 1) * (i + 1)) << ' ' << setw(step - 1) << (i + 1);
    for (int j = 0; j < side; j++) {
      int index = i * side + j + 1;
      strout << setw(step);
      if (board.getNodeValue(index) == hexgonValKind::EMPTY)
        strout << '.';
      else if (board.getNodeValue(index) == hexgonValKind::RED)
        strout << 'X';
      else
        strout << 'O';
      if (j < (side - 1))
        strout << setw(step - 1) << ' ';
    }
    if (i == side / 2)
      strout << setw(2 * step) << "EAST";
    strout << '\n';
  }
  strout << setw(totoalwide / 2) << ' ' << "SOUTH" << endl;
  return strout.str();
}
//get the winner
//INPUT:
//playera: the first player maybe red or blue
//playerb: the other player with different color with the first one
//OUTPUT:
//return the winner's name if there's a winer; otherwise, return "UNKNWON".
string Game::getWinner(Player& playera, Player& playerb) {
  if (playera.isArriveOpposite())
    return playera.getPlayername();
  else if (playerb.isArriveOpposite())
    return playerb.getPlayername();
  else
    return "UNKNOWN";
}
//TODO: elaborate the idea of Monte Carlo to evaluate and generate a good move for computer
//set the move for a particular player
//INPUT: NONE
//OUTPUT: NONE
int Game::genMove() {
  bool isoccupied = true;
  int index = -1;
  srand(clock());
  while (isoccupied) {
    index = rand() % (board.getSizeOfVertices()) + 1;
    if (board.getNodeValue(index) == hexgonValKind::EMPTY)
      isoccupied = false;
  }
  return index;
}
