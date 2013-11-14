/*
 * Game.cpp
 *
 *  Created on: Nov 14, 2013
 *      Author: renewang
 */

#include <strstream>
#include "Game.h"

using namespace std;
bool Game::setMove(Player& player, int indexofrow, int indexofcol){
	return player.setMove(indexofrow, indexofcol);
}
string Game::showView()
{
	strstream strout;
	int side = board.getNumofhexgons();
	int step = 3;
	int totoalwide = (side + (side - 1)) * step;
	strout << setw(totoalwide / 2 - 5) << ' ' << "NORTH" << endl;
	string alphabets = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	strout << setw(step) << ' ';
	for (int i = 0; i < side; i++)
	{
		strout << setw(step) << alphabets[i];
		if (i < (side - 1))
			strout << setw(step - 1) << ' ';
	}

	strout << endl;
	for (int i = 0; i < side; i++)
	{
		if (i == side / 2)
			strout << setw(side / 2 * step) << "WEST"
					<< setw(step * (i+1) - side / 2 * step) << ' ' << setw(step - 1)
					<< (i + 1);
		else
			strout << setw(side / 2 * step) << ' '<< setw(step * (i+1)) << ' ' << setw(step - 1) << (i + 1);
		for (int j = 0; j < side; j++)
		{
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
	strout << setw(totoalwide / 2 - 5) << ' ' << "SOUTH" << endl;
	return strout.str();
}
