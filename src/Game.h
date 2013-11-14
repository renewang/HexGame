/*
 * Game.h
 *
 *  Created on: Nov 14, 2013
 *      Author: renewang
 */

#ifndef GAME_H_
#define GAME_H_

#include <string>
#include "Player.h"
#include "HexBoard.h"

class Game
{
private:
	HexBoard& board;
public:
	Game();
	Game(HexBoard& board):board(board){};
	virtual ~Game(){};

	//show current board view
	std::string showView();
	bool setMove(Player& player, int indexofrow, int indexofcol);
};

#endif /* GAME_H_ */
