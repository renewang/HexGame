/*
 * HexBoardGameApp.cpp
 * This file defines the main function for the hex board game application.
 * User will be promoted to choose to move firstly depending on the color of player.
 * After choosing the role played by user, the game will proceed to play against with computer program.
 * User can make several moves until reaching the wining condition (connecting opposite two sides)
 *
 */
#include <cctype>
#include <string>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "Game.h"
#include "Player.h"
#include "HexBoard.h"

using namespace std;

string printHeader();
void parserMove(string movestring, int& row, int& col);
int queryHumanMove(int& userrow, int& usercol);

//global variable to set the hex board size as numofhexgon x numofhexgon
const int numofhexgon = 11;

int main(int argc, char **argv) {

  cout << printHeader() << "\n\n";
  cout << "Board (before moving)" << endl;

  bool isstop = false;

  while (!isstop) {  //main loop to keep different rounds of games

    //game setup
    HexBoard board(numofhexgon);
    Game hexboardgame(board);
    cout << hexboardgame.showView() << endl;

    Player* human, *babywatson;
    Player playerfirst(board, hexgonValKind::BLUE);
    Player playersecond(board, hexgonValKind::RED);

    string userchoice;

    bool ishumanfirst = false;

    while (true) {//loop to make sure user's input is correct.
      //let user decide the color of player and the choice to make first move or not.
      cout << "Please enter your choice of color?" << endl;
      cin >> userchoice;
      if (userchoice == "BLUE") {
        human = &playerfirst;
        babywatson = &playersecond;
        ishumanfirst = true;
        break;
      } else if (userchoice == "RED") {
        human = &playersecond;
        babywatson = &playerfirst;
        break;
      } else
        cout << "Invalid input. Please Try again!" << endl;
    }

    //continue moving until one of the players wins
    while (true) {

      if (ishumanfirst) {
        //human moves
        bool issetmove = false;
        while (!issetmove) {//loop to make sure user make a legal move
          int userrow, usercol;
          queryHumanMove(userrow, usercol);
          issetmove = hexboardgame.setMove(*human, userrow, usercol);
        }
        cout << hexboardgame.showView() << endl;
      }

      ishumanfirst = true;
      //computer aka baby watson moves
      int watsonmove, watsonrow, watsoncol;
      watsonmove = hexboardgame.genMove();
      watsonrow = (watsonmove - 1) / numofhexgon + 1;
      watsoncol = (watsonmove - 1) % numofhexgon + 1;
      hexboardgame.setMove(*babywatson, watsonrow, watsoncol);
      cout << "Baby Watson sets foot at (" << watsonrow << ',' << watsoncol
           << ')' << endl;
      cout << hexboardgame.showView() << endl;

      string winner = hexboardgame.getWinner(*human, *babywatson);
      if (winner == human->getPlayername()) {
        cout << "Human, you beats Baby Watson!" << endl;
        break;
      } else if (winner == babywatson->getPlayername()) {
        cout << "Human, sorry! Baby Watson rules!" << endl;
        break;
      }
    }

    //let user choose to continue another round of game
    string userkeepplay;
    cout << "Human, do you want to continue playing? (Yes, No)" << endl;
    cin >> userkeepplay;

    if (tolower(userkeepplay[0]) == 'n')
      isstop = true;
  }
  return 0;
}
//TODO need more robust methods or exceptions/errors handling for parsing
//function to collect logics of inquiring human to make legitimate move
int queryHumanMove(int& userrow, int& usercol) {
  string usermove;
  int index = -1;
  while (index < 1 || index > (numofhexgon * numofhexgon)) {
    cout << "Human, please enter your next move? (i,j)" << endl;
    cin >> usermove;
    parserMove(usermove, userrow, usercol);
    index = (userrow - 1) * numofhexgon + (usercol - 1) + 1;
  }
  return index;
}
//function to parse user's input
void parserMove(string movestring, int& row, int& col) {
  unsigned posofcomma = movestring.find(',');
  if (posofcomma != movestring.npos) {
    row = atoi(movestring.substr(0, posofcomma).c_str());
    col = atoi(movestring.substr(posofcomma + 1).c_str());
  } else
    row = col = 0;
}
//function to print out the header
string printHeader() {
  stringstream strout;
  strout << "\n\nHexborad Game Starts!!\n\n";
  strout << setfill('*') << setw(70) << '*' << endl;
  strout << '*' << setfill(' ') << setw(7) << ' ' << setw(4) << internal
         << "ITEM" << setfill(' ') << setw(8) << ' ';
  strout << '*' << setfill(' ') << setw(1) << ' ' << setw(6) << internal
         << "SYMBOL" << setfill(' ') << setw(1) << ' ';
  strout << '*' << setfill(' ') << setw(17) << ' ' << setw(5) << internal
         << "NOTES" << setfill(' ') << setw(17) << ' ';
  strout << '*' << endl;
  strout << setfill('*') << setw(70) << '*' << endl;
  strout << '*' << setfill(' ') << setw(2) << ' ' << setw(14) << internal
         << "EMPTY LOCATION" << setfill(' ') << setw(3) << ' ';
  strout << '*' << setfill(' ') << setw(4) << ' ' << '.' << setfill(' ')
         << setw(3) << ' ';
  strout << '*' << setfill(' ') << setw(39) << ' ';
  strout << '*' << endl;
  strout << '*' << setfill(' ') << setw(4) << ' ' << setw(11) << internal
         << "BLUE PLAYER" << setfill(' ') << setw(4) << ' ';
  strout << '*' << setfill(' ') << setw(4) << ' ' << 'X' << setfill(' ')
         << setw(3) << ' ';
  strout << '*' << setfill(' ') << setw(5) << ' ' << internal << setw(29)
         << "connects West-East, moves 1st" << setfill(' ') << setw(5) << ' ';
  strout << '*' << endl;
  strout << '*' << setfill(' ') << setw(4) << ' ' << setw(10) << internal
         << "RED PLAYER" << setfill(' ') << setw(5) << ' ';
  strout << '*' << setfill(' ') << setw(4) << ' ' << 'O' << setfill(' ')
         << setw(3) << ' ';
  strout << '*' << setfill(' ') << setw(9) << ' ' << internal << setw(20)
         << "connects North-South" << setfill(' ') << setw(10) << ' ';
  strout << '*' << endl;
  strout << setfill('*') << setw(70) << '*' << endl;
  return strout.str();
}