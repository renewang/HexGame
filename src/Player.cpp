/*
 * Player.cpp
 *
 */

#include "Player.h"
bool Player::setMove(int indexofrow, int indexofcol){
	bool issuccess = false;
	int numofhexgon = board.getNumofhexgons();
	int index = (indexofrow-1)*numofhexgon + (indexofcol-1) + 1;
	if(board.getNodeValue(index) == hexgonValKind::EMPTY)
	{
		board.setNodeValue(index, this->playerkind);
		issuccess = true;
	}
	return issuccess;
}
