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
#include <memory>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "Game.h"
#include "Global.h"
#include "Player.h"
#include "HexBoard.h"
#include "Strategy.h"
#include "MonteCarloTreeSearch.h"
#include "MultiMonteCarloTreeSearch.h"

using namespace std;

string printHeader();
void parserMove(string movestring, int& row, int& col);
int queryHumanMove(int& userrow, int& usercol);

//global variable to set the hex board size as numofhexgon x numofhexgon
int numofhexgon = 11;

int main(int argc, char **argv) {

  bool isstop = false;
  if (argc == 2) {
    numofhexgon = atoi(argv[1]);

    cout << "Doing Simulation for two virtual players" << endl;
    hexgame::unique_ptr<AbstractStrategy,
        hexgame::default_delete<AbstractStrategy> > ptrtostrategyforred(
        nullptr);
    hexgame::unique_ptr<AbstractStrategy,
        hexgame::default_delete<AbstractStrategy> > ptrtostrategyforblue(
        nullptr);

    simulations(ptrtostrategyforred, ptrtostrategyforblue, AIStrategyKind_NAIVE, AIStrategyKind_MCTS, numofhexgon);
    return 0;
  }
  cout << printHeader() << "\n\n";
  cout << "Board (before moving)" << endl;

  while (!isstop) {  //main loop to keep different rounds of games

    //game setup
    HexBoard board(numofhexgon);
    Game hexboardgame(board);

    Player* human, *babywatson;
    Player playerfirst(board, hexgonValKind_BLUE);
    Player playersecond(board, hexgonValKind_RED);
    cout << hexboardgame.showView(playerfirst, playersecond) << endl;

    string userchoice;

    bool ishumanfirst = false;

    while (true) {  //loop to make sure user's input is correct.
      //let user decide the color of player and the choice to make first move or not.
      cout << "Please enter your choice of color? (RED|BLUE)" << endl;
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

    //let user decide to play against which AI oppoenent with different strategies
    hexgame::unique_ptr<AbstractStrategy,
        hexgame::default_delete<AbstractStrategy> > watsonstrategy(nullptr);
    int aistrategykind = 2;
    while (true) {
      cout
          << "Please enter your choice of AI oppoenent? (1:NAIVE|2:MCST|3:Parallel MCST)"
          << endl;
      cin >> aistrategykind;
      if (aistrategykind < 1 || aistrategykind > 3)
        cout << "Invalid input. Please Try again!" << endl;
      else {
        ::selectStrategy(static_cast<AIStrategyKind>(aistrategykind),
                         watsonstrategy, *babywatson, board);
        break;
      }
    }
    //continue moving until one of the players wins
    while (true) {

      if (ishumanfirst) {
        //human moves
        bool issetmove = false;
        while (!issetmove) {      //loop to make sure user make a legal move
          int userrow, usercol;
          queryHumanMove(userrow, usercol);
          issetmove = hexboardgame.setMove(*human, userrow, usercol);
        }
        cout << hexboardgame.showView(*human, *babywatson) << endl;
      }

      ishumanfirst = true;
      //computer aka baby watson moves
      int watsonmove, watsonrow, watsoncol;
      watsonmove = hexboardgame.genMove(*watsonstrategy);
      watsonrow = (watsonmove - 1) / numofhexgon + 1;
      watsoncol = (watsonmove - 1) % numofhexgon + 1;
      bool isbwmove = hexboardgame.setMove(*babywatson, watsonrow, watsoncol);
      if (isbwmove) {
        cout << "Baby Watson sets foot at (" << watsonrow << ',' << watsoncol
             << ')' << endl;
        cout << hexboardgame.showView(*human, *babywatson) << endl;
      }
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
