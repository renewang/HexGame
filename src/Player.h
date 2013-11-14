/*
 * Player.h
 *
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include "HexBoard.h"
class Player
{
private:
	HexBoard& board;
	hexgonValKind playerkind;
	char viewlabel;
public:
	Player();
	Player(HexBoard& board, hexgonValKind kind):board(board),playerkind(kind){
		if(kind == hexgonValKind::RED)
			viewlabel = 'X';
		else if(kind == hexgonValKind::BLUE)
			viewlabel = 'O';
	};
	virtual ~Player(){};
	//INPUT:
	//indexofrow: index of row, starting from 1 to number of hexgon per side
	//indexofcol: index of column, starting from 1 to number of hexgon per side
	//OUTPUT:
	//a boolean variable to indicate if this move is successful or not
	bool setMove(int indexofrow, int indexofcol);

	char getViewLabel() const
	{
		return viewlabel;
	}

	hexgonValKind getPlayerlabel() const
	{
		return playerkind;
	}
};

#endif /* PLAYER_H_ */
