/*
 * Game.h
 * This file defines the declaration for the Game class.
 *
 *  Created on:
 *     Author: renewang
 */

#ifndef GAME_H_
#define GAME_H_

#include <string>

#include "Player.h"
#include "Strategy.h"
#include "HexBoard.h"
#include "AbstractStrategy.h"
/**
 * Game Class is used to control how to proceed and show the progress of the game. <br/>
 * If the game is played by human against computer, the game also employs the strategy for computer program to generate random move.<br/>
 * This class contains two constructors:<br/>
 * Game(): default constructor, not doing anything specific<br/>
 * Game(HexBoard& board): constructor which initializes the game with a specific board.<br/>
 * Sample Usage:<br/>
 *
 *     HexBoard board(numofhexgon);
 *     Game hexboardgame(board);
 *     cout << hexboardgame.showView() << endl;
 *     Player playerfirst(board, hexgonValKind::BLUE);
 *     Player playersecond(board, hexgonValKind::RED);
 *     hexboardgame.setMove(playerfirst, 1, 2);
 *     hexboardgame.setMove(playersecond, 2, 3);
 *     cout << hexboardgame.showView() << endl;
 *     string winner = hexboardgame.getWinner(playerfirst, playersecond);
 */
class Game
{
private:
	HexBoard& board; ///<the global board used by the game
public:
	///default constructor, doing nothing
	Game();
	///constructor, to initialize the game with the board
	///@param board is the hex board object which stores the actual game state
	Game(HexBoard& board):board(board){};
	///destructor
	virtual ~Game(){};

	//show current board view
	std::string showView(Player& playera, Player& playerb);
	//set the move for a particular player
	bool setMove(Player& player, int indexofrow, int indexofcol);
	//get the winner
	std::string getWinner(Player& playera, Player& playerb);
	//automatically generate move by AI strategy
	int genMove(AbstractStrategy& aistrategy);
	//reset the game to the initial state
	void resetGame(Player& playera, Player& playerb);
};

#endif /* GAME_H_ */
